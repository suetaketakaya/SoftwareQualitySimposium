#!/usr/bin/env python3
"""SQiP 2026 図表生成 v2 - 図5（品質ギャップ）+ 図1改善"""
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
# 図5: 既存テストの品質ギャップ（横棒グラフ）
# =========================================================================
def fig5():
    fig, ax = plt.subplots(figsize=(8, 4))
    fig.patch.set_facecolor("white")

    # データ（降順: DP→BV→EC→ER→BR）
    labels = ["BR（分岐網羅）", "ER（エラーパス）", "EC（同値クラス）", "BV（境界値）", "DP（依存切替）"]
    rates = [52.7, 66.7, 70.4, 72.7, 100.0]
    annotations = [
        "52.7% (29/55件)",
        "66.7% (2/3件)",
        "70.4% (19/27件)",
        "72.7% (8/11件)",
        "100% (3/3件)",
    ]

    y_pos = np.arange(len(labels))

    # グレースケール基調の色（値が高いほど濃く）
    colors = ["#999999", "#777777", "#666666", "#555555", "#333333"]

    bars = ax.barh(y_pos, rates, height=0.55, color=colors, edgecolor="#333333", linewidth=0.8)

    ax.set_yticks(y_pos)
    ax.set_yticklabels(labels, fontsize=11)
    ax.set_xlim(0, 120)
    ax.set_xlabel("未カバー率 (%)", fontsize=11)

    # 各棒の右にアノテーション
    for i, (bar, ann) in enumerate(zip(bars, annotations)):
        ax.text(bar.get_width() + 1.5, bar.get_y() + bar.get_height() / 2,
                ann, va="center", ha="left", fontsize=10, fontweight="bold", color="#333333")

    # 50%しきい値線（赤い点線）
    ax.axvline(x=50, color="#c44444", linestyle="--", linewidth=1.5, zorder=0)
    ax.text(50, len(labels) - 0.15, "50%", ha="center", va="bottom",
            fontsize=9, color="#c44444", fontweight="bold")

    ax.set_title("図5: 既存テストの品質ギャップ（種別ごとの未カバー率）",
                 fontsize=12, fontweight="bold", pad=12)
    ax.spines["top"].set_visible(False)
    ax.spines["right"].set_visible(False)

    fig.tight_layout()
    fig.savefig(f"{OUT}/fig5-quality-gap.png", dpi=200, bbox_inches="tight")
    plt.close()
    print("  fig5-quality-gap.png generated.")


# =========================================================================
# 図1改善: 全体構成図
# =========================================================================
def fig1_improved():
    fig, ax = plt.subplots(figsize=(12, 5))
    ax.set_xlim(0, 12)
    ax.set_ylim(0, 5.5)
    ax.axis("off")
    fig.patch.set_facecolor("white")

    # --- Step定義 ---
    # (x, title, bg_color, input_text, process_text, output_text)
    steps = [
        (0.2, "Step 1\nリポジトリ解析",
         "#d0d0d0",
         "入力: ソースコード\n（C++リポジトリ）",
         "LLM: 構造解析・関数抽出\n人間: 対象関数の選定承認",
         "出力: 対象関数8件"),
        (4.1, "Step 2\nTRM生成",
         "#888888",
         "入力: 対象ソースコード",
         "LLM: 静的解析+推論\n人間: レビュー・修正",
         "出力: TRM (YAML 99件)"),
        (8.0, "Step 3\nテストコード生成",
         "#d0d0d0",
         "入力: TRM (YAML)",
         "LLM: コード生成\n人間: 最終確認",
         "出力: Google Test 145件"),
    ]

    box_w = 3.5
    box_h = 4.0
    title_h = 0.5

    for i, (x, title, title_bg, inp, proc, out) in enumerate(steps):
        is_step2 = (i == 1)
        # Step2は太枠
        main_lw = 3.0 if is_step2 else 1.5
        main_ec = "#222222" if is_step2 else "#555555"
        main_fc = "#fafafa"

        # メインボックス
        ax.add_patch(plt.Rectangle((x, 0.5), box_w, box_h,
                     facecolor=main_fc, edgecolor=main_ec, lw=main_lw, zorder=1))
        # タイトルバー
        ax.add_patch(plt.Rectangle((x, 0.5 + box_h - title_h), box_w, title_h,
                     facecolor=title_bg, edgecolor=main_ec, lw=main_lw, zorder=2))
        title_fs = 11 if is_step2 else 10
        title_color = "white" if is_step2 else "black"
        ax.text(x + box_w / 2, 0.5 + box_h - title_h / 2, title,
                ha="center", va="center", fontsize=title_fs, fontweight="bold",
                color=title_color, zorder=3)

        # 入力欄（破線枠）
        inp_y = 3.45
        ax.add_patch(plt.Rectangle((x + 0.2, inp_y), box_w - 0.4, 0.6,
                     facecolor="white", edgecolor="#999999", lw=0.8, ls="--", zorder=2))
        ax.text(x + box_w / 2, inp_y + 0.3, inp,
                ha="center", va="center", fontsize=8.5, zorder=3)

        # 処理欄（背景付き）
        proc_y = 2.15
        proc_h = 1.05
        proc_fc = "#d5d5d5" if is_step2 else "#e8e8e8"
        ax.add_patch(plt.Rectangle((x + 0.2, proc_y), box_w - 0.4, proc_h,
                     facecolor=proc_fc, edgecolor="#aaaaaa", lw=0.8, zorder=2))
        ax.text(x + box_w / 2, proc_y + proc_h / 2, proc,
                ha="center", va="center", fontsize=8.5, zorder=3)

        # 出力欄
        out_y = 1.0
        out_h = 0.8
        if is_step2:
            # Step2の出力（TRM）は強調: 太枠、濃い背景
            out_fc = "#b0b0b0"
            out_ec = "#222222"
            out_lw = 3.0
            out_fs = 10
        else:
            out_fc = "white"
            out_ec = "#555555"
            out_lw = 1.2
            out_fs = 9
        ax.add_patch(plt.Rectangle((x + 0.2, out_y), box_w - 0.4, out_h,
                     facecolor=out_fc, edgecolor=out_ec, lw=out_lw, zorder=2))
        ax.text(x + box_w / 2, out_y + out_h / 2, out,
                ha="center", va="center", fontsize=out_fs, fontweight="bold", zorder=3)

    # --- 太い矢印: Step1 → Step2 → Step3 ---
    arrow_y = 2.7
    arrow_props = dict(arrowstyle="->,head_length=0.4,head_width=0.25",
                       color="#333333", lw=3)

    # Step1 → Step2
    ax.annotate("", xy=(4.1, arrow_y), xytext=(3.7, arrow_y),
                arrowprops=arrow_props)
    # Step2 → Step3
    ax.annotate("", xy=(8.0, arrow_y), xytext=(7.6, arrow_y),
                arrowprops=arrow_props)

    # --- TRMからの注釈矢印 ---
    # TRM出力の位置: x=4.1, 出力ボックス中央 = (5.85, 1.4)
    trm_cx = 4.1 + box_w / 2   # 5.85
    trm_top = 1.8               # 出力ボックスの上端
    trm_bot = 1.0               # 出力ボックスの下端

    # 上方向: 「人間がレビュー可能」
    ax.annotate("人間がレビュー可能\n（中間成果物として透明性を確保）",
                xy=(trm_cx, trm_top + 0.02),
                xytext=(trm_cx - 1.8, 5.2),
                fontsize=9, fontweight="bold", color="#333333",
                ha="center", va="center",
                bbox=dict(boxstyle="round,pad=0.3", facecolor="#f0f0f0",
                          edgecolor="#666666", lw=1),
                arrowprops=dict(arrowstyle="->,head_length=0.3,head_width=0.2",
                                color="#555555", lw=1.5,
                                connectionstyle="arc3,rad=-0.2"))

    # 下方向: 「テスト生成の入力」
    ax.annotate("テスト生成の入力\n（構造化された仕様記述）",
                xy=(trm_cx + 0.5, trm_bot - 0.02),
                xytext=(trm_cx + 1.5, -0.15),
                fontsize=9, fontweight="bold", color="#333333",
                ha="center", va="center",
                bbox=dict(boxstyle="round,pad=0.3", facecolor="#f0f0f0",
                          edgecolor="#666666", lw=1),
                arrowprops=dict(arrowstyle="->,head_length=0.3,head_width=0.2",
                                color="#555555", lw=1.5,
                                connectionstyle="arc3,rad=0.2"))

    # --- 結果ボックス ---
    rx, ry = 8.2, 0.0
    ax.add_patch(plt.Rectangle((rx, ry), 3.2, 0.45,
                 facecolor="#f0f0f0", edgecolor="#888888", lw=1, zorder=2))
    ax.text(rx + 1.6, ry + 0.23, "初回PASS率 91.7% → 修正後 100% (145/145)",
            ha="center", va="center", fontsize=8.5, fontweight="bold", zorder=3)

    ax.set_title("図1: 提案手法の全体構成（3段階パイプライン）",
                 fontsize=13, fontweight="bold", pad=12)
    fig.tight_layout()
    fig.savefig(f"{OUT}/fig1-overview.png", dpi=200, bbox_inches="tight")
    plt.close()
    print("  fig1-overview.png generated (improved).")


if __name__ == "__main__":
    print("Generating figures v2...")
    fig5()
    fig1_improved()
    print("All v2 figures generated.")
