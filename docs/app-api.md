# c-send アプリAPI設計

## 目的

GUI操作に依存せず、c-sendのAPI処理を機能テスト・自動化するための外部操作入口を定義する。これはGemini APIの仕様書ではなく、c-sendアプリ自身の操作APIである。

## CLI形式

```text
csend.exe /api <operation> [argument]
```

コマンドは単独プロセスとして実行し、処理完了後に終了する。成功したレスポンスはクリップボードへ格納する。APIキーはコマンドラインに渡さず、Windows Credential Managerから取得する。

## 操作

| 操作 | 引数 | 内容 |
| --- | --- | --- |
| `key-status` | なし | APIキー登録状態を終了コードで返す |
| `ping` | なし | 固定プロンプトで最小疎通テストを行う |
| `request` | プロンプト | 指定プロンプトをAPIへ送信する |
| `clipboard` | プロンプト | `{{clipboard}}` / `{{input}}` を現在のクリップボードで展開して送信する |

## 終了コード

| コード | 意味 |
| ---: | --- |
| `0` | 成功 |
| `2` | 操作名または引数が不正 |
| `3` | APIキー未登録 |
| `4` | クリップボード読み取り失敗 |
| `5` | レスポンスのクリップボード格納失敗 |
| `6` | API設定を選択できない（設定なし、または複数設定で選択指定なし） |
| `10` | API通信失敗 |

## 例

```powershell
csend.exe /api key-status
csend.exe /api ping
csend.exe /api request "Reply with exactly: API_TEST_OK"
csend.exe /api clipboard "次の文章を添削してください。`n`n{{clipboard}}"
if ($LASTEXITCODE -eq 0) { Get-Clipboard }
```

## 共通仕様

- API実装はGUIのAPI項目と同じ汎用通信処理を使用する。
- `CSEND_API_CONFIG` 環境変数にJSONファイルの絶対パスを指定した場合は、その設定と先頭Actionを使用する。これは自動テストや外部自動化で設定を明示するための入口である。
- `CSEND_API_CONFIG` が未指定の場合は、有効な`api/*.json`が1件だけならその設定と先頭Actionを使用する。0件または複数件の場合は推測で選択せず、終了コード`6`で終了する。
- CLIはGeminiなど特定サービスの設定ファイル名、エンドポイント、モデル、リクエスト形式を固定しない。
- 基本URL、相対URL、メソッド、タイムアウト、認証方式は設定JSONから読み込む。
- `request` JSONがある場合は`{{value}}`へ入力を設定して生成する。`request`を省略したActionはボディなしで実行する。
- レスポンス本文は `response` JSONの`{{value}}`位置から抽出する。
- 認証は `none`、`api-key-header`、`api-key-query`、`bearer`、`basic`に対応する。
- 認証以外の固定ヘッダーは`keyConfig.staticHeader.<ヘッダー名>`から追加する。
- タイムアウトは120秒、自動リトライは行わない。
- APIキーをログ、設定ファイル、コマンドライン引数へ出力しない。
- CLIのレスポンス表示UIは持たず、クリップボードへ格納する。
- `ping`、`request`、`clipboard` は自動テストの基本操作として維持する。
- 有効な`api/*.json`が存在しない場合は、API項目とAPIモード選択をUIに表示しない。
- API設定がない場合でも、通常のテンプレート・定型文・クリップボード機能は利用できる。

## Mockテスト

GitHub Actionsなど外部APIキーを使わない自動テストでは、環境変数 `CSEND_API_MOCK` を設定する。

| 値 | 動作 |
| --- | --- |
| `success` | 設定JSONのresponseパターンへ`API_TEST_OK`を設定した成功レスポンスを返す |
| `malformed` | JSON形式不正レスポンスを返す |
| `http401` | 認証エラーを返す |
| `timeout` | タイムアウトを返す |

MockモードではCredential Managerや外部APIを使用しない。実APIの疎通確認は手動または限定したワークフローで行う。

## API設定例

`api/gemini.json` はAPI固有の定義であり、通信実装にはGemini固有のリクエストJSONやレスポンスパスを持たせない。

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

## 将来の起動中アプリ操作

外部アプリからの最小連携として、将来 `/add "text"` で既定のテキスト項目へ文字を追加する案を検討する。これは未実装であり、現行の安定したCLI仕様には含めない。自動送信や複雑な管理APIは採用せず、詳細な方針は [AI連携方針・拡張設計](ai-integration.md) を参照する。

起動中のGUIへ操作を渡す必要が生じた場合は、既存の単一起動機構に `WM_COPYDATA` 等のIPCを追加する。その場合も本CLIの操作名と終了コードを基本仕様として引き継ぐ。
