#!/usr/bin/env python3
"""generate_visualization_pdf.py — TRM YAML から4種の可視化を PDF 化

Mermaid 形式の可視化 (generate_visualizations.py) がプレビュー環境によっては
日本語文字化けするため、matplotlib で直接描画して PDF 化する。

出力: 4種（Sunburst/Sankey/Heatmap/Chord）を対象ごとに PDF 1 ページずつ

Usage:
  python scripts/generate_visualization_pdf.py \\
    --trm experiments/click/test-requirements/test-requirements.yaml \\
    --output report/visualizations-click.pdf
"""
from __future__ import annotations

import argparse
import re
from collections import Counter, defaultdict
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
# TrueType フォント埋込（日本語フォントをPDFに埋め込む）
matplotlib.rcParams["pdf.fonttype"] = 42   # TrueType (embed)
matplotlib.rcParams["ps.fonttype"] = 42
matplotlib.rcParams["pdf.use14corefonts"] = False
# 日本語フォントの優先順位（macOS / Linux / Windows で動く順）
matplotlib.rcParams["font.family"] = [
    "Hiragino Sans", "Yu Gothic", "Noto Sans CJK JP",
    "Meiryo", "Arial Unicode MS", "sans-serif",
]
matplotlib.rcParams["axes.unicode_minus"] = False

import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import numpy as np
import yaml
from matplotlib.backends.backend_pdf import PdfPages


# ---------------------------------------------------------------------------
# 共通
# ---------------------------------------------------------------------------

TYPE_LABEL_JA = {
    "branch_coverage": "分岐網羅 (BR)",
    "equivalence_class": "同値クラス (EC)",
    "boundary_value": "境界値 (BV)",
    "error_path": "エラーパス (ER)",
    "dependency_path": "依存切替 (DP)",
    "class_inheritance": "クラス継承 (CI)",
    "state_variable": "状態変数 (SV)",
    "code_pattern": "コードパターン (CP)",
    "encapsulation": "カプセル化 (EN)",
}

TYPE_COLORS = {
    "branch_coverage": "#1976d2",
    "equivalence_class": "#388e3c",
    "boundary_value": "#f9a825",
    "error_path": "#d32f2f",
    "dependency_path": "#7b1fa2",
    "class_inheritance": "#0097a7",
    "state_variable": "#8d6e63",
    "code_pattern": "#455a64",
    "encapsulation": "#e64a19",
    "other": "#9e9e9e",
}

SEVERITY_COLOR = {
    "high": "#d32f2f",
    "medium": "#f9a825",
    "low": "#7cb342",
}


def infer_type_from_id(req_id: str) -> str:
    prefix_map = {
        "BR": "branch_coverage", "EC": "equivalence_class",
        "BV": "boundary_value", "ER": "error_path",
        "DP": "dependency_path", "CI": "class_inheritance",
        "SV": "state_variable", "CP": "code_pattern",
        "EN": "encapsulation",
    }
    prefix = req_id.split("-")[0] if "-" in req_id else ""
    return prefix_map.get(prefix, "other")


def req_type(req: dict) -> str:
    return req.get("type") or infer_type_from_id(req.get("id", ""))


def target_name(target: dict) -> str:
    cls = target.get("class_name", "")
    sig = target.get("function_signature", "")
    if cls:
        return cls.split(",")[0].strip()
    if sig:
        return sig.split("(")[0].strip()
    return target.get("id", "Unknown")


# ---------------------------------------------------------------------------
# 1. Sunburst (nested pie chart で近似表現)
# ---------------------------------------------------------------------------

def draw_sunburst(ax, target: dict, title: str):
    reqs = target.get("test_requirements", [])
    if not reqs:
        ax.text(0.5, 0.5, "要求がないため描画不可", ha="center", va="center", fontsize=12)
        ax.set_axis_off()
        return

    groups = defaultdict(list)
    for req in reqs:
        groups[req_type(req)].append(req)

    # 内側: 種別
    inner_labels = []
    inner_sizes = []
    inner_colors = []
    outer_labels = []
    outer_sizes = []
    outer_colors = []

    for rtype, rlist in groups.items():
        label = TYPE_LABEL_JA.get(rtype, rtype)
        count = len(rlist)
        color = TYPE_COLORS.get(rtype, "#9e9e9e")
        inner_labels.append(f"{label}\n{count}件")
        inner_sizes.append(count)
        inner_colors.append(color)
        for req in rlist:
            outer_labels.append(req.get("id", ""))
            outer_sizes.append(1)
            # Inner色をわずかに明るく
            outer_colors.append(color)

    # Outer: 個別要求
    if outer_sizes:
        wedges_outer, _ = ax.pie(
            outer_sizes, radius=1.0, colors=outer_colors,
            wedgeprops=dict(width=0.25, edgecolor="white", linewidth=0.5),
            startangle=90, counterclock=False,
        )
        # 個別要求IDを小さく
        for w, l in zip(wedges_outer, outer_labels):
            ang = (w.theta2 + w.theta1) / 2
            x = 0.88 * np.cos(np.deg2rad(ang))
            y = 0.88 * np.sin(np.deg2rad(ang))
            ax.text(x, y, l, ha="center", va="center", fontsize=5.5, color="white", weight="bold")

    # Inner: 種別
    wedges_inner, _ = ax.pie(
        inner_sizes, radius=0.72, colors=inner_colors,
        wedgeprops=dict(width=0.45, edgecolor="white", linewidth=1.5),
        startangle=90, counterclock=False,
    )
    # 種別ラベルを内側に
    for w, l in zip(wedges_inner, inner_labels):
        ang = (w.theta2 + w.theta1) / 2
        x = 0.5 * np.cos(np.deg2rad(ang))
        y = 0.5 * np.sin(np.deg2rad(ang))
        ax.text(x, y, l, ha="center", va="center", fontsize=8, weight="bold", color="white")

    # 中心
    ax.text(0, 0, f"{target_name(target)}\n計{len(reqs)}件", ha="center", va="center",
            fontsize=11, weight="bold")

    ax.set_title(f"Sunburst: {title}\n— 要求の種別階層 —", fontsize=13, pad=20)


# ---------------------------------------------------------------------------
# 2. Sankey (積み上げバーで Target → Type → Priority の流量を表現)
# ---------------------------------------------------------------------------

def draw_sankey(ax, target: dict, title: str):
    reqs = target.get("test_requirements", [])
    if not reqs:
        ax.text(0.5, 0.5, "要求がないため描画不可", ha="center", va="center")
        ax.set_axis_off()
        return

    # Type ごとに priority の内訳
    by_type_priority = defaultdict(lambda: Counter())
    for req in reqs:
        t = req_type(req)
        p = req.get("priority", "medium")
        by_type_priority[t][p] += 1

    # Data for stacked bar
    types = list(by_type_priority.keys())
    type_labels = [TYPE_LABEL_JA.get(t, t) for t in types]
    priorities = ["high", "medium", "low"]
    priority_colors = {"high": "#d32f2f", "medium": "#f9a825", "low": "#7cb342"}

    # Stacked horizontal bar
    y = np.arange(len(types))
    left = np.zeros(len(types))
    for p in priorities:
        vals = [by_type_priority[t].get(p, 0) for t in types]
        bars = ax.barh(y, vals, left=left, label=f"優先度 {p}",
                       color=priority_colors[p], edgecolor="white", linewidth=1)
        # 数値ラベル
        for i, (v, l_start) in enumerate(zip(vals, left)):
            if v > 0:
                ax.text(l_start + v/2, i, str(v), ha="center", va="center",
                        fontsize=9, color="white", weight="bold")
        left += np.array(vals)

    # 総数を右端に
    totals = [sum(by_type_priority[t].values()) for t in types]
    for i, total in enumerate(totals):
        ax.text(total + 0.3, i, f"計{total}", va="center", fontsize=9, weight="bold")

    ax.set_yticks(y)
    ax.set_yticklabels(type_labels, fontsize=10)
    ax.set_xlabel("要求数", fontsize=10)
    ax.set_title(f"Sankey: {title}\n— 種別×優先度の流量 —", fontsize=13, pad=15)
    ax.legend(loc="lower right", fontsize=9, framealpha=0.9)
    ax.spines["top"].set_visible(False)
    ax.spines["right"].set_visible(False)
    ax.grid(axis="x", alpha=0.3)
    ax.invert_yaxis()


# ---------------------------------------------------------------------------
# 3. Heatmap (state_variable × encapsulation_risk)
# ---------------------------------------------------------------------------

RISK_TYPES = [
    "missing_validation", "leaky_getter", "leaky_setter",
    "unintended_mutability", "external_mutation",
    "invariant_breach", "public_mutable_field",
]

SEVERITY_NUM = {"high": 3, "medium": 2, "low": 1}


def draw_heatmap(ax, target: dict, title: str):
    svars = target.get("state_variables") or []
    risks = target.get("encapsulation_risks") or []

    if not svars or not risks:
        ax.text(0.5, 0.5,
                "state_variables または encapsulation_risks が空\n"
                "（手続き型・関数型対象では該当なし）",
                ha="center", va="center", fontsize=12,
                transform=ax.transAxes)
        ax.set_axis_off()
        ax.set_title(f"Heatmap: {title}\n— 複合影響マトリクス —", fontsize=13)
        return

    risk_by_field = defaultdict(list)
    for r in risks:
        risk_by_field[r.get("field_name", "")].append(r)

    field_names = [sv.get("name", "") for sv in svars]
    matrix = np.zeros((len(field_names), len(RISK_TYPES)))
    annot = [["" for _ in RISK_TYPES] for _ in field_names]

    for i, fname in enumerate(field_names):
        for r in risk_by_field.get(fname, []):
            rtype = r.get("risk_type", "")
            severity = r.get("severity", "low")
            if rtype in RISK_TYPES:
                j = RISK_TYPES.index(rtype)
                matrix[i][j] = SEVERITY_NUM.get(severity, 1)
                annot[i][j] = severity[0].upper()

    # カラーマップ
    from matplotlib.colors import ListedColormap
    cmap = ListedColormap(["#ffffff", "#7cb342", "#f9a825", "#d32f2f"])

    im = ax.imshow(matrix, cmap=cmap, aspect="auto", vmin=0, vmax=3)

    # セルに重大度文字
    for i in range(len(field_names)):
        for j in range(len(RISK_TYPES)):
            if annot[i][j]:
                ax.text(j, i, annot[i][j], ha="center", va="center",
                        color="white", fontsize=11, weight="bold")
            else:
                ax.text(j, i, "—", ha="center", va="center",
                        color="#bbbbbb", fontsize=10)

    ax.set_xticks(np.arange(len(RISK_TYPES)))
    ax.set_xticklabels(RISK_TYPES, rotation=35, ha="right", fontsize=8)
    ax.set_yticks(np.arange(len(field_names)))
    ax.set_yticklabels(field_names, fontsize=10)
    ax.set_title(f"Heatmap: {title}\n— フィールド × リスク種別 —", fontsize=13, pad=15)

    # 凡例
    patches = [
        mpatches.Patch(color="#d32f2f", label="H: high"),
        mpatches.Patch(color="#f9a825", label="M: medium"),
        mpatches.Patch(color="#7cb342", label="L: low"),
    ]
    ax.legend(handles=patches, loc="center left", bbox_to_anchor=(1.01, 0.5), fontsize=9)


# ---------------------------------------------------------------------------
# 4. Chord (状態変数とリスクを円環配置、線種で重大度表現)
# ---------------------------------------------------------------------------

def draw_chord(ax, target: dict, title: str):
    svars = target.get("state_variables") or []
    risks = target.get("encapsulation_risks") or []

    if not svars or not risks:
        ax.text(0.5, 0.5,
                "state_variables または encapsulation_risks が空\n"
                "（手続き型・関数型対象では該当なし）",
                ha="center", va="center", fontsize=12,
                transform=ax.transAxes)
        ax.set_axis_off()
        ax.set_title(f"Chord: {title}\n— 状態変数とリスクの相互関係 —", fontsize=13)
        return

    # 左右に分けて配置: 左=state_variables, 右=risks(個別)
    n_left = len(svars)
    n_right = len(risks)

    left_y = np.linspace(1, -1, max(n_left, 1))
    right_y = np.linspace(1, -1, max(n_right, 1))

    left_x = -1.0
    right_x = 1.0

    # ノード描画
    for i, sv in enumerate(svars):
        ax.scatter(left_x, left_y[i], s=800, c="#1976d2", edgecolors="white", linewidth=2, zorder=3)
        ax.text(left_x - 0.15, left_y[i], sv.get("name", ""),
                ha="right", va="center", fontsize=10, weight="bold")

    for j, r in enumerate(risks):
        severity = r.get("severity", "low")
        color = SEVERITY_COLOR.get(severity, "#9e9e9e")
        ax.scatter(right_x, right_y[j], s=800, c=color, edgecolors="white", linewidth=2, zorder=3)
        ax.text(right_x + 0.15, right_y[j], r.get("risk_type", ""),
                ha="left", va="center", fontsize=9)

    # 接続線
    for j, r in enumerate(risks):
        fname = r.get("field_name", "")
        severity = r.get("severity", "low")
        color = SEVERITY_COLOR.get(severity, "#9e9e9e")
        style = {"high": "-", "medium": "--", "low": ":"}[severity]
        lw = {"high": 2.5, "medium": 1.8, "low": 1.2}[severity]
        for i, sv in enumerate(svars):
            if sv.get("name", "") == fname:
                ax.plot([left_x, right_x], [left_y[i], right_y[j]],
                        color=color, linestyle=style, linewidth=lw, alpha=0.7, zorder=2)

    ax.set_xlim(-2.5, 2.8)
    ax.set_ylim(-1.3, 1.3)
    ax.set_axis_off()
    ax.set_title(f"Chord: {title}\n— 状態変数とリスクの相互関係 —", fontsize=13, pad=15)

    # 凡例
    legend_elements = [
        plt.Line2D([0], [0], color="#d32f2f", linestyle="-", linewidth=2.5, label="high risk"),
        plt.Line2D([0], [0], color="#f9a825", linestyle="--", linewidth=1.8, label="medium risk"),
        plt.Line2D([0], [0], color="#7cb342", linestyle=":", linewidth=1.2, label="low risk"),
    ]
    ax.legend(handles=legend_elements, loc="lower center", ncol=3, fontsize=9)


# ---------------------------------------------------------------------------
# Page生成
# ---------------------------------------------------------------------------

def make_target_page(pdf: PdfPages, target: dict, project_name: str):
    name = target_name(target)
    target_label = f"{project_name} / {name}"

    fig, axes = plt.subplots(2, 2, figsize=(14, 10))
    draw_sunburst(axes[0][0], target, target_label)
    draw_sankey(axes[0][1], target, target_label)
    draw_heatmap(axes[1][0], target, target_label)
    draw_chord(axes[1][1], target, target_label)

    fig.suptitle(f"【4種の可視化レイヤ】{target_label}",
                 fontsize=15, weight="bold", y=0.99)
    plt.tight_layout()
    pdf.savefig(fig, bbox_inches="tight")
    plt.close(fig)


def make_title_page(pdf: PdfPages, project_name: str, targets: list[dict]):
    fig, ax = plt.subplots(figsize=(8.5, 11))
    ax.set_axis_off()
    title = "TRM 可視化レイヤ — 4種の図式"
    ax.text(0.5, 0.92, title, ha="center", va="top", fontsize=22, weight="bold",
            transform=ax.transAxes)
    ax.text(0.5, 0.87, f"対象: {project_name}", ha="center", va="top", fontsize=14,
            transform=ax.transAxes)
    ax.text(0.5, 0.83, "生成: scripts/generate_visualization_pdf.py",
            ha="center", va="top", fontsize=10, color="#666",
            transform=ax.transAxes)

    # 4種の説明
    desc = [
        ("Sunburst", "#1976d2", "要求の種別階層を中心から放射状に配置。3秒で全体像把握。"),
        ("Sankey",   "#f9a825", "種別×優先度の流量を横積みバーで表現。集中箇所と緊急度を可視化。"),
        ("Heatmap",  "#d32f2f", "state_variables × encapsulation_risks の交差表。複合リスクを定量表現。"),
        ("Chord",    "#388e3c", "状態変数とリスクを円環配置、線種（実線/破線/点線）で重大度を強調。"),
    ]
    y = 0.70
    for name, color, text in desc:
        ax.text(0.08, y, "■", fontsize=18, color=color, transform=ax.transAxes)
        ax.text(0.13, y, name, fontsize=14, weight="bold", transform=ax.transAxes)
        ax.text(0.13, y - 0.03, text, fontsize=10, transform=ax.transAxes,
                wrap=True)
        y -= 0.10

    ax.text(0.5, 0.15, f"対象ターゲット数: {len(targets)}", ha="center",
            fontsize=11, transform=ax.transAxes)
    ax.text(0.5, 0.10,
            "※ Heatmap/Chord は state_variables と encapsulation_risks を持つ\n"
            "OOP対象（v3.1スキーマ）でのみ描画される。\n"
            "手続き型・関数型対象では該当なしを明示表示する。",
            ha="center", va="top", fontsize=9, color="#555",
            transform=ax.transAxes)

    pdf.savefig(fig, bbox_inches="tight")
    plt.close(fig)


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--trm", required=True, type=Path)
    parser.add_argument("--output", required=True, type=Path)
    parser.add_argument("--target", help="特定 TGT-ID のみ (例: TGT-05)")
    parser.add_argument("--max-targets", type=int, default=None,
                        help="最大描画ターゲット数")
    args = parser.parse_args()

    trm = yaml.safe_load(args.trm.read_text(encoding="utf-8"))
    project = trm.get("project", {})
    project_name = project.get("name", "unknown")
    targets = trm.get("targets", [])

    if args.target:
        targets = [t for t in targets if t.get("id") == args.target]
    if args.max_targets:
        targets = targets[: args.max_targets]

    args.output.parent.mkdir(parents=True, exist_ok=True)

    with PdfPages(str(args.output)) as pdf:
        make_title_page(pdf, project_name, targets)
        for target in targets:
            make_target_page(pdf, target, project_name)

    print(f"Generated: {args.output}")
    print(f"  Pages: {1 + len(targets)} (title + {len(targets)} targets)")


if __name__ == "__main__":
    main()
