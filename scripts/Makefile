ROOT     := $(shell cd .. && pwd)
BIN      := $(ROOT)/build/piece-tokenizer
CN_INPUT := $(ROOT)/data/cn_sentences.txt
EN_INPUT := $(ROOT)/data/en_sentences.txt
MODEL_DIR := $(CURDIR)/output

VOCAB_SIZE := 80000
CPU        := 4
NORMALIZE  := identity

METHODS := bytepiece

# 默认训练所有方法
all: $(METHODS)

# 每种方法生成一个 .model 文件
define METHOD_RULE
$(1): $(MODEL_DIR)/$(1).model
$(MODEL_DIR)/$(1).model: $(CN_INPUT) $(EN_INPUT) | $(MODEL_DIR)
	$(BIN) count --method $(1) --input $(CN_INPUT) --input $(EN_INPUT) \
		--vocab-size $(VOCAB_SIZE) --model $(MODEL_DIR)/$(1) \
		--normalize $(NORMALIZE) --cpu $(CPU)
endef

$(foreach m,$(METHODS),$(eval $(call METHOD_RULE,$(m))))

$(MODEL_DIR):
	mkdir -p $(MODEL_DIR)

clean:
	rm -rf $(MODEL_DIR)

.PHONY: all clean $(METHODS)
