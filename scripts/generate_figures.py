#!/usr/bin/env python3
"""SQiP 2026 図表生成 (matplotlib, 日本語対応)"""
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import numpy as np

plt.rcParams["font.family"] = "Hiragino Sans"
plt.rcParams["font.size"] = 10
plt.rcParams["axes.unicode_minus"] = False

OUT = "/Users/suetaketakaya/SoftwareQualitySymposium/drafts/figures"

# =========================================================================
# 図1: 全体構成図
# =========================================================================
def fig1():
    fig, ax = plt.subplots(figsize=(10, 5))
    ax.set_xlim(0, 10)
    ax.set_ylim(0, 5)
    ax.axis("off")
    fig.patch.set_facecolor("white")

    # Phase boxes
    phases = [
        (0.3, "Step 1\nリポジトリ解析", "#d0d0d0", "入力:\nソースコード", "LLM: 構造解析\n人間: 選定承認", "出力:\n対象関数8件"),
        (3.5, "Step 2\nTRM生成", "#b0b0b0", "入力:\n対象ソースコード", "LLM: 静的解析+推論\n人間: レビュー修正", "出力:\nTRM (YAML 99件)"),
        (6.7, "Step 3\nテストコード生成", "#d0d0d0", "入力:\nTRM (YAML)", "LLM: コード生成\n人間: 確認", "出力:\nGoogle Test 145件"),
    ]
    for x, title, color, inp, role, out in phases:
        # Main box
        ax.add_patch(plt.Rectangle((x, 0.6), 2.7, 3.8, facecolor="#f5f5f5", edgecolor="#555", lw=1.5, zorder=1))
        # Title bar
        ax.add_patch(plt.Rectangle((x, 4.0), 2.7, 0.4, facecolor=color, edgecolor="#555", lw=1.5, zorder=2))
        ax.text(x+1.35, 4.2, title, ha="center", va="center", fontsize=9, fontweight="bold", zorder=3)
        # Input (dashed)
        ax.add_patch(plt.Rectangle((x+0.2, 3.3), 2.3, 0.5, facecolor="white", edgecolor="#999", lw=0.8, ls="--", zorder=2))
        ax.text(x+1.35, 3.55, inp, ha="center", va="center", fontsize=7.5, zorder=3)
        # Role
        ax.add_patch(plt.Rectangle((x+0.2, 2.3), 2.3, 0.8, facecolor="#e8e8e8", edgecolor="#aaa", lw=0.8, zorder=2))
        ax.text(x+1.35, 2.7, role, ha="center", va="center", fontsize=7.5, zorder=3)
        # Output
        lw = 2.5 if "TRM" in out else 1.2
        fc = "#d9d9d9" if "TRM" in out else "white"
        ax.add_patch(plt.Rectangle((x+0.2, 1.2), 2.3, 0.8, facecolor=fc, edgecolor="#333", lw=lw, zorder=2))
        ax.text(x+1.35, 1.6, out, ha="center", va="center", fontsize=7.5, fontweight="bold", zorder=3)

    # Arrows
    ax.annotate("", xy=(3.5, 2.7), xytext=(3.0, 2.7),
                arrowprops=dict(arrowstyle="->", color="#444", lw=2))
    ax.annotate("", xy=(6.7, 2.7), xytext=(6.2, 2.7),
                arrowprops=dict(arrowstyle="->", color="#444", lw=2))

    # Result box
    ax.add_patch(plt.Rectangle((6.9, 0.1), 2.3, 0.7, facecolor="#f0f0f0", edgecolor="#888", lw=1))
    ax.text(8.05, 0.55, "初回PASS率: 91.7%", ha="center", fontsize=8)
    ax.text(8.05, 0.3, "修正後: 100% (145/145)", ha="center", fontsize=8)

    ax.set_title("図1: 提案手法の全体構成（3段階パイプライン）", fontsize=12, fontweight="bold", pad=10)
    fig.tight_layout()
    fig.savefig(f"{OUT}/fig1-overview.png", dpi=200, bbox_inches="tight")
    plt.close()

# =========================================================================
# 図2: テスト要求 種別内訳 (円グラフ)
# =========================================================================
def fig2():
    fig, ax = plt.subplots(figsize=(7, 5))
    labels = ["BR (分岐網羅)\n55件", "EC (同値クラス)\n27件", "BV (境界値)\n11件", "ER (エラーパス)\n3件", "DP (依存切替)\n3件"]
    sizes = [55, 27, 11, 3, 3]
    colors = ["#555555", "#999999", "#bbbbbb", "#dddddd", "#444444"]
    hatches = ["", "//", "xx", "..", "\\\\"]
    explode = (0.02, 0.02, 0.02, 0.05, 0.05)

    wedges, texts, autotexts = ax.pie(
        sizes, labels=labels, autopct="%1.1f%%", startangle=90,
        colors=colors, explode=explode, textprops={"fontsize": 9},
        pctdistance=0.75)
    for w, h in zip(wedges, hatches):
        w.set_hatch(h)
        w.set_edgecolor("white")
    for t in autotexts:
        t.set_fontsize(9)
        t.set_color("white")
        t.set_fontweight("bold")

    ax.set_title("図2: テスト要求の種別内訳（N=99）", fontsize=12, fontweight="bold")
    fig.tight_layout()
    fig.savefig(f"{OUT}/fig2-requirements-breakdown.png", dpi=200, bbox_inches="tight")
    plt.close()

# =========================================================================
# 図3: 既存テスト vs 生成テスト (棒グラフ)
# =========================================================================
def fig3():
    fig, ax = plt.subplots(figsize=(8, 5))
    categories = ["format.cpp\n(3関数)", "CWordParse.cpp\n(3関数)", "convert_util.cpp\n(2関数)"]
    existing = [15, 15, 10]
    generated = [61, 53, 31]
    ratios = ["x4.1", "x3.5", "x3.1"]

    x = np.arange(len(categories))
    w = 0.35
    bars1 = ax.bar(x - w/2, existing, w, label="既存テスト", color="#aaaaaa", edgecolor="#333", hatch="//")
    bars2 = ax.bar(x + w/2, generated, w, label="生成テスト（提案手法）", color="#555555", edgecolor="#333")

    for i, (b, r) in enumerate(zip(bars2, ratios)):
        ax.text(b.get_x() + b.get_width()/2, b.get_height() + 1.5, r,
                ha="center", fontsize=11, fontweight="bold", color="#c44444")

    ax.set_ylabel("テストケース数", fontsize=11)
    ax.set_xticks(x)
    ax.set_xticklabels(categories, fontsize=10)
    ax.legend(fontsize=10)
    ax.set_ylim(0, 75)
    ax.set_title("図3: 既存テストと生成テストの比較", fontsize=12, fontweight="bold")
    ax.spines["top"].set_visible(False)
    ax.spines["right"].set_visible(False)

    fig.tight_layout()
    fig.savefig(f"{OUT}/fig3-test-comparison.png", dpi=200, bbox_inches="tight")
    plt.close()

# =========================================================================
# 図4: コンパイル・実行結果
# =========================================================================
def fig4():
    fig, ax = plt.subplots(figsize=(8, 5))

    # 3 columns: Total, Initial, After fix
    cols = ["生成テスト\n総数", "初回実行\n結果", "修正後"]
    x = [0, 1.2, 2.4]
    w = 0.7

    # Column 1: Total 145
    ax.bar(x[0], 145, w, color="#888888", edgecolor="#333")
    ax.text(x[0], 147, "145件", ha="center", fontsize=11, fontweight="bold")

    # Column 2: Stacked - PASS 133 + FAIL 12
    ax.bar(x[1], 133, w, color="#666666", edgecolor="#333", label="PASS")
    ax.bar(x[1], 6, w, bottom=133, color="#cccccc", edgecolor="#333", hatch="//", label="FAIL: ParseVersion (6)")
    ax.bar(x[1], 3, w, bottom=139, color="#eeeeee", edgecolor="#333", hatch="..", label="FAIL: IsMailAddress (3)")
    ax.bar(x[1], 3, w, bottom=142, color="#dddddd", edgecolor="#333", hatch="\\\\", label="FAIL: TwoChars (3)")

    ax.text(x[1], 118, "PASS\n133件\n(91.7%)", ha="center", fontsize=10, fontweight="bold", color="white")

    # Brace for FAIL
    ax.annotate("FAIL 12件\n(8.3%)", xy=(x[1]+w/2+0.05, 137.5), xytext=(x[1]+0.7, 137.5),
                fontsize=9, fontweight="bold", color="#c44444",
                arrowprops=dict(arrowstyle="-", color="#c44444", lw=1))

    # Column 3: All PASS 145
    ax.bar(x[2], 145, w, color="#444444", edgecolor="#333")
    ax.text(x[2], 72, "ALL PASS\n145件\n(100%)", ha="center", fontsize=10, fontweight="bold", color="white")
    ax.text(x[2], 147, "145件", ha="center", fontsize=11, fontweight="bold")

    # Arrow
    ax.annotate("", xy=(x[2]-w/2, 130), xytext=(x[1]+w/2+0.05, 130),
                arrowprops=dict(arrowstyle="->", color="#555", lw=1.5))
    ax.text((x[1]+x[2])/2, 125, "期待値修正", ha="center", fontsize=9, color="#555")

    ax.set_xticks(x)
    ax.set_xticklabels(cols, fontsize=10)
    ax.set_ylabel("テストケース数", fontsize=11)
    ax.set_ylim(0, 160)
    ax.set_xlim(-0.6, 3.2)
    ax.set_title("図4: コンパイル・実行結果", fontsize=12, fontweight="bold")
    ax.spines["top"].set_visible(False)
    ax.spines["right"].set_visible(False)
    ax.legend(loc="center right", fontsize=8)

    fig.tight_layout()
    fig.savefig(f"{OUT}/fig4-execution-results.png", dpi=200, bbox_inches="tight")
    plt.close()

if __name__ == "__main__":
    fig1()
    fig2()
    fig3()
    fig4()
    print("All figures generated.")
