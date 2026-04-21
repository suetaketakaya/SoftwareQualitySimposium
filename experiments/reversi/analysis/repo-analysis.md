# リバーシサンプル リポジトリ解析

> **対象**: `experiments/reversi/reversi_move.py`
> **実施日**: 2026-04-21
> **エージェント仕様準拠**: `.claude/commands/analyze.md`
> **実験目的**: パイプライン出力と手書きTRM（survey-design.md §5.3）との一致度検証

## 1. プロジェクト情報

| 項目 | 値 |
|---|---|
| プロジェクト名 | reversi-sample |
| ファイル数 | 1 (reversi_move.py) |
| 言語 | Python |
| テストFW | pytest |
| OOP解析 | 無効（純粋関数のみ） |
| 総コード行数（reversi_move.py） | 80 |

## 2. 関数一覧

| # | 関数名 | 行範囲 | 行数 | 分岐数 | ループ数 | 入力 | 出力 | 純粋性 |
|---|---|---|---|---|---|---|---|---|
| 1 | `is_on_board` | 19-21 | 3 | 1 (compound) | 0 | x, y | bool | 純粋 |
| 2 | `is_empty` | 24-26 | 3 | 1 | 0 | board, x, y | bool | 準純粋（board読み） |
| 3 | `count_flippable` | 29-43 | 15 | 4 | 1 | board, x, y, dx, dy, me | int | 準純粋（board読み） |
| 4 | `is_valid_move` | 46-64 | 19 | 3 + for | 1 | board, x, y, me | bool | 準純粋（board読み） |
| 5 | `make_empty_board` | 67-69 | 3 | 0 | 0 | - | board | 純粋 |
| 6 | `make_initial_board` | 72-79 | 8 | 0 | 0 | - | board | 純粋 |

### 選定基準適用結果

- `max_functions: 4` — 上から4関数を選定
- `prefer_pure_functions: true` — 純粋関数優先
- `exclude_patterns: test_*` — 除外対象なし
- **選定関数**: `is_on_board`, `is_empty`, `count_flippable`, `is_valid_move`
- **除外関数**: `make_empty_board`, `make_initial_board`（テスト/デモ用ヘルパー、業務ロジックではない）

## 3. 各関数の詳細分析

### TGT-01: `is_on_board(x, y) -> bool`

**責務**: 座標 (x, y) が 8×8 盤面の範囲内かを判定する。

**分岐条件**:
- 複合条件 `0 <= x < BOARD_SIZE and 0 <= y < BOARD_SIZE`
  - 原子条件1: `0 <= x` (下限)
  - 原子条件2: `x < BOARD_SIZE` (上限)
  - 原子条件3: `0 <= y` (下限)
  - 原子条件4: `y < BOARD_SIZE` (上限)

**同値クラス**:
- x: {負数, 0〜7, 8以上}
- y: {負数, 0〜7, 8以上}

**境界値候補**:
- x ∈ {-1, 0, 7, 8}
- y ∈ {-1, 0, 7, 8}

**エラーパス**: なし（例外を投げない設計）

**依存**: `BOARD_SIZE` (定数)

---

### TGT-02: `is_empty(board, x, y) -> bool`

**責務**: 指定マスが空（値=0）かを判定する。

**分岐条件**:
- 単一比較 `board[x][y] == 0`

**同値クラス**:
- `board[x][y]`: {0 (空), 1 (自分), 2 (相手)}

**境界値候補**: なし（離散値のみ）

**エラーパス**:
- `board` が None や不正サイズの場合、`IndexError` 可能性（前提違反。本関数では検査しない）

**依存**: なし

---

### TGT-03: `count_flippable(board, x, y, dx, dy, me) -> int`

**責務**: `(x, y)` から `(dx, dy)` 方向に、自分の石で挟める相手の石の数を返す。挟めなければ 0。

**分岐条件**:
1. 三項演算子 `opp = 2 if me == 1 else 1`
   - true分岐: me == 1 → opp = 2
   - false分岐: me != 1 → opp = 1
2. while ループ条件: `is_on_board(nx, ny) and board[nx][ny] == opp`
   - 原子条件1: `is_on_board(nx, ny)`
   - 原子条件2: `board[nx][ny] == opp`
3. 最終ガード: `count == 0 or not is_on_board(nx, ny) or board[nx][ny] != me`
   - 原子条件1: `count == 0` （1つも相手が連続しない）
   - 原子条件2: `not is_on_board(nx, ny)` （盤面端で途切れた）
   - 原子条件3: `board[nx][ny] != me` （自分の石で閉じていない）

**ループ**: while (1つ)

**同値クラス**:
- me: {1, 2}（仕様上想定。他値は未定義）
- dx, dy: {-1, 0, 1}（DIRECTIONS の各値）
- 相手の石の連続数: {0, 1, 2, 3〜(BOARD_SIZE-2)}

**境界値候補**:
- 相手連続数: 0, 1, 6（盤面最大連続数）
- 起点 (x, y) が盤端の場合の外向き方向
- `nx, ny` がループ途中で盤面を越える境界

**エラーパス**:
- `me ∉ {1, 2}` の場合、opp が意図しない値に（弱い仕様契約）
- `board` の形状不正時は `IndexError`（前提違反）

**依存**:
- `is_on_board` を呼ぶ

---

### TGT-04: `is_valid_move(board, x, y, me) -> bool`

**責務**: `(x, y)` に自分(me) の石を置けるかを判定する。

**分岐条件**:
1. 早期リターン1: `if not is_on_board(x, y): return False`
2. 早期リターン2: `if not is_empty(board, x, y): return False`
3. for ループ内の条件: `if count_flippable(...) > 0: return True`
4. 暗黙の最終リターン: `return False`（全方向で挟めなかった場合）

**ループ**: for (1つ, DIRECTIONS の 8方向)

**同値クラス**:
- x, y: {範囲外負, 範囲内, 範囲外正}
- me: {1, 2}
- 対象マス `board[x][y]`: {0 (空), 非0 (埋まっている)}
- 挟める方向の個数: {0, 1, 2, ..., 8}

**境界値候補**:
- x ∈ {-1, 0, 7, 8}
- y ∈ {-1, 0, 7, 8}
- 挟める方向: 0個と1個の境界、1個と2個の境界

**エラーパス**:
- 空盤面（全マス空）で True を返す位置がない
- 満盤（全マス埋まっている）で True を返す位置がない
- 範囲外 → 即 False

**依存**:
- `is_on_board` を呼ぶ
- `is_empty` を呼ぶ
- `count_flippable` を呼ぶ
- `DIRECTIONS` 定数を使う

**派生関数との関係**:
- count_flippable が 0 を返す方向は寄与しない
- 1方向でも count_flippable > 0 なら True
- → DP: `count_flippable` との結合テスト観点あり

## 4. OOP解析

`oop_analysis.enabled: false` により実施しない。

本サンプルには以下のOOP要素は一切存在しない:
- クラス定義なし
- インスタンス変数なし
- 継承なし
- デザインパターンなし
- マクロ・デコレータなし

よって CI / SV / CP の3種別は **全件0** となる。

## 5. 既存テストの参照（比較用）

本実験では `selection_criteria.require_existing_tests: false` に設定しているため、`test_reversi_move.py` は参照していない。手書きTRMおよび既存テストの存在はパイプライン生成に影響していない。

## 6. テスト困難度の評価

| 関数 | 困難度 | 理由 |
|---|---|---|
| is_on_board | 低 | 2変数の純粋関数、4境界のみ |
| is_empty | 低 | 単一比較の準純粋関数 |
| count_flippable | 中 | ループ + 最終ガード（3原子条件） |
| is_valid_move | 中 | 3早期リターン + ループ内呼び出し。盤面状態の組合せ多い |

## 7. 総括

- 対象4関数は純粋〜準純粋で、スタブ化不要
- 状態を持つオブジェクトはなく OOP拡張は非対象
- BR/EC/BV/ER/DP の5種別で設計可能
- 主要な複雑度は `count_flippable` の最終ガード（3原子条件） + `is_valid_move` のループ内分岐
