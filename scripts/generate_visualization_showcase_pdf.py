#!/usr/bin/env python3
"""generate_visualization_showcase_pdf.py — 全3対象の代表例を1つの PDF に統合

論文・アンケート・レビュー向けに、Sunburst/Sankey/Heatmap/Chord の
全4種類を対象別に比較できる「ショーケース」PDFを生成する。

構成:
  1. タイトルページ (4種の説明)
  2. reversi (関数型) - is_valid_move の4種類
  3. sakura (手続き型) - IsMailAddress の4種類 (Heatmap/Chord は該当なし)
  4. click (OOP) - IntRange の4種類 (全種類揃う)
  5. 対象間比較のまとめページ

Usage:
  python scripts/generate_visualization_showcase_pdf.py \\
    --output report/visualizations/4types-showcase.pdf
"""
from __future__ import annotations

import argparse
from pathlib import Path

import yaml
from matplotlib.backends.backend_pdf import PdfPages

# 共通関数を再利用するため同階層のスクリプトを import する
import sys
sys.path.insert(0, str(Path(__file__).parent))
from generate_visualization_pdf import (
    make_target_page, make_title_page,
    target_name,
)

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt


def make_summary_page(pdf: PdfPages):
    fig, ax = plt.subplots(figsize=(8.5, 11))
    ax.set_axis_off()

    ax.text(0.5, 0.95, "対象間比較のまとめ", ha="center", va="top",
            fontsize=18, weight="bold", transform=ax.transAxes)

    data = [
        ["対象", "リバーシ", "sakura IsMailAddress", "click IntRange"],
        ["設計スタイル", "関数型", "手続き型", "OOP (ダイヤモンド継承)"],
        ["総要求数", "9件", "17件", "48件"],
        ["Sunburst", "有効 ◯", "有効 ◯", "有効 ◯"],
        ["Sankey", "有効 ◯", "有効 ◯", "有効 ◯"],
        ["Heatmap", "該当なし ×", "該当なし ×", "有効 ◯ (EN検出)"],
        ["Chord", "該当なし ×", "該当なし ×", "有効 ◯ (EN検出)"],
        ["可読率", "97.8%", "65.7% (手動)", "24.3% (v2辞書)"],
        ["アンケート題材", "パターン3", "題材B", "追加案A"],
    ]

    table = ax.table(cellText=data[1:], colLabels=data[0],
                     loc="center", cellLoc="center",
                     colWidths=[0.22, 0.22, 0.28, 0.28])
    table.auto_set_font_size(False)
    table.set_fontsize(9)
    table.scale(1.0, 1.8)

    # ヘッダ行の装飾
    for j in range(4):
        cell = table[0, j]
        cell.set_facecolor("#1976d2")
        cell.set_text_props(weight="bold", color="white")

    # 有効/該当なし のセル装飾
    for i in range(1, len(data)):
        for j in range(1, 4):
            cell = table[i, j]
            text = data[i][j]
            if "有効" in text:
                cell.set_facecolor("#c8e6c9")
            elif "該当なし" in text:
                cell.set_facecolor("#ffcdd2")

    ax.text(0.5, 0.12,
            "所見: Sunburst と Sankey は設計スタイルを問わず有効、"
            "Heatmap と Chord は OOP 対象でのみ機能する。\n"
            "可視化は対象の性質に応じて組み合わせて使うことが本研究の推奨である。",
            ha="center", va="bottom", fontsize=10, wrap=True,
            transform=ax.transAxes)

    pdf.savefig(fig, bbox_inches="tight")
    plt.close(fig)


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--output", required=True, type=Path)
    args = parser.parse_args()

    base = Path(__file__).parent.parent
    sources = [
        ("reversi", "is_valid_move", base / "experiments/reversi/test-requirements/test-requirements.yaml"),
        ("sakura-editor", "TGT-04", base / "test-requirements/test-requirements.yaml"),
        ("pallets/click", "TGT-05", base / "experiments/click/test-requirements/test-requirements.yaml"),
    ]

    args.output.parent.mkdir(parents=True, exist_ok=True)

    with PdfPages(str(args.output)) as pdf:
        # タイトルページ
        first_trm = yaml.safe_load(sources[0][2].read_text(encoding="utf-8"))
        all_targets = []
        for _, tid_or_name, path in sources:
            trm = yaml.safe_load(path.read_text(encoding="utf-8"))
            for t in trm.get("targets", []):
                tid = t.get("id", "")
                name = target_name(t)
                if tid == tid_or_name or tid_or_name in name or name == tid_or_name:
                    all_targets.append((trm.get("project", {}).get("name", "?"), t))
                    break
                elif tid_or_name == "is_valid_move" and name == "is_valid_move":
                    all_targets.append((trm.get("project", {}).get("name", "?"), t))
                    break

        fig, ax = plt.subplots(figsize=(8.5, 11))
        ax.set_axis_off()
        ax.text(0.5, 0.92, "4種の可視化レイヤ — 3対象ショーケース", ha="center",
                fontsize=20, weight="bold", transform=ax.transAxes)
        ax.text(0.5, 0.86,
                "TRM を入力として Sunburst / Sankey / Heatmap / Chord を自動生成",
                ha="center", fontsize=12, transform=ax.transAxes)
        ax.text(0.5, 0.82, "生成: scripts/generate_visualization_showcase_pdf.py",
                ha="center", fontsize=9, color="#666", transform=ax.transAxes)

        desc = [
            ("Sunburst", "#1976d2", "要求の種別階層を中心から放射状に配置。3秒で全体像把握。"),
            ("Sankey",   "#f9a825", "種別×優先度の流量を横積みバー。集中箇所と緊急度を可視化。"),
            ("Heatmap",  "#d32f2f", "state×risk の交差表。複合リスクを定量表現 (OOP対象のみ)。"),
            ("Chord",    "#388e3c", "状態変数とリスクを円環配置、線種で重大度を強調 (OOP対象のみ)。"),
        ]
        y = 0.68
        for name, color, text in desc:
            ax.text(0.08, y, "■", fontsize=18, color=color, transform=ax.transAxes)
            ax.text(0.14, y, name, fontsize=14, weight="bold", transform=ax.transAxes)
            ax.text(0.14, y - 0.025, text, fontsize=10, transform=ax.transAxes)
            y -= 0.08

        ax.text(0.5, 0.30, "対象3件:", ha="center", fontsize=13, weight="bold",
                transform=ax.transAxes)
        cases = [
            ("1. リバーシ is_valid_move",    "関数型・合成題材"),
            ("2. sakura IsMailAddress",      "手続き型・実OSS (C++)"),
            ("3. pallets/click IntRange",    "OOP・実OSS (Python, ダイヤモンド継承)"),
        ]
        y = 0.25
        for title, note in cases:
            ax.text(0.15, y, title, fontsize=11, weight="bold", transform=ax.transAxes)
            ax.text(0.62, y, f"({note})", fontsize=10, color="#555", transform=ax.transAxes)
            y -= 0.04

        pdf.savefig(fig, bbox_inches="tight")
        plt.close(fig)

        for project, target in all_targets:
            make_target_page(pdf, target, project)

        make_summary_page(pdf)

    print(f"Generated: {args.output}")
    print(f"  Pages: {1 + 2 * len(all_targets) + 1} "
          f"(title + {len(all_targets)} targets × 2 + summary)")


if __name__ == "__main__":
    main()
