# Geminiと連携してみる

c-sendには、Gemini APIを使って文章を要約・添削するサンプルが用意されています。
このチュートリアルでは、API設定JSONを自分で作らずに、Geminiとの連携を試します。

通常の定型文・コマンド機能だけを使う場合、この手順は必要ありません。

## 1. Gemini API keyを取得する

[Google AI StudioのAPI Keys画面](https://aistudio.google.com/apikey)を開き、Gemini API keyを作成してコピーします。

詳しい取得手順は、[Gemini API keyの取得と登録](./other/geminiapi.md)を参照してください。

API keyは他人へ送ったり、JSONやチャットへ貼り付けたりしないでください。

## 2. c-sendへAPI keyを登録する

1. `csend.exe`を起動します。
2. システムメニューから「API」を開きます。
3. 「Gemini」を選び、「詳細」を開きます。
4. 「API token」欄へ、取得したAPI keyを貼り付けます。
5. 「保存」を押します。

c-sendはAPI keyをWindows Credential Managerへ保存します。`api/gemini.json`や`setting.ini`には保存されません。

## 3. Geminiのサンプルカテゴリを追加する

1. システムメニューからカテゴリ設定を開きます。
2. 一覧の末尾にある「（追加）」を選びます。
3. カテゴリ名に`Gemini API`と入力します。
4. データファイルのパスに次を入力します。

```text
samples\jp\Gemini API.json
```

5. 保存します。

## 4. 実行する

1. クリップボードへ短い文章をコピーします。
2. `Gemini API`カテゴリを開きます。
3. 「クリップボードを要約」または「文章を添削」を選びます。
4. Geminiの結果が返り、クリップボードへコピーされたことを確認します。

最初は、機密情報を含まない短い文章で試してください。

## うまくいかない場合

- GeminiがAPI一覧にない場合は、`csend.exe`と同じ場所の`api\gemini.json`を確認して再起動します。
- 実行不可のままの場合は、API詳細画面でAPI tokenを保存し直します。
- 認証エラーの場合は、Google AI StudioでAPI keyが有効か確認します。
- 利用料金や上限は、Google AI StudioのUsage・Billing画面を確認してください。

API設定を自分で追加したい場合は、[API利用者向け導入手順](./getting-started.md)を参照してください。
