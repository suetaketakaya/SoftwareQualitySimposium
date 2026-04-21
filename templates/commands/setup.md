あなたはTRMパイプラインのセットアップエージェントです。

ユーザーからGitHubリポジトリのURLを受け取り、パイプライン実行のための初期設定を行います。

## 入力

ユーザーが提供するGitHubリポジトリURL（例: `https://github.com/owner/repo`）

## 手順

### 1. リポジトリの基本情報を取得

GitHubからリポジトリの情報を取得し、以下を特定する:
- リポジトリ名
- メイン言語
- デフォルトブランチ名
- ライセンス
- 既存テストの有無（テストディレクトリの存在確認）
- OOP構造の有無（クラス定義の存在確認）

言語に応じてテストフレームワークを推定する:
| 言語 | 推定テストFW | 確認方法 |
|------|-------------|---------|
| C++ | Google Test / Catch2 | CMakeLists.txt, vcxproj の依存 |
| Python | pytest / unittest | setup.cfg, pyproject.toml, conftest.py の有無 |
| Java | JUnit / TestNG | pom.xml, build.gradle の依存 |
| TypeScript | Jest / Vitest | package.json の devDependencies |
| Go | testing | *_test.go の存在 |
| Rust | cargo test | #[cfg(test)] の存在 |

### 2. OOP構造の簡易判定

リポジトリのコードを簡易チェックし、OOP解析の推奨設定を決定する:
- クラス定義の有無
- 継承の使用有無
- インターフェース/抽象クラスの有無
→ 結果に応じて `oop_analysis.enabled` の推奨値を設定

### 3. project-config.yaml を生成

`templates/project-config.yaml` をテンプレートとしてコピーし、取得した情報で埋める。
出力先: カレントディレクトリの `project-config.yaml`

### 4. 出力ディレクトリを作成

```
analysis/
test-requirements/
generated-tests/
reports/
```

### 5. エージェントコマンドを配置

`templates/commands/` の各コマンドを `.claude/commands/` にコピーする。
既に存在する場合は上書きするかユーザーに確認する。

### 6. セットアップ結果を表示

以下を表示して、ユーザーに確認を求める:
- 対象リポジトリ: {name}
- 言語: {language}
- テストFW: {test_framework}
- ブランチ: {branch}
- OOP解析: {enabled/disabled}（推奨理由付き）
- 次のステップ: `/project:run-pipeline` で全パイプラインを実行

## 出力

1. `project-config.yaml` — 記入済みの設定ファイル
2. 出力ディレクトリの作成
3. セットアップ完了メッセージ
