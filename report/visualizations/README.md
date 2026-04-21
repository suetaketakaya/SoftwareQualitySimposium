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

## 対象条件とフォールバック仕様

各可視化は対象の性質に応じて適切な描画を自動選択:

| 可視化 | OOP対象 (v3.1) | 手続き型・関数型 (v1.0 含む) |
|---|---|---|
| Sunburst | 要求の種別階層 | 同左 (種別は ID prefix で推定) |
| Sankey | 種別×優先度の流量 | 同左 |
| **Heatmap** | **フィールド × リスク種別** (state×risk) | **要求種別 × 優先度** (代替表示) |
| **Chord** | **状態変数 ↔ リスク** (線種=重大度) | **依存関係マップ** (calls/called_by/globals) |

### Heatmap のフォールバック内容

OOPでない対象では `要求種別 × 優先度` のマトリクスを表示:
- 行: BR/EC/BV/ER/DP 等の要求種別
- 列: high / medium / low
- セル: 件数（カラー濃度で表現）

### Chord のフォールバック内容

OOPでない対象では `依存関係マップ` を表示:
- 中央: 対象関数名（青ノード）
- 左上: `calls` (呼び出す関数、緑ノード、矢印→対象)
- 右下: `called_by` (呼ばれる元、オレンジ、矢印←対象)
- 下: `globals` (グローバル変数、紫角ノード)
- 依存が全く無い場合のみ「独立関数」表示

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
