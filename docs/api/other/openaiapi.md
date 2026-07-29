# OpenAI API key の取得と c-send への登録

最終確認日: 2026-07-28

対象設定は`api/openai.json`です。これはResponses APIへ本文を送るサンプルです。

## 公式資料

- [OpenAI API Quickstart](https://platform.openai.com/docs/quickstart/make-your-first-api-request)
- [OpenAI API keys](https://platform.openai.com/api-keys)
- [Responses API](https://platform.openai.com/docs/api-reference/responses)

画面名、料金、利用可能モデルは変更されるため、実際の利用前には公式資料を確認してください。

## 登録手順

1. OpenAI Platformへログインし、[API keys](https://platform.openai.com/api-keys)を開く。
2. 画面の案内に従って新しいAPI keyを作成し、表示時に安全な場所へコピーする。キーをJSON、Git、チャットへ貼り付けない。
3. c-sendのシステムメニューから「API」を開き、「OpenAI」の詳細を開く。
4. クレデンシャルIDが`openai-responses`であることを確認し、「API token」欄へキーを貼り付けて保存する。
5. API項目を追加し、OpenAI / Responses を生成を選ぶ。本文には`{{clipboard}}`を使ったプロンプトを設定する。

サンプルは`gpt-5-mini`をrequest JSONへ記載している。利用可能なモデルはアカウントによって異なるため、公式モデル資料に従って必要ならAPI ActionのRequest JSONだけを変更する。

## AIへの依頼文

```text
OpenAI公式資料と添付したc-sendのdocs/api/config-spec.md、api/openai.jsonを読んでください。
私の利用目的に適したResponses APIのモデルとrequest JSONを提案し、既存のopenai.jsonを更新してください。
API keyの実値は質問・出力せず、取得、c-sendへの登録、短い動作確認までを日本語で案内してください。
```
