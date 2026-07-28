# 新しいAPIサービスを追加するための資料テンプレート

OpenAI互換APIや中国系を含む新しいサービスは、`docs/api/config-spec.md`と公式API資料をAIへ渡して設定JSONを作成する。JSONを直接作る前に、次を公式資料で確認する。

1. 基本URL、Action URL、HTTP method
2. 認証方式（none、API key header/query、Bearer、Basic）
3. 認証以外に必要な固定ヘッダー
4. JSON request内の入力位置とJSON response内の結果テキスト位置
5. 利用するモデル名、APIバージョン、料金、レート制限

対応済みの範囲はJSON HTTP API、固定ヘッダー、JSONボディ、ボディなしAction、JSONレスポンス抽出である。OAuthの対話ログイン、署名認証、multipart upload、SSEストリーミング、WebSocket、CLI呼び出しが必要なサービスは、JSONだけでは追加できない。必要なら公式仕様とともにc-send本体の汎用拡張を依頼する。

## AIへの依頼文

```text
添付したc-sendのdocs/api/config-spec.mdと、対象APIの公式ドキュメントを読んでください。
対象APIは以下です。

- サービス名: （名前）
- 使いたい機能: （例: 文章の要約）
- 公式ドキュメント: （URLまたは添付）

まず、このAPIがc-sendの対応範囲（JSON HTTP API、none/api-key-header/api-key-query/bearer/basic、固定ヘッダー、JSONまたはボディなしリクエスト、JSONレスポンス）で実装できるかを判定してください。
対応可能な場合だけ、config-spec.mdの指定形式に従ってapi/<id>.jsonと、API key/token取得・c-send登録・動作確認の日本語手順を出力してください。
対応できない場合はJSONを推測で作らず、必要となる認証方式または通信機能を説明してください。
秘密情報の実値は質問・出力しないでください。
```
