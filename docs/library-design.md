# c-send ライブラリ設計書

## 1. 目的

c-send PWAから利用できるAPI設定、AIプロンプト、定型文、テンプレートを検索・ダウンロードできる静的ライブラリを提供する。初期版はGitHub Pagesで公開し、更新はGitHub Actionsの手動実行で行う。

pushイベントでは公開しない。サンプルの確認後、Actionsの`Run workflow`を実行して公開する。

## 2. ライブラリの入口

ライブラリは、用途の異なる次の2系統を持つ。

```text
ライブラリ
├─ API設定
│  └─ API定義JSONを登録する
└─ データセット
   ├─ AIプロンプト
   ├─ 定型文
   └─ テンプレート
```

API設定は通信先とActionだけを含み、tokenやパスワードは含めない。認証情報はPWA利用者がAPI設定画面で入力し、IndexedDBへ保存する。

データセットは既存の共通バックアップJSON形式を使用する。AIプロンプトは、固定文言系とクリップボード送信系を区別できるようにする。

```text
AIプロンプト
├─ 固定文言系: 指示文だけを送信
└─ クリップボード送信系: {{clipboard}}を含む入力を送信
```

## 3. リポジトリ構成

```text
api/                              # API定義の正本（ネイティブ版と共通）
├─ gemini.json
└─ <provider>.json

samples/                          # データセットの正本
├─ jp/
│  ├─ AI API.json
│  └─ <dataset>.json
└─ en/
   └─ <dataset>.json

scripts-ver/library-site/          # ライブラリページの静的ソース
├─ index.html
└─ generate_catalog.py             # カタログ生成処理

_site/                             # Actions実行時だけ作るPages配置物（コミットしない）
├─ PWA本体
└─ library/
   ├─ index.html
   ├─ catalog.json
   ├─ api/
   └─ datasets/
      ├─ jp/
      └─ en/
```

`api/`と`samples/`が配布元であり、Pagesへ直接手作業でファイルを配置しない。Actionsが必要なファイルを`_site/library/`へコピーする。

## 4. カタログ仕様

`library/catalog.json`はActionsが`api/`と`samples/`から生成する。

```json
{
  "format": "c-send-library-catalog",
  "version": 1,
  "generatedAt": "2026-07-31T00:00:00Z",
  "apis": [
    {
      "id": "gemini",
      "name": "Gemini",
      "downloadUrl": "./api/gemini.json",
      "authType": "api-key-header"
    }
  ],
  "datasets": [
    {
      "id": "jp-ai-api",
      "name": "AI APIプロンプト",
      "language": "ja",
      "datasetType": "ai-prompt",
      "inputMode": "clipboard",
      "requiredApis": ["gemini"],
      "downloadUrl": "./datasets/jp/AI%20API.json"
    }
  ]
}
```

カタログ生成時に、token、credential、パスワード、IndexedDBバックアップは読み込まない。配布ファイルに秘密情報が含まれていないことを検査してから公開する。

## 5. PWAでの利用フロー

1. PWAの「ライブラリ」を開く
2. API設定またはデータセットを検索する
3. 詳細画面で説明、依存API、更新日時を確認する
4. 「ダウンロード」または「インストール」を選ぶ
5. API設定はAPI設定画面へ、データセットは定型文へ取り込む
6. `requiredApis`が未登録の場合は、先にAPI設定の登録を案内する

初期版のライブラリページは検索とJSONダウンロードに限定する。PWAへの直接インストールは、カタログとURL形式が安定した後に追加する。

## 6. GitHub Actions

workflowは`.github/workflows/library-deploy.yml`とする。

- 起動条件は`workflow_dispatch`のみ
- 任意の`ref`を選択して生成可能
- `api/`、`samples/jp/`、`samples/en/`を収集
- `catalog.json`を生成
- PWA本体と`library/`をまとめてPages artifactへ登録
- `actions/deploy-pages`で公開
- tokenや秘密鍵を使用しない

公開URLは既存のPWA公開URL配下の`/library/`とする。

```text
https://narusue.github.io/c-send/library/
```
