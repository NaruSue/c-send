# API認証方式仕様

## 目的

API公式ドキュメントと本仕様書をAIへ渡し、c-send用のAPI設定JSONを生成・検証できるようにする。

認証方式の処理自体はアプリケーション側に実装する。認証方式ごとのヘッダー名、クエリ名などはAPI設定JSONで変更できるようにする。

## 共通ルール

- APIキー、トークン、ユーザー名、パスワードをJSONへ直接記載しない。
- 機密情報はWindows Credential Managerへ保存する。
- 認証情報は、API設定で明示する`credentialId`を保存・参照単位に使用する。
- `credentialId`は設定画面で編集可能とする。同じ認証情報を共有するAPI設定では同じIDを、分離する場合は別のIDを指定する。
- 新形式ではアプリが`C-Send/API/<credentialId>/<field>`形式の対象名を生成する。
- 認証情報はURL、リクエストJSON、ログ、エラーメッセージへ出力しない。
- HTTPSを優先し、HTTPは明示的に必要な場合だけ許可する。
- 認証方式が不明な場合、AIは推測せず未確定として扱う。

## 対応する認証方式

### `none`

認証を行わない公開APIで使用する。

```json
{
  "authType": "none",
  "keyConfig": {}
}
```

### `api-key-header`

APIキーをHTTPヘッダーへ付与する。Geminiの`x-goog-api-key`などが該当する。

```json
{
  "authType": "api-key-header",
  "credentialId": "example-api-v2",
  "keyConfig": {
    "headerName": "x-api-key",
    "prefix": ""
  }
}
```

`prefix`が必要な場合は値と区切り文字を含めて指定する。

### `api-key-query`

APIキーをURLクエリへ付与する。API仕様上、ヘッダーを利用できない場合だけ使用する。

```json
{
  "authType": "api-key-query",
  "credentialId": "example-api-v2",
  "keyConfig": {
    "queryName": "api_key",
    "prefix": ""
  }
}
```

URLへ機密情報が含まれるため、ログ、履歴、エラー表示へ完全なURLを出力してはいけない。

### `bearer`

Bearerトークンをヘッダーへ付与する。

```json
{
  "authType": "bearer",
  "credentialId": "example-api-v2",
  "keyConfig": {
    "headerName": "Authorization",
    "prefix": "Bearer "
  }
}
```

`headerName`の既定値は`Authorization`、`prefix`の既定値は`Bearer `とする。

### `basic`

ユーザー名とパスワードをBasic認証へ変換して付与する。

```json
{
  "authType": "basic",
  "credentialId": "example-api-v2",
  "keyConfig": {}
}
```

ユーザー名・パスワードはBase64化しても機密情報として扱う。Base64は暗号化ではない。

## 認証設定UI

API詳細画面では認証方式を選択式にし、選択に応じて認証情報入力欄を切り替える。

### APIキー方式

`api-key-header`または`api-key-query`を選択した場合は次を表示する。

- API token
- クレデンシャルID
- 設定key定義

設定key定義には、ヘッダー方式では`headerName`と`prefix`、クエリ方式では`queryName`と`prefix`を保存する。

### Bearer方式

`bearer`を選択した場合は次を表示する。

- API token
- クレデンシャルID
- 設定key定義

設定key定義には`headerName`と`prefix`を保存する。

### Basic方式

`basic`を選択した場合は次を表示する。

- ID
- PASS
- クレデンシャルID
- 設定key定義

1つの`credentialId`を名前空間とし、IDとPASSを別々のCredential Managerエントリへ保存する。

### 設定key定義

認証ヘッダー名、クエリ名、プレフィックス、認証以外の固定ヘッダーなどは`keyConfig` JSONへ保存する。画面ではベタのJSONを表示せず、「設定項目／値」の一覧へパースして表示・編集する。

既知のキーは人間向けの表示名へ変換する。`staticHeader.<ヘッダー名>`は`固定ヘッダー: <ヘッダー名>`と表示する。未知のキーも削除せず、元のキー名で表示して編集可能にする。固定ヘッダーへtokenなどの秘密情報を書かず、認証方式とCredential Managerを使用する。

| JSONキー | 画面表示 |
|---|---|
| `headerName` | ヘッダー名 |
| `queryName` | クエリ名 |
| `prefix` | プレフィックス |

### UI切り替えルール

- 認証方式を変更したら、不要な入力欄を非表示にする。
- 非表示になった方式の値をリクエストへ送信しない。
- 保存時に必要な設定が空の場合はエラーにする。
- 既存の認証情報は、方式を変更してもCredential Managerから削除しない。
- 保存済みの秘密情報を入力欄へ復元しない。
- 認証情報入力欄が空の場合は既存値を維持し、入力された場合だけ更新する。
- 既存API設定の`credentialId`も編集可能とする。
- `credentialId`を変更した場合、変更後のIDに保存済みの認証情報を参照する。
- 変更前のIDに保存済みの認証情報は、自動移行・自動削除しない。
- API設定JSONをAI生成する場合も認証情報そのものは生成しない。
- APIキー・Bearerではtoken、Basic認証ではIDとPASSの登録状態を実行前に確認する。
- 必須資格情報が未登録のAPI項目は実行不可とし、不足項目と設定画面への導線を表示する。

## API設定JSONへの組み込み例

```json
{
  "id": "gemini",
  "name": "Gemini",
  "baseUrl": "https://generativelanguage.googleapis.com",
  "timeoutMs": 120000,
  "authType": "api-key-header",
  "credentialId": "gemini-v1beta",
  "keyConfig": {
    "headerName": "x-goog-api-key",
    "prefix": ""
  },
  "actions": [
    {
      "id": "generateContent",
      "name": "Generate content",
      "url": "/v1beta/models/gemini-3.5-flash-lite:generateContent",
      "method": "POST",
      "description": "入力文章から応答を生成する",
      "request": {
        "contents": [
          {
            "parts": [
              {
                "text": "{{value}}"
              }
            ]
          }
        ]
      },
      "response": {
        "candidates": [
          {
            "content": {
              "parts": [
                {
                  "text": "{{value}}"
                }
              ]
            }
          }
        ]
      }
    }
  ]
}
```

共通JSON形式の詳細は[API設定JSON仕様](config-spec.md)を参照する。

## API設定IDによる分離例

同じドメインでもAPIバージョンが異なる場合は認証情報を分離する。

```text
example-api-v1 -> C-Send/API/example-api-v1/token
example-api-v2 -> C-Send/API/example-api-v2/token
```

同じAPIを複数アカウントで利用する場合は、別のAPI設定とクレデンシャルIDを作成する。

```text
example-api-v2-customer-a
example-api-v2-customer-b
```

Action単位のURLやHTTPメソッドは同じAPI定義内で認証情報を共有する。別認証が必要な場合は別API定義にする。

## AIによる設定生成時の指示

AIへ公式APIドキュメント、本仕様書、[API設定JSON仕様](config-spec.md)、既存サンプルを渡す場合は、次の条件を指示する。

1. 公式ドキュメントに記載された認証方式だけを使用する。
2. 対応済みの`authType`から選択する。
3. 認証情報そのものは出力しない。
4. 不明な項目は推測せず、`TODO`または未確定項目として示す。
5. JSON以外の説明文を生成結果へ混ぜない。
6. `request`、`response`、`method`、`baseUrl`、`url`を検証する。
7. 成功、認証失敗、レスポンス形式不正のMockテスト例も併せて生成する。

## 今後の認証方式

OAuth 2.0、HMAC署名、AWS Signatureなどは、認証フローや署名処理が複雑なため本仕様の対象外とする。必要になった時点で認証アダプターと設定項目を追加する。
