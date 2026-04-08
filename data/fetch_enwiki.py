#!/usr/bin/env python3
"""Download the English (enwiki) subset of HuggingFaceFW/finewiki."""

from __future__ import annotations

import argparse
import sys
import time
from pathlib import Path

from datasets import load_dataset


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Download enwiki from HuggingFaceFW/finewiki."
    )
    parser.add_argument(
        "--output",
        type=Path,
        default=Path("data/enwiki.jsonl"),
        help="output path (default: data/enwiki.jsonl)",
    )
    parser.add_argument(
        "--columns",
        nargs="+",
        default=["title", "text"],
        help="columns to keep (default: title text)",
    )
    parser.add_argument(
        "--retries",
        type=int,
        default=5,
        help="max retry attempts on network error (default: 5)",
    )
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    args.output.parent.mkdir(parents=True, exist_ok=True)

    print("Loading HuggingFaceFW/finewiki en split...", file=sys.stderr)
    for attempt in range(1, args.retries + 1):
        try:
            ds = load_dataset("HuggingFaceFW/finewiki", name="en", split="train")
            break
        except Exception as e:
            if attempt == args.retries:
                raise
            wait = min(30, 5 * attempt)
            print(f"  attempt {attempt} failed: {e}", file=sys.stderr)
            print(f"  retrying in {wait}s...", file=sys.stderr)
            time.sleep(wait)
    print(f"  {len(ds)} articles", file=sys.stderr)

    if args.columns:
        ds = ds.select_columns(args.columns)

    ds.to_json(str(args.output))
    print(f"wrote {args.output}", file=sys.stderr)


if __name__ == "__main__":
    main()
