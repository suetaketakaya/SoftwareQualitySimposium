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
from mpl_toolkits.mplot3d import Axes3D  # noqa: F401

# 機能分類マップのインポート
import sys
sys.path.insert(0, str(Path(__file__).parent))
from functional_categories import (
    FUNCTIONAL_CATEGORIES, RISK_TYPE_JA, VARIABLE_ROLE_JA,
    EXECUTION_RESULTS, get_categories, default_categories,
    get_execution_results,
)


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
    """Heatmap 描画。

    v3.1 OOP データ（state_variables + encapsulation_risks）があれば
    フィールド × リスク種別の交差表を表示。
    無い場合は「要求種別 × 優先度」の代替マトリクスを表示（常に非空）。
    """
    svars = target.get("state_variables") or []
    risks = target.get("encapsulation_risks") or []

    if svars and risks:
        _draw_heatmap_oop(ax, svars, risks, title)
    else:
        _draw_heatmap_type_priority(ax, target, title)


def _draw_heatmap_oop(ax, svars, risks, title):
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

    from matplotlib.colors import ListedColormap
    cmap = ListedColormap(["#ffffff", "#7cb342", "#f9a825", "#d32f2f"])

    ax.imshow(matrix, cmap=cmap, aspect="auto", vmin=0, vmax=3)

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
    ax.set_title(f"Heatmap: {title}\n— フィールド × リスク種別 (OOP) —",
                 fontsize=13, pad=15)

    patches = [
        mpatches.Patch(color="#d32f2f", label="H: high"),
        mpatches.Patch(color="#f9a825", label="M: medium"),
        mpatches.Patch(color="#7cb342", label="L: low"),
    ]
    ax.legend(handles=patches, loc="center left", bbox_to_anchor=(1.01, 0.5),
              fontsize=9)


def _draw_heatmap_type_priority(ax, target, title):
    """OOP データが無い対象向けの代替: 種別 × 優先度"""
    reqs = target.get("test_requirements", [])
    if not reqs:
        ax.text(0.5, 0.5, "要求がないため描画不可", ha="center", va="center")
        ax.set_axis_off()
        return

    by_type_priority = defaultdict(lambda: Counter())
    for req in reqs:
        t = req_type(req)
        p = req.get("priority", "medium")
        by_type_priority[t][p] += 1

    types = sorted(by_type_priority.keys(), key=lambda x: -sum(by_type_priority[x].values()))
    type_labels = [TYPE_LABEL_JA.get(t, t) for t in types]
    priorities = ["high", "medium", "low"]
    priority_colors = ["#d32f2f", "#f9a825", "#7cb342"]

    matrix = np.zeros((len(types), len(priorities)))
    for i, t in enumerate(types):
        for j, p in enumerate(priorities):
            matrix[i][j] = by_type_priority[t].get(p, 0)

    max_val = matrix.max() if matrix.max() > 0 else 1

    from matplotlib.colors import LinearSegmentedColormap
    cmap = LinearSegmentedColormap.from_list(
        "count", ["#ffffff", "#ffe0b2", "#ff9800", "#e65100"])

    ax.imshow(matrix, cmap=cmap, aspect="auto", vmin=0, vmax=max_val)

    for i in range(len(types)):
        for j in range(len(priorities)):
            v = int(matrix[i][j])
            if v > 0:
                color = "white" if v > max_val * 0.5 else "#333"
                ax.text(j, i, str(v), ha="center", va="center",
                        color=color, fontsize=12, weight="bold")
            else:
                ax.text(j, i, "—", ha="center", va="center",
                        color="#bbbbbb", fontsize=11)

    ax.set_xticks(np.arange(len(priorities)))
    ax.set_xticklabels(["high", "medium", "low"], fontsize=10)
    for xtick, color in zip(ax.get_xticklabels(), priority_colors):
        xtick.set_color(color)
        xtick.set_weight("bold")
    ax.set_yticks(np.arange(len(types)))
    ax.set_yticklabels(type_labels, fontsize=9)
    ax.set_title(f"Heatmap: {title}\n— 要求種別 × 優先度 (手続き型代替表示) —",
                 fontsize=13, pad=15)

    # サマリ情報
    totals = {p: sum(by_type_priority[t].get(p, 0) for t in types) for p in priorities}
    summary = f"計: high={totals['high']}, medium={totals['medium']}, low={totals['low']}"
    ax.text(0.5, -0.22, summary, ha="center", va="top", fontsize=9,
            transform=ax.transAxes, color="#555")


# ---------------------------------------------------------------------------
# 4. Chord (状態変数とリスクを円環配置、線種で重大度表現)
# ---------------------------------------------------------------------------

def draw_chord(ax, target: dict, title: str):
    """Chord 描画。

    v3.1 OOP データ (state_variables + encapsulation_risks) があれば
    フィールドとリスクを円環配置。無い場合は依存関係
    (calls / called_by) のグラフを代替表示。
    """
    svars = target.get("state_variables") or []
    risks = target.get("encapsulation_risks") or []

    if svars and risks:
        _draw_chord_oop(ax, svars, risks, title)
    else:
        _draw_chord_dependencies(ax, target, title)


def _draw_chord_dependencies(ax, target, title):
    """OOPデータが無い対象向けの代替: 依存関係グラフ"""
    deps = target.get("dependencies")
    # sakura v1.0 TRM では dependencies がリストか空の場合あり
    if isinstance(deps, dict):
        calls = deps.get("calls") or []
        called_by = deps.get("called_by") or []
        globals_ = deps.get("globals") or []
    elif isinstance(deps, list) and deps:
        # リスト形式: 各要素が {name, type, description} の辞書
        calls = deps
        called_by = []
        globals_ = []
    else:
        calls = []
        called_by = []
        globals_ = []

    if not (calls or called_by or globals_):
        # 最終フォールバック: 要求数を対象単独ノードで表示
        ax.text(0.5, 0.5,
                f"{target_name(target)}\n"
                f"独立関数 (外部依存なし)\n"
                f"要求数: {len(target.get('test_requirements', []))}",
                ha="center", va="center", fontsize=12,
                transform=ax.transAxes,
                bbox=dict(boxstyle="round,pad=0.5", facecolor="#e3f2fd",
                          edgecolor="#1976d2"))
        ax.set_axis_off()
        ax.set_title(f"Chord: {title}\n— 依存関係 (手続き型代替) —",
                     fontsize=13, pad=15)
        return

    # 関数ノード名を取得
    def _name(d):
        if isinstance(d, dict):
            return d.get("name", str(d))
        return str(d)

    calls_names = [_name(c) for c in calls]
    called_by_names = [_name(c) for c in called_by]
    global_names = [_name(g) for g in globals_]

    # 中央に対象、左に calls、右に called_by、下に globals を配置
    center_name = target_name(target)
    ax.scatter(0, 0, s=2000, c="#1976d2", edgecolors="white",
               linewidth=2, zorder=3)
    ax.text(0, 0, center_name, ha="center", va="center", fontsize=9,
            color="white", weight="bold", zorder=4)

    # calls (左上半円)
    if calls_names:
        for i, name in enumerate(calls_names):
            angle = 150 + (i * 60 / max(len(calls_names), 1))
            x = 1.2 * np.cos(np.deg2rad(angle))
            y = 1.2 * np.sin(np.deg2rad(angle))
            ax.scatter(x, y, s=600, c="#66bb6a", edgecolors="white",
                       linewidth=1.5, zorder=3)
            ax.text(x, y, name[:15], ha="center", va="center", fontsize=7,
                    color="white", weight="bold", zorder=4)
            ax.annotate("", xy=(0, 0), xytext=(x, y),
                        arrowprops=dict(arrowstyle="->",
                                        color="#66bb6a",
                                        lw=1.5, alpha=0.7))
        ax.text(-1.5, 1.4, "calls (呼び出す関数)", fontsize=9, color="#388e3c",
                weight="bold")

    # called_by (右下半円)
    if called_by_names:
        for i, name in enumerate(called_by_names):
            angle = -30 - (i * 60 / max(len(called_by_names), 1))
            x = 1.2 * np.cos(np.deg2rad(angle))
            y = 1.2 * np.sin(np.deg2rad(angle))
            ax.scatter(x, y, s=600, c="#ff7043", edgecolors="white",
                       linewidth=1.5, zorder=3)
            ax.text(x, y, name[:15], ha="center", va="center", fontsize=7,
                    color="white", weight="bold", zorder=4)
            ax.annotate("", xy=(x, y), xytext=(0, 0),
                        arrowprops=dict(arrowstyle="->",
                                        color="#ff7043",
                                        lw=1.5, alpha=0.7))
        ax.text(0.8, -1.4, "called_by (呼ばれる元)", fontsize=9, color="#e64a19",
                weight="bold")

    # globals (下)
    if global_names:
        for i, name in enumerate(global_names[:4]):
            x = -1.0 + i * 0.7
            y = -1.0
            ax.scatter(x, y, s=400, c="#ba68c8", edgecolors="white",
                       linewidth=1.5, zorder=3, marker="s")
            ax.text(x, y - 0.2, name[:12], ha="center", va="top", fontsize=7,
                    color="#4a148c")
        ax.text(-1.5, -1.4, "globals", fontsize=9, color="#6a1b9a",
                weight="bold")

    ax.set_xlim(-2.0, 2.0)
    ax.set_ylim(-1.8, 1.8)
    ax.set_aspect("equal")
    ax.set_axis_off()
    ax.set_title(f"Chord: {title}\n— 依存関係マップ (手続き型代替) —",
                 fontsize=13, pad=15)


def _draw_chord_oop(ax, svars, risks, title):

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
# 5. Diorama (機能分類3層ジオラマ) — 非エンジニア向け主役図版
# ---------------------------------------------------------------------------

LAYER_COLORS = {
    "input":      "#e3f2fd",  # 薄青
    "functional": "#fff9c4",  # 薄黄
    "perspective": "#c8e6c9",  # 薄緑
}
LAYER_EDGE_COLORS = {
    "input":      "#1976d2",
    "functional": "#f9a825",
    "perspective": "#388e3c",
}


def _draw_floor(ax, z, x_range, y_range, color, edge_color, alpha=0.25):
    """等角投影の半透明フロア"""
    x = np.linspace(x_range[0], x_range[1], 2)
    y = np.linspace(y_range[0], y_range[1], 2)
    xx, yy = np.meshgrid(x, y)
    zz = np.full_like(xx, z)
    ax.plot_surface(xx, yy, zz, color=color, alpha=alpha,
                    edgecolor=edge_color, linewidth=1.5,
                    shade=False, antialiased=True)


def _place_nodes(ax, nodes, z, color, spacing=1.6, y=0.0, node_size=500,
                 labels_list=None):
    """フロア上にノードを配置し、位置情報を返す。テキストは別途2Dオーバーレイで描く"""
    positions = []
    n = len(nodes)
    if n == 0:
        return positions
    total_width = (n - 1) * spacing
    start_x = -total_width / 2
    for i, node in enumerate(nodes):
        x = start_x + i * spacing
        ax.scatter([x], [y], [z], s=node_size, c=color,
                   edgecolors="white", linewidths=2, depthshade=True)
        positions.append((x, y, z))
        if labels_list is not None:
            labels_list.append({
                "x3d": x, "y3d": y, "z3d": z + 0.2,
                "text": f"{node['icon']}\n{node['label']}",
                "color": color,
            })
    return positions


def _connect_layers(ax, lower_positions, upper_positions, color="#666666"):
    """下層の各ノードから上層の各ノードへ線を描く"""
    for lx, ly, lz in lower_positions:
        for ux, uy, uz in upper_positions:
            ax.plot([lx, ux], [ly, uy], [lz, uz],
                    color=color, alpha=0.3, linewidth=1.0, zorder=1)


def draw_diorama(ax, target: dict, project_name: str):
    """機能分類3層ジオラマ (3D等角投影)。

    日本語テキストは3D空間ではなく ax.text2D で後処理的に重ね描きする
    (matplotlib 3D backend の日本語フォント問題回避のため)。
    """
    name = target_name(target)
    cats = get_categories(name)
    if cats is None:
        cats = default_categories(
            name,
            target.get("responsibility", ""),
            len(target.get("test_requirements", [])),
        )

    # 等角投影 (isometric-like)
    ax.view_init(elev=22, azim=-58)

    # 3つのフロア描画（Z=0, 1, 2）
    x_range = (-3.5, 3.5)
    y_range = (-0.9, 0.9)

    _draw_floor(ax, 0.0, x_range, y_range,
                LAYER_COLORS["input"], LAYER_EDGE_COLORS["input"], alpha=0.45)
    _draw_floor(ax, 1.5, x_range, y_range,
                LAYER_COLORS["functional"], LAYER_EDGE_COLORS["functional"], alpha=0.45)
    _draw_floor(ax, 3.0, x_range, y_range,
                LAYER_COLORS["perspective"], LAYER_EDGE_COLORS["perspective"], alpha=0.45)

    # 各層にノード配置 (テキストラベルは後で投影→2D描画)
    label_data = []
    input_nodes = _place_nodes(
        ax, cats.get("inputs", []), 0.0,
        LAYER_EDGE_COLORS["input"], spacing=1.4, node_size=400,
        labels_list=label_data,
    )
    functional_nodes = _place_nodes(
        ax, cats.get("functional", []), 1.5,
        LAYER_EDGE_COLORS["functional"], spacing=1.6, node_size=500,
        labels_list=label_data,
    )
    perspective_nodes = _place_nodes(
        ax, cats.get("perspectives", []), 3.0,
        LAYER_EDGE_COLORS["perspective"], spacing=1.5, node_size=500,
        labels_list=label_data,
    )

    # 層間接続（下→上）
    _connect_layers(ax, input_nodes, functional_nodes, color="#1976d2")
    _connect_layers(ax, functional_nodes, perspective_nodes, color="#f9a825")

    # 軸を非表示
    ax.set_xlim(-4.5, 3.5)
    ax.set_ylim(-1.3, 1.3)
    ax.set_zlim(-0.3, 4.2)
    ax.set_xticks([])
    ax.set_yticks([])
    ax.set_zticks([])
    ax.grid(False)
    # 軸背景を白に
    ax.xaxis.set_pane_color((1, 1, 1, 0))
    ax.yaxis.set_pane_color((1, 1, 1, 0))
    ax.zaxis.set_pane_color((1, 1, 1, 0))

    # --- 日本語ラベルを2Dオーバーレイで描画 ---
    # 3D座標 → 表示座標変換
    from mpl_toolkits.mplot3d import proj3d
    fig = ax.figure

    def project(x3d, y3d, z3d):
        x2, y2, _ = proj3d.proj_transform(x3d, y3d, z3d, ax.get_proj())
        # ax の data座標 → ax の figure座標
        return ax.transData.transform((x2, y2))

    # ラベル描画 (fig.text で描く)
    for label in label_data:
        pixel_xy = project(label["x3d"], label["y3d"], label["z3d"])
        inv = fig.transFigure.inverted()
        fx, fy = inv.transform(pixel_xy)
        fig.text(fx, fy, label["text"], ha="center", va="bottom",
                 fontsize=8, weight="bold",
                 bbox=dict(boxstyle="round,pad=0.25",
                           facecolor="white",
                           edgecolor=label["color"], linewidth=1, alpha=0.9))

    # 層名ラベル (2D overlay、ax左側)
    for z, text, color in [
        (0.15, "① 目的・対象層\n(入力・パラメータ)", "#1976d2"),
        (1.65, "② 機能分類層\n(業務的な処理分類)", "#f9a825"),
        (3.15, "③ テスト観点層\n(検証の切り口)", "#388e3c"),
    ]:
        pixel_xy = project(-4.2, 0.0, z)
        inv = fig.transFigure.inverted()
        fx, fy = inv.transform(pixel_xy)
        fig.text(fx, fy, text, color=color, fontsize=10, weight="bold",
                 ha="left", va="center")

    # タイトル (2D overlay)
    purpose = cats.get("purpose", "")
    project = cats.get("project") or project_name
    ax.text2D(0.5, 0.97, f"【{project}】 {name}",
              transform=ax.transAxes, fontsize=14, weight="bold", ha="center")
    ax.text2D(0.5, 0.93, f"目的: {purpose}",
              transform=ax.transAxes, fontsize=11, ha="center", color="#333")
    ax.text2D(0.5, 0.02,
              "▼ 入力層 (何が入力か) → 機能分類層 (何を処理するか) → テスト観点層 (どう検証するか) ▼",
              transform=ax.transAxes, fontsize=9, ha="center", color="#666",
              style="italic")


# ---------------------------------------------------------------------------
# Dashboard panels (ジオラマ下部3パネル)
# ---------------------------------------------------------------------------

def draw_priority_donut(ax, target: dict):
    """評価の傾き: 要求の優先度分布をドーナツで表示"""
    reqs = target.get("test_requirements", [])
    if not reqs:
        ax.text(0.5, 0.5, "要求なし", ha="center", va="center",
                transform=ax.transAxes)
        ax.set_axis_off()
        return

    counts = Counter(req.get("priority", "medium") for req in reqs)
    order = ["high", "medium", "low"]
    values = [counts.get(p, 0) for p in order]
    colors = ["#d32f2f", "#f9a825", "#7cb342"]
    labels = [f"{p}\n{v}件" for p, v in zip(order, values) if v > 0]
    vals = [v for v in values if v > 0]
    cols = [c for c, v in zip(colors, values) if v > 0]

    if not vals:
        ax.text(0.5, 0.5, "優先度情報なし", ha="center", va="center",
                transform=ax.transAxes)
        ax.set_axis_off()
        return

    wedges, texts = ax.pie(
        vals, colors=cols, labels=labels,
        startangle=90, counterclock=False,
        wedgeprops=dict(width=0.35, edgecolor="white", linewidth=2),
        textprops=dict(fontsize=10, weight="bold"),
    )
    total = sum(values)
    high_rate = values[0] / total * 100 if total else 0
    ax.text(0, 0, f"計{total}件\nhigh比率\n{high_rate:.0f}%",
            ha="center", va="center", fontsize=10, weight="bold")
    ax.set_title("① 評価の傾き\n(優先度分布)", fontsize=12, weight="bold", pad=8)


def draw_volume_bars(ax, target: dict):
    """テストボリューム: 要求を種別別に横積みで表示"""
    reqs = target.get("test_requirements", [])
    if not reqs:
        ax.text(0.5, 0.5, "要求なし", ha="center", va="center",
                transform=ax.transAxes)
        ax.set_axis_off()
        return

    by_type_priority = defaultdict(lambda: Counter())
    for req in reqs:
        t = req_type(req)
        p = req.get("priority", "medium")
        by_type_priority[t][p] += 1

    types = sorted(by_type_priority.keys(),
                   key=lambda x: -sum(by_type_priority[x].values()))
    type_labels = [TYPE_LABEL_JA.get(t, t) for t in types]
    priorities = ["high", "medium", "low"]
    priority_colors = {"high": "#d32f2f", "medium": "#f9a825", "low": "#7cb342"}

    y = np.arange(len(types))
    left = np.zeros(len(types))
    for p in priorities:
        vals = [by_type_priority[t].get(p, 0) for t in types]
        ax.barh(y, vals, left=left, label=f"{p}",
                color=priority_colors[p], edgecolor="white", linewidth=0.8,
                height=0.7)
        for i, (v, l_start) in enumerate(zip(vals, left)):
            if v > 0:
                ax.text(l_start + v/2, i, str(v), ha="center", va="center",
                        fontsize=8, color="white", weight="bold")
        left += np.array(vals)

    # 総数を右端に
    totals = [sum(by_type_priority[t].values()) for t in types]
    max_total = max(totals) if totals else 1
    for i, total in enumerate(totals):
        ax.text(total + max_total * 0.05, i, f"計{total}", va="center",
                fontsize=8, weight="bold")

    ax.set_yticks(y)
    ax.set_yticklabels(type_labels, fontsize=8)
    ax.set_xlabel("要求件数", fontsize=9)
    ax.set_title(f"② テストボリューム\n(計{sum(totals)}件・種別×優先度)",
                 fontsize=12, weight="bold", pad=8)
    ax.legend(loc="lower right", fontsize=7, framealpha=0.9)
    ax.spines["top"].set_visible(False)
    ax.spines["right"].set_visible(False)
    ax.grid(axis="x", alpha=0.3)
    ax.invert_yaxis()
    ax.set_xlim(0, max_total * 1.25)


def draw_execution_status(ax, project_name: str):
    """実行結果: PASS/FAIL/SKIP の状況"""
    results = get_execution_results(project_name)
    total = results.get("total")

    if total is None:
        ax.text(0.5, 0.7, "⏸", ha="center", va="center", fontsize=60,
                color="#9e9e9e", transform=ax.transAxes)
        ax.text(0.5, 0.35, results.get("status", "未実行"),
                ha="center", va="center", fontsize=14, weight="bold",
                color="#555", transform=ax.transAxes)
        ax.text(0.5, 0.22, results.get("note", ""),
                ha="center", va="center", fontsize=9,
                color="#666", transform=ax.transAxes, wrap=True)
        ax.text(0.5, 0.08, "TRM から次段階でテスト生成・実行予定",
                ha="center", va="center", fontsize=8, style="italic",
                color="#888", transform=ax.transAxes)
        ax.set_axis_off()
        ax.set_title("③ 実行結果\n(PASS/FAIL/SKIP)",
                     fontsize=12, weight="bold", pad=8)
        return

    passed = results.get("pass", 0)
    failed = results.get("fail", 0)
    skipped = results.get("skip", 0)
    final_rate = results.get("final_pass_rate", 0)
    initial_rate = results.get("initial_pass_rate")

    # 横積みバー（1本）
    ax.barh([0], [passed], color="#4caf50", edgecolor="white", linewidth=1.5,
            label=f"PASS {passed}")
    ax.barh([0], [failed], left=[passed], color="#f44336", edgecolor="white",
            linewidth=1.5, label=f"FAIL {failed}")
    ax.barh([0], [skipped], left=[passed + failed], color="#bdbdbd",
            edgecolor="white", linewidth=1.5, label=f"SKIP {skipped}")

    # 中央にラベル
    if passed > 0:
        ax.text(passed / 2, 0, f"✅ {passed}", ha="center", va="center",
                color="white", fontsize=11, weight="bold")
    if failed > 0:
        ax.text(passed + failed / 2, 0, f"❌ {failed}", ha="center", va="center",
                color="white", fontsize=10, weight="bold")
    if skipped > 0:
        ax.text(passed + failed + skipped / 2, 0, f"⏸ {skipped}",
                ha="center", va="center", color="white", fontsize=10,
                weight="bold")

    ax.set_xlim(0, total * 1.05)
    ax.set_yticks([])
    ax.set_xlabel(f"テストケース数 (計 {total})", fontsize=9)
    ax.spines["top"].set_visible(False)
    ax.spines["right"].set_visible(False)
    ax.spines["left"].set_visible(False)

    # 下部にPASS率サマリ
    status_lines = []
    if initial_rate is not None and initial_rate != final_rate:
        status_lines.append(
            f"初回 PASS: {initial_rate:.1f}% → 修正後: {final_rate:.1f}%")
    else:
        status_lines.append(f"PASS率: {final_rate:.1f}%")

    readability = results.get("readability_rate")
    if readability is not None:
        status_lines.append(f"可読率: {readability:.1f}% (L1+L2)")

    status_text = " / ".join(status_lines)
    ax.text(0.5, -0.6, status_text, ha="center", va="center",
            fontsize=10, weight="bold", color="#333",
            transform=ax.transAxes)
    ax.text(0.5, -0.95, results.get("note", ""), ha="center", va="center",
            fontsize=8, color="#666", transform=ax.transAxes, style="italic")

    ax.set_title(f"③ 実行結果\n({results.get('status', '')})",
                 fontsize=12, weight="bold", pad=8)


# ---------------------------------------------------------------------------
# Page生成
# ---------------------------------------------------------------------------

def make_target_page(pdf: PdfPages, target: dict, project_name: str):
    """対象ごとに2ページ生成: (1) 4種の2x2図 + (2) ジオラマフルページ"""
    name = target_name(target)
    target_label = f"{project_name} / {name}"

    # ページ1: 4種の2x2
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

    # ページ2: ジオラマ + ダッシュボード3パネル
    # 3Dジオラマ部分を先にPNGにラスタ化
    import io
    from PIL import Image
    diorama_fig = plt.figure(figsize=(14, 7.5))
    ax3d = diorama_fig.add_subplot(111, projection="3d")
    draw_diorama(ax3d, target, project_name)
    plt.tight_layout()
    buf = io.BytesIO()
    diorama_fig.savefig(buf, format="png", dpi=180, bbox_inches="tight")
    plt.close(diorama_fig)
    buf.seek(0)
    img = Image.open(buf)

    # 統合ページ: 上部ジオラマ画像 + 下部ダッシュボード
    fig2 = plt.figure(figsize=(14, 11))
    gs = fig2.add_gridspec(
        nrows=2, ncols=3,
        height_ratios=[2.2, 1.0],
        hspace=0.35, wspace=0.35,
        left=0.05, right=0.97, top=0.94, bottom=0.07,
    )

    # 上部: ジオラマ画像を全幅で
    ax_top = fig2.add_subplot(gs[0, :])
    ax_top.imshow(np.array(img))
    ax_top.set_axis_off()

    # 下部: 3ダッシュボード
    ax_priority = fig2.add_subplot(gs[1, 0])
    draw_priority_donut(ax_priority, target)

    ax_volume = fig2.add_subplot(gs[1, 1])
    draw_volume_bars(ax_volume, target)

    ax_exec = fig2.add_subplot(gs[1, 2])
    draw_execution_status(ax_exec, project_name)

    fig2.suptitle(
        f"【機能分類ジオラマ + 評価ダッシュボード】{target_label}",
        fontsize=14, weight="bold", y=0.98,
    )
    pdf.savefig(fig2, bbox_inches="tight")
    plt.close(fig2)


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
    print(f"  Pages: {1 + 2 * len(targets)} "
          f"(title + {len(targets)} targets × 2 = 4種図 + ジオラマ)")


if __name__ == "__main__":
    main()
