# create_survey_form.gs — 利用ガイド

`scripts/create_survey_form.gs` は、非エンジニア向けTRM可読性アンケートを Google Forms として自動生成する Google Apps Script です。

## 使い方

### 1. スクリプトの配置

1. https://script.google.com にアクセス（Googleアカウント必須）
2. 「新しいプロジェクト」をクリック
3. 左ペインの `コード.gs` を削除し、代わりに `scripts/create_survey_form.gs` の内容を貼り付け
4. プロジェクト名を任意で設定（例: "TRM可読性アンケート生成"）

### 2. 画像の埋め込み（任意、パターン3用）

パターン3（階層＋図解）で盤面ケースの図を表示する場合:

1. 盤面図を画像として用意（PNG/JPEG）
2. Googleドライブにアップロードし、共有設定を「リンクを知っている全員」に
3. 共有URLから ID を抽出（例: `https://drive.google.com/file/d/XXXXXX/view` → ID は `XXXXXX`）
4. スクリプト冒頭の `PATTERN_3_IMAGE_URL` を以下のように設定:
   ```javascript
   const PATTERN_3_IMAGE_URL = "https://drive.google.com/uc?id=XXXXXX";
   ```
5. 未設定の場合、画像なしで Form が作成されます（手動で後から挿入可）

### 3. 実行

1. 関数選択プルダウンから `createSurveyForm` を選択
2. 「実行」をクリック
3. 初回は権限承認ダイアログが表示される（Forms作成権限・ログ出力権限）
4. 実行ログ（表示 > 実行ログ）に以下が出力される:
   - `Published URL`: 回答者に配布するURL
   - `Edit URL`: フォーム編集用URL

### 4. 配布・運用

1. Published URL をアンケート対象者に配布
2. 回答はフォームの「回答」タブで確認できる
3. Googleスプレッドシートに連携すると一覧集計しやすい（フォーム右上メニューから設定）

## カスタマイズ箇所

| 変数名 | 用途 | デフォルト |
|---|---|---|
| `SAMPLE_REVERSI_CODE` | 題材Aのコード | `experiments/reversi/reversi_move.py` の要点を転記済 |
| `PATTERN_3_IMAGE_URL` | 盤面図解の画像URL | `""` (未設定) |

質問文を変更したい場合は、対応する `addSection_X_*` 関数内を編集してください。

## Form構造（自動生成される内容）

```
[0] 同意                  (1問)
[1] 属性質問              (5問: 職種/経験年数/コード頻度/単体テスト認知/立場)
[2] 前提知識確認          (1問)
[3] 題材A: リバーシ       (12問: コード理解 → 3パターン評価 → 比較 → 自由記述)
[4] 題材B: sakura-editor  (3問: 理解度/変化/形式選好)
[5] 総合評価              (4問: 有用性/利用意向/自由記述/連絡先)
```

計: 約25問、想定回答時間 12〜15分（`knowledge/survey-design.md` §1 の設計に準拠）

## トラブルシュート

| 症状 | 原因 | 対処 |
|---|---|---|
| 権限エラーで実行できない | Apps Script 権限未承認 | 初回実行時のダイアログで「許可」を選択 |
| 画像が表示されない | 共有設定が不十分 | Googleドライブで「リンクを知っている全員」に設定 |
| 日本語が文字化け | (通常は発生しない) | スクリプトファイルの文字コードが UTF-8 か確認 |
| 実行タイムアウト | Apps Scriptの6分制限 | 本スクリプトは30秒未満で完了するはず。リトライを推奨 |

## 関連ドキュメント

- `knowledge/survey-design.md` — アンケート本体の設計仕様
- `experiments/reversi/reversi_move.py` — 題材Aのコード本体（テスト済）
- `experiments/reversi/test_reversi_move.py` — 題材Aの振る舞い検証（9テストPASS）
