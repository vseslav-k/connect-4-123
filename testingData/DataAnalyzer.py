#!/usr/bin/env python3
"""
Parse log lines like:
  [DEBUG] AI2 ThinkTime: 10000
  AI1 NoMoves
  [INFO]  P1 EvalScore; 42
  BOT_A Nodes: 15230

Accepted formats:
  [TAG] Player datatype: data
  [TAG] Player datatype; data
  Player datatype: data
  Player datatype; data
  Player datatype

- TAG is optional and ignored
- Player is auto-detected (no hardcoded list)
- datatype has no spaces
- data is optional, numeric values are used for stats

Outputs:
- Count per (player, datatype)
- Mean/Median/Min/Max/Mode/StandardDeviation for numeric values per (player, datatype)
"""

from __future__ import annotations

import argparse
import math
import re
from collections import defaultdict
from statistics import mean, median, multimode, pstdev
from typing import Dict, List, Tuple, Optional


# Optional [TAG] prefix, then Player and datatype.
LINE_RE = re.compile(
    r"""
    ^(?:\[[^\]]+\]\s+)?      # optional [TAG] and following space(s)
    (?P<player>\S+)\s+       # player token
    (?P<dtype>\S+)           # datatype token (no spaces)
    (?:[;:]\s*(?P<data>.+?))? # optional : data or ; data
    \s*$
    """,
    re.VERBOSE,
)


def parse_numeric(s: str) -> Optional[float]:
    s = s.strip()
    try:
        return float(s)
    except ValueError:
        return None


def format_number(x: float) -> str:
    if math.isfinite(x) and x.is_integer():
        return str(int(x))
    return f"{x:.6g}"


def process_file(path: str):
    counts: Dict[Tuple[str, str], int] = defaultdict(int)
    numeric_values: Dict[Tuple[str, str], List[float]] = defaultdict(list)

    with open(path, "r", encoding="utf-8") as f:
        for raw in f:
            line = raw.strip()
            if not line:
                continue

            m = LINE_RE.match(line)
            if not m:
                # Skip malformed lines
                continue

            player = m.group("player")
            dtype = m.group("dtype")
            data = m.group("data")

            key = (player, dtype)
            counts[key] += 1

            if data is not None:
                num = parse_numeric(data)
                if num is not None:
                    numeric_values[key].append(num)

    return counts, numeric_values


def build_output_lines(counts, numeric_values) -> List[str]:
    lines: List[str] = []

    # Stable sort by player then datatype
    all_keys = sorted(counts.keys(), key=lambda k: (k[0].lower(), k[0], k[1].lower(), k[1]))

    for player, dtype in all_keys:
        vals = numeric_values.get((player, dtype), [])

        lines.append(f"{player} {dtype} Count: {counts[(player, dtype)]}")

        if vals:
            vals_sorted = sorted(vals)
            m = mean(vals)
            med = median(vals_sorted)
            mn = vals_sorted[0]
            mx = vals_sorted[-1]
            sd = pstdev(vals) if len(vals) > 1 else 0.0

            # If multiple modes, pick smallest for deterministic output
            mode_candidates = multimode(vals)
            mode_val = min(mode_candidates) if mode_candidates else vals[0]

            lines.append(f"{player} {dtype} Mean: {format_number(m)}")
            lines.append(f"{player} {dtype} Median: {format_number(med)}")
            lines.append(f"{player} {dtype} Min: {format_number(mn)}")
            lines.append(f"{player} {dtype} Max: {format_number(mx)}")
            lines.append(f"{player} {dtype} Mode: {format_number(mode_val)}")
            lines.append(f"{player} {dtype} StandardDeviation: {format_number(sd)}")

        lines.append("\n")

    if lines and lines[-1] == "":
        lines.pop()

    return lines


def main():
    parser = argparse.ArgumentParser(description="Compute per-player per-datatype log stats.")
    parser.add_argument("input_file", help="Path to input .txt log file")
    parser.add_argument("-o", "--output", help="Optional output file path")
    args = parser.parse_args()

    counts, numeric_values = process_file(args.input_file)
    out_lines = build_output_lines(counts, numeric_values)
    output_text = "\n".join(out_lines)

    if args.output:
        with open(args.output, "w", encoding="utf-8") as f:
            f.write(output_text + "\n")
    else:
        print(output_text)


if __name__ == "__main__":
    main()
