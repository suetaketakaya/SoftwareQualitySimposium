#!/usr/bin/env python3
"""SQiP 2026 提出用 docx 生成スクリプト (v3: 品質保証可視化フレーミング)"""

import os
import cairosvg
from docx import Document
from docx.shared import Pt, Cm, RGBColor
from docx.enum.text import WD_ALIGN_PARAGRAPH
from docx.enum.table import WD_TABLE_ALIGNMENT

BASE = "/Users/suetaketakaya/SoftwareQualitySymposium"
FIGURES = os.path.join(BASE, "drafts", "figures")
OUTPUT = os.path.join(BASE, "report", "submission_v3_2026.docx")

def convert_svgs():
    pngs = {}
    for name in ["fig1-overview", "fig2-requirements-breakdown",
                  "fig3-test-comparison", "fig4-execution-results"]:
        svg_path = os.path.join(FIGURES, f"{name}.svg")
        png_path = os.path.join(FIGURES, f"{name}.png")
        cairosvg.svg2png(url=svg_path, write_to=png_path, scale=2.0)
        pngs[name] = png_path
    return pngs

def add_heading(doc, text, level=2):
    h = doc.add_heading(text, level=level)
    for run in h.runs:
        run.font.color.rgb = RGBColor(0, 0, 0)
        run.font.size = Pt(12 if level == 2 else 11)
    h.paragraph_format.space_before = Pt(8)
    h.paragraph_format.space_after = Pt(4)
    return h

def add_para(doc, text, bold=False, size=10, align=None, space_after=Pt(3), indent=None):
    p = doc.add_paragraph()
    run = p.add_run(text)
    run.font.size = Pt(size)
    run.bold = bold
    run.font.name = "游明朝"
    if align:
        p.alignment = align
    p.paragraph_format.space_after = space_after
    p.paragraph_format.space_before = Pt(0)
    if indent:
        p.paragraph_format.first_line_indent = indent
    return p

def add_table(doc, headers, rows, col_widths=None, font_size=8.5):
    table = doc.add_table(rows=1 + len(rows), cols=len(headers))
    table.style = "Table Grid"
    table.alignment = WD_TABLE_ALIGNMENT.CENTER
    for i, h in enumerate(headers):
        cell = table.rows[0].cells[i]
        cell.text = h
        for p in cell.paragraphs:
            p.alignment = WD_ALIGN_PARAGRAPH.CENTER
            for run in p.runs:
                run.bold = True
                run.font.size = Pt(font_size)
    for r_idx, row in enumerate(rows):
        for c_idx, val in enumerate(row):
            cell = table.rows[r_idx + 1].cells[c_idx]
            cell.text = str(val)
            for p in cell.paragraphs:
                for run in p.runs:
                    run.font.size = Pt(font_size)
    if col_widths:
        for i, w in enumerate(col_widths):
            for row in table.rows:
                row.cells[i].width = Cm(w)
    return table

def add_figure(doc, png_path, caption, width=Cm(14)):
    p = doc.add_paragraph()
    p.alignment = WD_ALIGN_PARAGRAPH.CENTER
    run = p.add_run()
    run.add_picture(png_path, width=width)
    p.paragraph_format.space_after = Pt(1)
    cap = doc.add_paragraph(caption)
    cap.alignment = WD_ALIGN_PARAGRAPH.CENTER
    for run in cap.runs:
        run.font.size = Pt(8.5)
        run.italic = True
    cap.paragraph_format.space_after = Pt(4)
    return cap


def main():
    pngs = convert_svgs()
    doc = Document()

    section = doc.sections[0]
    section.page_width = Cm(21.0)
    section.page_height = Cm(29.7)
    section.top_margin = Cm(1.8)
    section.bottom_margin = Cm(1.8)
    section.left_margin = Cm(2.0)
    section.right_margin = Cm(2.0)

    style = doc.styles["Normal"]
    style.font.name = "游明朝"
    style.font.size = Pt(10)
    style.paragraph_format.space_after = Pt(3)
    style.paragraph_format.space_before = Pt(0)
    style.paragraph_format.line_spacing = Pt(14)

    # ======================================================================
    # PAGE 1: 発表申込書
    # ======================================================================
    add_para(doc, "ソフトウェア品質シンポジウム2026　「経験論文」「経験発表」", bold=True, size=11,
             align=WD_ALIGN_PARAGRAPH.CENTER)
    add_para(doc, "発表申込／アブストラクト　記入用紙", bold=True, size=10.5,
             align=WD_ALIGN_PARAGRAPH.CENTER, space_after=Pt(10))

    add_para(doc, "〔発表申込書〕", bold=True, size=10.5, space_after=Pt(6))

    add_para(doc, "タイトル", bold=True, size=9.5)
    add_para(doc, "テスト要求モデルによるLLM活用開発の品質保証可視化"
             "\n── コード非専門者が参加可能な単体テスト品質レビューの実現 ──", size=10)

    add_para(doc, "申込区分", bold=True, size=9.5)
    add_para(doc, "経験発表", size=10)

    add_para(doc, "カテゴリ", bold=True, size=9.5)
    add_para(doc, "・品質管理・テスト技術の観点", size=10)

    add_para(doc, "キーワード", bold=True, size=9.5)
    add_para(doc, "LLM活用開発 / 品質保証の可視化 / テスト要求モデル / "
             "単体テスト / トレーサビリティ / コード非専門者", size=10,
             space_after=Pt(10))

    # ======================================================================
    # PAGE 2-3: アブストラクト記入用紙
    # ======================================================================
    doc.add_page_break()
    add_para(doc, "〔アブストラクト記入用紙〕", bold=True, size=10.5,
             align=WD_ALIGN_PARAGRAPH.CENTER, space_after=Pt(6))

    # ------------------------------------------------------------------
    # 1. ねらい
    # ------------------------------------------------------------------
    add_heading(doc, "1. ねらい")

    add_para(doc,
        "大規模言語モデル（LLM）の発展により、荒い要求仕様からLLMに実装コードを直接生成させる"
        "開発スタイルが広がりつつある。この開発スタイルでは、品質の評価が「要求から現れる基本機能の"
        "動作確認」に偏り、単体テスト・コンポーネントテストのレイヤーが構造的に軽視される傾向にある。"
        "しかし、LLMが生成したコードを実リリースに載せるためには、単体テストによる品質担保、"
        "テスト網羅性の把握、静的テスト結果の確認といった品質情報が不可欠である。")

    add_para(doc,
        "ここで浮上する本質的課題は、品質保証に必要な情報がコードの中に閉じ込められており、"
        "要求作成ロール（LLMに要求を書いてコードを生成させるプロダクトオーナーや"
        "ビジネスアナリスト等の非開発者を含む）がリリース判断に必要な品質情報に"
        "アクセスできないという点である。従来、開発者がコードを読んで暗黙的に把握していた"
        "品質情報を、コード非専門者が可読な形で提示する方法論が確立されていない。")

    add_para(doc,
        "本研究は、テスト要求モデル（YAML形式の構造化中間成果物）を品質保証の"
        "コミュニケーション基盤として導入し、この可読性ギャップを埋める手法を提案する。"
        "テスト要求モデルは、(1) コード非専門者でも「何がテストされているか」を確認できるレビュー可能性、"
        "(2) 要求からテストコードへの追跡関係を明示するトレーサビリティ、"
        "(3) 要求の言葉で網羅性を説明できるカバー率指標、"
        "(4) 既存テストの品質ギャップを種別ごとに可視化する機能を提供する。")

    add_para(doc,
        "LLMテスト生成の主要研究 [1]-[4] はいずれもテスト生成精度の向上を主眼としており、"
        "生成されたテストの品質情報を非開発者が理解・レビュー可能にする課題には取り組んでいない [5]。"
        "BDD/Specification by Example [6] は非技術者にも可読なテスト仕様という思想を共有するが、"
        "受入テスト層が対象であり、単体テスト層の構造的品質情報の可視化は対象外である。"
        "本研究の新規性は、テスト要求モデルを品質保証のコミュニケーション基盤として位置づけ、"
        "単体テスト層の品質情報をコード非専門者に可視化する点にある。",
        size=9.5)

    # ------------------------------------------------------------------
    # 2. 実施概要
    # ------------------------------------------------------------------
    add_heading(doc, "2. 実施概要")

    add_para(doc,
        "提案手法は3つのフェーズで構成される（図1）。"
        "Phase 1（リポジトリ解析）ではLLMがソースコード構造を解析しテスト対象を選定する。"
        "Phase 2（テスト要求モデル生成）では分岐条件を抽出し、"
        "5種別でテスト要求をYAML形式で定義する。"
        "Phase 3（テストコード生成）では承認されたテスト要求モデルからテストコードを生成する。"
        "各フェーズでLLMが主導し、人間がレビュー・承認する。")

    # 5種別の表
    add_table(doc,
        ["種別", "記号", "コード非専門者向けの説明"],
        [
            ["分岐網羅", "BR", "コード内の全判断分岐が正しく動作するかの確認"],
            ["同値クラス", "EC", "入力データの各カテゴリに正しく処理されるかの確認"],
            ["境界値", "BV", "入力の上限・下限・切り替わり点での動作確認"],
            ["エラーパス", "ER", "異常な入力に対して安全に処理されるかの確認"],
            ["依存切替", "DP", "関連する機能間で整合性があるかの確認"],
        ],
        col_widths=[2.2, 1.0, 10.0], font_size=8)

    add_para(doc,
        "各テスト要求にはID・自然言語の説明・優先度・ソースコード参照を付与する。"
        "対象が純粋関数であるため状態遷移テスト等は不要であり、上記5種別で網羅的に分類できる。",
        size=9.5)

    # 図1
    add_figure(doc, pngs["fig1-overview"], "図1: 提案手法の全体構成", width=Cm(14))

    add_para(doc,
        "OSSのC++プロジェクト（Windows向け日本語テキストエディタ、"
        "Google Test整備済み）に適用した。3領域8関数（約385行）を選定し、"
        "(A) 既存テスト（約45件）、"
        "(B) LLM直接生成（先行研究 [5][7] の知見に基づく定性的評価）、"
        "(C) 提案方式の3方式を比較した。", size=9.5)

    # ------------------------------------------------------------------
    # 3. 実施結果
    # ------------------------------------------------------------------
    add_heading(doc, "3. 実施結果")

    add_para(doc,
        "8関数に対し計99件のテスト要求を5種別で定義した（図2）。"
        "内訳はBR 55件、EC 27件、BV 11件、ER 3件、DP 3件であった。"
        "これは「約385行のC++コードを読む」行為を「99件のYAMLエントリをレビューする」"
        "行為に変換したことを意味する（圧縮率: 約3.9行/要求）。"
        "99件のうち65件（65.7%）はコード知識なしで理解可能であり、"
        "残り34件はコード構造に依存する記述を含む。"
        "可読率は対象ドメインにより異なり、日時書式変換78.3%から文字種判定41.2%まで幅がある。",
        size=9.5)

    # 図2
    add_figure(doc, pngs["fig2-requirements-breakdown"],
               "図2: テスト要求の種別内訳（N=99）", width=Cm(9))

    add_para(doc,
        "テスト要求モデルを入力として145件のテストコードを生成し、"
        "テスト要求カバー率100%を達成した（図3）。"
        "この指標の本質的価値は、コードを参照せずに品質を説明可能になった点にある。"
        "「99件の品質要求が全て検証済みで全てPASS」とコード非依存に回答できる。",
        size=9.5)

    # 図3
    add_figure(doc, pngs["fig3-test-comparison"],
               "図3: 既存テストと生成テストの比較", width=Cm(11))

    add_para(doc,
        "macOS環境でコンパイル・実行した結果、初回133件PASS（91.7%）、12件FAIL。"
        "FAILは全てLLMのテスト期待値推論誤りであり対象関数のバグは0件（図4）。"
        "テスト要求モデルに照らして12件を5分類に即時診断し、修正後に全145件がPASSした。"
        "12件中8件（67%）はコード非専門者でも障害の性質を把握可能であった。"
        "テスト要求モデルは障害診断を「コード読解」から「要求照合」に変換した。",
        size=9.5)

    # 図4
    add_figure(doc, pngs["fig4-execution-results"],
               "図4: コンパイル・実行結果", width=Cm(11))

    add_para(doc,
        "既存テスト（約45件）との照合により、全要求の約62%が既存テスト未カバーであることを発見した。"
        "種別ごとの未カバー率はDP 100%、BV 73%、EC 70%、ER 67%、BR 53%であり、"
        "特にDP（関数間の整合性テスト）は完全に欠落していた。"
        "テスト要求モデルは「何がテストされていないか」を種別ごとに構造化して可視化する。",
        size=9.5)

    # 三方式比較表
    add_para(doc, "表: 三方式の品質情報可視性比較", bold=True, size=8.5,
             align=WD_ALIGN_PARAGRAPH.CENTER, space_after=Pt(1))
    add_table(doc,
        ["評価軸", "(A) 既存テスト", "(B) LLM直接(*)", "(C) 提案方式"],
        [
            ["品質情報の所在", "コード内（暗黙的）", "コード内（暗黙的）", "YAML（明示的）"],
            ["非専門者の参加", "不可", "不可", "可能（65.7%可読）"],
            ["網羅性の説明", "カバレッジ%", "不明", "要求カバー率"],
            ["ギャップ発見", "コード比較が必要", "困難", "種別ごとに発見"],
            ["障害診断", "コード読解", "コード読解", "要求IDで即時分類"],
            ["トレーサビリティ", "なし〜コメント", "なし", "双方向追跡"],
        ],
        col_widths=[2.8, 3.2, 3.2, 3.8], font_size=8)
    add_para(doc,
        "(*) 方式(B)は先行研究 [5][7] の知見に基づく定性的評価。(A)(C)は実測値。",
        size=8, space_after=Pt(4))

    # ------------------------------------------------------------------
    # 4. 結論
    # ------------------------------------------------------------------
    add_heading(doc, "4. 結論")

    add_para(doc,
        "本研究では、LLM活用開発における「品質情報がコードに閉じ込められ、"
        "コード非専門者がアクセスできない」という課題に対し、"
        "テスト要求モデルを品質保証のコミュニケーション基盤として導入する手法を提案した。"
        "OSSのC++プロジェクト8関数（約385行）に適用し、"
        "(1) 品質情報を99件の構造化要求に集約しコード非専門者の65.7%が可読な形式で提示、"
        "(2) テスト要求カバー率100%によりコード非依存な品質説明を実現、"
        "(3) 12件のテスト失敗を要求レベルで即時診断、"
        "(4) 既存テストの品質ギャップを種別ごとに構造化して発見、"
        "という成果を得た。",
        size=9.5)

    add_para(doc,
        "テスト要求モデルは、リリース判断時の品質確認、障害の迅速な把握、"
        "テスト改善の優先順位決定において品質保証基盤として活用できる。"
        "5種別は言語非依存の汎用分類であり他プロジェクトへの適用可能性がある。"
        "ただし本実験は単一プロジェクトの純粋関数に限定されており、"
        "一般化可能性の検証、コードカバレッジとの相関分析、副作用関数への拡張、"
        "および最も重要な今後の課題として要求作成ロールによる実際のレビュー実験が必要である。",
        size=9.5)

    # 参考文献
    add_para(doc, "", size=4, space_after=Pt(1))
    add_para(doc, "参考文献", bold=True, size=9, space_after=Pt(2))
    refs = [
        '[1] C. Lemieux et al., "CodaMOSA: Escaping Coverage Plateaus in Test Generation with Pre-trained Large Language Models," ICSE 2023.',
        '[2] Y. Chen et al., "ChatUniTest: A Framework for LLM-Based Test Generation," FSE 2024.',
        '[3] M. Schafer et al., "An Empirical Evaluation of Using Large Language Models for Automated Unit Test Generation," IEEE TSE 2024.',
        '[4] J. A. Ramos et al., "CoverUp: Coverage-Guided LLM-Based Test Generation," arXiv:2403.16218, 2024.',
        '[5] B. Chu et al., "Large Language Models for Unit Test Generation: Achievements, Challenges, and the Road Ahead," arXiv:2511.21382, 2025.',
        '[6] G. Adzic, Specification by Example, Manning, 2011.',
        '[7] N. S. Mathews and M. Nagappan, "Design choices made by LLM-based test generators prevent them from finding bugs," arXiv:2412.14137, 2024.',
        '[8] AIST, "生成AI品質マネジメントガイドライン 第1版," Rev.1.0.0, 2025.',
    ]
    add_para(doc, "\n".join(refs), size=7.5, space_after=Pt(0))

    doc.save(OUTPUT)
    print(f"Saved: {OUTPUT}")

if __name__ == "__main__":
    main()
