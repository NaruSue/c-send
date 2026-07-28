# Gemini API

この文書は、c-send ネイティブ版で最初に対応する Gemini API の調査結果をまとめたものです。

## 対応方針

- 最初の対応サービスは Gemini API とする。
- 初期実装では、テキストを送信して応答全体を受け取る `generateContent` を使用する。
- ストリーミング、画像・音声・動画、Function calling、Interactions API などは初期実装の対象外とし、後から action として追加できる構造を検討する。
- モデル名はコードに固定せず、ネイティブ版の設定ファイルに記載する。初期モデルは `gemini-3.5-flash-lite` とする。
- API キーは設定ファイルへ保存せず、Windows の OS 管理機構に保存する。保存先の具体的な API は別途確定する。

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

初期実装では `contents` のテキスト送信を必須とし、`systemInstruction` と `generationConfig` は action のリクエストテンプレートから指定できる拡張項目として扱う。

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

c-send が結果本文として取り出す基本位置は、最初の候補に含まれる次の `text` とする。

```text
candidates[0].content.parts[*].text
```

複数の text パートがある場合は、順番を維持して連結する。`candidates` が空、または text が存在しない場合は、成功扱いにせず結果リストへエラー内容を保存する。

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

## 未確定事項

- 設定ファイルの形式と保存場所
- モデル設定のキー名（初期モデルは `gemini-3.5-flash-lite`）
- Windows での API キー保存 API（Credential Manager、DPAPI 等）の選択
- API リスト、API 詳細、API action 詳細画面の具体的な項目構造
- action のリクエスト・レスポンスをどの粒度でテンプレート化するか
- タイムアウト値、再試行回数、指数バックオフの間隔
- `systemInstruction`、`generationConfig` などを画面から編集可能にする時期

## 参考資料（公式）

- [Gemini API reference](https://ai.google.dev/api)
- [Generating content / models.generateContent](https://ai.google.dev/api/generate-content)
- [Text generation](https://ai.google.dev/gemini-api/docs/generate-content/text-generation)
- [Using Gemini API keys](https://ai.google.dev/gemini-api/docs/api-key)
- [Models API](https://ai.google.dev/api/models)

この文書は 2026-07-28 時点の公式資料をもとにした調査メモであり、API の更新時には内容を再確認する。
