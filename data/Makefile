export HF_ENDPOINT := https://hf-mirror.com

all: download process sentences

download: data/cnwiki.jsonl data/enwiki.jsonl

data/cnwiki.jsonl:
	python3 fetch_finewiki.py

data/enwiki.jsonl:
	python3 fetch_enwiki.py

process: cn.txt en.txt

cn.txt: data/cnwiki.jsonl
	python3 process.py data/cnwiki.jsonl --output cn.txt --t2s

en.txt: data/enwiki.jsonl
	python3 process.py data/enwiki.jsonl --output en.txt

sentences: cn_sentences.txt en_sentences.txt

cn_sentences.txt: cn.txt
	python3 split_sentences.py cn.txt cn_sentences.txt

en_sentences.txt: en.txt
	python3 split_sentences.py en.txt en_sentences.txt

clean:
	rm -rf data/ cn.txt en.txt cn_sentences.txt en_sentences.txt

.PHONY: all download process sentences clean
