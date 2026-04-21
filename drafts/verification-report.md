# 技術的整合性検証レポート

検証日: 2026-03-31
検証対象: SQiP 2026 論文全成果物（7ファイル）

---

## 1. 一致している項目

| 検証項目 | 結果 |
|---------|------|
| テスト要求総数 99件 | 全ファイルで一致（traceability-matrix, evaluation-report, additional-analysis, abstract-v4） |
| テストケース総数 145件 | 全ファイルで一致（macos-build-report: 61+53+31=145, evaluation-report, abstract-v4） |
| 初回PASS率 91.7%（133/145） | macos-build-report と abstract-v4 で一致 |
| FAIL 12件、対象関数バグ 0件 | 全ファイルで一致 |
| 種別内訳 BR55/EC27/BV11/ER3/DP3 | traceability-matrix, evaluation-report, additional-analysis, abstract-v4 全て一致 |
| 既存テスト未カバー率 61.6%（61/99） | additional-analysis と abstract-v4 で一致 |
| 種別別未カバー率（BR52.7%/EC70.4%/BV72.7%/ER66.7%/DP100%） | additional-analysis と abstract-v4 で一致 |
| 既存テスト約45件、生成テスト3.2倍 | evaluation-report と abstract-v4 で一致 |
| コード行数（約115+180+90=385行） | evaluation-report の関数別行数合計と abstract-v4 で一致 |
| 優先度別件数（high61/medium36/low2） | traceability-matrix と evaluation-report で一致 |
| 関数別テスト数（21/27/13/27/18/8/17/11/3） | macos-build-report と evaluation-report で一致 |
| 関数別FAIL数（ParseVersion6/IsMailAddress3/WhatKindOfTwoChars3） | macos-build-report と evaluation-report で一致 |

## 2. 不一致・矛盾がある項目

### 2.1 FAIL原因分類のTRM ID対応に不正確さ（abstract-v4 セクション3.4）

**問題**: abstract-v4 の原因分類表で「コンポーネント分割の誤解 5件」の対応TRM IDが「BR-20〜23」（4つ）となっている。

**実態**: macos-build-report によると失敗テストは AlphaModifier/BetaModifier/RcModifier/PlModifier/UnknownModifier の5件。evaluation-report 付録ではこれらは BR-11〜15 に対応。BR-20〜23 は PartialMatch テスト群であり、これらは PASS している。additional-analysis でも「BR-11〜15, BR-20〜23」と両方記載されている。

**修正案**: abstract-v4 のTRM IDを「BR-11〜15」に修正するか、「BR-11〜15（BR-20〜23と関連）」とする。

### 2.2 FAIL原因の「文字数カウントミス」のTRM ID対応が不正確（abstract-v4 セクション3.4）

**問題**: abstract-v4 で「文字数カウントミス 2件」の対応TRM IDが「EC-13,14」となっている。

**実態**: macos-build-report によると失敗テストは BR34_DomainDotSeparation と EC_MultipleDotsDomain。additional-analysis では BR-34 と EC-04-06 として分類。EC-13 は「標準的なメールアドレス」、EC-14 は「各種無効メールアドレス」であり、これらのテストは PASS している。

**修正案**: 「BR-34, EC-04-06（ドメイン構造のテスト）」等に修正。

### 2.3 FAIL原因の「数字グルーピング」のTRM ID対応が不正確（abstract-v4 セクション3.4）

**問題**: abstract-v4 で対応TRM IDが「EC-08」となっている。

**実態**: additional-analysis では EC-02-01、evaluation-report 付録では EC_DigitsOnly テストの要求IDは (EC-02-01)。EC-08 は「各修飾子付きバージョンの順序関係を検証する」であり別の要求。

**修正案**: 「EC-02-01（数字のみ同値クラス）」等に修正。ただし EC-02-01 は同値クラスの内部IDであり、テスト要求レベルのIDではないため、表現方法の検討が必要。

### 2.4 repo-analysis.md のコード行数表記と abstract-v4 の軽微な不一致

**問題**: repo-analysis.md では format.cpp を「約150行」と記載（3関数合計）。abstract-v4 では「約115行」。evaluation-report では個別に ~50+~60+~5=~115行。

**実態**: 150行はファイル全体の行数、115行は3関数のコード行数の可能性がある。矛盾ではなく粒度の違いだが、論文読者が混乱する可能性あり。

## 3. 過大主張のリスクがある記述

### 3.1 既存テスト比較の限定条件が結論部で薄い

abstract-v4 結論の「既存テスト45件の3.2倍を体系的に生成」は、evaluation-report セクション6.3で「既存テストは異なる目的で作成されており比較は公平でない可能性」と注記があるが、結論部（セクション4）ではこの留保なく記述されている。**結論部でも「既存テスト約45件と比較して」等の限定表現を入れることを推奨。**

### 3.2 「品質ギャップ発見」の表現

abstract-v4 結論の「既存テストの未カバー要求61件（61.6%）を種別ごとに特定」は正確だが、未カバー要求の特定は既存テストとの突合が**推定ベース**（additional-analysis セクション4.1 で「推定」と明記）であることが結論部では省略されている。

### 3.3 参考文献の範囲

abstract-v4 には参考文献が [1]〜[5] の5件のみ。本文中の引用は [1]〜[4] のみで整合している。検証項目の「[1]-[17]」は本アブストラクト版には該当しない（最終論文版で拡張予定と思われる）。現状では問題なし。

## 4. 修正すべき箇所のリスト

| # | ファイル | 箇所 | 内容 | 優先度 |
|---|---------|------|------|--------|
| 1 | abstract-v4.md | セクション3.4 原因分類表1行目 | TRM ID「BR-20〜23」→「BR-11〜15」に修正 | 高 |
| 2 | abstract-v4.md | セクション3.4 原因分類表3行目 | TRM ID「EC-13,14」→ 正確なID（BR-34等）に修正 | 高 |
| 3 | abstract-v4.md | セクション3.4 原因分類表2行目 | TRM ID「EC-08」→ 正確なID（同値クラス内部ID）に修正 | 中 |
| 4 | abstract-v4.md | セクション4 結論2 | 「既存テスト45件の3.2倍」に比較条件の留保を追加 | 低 |
| 5 | abstract-v4.md | セクション4 結論4 | 「61件（61.6%）を特定」に推定ベースである旨を追記 | 低 |

**総合評価**: 主要な定量データ（99件、145件、91.7%、12件、61.6%、種別内訳）はファイル間で高い整合性を持つ。修正が必要なのは abstract-v4 セクション3.4 のFAIL原因分類表におけるTRM ID対応の3箇所（具体的なID番号の誤り）が中心。論文の主張を覆すような重大な矛盾はない。
