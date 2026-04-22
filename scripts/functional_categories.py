"""functional_categories.py — ターゲット別の機能分類データ

テスト設計書の「機能ツリー」「機能分類」の考え方に沿って、
TRM の対象ごとに以下3層を定義:

  Z=0 目的・対象層: 責務、入力・出力
  Z=1 機能分類層:   業務的なグルーピング（大・中・小機能）
  Z=2 テスト観点層: 正常系/異常系/境界値/エラー処理/連携動作

絵文字＋日本語ラベルで非エンジニアに直感的に伝達する。
"""

# 英語 → 日本語の翻訳テーブル

RISK_TYPE_JA = {
    "missing_validation":      "検証漏れ",
    "leaky_getter":            "値の漏洩（読み）",
    "leaky_setter":            "無制限な書き換え",
    "unintended_mutability":   "意図せぬ変更",
    "external_mutation":       "外部からの変更",
    "invariant_breach":        "不変条件の破壊",
    "public_mutable_field":    "公開された可変項目",
}


VARIABLE_ROLE_JA = {
    # click IntRange
    "min":          "下限値",
    "max":          "上限値",
    "min_open":     "下限を含まない",
    "max_open":     "上限を含まない",
    "clamp":        "範囲外を丸めるか",
    # click Choice
    "choices":      "選択肢リスト",
    "case_sensitive": "大小文字区別",
    "name":         "型の名称",
    # click ParamType
    "is_composite":         "複合型フラグ",
    "arity":                "引数個数",
    "envvar_list_splitter": "環境変数の区切り文字",
    # click ClickException
    "exit_code":    "終了コード",
    "show_color":   "色付き表示",
    "message":      "エラーメッセージ",
    # click BadParameter
    "ctx":          "実行コンテキスト",
    "cmd":          "コマンド",
    "param":        "パラメータ情報",
    "param_hint":   "パラメータ名ヒント",
    "param_type":   "パラメータ種別",
    # click _NumberParamTypeBase
    "_number_class": "数値変換クラス",
}


# ターゲット別の機能分類データ

FUNCTIONAL_CATEGORIES = {

    # -----------------------------------------------------------
    # reversi は関数ごとに定義（4関数）
    # -----------------------------------------------------------
    "is_valid_move": {
        "project": "リバーシ",
        "purpose": "石を置けるかを判定",
        "inputs": [
            {"icon": "🗺️", "label": "盤面の状態"},
            {"icon": "📍", "label": "置く座標 (x, y)"},
            {"icon": "⚫", "label": "自分の色"},
        ],
        "functional": [
            {"icon": "🗺️", "label": "盤面範囲チェック"},
            {"icon": "⬜", "label": "マス状態の確認"},
            {"icon": "🎯", "label": "挟み判定"},
        ],
        "perspectives": [
            {"icon": "🟢", "label": "正常系\n(置ける)"},
            {"icon": "🔴", "label": "異常系\n(置けない)"},
            {"icon": "🟡", "label": "境界値\n(角・辺)"},
        ],
    },

    "is_on_board": {
        "project": "リバーシ",
        "purpose": "座標が盤面内かを判定",
        "inputs": [
            {"icon": "📍", "label": "座標 (x, y)"},
        ],
        "functional": [
            {"icon": "📏", "label": "下限チェック"},
            {"icon": "📏", "label": "上限チェック"},
        ],
        "perspectives": [
            {"icon": "🟢", "label": "正常系\n(範囲内)"},
            {"icon": "🔴", "label": "異常系\n(範囲外)"},
            {"icon": "🟡", "label": "境界値\n(0, 7)"},
        ],
    },

    "is_empty": {
        "project": "リバーシ",
        "purpose": "指定マスが空かを判定",
        "inputs": [
            {"icon": "🗺️", "label": "盤面"},
            {"icon": "📍", "label": "座標 (x, y)"},
        ],
        "functional": [
            {"icon": "⬜", "label": "マス値の確認"},
        ],
        "perspectives": [
            {"icon": "🟢", "label": "正常系\n(空)"},
            {"icon": "🔴", "label": "異常系\n(石あり)"},
        ],
    },

    "count_flippable": {
        "project": "リバーシ",
        "purpose": "指定方向に挟める相手石数を返す",
        "inputs": [
            {"icon": "🗺️", "label": "盤面"},
            {"icon": "📍", "label": "起点座標"},
            {"icon": "➡️", "label": "方向 (dx, dy)"},
            {"icon": "⚫", "label": "自分の色"},
        ],
        "functional": [
            {"icon": "🔍", "label": "相手石の連続検出"},
            {"icon": "🎯", "label": "挟み込み確認"},
            {"icon": "📏", "label": "盤端チェック"},
        ],
        "perspectives": [
            {"icon": "🟢", "label": "正常系\n(挟める)"},
            {"icon": "🔴", "label": "異常系\n(挟めない)"},
            {"icon": "🟡", "label": "境界値\n(盤端)"},
        ],
    },

    # -----------------------------------------------------------
    # sakura-editor の対象関数
    # -----------------------------------------------------------
    "BOOL IsMailAddress": {
        "project": "sakura-editor",
        "purpose": "文字列がメールアドレス形式かを判定",
        "inputs": [
            {"icon": "📝", "label": "テキスト文字列"},
            {"icon": "📍", "label": "検索開始位置"},
            {"icon": "📏", "label": "長さ出力先"},
        ],
        "functional": [
            {"icon": "📋", "label": "メール形式妥当性"},
            {"icon": "🔍", "label": "位置指定での抽出"},
            {"icon": "🛡️", "label": "安全性確保"},
        ],
        "perspectives": [
            {"icon": "🟢", "label": "正常系\n判定"},
            {"icon": "🔴", "label": "異常系\n判定"},
            {"icon": "🟡", "label": "境界値\n処理"},
            {"icon": "⚠️", "label": "エラー\n処理"},
        ],
    },

    "std::wstring GetDateTimeFormat": {
        "project": "sakura-editor",
        "purpose": "日時を指定書式で文字列化",
        "inputs": [
            {"icon": "📋", "label": "書式文字列"},
            {"icon": "📅", "label": "日時情報"},
        ],
        "functional": [
            {"icon": "🔣", "label": "書式指定子の解釈"},
            {"icon": "🔢", "label": "値の置換"},
            {"icon": "📝", "label": "リテラル出力"},
        ],
        "perspectives": [
            {"icon": "🟢", "label": "正常系\n(各指定子)"},
            {"icon": "🟡", "label": "境界値\n(0パディング)"},
            {"icon": "⚠️", "label": "エラー\n(不明指定子)"},
        ],
    },

    "UINT32 ParseVersion": {
        "project": "sakura-editor",
        "purpose": "バージョン文字列を数値にパース",
        "inputs": [
            {"icon": "📝", "label": "バージョン文字列"},
        ],
        "functional": [
            {"icon": "🔍", "label": "数値要素の抽出"},
            {"icon": "🏷️", "label": "修飾子の判別"},
            {"icon": "🔢", "label": "数値化・パック"},
        ],
        "perspectives": [
            {"icon": "🟢", "label": "正常系\n(標準形式)"},
            {"icon": "🟡", "label": "境界値\n(桁数境界)"},
            {"icon": "⚠️", "label": "エラー\n(空・不正)"},
        ],
    },

    "int CompareVersion": {
        "project": "sakura-editor",
        "purpose": "2つのバージョンを比較",
        "inputs": [
            {"icon": "📝", "label": "バージョンA"},
            {"icon": "📝", "label": "バージョンB"},
        ],
        "functional": [
            {"icon": "⚖️", "label": "数値比較"},
        ],
        "perspectives": [
            {"icon": "🟢", "label": "正常系\n(大小判定)"},
            {"icon": "🟡", "label": "境界値\n(同一)"},
        ],
    },

    "static ECharKind CWordParse::WhatKindOfTwoChars": {
        "project": "sakura-editor",
        "purpose": "2文字の文字種関係を判定",
        "inputs": [
            {"icon": "🔣", "label": "前文字の種別"},
            {"icon": "🔣", "label": "現文字の種別"},
        ],
        "functional": [
            {"icon": "🔍", "label": "同種判定"},
            {"icon": "🗾", "label": "日本語文字処理"},
            {"icon": "🔀", "label": "種別マッピング"},
        ],
        "perspectives": [
            {"icon": "🟢", "label": "正常系\n(同種)"},
            {"icon": "🔴", "label": "異常系\n(別種)"},
        ],
    },

    "static ECharKind CWordParse::WhatKindOfTwoChars4KW": {
        "project": "sakura-editor",
        "purpose": "キーワード用の文字種関係判定",
        "inputs": [
            {"icon": "🔣", "label": "前文字の種別"},
            {"icon": "🔣", "label": "現文字の種別"},
        ],
        "functional": [
            {"icon": "🔀", "label": "KW専用マッピング"},
            {"icon": "🔗", "label": "通常版との差異"},
        ],
        "perspectives": [
            {"icon": "🟢", "label": "正常系"},
            {"icon": "🔗", "label": "連携動作\n(通常版比較)"},
        ],
    },

    "void Convert_ZeneisuToHaneisu": {
        "project": "sakura-editor",
        "purpose": "全角英数字を半角に変換",
        "inputs": [
            {"icon": "📝", "label": "変換対象文字列"},
        ],
        "functional": [
            {"icon": "🔤", "label": "文字種別判定"},
            {"icon": "🔄", "label": "変換テーブル参照"},
        ],
        "perspectives": [
            {"icon": "🟢", "label": "正常系\n(英数字変換)"},
            {"icon": "🟡", "label": "境界値\n(範囲境界)"},
            {"icon": "🔴", "label": "異常系\n(対象外)"},
        ],
    },

    "void Convert_HaneisuToZeneisu": {
        "project": "sakura-editor",
        "purpose": "半角英数字を全角に変換",
        "inputs": [
            {"icon": "📝", "label": "変換対象文字列"},
        ],
        "functional": [
            {"icon": "🔤", "label": "文字種別判定"},
            {"icon": "🔄", "label": "変換テーブル参照"},
        ],
        "perspectives": [
            {"icon": "🟢", "label": "正常系"},
            {"icon": "🟡", "label": "境界値"},
            {"icon": "🔗", "label": "連携動作\n(往復変換)"},
        ],
    },

    # -----------------------------------------------------------
    # pallets/click の対象
    # -----------------------------------------------------------
    "IntRange": {
        "project": "pallets/click",
        "purpose": "CLI入力された数値を範囲制約で検証",
        "inputs": [
            {"icon": "📝", "label": "入力値"},
            {"icon": "⚙️", "label": "下限値"},
            {"icon": "⚙️", "label": "上限値"},
            {"icon": "🚩", "label": "境界含む\n(open)"},
            {"icon": "🔧", "label": "丸めモード\n(clamp)"},
        ],
        "functional": [
            {"icon": "🔢", "label": "数値変換"},
            {"icon": "📐", "label": "範囲妥当性"},
            {"icon": "🎛️", "label": "境界モード"},
            {"icon": "🔧", "label": "範囲外処理"},
        ],
        "perspectives": [
            {"icon": "🟢", "label": "正常系\n(受理)"},
            {"icon": "🔴", "label": "異常系\n(拒否)"},
            {"icon": "🟡", "label": "境界値"},
            {"icon": "⚠️", "label": "エラー\n処理"},
            {"icon": "⚙️", "label": "設定\n組合せ"},
        ],
    },

    "Choice": {
        "project": "pallets/click",
        "purpose": "候補値の集合に対する入力検証",
        "inputs": [
            {"icon": "📝", "label": "入力文字列"},
            {"icon": "📋", "label": "選択肢リスト"},
            {"icon": "🚩", "label": "大小文字区別"},
        ],
        "functional": [
            {"icon": "🔄", "label": "値の正規化"},
            {"icon": "🔍", "label": "候補との照合"},
            {"icon": "📢", "label": "メッセージ生成"},
        ],
        "perspectives": [
            {"icon": "🟢", "label": "正常系\n(一致)"},
            {"icon": "🔴", "label": "異常系\n(候補外)"},
            {"icon": "⚠️", "label": "エラー\n処理"},
            {"icon": "⚙️", "label": "設定\n組合せ"},
        ],
    },

    "ParamType": {
        "project": "pallets/click",
        "purpose": "パラメータ型の基底 (共通プロトコル)",
        "inputs": [
            {"icon": "📝", "label": "任意の値"},
            {"icon": "⚙️", "label": "コンテキスト"},
        ],
        "functional": [
            {"icon": "🔄", "label": "型変換プロトコル"},
            {"icon": "📝", "label": "情報辞書生成"},
            {"icon": "⚠️", "label": "失敗通知"},
        ],
        "perspectives": [
            {"icon": "🟢", "label": "正常系"},
            {"icon": "🔗", "label": "連携動作\n(派生との)"},
            {"icon": "⚙️", "label": "継承\n構造"},
        ],
    },

    "StringParamType": {
        "project": "pallets/click",
        "purpose": "文字列変換（bytes/str 両対応）",
        "inputs": [
            {"icon": "📝", "label": "入力値"},
        ],
        "functional": [
            {"icon": "🔄", "label": "バイト列デコード"},
            {"icon": "🛡️", "label": "フォールバック"},
        ],
        "perspectives": [
            {"icon": "🟢", "label": "正常系"},
            {"icon": "⚠️", "label": "エラー処理\n(多段)"},
        ],
    },

    "_NumberParamTypeBase": {
        "project": "pallets/click",
        "purpose": "数値型変換の共通基盤",
        "inputs": [
            {"icon": "📝", "label": "入力値"},
            {"icon": "⚙️", "label": "数値クラス"},
        ],
        "functional": [
            {"icon": "🔢", "label": "型変換試行"},
            {"icon": "⚠️", "label": "失敗通知"},
        ],
        "perspectives": [
            {"icon": "🟢", "label": "正常系"},
            {"icon": "⚠️", "label": "エラー処理"},
            {"icon": "⚙️", "label": "派生構造"},
        ],
    },

    "ClickException": {
        "project": "pallets/click",
        "purpose": "Clickが処理する例外の基底",
        "inputs": [
            {"icon": "📝", "label": "エラーメッセージ"},
        ],
        "functional": [
            {"icon": "📢", "label": "メッセージ整形"},
            {"icon": "📤", "label": "stderr出力"},
        ],
        "perspectives": [
            {"icon": "🟢", "label": "正常系"},
            {"icon": "🔗", "label": "例外階層"},
        ],
    },

    "BadParameter, MissingParameter": {
        "project": "pallets/click",
        "purpose": "パラメータ不正の例外（3段継承）",
        "inputs": [
            {"icon": "📝", "label": "エラー内容"},
            {"icon": "⚙️", "label": "パラメータ情報"},
        ],
        "functional": [
            {"icon": "📢", "label": "詳細メッセージ生成"},
            {"icon": "🔗", "label": "3段継承チェーン"},
        ],
        "perspectives": [
            {"icon": "🟢", "label": "正常系"},
            {"icon": "🔗", "label": "階層関係"},
            {"icon": "⚙️", "label": "構築契約"},
        ],
    },

    "make_pass_decorator(object_type": {
        "project": "pallets/click",
        "purpose": "型付きオブジェクトを渡すデコレータ生成",
        "inputs": [
            {"icon": "⚙️", "label": "対象型"},
            {"icon": "🚩", "label": "自動生成フラグ"},
        ],
        "functional": [
            {"icon": "🏭", "label": "デコレータ生成"},
            {"icon": "🔍", "label": "オブジェクト探索"},
            {"icon": "📝", "label": "メタデータ転写"},
        ],
        "perspectives": [
            {"icon": "🟢", "label": "正常系"},
            {"icon": "⚠️", "label": "エラー\n(未構築)"},
        ],
    },
}


# ==============================================================
# 実行結果データ（プロジェクト単位）
# ==============================================================
# TRM に対してテストを実行した結果。各プロジェクトの
# evaluation-report.md / test 実行ログから転記。

EXECUTION_RESULTS = {
    "sakura-editor/sakura": {
        "total": 248,
        "pass": 239,
        "fail": 0,
        "skip": 9,
        "initial_pass_rate": 91.7,  # 初回PASS率
        "final_pass_rate": 100.0,   # 修正後PASS率
        "readability_rate": 65.7,   # 可読率(L1+L2)
        "status": "実行完了",
        "note": "初回 91.7% → 修正後 100% PASS",
        "suites": [
            ("Format",     103, 4, 107),
            ("CWordParse", 87,  1, 88),
            ("Convert",    49,  4, 53),
        ],
    },
    "reversi-sample": {
        "total": 9,
        "pass": 9,
        "fail": 0,
        "skip": 0,
        "initial_pass_rate": 100.0,
        "final_pass_rate": 100.0,
        "readability_rate": 97.8,
        "status": "実行完了",
        "note": "全件 PASS、survey 題材として完成",
        "suites": [
            ("reversi_move", 9, 0, 9),
        ],
    },
    "pallets/click": {
        "total": None,
        "pass": None,
        "fail": None,
        "skip": None,
        "initial_pass_rate": None,
        "final_pass_rate": None,
        "readability_rate": 24.3,  # 自動分類のみ
        "status": "未実行",
        "note": "TRM (173要求) 生成済、テスト実装は次段階",
        "suites": [],
    },
}


def get_execution_results(project_name: str) -> dict:
    """プロジェクト名から実行結果を取得。部分一致フォールバック付き"""
    if project_name in EXECUTION_RESULTS:
        return EXECUTION_RESULTS[project_name]
    for key, val in EXECUTION_RESULTS.items():
        if project_name in key or key in project_name:
            return val
    return {
        "total": None, "pass": None, "fail": None, "skip": None,
        "status": "情報なし", "note": "", "suites": [],
    }


def get_categories(target_key: str) -> dict | None:
    """対象名から機能分類データを取得。部分一致フォールバック付き"""
    if target_key in FUNCTIONAL_CATEGORIES:
        return FUNCTIONAL_CATEGORIES[target_key]
    # 部分一致（スペースや記号の違いを許容）
    for key, val in FUNCTIONAL_CATEGORIES.items():
        if target_key in key or key in target_key:
            return val
    return None


def default_categories(target_name_str: str, purpose: str, req_count: int) -> dict:
    """機能分類データが定義されていない対象向けの汎用フォールバック"""
    return {
        "project": "",
        "purpose": purpose or f"{target_name_str} の処理",
        "inputs": [
            {"icon": "📝", "label": "入力"},
        ],
        "functional": [
            {"icon": "⚙️", "label": "主処理"},
        ],
        "perspectives": [
            {"icon": "🟢", "label": "正常系"},
            {"icon": "🔴", "label": "異常系"},
        ],
    }
