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

- API実装はGUIのAPI項目と同じGemini通信処理を使用する。
- エンドポイント、メソッド、モデル、ポート、TLS、タイムアウトはアプリ配下の `api/gemini.json` から読み込む。
- リクエスト本文は `requestTemplate` の `{{prompt_json}}` を置換して生成する。
- レスポンス本文は `responsePath` で指定されたJSONパスの末端値を抽出する。
- 認証は `authType` で選択する。現在は `credential-header` のみ対応し、Credential Managerの対象名とヘッダー名も設定ファイルから読み込む。
- タイムアウトは120秒、自動リトライは行わない。
- APIキーをログ、設定ファイル、コマンドライン引数へ出力しない。
- CLIのレスポンス表示UIは持たず、クリップボードへ格納する。
- `ping`、`request`、`clipboard` は自動テストの基本操作として維持する。
- `api/gemini.json` が存在しない、または読み込めない場合は、API項目とAPIモード選択をUIに表示しない。
- API設定がない場合でも、通常のテンプレート・定型文・クリップボード機能は利用できる。

## Mockテスト

GitHub Actionsなど外部APIキーを使わない自動テストでは、環境変数 `CSEND_GEMINI_MOCK` を設定する。

| 値 | 動作 |
| --- | --- |
| `success` | `API_TEST_OK` を含む成功レスポンスを返す |
| `malformed` | JSON形式不正レスポンスを返す |
| `http401` | 認証エラーを返す |
| `timeout` | タイムアウトを返す |

MockモードではCredential Managerや外部Gemini APIを使用しない。実APIの疎通確認は手動または限定したワークフローで行う。

## API設定例

`api/gemini.json` はAPI固有の定義であり、通信実装にはGemini固有のリクエストJSONやレスポンスパスを持たせない。

```json
{
  "endpoint": "generativelanguage.googleapis.com",
  "path": "/v1beta/models/{model}:generateContent",
  "method": "POST",
  "model": "gemini-3.5-flash-lite",
  "timeoutMs": 120000,
  "authType": "credential-header",
  "credentialTarget": "C-Send/GeminiAPIKey",
  "authHeader": "x-goog-api-key",
  "requestTemplate": "{\"contents\":[{\"parts\":[{\"text\":{{prompt_json}}}]}]}",
  "responsePath": "candidates[].content.parts[].text"
}
```

## 将来の起動中アプリ操作

起動中のGUIへ操作を渡す必要が生じた場合は、既存の単一起動機構に `WM_COPYDATA` 等のIPCを追加する。その場合も本CLIの操作名と終了コードを基本仕様として引き継ぐ。
