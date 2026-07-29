# c-send

c-sendは、よく使う文章やコマンドを登録して、必要なときにすぐ呼び出せるWindows用ツールです。

初めて使う方は、まず [はじめて使う人へ](docs/first-use.md) を読んでください。

## こんな使い方ができます

- よく使うメールやチャットの文章を、すぐ入力できるようにする
- よく使うコマンド、URL、SQL、PowerShellをまとめておく
- 文章をカテゴリごとに整理して、必要なものだけを探す
- 今コピーしている文章を、登録した文章の中へ差し込む
- 日付や連番を自動で入れた文章を作る
- 文章をAIに送って、要約・添削・翻訳をする
- 外部サービスから天気などの情報を取得する

選んだ文章や処理結果は、クリップボードへ送られます。普段使う文章や操作を、決まった場所からすぐ使えるようにするためのツールです。

## まず使ってみる

1. `csend.exe`を起動します。
2. システムメニューからカテゴリを設定します。
3. 一覧から使いたい文章やコマンドを選びます。

選んだ内容がクリップボードへコピーされるので、そのままメール、チャット、ターミナルなどへ貼り付けて使えます。

詳しい操作は [使い方](docs/usage.md) を参照してください。

## AIで文章を整えてみる

登録した文章をGeminiへ送り、要約や添削を試せます。API設定JSONを自分で作らずに試せるサンプルを用意しています。

[Geminiと連携してみる](docs/api/gemini-tutorial.md)

## できること

- 定型文、コマンド、URLなどの登録・編集・削除
- カテゴリごとの整理と並べ替え
- クリップボードの内容を差し込むテンプレート
- 日時や連番などの自動入力
- 日本語・英語のサンプルカテゴリ
- AIや外部サービスとの連携
- APIの実行状態、エラー、実行結果の確認
- APIキーやtokenなどの認証情報をWindows Credential Managerへ保存

## 同梱サンプル

通常のサンプルとして、次のカテゴリを同梱しています。

- メール・チャット
- 挨拶
- Git
- SQL
- PowerShell
- Markdown
- JSON
- HTML
- URL
- Windowsコマンド

`samples/jp/`と`samples/en/`にあるJSONをカテゴリとして追加すると、そのまま利用できます。

公開テンプレートをURLから参照する方法と、ダウンロードして自分用に編集する方法は [テンプレートを探して使う](docs/template-catalog.md) を参照してください。

## AI・外部サービスとの連携

### Geminiを使う

[Geminiと連携してみる](docs/api/gemini-tutorial.md)では、Gemini API keyの取得、c-sendへの登録、サンプルカテゴリの追加、要約・添削の実行までを説明しています。

### 別のAIやサービスを追加する

利用したいサービスの公式資料と、次の仕様書をAIへ渡すと、c-send用の設定JSONと導入手順を作成できます。

- [API利用者向け導入手順](docs/api/getting-started.md)
- [API設定JSON仕様](docs/api/config-spec.md)

現在、次の設定サンプルを同梱しています。

| サービス | できること |
| --- | --- |
| Gemini | 文章生成 |
| OpenAI | Responses APIによる文章生成 |
| Anthropic Claude API | Messages APIによる文章生成 |
| GitHub | リポジトリ情報の取得 |
| Open-Meteo | 東京の現在気温・最高気温の取得 |

APIを使う場合は、`csend.exe`と同じ場所の`api/`へ設定JSONを置き、システムメニューの「API」から認証情報を登録します。

### 認証情報について

API key、token、ID、PASSをJSON、カテゴリファイル、Git、チャット、スクリーンショットへ保存・投稿しないでください。c-sendではAPI詳細画面から登録し、Windows Credential Managerへ保存します。

API機能の詳しい仕様は、次の資料を参照してください。

- [API機能設計書](docs/api/design.md)
- [認証方式仕様](docs/api/auth-spec.md)
- [新しいサービスを追加する資料テンプレート](docs/api/other/provider-template.md)
- [各サービスの認証情報取得ガイド](docs/api/other/)

## 提供形態

このREADMEはWindowsネイティブ版（`csend.exe`）の説明です。

- [HTA版](scripts-ver/hta/readme.md): 外部バイナリを導入できないWindows環境向け
- [PWA版](scripts-ver/pwa/readme.md): iPhone / iPadOS / Android向け

API通信機能はネイティブ版の機能です。

## ビルド

```bat
build.cmd Release
```

API機能は外部サービスへ接続せずモックテストできます。テスト仕様は [API mock test specification](docs/api/mock-test-spec.md) を参照してください。

## ダウンロード

- [GitHub Releases](https://github.com/NaruSue/c-send/releases)
- [Vector](https://www.vector.co.jp/soft/win95/util/se061481.html)

## ライセンス

このプロジェクトは [LICENSE](LICENSE) の条件に従って利用できます。
