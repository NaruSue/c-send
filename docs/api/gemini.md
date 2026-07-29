# Gemini API

この文書は、c-send ネイティブ版で最初に対応する Gemini API の調査結果をまとめたものです。

Gemini API keyの取得とc-sendへの登録方法は、[Gemini API keyの取得手順](other/geminiapi.md)を参照してください。

## 対応方針

- 最初の対応サービスは Gemini API とする。
- 初期実装では、テキストを送信して応答全体を受け取る `generateContent` を使用する。
- ストリーミング、画像・音声・動画、Function calling、Interactions API などは初期実装の対象外とし、後から action として追加できる構造を検討する。
- モデル名はコードに固定せず、Action の相対 URL に記載する。初期モデルは `gemini-3.5-flash-lite` とする。
- API キーは設定ファイルへ保存せず、Windows Credential Managerへ保存する。

Gemini API には `generateContent` のほか、ストリーミング、Live API、Batch、Embeddings などがある。初期実装は同期的な `generateContent` に限定する。

## 共通情報

| 項目 | 内容 |
|---|---|
| サービス | Gemini API |
| API バージョン | `v1beta`（現行公式 REST 例に基づく。将来変更の可能性あり） |
| ベース URL | `https://generativelanguage.googleapis.com/v1beta` |
| 認証 | `x-goog-api-key` ヘッダー |
| Content-Type | `application/json` |
| 初期通信方式 | HTTPS / REST / JSON |

## Action: generateContent

### 概要

指定したモデルに `contents` を送信し、生成完了後にレスポンス全体を返す。

### エンドポイント

```text
POST https://generativelanguage.googleapis.com/v1beta/models/{model}:generateContent
```

`{model}` には、設定ファイルに記載したモデル名を指定する。例:

```text
models/gemini-3.5-flash-lite:generateContent
```

実際の設定値には `gemini-3.5-flash-lite` のようなモデル ID を保存し、URL 生成時に `models/` と `:generateContent` を付加する。

### HTTP ヘッダー

```http
Content-Type: application/json
x-goog-api-key: <API_KEY>
```

API キーを URL のクエリ文字列やリクエスト JSON に含めない。ログやエラー表示にも API キーを出力しない。

### リクエスト JSON（最小構成）

```json
{
  "contents": [
    {
      "parts": [
        {
          "text": "ここにプロンプトを指定する"
        }
      ]
    }
  ]
}
```

### リクエスト JSON（主な項目）

```json
{
  "systemInstruction": {
    "parts": [
      { "text": "システム指示" }
    ]
  },
  "contents": [
    {
      "role": "user",
      "parts": [
        { "text": "ユーザー入力" }
      ]
    }
  ],
  "generationConfig": {
    "temperature": 0.7,
    "maxOutputTokens": 1024
  }
}
```

初期実装では `contents` のテキスト送信を必須とし、入力位置を文字列値 `{{value}}` で定義する。`systemInstruction` と `generationConfig` も Action の整形済み Request JSON テキストへ追加・編集できる。

```json
{
  "contents": [
    {
      "parts": [
        {
          "text": "{{value}}"
        }
      ]
    }
  ]
}
```

### レスポンス JSON（代表構成）

```json
{
  "candidates": [
    {
      "content": {
        "role": "model",
        "parts": [
          {
            "text": "モデルが生成した応答"
          }
        ]
      },
      "finishReason": "STOP",
      "index": 0,
      "safetyRatings": []
    }
  ],
  "usageMetadata": {
    "promptTokenCount": 0,
    "candidatesTokenCount": 0,
    "totalTokenCount": 0
  },
  "modelVersion": "モデルのバージョン"
}
```

c-send が結果本文として取り出す位置は、Response JSON の文字列値 `{{value}}` で定義する。

```json
{
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
```

配列パターンを実配列の各要素へ適用し、複数の text パートがある場合は順番を維持して連結する。`candidates` が空、または `{{value}}` に対応する値が存在しない場合は、成功扱いにせず結果形式エラーを保存する。

### エラー時の扱い

- HTTP ステータス、Gemini のエラーコード、メッセージを結果リストへ保存する。
- API キーそのものはエラー表示・ログ・結果保存に含めない。
- 認証エラー、権限エラー、リクエスト不正は自動再試行しない。
- 一時的な通信エラー、レート制限、サーバーエラーも初期実装では自動再試行しない。
- 将来的にエラー内容を判別し、再試行可能な場合だけ再試行する。

## 将来の候補 Action

| Action | 概要 | 初期実装 |
|---|---|---|
| `streamGenerateContent` | SSE で生成途中の応答を受信 | 対象外 |
| `models.list` | 利用可能なモデル一覧を取得 | 対象外。モデルは設定ファイルで指定 |
| `models.get` | モデルのメタデータを取得 | 対象外 |
| `embedContent` | テキストから埋め込みベクトルを生成 | 対象外 |
| Files API | 大きな画像・音声・動画・PDF 等をアップロード | 対象外 |
| Interactions API | 状態管理を含む対話・エージェント向け API | 対象外 |

## 確定済みの設定方針

- 設定ファイルは実行ファイルと同じ場所の`api`ディレクトリへ保存する。
- 共通形式は[API設定JSON仕様](config-spec.md)に従う。
- APIキーはWindows Credential Managerへ保存する。
- API詳細・Action詳細の画面項目は[API機能設計書](design.md)に従う。
- Request／Response JSONは画面で整形済みJSONテキストとして表示・編集し、`{{value}}`で入力・出力位置を定義する。
- タイムアウト初期値は120秒、自動再試行は行わない。

## 参考資料（公式）

- [Gemini API reference](https://ai.google.dev/api)
- [Generating content / models.generateContent](https://ai.google.dev/api/generate-content)
- [Text generation](https://ai.google.dev/gemini-api/docs/generate-content/text-generation)
- [Using Gemini API keys](https://ai.google.dev/gemini-api/docs/api-key)
- [Models API](https://ai.google.dev/api/models)

この文書は 2026-07-28 時点の公式資料をもとにした調査メモであり、API の更新時には内容を再確認する。
