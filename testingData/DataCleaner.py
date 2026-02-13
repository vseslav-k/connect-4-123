#!/usr/bin/env python3
"""
clean_logs.py

Cleans log data from lines like:
  [TAG] Player datatype: data
  [TAG] Player datatype; data
  Player datatype: data
  Player datatype

Features:
1) Per-datatype min/max filtering for numeric values.
2) Optional [TAG] removal.
3) Optional removal of ALL lines whose datatype has non-numeric/missing data.
4) Optional replacement of spaces (left of first ':' or ';') with underscores.
5) Writes output to cleanedData.txt

New option:
  --underscores-left-of-sep true|false

If true:
  Example:
    "[DEBUG] AI1 Move Choice: 42"
  becomes (before/after tag removal depending on --remove-tags):
    "[DEBUG]_AI1_Move_Choice: 42"
  or
    "AI1_Move_Choice: 42"
"""

from __future__ import annotations

import argparse
import re
from typing import Dict, Tuple, Optional, List

LINE_RE = re.compile(
    r"""
    ^
    (?:\[(?P<tag>[^\]]+)\]\s+)?      # optional [TAG]
    (?P<player>\S+)\s+               # player token
    (?P<dtype>\S+)                   # datatype token (no spaces)
    (?:[;:]\s*(?P<data>.+?))?        # optional : data OR ; data
    \s*$
    """,
    re.VERBOSE,
)

TAG_PREFIX_RE = re.compile(r"^\[[^\]]+\]\s+")


def parse_numeric(s: str) -> Optional[float]:
    try:
        return float(s.strip())
    except (ValueError, AttributeError):
        return None


def parse_bool_flag(s: str, name: str) -> bool:
    v = s.strip().lower()
    if v in {"true", "1", "yes", "y"}:
        return True
    if v in {"false", "0", "no", "n"}:
        return False
    raise ValueError(f"{name} must be true/false (or yes/no, 1/0).")


def parse_range_specs(specs: List[str]) -> Dict[str, Tuple[float, float]]:
    """
    Parse repeated --range entries of form:
      Datatype:min:max
    Example:
      --range ThinkTime:0:20000 --range Eval:-100000:100000
    """
    ranges: Dict[str, Tuple[float, float]] = {}
    for spec in specs:
        parts = spec.split(":")
        if len(parts) != 3:
            raise ValueError(
                f"Invalid --range '{spec}'. Expected Datatype:min:max "
                f"(example: ThinkTime:0:20000)"
            )
        dtype, min_s, max_s = parts
        dtype = dtype.strip()
        if not dtype:
            raise ValueError(f"Invalid --range '{spec}': datatype cannot be empty.")
        try:
            min_v = float(min_s)
            max_v = float(max_s)
        except ValueError:
            raise ValueError(f"Invalid --range '{spec}': min/max must be numeric.")
        if min_v > max_v:
            raise ValueError(f"Invalid --range '{spec}': min > max.")
        ranges[dtype] = (min_v, max_v)
    return ranges


def replace_spaces_left_of_sep(line: str) -> str:
    """
    Replace spaces with underscores only to the left of the first ':' or ';'.
    If no separator exists, replace spaces in whole line.
    """
    colon_idx = line.find(":")
    semi_idx = line.find(";")

    sep_idx = -1
    if colon_idx == -1:
        sep_idx = semi_idx
    elif semi_idx == -1:
        sep_idx = colon_idx
    else:
        sep_idx = min(colon_idx, semi_idx)

    if sep_idx == -1:
        return line.replace(" ", "_")

    left = line[:sep_idx].replace(" ", "_")
    right = line[sep_idx:]  # keep right side unchanged
    return left + right


def clean_file(
    input_path: str,
    output_path: str,
    ranges: Dict[str, Tuple[float, float]],
    remove_tags: bool,
    drop_non_numeric: bool,
    underscores_left_of_sep: bool,
) -> None:
    dtype_seen_in_file = {dtype: False for dtype in ranges.keys()}

    kept_lines: List[str] = []
    removed_out_of_range = 0
    removed_non_numeric = 0
    kept_malformed = 0

    with open(input_path, "r", encoding="utf-8") as f:
        for line_no, raw in enumerate(f, 1):
            line = raw.rstrip("\n")
            stripped = line.strip()

            if stripped == "":
                kept_lines.append(line)
                continue

            m = LINE_RE.match(stripped)
            if not m:
                # Keep malformed lines as-is but still allow optional text transforms
                out_line = line
                if remove_tags:
                    out_line = TAG_PREFIX_RE.sub("", out_line)
                if underscores_left_of_sep:
                    out_line = replace_spaces_left_of_sep(out_line)
                kept_lines.append(out_line)
                print(f"[WARN] Line {line_no}: could not parse, kept.")
                kept_malformed += 1
                continue

            dtype = m.group("dtype")
            data = m.group("data")
            num = parse_numeric(data) if data is not None else None

            # Optional: drop all non-numeric/missing data lines
            if drop_non_numeric and num is None:
                removed_non_numeric += 1
                continue

            # Apply per-datatype range if configured
            if dtype in ranges:
                dtype_seen_in_file[dtype] = True
                min_v, max_v = ranges[dtype]

                if num is None:
                    if not drop_non_numeric:
                        print(
                            f"[WARN] Line {line_no}: datatype '{dtype}' has range "
                            f"[{min_v}, {max_v}] but data is non-numeric/missing; kept."
                        )
                else:
                    if num < min_v or num > max_v:
                        removed_out_of_range += 1
                        continue

            out_line = line
            if remove_tags:
                out_line = TAG_PREFIX_RE.sub("", out_line)
            if underscores_left_of_sep:
                out_line = replace_spaces_left_of_sep(out_line)

            kept_lines.append(out_line)

    for dtype, seen in dtype_seen_in_file.items():
        if not seen:
            print(f"[WARN] Datatype '{dtype}' not found in input; skipped.")

    with open(output_path, "w", encoding="utf-8") as out:
        for l in kept_lines:
            out.write(l + "\n")

    print(f"[INFO] Wrote cleaned file: {output_path}")
    print(f"[INFO] Removed out-of-range lines: {removed_out_of_range}")
    print(f"[INFO] Removed non-numeric/missing-data lines: {removed_non_numeric}")
    if kept_malformed:
        print(f"[INFO] Kept malformed lines: {kept_malformed}")


def main():
    parser = argparse.ArgumentParser(
        description="Clean log file by datatype min/max and optional text transforms."
    )
    parser.add_argument("input_file", help="Path to input txt log file")
    parser.add_argument(
        "--range",
        action="append",
        default=[],
        help="Datatype:min:max (repeatable). Example: --range ThinkTime:0:20000",
    )
    parser.add_argument(
        "--remove-tags",
        default="false",
        help="true/false (remove [TAG] prefix). Default: false",
    )
    parser.add_argument(
        "--drop-non-numeric",
        default="false",
        help="true/false (remove all lines with non-numeric or missing data). Default: false",
    )
    parser.add_argument(
        "--underscores",
        default="false",
        help="true/false (replace spaces with '_' to the left of first ':' or ';'). Default: false",
    )
    args = parser.parse_args()

    try:
        ranges = parse_range_specs(args.range)
        remove_tags = parse_bool_flag(args.remove_tags, "--remove-tags")
        drop_non_numeric = parse_bool_flag(args.drop_non_numeric, "--drop-non-numeric")
        underscores_left_of_sep = parse_bool_flag(
            args.underscores, "--underscores"
        )
    except ValueError as e:
        raise SystemExit(f"Argument error: {e}")

    clean_file(
        input_path=args.input_file,
        output_path="cleanedData.txt",
        ranges=ranges,
        remove_tags=remove_tags,
        drop_non_numeric=drop_non_numeric,
        underscores_left_of_sep=underscores_left_of_sep,
    )


if __name__ == "__main__":
    main()


#python3 DataCleaner.py log.txt --range ThinkTime:0:20000  --remove-tags true  --drop-non-numeric true -underscores-left-of-sep true