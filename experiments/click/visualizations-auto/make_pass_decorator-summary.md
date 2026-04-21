# make_pass_decorator (TGT-08) — 可視化レイヤ（自動生成）

> **対象**: `make_pass_decorator(object_type: type[T], ensure: bool = False)`
> **責務**: 指定した型のオブジェクトをContextから探し、デコレート対象関数の第1引数として渡すデコレータを生成する
> **総要求数**: 13
> **種別内訳**: 🟦 分岐網羅 (BR) 4, 🟩 同値クラス (EC) 2, 🟥 エラーパス (ER) 2, 🟪 依存切替 (DP) 1, ⬛ コードパターン (CP) 4

---

## 1. トリガー階層（Sunburst / Mindmap）

```mermaid
mindmap
  root((make_pass_decorator))
    分岐網羅 (BR)
      BR-08-01: ensure=True で ensure_object が呼ばれ、なければ新規生
      BR-08-02: ensure=False で find_object が呼ばれ、なければ Non
      BR-08-03: obj が None のとき RuntimeError を投げる
      BR-08-04: obj が存在するとき ctx.invoke(f, obj, ...) が呼ばれ
    同値クラス (EC)
      EC-08-01: object_type が class / dataclass / Protoc
      EC-08-02: ensure True/False × obj 存在/非存在 の4組合せ
    エラーパス (ER)
      ER-08-01: context 外で呼び出したとき get_current_context が 
      ER-08-02: ensure=False かつ obj が None で RuntimeErro
    依存切替 (DP)
      DP-08-01: get_current_context → find_object/ensure
    コードパターン (CP)
      CP-08-01: Decorator Factory パターンが正しく機能し、異なる object
      CP-08-02: update_wrapper により元関数の __name__, __doc__
      CP-08-03: Closure で object_type / ensure を正しくキャプチャ
      CP-08-04: Click の Context lifecycle（get_current_co
```

## 2. 種別分布の流量（Sankey）

```mermaid
sankey-beta

make_pass_decorator,分岐網羅 (BR),4
make_pass_decorator,同値クラス (EC),2
make_pass_decorator,エラーパス (ER),2
make_pass_decorator,依存切替 (DP),1
make_pass_decorator,コードパターン (CP),4
分岐網羅 (BR),優先度:high,2
分岐網羅 (BR),優先度:medium,2
同値クラス (EC),優先度:high,1
同値クラス (EC),優先度:medium,1
エラーパス (ER),優先度:high,1
エラーパス (ER),優先度:medium,1
依存切替 (DP),優先度:high,1
コードパターン (CP),優先度:high,2
コードパターン (CP),優先度:medium,2
```

## 3. 複合影響のヒートマップ（field × risk）

> (state_variables または encapsulation_risks が空のためヒートマップ対象外)

## 4. トリガー相互関係（Chord 風 Flowchart）

> (state_variables が空のため Chord 生成不可)

---

## 自動生成のメタ情報

- ツール: `scripts/generate_visualizations.py`
- 入力スキーマ: TRM v3.1 (`templates/trm-schema.yaml`)
- 図解形式: Mermaid + Markdown
- 対象読者: 非エンジニア + 技術系PM + レビュアー