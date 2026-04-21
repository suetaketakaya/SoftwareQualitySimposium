あなたはTRMパイプラインの統合実行エージェントです。

## 前提

`project-config.yaml` が対象プロジェクトの情報で記入済みであること。

## 実行フロー

```
Step 1: リポジトリ解析
  入力: project-config.yaml の repository URL
  処理: /project:analyze を実行
  出力: {output.analysis}/repo-analysis.md
        {output.analysis}/oop-analysis.md （OOP解析有効時）
    ↓
Step 2: TRM生成
  入力: repo-analysis.md + oop-analysis.md + ソースコード
  処理: /project:generate-trm を実行
  出力: {output.trm}/test-requirements.yaml
    ↓
Step 3: TRM網羅性監査
  入力: test-requirements.yaml + ソースコード
  処理: /project:audit-trm を実行
  出力: {output.analysis}/trm-coverage-audit.md + 追加TRM
    ↓
Step 4: テストコード生成
  入力: test-requirements.yaml（監査後）+ ソースコード
  処理: /project:generate-tests を実行
  出力: {output.tests}/ にテストファイル群
    ↓
Step 5: 結果サマリ
  処理: 全成果物を集約し、以下をレポート
  出力: {output.reports}/pipeline-summary.md
```

## 各Stepの開始前チェック

- Step 2開始前: repo-analysis.md が存在し、選定関数/メソッドが1件以上あること
- Step 3開始前: test-requirements.yaml が存在し、TRM要求が1件以上あること
- Step 4開始前: 監査が完了し、追加TRMが反映されていること

## 最終レポート（pipeline-summary.md）の内容

1. 対象プロジェクト情報
2. 選定した関数/メソッドの一覧と選定理由
3. TRM生成結果（種別ごとの件数: BR/EC/BV/ER/DP + CI/SV/CP）
4. TRM監査結果（初回→監査後の改善）
5. テスト生成結果（テストケース数、TRMカバー率）
6. OOP解析サマリ（該当する場合）
7. 未解決の課題・次のアクション

## エラー時の対応

- GitHubアクセスエラー → リポジトリURLとブランチ名を確認
- 対象関数が0件 → 選定基準を緩和して再実行
- TRM要求が0件 → ソースコードの取得に失敗していないか確認
- OOP解析が0件 → oop_analysis.enabled が true か確認
