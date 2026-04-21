#!/usr/bin/env python3
"""generate_visualizations.py — TRM YAML から可視化 Markdown を生成する。

研究論文の手法章で "可視化レイヤ" として提案される自動生成パイプライン。
入力の test-requirements.yaml から各 TGT について以下を出力:

  - {name}-sunburst.md   — Mermaid mindmap (トリガー階層)
  - {name}-sankey.md      — Mermaid sankey-beta (種別分布)
  - {name}-heatmap.md     — Markdown テーブル (複合影響マトリクス)
  - {name}-chord.md       — Mermaid flowchart (相互関係・禁忌)
  - {name}-summary.md     — 上記4枚を統合した1ファイル

使い方:
  python scripts/generate_visualizations.py \\
    --trm experiments/click/test-requirements/test-requirements.yaml \\
    --output experiments/click/visualizations/

  # 特定 TGT のみ:
  python scripts/generate_visualizations.py \\
    --trm ... --target TGT-05 --output ...
"""
from __future__ import annotations

import argparse
import re
from collections import Counter, defaultdict
from pathlib import Path

import yaml


# ---------------------------------------------------------------------------
# TRM 読み込み
# ---------------------------------------------------------------------------

def load_trm(path: Path) -> dict:
    return yaml.safe_load(path.read_text(encoding="utf-8"))


# ---------------------------------------------------------------------------
# 共通ユーティリティ
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

TYPE_COLOR_EMOJI = {
    "branch_coverage": "🟦",
    "equivalence_class": "🟩",
    "boundary_value": "🟨",
    "error_path": "🟥",
    "dependency_path": "🟪",
    "class_inheritance": "🔷",
    "state_variable": "🟫",
    "code_pattern": "⬛",
    "encapsulation": "🟧",
}


def sanitize_name(name: str) -> str:
    """ファイル名用: ハイフンを保持"""
    return re.sub(r"[^\w\-]+", "_", name).strip("_")


def sanitize_mermaid_id(name: str) -> str:
    """Mermaid ノードID用: ハイフンも '_' に置換"""
    return re.sub(r"[^A-Za-z0-9_]+", "_", name).strip("_")


def target_name(target: dict) -> str:
    cls = target.get("class_name", "")
    sig = target.get("function_signature", "")
    if cls:
        return cls.split(",")[0].strip()
    if sig:
        return sig.split("(")[0].strip()
    return target.get("id", "Unknown")


def infer_type_from_id(req_id: str) -> str:
    """ID prefix から種別を推定 (v1.0 スキーマ用フォールバック)

    v3.1 スキーマは test_requirements の各項目に type フィールドを持つが、
    v1.0 (旧 sakura TRM) では type が欠落しているため ID prefix で判別する。
    """
    prefix_map = {
        "BR": "branch_coverage",
        "EC": "equivalence_class",
        "BV": "boundary_value",
        "ER": "error_path",
        "DP": "dependency_path",
        "CI": "class_inheritance",
        "SV": "state_variable",
        "CP": "code_pattern",
        "EN": "encapsulation",
    }
    prefix = req_id.split("-")[0] if "-" in req_id else ""
    return prefix_map.get(prefix, "other")


def req_type(req: dict) -> str:
    """要求の種別を取得: v3.1 の type フィールド優先、無ければ ID prefix から推定"""
    t = req.get("type")
    if t:
        return t
    return infer_type_from_id(req.get("id", ""))


def count_by_type(target: dict) -> Counter:
    """種別別カウント (v1.0/v3.1 両対応)"""
    return Counter(req_type(req) for req in target.get("test_requirements", []))


# ---------------------------------------------------------------------------
# Sunburst (Mermaid mindmap)
# ---------------------------------------------------------------------------

def generate_sunburst(target: dict) -> str:
    name = target_name(target)
    reqs = target.get("test_requirements", [])
    groups: dict[str, list[dict]] = defaultdict(list)
    for req in reqs:
        groups[req_type(req)].append(req)

    lines = ["```mermaid", "mindmap", f"  root(({name}))"]
    for rtype, rlist in groups.items():
        label = TYPE_LABEL_JA.get(rtype, rtype)
        lines.append(f"    {label}")
        for req in rlist[:4]:
            desc = req.get("description", "").replace("\n", " ")[:40]
            lines.append(f"      {req['id']}: {desc}")
        if len(rlist) > 4:
            lines.append(f"      ...他{len(rlist) - 4}件")
    lines.append("```")
    return "\n".join(lines)


# ---------------------------------------------------------------------------
# Sankey (Mermaid sankey-beta)
# ---------------------------------------------------------------------------

def generate_sankey(target: dict) -> str:
    name = target_name(target)
    counts = count_by_type(target)
    total = sum(counts.values())
    if total == 0:
        return "> (要求なしのため Sankey 生成不可)"

    lines = ["```mermaid", "sankey-beta", ""]
    for rtype, n in counts.items():
        label = TYPE_LABEL_JA.get(rtype, rtype)
        lines.append(f"{name},{label},{n}")

    priority = Counter(
        req.get("priority", "medium") for req in target.get("test_requirements", [])
    )
    for rtype in counts:
        label = TYPE_LABEL_JA.get(rtype, rtype)
        for pri, pn in priority.items():
            share = round(counts[rtype] * pn / total)
            if share > 0:
                lines.append(f"{label},優先度:{pri},{share}")

    lines.append("```")
    return "\n".join(lines)


# ---------------------------------------------------------------------------
# Heatmap (Markdown table)
# ---------------------------------------------------------------------------

SEVERITY_EMOJI = {"high": "🔴", "medium": "🟡", "low": "🟢"}


def generate_heatmap(target: dict) -> str:
    """state_variables × encapsulation_risks の交差ヒートマップ。

    OOP 対象では state × risk の観点で複合影響が最も顕在化するため、
    このペアをデフォルトのヒートマップ軸として採用する。
    """
    svars = target.get("state_variables") or []
    risks = target.get("encapsulation_risks") or []
    if not svars or not risks:
        return "> (state_variables または encapsulation_risks が空のためヒートマップ対象外)"

    risk_by_field: dict[str, list[dict]] = defaultdict(list)
    for r in risks:
        risk_by_field[r.get("field_name", "")].append(r)

    cols = ["missing_validation", "leaky_getter", "leaky_setter",
            "unintended_mutability", "external_mutation", "invariant_breach",
            "public_mutable_field"]

    lines = ["| field | " + " | ".join(cols) + " |"]
    lines.append("|---|" + "|".join("---" for _ in cols) + "|")

    for sv in svars:
        fname = sv.get("name", "")
        row = [fname]
        matched = risk_by_field.get(fname, [])
        for col in cols:
            severity = ""
            for r in matched:
                if r.get("risk_type") == col:
                    severity = SEVERITY_EMOJI.get(r.get("severity", "low"), "")
                    break
            row.append(severity or "—")
        lines.append("| " + " | ".join(row) + " |")

    lines.append("")
    lines.append("**凡例**: 🔴 high / 🟡 medium / 🟢 low / — 検出なし")
    return "\n".join(lines)


# ---------------------------------------------------------------------------
# Chord (Mermaid flowchart)
# ---------------------------------------------------------------------------

def generate_chord(target: dict) -> str:
    """state_variables を円環配置し、encapsulation_risks を線で結ぶ。"""
    svars = target.get("state_variables") or []
    risks = target.get("encapsulation_risks") or []
    if not svars:
        return "> (state_variables が空のため Chord 生成不可)"

    lines = ["```mermaid", "flowchart LR"]
    for sv in svars:
        vid = sanitize_mermaid_id(sv["id"])
        name = sv.get("name", "")
        lines.append(f'    {vid}["🔵 {name}"]')

    for i, r in enumerate(risks):
        field = r.get("field_name", "")
        src = next((sanitize_mermaid_id(sv["id"]) for sv in svars if sv.get("name") == field), None)
        if not src:
            continue
        rtype = r.get("risk_type", "")
        severity = r.get("severity", "low")
        edge_style = {"high": "==>", "medium": "-->", "low": "-.->"}[severity]
        rnode = f"RISK_{i}"
        emoji = SEVERITY_EMOJI.get(severity, "")
        lines.append(f'    {rnode}["{emoji} {rtype}"]')
        lines.append(f"    {src} {edge_style} {rnode}")

    lines.append("```")
    return "\n".join(lines)


# ---------------------------------------------------------------------------
# Summary Generation
# ---------------------------------------------------------------------------

def generate_summary(target: dict) -> str:
    name = target_name(target)
    tid = target.get("id", "")
    sig = target.get("function_signature", "")
    resp = target.get("responsibility", "")
    reqs = target.get("test_requirements", [])
    counts = count_by_type(target)

    lines = [
        f"# {name} ({tid}) — 可視化レイヤ（自動生成）",
        "",
        f"> **対象**: `{sig}`",
        f"> **責務**: {resp}",
        f"> **総要求数**: {len(reqs)}",
        f"> **種別内訳**: {', '.join(f'{TYPE_COLOR_EMOJI.get(k, '')} {TYPE_LABEL_JA.get(k, k)} {v}' for k, v in counts.items())}",
        "",
        "---",
        "",
        "## 1. トリガー階層（Sunburst / Mindmap）",
        "",
        generate_sunburst(target),
        "",
        "## 2. 種別分布の流量（Sankey）",
        "",
        generate_sankey(target),
        "",
        "## 3. 複合影響のヒートマップ（field × risk）",
        "",
        generate_heatmap(target),
        "",
        "## 4. トリガー相互関係（Chord 風 Flowchart）",
        "",
        generate_chord(target),
        "",
        "---",
        "",
        "## 自動生成のメタ情報",
        "",
        "- ツール: `scripts/generate_visualizations.py`",
        "- 入力スキーマ: TRM v3.1 (`templates/trm-schema.yaml`)",
        "- 図解形式: Mermaid + Markdown",
        "- 対象読者: 非エンジニア + 技術系PM + レビュアー",
    ]
    return "\n".join(lines)


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--trm", required=True, type=Path, help="TRM YAML file")
    parser.add_argument("--target", help="Specific target ID (e.g. TGT-05)")
    parser.add_argument("--output", required=True, type=Path, help="Output directory")
    parser.add_argument("--split", action="store_true",
                        help="Write separate files per visualization (sunburst/sankey/...)")
    args = parser.parse_args()

    args.output.mkdir(parents=True, exist_ok=True)
    trm = load_trm(args.trm)

    generated = []
    for target in trm.get("targets", []):
        if args.target and target.get("id") != args.target:
            continue

        name = sanitize_name(target_name(target))
        if args.split:
            for kind, fn in [
                ("sunburst", generate_sunburst),
                ("sankey", generate_sankey),
                ("heatmap", generate_heatmap),
                ("chord", generate_chord),
            ]:
                out = args.output / f"{name}-{kind}.md"
                out.write_text(fn(target), encoding="utf-8")
                generated.append(out)
        out = args.output / f"{name}-summary.md"
        out.write_text(generate_summary(target), encoding="utf-8")
        generated.append(out)

    print(f"Generated {len(generated)} file(s):")
    for p in generated:
        print(f"  {p}")


if __name__ == "__main__":
    main()
