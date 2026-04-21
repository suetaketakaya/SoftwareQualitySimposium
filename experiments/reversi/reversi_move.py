"""reversi_move.py — リバーシの合法手判定。

非エンジニア向けアンケートの題材として、以下の制約で設計されている:
- 80行以下に収める
- 業務コードを模した可読な命名
- 例外やロギングは入れない（題材の複雑性を最小化）
- 盤面は 8x8、空=0 / 自分=1 / 相手=2
"""

BOARD_SIZE = 8

DIRECTIONS = [
    (-1, -1), (-1, 0), (-1, 1),
    ( 0, -1),          ( 0, 1),
    ( 1, -1), ( 1, 0), ( 1, 1),
]


def is_on_board(x: int, y: int) -> bool:
    """盤面の範囲内かを判定する"""
    return 0 <= x < BOARD_SIZE and 0 <= y < BOARD_SIZE


def is_empty(board, x: int, y: int) -> bool:
    """指定マスが空かを判定する"""
    return board[x][y] == 0


def count_flippable(board, x: int, y: int, dx: int, dy: int, me: int) -> int:
    """ (x,y) から (dx,dy) 方向に、自分の石で挟める相手の石の数を返す """
    opp = 2 if me == 1 else 1
    nx, ny = x + dx, y + dy
    count = 0

    while is_on_board(nx, ny) and board[nx][ny] == opp:
        count += 1
        nx += dx
        ny += dy

    if count == 0 or not is_on_board(nx, ny) or board[nx][ny] != me:
        return 0

    return count


def is_valid_move(board, x: int, y: int, me: int) -> bool:
    """(x,y) に自分(me)の石を置けるかを判定する。

    - 盤面の範囲外: False
    - すでに石がある: False
    - どの方向でも1つも挟めない: False
    - 1つでも挟める方向がある: True
    """
    if not is_on_board(x, y):
        return False

    if not is_empty(board, x, y):
        return False

    for dx, dy in DIRECTIONS:
        if count_flippable(board, x, y, dx, dy, me) > 0:
            return True

    return False


def make_empty_board():
    """全マス空の8x8盤を返す（テスト・デモ用）"""
    return [[0 for _ in range(BOARD_SIZE)] for _ in range(BOARD_SIZE)]


def make_initial_board():
    """リバーシ初期配置の盤を返す（テスト・デモ用）"""
    board = make_empty_board()
    board[3][3] = 2
    board[3][4] = 1
    board[4][3] = 1
    board[4][4] = 2
    return board
