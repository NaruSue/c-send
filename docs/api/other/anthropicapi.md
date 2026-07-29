# Anthropic Claude API key の取得と c-send への登録

最終確認日: 2026-07-28

対象設定は`api/anthropic.json`です。これはClaude Codeではなく、Anthropicの直接Messages APIを呼ぶサンプルです。

## 公式資料

- [Claude API overview](https://platform.claude.com/docs/en/api/overview)
- [Create a Message](https://platform.claude.com/docs/en/api/messages/create)
- [Anthropic Console](https://console.anthropic.com/)

Claude APIでは`x-api-key`に加え、`anthropic-version`と`content-type`が必要です。サンプルの`staticHeader.anthropic-version`はこの固定ヘッダーをJSONで定義する例です。

## 登録手順

1. Anthropic Consoleへログインし、公式画面の案内に従ってAPI keyを作成する。
2. 必要な請求設定、利用可能なモデル、レート制限を公式画面で確認する。
3. c-sendのシステムメニューから「API」を開き、「Anthropic Claude API」の詳細を開く。
4. クレデンシャルIDが`anthropic-messages`であることを確認し、「API token」欄へAPI keyを貼り付けて保存する。
5. API項目を追加し、Anthropic Claude API / Message を生成を選択する。

`model`と`max_tokens`はActionのRequest JSONで保持する。モデル名や利用条件が変わったときは、公式モデル資料を優先してJSONを更新する。API keyはWindows Credential Managerへ保存され、JSONには保存されない。

## Claude Codeについて

Claude CodeはCLI／エージェント製品であり、同じ設定JSONから直接呼ぶ対象ではない。Claude Code連携を追加するには、別途CLI・OAuth・実行権限を扱う専用アダプターが必要である。

## AIへの依頼文

```text
Anthropicの公式API資料、添付したc-sendのdocs/api/config-spec.md、api/anthropic.jsonを読んでください。
利用可能なClaude APIモデルとMessages API仕様に合わせて、既存の設定JSONを更新してください。
anthropic-versionを含む必須ヘッダーを確認し、API keyの実値を求めずに、キー取得、c-send登録、動作確認の手順を日本語で出力してください。
Claude Code用の設定JSONは作らず、直接Claude APIだけを対象にしてください。
```
