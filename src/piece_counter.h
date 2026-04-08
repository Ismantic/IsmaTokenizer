#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include "common.h"
#include "misc.h"
#include "normalizer.h"
#include "piece_spec.h"
#include "piece_tokenizer.h"
#include "sentence.h"
#include "ustr.h"

namespace piece {

class PieceCounter {
public:
  PieceCounter(const CounterSpec& counter_spec,
               const NormalizerSpec& normalizer_spec);
  ~PieceCounter();

  bool Count();
  bool Save() const;
  bool Serialize(Model* model) const;

  struct Token {
    int value;
    Token* prev;
    Token* next;
  };

private:
  using Sentence = std::pair<std::string, int64_t>;
  using Sentences = std::vector<Sentence>;

  struct PairHash {
    size_t operator()(const std::pair<int, int>& p) const {
      return std::hash<int>{}(p.first) ^ (std::hash<int>{}(p.second) << 1);
    }
  };

  // pair -> sentence indices that contain this pair (may have duplicates).
  using PairIndex = std::unordered_map<std::pair<int, int>,
                                       std::vector<size_t>, PairHash>;
  // pair -> frequency delta (for multi-threaded stats accumulation).
  using DeltaMap = std::unordered_map<std::pair<int, int>, int64_t, PairHash>;
  // (pair, sentence_idx) entry for deferred index updates.
  using IndexEntry = std::pair<std::pair<int, int>, size_t>;

  bool InitMetaPieces();
  bool LoadSentences();
  void SplitSentencesByWhitespace();

  static Token* BuildTokenList(const std::string& text);
  static void FreeTokenList(Token* head);

  void InitPairsStatsAndIndex(Multiset<std::pair<int, int>>& stats,
                              PairIndex& pair_index);

  // Merge a pair in one sentence. Updates stats and pair_index directly.
  static void MergeSentence(const std::pair<int, int>& pair, int new_id,
                            Token* head, int64_t freq,
                            Multiset<std::pair<int, int>>& stats,
                            size_t sentence_idx, PairIndex& pair_index);

  // Thread-safe variant: accumulates stats into delta, index into idx_adds.
  static void MergeSentenceAsync(const std::pair<int, int>& pair, int new_id,
                                 Token* head, int64_t freq,
                                 DeltaMap& delta,
                                 size_t sentence_idx,
                                 std::vector<IndexEntry>& idx_adds);

  std::map<int, std::pair<std::string, Model::Piece::Type>> meta_pieces_;
  std::vector<std::pair<std::vector<std::string>, float>> pieces_;
  Sentences sentences_;
  std::vector<Token*> token_lists_;
  CounterSpec counter_spec_;
  NormalizerSpec normalizer_spec_;
  std::unordered_map<int, std::string> vocab_;
};

}  // namespace piece
