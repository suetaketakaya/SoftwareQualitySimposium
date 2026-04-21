#!/usr/bin/env python3
"""classify_readability.py — TRM description を 3レベルで自動分類

分類基準は knowledge/trm-evaluation-criteria.md §3.1 に準拠:
  Level 1: 一般用語のみで理解可能（コード知識不要）
  Level 2: ドメイン用語が含まれるが、その分野の人なら理解可能
  Level 3: 変数名・型名・制御構造等の実装語彙を含む

ルールベースの ヒューリスティック分類器。LLM呼び出しなしで決定的に動く
ため、論文の再現性担保に適する。精度向上が必要な場合は
knowledge/trm-evaluation-criteria.md §B-2 の判定ルールを拡張する。

Usage:
  python scripts/classify_readability.py \\
    --trm experiments/click/test-requirements/test-requirements.yaml \\
    --output experiments/click/reports/readability-classification.md
"""
from __future__ import annotations

import argparse
import re
from collections import Counter, defaultdict
from pathlib import Path

import yaml

# ---------------------------------------------------------------------------
# 3レベル判定の語彙
# ---------------------------------------------------------------------------

# Level 3 のシグナル: 実装語彙・制御構造
L3_KEYWORDS = [
    # 制御構造・言語キーワード
    "NULL", "None", "null", "break", "else", "elif", "continue", "goto",
    "return", "raise", "throw", "try", "except", "catch", "finally",
    "lambda", "yield", "async", "await",
    # 型関連
    "int", "float", "bool", "str", "bytes", "char", "wchar", "void",
    "ClassVar", "TypeVar", "Optional", "Generic",
    "size_t", "uint32", "int32", "int64",
    # OOP 内部
    "__init__", "__call__", "__repr__", "__str__", "__mro__",
    "dunder", "super()", "override", "virtual", "abstract",
    "metaclass",
    # 実装詳細
    "offset", "pnAddressLength", "ECharKind", "CSYM", "UDEF", "CK_",
    "IndexError", "AttributeError", "TypeError", "ValueError",
    "BadParameter", "RuntimeError", "StopIteration",
    "MRO", "ABC", "token_normalize_func",
    # コード構造
    "ビット演算", "ポインタ", "参照", "アドレス",
    "分岐条件", "条件式", "戻り値", "引数",
    "メソッド", "インスタンス", "クラス変数",
    "コンストラクタ", "デストラクタ", "public", "private", "protected",
    "ClassVar", "readonly", "mutable", "immutable",
]

# Level 2 のシグナル: ドメイン用語・テスト工学用語
L2_KEYWORDS = [
    # テスト工学
    "境界値", "同値クラス", "同値分割", "網羅", "カバレッジ",
    "0パディング", "エスケープ", "null文字", "正規化",
    "オーバーライド", "継承", "多態", "ポリモフィズム",
    "直前", "直後", "境界外", "上限", "下限",
    # メール関連ドメイン
    "メールアドレス", "ドメイン", "ローカルパート", "ラベル",
    # バージョン関連
    "バージョン", "修飾子", "コンポーネント", "リリース",
    # 書式・文字処理
    "書式指定子", "書式文字列", "リテラル", "デコード", "エンコード",
    "全角", "半角", "長音", "濁点", "カタカナ", "ひらがな",
    # click ドメイン
    "CLI", "コマンドライン", "オプション", "引数", "フラグ",
    "選択肢", "候補", "列挙",
    "構築", "初期化", "インスタンス化",
    # リバーシ・盤面
    "盤面", "方向", "挟める", "合法手", "手番",
]


def classify_description(description: str) -> tuple[str, list[str], list[str]]:
    """description を L1/L2/L3 に分類。一致したキーワードも返す。

    Returns:
        (level, l3_hits, l2_hits)
        level: "L1" | "L2" | "L3"
    """
    text = description or ""
    l3_hits = [kw for kw in L3_KEYWORDS if kw in text]
    l2_hits = [kw for kw in L2_KEYWORDS if kw in text]

    if l3_hits:
        return "L3", l3_hits, l2_hits
    if l2_hits:
        return "L2", l3_hits, l2_hits
    return "L1", [], []


# ---------------------------------------------------------------------------
# 集計・レポート
# ---------------------------------------------------------------------------

def classify_trm(trm: dict) -> list[dict]:
    """TRM 全要求を分類してリストで返す"""
    rows = []
    for target in trm.get("targets", []):
        tid = target.get("id", "")
        cls = target.get("class_name", "") or target.get("function_signature", "")[:40]
        for req in target.get("test_requirements", []):
            rid = req.get("id", "")
            rtype = req.get("type", "")
            desc = req.get("description", "")
            priority = req.get("priority", "medium")
            level, l3, l2 = classify_description(desc)
            rows.append({
                "target_id": tid,
                "target_name": cls,
                "req_id": rid,
                "type": rtype,
                "priority": priority,
                "level": level,
                "description": desc,
                "l3_hits": l3,
                "l2_hits": l2,
            })
    return rows


def generate_report(rows: list[dict], trm: dict) -> str:
    """レポートを Markdown で生成"""
    total = len(rows)
    if total == 0:
        return "# 可読性分類レポート\n\n要求が0件のため分類結果なし。\n"

    levels = Counter(r["level"] for r in rows)
    l1 = levels.get("L1", 0)
    l2 = levels.get("L2", 0)
    l3 = levels.get("L3", 0)
    readable_rate = (l1 + l2) / total * 100

    project = trm.get("project", {})
    project_name = project.get("name", "unknown")

    lines = [
        f"# 可読性自動分類レポート — {project_name}",
        "",
        f"> **対象**: {total}件の要求",
        f"> **判定**: ルールベース分類（`scripts/classify_readability.py`）",
        f"> **準拠**: `knowledge/trm-evaluation-criteria.md` §3.1",
        "",
        "---",
        "",
        "## 1. 全体サマリ",
        "",
        f"| Level | 件数 | 割合 |",
        f"|---|---|---|",
        f"| L1 (コード知識不要) | {l1} | {l1/total*100:.1f}% |",
        f"| L2 (ドメイン知識のみ) | {l2} | {l2/total*100:.1f}% |",
        f"| L3 (コード知識必要) | {l3} | {l3/total*100:.1f}% |",
        f"| **合計** | **{total}** | 100% |",
        "",
        f"**可読率 (L1+L2): {readable_rate:.1f}%**",
        "",
    ]

    # 種別別集計
    by_type = defaultdict(lambda: Counter())
    for r in rows:
        by_type[r["type"]][r["level"]] += 1

    lines += [
        "## 2. 種別別可読性",
        "",
        "| 種別 | 件数 | L1 | L2 | L3 | 可読率 (L1+L2) |",
        "|---|---|---|---|---|---|",
    ]
    for rtype, cnt in sorted(by_type.items()):
        total_t = sum(cnt.values())
        l1_t = cnt.get("L1", 0)
        l2_t = cnt.get("L2", 0)
        l3_t = cnt.get("L3", 0)
        rate = (l1_t + l2_t) / total_t * 100 if total_t else 0
        lines.append(
            f"| {rtype} | {total_t} | {l1_t} | {l2_t} | {l3_t} | {rate:.1f}% |"
        )
    lines.append("")

    # TGT 別集計
    by_target = defaultdict(lambda: Counter())
    target_names = {}
    for r in rows:
        by_target[r["target_id"]][r["level"]] += 1
        target_names[r["target_id"]] = r["target_name"]

    lines += [
        "## 3. ターゲット別可読性",
        "",
        "| TGT | 対象 | 件数 | L1 | L2 | L3 | 可読率 |",
        "|---|---|---|---|---|---|---|",
    ]
    for tid in sorted(by_target.keys()):
        cnt = by_target[tid]
        total_t = sum(cnt.values())
        l1_t = cnt.get("L1", 0)
        l2_t = cnt.get("L2", 0)
        l3_t = cnt.get("L3", 0)
        rate = (l1_t + l2_t) / total_t * 100 if total_t else 0
        name = target_names.get(tid, "")[:30]
        lines.append(
            f"| {tid} | {name} | {total_t} | {l1_t} | {l2_t} | {l3_t} | {rate:.1f}% |"
        )
    lines.append("")

    # サンプル（各Levelから3件ずつ）
    lines += [
        "## 4. 分類結果サンプル",
        "",
    ]
    for level in ["L1", "L2", "L3"]:
        samples = [r for r in rows if r["level"] == level][:3]
        lines.append(f"### {level} の例")
        lines.append("")
        for r in samples:
            hits = ""
            if r["l3_hits"]:
                hits = f"（L3キーワード: {', '.join(r['l3_hits'][:3])}）"
            elif r["l2_hits"]:
                hits = f"（L2キーワード: {', '.join(r['l2_hits'][:3])}）"
            lines.append(f"- `{r['req_id']}` ({r['type']}): {r['description']} {hits}")
        lines.append("")

    # 比較基準
    lines += [
        "## 5. 参照値との比較",
        "",
        "| 対象 | 可読率 | 出典 |",
        "|---|---|---|",
        f"| {project_name} (本分析) | {readable_rate:.1f}% | 自動分類 |",
        "| sakura-editor (手動分類) | 65.7% | experiments/additional-analysis.md |",
        "| 目標: 成熟設計・ドメイン主導 | ≥ 75% | trm-evaluation-criteria.md §3.4 |",
        "",
        "## 6. 留意点",
        "",
        "- 本分類は **ルールベースのヒューリスティック**。LLM分類や手動分類との整合性検証は別途必要。",
        "- キーワード辞書は `scripts/classify_readability.py` の `L2_KEYWORDS` / `L3_KEYWORDS` で拡張可能。",
        "- 「誤分類」が疑われるサンプルは手動確認のうえ辞書を更新する運用を推奨。",
        "",
    ]
    return "\n".join(lines)


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--trm", required=True, type=Path, help="TRM YAML file")
    parser.add_argument("--output", required=True, type=Path,
                        help="Output Markdown report file")
    parser.add_argument("--csv", type=Path, help="Optional CSV output")
    args = parser.parse_args()

    trm = yaml.safe_load(args.trm.read_text(encoding="utf-8"))
    rows = classify_trm(trm)
    report = generate_report(rows, trm)

    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(report, encoding="utf-8")
    print(f"Report written: {args.output}")

    if args.csv:
        import csv
        with args.csv.open("w", encoding="utf-8", newline="") as f:
            writer = csv.DictWriter(
                f,
                fieldnames=["target_id", "target_name", "req_id", "type",
                            "priority", "level", "description"],
            )
            writer.writeheader()
            for r in rows:
                writer.writerow({k: r[k] for k in writer.fieldnames})
        print(f"CSV written: {args.csv}")

    # 標準出力に1行サマリ
    total = len(rows)
    if total:
        levels = Counter(r["level"] for r in rows)
        l1 = levels.get("L1", 0)
        l2 = levels.get("L2", 0)
        rate = (l1 + l2) / total * 100
        print(f"Summary: total={total} L1={l1} L2={l2} L3={levels.get('L3', 0)} "
              f"readable={rate:.1f}%")


if __name__ == "__main__":
    main()
