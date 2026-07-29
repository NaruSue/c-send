# API 設定 JSON 仕様

## 目的

API 公式仕様と本書を AI へ渡すことで、c-send で利用できる API 設定を追加・更新できるようにする。本書の JSON は保存形式であり、アプリの設定画面では通常項目をラベルと一覧で表示し、Request／Response JSONは整形済みJSONテキストとして表示・編集する。

利用方法は次の2通りとする。

1. APIとJSONの知識を持つ利用者が、本書に従って設定を直接作成・編集する。
2. 一般利用者が、本書、認証方式仕様、利用したいAPIの公式資料をAIへ渡し、設定JSONと導入手順を作成してもらう。

推奨する入口は2とする。AIの出力にはJSONだけでなく、ファイルの配置、認証情報の登録、API項目の作成、動作確認までの手順を含める。

## API 設定

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
  "actions": []
}
```

| キー | 必須 | 内容 |
|---|---:|---|
| `id` | 必須 | API 設定を参照する不変 ID |
| `name` | 必須 | 画面に表示するサービス名称 |
| `baseUrl` | 必須 | スキームを含む基本 URL |
| `timeoutMs` | 必須 | タイムアウト。既定値は `120000` |
| `authType` | 必須 | 対応済み認証タイプ |
| `credentialId` | 認証時必須 | Windows Credential Manager の保存名前空間 |
| `keyConfig` | 方式依存 | ヘッダー名、クエリ名、プレフィックス、固定ヘッダーなどの非機密設定 |
| `actions` | 必須 | Action 定義の配列 |

API token、Bearer token、Basic 認証の ID／PASS はこの JSON へ保存しない。

## 認証タイプと keyConfig

### 認証なし

```json
{
  "authType": "none",
  "keyConfig": {}
}
```

### API キー・ヘッダー

```json
{
  "authType": "api-key-header",
  "credentialId": "example-api",
  "keyConfig": {
    "headerName": "X-API-Key",
    "prefix": ""
  }
}
```

### API キー・クエリ

```json
{
  "authType": "api-key-query",
  "credentialId": "example-api",
  "keyConfig": {
    "queryName": "api_key",
    "prefix": ""
  }
}
```

### Bearer

```json
{
  "authType": "bearer",
  "credentialId": "example-api",
  "keyConfig": {
    "headerName": "Authorization",
    "prefix": "Bearer "
  }
}
```

### Basic

```json
{
  "authType": "basic",
  "credentialId": "example-api",
  "keyConfig": {}
}
```

Basic 認証では `credentialId` を名前空間として、ID と PASS を別々の Credential Manager エントリへ保存する。

### 固定ヘッダー

認証情報ではない固定ヘッダーが必要な場合は、`keyConfig` に`staticHeader.`で始まるキーを追加する。キーの`staticHeader.`より後ろがヘッダー名、値がヘッダー値となる。ヘッダー名・値に改行は指定できない。認証情報そのものはここへ書かない。

```json
{
  "keyConfig": {
    "headerName": "x-api-key",
    "prefix": "",
    "staticHeader.anthropic-version": "2023-06-01"
  }
}
```

API詳細画面では`固定ヘッダー: anthropic-version`のように表示され、他の設定keyと同様に追加・編集できる。

設定画面では `keyConfig` をベタの JSON として表示せず、キーを表示名へ変換した「設定項目／値」の一覧として表示・編集する。未知のキーも削除せず、元のキー名を表示して編集可能にする。

## Action 設定

```json
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
```

| キー | 必須 | 内容 |
|---|---:|---|
| `id` | 必須 | API 設定内で一意な Action ID |
| `name` | 必須 | Action 名 |
| `url` | 必須 | 基本 URL より後ろの相対 URL |
| `method` | 必須 | `GET`、`POST`、`PUT`、`PATCH`、`DELETE` |
| `description` | 任意 | 人間向けの説明 |
| `request` | 任意 | リクエスト JSON と `{{value}}` の設定位置。未指定ならボディなし |
| `response` | 必須 | レスポンス構造パターンと結果取得位置 |

`request` と `response` は画面でインデントと改行を付けたJSONテキストとして表示する。AIが生成・修正したJSONを直接貼り付けできるようにし、保存時にはテキストをJSONとしてパースして構文と `{{value}}` の位置を検証する。GETなどボディを送らないActionでは、Request JSON欄を空欄にする。

## 保存・読み込み時の検証

新形式のAPI設定JSONでは、必須項目の暗黙補完を行わない。`id`、`name`、`baseUrl`、正の整数の`timeoutMs`、`authType`、`keyConfig`、1件以上の`actions`が欠けている設定は無効とする。

- `authType`は`none`、`api-key-header`、`api-key-query`、`bearer`、`basic`のいずれかとする。
- 認証ありの設定では`credentialId`を必須とする。
- `api-key-header`と`bearer`では`keyConfig.headerName`を、`api-key-query`では`keyConfig.queryName`を必須とする。
- `keyConfig`の`staticHeader.`で始まる値は、空でない文字列で改行を含まないものとする。
- Actionの`method`は`GET`、`POST`、`PUT`、`PATCH`、`DELETE`のいずれかとする。
- Actionの`url`は基本URLより後ろの相対URLであり、`://`または`//`から始まる絶対URLは指定できない。

## `{{value}}` の規則

### Request

- 項目側の `value` に `{{clipboard}}` などを展開した最終入力文字列を設定する。
- `{{value}}` は JSON の文字列値として置換し、必要なエスケープをアプリ側で行う。
- Request JSONを指定する場合は最低1つ必要とする。Request JSONを省略したActionはボディを送信しない。

### Response

- `{{value}}` の位置に対応する実レスポンスの値を結果本文として取得する。
- 配列のパターン要素は実配列の各要素へ適用する。
- 複数取得時はレスポンス順に連結する。
- 取得件数が 0 の場合は結果形式エラーとする。

## AIへAPI設定JSONと導入手順を作成させるプロンプト

利用者がAIへ渡す必須資料は、原則として次の2つだけとする。

1. 本書
2. 利用したいAPIの公式ドキュメント、または公式ドキュメントのURL

利用したい機能やActionが決まっている場合は、その希望を短く追加する。本書には共通認証方式と生成プロンプトを含むため、通常は別のc-send資料を追加しなくても設定を作成できる。独自認証など本書だけでは判断できない場合に限り、[API認証方式仕様](auth-spec.md)も追加する。

c-sendはGitHubでソースを公開しているため、本書のファイル添付に代えて公開リポジトリをAIへ参照させてもよい。ただし、利用中のアプリと仕様を合わせるため、可能なら同じリリースタグの `docs/api/config-spec.md` を指定する。`master`などの最新ブランチは、利用中のリリースより新しい仕様を含む可能性がある。

```text
https://github.com/NaruSue/c-send/blob/<利用中のリリースタグ>/docs/api/config-spec.md
```

AIがGitHubのURLを参照できない場合や、リポジトリ内のファイルを正しく取得できない場合は、配布物に同梱された本書を直接添付する。

認証が必要なAPIでは、秘密情報そのものを書かせず、`authType`、`credentialId`、`keyConfig`へ必要な認証方式を反映させる。

### AIでAPI設定を追加する手順

1. 配布物の `docs/api/config-spec.md` を用意する。または、利用中の版と同じGitHubリリースタグにある同ファイルのURLを確認する。
2. 利用したいAPIの公式ドキュメントを用意する。URLをAIが参照できない場合は、公式ページをPDF、Markdown、またはテキストとして保存して添付する。
3. `docs/api/other/`に対象サービスの認証情報取得ガイドがある場合は、その文書も用意する。Geminiの場合は`docs/api/other/geminiapi.md`を使用する。
4. 利用したい機能を決める。例として「文章を翻訳したい」「天気情報を取得したい」「指定したモデルへ文章を送信したい」などを一文で整理する。
5. AIの新しいチャットを開き、本書と公式ドキュメントを添付する。URL参照に対応するAIでは、本書のGitHub URLと公式ドキュメントURLを伝える方法でもよい。
6. 次の短い依頼文を送る。本書内の詳細プロンプトはAIが読み取って使用する。

```text
添付したc-send API設定JSON仕様の「AIへAPI設定JSONと導入手順を作成させるプロンプト」に従ってください。

c-send仕様をURLで指定する場合:
https://github.com/NaruSue/c-send/blob/<利用中のリリースタグ>/docs/api/config-spec.md

対象API:
（APIサービス名）

実現したいこと:
（利用したい機能やAction）

公式ドキュメント:
（添付ファイル名または公式URL）

認証情報取得ガイド:
（docs/api/other内の文書がある場合は、添付ファイル名またはURL）
```

7. AIから不足情報を質問された場合は、利用するAction、モデル、APIバージョンなどを回答する。token、APIキー、ID、PASSの実際の値は回答しない。
8. AIが出力した設定概要で、対象API、Action、認証方式、必要な資格情報を確認する。
9. 出力されたJSONコードブロックの内容だけを、AIが提案した `api/<設定ID>.json` へUTF-8のJSONファイルとして保存する。Markdownのコードフェンスは保存しない。
10. `api`フォルダは `csend.exe` と同じフォルダへ置く。すでにc-sendを起動している場合は再起動する。
11. c-sendのシステムメニューから「API」を開き、追加したサービスとActionが正しく表示されることを確認する。
12. 認証が必要な場合は、認証情報取得ガイドまたはAPI公式資料に従って利用者自身が認証情報を取得する。実際の秘密情報はAIへ送らない。
13. API詳細画面を開き、クレデンシャルIDを確認してから、tokenまたはBasic認証のID／PASSを登録して保存する。
14. 使用するカテゴリの「（追加）」を開き、APIを選択して、追加したAPIとActionを指定する。
15. 項目本文へ、`{{clipboard}}`または`{{clipboard|代替文字列}}`を含む実行用プロンプトを入力して保存する。
16. AIが出力した動作確認手順に従い、機密情報を含まない短い入力で試す。
17. 実行不可アイコンが表示された場合は、API詳細画面で不足しているtoken、ID、PASSを登録する。通信エラーの場合は、ActionのURL、method、Request JSON、Response JSONと公式仕様を確認する。

APIの仕様変更へ追従するときは、本書、最新の公式ドキュメント、現在使用中のAPI設定JSONをAIへ渡し、「既存設定を最新仕様へ更新し、変更点と再確認手順も出力してください」と依頼する。

```text
あなたは c-send のAPI設定作成担当者です。

添付した次の資料を読み、c-sendで利用できるAPI設定JSONと、初めて使う人向けの導入手順を作成してください。

1. プロンプトと認証方式を含むc-send API設定JSON仕様（本書）
2. 利用したいAPIの公式ドキュメント、または公式ドキュメントのURL
3. 利用したい機能・エンドポイント・Action（指定がある場合）
4. 対象サービスの認証情報取得ガイド（指定がある場合）

作成規則:

- 公式ドキュメントに記載された仕様だけを使用し、存在しないURL、項目、レスポンス構造を推測で追加しないでください。
- 情報が不足して正しい設定を作れない場合は、JSONを作らず、不足している資料または確認事項を質問してください。
- 認証方式を確認し、none、api-key-header、api-key-query、bearer、basicのいずれかをauthTypeへ設定してください。
- token、APIキー、ID、PASSなどの秘密情報そのものはJSONへ絶対に記載しないでください。
- 認証が必要な場合はcredentialIdを設定し、ヘッダー名、クエリ名、prefixをkeyConfigへ正確に設定してください。
- 認証以外に固定ヘッダーが必要な場合は、`staticHeader.<ヘッダー名>`をkeyConfigへ設定してください。APIキーなどの秘密情報を固定ヘッダー値へ書いてはいけません。
- APIキーまたはtokenが必要な場合は、アプリ上でtoken登録が必要だと判定できるauthTypeを必ず設定してください。
- Basic認証が必要な場合はauthTypeをbasicとし、アプリ上でIDとPASSの登録が必要だと判定できるようにしてください。
- 認証情報取得ガイドが添付されている場合は、利用者自身が公式画面で認証情報を取得する手順も導入手順へ含めてください。秘密情報の値を質問したり、チャットへ貼り付けるよう求めたりしないでください。
- baseUrlとActionの相対urlを分離してください。
- methodはGET、POST、PUT、PATCH、DELETEから公式仕様に合う値を設定してください。
- Request JSONを送る場合は、c-sendから渡す本文の位置を文字列値"{{value}}"で示してください。GETなどボディを送らないActionではrequestを省略してください。
- Response JSONでは、結果本文として取り出す位置を文字列値"{{value}}"で示してください。
- モデル名などAPI固有の固定値は、公式仕様に従ってActionのurlまたはrequestへ記載し、c-send本体の専用処理を前提にしないでください。
- timeoutMsは特別な指定がなければ120000にしてください。
- 複数Actionが必要な場合はactions配列へ追加してください。
- idは同じ設定内で重複させないでください。

情報確認:

- 公式ドキュメントのURLを参照できない場合は、推測せず、必要なページ本文またはファイルを利用者へ要求してください。
- 利用したいAction、モデル、APIバージョンなどに選択肢があり、資料だけで決められない場合は、JSONを作る前に利用者へ質問してください。
- 料金、無料枠、利用制限、送信データの取り扱いなど、利用前に確認すべき公式情報があれば簡潔に示してください。

情報が揃った後の最終回答は、必ず次の順序で出力してください。

1. 設定概要
   - APIサービス名
   - 作成するAction
   - 認証方式
   - アプリで登録が必要な秘密情報（token、ID、PASSなど。値そのものは要求・表示しない）
2. 推奨ファイル名
   - api/<設定ID>.json の形式
3. API設定JSON
   - JSONコードブロックを1つ出力する
   - コメントを含まない有効なJSONにする
4. 導入手順
   - 認証が必要で取得ガイドがある場合は、公式ページから認証情報を取得する手順を先に説明する
   - JSONをcsend.exeと同じ場所のapiフォルダへ保存する
   - c-sendのシステムメニューから「API」を開き、追加された設定を確認する
   - 認証が必要ならAPI詳細画面でクレデンシャルIDを確認し、tokenまたはBasic認証のID／PASSを登録する
   - カテゴリ内の「（追加）」からAPI項目を作り、APIとActionを選択する
   - API項目の本文へ、{{clipboard}}または{{clipboard|代替文字列}}を使った実行用プロンプトを設定する
   - 設定を反映するために必要ならc-sendを再起動する
5. 動作確認手順
   - 初回確認に適した安全で短い入力例を示す
   - API項目の実行方法と、結果が自動コピーされない場合の結果保持アイコンの使い方を説明する
   - 実行不可アイコンが表示された場合は、token、ID、PASSの不足をAPI詳細画面で確認するよう説明する
6. 注意事項
   - token、APIキー、ID、PASSをJSON、定型文、プロンプト、チャットへ貼り付けない
   - 公式仕様の変更時は、同じ資料と現在のJSONをAIへ渡して更新版を作成できることを説明する

回答は日本語で、専門知識がない利用者でも上から順に操作できる具体的な手順にしてください。
```

生成後は、AIが出力した手順に従ってJSONの配置、API詳細画面での確認、必要なtoken、ID、PASSの登録、API項目の作成、動作確認を行う。秘密情報が未登録のAPI項目は実行不可として表示される。
