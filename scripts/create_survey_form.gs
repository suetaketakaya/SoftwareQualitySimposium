/**
 * create_survey_form.gs — 非エンジニア向けTRM可読性アンケートを Google Forms として生成するスクリプト
 *
 * 使い方:
 *  1. https://script.google.com で新規プロジェクトを作成
 *  2. 本ファイルの内容をコードエディタに貼り付け
 *  3. リバーシサンプルコードを SAMPLE_REVERSI_CODE に埋め込む（既に埋め込み済み）
 *  4. パターン3で利用する画像URLを PATTERN_3_IMAGE_URL に設定（Googleドライブ共有URL等）
 *  5. 関数 `createSurveyForm` を実行
 *  6. ログに表示される公開URLをアンケート配布に利用
 *
 * 参考: knowledge/survey-design.md
 */

// -----------------------------------------------------------------------------
// 題材: リバーシサンプルコード（experiments/reversi/reversi_move.py から転記）
// -----------------------------------------------------------------------------

const SAMPLE_REVERSI_CODE = `BOARD_SIZE = 8
DIRECTIONS = [(-1, -1), (-1, 0), (-1, 1),
              ( 0, -1),          ( 0, 1),
              ( 1, -1), ( 1, 0), ( 1, 1)]

def is_on_board(x, y):
    return 0 <= x < BOARD_SIZE and 0 <= y < BOARD_SIZE

def is_empty(board, x, y):
    return board[x][y] == 0

def count_flippable(board, x, y, dx, dy, me):
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

def is_valid_move(board, x, y, me):
    if not is_on_board(x, y):
        return False
    if not is_empty(board, x, y):
        return False
    for dx, dy in DIRECTIONS:
        if count_flippable(board, x, y, dx, dy, me) > 0:
            return True
    return False`;

// パターン3（階層＋図解）で表示する盤面図のURL。未設定でもForm生成は成立
const PATTERN_3_IMAGE_URL = "";  // 例: "https://drive.google.com/uc?id=XXXXXXXX"

// -----------------------------------------------------------------------------
// メイン
// -----------------------------------------------------------------------------

function createSurveyForm() {
  const form = FormApp.create("TRM可読性アンケート — 品質情報の共有形式に関する評価")
    .setDescription(
      "このアンケートは、ソフトウェア開発で使われる「テスト要求」という品質情報を、" +
      "非エンジニアの方にも理解しやすい形で共有するための研究にご協力いただくものです。\n\n" +
      "所要時間は約12〜15分です。全ての回答は研究目的にのみ使用し、" +
      "個人を特定する情報を公表することはありません。\n\n" +
      "コードを読んだ経験がなくても問題ありません。感じたままにお答えください。"
    )
    .setCollectEmail(false)
    .setAllowResponseEdits(false)
    .setShowLinkToRespondAgain(false)
    .setProgressBar(true);

  addSection_0_Consent(form);
  addSection_1_Demographics(form);
  addSection_2_PriorKnowledge(form);
  addSection_3_Reversi(form);
  addSection_4_Sakura(form);
  addSection_5_Overall(form);

  const publicUrl = form.getPublishedUrl();
  const editUrl = form.getEditUrl();
  Logger.log("Published URL (回答用): " + publicUrl);
  Logger.log("Edit URL (編集用): " + editUrl);
  return { publicUrl: publicUrl, editUrl: editUrl };
}

// -----------------------------------------------------------------------------
// [0] 同意
// -----------------------------------------------------------------------------

function addSection_0_Consent(form) {
  form.addPageBreakItem().setTitle("はじめに");
  form.addMultipleChoiceItem()
    .setTitle("回答に同意いただけますか？")
    .setRequired(true)
    .setChoiceValues(["同意する"]);
}

// -----------------------------------------------------------------------------
// [1] 属性質問
// -----------------------------------------------------------------------------

function addSection_1_Demographics(form) {
  form.addPageBreakItem().setTitle("あなたについて").setHelpText("5問、約2分");

  form.addMultipleChoiceItem()
    .setTitle("Q1. あなたの職種を教えてください")
    .setRequired(true)
    .setChoiceValues([
      "企画", "デザイン", "PM (プロダクトマネージャー)", "営業",
      "マーケティング", "エンジニア", "QA・テスト", "その他"
    ]);

  form.addMultipleChoiceItem()
    .setTitle("Q2. エンジニアとしての実務経験年数")
    .setRequired(true)
    .setChoiceValues([
      "未経験", "1年未満", "1〜3年", "3〜5年", "5〜10年", "10年以上"
    ]);

  form.addMultipleChoiceItem()
    .setTitle("Q3. 日常的にコードを読む頻度")
    .setRequired(true)
    .setChoiceValues(["読まない", "月に数回", "週に数回", "ほぼ毎日"]);

  form.addMultipleChoiceItem()
    .setTitle("Q4. 「単体テスト」という言葉を聞いたことがありますか？")
    .setRequired(true)
    .setChoiceValues([
      "初めて聞いた",
      "聞いたことはある",
      "意味を知っている",
      "自分で書いたことがある"
    ]);

  form.addMultipleChoiceItem()
    .setTitle("Q5. 今回の回答に対するあなたの立場を選んでください（分析軸として使用）")
    .setRequired(true)
    .setChoiceValues([
      "非エンジニア",
      "エンジニア寄り非エンジニア（技術系PM等）",
      "エンジニア"
    ]);
}

// -----------------------------------------------------------------------------
// [2] 前提知識
// -----------------------------------------------------------------------------

function addSection_2_PriorKnowledge(form) {
  form.addPageBreakItem()
    .setTitle("前提知識の確認")
    .setHelpText(
      "テスト要求とは、プログラムの機能が「こういう入力なら、こう動くべき」という検証ポイントを" +
      "一つひとつ書き出したものです。\n\n例: 「1歳未満のユーザーには割引を適用しないこと」"
    );

  form.addScaleItem()
    .setTitle("Q6. 上記の説明は理解できましたか？")
    .setRequired(true)
    .setBounds(1, 5)
    .setLabels("全くわからない", "とてもよくわかる");
}

// -----------------------------------------------------------------------------
// [3] 題材A: リバーシ
// -----------------------------------------------------------------------------

function addSection_3_Reversi(form) {
  // A-0: コード提示
  form.addPageBreakItem()
    .setTitle("題材A: リバーシの合法手判定")
    .setHelpText(
      "これから、リバーシの「この場所に石を置けるかどうか」を判定する処理を題材にします。\n" +
      "リバーシのルールをご存じなくても、以下で十分です:\n" +
      "・盤面の空きマスに、自分の石を置く\n" +
      "・置いた石の直線上に、相手の石を挟める向きがあるとき、その手は「合法」\n" +
      "・何も挟めない場所には置けない\n\n" +
      "次のコードを見てください:\n\n" +
      SAMPLE_REVERSI_CODE
    );

  form.addScaleItem()
    .setTitle("Q7. 上のコードは、リバーシの「置けるか置けないか」を判定する処理です。おおよその意図が理解できましたか？")
    .setRequired(true)
    .setBounds(1, 5)
    .setLabels("全くわからない", "とてもよくわかる");

  form.addCheckboxItem()
    .setTitle("Q8. 以下のうち、このコードが扱っていそうな判断はどれですか（複数可）")
    .setRequired(true)
    .setChoiceValues([
      "盤面の範囲外かどうか",
      "すでに石が置かれているか",
      "挟める方向があるか",
      "相手の番かどうか",
      "ゲームが終わっているか"
    ]);

  // A-2: パターン1（自然文）
  form.addPageBreakItem()
    .setTitle("題材A パターン1: 自然文リスト")
    .setHelpText(
      "コードが正しく動くかを確認する「テスト要求」を、以下の形式で書き起こしました。\n\n" +
      "TR-01: 盤面の範囲外にある場所には、石を置けないこと\n" +
      "TR-02: すでに石が置かれている場所には、石を置けないこと\n" +
      "TR-03: 挟める方向が1つ以上ある場合のみ、石を置けること\n" +
      "TR-04: 盤面の端（角）に置く場合でも、挟める方向があれば置けること\n" +
      "TR-05: 相手の石が連続していても、最後が自分の石で終わっていない場合は挟まないこと"
    );

  form.addScaleItem()
    .setTitle("Q9. パターン1（自然文リスト）は、何を確認しようとしているか理解できましたか？")
    .setRequired(true)
    .setBounds(1, 5)
    .setLabels("全くわからない", "とてもよくわかる");

  form.addScaleItem()
    .setTitle("Q10. パターン1を見て、このコードが「十分に検証されている」と感じられましたか？")
    .setRequired(true)
    .setBounds(1, 5)
    .setLabels("全く感じない", "強く感じる");

  // A-3: パターン2（階層化）
  form.addPageBreakItem()
    .setTitle("題材A パターン2: 要約＋詳細の階層")
    .setHelpText(
      "同じテスト要求を、要約と詳細の2階層に分けて整理しました。\n\n" +
      "要約1: 置ける場所のルールを正しく判定する\n" +
      "  ├ TR-01: 盤面の範囲外の場所は拒否\n" +
      "  ├ TR-02: 石が既にある場所は拒否\n" +
      "  └ TR-03: 挟める方向がない場所は拒否\n\n" +
      "要約2: 特殊な盤面位置でも正しく判定する\n" +
      "  ├ TR-04: 盤面の角での判定\n" +
      "  └ TR-05: 盤面の辺での判定\n\n" +
      "要約3: 「挟める」の判定が正しく行われる\n" +
      "  ├ TR-06: 相手の石が1個だけでも挟めれば合法\n" +
      "  ├ TR-07: 途中に自分の石があると挟めない\n" +
      "  └ TR-08: 空きマスで止まっていたら挟めない"
    );

  form.addScaleItem()
    .setTitle("Q11. パターン2（要約＋詳細の階層）は、何を確認しようとしているか理解できましたか？")
    .setRequired(true)
    .setBounds(1, 5)
    .setLabels("全くわからない", "とてもよくわかる");

  form.addScaleItem()
    .setTitle("Q12. パターン2は、要約だけ読むと意図が伝わると感じましたか？")
    .setRequired(true)
    .setBounds(1, 5)
    .setLabels("全く伝わらない", "よく伝わる");

  // A-4: パターン3（視覚化併用）
  const p3 = form.addPageBreakItem()
    .setTitle("題材A パターン3: 階層＋図解");

  let p3Help = "パターン2の階層に加えて、盤面のケースを図にしたものです。";
  if (PATTERN_3_IMAGE_URL) {
    p3Help += "\n\n図: " + PATTERN_3_IMAGE_URL;
  } else {
    p3Help += "\n\n(運用時には盤面図をここに挿入します)";
  }
  p3.setHelpText(p3Help);

  if (PATTERN_3_IMAGE_URL) {
    try {
      const image = UrlFetchApp.fetch(PATTERN_3_IMAGE_URL).getBlob();
      form.addImageItem()
        .setImage(image)
        .setTitle("盤面ケース図解");
    } catch (e) {
      Logger.log("Warning: 画像取得に失敗しました: " + e);
    }
  }

  form.addScaleItem()
    .setTitle("Q13. パターン3（階層＋図解）は、何を確認しようとしているか理解できましたか？")
    .setRequired(true)
    .setBounds(1, 5)
    .setLabels("全くわからない", "とてもよくわかる");

  form.addScaleItem()
    .setTitle("Q14. 図が加わったことで、パターン2より理解が進んだと感じましたか？")
    .setRequired(true)
    .setBounds(1, 5)
    .setLabels("全く感じない", "強く感じる");

  // A-5: 比較
  form.addPageBreakItem().setTitle("題材A 3パターンの比較");

  form.addMultipleChoiceItem()
    .setTitle("Q15. 3つの中で、最も理解しやすかったのはどれですか？")
    .setRequired(true)
    .setChoiceValues([
      "パターン1（自然文リスト）",
      "パターン2（階層化）",
      "パターン3（階層＋図解）",
      "差は感じなかった"
    ]);

  form.addMultipleChoiceItem()
    .setTitle("Q16. 3つの中で、このコードの品質に最も納得できたのはどれですか？")
    .setRequired(true)
    .setChoiceValues([
      "パターン1（自然文リスト）",
      "パターン2（階層化）",
      "パターン3（階層＋図解）",
      "差は感じなかった"
    ]);

  form.addMultipleChoiceItem()
    .setTitle("Q17. もし社内で品質情報を共有されるとしたら、最もあってほしい形式はどれですか？")
    .setRequired(true)
    .setChoiceValues([
      "パターン1（自然文リスト）",
      "パターン2（階層化）",
      "パターン3（階層＋図解）",
      "どれでもよい"
    ]);

  form.addParagraphTextItem()
    .setTitle("Q18. Q15〜17でその選択肢を選んだ理由を教えてください（自由記述）")
    .setRequired(false);
}

// -----------------------------------------------------------------------------
// [4] 題材B: sakura-editor 実コード
// -----------------------------------------------------------------------------

function addSection_4_Sakura(form) {
  form.addPageBreakItem()
    .setTitle("題材B: 実際のソフトウェアで試す")
    .setHelpText(
      "次に、実際のオープンソースのテキストエディタ「サクラエディタ」で、" +
      "メールアドレスかどうかを判定する処理の一部を題材にします。" +
      "業務で使われる実コードに、これまで見ていただいた形式をそのまま適用できるか確認します。\n\n" +
      "テスト要求（最も好評だった形式で提示）:\n\n" +
      "TR-B1: 先頭がドット（.）の場合は無効と判定する\n" +
      "TR-B2: @ が見つからない場合は無効と判定する\n" +
      "TR-B3: ドメイン部にドットが1つもない場合は無効と判定する\n" +
      "TR-B4: 標準的なメールアドレス（a@b.cc）は有効と判定する\n" +
      "TR-B5: 空文字列を渡してもクラッシュせず無効と判定する\n" +
      "TR-B6: @ だけの文字列は無効と判定する"
    );

  form.addScaleItem()
    .setTitle("Q19. 実コードの題材Bの内容は、どの程度理解できましたか？")
    .setRequired(true)
    .setBounds(1, 5)
    .setLabels("全くわからない", "とてもよくわかる");

  form.addMultipleChoiceItem()
    .setTitle("Q20. 題材A（リバーシ）と比べて、題材B（実コード）の理解度はどう変化しましたか？")
    .setRequired(true)
    .setChoiceValues([
      "大きく下がった", "下がった", "変わらない", "上がった"
    ]);

  form.addMultipleChoiceItem()
    .setTitle("Q21. 題材Bでも、あなたが最も良いと感じた形式は同じでしたか？")
    .setRequired(true)
    .setChoiceValues([
      "はい（題材Aと同じ形式が良かった）",
      "いいえ（別の形式の方が良かった）",
      "どちらとも言えない"
    ]);
}

// -----------------------------------------------------------------------------
// [5] 総合評価
// -----------------------------------------------------------------------------

function addSection_5_Overall(form) {
  form.addPageBreakItem().setTitle("最後に — 全体について");

  form.addScaleItem()
    .setTitle("Q22. テスト要求を非エンジニアが読めることは、現場にとって役に立つと思いますか？")
    .setRequired(true)
    .setBounds(1, 5)
    .setLabels("全く思わない", "強く思う");

  form.addScaleItem()
    .setTitle("Q23. あなたの現場で使いたいと思いますか？")
    .setRequired(true)
    .setBounds(1, 5)
    .setLabels("全く思わない", "ぜひ使いたい");

  form.addParagraphTextItem()
    .setTitle("Q24. この仕組みに期待すること、不安なこと、改善してほしいことを自由にお書きください")
    .setRequired(false);

  form.addMultipleChoiceItem()
    .setTitle("Q25. 後日、結果報告や続きのインタビューにご協力いただけますか？（任意）")
    .setRequired(false)
    .setChoiceValues([
      "はい（連絡可）",
      "いいえ"
    ]);

  form.addTextItem()
    .setTitle("Q25-補足. ご協力可の方は連絡先メールアドレスをご記入ください（任意）")
    .setRequired(false);

  form.addPageBreakItem().setTitle("ご協力ありがとうございました");
}
