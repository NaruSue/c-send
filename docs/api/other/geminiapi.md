# Gemini API key の取得と c-send への登録

この文書は、Gemini APIを初めて利用する人が、Google AI StudioでAPI keyを取得し、c-sendネイティブ版へ安全に登録するまでの手順です。

最終確認日: 2026-07-28

## 用語

Googleの公式資料では、Gemini APIの認証情報を「API key」と呼びます。c-sendの画面にある「API token」欄には、このGemini API keyを入力します。

Geminiとの会話だけでAPI keyが自動発行されるわけではありません。利用者本人がGoogleアカウントでGoogle AI Studioへログインし、画面上で作成する必要があります。

## 公式資料

- [Gemini API keyの公式ガイド](https://ai.google.dev/gemini-api/docs/api-key)
- [Google AI StudioのAPI Keys画面](https://aistudio.google.com/apikey)
- [Gemini APIの公式リファレンス](https://ai.google.dev/api)

画面名やキーの種類は変更される可能性があります。この文書と実際の画面が異なる場合は、上記のGoogle公式ガイドを優先してください。

## 1. Gemini API keyを取得する

1. [Google AI StudioのAPI Keys画面](https://aistudio.google.com/apikey)を開きます。
2. Gemini APIを利用するGoogleアカウントでログインします。
3. 初回利用時は、表示された利用規約を確認して同意します。
4. API Keys画面を開きます。
5. 初めて利用するアカウントでは、既定のGoogle CloudプロジェクトとAPI keyが自動作成される場合があります。表示済みのキーを利用する場合は、そのキーを選択してコピーします。
6. 新しいキーを作る場合は「Create API key」を選択します。
7. 使用するGoogle Cloudプロジェクトを選び、画面の案内に従ってAPI keyを作成します。
8. 作成されたAPI keyをコピーし、パスワードと同様に安全に扱います。

既存のGoogle Cloudプロジェクトが一覧にない場合は、Google AI Studioの左側にあるDashboardからProjectsを開き、「Import projects」で対象プロジェクトを追加してからAPI Keys画面へ戻ります。

Google公式資料では、Google AI Studioで新しく作成するキーはAuth keyとして作成されます。古いStandard keyを使用している場合は、公式ガイドの移行案内を確認してください。

無料枠、利用可能モデル、レート制限、請求条件はGoogleアカウント、地域、プロジェクトおよびGoogle側の提供条件によって変わる可能性があります。キーを作成できることと、すべての利用が無料であることは同じではありません。API Keys、Usage、Billingの各画面を確認してください。

## 2. c-sendへAPI keyを登録する

1. `csend.exe`と同じ場所に`api`フォルダとGeminiのAPI設定JSONがあることを確認します。
2. c-sendを起動します。
3. ウィンドウのシステムメニューから「API」を開きます。
4. APIリストで「Gemini」を選び、「詳細」を開きます。
5. 認証タイプが`api-key-header`になっていることを確認します。
6. クレデンシャルIDを確認します。標準サンプルでは`gemini-v1beta`です。同じ認証情報を共有する設定では、同じIDを使用できます。
7. 「API token」欄へ、Google AI StudioでコピーしたGemini API keyを貼り付けます。
8. 「保存」を押します。
9. Geminiを使用するAPI項目の実行不可表示が解除されたことを確認します。
10. 機密情報を含まない短い文章で動作確認します。

c-sendはAPI keyをAPI設定JSONや`setting.ini`へ保存せず、Windows Credential Managerへ保存します。API keyを`api/gemini.json`へ直接書かないでください。

## 3. API keyを安全に扱う

- API keyをGitHub、設定JSON、サンプル、スクリーンショット、チャット、メールへ貼り付けないでください。
- Geminiや他のAIへ手順を質問するときも、実際のAPI keyは送らないでください。
- API keyが漏えいした可能性がある場合は、Google AI StudioまたはGoogle Cloud Consoleで新しいキーを作成し、c-sendへ登録し直した後、古いキーを無効化または削除してください。
- 使用量と請求状況をGoogle AI StudioまたはGoogle Cloud Consoleで定期的に確認してください。
- 組織管理のGoogle Cloudプロジェクトで「Create API key」が使用できない場合は、プロジェクト管理者へ必要な権限を確認してください。

## 4. Geminiに取得手順を案内してもらうプロンプト

次のプロンプトをGeminiへ送ると、現在のGoogle公式資料を確認しながら、API keyの取得とc-sendへの登録を対話形式で案内させることができます。

```text
私はWindows版c-sendでGemini APIを利用したいです。
Gemini API keyをまだ持っていないため、取得からc-sendへの登録まで案内してください。

次の条件を守ってください。

1. Google AI for DevelopersとGoogle AI Studioの最新の公式資料だけを根拠にしてください。
2. 最初に、現在の公式なAPI key作成ページへのリンクを示してください。
3. Googleアカウントへのログイン、利用規約への同意、Google Cloudプロジェクトの選択またはインポート、API keyの作成とコピーまでを、画面の順番に説明してください。
4. 実際の画面名が公式資料と異なる可能性がある場合は、そのことを明記し、現在の表示に合わせて案内してください。
5. 一度にすべて説明せず、私が各手順を完了したことを確認しながら、一段階ずつ進めてください。
6. 私のAPI keyそのものを質問したり、チャットへ貼り付けるよう求めたりしないでください。
7. API keyはGit、JSON、設定ファイル、チャットへ保存しないよう注意してください。
8. 無料枠、請求、利用制限は変更される可能性があるため、現在のUsageとBillingの確認方法も説明してください。
9. キー取得後は、c-sendで次の順に登録するよう案内してください。
   - c-sendのシステムメニューから「API」を開く
   - 「Gemini」の詳細画面を開く
   - 認証タイプが「api-key-header」であることを確認する
   - クレデンシャルIDを確認する
   - 「API token」欄へキーを貼り付ける
   - 「保存」を押す
10. c-sendではキーがWindows Credential Managerへ保存され、api/gemini.jsonやsetting.iniには保存されないことを説明してください。
11. 最後に、機密情報を含まない短い文章で動作確認する手順を案内してください。

まず、API key作成ページを開くところから始めてください。
```

Geminiは利用者のGoogleアカウントを代わりに操作したり、秘密のAPI keyを会話内で取得したりするものではありません。このプロンプトは、利用者自身の画面操作を一段階ずつ案内させるためのものです。

## 5. 問題が起きた場合

### Create API keyが表示されない

- 正しいGoogleアカウントでログインしているか確認します。
- 利用規約への同意が完了しているか確認します。
- Google AI StudioのProjects画面で対象プロジェクトをインポートします。
- 組織管理プロジェクトの場合は、API key作成やサービス有効化の権限を管理者へ確認します。

### c-sendで実行不可のままになる

- API詳細画面で認証タイプが`api-key-header`になっているか確認します。
- クレデンシャルIDが空でないか確認します。
- 「API token」欄へキーを入力して「保存」を押したか確認します。
- 複数のGemini設定を使用している場合は、API項目が参照する設定と登録先のクレデンシャルIDが一致しているか確認します。

### 認証エラーになる

- キーの前後へ空白や引用符が入っていないか確認します。
- Google AI Studioでキーが無効、削除済み、ブロック済みになっていないか確認します。
- Standard keyを使用している場合は、Google公式ガイドに従って新しいAuth keyへの移行を確認します。
- 漏えいが疑われるキーは再利用せず、新しいキーへ交換します。

## 6. 他のAPIへ展開する場合

今後OpenAI APIなどへ対応するときも、`docs/api/other/<サービス名>.md`に同じ構成の文書を追加します。

1. 公式の認証情報作成ページ
2. アカウント、プロジェクト、請求に関する前提
3. 認証情報の取得手順
4. c-sendへの登録手順
5. 秘密情報の安全な管理
6. AIに対話形式で案内させるプロンプト
7. よくあるエラーと解決方法

サービスごとに用語、認証方式、クレデンシャルID、設定key定義が異なるため、公式資料を確認したうえで個別文書を作成します。
