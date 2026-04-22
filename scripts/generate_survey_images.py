#!/usr/bin/env python3
"""generate_survey_images.py — アンケート用画像5枚を生成

Google Forms に埋め込むための PNG 画像を report/survey-images/ に出力:
  1. reversi-code.png            — リバーシコードのスクリーンショット風
  2. reversi-diorama.png         — リバーシ is_valid_move の機能分類ジオラマ
  3. sakura-diorama.png          — sakura IsMailAddress の機能分類ジオラマ
  4. click-dashboard.png         — click IntRange のジオラマ + 評価ダッシュボード
  5. readability-comparison.png  — 3対象の可読率比較バーグラフ

Usage:
  python scripts/generate_survey_images.py
"""
from __future__ import annotations

import io
import sys
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
matplotlib.rcParams["pdf.fonttype"] = 42
matplotlib.rcParams["font.family"] = ["Hiragino Sans", "sans-serif"]
matplotlib.rcParams["axes.unicode_minus"] = False

import matplotlib.pyplot as plt
import numpy as np
import yaml
from PIL import Image

sys.path.insert(0, str(Path(__file__).parent))
from generate_visualization_pdf import (
    draw_diorama, draw_gap_comparison,
    draw_readability_distribution, draw_v31_oop_effect,
    target_name,
)

BASE = Path(__file__).parent.parent
OUT_DIR = BASE / "report" / "survey-images"


def load_target(trm_path: Path, target_id: str | None = None,
                name_contains: str | None = None) -> tuple[str, dict]:
    trm = yaml.safe_load(trm_path.read_text(encoding="utf-8"))
    project = trm.get("project", {}).get("name", "")
    for t in trm.get("targets", []):
        if target_id and t.get("id") == target_id:
            return project, t
        if name_contains and name_contains in target_name(t):
            return project, t
    raise ValueError(f"Target not found in {trm_path}")


def make_reversi_code_image(output_path: Path):
    code = """# リバーシ: 石を置けるか判定する処理

BOARD_SIZE = 8
DIRECTIONS = [(-1,-1),(-1,0),(-1,1),(0,-1),(0,1),(1,-1),(1,0),(1,1)]

def is_on_board(x, y):
    return 0 <= x < BOARD_SIZE and 0 <= y < BOARD_SIZE

def is_empty(board, x, y):
    return board[x][y] == 0

def count_flippable(board, x, y, dx, dy, me):
    opp = 2 if me == 1 else 1
    nx, ny = x + dx, y + dy
    count = 0
    while is_on_board(nx, ny) and board[nx][ny] == opp:
        count += 1
        nx += dx
        ny += dy
    if count == 0 or not is_on_board(nx, ny) or board[nx][ny] != me:
        return 0
    return count

def is_valid_move(board, x, y, me):
    if not is_on_board(x, y):
        return False
    if not is_empty(board, x, y):
        return False
    for dx, dy in DIRECTIONS:
        if count_flippable(board, x, y, dx, dy, me) > 0:
            return True
    return False
"""
    fig = plt.figure(figsize=(10, 11), facecolor="#1e1e1e")
    ax = fig.add_subplot(111)
    ax.set_facecolor("#1e1e1e")
    ax.text(0.5, 0.97, "【題材A】リバーシの合法手判定（Python 約40行）",
            ha="center", va="top", fontsize=16, weight="bold",
            color="white", transform=ax.transAxes)
    ax.text(0.5, 0.93,
            "以下のコードを読み、何を判定する処理かを想像してください。\n"
            "コードを読んだ経験がなくてもOKです（感じたままでお答えください）。",
            ha="center", va="top", fontsize=11, color="#cccccc",
            transform=ax.transAxes, style="italic")
    ax.text(0.03, 0.86, code, ha="left", va="top", fontsize=10,
            family="Consolas", color="#d4d4d4", transform=ax.transAxes,
            bbox=dict(boxstyle="round,pad=0.6", facecolor="#252526",
                      edgecolor="#404040", linewidth=1))
    ax.set_axis_off()
    fig.savefig(output_path, dpi=180, bbox_inches="tight", facecolor="#1e1e1e")
    plt.close(fig)
    print(f"Generated: {output_path}")


def make_diorama_only_image(output_path: Path, target: dict, project: str,
                            subtitle: str = ""):
    fig = plt.figure(figsize=(14, 9))
    ax = fig.add_subplot(111, projection="3d")
    draw_diorama(ax, target, project)
    plt.tight_layout()
    buf = io.BytesIO()
    fig.savefig(buf, format="png", dpi=180, bbox_inches="tight")
    plt.close(fig)
    buf.seek(0)
    img = Image.open(buf)

    wrap = plt.figure(figsize=(14, 10))
    wax = wrap.add_subplot(111)
    wax.imshow(np.array(img))
    wax.set_axis_off()
    if subtitle:
        wrap.suptitle(subtitle, fontsize=14, weight="bold", y=0.98)
    wrap.subplots_adjust(0, 0, 1, 0.95)
    wrap.savefig(output_path, dpi=180, bbox_inches="tight")
    plt.close(wrap)
    print(f"Generated: {output_path}")


def make_full_dashboard_image(output_path: Path, target: dict, project: str,
                              subtitle: str = ""):
    d_fig = plt.figure(figsize=(14, 7.5))
    d_ax = d_fig.add_subplot(111, projection="3d")
    draw_diorama(d_ax, target, project)
    plt.tight_layout()
    buf = io.BytesIO()
    d_fig.savefig(buf, format="png", dpi=180, bbox_inches="tight")
    plt.close(d_fig)
    buf.seek(0)
    img = Image.open(buf)

    fig = plt.figure(figsize=(14, 11))
    gs = fig.add_gridspec(
        nrows=2, ncols=3, height_ratios=[2.2, 1.0],
        hspace=0.35, wspace=0.35,
        left=0.05, right=0.97, top=0.94, bottom=0.07,
    )
    ax_top = fig.add_subplot(gs[0, :])
    ax_top.imshow(np.array(img))
    ax_top.set_axis_off()
    draw_gap_comparison(fig.add_subplot(gs[1, 0]), target, project)
    draw_readability_distribution(fig.add_subplot(gs[1, 1]), target, project)
    draw_v31_oop_effect(fig.add_subplot(gs[1, 2]), target, project)
    if subtitle:
        fig.suptitle(subtitle, fontsize=14, weight="bold", y=0.98)

    buf2 = io.BytesIO()
    fig.savefig(buf2, format="png", dpi=180, bbox_inches="tight")
    plt.close(fig)
    buf2.seek(0)
    Image.open(buf2).save(output_path, "PNG")
    print(f"Generated: {output_path}")


def make_readability_comparison_image(output_path: Path):
    from functional_categories import EXECUTION_RESULTS

    targets_label = ["リバーシ\n(合成・単純)", "sakura-editor\n(手続き型 C++)",
                     "pallets/click\n(OOP Python)"]
    keys = ["reversi-sample", "sakura-editor/sakura", "pallets/click"]
    data = [EXECUTION_RESULTS[k] for k in keys]

    l1 = [d["readability_l1_pct"] for d in data]
    l2 = [d["readability_l2_pct"] for d in data]
    l3 = [d["readability_l3_pct"] for d in data]
    total_readable = [l1_ + l2_ for l1_, l2_ in zip(l1, l2)]

    fig, ax = plt.subplots(figsize=(12, 7))
    x = np.arange(len(targets_label))
    width = 0.5

    ax.bar(x, l1, width, color="#4caf50", edgecolor="white", linewidth=1.5,
           label="L1: コード知識不要")
    ax.bar(x, l2, width, bottom=l1, color="#fbc02d", edgecolor="white",
           linewidth=1.5, label="L2: ドメイン用語で理解可")
    ax.bar(x, l3, width,
           bottom=[l1_ + l2_ for l1_, l2_ in zip(l1, l2)],
           color="#e53935", edgecolor="white", linewidth=1.5,
           label="L3: コード知識が必要")

    for i, r in enumerate(total_readable):
        ax.text(i, 102, f"可読率\n{r:.1f}%", ha="center", va="bottom",
                fontsize=13, weight="bold", color="#1b5e20")

    for i in range(len(targets_label)):
        if l1[i] >= 8:
            ax.text(i, l1[i] / 2, f"L1 {l1[i]:.0f}%", ha="center", va="center",
                    fontsize=11, color="white", weight="bold")
        if l2[i] >= 8:
            ax.text(i, l1[i] + l2[i] / 2, f"L2 {l2[i]:.0f}%",
                    ha="center", va="center", fontsize=11, color="white",
                    weight="bold")
        if l3[i] >= 8:
            ax.text(i, l1[i] + l2[i] + l3[i] / 2, f"L3 {l3[i]:.0f}%",
                    ha="center", va="center", fontsize=11, color="white",
                    weight="bold")

    ax.set_xticks(x)
    ax.set_xticklabels(targets_label, fontsize=11, weight="bold")
    ax.set_ylabel("要求の可読性レベル分布 (%)", fontsize=11)
    ax.set_ylim(0, 115)
    ax.set_yticks([0, 25, 50, 75, 100])
    ax.set_yticklabels(["0%", "25%", "50%", "75%", "100%"], fontsize=10)
    ax.set_title(
        "【補足】3対象における可読性レベル分布の比較\n"
        "対象の技術的複雑度（左→右）に応じて、コード知識必要度 (L3) が増加する",
        fontsize=13, weight="bold", pad=12,
    )
    ax.legend(loc="upper right", fontsize=10, framealpha=0.95)
    ax.spines["top"].set_visible(False)
    ax.spines["right"].set_visible(False)
    ax.grid(axis="y", alpha=0.3)
    fig.tight_layout()
    fig.savefig(output_path, dpi=180, bbox_inches="tight")
    plt.close(fig)
    print(f"Generated: {output_path}")


def main():
    OUT_DIR.mkdir(parents=True, exist_ok=True)

    make_reversi_code_image(OUT_DIR / "01-reversi-code.png")

    project, target = load_target(
        BASE / "experiments/reversi/test-requirements/test-requirements.yaml",
        name_contains="is_valid_move",
    )
    make_diorama_only_image(
        OUT_DIR / "02-reversi-diorama.png", target, project,
        subtitle="【題材A・パターン3】リバーシ is_valid_move の機能分類ジオラマ",
    )

    project, target = load_target(
        BASE / "test-requirements/test-requirements.yaml",
        name_contains="IsMailAddress",
    )
    make_diorama_only_image(
        OUT_DIR / "03-sakura-diorama.png", target, project,
        subtitle="【題材B】sakura-editor IsMailAddress の機能分類ジオラマ",
    )

    project, target = load_target(
        BASE / "experiments/click/test-requirements/test-requirements.yaml",
        target_id="TGT-05",
    )
    make_full_dashboard_image(
        OUT_DIR / "04-click-dashboard.png", target, project,
        subtitle="【題材C】pallets/click IntRange のジオラマ + 評価ダッシュボード",
    )

    make_readability_comparison_image(OUT_DIR / "05-readability-comparison.png")

    print("\n=== Generated survey images ===")
    for p in sorted(OUT_DIR.glob("*.png")):
        size_kb = p.stat().st_size // 1024
        print(f"  {p.name:40} {size_kb:>4}KB")


if __name__ == "__main__":
    main()
