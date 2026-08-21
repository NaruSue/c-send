# c-send ライブラリ設計書

## 1. 目的

c-send PWAから利用できるAPI設定、AIプロンプト、定型文、テンプレートを検索・ダウンロードできる静的ライブラリを提供する。ライブラリは`NaruSue/c-send-library`で独立管理し、GitHub Pagesで公開する。

`master`へのpushまたはActionsの`Run workflow`で公開する。

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

NaruSue/c-send-library/            # 独立ライブラリリポジトリ
├─ api/
├─ samples/jp/
├─ samples/en/
scripts/library-site/              # ライブラリページの静的ソース
├─ index.html
└─ generate_catalog.py             # カタログ生成処理

_site/                             # Actions実行時だけ作るPages配置物（コミットしない）
├─ index.html
├─ catalog.json
├─ api/
└─ datasets/
   ├─ jp/
   └─ en/
```

`api/`と`samples/`が配布元であり、Pagesへ直接手作業でファイルを配置しない。Actionsが必要なファイルを`_site/`へコピーする。

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

データセットの各項目に付いたタグを集計し、カタログの各エントリへ`tags`として格納する。ライブラリページとPWAのライブラリ画面は、このタグを使って絞り込む。

## 5. PWAでの利用フロー

1. PWAの「ライブラリ」を開く
2. API設定またはデータセットを検索する
3. タグチップで用途を絞り込む
4. 詳細画面で説明、依存API、更新日時を確認する
5. 「ダウンロード」または「インストール」を選ぶ
6. API設定はAPI設定画面へ、データセットは定型文へ取り込む
7. `requiredApis`が未登録の場合は、先にAPI設定の登録を案内する

ライブラリページは検索、タグ絞り込み、JSONダウンロードを提供する。PWAのライブラリ画面では、同じタグで絞り込んで直接インストールできる。

## 6. GitHub Actions

workflowは`.github/workflows/library-deploy.yml`とする。

- 起動条件は`master`へのpushまたは`workflow_dispatch`
- `api/`、`samples/jp/`、`samples/en/`を収集
- `catalog.json`を生成
- ライブラリ単体をPages artifactへ登録
- `actions/deploy-pages`で公開
- tokenや秘密鍵を使用しない

公開URLはPWAとは独立したGitHub Pagesとする。

```text
https://narusue.github.io/c-send-library/
```
