# 可視化レイヤ — PDF 配布パッケージ

TRM (Test Requirement Model) から自動生成した **4種の技術図 + 機能分類ジオラマ** を PDF として閲覧できる一式です。

**構成（対象ごとに2ページ）**:
- ページ1: Sunburst / Sankey / Heatmap / Chord の4枠（技術者・レビュアー向け）
- ページ2: **機能分類ジオラマ 3D図**（非エンジニア向け、絵文字＋日本語のみ）

ジオラマは「目的・対象層 → 機能分類層 → テスト観点層」の3層構造で、テスト設計書の機能分類ツリーに沿った命名規則（業務的・機能的なラベル）を採用しています。

## PDF 一覧（更新: 機能分類ジオラマ追加）

| ファイル | 頁数 | サイズ | 用途 |
|---|---|---|---|
| **4types-showcase.pdf** | 8 | 699KB | **推奨: 3対象 × (4種図+ジオラマ) + 比較サマリ** |
| 4types-IntRange-sample.pdf | 3 | 310KB | click IntRange 単体（4種図+ジオラマ） |
| 4types-Choice-sample.pdf | 3 | 273KB | click Choice 単体 |
| click-visualizations.pdf | 17 | 1.5MB | click 全8ターゲット × 2頁 |
| sakura-visualizations.pdf | 17 | 1.5MB | sakura-editor 全8関数 × 2頁 |
| reversi-visualizations.pdf | 9 | 751KB | リバーシ全4関数 × 2頁 |

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

## 5種類の可視化と各性質

| 可視化 | 対象読者 | OOP対象 (v3.1) | 手続き型・関数型 (v1.0含) |
|---|---|---|---|
| Sunburst | エンジニア・PM | 要求の種別階層 | 同左 |
| Sankey | エンジニア・PM | 種別×優先度の流量 | 同左 |
| **Heatmap** | エンジニア・レビュアー | **フィールド × リスク種別** | **要求種別 × 優先度** (代替) |
| **Chord** | エンジニア・レビュアー | **状態変数 ↔ リスク** (線種=重大度) | **依存関係マップ** (calls/called_by) |
| **🆕 機能分類ジオラマ** | **非エンジニア・PM** | 3層構造で共通 | 3層構造で共通 |

### 機能分類ジオラマの3層構造

```
┌───────────────────────────────────┐
│ ③ テスト観点層 (奥)               │
│   🟢 正常系 / 🔴 異常系 / 🟡 境界値 /│
│   ⚠️ エラー処理 / 🔗 連携動作       │
└───────────────────────────────────┘
            ↑
┌───────────────────────────────────┐
│ ② 機能分類層 (中)                 │
│   業務的な処理のグルーピング       │
│   (例: 範囲妥当性・境界モード等)   │
└───────────────────────────────────┘
            ↑
┌───────────────────────────────────┐
│ ① 目的・対象層 (手前)             │
│   🎯 この関数の目的 + 入力パラメータ │
└───────────────────────────────────┘
```

- 3Dジオラマ形式（等角投影）で層の重なりを表現
- 各層のノードは絵文字＋日本語ラベルのみ（実装用語を完全排除）
- 層間は矢印で「どの入力がどの処理を経てどの観点で検証されるか」を連結
- テスト設計書の機能ツリーに沿う命名規則

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
