"""test_reversi_move.py — survey-design.md に記載した TRM と実装の整合を検証。

survey-design.md §5.3 で提示した全TRM例に対応するテストを用意する:
- BR-01: 範囲外は False
- BR-02: 空でないマスは False
- BR-03: 挟めない場合は False
- BR-04: 1方向でも挟めれば True
- BV-01: 四隅（角）で正しく判定
- BV-02: 辺で正しく判定
- EC-01: 自分/相手/空 の3状態網羅
- ER-01: 空盤面は全マス False
- DP-01: count_flippable が 0 の方向は寄与しない

pytest が無い環境でも動くよう assert ベースで記述し、__main__ から直接走らせる。
"""

import sys
from reversi_move import (
    BOARD_SIZE,
    count_flippable,
    is_empty,
    is_on_board,
    is_valid_move,
    make_empty_board,
    make_initial_board,
)


# ------------------------------------------------------------------
# 補助: コンパクトな盤面記法
# ------------------------------------------------------------------

def board_from(rows):
    """文字列リストから盤を作る。'.'=空, '1'=自分, '2'=相手"""
    chars = {'.': 0, '1': 1, '2': 2}
    return [[chars[c] for c in row] for row in rows]


# ------------------------------------------------------------------
# BR-01: 盤面の範囲外は False
# ------------------------------------------------------------------

def test_BR_01_off_board_returns_false():
    board = make_initial_board()
    for (x, y) in [(-1, 0), (0, -1), (BOARD_SIZE, 0), (0, BOARD_SIZE), (-1, -1), (8, 8)]:
        assert is_valid_move(board, x, y, 1) is False, f"off-board ({x},{y}) should be False"


# ------------------------------------------------------------------
# BR-02: 空でないマスは False
# ------------------------------------------------------------------

def test_BR_02_non_empty_returns_false():
    board = make_initial_board()
    # 初期配置の中央4マスはすべて埋まっている
    for (x, y) in [(3, 3), (3, 4), (4, 3), (4, 4)]:
        assert is_valid_move(board, x, y, 1) is False
        assert is_valid_move(board, x, y, 2) is False


# ------------------------------------------------------------------
# BR-03: どの方向でも挟めない場合は False
# ------------------------------------------------------------------

def test_BR_03_no_flippable_returns_false():
    board = make_initial_board()
    # 盤の隅は挟めない位置
    assert is_valid_move(board, 0, 0, 1) is False
    assert is_valid_move(board, 7, 7, 1) is False
    # 孤立した中央以外のマスも挟めない
    assert is_valid_move(board, 0, 7, 1) is False


# ------------------------------------------------------------------
# BR-04: 1方向でも挟めれば True
# ------------------------------------------------------------------

def test_BR_04_one_direction_flippable_returns_true():
    board = make_initial_board()
    # 初期配置で合法な4手: (2,3) (3,2) (4,5) (5,4)
    assert is_valid_move(board, 2, 3, 1) is True
    assert is_valid_move(board, 3, 2, 1) is True
    assert is_valid_move(board, 4, 5, 1) is True
    assert is_valid_move(board, 5, 4, 1) is True


# ------------------------------------------------------------------
# BV-01: 四隅（角）で正しく判定
# ------------------------------------------------------------------

def test_BV_01_corner_valid_when_flippable():
    # 左上隅に置くと右方向に挟める盤面
    board = board_from([
        '.21.....',
        '........',
        '........',
        '........',
        '........',
        '........',
        '........',
        '........',
    ])
    assert is_valid_move(board, 0, 0, 1) is True
    # すでに石があれば角でも False
    board[0][0] = 1
    assert is_valid_move(board, 0, 0, 1) is False


# ------------------------------------------------------------------
# BV-02: 辺で正しく判定
# ------------------------------------------------------------------

def test_BV_02_edge_valid_when_flippable():
    # 上辺の中央付近に置くと右へ挟める盤面
    board = board_from([
        '..21....',
        '........',
        '........',
        '........',
        '........',
        '........',
        '........',
        '........',
    ])
    assert is_valid_move(board, 0, 1, 1) is True


# ------------------------------------------------------------------
# EC-01: 自分/相手/空 の3状態網羅（対象マスの状態）
# ------------------------------------------------------------------

def test_EC_01_target_state_partitioning():
    board = make_initial_board()
    # 自分の石が置かれているマス
    assert is_valid_move(board, 3, 4, 1) is False
    # 相手の石が置かれているマス
    assert is_valid_move(board, 3, 3, 1) is False
    # 空マス（挟める位置）
    assert is_valid_move(board, 2, 3, 1) is True


# ------------------------------------------------------------------
# ER-01: 空盤面は全マス False
# ------------------------------------------------------------------

def test_ER_01_empty_board_all_false():
    board = make_empty_board()
    for x in range(BOARD_SIZE):
        for y in range(BOARD_SIZE):
            assert is_valid_move(board, x, y, 1) is False
            assert is_valid_move(board, x, y, 2) is False


# ------------------------------------------------------------------
# DP-01: count_flippable が 0 の方向は is_valid_move に寄与しない
# ------------------------------------------------------------------

def test_DP_01_zero_flippable_does_not_contribute():
    # 右方向だけ挟める盤面を作り、他方向がすべて 0 を返すことを確認
    board = board_from([
        '.21.....',
        '........',
        '........',
        '........',
        '........',
        '........',
        '........',
        '........',
    ])
    # 右方向は 1 以上
    assert count_flippable(board, 0, 0, 0, 1, 1) > 0
    # 他方向は 0
    for (dx, dy) in [(-1, 0), (0, -1), (-1, -1), (1, 0), (1, 1), (-1, 1), (1, -1)]:
        assert count_flippable(board, 0, 0, dx, dy, 1) == 0, f"direction ({dx},{dy}) should be 0"
    # 総合的には True
    assert is_valid_move(board, 0, 0, 1) is True


# ------------------------------------------------------------------
# 実行
# ------------------------------------------------------------------

def run_all():
    tests = [v for k, v in globals().items() if k.startswith("test_") and callable(v)]
    failed = 0
    for t in tests:
        try:
            t()
            print(f"PASS: {t.__name__}")
        except AssertionError as e:
            failed += 1
            print(f"FAIL: {t.__name__} — {e}")
    print(f"\n{len(tests) - failed}/{len(tests)} passed")
    return failed


if __name__ == "__main__":
    sys.exit(1 if run_all() > 0 else 0)
