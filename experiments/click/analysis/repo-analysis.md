# pallets/click リポジトリ解析（types.py スコープ）

> **対象**: `pallets/click` の `src/click/types.py`（1209行、19クラス）
> **実施日**: 2026-04-21
> **スキーマ**: v3.1（CI/SV/CP + EN）
> **フォーカス**: `ParamType` 継承階層の代表5クラス

## 1. プロジェクト情報

| 項目 | 値 |
|---|---|
| プロジェクト名 | pallets/click |
| バージョン | main ブランチ HEAD（2026-04-17 時点） |
| 言語 | Python |
| テストFW | pytest |
| OOP解析 | 有効（CI/SV/CP/EN） |
| 解析対象ファイル | `src/click/types.py`（1,209行） |

## 2. クラス一覧（types.py内の全19クラス）

| クラス名 | 行 | 継承 | 役割 |
|---|---|---|---|
| `ParamType` | 30 | - | 基底クラス。`convert` の既定実装を持つ |
| `CompositeParamType` | 163 | ParamType | コンポジット型（Tuple等の基底） |
| `FuncParamType` | 171 | ParamType | 関数呼び出し型 |
| `UnprocessedParamType` | 195 | ParamType | 変換なしのパススルー |
| `StringParamType` | 207 | ParamType | 文字列変換 |
| `Choice` | 233 | ParamType, Generic[T] | 列挙型 |
| `DateTime` | 401 | ParamType | 日時解析 |
| `_NumberParamTypeBase` | 472 | ParamType | 数値型の中間基底 |
| `_NumberRangeBase` | 490 | _NumberParamTypeBase | 範囲制約付き数値 |
| `IntParamType` | 576 | _NumberParamTypeBase | int 変換 |
| **`IntRange`** | 584 | **_NumberRangeBase, IntParamType** | **ダイヤモンド継承** |
| `FloatParamType` | 610 | _NumberParamTypeBase | float 変換 |
| **`FloatRange`** | 618 | **_NumberRangeBase, FloatParamType** | **ダイヤモンド継承** |
| `BoolParamType` | 661 | ParamType | bool 変換 |
| `UUIDParameterType` | 730 | ParamType | UUID 変換 |
| `File` | 754 | ParamType | ファイルパス変換 |
| `Path` | 879 | ParamType | パス変換 |
| `Tuple` | 1060 | CompositeParamType | タプル変換 |
| `OptionHelpExtra` | 1205 | TypedDict | ヘルプ拡張情報（テスト対象外） |

## 3. 解析スコープの選定

`max_functions: 12` を意識しつつ、**以下5クラスを代表選定**:

| TGT-ID | クラス | 選定理由 |
|---|---|---|
| TGT-01 | `ParamType` (基底) | 継承契約の源。abstract-likeな`convert`の既定動作とprotocol定義 |
| TGT-02 | `StringParamType` | シンプルな派生。oracle として比較しやすい |
| TGT-03 | `Choice` | 状態持ちクラス（`choices`, `case_sensitive`）。SV/EN検証の中心 |
| TGT-04 | `_NumberParamTypeBase` | 中間抽象。`_number_class: ClassVar[type]` の継承戦略 |
| TGT-05 | `IntRange` | **ダイヤモンド継承** (_NumberRangeBase + IntParamType)。MROとCI検証の主役 |

**除外**: `File`, `Path`, `UUIDParameterType` はIO依存・外部リソース検証の色が強いため初回解析から除外。`DateTime` は要求数が嵩むため次回に回す。

## 4. 各クラスの詳細分析

### TGT-01: `ParamType` (line 30-160)

**責務**: すべてのパラメータ型の基底。文字列→型の変換protocol (`convert`) を定義し、名前・splitter・arity などのクラス属性を提供する。

**メンバ（ClassVar）**:
| name | type | visibility | 初期値 | 変更可能 |
|---|---|---|---|---|
| `is_composite` | `ClassVar[bool]` | public | `False` | 派生で上書き |
| `arity` | `ClassVar[int]` | public | `1` | 派生で上書き |
| `name` | `str`（宣言のみ） | public | なし | 派生で必須設定 |
| `envvar_list_splitter` | `ClassVar[str \| None]` | public | `None` | 派生で上書き |

**メソッド**:
- `__call__(value, param, ctx)` — value 非None時に convert を呼ぶ（多態性ディスパッチ）
- `to_info_dict()` — `type(self).__name__` ベースの情報辞書生成
- `convert(value, param, ctx)` — 既定で value をそのまま返す（派生で上書き期待）
- `get_metavar`, `get_missing_message` — 既定でNone返し
- `split_envvar_value(rv)` — envvar を splitter で分割
- `fail(message, param, ctx)` — BadParameter を投げる
- `shell_complete` — 既定で空リスト

**特徴**:
- 明示的ABCではないが、`convert` は派生での上書きが前提の「実質抽象」メソッド
- `name` 属性は docstring で「派生で必ず設定」と明記
- `to_info_dict` は class name に `"ParamType"` / `"ParameterType"` suffix を含む前提

### TGT-02: `StringParamType(ParamType)` (line 207-230)

**責務**: 文字列型の変換。bytesを適切なエンコーディングでdecodeし、他はstrで囲む。

**メンバ**:
- `name = "text"` (ClassVar, 派生固有)

**メソッド**:
- `convert` — bytes処理分岐（argv encoding → fs encoding → utf-8 replace の3段フォールバック）
- `__repr__` — `"STRING"` を返す

**分岐**: `isinstance(value, bytes)` → 4段ネスト（enc, fs_enc, Unicode成否）

### TGT-03: `Choice(ParamType, Generic[ParamTypeValue])` (line 233-398)

**責務**: 候補値の集合に対して入力を検証・正規化して、元のchoiceを返す列挙型。

**メンバ（インスタンス変数）**:
| name | type | 初期化 | 必須 | 不変性 |
|---|---|---|---|---|
| `choices` | `Sequence[T]` | `__init__` で `tuple(choices)` | ✓ | 宣言は明示無し（実装契約でfrozen想定） |
| `case_sensitive` | `bool` | `__init__` でデフォルト True | optional | 同上 |

**メソッド**:
- `__init__(choices, case_sensitive=True)` — choices をtuple化、case_sensitiveをそのまま格納
- `to_info_dict()` — super呼び出し + choices/case_sensitive 追加（super_delegation例）
- `_normalized_mapping(ctx)` — プライベート: choice → normalized 辞書
- `normalize_choice(choice, ctx)` — Enum/str分岐、token_normalize_func適用、casefold
- `get_metavar(param, ctx)` — option/argument 分岐で括弧形式切替
- `get_missing_message(param, ctx)` — gettext翻訳
- `convert(value, param, ctx)` — normalize → mapping検索 → StopIteration時 fail
- `get_invalid_choice_message` — 単数/複数形 ngettext
- `__repr__` / `shell_complete` — 出力系

**状態**: 外部から `self.choices`, `self.case_sensitive` を**直接変更可能**（private化・frozen化されていない）

### TGT-04: `_NumberParamTypeBase(ParamType)` (line 472-488)

**責務**: int/float 変換の共通処理。`_number_class` を派生で指定する戦略パターン。

**メンバ**:
- `_number_class: ClassVar[type[t.Any]]` — **宣言のみ**。派生で必ず設定されるべき

**メソッド**:
- `convert` — `self._number_class(value)` を呼び、ValueErrorで `self.fail`

**特徴**:
- `_number_class` は派生で未設定のとき `AttributeError` になる暗黙抽象
- `name` も派生必須

### TGT-05: `IntRange(_NumberRangeBase, IntParamType)` (line 584-607)

**責務**: int の範囲制約。min/max境界、open/closed、clamp を扱う。

**ダイヤモンド継承**:
```
         ParamType
            │
    _NumberParamTypeBase
        /         \
_NumberRangeBase  IntParamType
        \         /
         IntRange
```

**MRO (Python)**: `IntRange → _NumberRangeBase → IntParamType → _NumberParamTypeBase → ParamType → object`

**継承された状態**（`_NumberRangeBase.__init__` 由来）:
| name | type | 初期値 | 不変性 |
|---|---|---|---|
| `min` | `float \| None` | None | 宣言無し |
| `max` | `float \| None` | None | 宣言無し |
| `min_open` | `bool` | False | 宣言無し |
| `max_open` | `bool` | False | 宣言無し |
| `clamp` | `bool` | False | 宣言無し |

**継承された ClassVar**:
- `name = "integer range"` (IntRange で上書き)
- `_number_class = int` (IntParamType から継承)

**オーバーライド**:
- `_clamp(bound, dir, open)` — abstract 相当（`_NumberRangeBase` では `raise NotImplementedError`）
- `convert` は `_NumberRangeBase.convert` が使われる（MRO最左）

## 5. OOP構造の観察

### 5.1 継承階層の深さ

| クラス | depth |
|---|---|
| ParamType | 0 |
| _NumberParamTypeBase | 1 |
| IntParamType, StringParamType, Choice | 1 or 2 |
| _NumberRangeBase | 2 |
| IntRange, FloatRange | 3（ダイヤモンド） |

### 5.2 多態性ディスパッチ箇所

1. `ParamType.__call__` → `self.convert` (派生で上書き)
2. `ParamType.to_info_dict` → `type(self).__name__` 経由
3. `Choice.to_info_dict` → `super().to_info_dict()` 呼び出し
4. `_NumberRangeBase.convert` → `super().convert(...)` で `_NumberParamTypeBase.convert` を呼ぶ
5. `FloatRange.__init__` → `super().__init__(...)` で `_NumberRangeBase.__init__`

### 5.3 インスタンス変数の初期化パターン

- **Choice**: `__init__` で tuple化して格納 — 入力が iterable であれば tuple の不変性で部分的に保護
- **_NumberRangeBase**: `__init__` で直接代入 — **不変性契約なし**
- **_NumberParamTypeBase, StringParamType, IntParamType, BoolParamType**: `__init__` なし（ClassVar のみ）

### 5.4 カプセル化の観察（v3.1 追加）

| クラス | field | declared_visibility | declared_mutability | 外部からの変更可能性 | リスク |
|---|---|---|---|---|---|
| Choice | `choices` | public | なし（tuple格納で部分的） | 直接代入は防げない | medium: 再代入で state 破壊 |
| Choice | `case_sensitive` | public | なし | 直接代入可能 | medium: 生存期間中の変更で動作変化 |
| _NumberRangeBase | `min` | public | なし | 直接代入可能 | high: 範囲検証中の変更は未定義動作 |
| _NumberRangeBase | `max` | public | なし | 同上 | high |
| _NumberRangeBase | `min_open/max_open/clamp` | public | なし | 同上 | medium |
| ParamType | `name` | public ClassVar | なし | 派生で上書きは必須 | low: 派生側の責任 |
| _NumberParamTypeBase | `_number_class` | "protected" (_prefix) | ClassVar | 派生で上書き | low: _ prefix で慣例的保護 |

**検出されるカプセル化リスク:**
1. `Choice.choices` — 不変宣言なし、後から再代入可能（`unintended_mutability`）
2. `_NumberRangeBase.min/max` — 構築後の変更が behavior に影響する可能性（`external_mutation`）
3. `ParamType.name` — 派生で設定忘れで AttributeError（`missing_validation`）
4. `_NumberParamTypeBase._number_class` — 派生未設定で AttributeError（`missing_validation`）

### 5.5 検出されるデザインパターン・イディオム

| パターン | 箇所 | 説明 |
|---|---|---|
| **Template Method** | ParamType.convert / 派生の上書き | 基底は既定動作、派生で特化 |
| **Strategy** | _NumberParamTypeBase._number_class | 派生で数値コンストラクタを差し替え |
| **Decorator pattern** | なし | 本ファイルでは使用なし |
| **Generic/型パラメータ** | Choice[ParamTypeValue] | TypeVar でchoice型を外部指定 |
| **Class-level抽象契約** | ParamType (非ABC) | `convert` の既定が "identity" だが派生上書き前提 |
| **Super delegation** | Choice.to_info_dict, FloatRange.__init__ | super呼び出しで基底処理に委譲 |

## 6. テスト環境情報

- 依存: `typing_extensions`（TYPE_CHECKING時）、`_compat`, `exceptions`, `utils`（プロジェクト内）
- 既存テスト: `tests/test_types.py` 等（比較対象として別途参照）
- テストFW: pytest
- 言語: Python 3.10+（`from __future__ import annotations`, `t.TypeVar` 構文）

## 7. 期待される要求数（更新見積）

v3.1 での追加（EN）を踏まえた見積（本解析5クラス分）:

| 種別 | TGT-01 | TGT-02 | TGT-03 | TGT-04 | TGT-05 | 計 |
|---|---|---|---|---|---|---|
| BR | 5 | 8 | 12 | 3 | 8 | 36 |
| EC | 3 | 3 | 5 | 2 | 5 | 18 |
| BV | 0 | 1 | 0 | 0 | 10 | 11 |
| ER | 2 | 1 | 3 | 1 | 2 | 9 |
| DP | 1 | 0 | 2 | 0 | 2 | 5 |
| CI | 4 | 2 | 4 | 2 | 6 | 18 |
| SV | 0 | 0 | 4 | 0 | 5 | 9 |
| CP | 2 | 1 | 2 | 2 | 2 | 9 |
| **EN** | **2** | **1** | **4** | **2** | **6** | **15** |
| **小計** | 19 | 17 | 36 | 12 | 46 | **130** |

sakura-editorの99件（関数4つ）と比較して、5クラスで130件程度。
