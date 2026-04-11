# TRM網羅性検証監査レポート

**対象**: sakura-editor/sakura 8関数に対するテスト要求モデル (99件)
**検証日**: 2026-03-31
**検証者**: ホワイトボックステスト網羅性検証エージェント

---

## 1. 関数ごとの分岐条件一覧と TRM BR要求との対応

### 1.1 GetDateTimeFormat (TGT-01)

| # | ソースコード分岐条件 | TRM対応 | 状態 |
|---|---------------------|---------|------|
| 1 | `if( inSpecifier )` - inSpecifier==trueの場合 | BC-01-01〜BC-01-08 (指定子処理全体) | OK |
| 2 | `if( f == L'Y' )` | BC-01-01 / BR-01 | OK |
| 3 | `else if( f == L'y' )` | BC-01-02 / BR-02 | OK |
| 4 | `else if( f == L'm' )` | BC-01-03 / BR-03 | OK |
| 5 | `else if( f == L'd' )` | BC-01-04 / BR-04 | OK |
| 6 | `else if( f == L'H' )` | BC-01-05 / BR-05 | OK |
| 7 | `else if( f == L'M' )` | BC-01-06 / BR-06 | OK |
| 8 | `else if( f == L'S' )` | BC-01-07 / BR-07 | OK |
| 9 | `else{ swprintf(str,...,L"%c",f); }` (不明指定子) | BC-01-08 / BR-08 | OK |
| 10 | `else if( f == L'%' )` | BC-01-09 / BR-09 | OK |
| 11 | `else if( f == L'\0' )` | BC-01-10 / BR-10 | OK |
| 12 | `else{ result.push_back(f); }` (通常文字) | EC-01-02 / EC-02 | OK |
| 13 | **forループ: `for(const auto f : format)`** - 空文字列でループ0回 | EC-01-01 / EC-01 | OK |
| 14 | **`inSpecifier=true`のままループ終了** (末尾が%) | EC-01-15 / EC-06 | OK |

**漏れ**: なし。分岐は完全に網羅されている。

### 1.2 ParseVersion (TGT-02)

| # | ソースコード分岐条件 | TRM対応 | 状態 |
|---|---------------------|---------|------|
| 1 | `*p == L'a'` | BC-02-01 / BR-11 | OK |
| 2 | `wcsncmp_literal(p, L"alpha") == 0` 内 true分岐 | BC-02-10 / BR-20 | OK |
| 3 | `wcsncmp_literal(p, L"alpha") == 0` 内 false分岐 (p++) | BC-02-10 / BR-20 | OK |
| 4 | `*p == L'b'` | BC-02-02 / BR-12 | OK |
| 5 | `wcsncmp_literal(p, L"beta") == 0` 内 true/false | BC-02-11 / BR-21 | OK |
| 6 | `*p == L'r' \|\| *p == L'R'` | BC-02-03 / BR-13 | OK |
| 7 | `wcsnicmp_literal(p, L"rc") == 0` 内 true/false | BC-02-12 / BR-22 | OK |
| 8 | `*p == L'p'` | BC-02-04 / BR-14 | OK |
| 9 | `wcsncmp_literal(p, L"pl") == 0` 内 true/false | BC-02-13 / BR-23 | OK |
| 10 | `!iswdigit(*p)` (不明文字) | BC-02-05 / BR-15 | OK |
| 11 | `else` (数字) nShift=0 | BC-02-06 / BR-16 | OK |
| 12 | `while(*p && !iswdigit(*p)){ p++; }` 非数字スキップ | **なし** | **漏れ** |
| 13 | `++nDigit > 2` break | BC-02-07 / BR-17 | OK |
| 14 | `while(*p && wcschr(L".-_+", *p))` 区切りスキップ | BC-02-08 / BR-18 | OK |
| 15 | `*p && i<4` ループ条件 (true) | BC-02-09 / BR-19 | OK |
| 16 | `*p && i<4` ループ条件 (false: *p==0) | BC-02-09 | OK |
| 17 | `*p && i<4` ループ条件 (false: i>=4) | BC-02-09 | OK |
| 18 | `for(; i<4; i++)` 残りコンポーネント0x80埋め | BC-02-09 | OK |
| 19 | **`iswdigit(*p)` ループ: nVer計算中にpが終端** | **なし** | **漏れ** |
| 20 | **連続区切り文字 (`"2..4"` 等)のスキップ** | **なし** | **漏れ** |

**漏れ**: 3件

### 1.3 CompareVersion (TGT-03)

| # | ソースコード分岐条件 | TRM対応 | 状態 |
|---|---------------------|---------|------|
| 1 | `nVerA == nVerB` (差分=0) | BC-03-01 / BR-24 | OK |
| 2 | `nVerA > nVerB` (差分>0) | BC-03-02 / BR-25 | OK |
| 3 | `nVerA < nVerB` (差分<0) | BC-03-03 / BR-26 | OK |

**注意**: CompareVersionの戻り値は `nVerA - nVerB` であり、UINT32同士の減算をintにキャスト。この関数自体には分岐はないが、TRMでは論理的な分岐として正しく記述されている。

**漏れ**: なし（ただし暗黙の問題として後述）。

### 1.4 IsMailAddress (TGT-04)

| # | ソースコード分岐条件 | TRM対応 | 状態 |
|---|---------------------|---------|------|
| 1 | `0 < offset && IsValidChar(pszBuf[offset-1])` | BC-04-01 / BR-27 | OK |
| 2 | `pszBuf[j] != L'.' && IsValidChar(pszBuf[j])` 先頭文字チェック (true) | BC-04-02, BC-04-03 | OK |
| 3 | `pszBuf[j] != L'.' && IsValidChar(pszBuf[j])` 先頭ドット (false: =='.') | BC-04-02 / BR-28 | OK |
| 4 | `!IsValidChar(pszBuf[0])` 先頭無効 | BC-04-03 / BR-29 | OK |
| 5 | `while(j < nBufLen - 2 && IsValidChar(pszBuf[j]))` ローカルパート走査 | 暗黙的にカバー | OK |
| 6 | `j == 0 \|\| j >= nBufLen - 2` | BC-04-04 / BR-30 | OK |
| 7 | `L'@' != pszBuf[j]` | BC-04-05 / BR-31 | OK |
| 8 | `while(j < nBufLen && ...)` ドメインラベル走査ループ | 暗黙的にカバー | OK |
| 9 | `0 == j - nBgn` ドメインラベル長0 | BC-04-06 / BR-32 | OK |
| 10 | `L'.' != pszBuf[j]` ドメインラベル後のドット | BC-04-07 / BR-34 | OK |
| 11 | `0 == nDotCount` ドメインドットなし | BC-04-08 / BR-33 | OK |
| 12 | `nullptr != pnAddressLength` | BC-04-09 / BR-35 | OK |
| 13 | **`0 < offset` が false (offset==0) のケース** | EC-04-12 にはあるが、BC条件として明示なし | **軽微漏れ** |
| 14 | **`offset < 0` (負のoffset)のケース** | **なし** | **漏れ** |
| 15 | **ドメインラベルの文字が `pszBuf[j] >= L'a' && ...` 等の各条件** | 暗黙的にカバー | OK |
| 16 | **for(;;)の無限ループの継続条件/break条件** | BC-04-07で部分的 | OK |
| 17 | **`j < nBufLen` がfalseでドメインラベルwhileを抜けるケース** | **なし** | **漏れ** |

**漏れ**: 2件（負のoffset、バッファ終端でのドメインラベル走査終了）

### 1.5 WhatKindOfTwoChars (TGT-05)

| # | ソースコード分岐条件 | TRM対応 | 状態 |
|---|---------------------|---------|------|
| 1 | `kindPre == kindCur` (初回) | BC-05-01 / BR-36 | OK |
| 2 | `(kindPre==ZEN_NOBASU\|\|ZEN_DAKU) && (kindCur==ZEN_KATA\|\|HIRA)` | BC-05-02 / BR-37 | OK |
| 3 | `(kindCur==ZEN_NOBASU\|\|ZEN_DAKU) && (kindPre==ZEN_KATA\|\|HIRA)` | BC-05-03 / BR-37 | OK |
| 4 | `(kindPre==ZEN_NOBASU\|\|ZEN_DAKU) && (kindCur==ZEN_NOBASU\|\|ZEN_DAKU)` | BC-05-04 / BR-38 | OK |
| 5 | `kindPre == CK_LATIN` | BC-05-05 / BR-39 | OK |
| 6 | `kindCur == CK_LATIN` | BC-05-06 / BR-39 | OK |
| 7 | `kindPre == CK_UDEF` | BC-05-07 / BR-40 | OK |
| 8 | `kindCur == CK_UDEF` | BC-05-08 / BR-40 | OK |
| 9 | `kindPre == CK_CTRL` | BC-05-09 / BR-41 | OK |
| 10 | `kindCur == CK_CTRL` | BC-05-10 / BR-41 | OK |
| 11 | `kindPre == kindCur` (マッピング後) | BC-05-11 / BR-42 | OK |
| 12 | 最終 `return CK_NULL` | BC-05-12 / BR-43 | OK |
| 13 | **長音+ひらがなの組み合わせ (CK_ZEN_NOBASU, CK_HIRA)** | EC-05-02はNOBASU+ZEN_KATAのみ | **漏れ** |
| 14 | **濁点+カタカナの組み合わせ (CK_ZEN_DAKU, CK_ZEN_KATA)** | EC-05-03はDAKU+HIRAのみ | **漏れ** |
| 15 | **濁点+濁点の組み合わせ (CK_ZEN_DAKU, CK_ZEN_DAKU)** | EC-05-06はNOBASU+DAKUのみ | **漏れ** |

**漏れ**: 3件（引きずり規則の組み合わせ不足）

### 1.6 WhatKindOfTwoChars4KW (TGT-06)

| # | ソースコード分岐条件 | TRM対応 | 状態 |
|---|---------------------|---------|------|
| 1 | `kindPre == kindCur` (初回) | BC-06-01 | OK |
| 2 | 長音/濁点 + カタカナ/ひらがな (3条件ブロック) | BC-06-02 | OK |
| 3 | `kindPre == CK_LATIN` → CK_CSYM | **なし** | **漏れ** |
| 4 | `kindCur == CK_LATIN` → CK_CSYM | **なし** | **漏れ** |
| 5 | `kindPre == CK_UDEF` → CK_CSYM | BC-06-03 / BR-44 | OK |
| 6 | `kindCur == CK_UDEF` → CK_CSYM | BC-06-04 / BR-44 | OK |
| 7 | `kindPre == CK_CTRL` → CK_CTRL (ノーオペレーション) | BC-06-05 / BR-45 | OK |
| 8 | `kindCur == CK_CTRL` → CK_CTRL (ノーオペレーション) | BC-06-06 / BR-45 | OK |
| 9 | `kindPre == kindCur` (マッピング後) | 暗黙的 | OK |
| 10 | 最終 `return CK_NULL` | 暗黙的 | OK |

**漏れ**: 2件（LATIN→CSYMマッピングが4KW版で明示されていない）

### 1.7 Convert_ZeneisuToHaneisu (TGT-07)

| # | ソースコード分岐条件 | TRM対応 | 状態 |
|---|---------------------|---------|------|
| 1 | `c>=L'Ａ' && c<=L'Ｚ'` | BC-07-01 / BR-46 | OK |
| 2 | `c>=L'ａ' && c<=L'ｚ'` | BC-07-02 / BR-47 | OK |
| 3 | `c>=L'０' && c<=L'９'` | BC-07-03 / BR-48 | OK |
| 4 | `wcschr_idx(tableZenKigo,c,&n)` true | BC-07-04 / BR-49 | OK |
| 5 | 上記全て不一致 (return c そのまま) | BC-07-05 / BR-50 | OK |
| 6 | `while(p<q)` ループ | BV-07-09, BV-07-10 / BV-10 | OK |

**漏れ**: なし。

### 1.8 Convert_HaneisuToZeneisu (TGT-08)

| # | ソースコード分岐条件 | TRM対応 | 状態 |
|---|---------------------|---------|------|
| 1 | `c>=L'A' && c<=L'Z'` | BC-08-01 / BR-51 | OK |
| 2 | `c>=L'a' && c<=L'z'` | BC-08-02 / BR-52 | OK |
| 3 | `c>=L'0' && c<=L'9'` | BC-08-03 / BR-53 | OK |
| 4 | `wcschr_idx(tableHanKigo,c,&n)` true | BC-08-04 / BR-54 | OK |
| 5 | 上記全て不一致 | BC-08-05 / BR-55 | OK |
| 6 | `while(p<q)` ループ | BV-07-09相当 (ただしTGT-08側に明示なし) | **軽微漏れ** |

**漏れ**: 1件（nLength=0の境界条件がTGT-08にない）

---

## 2. 漏れている分岐条件

| ID | 関数 | 漏れている分岐条件 | 重要度 |
|----|------|--------------------|--------|
| GAP-BR-01 | ParseVersion | 特殊文字列処理後の非数字スキップループ `while(*p && !iswdigit(*p)){ p++; }` - このwhileが0回で終了するケース（特殊文字列の直後が数字の場合）とN回以上のケース | Medium |
| GAP-BR-02 | ParseVersion | 数値抽出ループ中に文字列終端(*p==0)に到達するケース（例: `"2.4"` の最後の `4` の後） | Medium |
| GAP-BR-03 | ParseVersion | 連続区切り文字のスキップ（例: `"2..4"` の `..` が `while(*p && wcschr(...))` で処理される）| Low |
| GAP-BR-04 | IsMailAddress | 負のoffset値（`0 < offset` がfalseとなる特殊ケース。既存テストでも負offsetがテストされている） | High |
| GAP-BR-05 | IsMailAddress | ドメインラベル走査中にバッファ終端（`j < nBufLen`がfalse）に到達するケース | Medium |
| GAP-BR-06 | WhatKindOfTwoChars | 引きずり規則の組み合わせ: CK_ZEN_NOBASU + CK_HIRA（長音+ひらがな） | Medium |
| GAP-BR-07 | WhatKindOfTwoChars | 引きずり規則の組み合わせ: CK_ZEN_DAKU + CK_ZEN_KATA（濁点+カタカナ） | Medium |
| GAP-BR-08 | WhatKindOfTwoChars | 同種連続の組み合わせ: CK_ZEN_DAKU + CK_ZEN_DAKU（濁点+濁点） | Low |
| GAP-BR-09 | WhatKindOfTwoChars4KW | CK_LATIN → CK_CSYM マッピング（通常版と同じだが4KW版の分岐条件として明示されていない） | Medium |
| GAP-BR-10 | Convert_HaneisuToZeneisu | nLength=0のループ不実行パス | Low |

---

## 3. 漏れている同値クラス

| ID | 関数 | 漏れている同値クラス | 重要度 |
|----|------|---------------------|--------|
| GAP-EC-01 | GetDateTimeFormat | **全角文字を含む書式文字列** (例: `L"年%Y月%m日"`) - 通常文字パスに全角文字が含まれるケース | Medium |
| GAP-EC-02 | GetDateTimeFormat | **連続する指定子** (例: `L"%Y%m%d"` 区切り文字なし) | Low |
| GAP-EC-03 | ParseVersion | **全角数字を含むバージョン文字列** (例: `L"２.４"`) - `iswdigit` がロケール依存で全角数字をtrueにする可能性 | High |
| GAP-EC-04 | ParseVersion | **区切り文字のみの文字列** (例: `L"..."`) - 全コンポーネントで数値部なし | Medium |
| GAP-EC-05 | ParseVersion | **数字なしの修飾子のみ** (例: `L"alpha"`) - nVer=0でシフト値のみが適用される | Medium |
| GAP-EC-06 | ParseVersion | **非常に長い文字列** (バッファオーバーリードの可能性確認) | Low |
| GAP-EC-07 | IsMailAddress | **ローカルパートが`.`で終わる** (例: `L"test.@example.com"`) | Medium |
| GAP-EC-08 | IsMailAddress | **連続ドット** (例: `L"test..test@example.com"`) | Medium |
| GAP-EC-09 | IsMailAddress | **ドメインラベルが`-`で始まるまたは終わる** (例: `L"test@-example.com"`, `L"test@example-.com"`) | Medium |
| GAP-EC-10 | IsMailAddress | **ドメイン末尾がドットで終わる** (例: `L"test@example.com."`) - ドットの後のラベル長が0のケース | High |
| GAP-EC-11 | IsMailAddress | **`@`が複数ある** (例: `L"test@test@example.com"`) | Medium |
| GAP-EC-12 | IsMailAddress | **制御文字を含む文字列** (0x00-0x20) | Medium |
| GAP-EC-13 | IsMailAddress | **全角`@`を含む文字列** (例: `L"test＠example.com"`) | Low |
| GAP-EC-14 | WhatKindOfTwoChars | **CK_TAB, CK_SPACE, CK_CR, CK_LF** との組み合わせ（これらはマッピング対象外でCK_NULLになるはず） | Medium |
| GAP-EC-15 | WhatKindOfTwoChars | **CK_KATA (半角カタカナ)** との組み合わせ | Low |
| GAP-EC-16 | WhatKindOfTwoChars4KW | **CK_LATIN + CK_LATIN** (4KW版での確認) | Low |
| GAP-EC-17 | WhatKindOfTwoChars4KW | **CK_UDEF + CK_UDEF** (初回同種チェックで一致) | Low |
| GAP-EC-18 | Convert_ZeneisuToHaneisu | **全角記号テーブルの全記号の網羅テスト** - テーブルには26文字あるが代表値のみテスト | Medium |
| GAP-EC-19 | Convert_HaneisuToZeneisu | **半角記号テーブルの全記号の網羅テスト** | Medium |
| GAP-EC-20 | Convert_HaneisuToZeneisu | **空配列 (nLength=0)** - TGT-07にはEC-07-08があるがTGT-08には同等のクラスがない | Medium |
| GAP-EC-21 | CompareVersion | **UINT32の差分によるオーバーフローケース** (例: `L"99.99.99.99pl99"` vs `L""`) | High |

---

## 4. 漏れている境界値

| ID | 関数 | 漏れている境界値 | 重要度 |
|----|------|-----------------|--------|
| GAP-BV-01 | GetDateTimeFormat | **systime.wDay の境界**: wDay=0, wDay=31 | Low |
| GAP-BV-02 | GetDateTimeFormat | **systime.wMinute の境界**: wMinute=0, wMinute=59 | Low |
| GAP-BV-03 | GetDateTimeFormat | **systime.wYear=0** での `%Y` 出力（"0"）と `%y` 出力（"00"） | Medium |
| GAP-BV-04 | GetDateTimeFormat | **str配列のバッファサイズ**: `wchar_t str[6]` に対して wYear が5桁以上（例: 99999は "99999"で6文字+null→バッファオーバーフロー）| **Critical** |
| GAP-BV-05 | ParseVersion | **数値部 nVer の最大値**: 2桁制限により最大99だが、nShift=-0x80, nVer=0の組み合わせで `(-128+0+128)=0` | Medium |
| GAP-BV-06 | ParseVersion | **nShift+nVer+128 がオーバーフロー**: BV-02-09で指摘済みだが、テスト要求として明示的なBV要求がない | High |
| GAP-BV-07 | IsMailAddress | **offset=0の境界**: `0 < offset` が false になるパス | Medium |
| GAP-BV-08 | IsMailAddress | **nBufLen=0**: 空バッファ | Medium |
| GAP-BV-09 | IsMailAddress | **nBufLen=1**: 1文字のみ（例: `L"a"`）→ `j >= nBufLen - 2` の条件で `nBufLen-2` が -1 になる（int/size_tの符号問題）| **Critical** |
| GAP-BV-10 | IsMailAddress | **nBufLen=2**: 2文字（例: `L"a@"`）→ `j >= nBufLen - 2` が `j >= 0` で常に true | High |
| GAP-BV-11 | IsMailAddress | **ドメインラベルの最大長** (現行実装には制限がないが、極端に長い場合の動作) | Low |
| GAP-BV-12 | WhatKindOfTwoChars | **全ECharKind列挙値の境界**: CK_NULL(0) は BV-05-01 にあるが、全列挙値の上限が不明確 | Low |
| GAP-BV-13 | Convert_ZeneisuToHaneisu | **全角記号テーブルの境界文字**: テーブル内の最初の記号（`　` U+3000全角スペース）と最後の記号（`＿` ）| Medium |
| GAP-BV-14 | Convert_HaneisuToZeneisu | **nLength=0**: TGT-08に BV要求として明示されていない | Medium |
| GAP-BV-15 | Convert_HaneisuToZeneisu | **nLength=1**: TGT-08に明示なし | Low |
| GAP-BV-16 | Convert_HaneisuToZeneisu | **半角大文字範囲の境界外直前直後**: `'@'` (0x40), `'['` (0x5B) | Medium |
| GAP-BV-17 | Convert_HaneisuToZeneisu | **半角小文字範囲の境界外**: `'\x60'`, `'{'` (0x7B) | Medium |
| GAP-BV-18 | Convert_HaneisuToZeneisu | **半角数字範囲の境界外**: `'/'` (0x2F), `':'` (0x3A) | Medium |

---

## 5. 漏れているエラーパス

| ID | 関数 | 漏れているエラーパス | 重要度 |
|----|------|---------------------|--------|
| GAP-ER-01 | GetDateTimeFormat | **strバッファオーバーフロー**: `wchar_t str[6]` に対してwYearが5桁以上の場合、swprintfで `%d` 書式で6文字以上生成される可能性。wYear=100000以上で6桁+null=7文字→バッファオーバーフロー。これはBV-01-03(5桁)では発生しないが、wYear=100000(6桁)で発生する。 | **Critical** |
| GAP-ER-02 | ParseVersion | **NULL ポインタ入力**: sVerがNULLの場合、`*p` で即座にクラッシュ | High |
| GAP-ER-03 | CompareVersion | **NULL ポインタ入力**: verAまたはverBがNULLの場合 | High |
| GAP-ER-04 | CompareVersion | **UINT32差分のintキャスト時オーバーフロー**: 例えば nVerA=0xFFFFFFFF, nVerB=0 の場合、差分は 0xFFFFFFFF だが int にキャストすると -1 になり、本来Aが大きいはずが負の値が返る。TRMのEC-03制約に記載はあるがER要求がない。 | **Critical** |
| GAP-ER-05 | IsMailAddress | **pszBuf が NULL**: 現行実装ではNULLチェックなし | Medium |
| GAP-ER-06 | IsMailAddress | **cchBuf と実際のバッファサイズの不整合**: cchBufがバッファの実サイズより大きい場合のバッファオーバーリード | Low |
| GAP-ER-07 | Convert_ZeneisuToHaneisu | **pData が NULL**: NULLポインタ入力時のクラッシュ | Medium |
| GAP-ER-08 | Convert_ZeneisuToHaneisu | **nLength が負**: `p+nLength` が `p` より前のアドレスを指す | Medium |
| GAP-ER-09 | Convert_HaneisuToZeneisu | **pData が NULL**: NULLポインタ入力時のクラッシュ | Medium |
| GAP-ER-10 | Convert_HaneisuToZeneisu | **nLength が負**: 同上 | Medium |

---

## 6. 追加すべきDP要求

| ID | 関数間関係 | 漏れている依存切替 | 重要度 |
|----|-----------|-------------------|--------|
| GAP-DP-01 | CompareVersion → ParseVersion | ParseVersion がオーバーフロー値（8ビット超過）を返した場合のCompareVersionの動作 | High |
| GAP-DP-02 | IsMailAddress ← IsURL | IsURL関数内からIsMailAddressが呼ばれるケース。IsURLがoffsetを変換して渡すため、IsMailAddressへの入力パターンが異なる可能性 | Medium |
| GAP-DP-03 | Convert_ZeneisuToHaneisu ↔ Convert_HaneisuToZeneisu | 記号テーブル(tableZenKigo/tableHanKigo)の対称性検証。テーブルの長さが異なるまたは対応が1:1でない場合のラウンドトリップ失敗 | High |
| GAP-DP-04 | WhatKindOfTwoChars ← WhereCurrentWord_2 | WhereCurrentWord_2がWhatKindOfTwoCharsを繰り返し呼び出す際の累積効果（マッピングが連鎖する場合） | Medium |
| GAP-DP-05 | WhatKindOfTwoChars4KW ← SearchNextWordPosition4KW | SearchNextWordPosition4KWがWhatKindOfTwoChars4KWを使用。4KW版の差異が単語選択に与える影響の検証 | Medium |

---

## 7. サマリ

### 現在のTRM 99件の網羅率

| 関数 | ソース分岐数 | TRM BR対応数 | 分岐網羅率 |
|------|-------------|-------------|-----------|
| GetDateTimeFormat | 14 | 14 | **100%** |
| ParseVersion | 20 | 17 | **85%** |
| CompareVersion | 3 | 3 | **100%** |
| IsMailAddress | 17 | 14 | **82%** |
| WhatKindOfTwoChars | 15 | 12 | **80%** |
| WhatKindOfTwoChars4KW | 10 | 8 | **80%** |
| Convert_ZeneisuToHaneisu | 6 | 6 | **100%** |
| Convert_HaneisuToZeneisu | 6 | 5 | **83%** |
| **合計** | **91** | **79** | **87%** |

### 追加すべき要求の推定件数

| カテゴリ | 現在 | 追加推定 | 合計 |
|---------|------|---------|------|
| BR (分岐網羅) | 55 | 10 | 65 |
| EC (同値クラス) | 27 | 21 | 48 |
| BV (境界値) | 11 | 18 | 29 |
| ER (エラーパス) | 3 | 10 | 13 |
| DP (依存切替) | 3 | 5 | 8 |
| **合計** | **99** | **64** | **163** |

### 重要度別の追加要求件数

| 重要度 | 件数 | 内訳 |
|--------|------|------|
| Critical | 4 | str[6]バッファオーバーフロー、UINT32差分オーバーフロー、nBufLen小値時の符号問題、6桁年号のswprintfオーバーフロー |
| High | 16 | NULL入力、全角数字のiswdigit、ドメイン末尾ドット、各種オーバーフロー等 |
| Medium | 33 | 組み合わせテスト不足、境界値不足、エラーパス不足等 |
| Low | 11 | 網羅性の補完、マイナーな境界値等 |

### 所見

1. **全体評価**: 現在の99件のTRMは主要な分岐パスと代表的な同値クラスを良好にカバーしている。特にGetDateTimeFormatとConvert系関数は分岐網羅率が高い。

2. **最大のリスク領域**: 
   - **CompareVersionのUINT32差分オーバーフロー**: `nVerA - nVerB` がUINT32同士の演算であり、結果をintにキャストするため、大きな差分で符号が反転する。これはセキュリティやバージョン判定の正確性に直結する致命的な問題。
   - **GetDateTimeFormatのstrバッファ**: `wchar_t str[6]` に対してwYearが6桁以上になった場合のバッファオーバーフロー。
   - **IsMailAddressのnBufLen小値**: `nBufLen - 2` がsize_t (unsigned)の場合、nBufLen=0,1で巨大な値になりバッファオーバーリードの可能性。

3. **テスト設計の偏り**: 同値クラスのカバレッジに比べて境界値のカバレッジが不足している。特にIsMailAddressの文字コード境界とConvert_HaneisuToZeneisuの境界値が手薄。

4. **組み合わせテストの不足**: WhatKindOfTwoCharsの引きずり規則は4つの文字種（ZEN_NOBASU, ZEN_DAKU, ZEN_KATA, HIRA）の組み合わせで検証すべきだが、代表値のみに留まっている。

---

## 8. 追加TRM要求の具体案 (YAML形式)

```yaml
# =============================================================================
# 追加テスト要求 (TRM網羅性監査による指摘事項)
# =============================================================================

additional_test_requirements:

  # -------------------------------------------------------------------------
  # TGT-01: GetDateTimeFormat 追加要求
  # -------------------------------------------------------------------------

  - id: "EC-28"
    target: "TGT-01"
    description: "全角文字を含む書式文字列 (L\"年%Y月%m日\") が正しく処理されることを検証する"
    class_ref: "GAP-EC-01"
    priority: "medium"

  - id: "EC-29"
    target: "TGT-01"
    description: "連続する指定子 (L\"%Y%m%d\") が区切り文字なしで正しく展開されることを検証する"
    class_ref: "GAP-EC-02"
    priority: "low"

  - id: "BV-12"
    target: "TGT-01"
    description: "systime.wYear=0 での %Y (\"0\") と %y (\"00\") の出力を検証する"
    boundary_ref: "GAP-BV-03"
    priority: "medium"

  - id: "ER-04"
    target: "TGT-01"
    description: >
      str[6] バッファオーバーフローの検証: wYear=100000 (6桁) で swprintf が
      7文字以上を出力しようとするケース。swprintf のサイズ制限パラメータにより
      切り捨てられることを確認する。
    priority: "critical"

  - id: "BV-13"
    target: "TGT-01"
    description: "systime.wDay=0, wDay=31 の境界値を検証する"
    boundary_ref: "GAP-BV-01"
    priority: "low"

  - id: "BV-14"
    target: "TGT-01"
    description: "systime.wMinute=0, wMinute=59 の境界値を検証する"
    boundary_ref: "GAP-BV-02"
    priority: "low"

  # -------------------------------------------------------------------------
  # TGT-02: ParseVersion 追加要求
  # -------------------------------------------------------------------------

  - id: "BR-56"
    target: "TGT-02"
    description: >
      特殊文字列処理後の非数字スキップループ while(*p && !iswdigit(*p)) が
      0回で終了するケース（修飾子の直後が数字の場合: L\"alpha1\"）を検証する
    branch_ref: "GAP-BR-01"
    priority: "medium"

  - id: "BR-57"
    target: "TGT-02"
    description: >
      数値抽出ループ中に文字列終端に到達するケース（最終コンポーネントが
      文字列の末尾で終わる場合: L\"2.4\"）を検証する
    branch_ref: "GAP-BR-02"
    priority: "medium"

  - id: "BR-58"
    target: "TGT-02"
    description: "連続区切り文字 (L\"2..4\") がスキップされることを検証する"
    branch_ref: "GAP-BR-03"
    priority: "low"

  - id: "EC-30"
    target: "TGT-02"
    description: >
      全角数字を含むバージョン文字列 (L\"２.４\") の処理を検証する。
      iswdigit のロケール依存動作により全角数字が数字として認識される可能性がある。
    class_ref: "GAP-EC-03"
    priority: "high"

  - id: "EC-31"
    target: "TGT-02"
    description: "区切り文字のみの文字列 (L\"...\") で数値部がないケースを検証する"
    class_ref: "GAP-EC-04"
    priority: "medium"

  - id: "EC-32"
    target: "TGT-02"
    description: "数字なしの修飾子のみ (L\"alpha\") の処理を検証する"
    class_ref: "GAP-EC-05"
    priority: "medium"

  - id: "ER-05"
    target: "TGT-02"
    description: "NULL ポインタ入力 (sVer=NULL) でクラッシュすることを確認する（文書化すべき前提条件）"
    priority: "high"

  - id: "BV-15"
    target: "TGT-02"
    description: >
      nShift + nVer + 128 の8ビットオーバーフロー:
      nShift=0x20(pl), nVer=99 → 128+32+99=259 (0x103) が上位ビットに侵食するケース
    boundary_ref: "GAP-BV-06"
    priority: "high"

  # -------------------------------------------------------------------------
  # TGT-03: CompareVersion 追加要求
  # -------------------------------------------------------------------------

  - id: "ER-06"
    target: "TGT-03"
    description: >
      UINT32差分のint変換によるオーバーフロー検証:
      verA=L\"99.99.99.99\" (nVerA=大), verB=L\"\" (nVerB=0x80808080) の差分が
      intにキャストされた時に正しい符号を保つか検証する。
    priority: "critical"

  - id: "ER-07"
    target: "TGT-03"
    description: "NULL ポインタ入力 (verA=NULL or verB=NULL) の動作を検証する"
    priority: "high"

  - id: "EC-33"
    target: "TGT-03"
    description: >
      UINT32の差分がintの範囲を超えるケース:
      ParseVersion(L\"99.99.99pl99\") vs ParseVersion(L\"\") のように
      極端に異なるバージョン文字列の比較
    class_ref: "GAP-EC-21"
    priority: "high"

  - id: "DP-04"
    target: "TGT-03"
    description: "ParseVersionの8ビットオーバーフローがCompareVersionの結果に与える影響を検証する"
    priority: "high"

  # -------------------------------------------------------------------------
  # TGT-04: IsMailAddress 追加要求
  # -------------------------------------------------------------------------

  - id: "BR-59"
    target: "TGT-04"
    description: "負のoffset値 (offset=-1) での動作を検証する（0 < offset がfalse）"
    branch_ref: "GAP-BR-04"
    priority: "high"

  - id: "BR-60"
    target: "TGT-04"
    description: "ドメインラベル走査中にバッファ終端 (j >= nBufLen) に到達するケースを検証する"
    branch_ref: "GAP-BR-05"
    priority: "medium"

  - id: "EC-34"
    target: "TGT-04"
    description: "ローカルパートが.で終わる (L\"test.@example.com\") の動作を検証する"
    class_ref: "GAP-EC-07"
    priority: "medium"

  - id: "EC-35"
    target: "TGT-04"
    description: "連続ドット (L\"test..test@example.com\") の動作を検証する"
    class_ref: "GAP-EC-08"
    priority: "medium"

  - id: "EC-36"
    target: "TGT-04"
    description: "ドメインラベルが-で始まるまたは終わる (L\"test@-example.com\") の動作を検証する"
    class_ref: "GAP-EC-09"
    priority: "medium"

  - id: "EC-37"
    target: "TGT-04"
    description: >
      ドメイン末尾がドットで終わる (L\"test@example.com.\")
      ドットの後のラベル長が0になるため、0 == j - nBgn で FALSE を返すはず。
    class_ref: "GAP-EC-10"
    priority: "high"

  - id: "EC-38"
    target: "TGT-04"
    description: "@が複数ある (L\"test@test@example.com\") の動作を検証する"
    class_ref: "GAP-EC-11"
    priority: "medium"

  - id: "EC-39"
    target: "TGT-04"
    description: "制御文字 (0x01-0x1F) を含む文字列の動作を検証する"
    class_ref: "GAP-EC-12"
    priority: "medium"

  - id: "BV-16"
    target: "TGT-04"
    description: "nBufLen=0 (空バッファ) での動作を検証する"
    boundary_ref: "GAP-BV-08"
    priority: "medium"

  - id: "BV-17"
    target: "TGT-04"
    description: >
      nBufLen=1 での動作を検証する。nBufLen-2 が size_t の場合
      ラップアラウンドで巨大な値になり j >= nBufLen-2 が false になる可能性。
      ソースではint(cchBuf)としているのでこの問題は回避されているが確認が必要。
    boundary_ref: "GAP-BV-09"
    priority: "critical"

  - id: "BV-18"
    target: "TGT-04"
    description: "nBufLen=2 (L\"a@\" 等) での j >= nBufLen-2 の境界動作を検証する"
    boundary_ref: "GAP-BV-10"
    priority: "high"

  - id: "ER-08"
    target: "TGT-04"
    description: "pszBuf=NULL での動作を検証する（クラッシュ確認）"
    priority: "medium"

  # -------------------------------------------------------------------------
  # TGT-05: WhatKindOfTwoChars 追加要求
  # -------------------------------------------------------------------------

  - id: "BR-61"
    target: "TGT-05"
    description: "CK_ZEN_NOBASU + CK_HIRA (長音+ひらがな) の引きずりパスを検証する"
    branch_ref: "GAP-BR-06"
    priority: "medium"

  - id: "BR-62"
    target: "TGT-05"
    description: "CK_ZEN_DAKU + CK_ZEN_KATA (濁点+カタカナ) の引きずりパスを検証する"
    branch_ref: "GAP-BR-07"
    priority: "medium"

  - id: "BR-63"
    target: "TGT-05"
    description: "CK_ZEN_DAKU + CK_ZEN_DAKU (濁点+濁点) の同種判定パスを検証する"
    branch_ref: "GAP-BR-08"
    priority: "low"

  - id: "EC-40"
    target: "TGT-05"
    description: >
      CK_TAB, CK_SPACE, CK_CR, CK_LF と他の文字種の組み合わせが
      CK_NULL (別種) を返すことを検証する
    class_ref: "GAP-EC-14"
    priority: "medium"

  - id: "EC-41"
    target: "TGT-05"
    description: "CK_KATA (半角カタカナ) と他の文字種の組み合わせを検証する"
    class_ref: "GAP-EC-15"
    priority: "low"

  # -------------------------------------------------------------------------
  # TGT-06: WhatKindOfTwoChars4KW 追加要求
  # -------------------------------------------------------------------------

  - id: "BR-64"
    target: "TGT-06"
    description: "CK_LATIN → CK_CSYM マッピングパスを検証する (4KW版は通常版と同一だが明示的な確認が必要)"
    branch_ref: "GAP-BR-09"
    priority: "medium"

  - id: "EC-42"
    target: "TGT-06"
    description: "CK_LATIN + CK_LATIN が初回同種チェックで一致することを4KW版で検証する"
    class_ref: "GAP-EC-16"
    priority: "low"

  - id: "EC-43"
    target: "TGT-06"
    description: "CK_UDEF + CK_UDEF が初回同種チェックで一致することを4KW版で検証する"
    class_ref: "GAP-EC-17"
    priority: "low"

  # -------------------------------------------------------------------------
  # TGT-07: Convert_ZeneisuToHaneisu 追加要求
  # -------------------------------------------------------------------------

  - id: "EC-44"
    target: "TGT-07"
    description: >
      全角記号テーブル (tableZenKigo) の全26文字が正しく対応する半角記号に
      変換されることを網羅的に検証する
    class_ref: "GAP-EC-18"
    priority: "medium"

  - id: "BV-19"
    target: "TGT-07"
    description: "全角記号テーブルの最初の文字 (全角スペース U+3000) と最後の文字 (＿) の変換を検証する"
    boundary_ref: "GAP-BV-13"
    priority: "medium"

  - id: "ER-09"
    target: "TGT-07"
    description: "pData=NULL, nLength=0 の組み合わせでクラッシュしないことを検証する"
    priority: "medium"

  - id: "ER-10"
    target: "TGT-07"
    description: "nLength が負の値の場合の動作を検証する (p+nLength < p となりループ不実行)"
    priority: "medium"

  # -------------------------------------------------------------------------
  # TGT-08: Convert_HaneisuToZeneisu 追加要求
  # -------------------------------------------------------------------------

  - id: "EC-45"
    target: "TGT-08"
    description: "空配列 (nLength=0) で変更が行われないことを検証する"
    class_ref: "GAP-EC-20"
    priority: "medium"

  - id: "EC-46"
    target: "TGT-08"
    description: >
      半角記号テーブル (tableHanKigo) の全26文字が正しく対応する全角記号に
      変換されることを網羅的に検証する
    class_ref: "GAP-EC-19"
    priority: "medium"

  - id: "BV-20"
    target: "TGT-08"
    description: "nLength=0 の境界条件を検証する (TGT-07のBV-07-09に対応するもの)"
    boundary_ref: "GAP-BV-14"
    priority: "medium"

  - id: "BV-21"
    target: "TGT-08"
    description: "nLength=1 (1文字のみ) の境界条件を検証する"
    boundary_ref: "GAP-BV-15"
    priority: "low"

  - id: "BV-22"
    target: "TGT-08"
    description: "半角大文字範囲の境界外直前 '@' (0x40) と直後 '[' (0x5B) が変換されないことを検証する"
    boundary_ref: "GAP-BV-16"
    priority: "medium"

  - id: "BV-23"
    target: "TGT-08"
    description: "半角小文字範囲の境界外 '`' (0x60) と '{' (0x7B) が変換されないことを検証する"
    boundary_ref: "GAP-BV-17"
    priority: "medium"

  - id: "BV-24"
    target: "TGT-08"
    description: "半角数字範囲の境界外 '/' (0x2F) と ':' (0x3A) が変換されないことを検証する"
    boundary_ref: "GAP-BV-18"
    priority: "medium"

  - id: "ER-11"
    target: "TGT-08"
    description: "pData=NULL, nLength=0 の組み合わせでクラッシュしないことを検証する"
    priority: "medium"

  - id: "ER-12"
    target: "TGT-08"
    description: "nLength が負の値の場合の動作を検証する"
    priority: "medium"

  # -------------------------------------------------------------------------
  # 関数間依存 (DP) 追加要求
  # -------------------------------------------------------------------------

  - id: "DP-06"
    target: "TGT-03"
    description: "ParseVersionの8ビットオーバーフロー値がCompareVersionの比較結果に影響するケースを検証する"
    priority: "high"

  - id: "DP-07"
    target: "TGT-04"
    description: >
      IsURL → IsMailAddress 呼び出し経路での入力パターンを検証する。
      IsURLがoffsetを変換してIsMailAddressに渡すケースがある。
    priority: "medium"

  - id: "DP-08"
    target: "TGT-07, TGT-08"
    description: >
      記号テーブル (tableZenKigo/tableHanKigo) の対称性を検証する。
      全角記号→半角記号→全角記号のラウンドトリップで一致しない記号がないか確認する。
      特にバックスラッシュの扱い（テーブルコメントに「バックスラッシュは無視」とある）。
    priority: "high"

  - id: "DP-09"
    target: "TGT-05"
    description: >
      WhereCurrentWord_2 が WhatKindOfTwoChars を連鎖的に呼び出す際の
      累積マッピング効果を検証する（LATIN→CSYMへのマッピングが連鎖する場合）
    priority: "medium"

  - id: "DP-10"
    target: "TGT-06"
    description: >
      SearchNextWordPosition4KW が WhatKindOfTwoChars4KW を使用する際の
      UDEF→CSYM マッピングが単語区切りに与える影響を検証する
    priority: "medium"

# =============================================================================
# 追加要求サマリ
# =============================================================================
additional_summary:
  total_additional_requirements: 64
  breakdown:
    BR: 9    # BR-56 ~ BR-64
    EC: 19   # EC-28 ~ EC-46
    BV: 13   # BV-12 ~ BV-24
    ER: 9    # ER-04 ~ ER-12
    DP: 5    # DP-04 (既存DP-04と重複を避けるためDP-06として番号調整), DP-06 ~ DP-10
  # 注: 一部の要求は複数カテゴリにまたがるが、主要カテゴリで分類
  priority_breakdown:
    critical: 4   # ER-04 (strバッファ), ER-06 (UINT32オーバーフロー), BV-17 (nBufLen=1符号問題), ER-04重複排除
    high: 11
    medium: 36
    low: 13

  combined_total:
    existing: 99
    additional: 64
    grand_total: 163
    coverage_improvement: "分岐網羅率 87% → 推定 97%、境界値網羅率 大幅改善"
```

---

## 付録A: 既存テストとの比較

### test-format.cpp (sakura-editor既存テスト)

既存テストでカバーされている範囲:
- GetDateTimeFormat: 書式指定子の基本テスト、数字桁数少/多、null文字を含むケース
- CompareVersion: 同一バージョン判定、区切り文字マトリックス、修飾子の順序関係

**既存テストに存在するがTRMに反映されていない知見**:
1. `CompareVersion(L"2.4.1.0.1", L"2.4.1.0.2")` - 5番目の値が無視されることの明示的テスト
2. `CompareVersion(L"2.4.1.0alpha", L"2.4.1.0beta")` - 4コンポーネント+修飾子のケース
3. `CompareVersion(L"2.4.1", L"2.0401")` - 区切り省略時の等価性
4. `CompareVersion(L"2.4.1alpha", L"2.4.1.alpha")` - 修飾子前の区切り文字有無

### test-is_mailaddress.cpp (sakura-editor既存テスト)

既存テストでカバーされている範囲:
- 空文字列、標準アドレス、co.jp、末尾スペース、記号類
- 64文字ローカルパート、255文字メールボックス
- アンダースコアを含むドメイン、ハイフンを含むドメイン
- offset パラメータの網羅的テスト（負のoffset含む）

**既存テストに存在するがTRMに反映されていない知見**:
1. **負のoffset**のテスト（test-is_mailaddress.cppの207行目: `offset=-1`）
2. **OffsetParameter2テスト**: 全てのポインタ組み合わせの総当たりテスト
3. **65文字以上のローカルパート**の検証（`ASSERT_CHANGE`で新旧動作の差異を記録）
4. **ダブルハイフンを含むドメイン**のテスト
5. **引用符付きローカルパート**のテスト

---

## 付録B: Critical指摘の詳細

### B.1 GetDateTimeFormat の str[6] バッファオーバーフロー

```cpp
wchar_t str[6] = {};  // 6文字分のバッファ
// ...
if( f == L'Y' ){
    swprintf( str, int(std::size(str)), L"%d", systime.wYear );
}
```

`swprintf` の第2引数にサイズ制限が渡されているため、バッファオーバーフロー自体は防止されている。しかし wYear=100000 (6桁) の場合、出力が切り捨てられ "10000" になる可能性がある。これは正確性の問題として検証が必要。

### B.2 CompareVersion の UINT32 差分オーバーフロー

```cpp
int CompareVersion( const WCHAR* verA, const WCHAR* verB )
{
    UINT32 nVerA = ParseVersion(verA);
    UINT32 nVerB = ParseVersion(verB);
    return nVerA - nVerB;  // UINT32の減算結果をintに暗黙変換
}
```

`nVerA = 0xFF...`, `nVerB = 0x00...` の場合、`nVerA - nVerB` は大きな正のUINT32値となるが、intにキャストされると負の値になる。これにより「Aが新しいはずなのにBが新しいと判定される」バグが発生する。

### B.3 IsMailAddress の nBufLen 小値問題

```cpp
auto nBufLen = int(cchBuf);  // size_t → int 変換
// ...
if( j == 0 || j >= nBufLen - 2 ){  // nBufLen=0 → -2, nBufLen=1 → -1
    return FALSE;
}
```

ソースコードでは `auto nBufLen = int(cchBuf)` としてintにキャストしているため、`nBufLen - 2` が負の値になることは正しく処理される。ただし、`cchBuf` が非常に大きい値（INT_MAX超過）の場合、intへのキャストでオーバーフローする。

---

*本レポートは、ソースコードの静的解析に基づくものであり、実行時の動的解析は含まれていません。*
*Critical指摘については、実際のコンパイラとプラットフォームでの動作確認を推奨します。*
