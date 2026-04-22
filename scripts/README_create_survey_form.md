# create_survey_form.gs — 利用ガイド (v2: フル画像版)

`scripts/create_survey_form.gs` は、非エンジニア向けTRM可読性アンケートを Google Forms として自動生成する Google Apps Script です。

**v2 更新点**:
- 画像5枚を埋め込み可能にした（`SURVEY_IMAGES` オブジェクトで一元管理）
- 新セクション「題材C: 評価ダッシュボードの理解」追加（Q26-Q29）
- 補足資料として可読率比較グラフを埋込
- 既存 Q22-25 は Q30-33 に繰り下げ、**最終 33問 / 約 18分** 構成

## 使い方

### 1. スクリプトの配置

1. https://script.google.com にアクセス（Googleアカウント必須）
2. 「新しいプロジェクト」をクリック
3. 左ペインの `コード.gs` を削除し、代わりに `scripts/create_survey_form.gs` の内容を貼り付け
4. プロジェクト名を任意で設定（例: "TRM可読性アンケート生成"）

### 2. 画像5枚の Googleドライブ登録（フル版必須）

`report/survey-images/` 配下の5枚をアップロードして `SURVEY_IMAGES` に ID を設定:

| 画像ファイル | 設定キー | 使用箇所 |
|---|---|---|
| 01-reversi-code.png | `reversi_code` | 題材A 導入（Q7前） |
| 02-reversi-diorama.png | `reversi_diorama` | 題材A パターン3（Q13前） |
| 03-sakura-diorama.png | `sakura_diorama` | 題材B（Q19前） |
| 04-click-dashboard.png | `click_dashboard` | 題材C（Q26前） |
| 05-readability-comparison.png | `readability_comparison` | Q29後の補足資料 |

**手順**:
1. 各 PNG を Googleドライブにアップロード
2. 各ファイルで「共有」→「リンクを知っている全員」に設定
3. 共有URL（`https://drive.google.com/file/d/XXXXXX/view`）から `XXXXXX` の ID を抽出
4. スクリプト冒頭の `SURVEY_IMAGES` に各 ID を設定:
   ```javascript
   const SURVEY_IMAGES = {
     reversi_code:           "XXXXXXXX",
     reversi_diorama:        "YYYYYYYY",
     sakura_diorama:         "ZZZZZZZZ",
     click_dashboard:        "AAAAAAAA",
     readability_comparison: "BBBBBBBB",
   };
   ```
5. 未設定の画像は挿入がスキップされる（Form は生成される）

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

## Form構造（自動生成される内容・v2 フル版）

```
[0] 同意                    (1問)
[1] 属性質問                (5問)
[2] 前提知識確認            (1問)
[3] 題材A: リバーシ         (12問: コード + 3パターン + 比較 + 自由記述)
    └ 画像: reversi_code, reversi_diorama
[4] 題材B: sakura-editor    (3問)
    └ 画像: sakura_diorama
[5] 題材C: ダッシュボード   (4問: NEW Q26-Q29)
    └ 画像: click_dashboard, readability_comparison
[6] 総合評価                (4問: Q30-Q33)
```

計: **33問**、想定回答時間 15〜18分

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
