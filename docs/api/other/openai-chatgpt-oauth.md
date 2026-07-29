# OpenAI ChatGPT OAuth（実験的）

これは、ローカルの`openai-oauth`プロキシを利用する実験的な接続です。
OpenAI公式のAPI連携ではありません。安定した利用には、通常のOpenAI API接続を推奨します。

## 導入方法

`openai-oauth`の導入と起動は、GitHubのREADMEを参照してください。

https://github.com/EvanZhouDev/openai-oauth

通常のローカル導入方法は次のとおりです。

```text
npx openai-oauth@latest
```

プロキシは通常、次のURLで待ち受けます。

```text
http://127.0.0.1:10531/v1
```

ブラウザでのログインを求められた場合は、ChatGPTのログインを完了してください。
認証情報はローカルのCodex／openai-oauth側で管理します。ChatGPTのメールアドレス、
パスワード、アクセストークン、APIキーをc-sendのJSONへ入力しないでください。

## c-sendの設定

`api/openai-chatgpt-oauth.json`を、c-sendの`api`フォルダーへ配置してAPI一覧を再読み込みしてください。
このサンプルでは、認証をローカルプロキシが処理するため`authType: none`を指定しています。

利用できるモデルと利用枠は、ChatGPTアカウントと契約プランに依存します。
モデルを変更する場合は、ローカルプロキシの`/v1/models`で表示されるモデルだけを指定してください。

## 注意事項

- このプロキシと接続設定は非公式のため、ChatGPTまたはCodexの変更後に動作しなくなる可能性があります。
- c-sendからリクエストを送る前に、プロキシを起動しておく必要があります。
- 接続先は標準では自分のPC内だけから接続できます。アクセス制御なしでネットワークへ公開しないでください。
- 自分自身のChatGPTアカウントだけを使用してください。認証情報を共有・再配布しないでください。
