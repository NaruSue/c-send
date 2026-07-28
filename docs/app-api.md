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
- タイムアウトは120秒、自動リトライは行わない。
- APIキーをログ、設定ファイル、コマンドライン引数へ出力しない。
- CLIのレスポンス表示UIは持たず、クリップボードへ格納する。
- `ping`、`request`、`clipboard` は自動テストの基本操作として維持する。

## 将来の起動中アプリ操作

起動中のGUIへ操作を渡す必要が生じた場合は、既存の単一起動機構に `WM_COPYDATA` 等のIPCを追加する。その場合も本CLIの操作名と終了コードを基本仕様として引き継ぐ。
