# c-send

c-send は、定型文・コマンド・AIプロンプトをカテゴリごとに管理し、必要な内容をすぐクリップボードへ送れる Windows 用スニペット管理ツールです。

v2.0 では、定型文を外部APIへ送信して結果を受け取る、設定駆動のAPI機能を追加しました。Gemini、OpenAI、Anthropic Claude API、GitHub REST API、Open-Meteoなどを、アプリ本体へサービス固有コードを追加せず設定JSONで利用できます。

## v2.0 の方針

API機能の目的は、利用者が自分の使いたいAI・Web APIを安全に追加・調整できることです。

1. `docs/api/config-spec.md` と対象APIの公式資料をAIへ渡す。
2. AIに設定JSON、認証情報の取得手順、導入・動作確認手順を作成させる。
3. JSONを`api/`へ置き、API詳細画面で秘密情報だけを登録する。
4. カテゴリへAPI項目を追加し、プロンプトや定型文を自由にカスタマイズする。

公開リポジトリの資料・サンプル・AI向けプロンプトを充実させ、利用者や外部コントリビューターが設定JSON、導入ガイド、カテゴリサンプルを追加できる形を目指します。

## 主な機能

- 通常の定型文、テンプレート、カウンター、API項目を同じカテゴリへ混在
- `{{clipboard}}`、`{{clipboard|代替文字列}}`などのテンプレート展開
- APIごとの基本URL、Action、HTTP method、JSON request／response、固定ヘッダー、タイムアウト設定
- 認証なし、API key header/query、Bearer、Basic 認証
- APIキー・token・Basic認証情報を Windows Credential Manager へ保存
- API実行状態、エラー、保持結果を右寄せアイコンで表示
- クリップボードが実行中に変更された場合は自動上書きを避け、保持結果から手動コピー
- API設定・カテゴリサンプルをJSONファイルとして共有・更新

## 同梱APIサンプル

`api/`には次の設定JSONを同梱しています。秘密情報は含まれません。

| サービス | 例 | 認証 |
| --- | --- | --- |
| Gemini | 文章生成 | API key header |
| OpenAI | Responses API | Bearer |
| Anthropic Claude API | Messages API | API key header + 固定ヘッダー |
| GitHub REST API | 既定ブランチ名・ブランチ一覧 | Bearer |
| Open-Meteo | 東京の現在気温・最高気温 | 不要 |

各サービスの認証情報取得・登録方法は [docs/api/other/](docs/api/other/) にあります。天気APIのカテゴリ例は [samples/jp/天気 API.json](<samples/jp/天気 API.json>) と [samples/en/Weather API.json](<samples/en/Weather API.json>) です。

## APIを追加する

最初に [API設定JSON仕様](docs/api/config-spec.md) を読み、対象APIの公式資料と一緒にAIへ渡してください。この仕様書には、AIが設定JSONだけでなく、キー取得、c-sendへの登録、カテゴリへの追加、動作確認までを出力するための依頼文が含まれています。

利用者自身で作る場合も、次の資料を起点にできます。

- [API機能設計書](docs/api/design.md)
- [認証方式仕様](docs/api/auth-spec.md)
- [API設定JSON仕様](docs/api/config-spec.md)
- [新しいサービスを追加する資料テンプレート](docs/api/other/provider-template.md)

API仕様が変わった場合は、現在の設定JSON、同じバージョンの`config-spec.md`、最新の公式資料をAIへ渡して更新版を作成できます。

### 秘密情報の扱い

API key、token、ID、PASSをJSON、カテゴリファイル、Git、チャット、スクリーンショットへ保存・投稿しないでください。ネイティブ版ではAPI詳細画面から登録し、Windows Credential Managerへ保存します。

## 基本的な使い方

1. `csend.exe`を任意のフォルダへ置いて起動します。
2. システムメニューからカテゴリを設定します。
3. 一覧から通常項目を選ぶとクリップボードへコピーされます。
4. APIを使う場合は`csend.exe`と同じ場所の`api/`へ設定JSONを置き、システムメニューの「API」から認証情報を登録します。
5. カテゴリの「（追加）」でAPIとActionを選び、本文へプロンプトを設定します。

詳しい操作は [使い方](docs/usage.md) を参照してください。

## サンプルデータ

`samples/jp/` と `samples/en/` に、Git、SQL、PowerShell、Markdown、JSON、HTML、URL、AIプロンプト、天気APIなどのカテゴリ例を入れています。公開JSONをローカルへ取り込んで自由に編集したり、GitHub Raw URLで読み取り専用カタログとして参照したりできます。

## 提供形態

このREADMEはWindowsネイティブ版（`csend.exe`）の説明です。

- [HTA版](scripts-ver/hta/readme.md): 外部バイナリを導入できないWindows環境向け
- [PWA版](scripts-ver/pwa/readme.md): iPhone / iPadOS / Android向け

API通信機能は、ネイティブ版の機能として提供します。

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
