# 4種可視化レイヤ — PDF 配布パッケージ

TRM (Test Requirement Model) から自動生成した **Sunburst / Sankey / Heatmap / Chord** の4種類を PDF として閲覧できる一式です。

## PDF 一覧

| ファイル | ページ数 | サイズ | 用途 |
|---|---|---|---|
| **4types-showcase.pdf** | 5 | 133KB | **推奨: 論文・アンケート・レビュー用の代表例集** |
| 4types-IntRange-sample.pdf | 2 | 100KB | click IntRange 単体の4種類 (OOP 代表例) |
| 4types-Choice-sample.pdf | 2 | 96KB | click Choice 単体の4種類 (stateful OOP 例) |
| click-visualizations.pdf | 9 | 166KB | click 全8ターゲット × 4種類 |
| sakura-visualizations.pdf | 9 | 128KB | sakura-editor 全8関数 × 4種類 |
| reversi-visualizations.pdf | 5 | 101KB | リバーシ全4関数 × 4種類 |

## 推奨用途別の選択

| 用途 | 推奨PDF | 理由 |
|---|---|---|
| アンケートの「題材と図面の併記」に使う | **4types-showcase.pdf** | 3対象を一冊で比較可能 |
| 論文§3.4/§6.4 の図版として引用 | 4types-IntRange-sample.pdf | OOP対象で全4種が揃う最良例 |
| click 実証の全貌を見せたい | click-visualizations.pdf | 全8クラスの可視化網羅 |
| 可視化の対象間差を確認したい | 4types-showcase.pdf の §5 まとめページ | 有効/該当なしマトリクス |

## 生成コマンド

```bash
# 対象単体
python scripts/generate_visualization_pdf.py \
    --trm experiments/click/test-requirements/test-requirements.yaml \
    --output report/visualizations/click-visualizations.pdf

# 特定ターゲットのみ
python scripts/generate_visualization_pdf.py \
    --trm experiments/click/test-requirements/test-requirements.yaml \
    --output report/visualizations/4types-IntRange-sample.pdf \
    --target TGT-05

# 3対象ショーケース
python scripts/generate_visualization_showcase_pdf.py \
    --output report/visualizations/4types-showcase.pdf
```

## 文字化けしない仕組み

前版の Mermaid 方式はプレビュー環境のフォント依存で文字化けしたが、本PDFは以下により解決:

- **matplotlib で直接描画** (Mermaid を介さない)
- **TrueType フォント埋込** (`pdf.fonttype = 42`) で Hiragino Sans を PDF 内に埋め込む
- 再生成時も同じ環境（Hiragino Sans 利用可能）であれば文字化けなし

## 対象条件

| 可視化 | 必要なTRMセクション | 対応 |
|---|---|---|
| Sunburst | test_requirements (種別フィールド or ID prefix) | v1.0 / v3.1 両対応 |
| Sankey | 同上 + priority | v1.0 / v3.1 両対応 |
| Heatmap | **state_variables + encapsulation_risks** | v3.1 (OOPクラス) のみ |
| Chord | 同上 | v3.1 (OOPクラス) のみ |

手続き型・関数型対象では Heatmap / Chord は「該当なし」と明示表示する設計（欺瞞を避ける）。

## 論文への貼り付け

Word / Google Docs に PDF を画像として取り込む場合:

```bash
# PDF → PNG 変換 (各ページ個別)
pdftoppm -r 200 -png 4types-showcase.pdf page
# page-1.png, page-2.png, ... が生成される

# or Preview.app で PDF を開き、ページを右クリック→「別名で書き出す」→PNG
```

## アンケート埋込

Google Forms に画像として添付する場合の推奨:

1. Preview.app で `4types-IntRange-sample.pdf` の2ページ目を画像エクスポート (PNG/JPEG)
2. Google ドライブにアップロード
3. Google Forms の画像アイテムで挿入、共有リンクで埋込
