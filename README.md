# c-send

c-send は、よく使う文章・コマンド・プロンプトをカテゴリごとに管理し、クリックだけでクリップボードへ送れる Windows 用の軽量スニペット管理ツールです。

AI プロンプト、テストデータ、Git コマンド、SQL、PowerShell、メール定型文、チャットの定型返信など、繰り返し入力する内容を一覧からすぐ選べます。

## c-send とは

c-send は、単なるクリップボードツールではなく、繰り返し入力を高速化するためのツールです。

- カテゴリごとにスニペットを整理できます
- 一覧から見て選べるので、辞書登録のように「覚えて打つ」必要がありません
- テキストファイルベースで管理できるので、編集やバックアップがしやすいです
- ローカルファイルと URL の両方をデータ元にできます
- URL のカテゴリは参照専用です
- 読み取り専用のカテゴリは `RO` と表示されます
- コピー時にカーソル付近へアプリ内 tip を表示します
- 必要に応じて Windows の toast 通知も使えます

## こんな用途に使えます

- AI プロンプト管理
- テストデータ入力
- Git コマンド管理
- SQL の定型文管理
- PowerShell コマンド管理
- メール文面のひな形管理
- チャットの定型返信管理

## 特長

- 軽量でシンプルです
- タスクトレイに常駐します
- タスクトレイからすばやく操作できます
- カテゴリごとに定型文を分けられます
- ローカルファイルと URL の両方をデータ元にできます
- URL カテゴリは参照専用です
- 読み取り専用のカテゴリは `RO` と表示されます
- コピー時にアプリ内 tip を表示します
- 必要に応じて Windows の toast 通知も使えます
- `setting.ini` で表示や通知を調整できます

## 使い方

基本手順は [使い方](docs/usage.md) にまとめています。

1. `csend.exe` を任意のフォルダに置きます
2. `csend.exe` を起動します
3. タスクトレイのアイコンから本体画面を開きます
4. カテゴリを選びます
5. 一覧から定型文をクリックすると、クリップボードに送られます

カテゴリの新規作成や並べ替えの説明は、[使い方](docs/usage.md) の「カテゴリの作成」を見てください。

## サンプルデータ

`samples/` に、すぐ試せるサンプルデータを入れています。

- `samples/jp/` 日本語サンプル
- `samples/en/` 英語サンプル

たとえば、Git、SQL、PowerShell、Markdown、JSON、HTML、URL などの例があります。

## インストール / ダウンロード

配布物は次のページから入手できます。

- [GitHub Releases](https://github.com/NaruSue/c-send/releases)
- [Vector](https://www.vector.co.jp/soft/win95/util/se061481.html?srsltid=AfmBOooY1phVnlrRnupG-bHEt6iyhr1YMWpCVOBYakbIfdn4s3pHxnWY#google_vignette)

ダウンロードした `csend.exe` を任意のフォルダに置いて起動してください。

## ビルド方法

ソースからビルドする場合は、リポジトリ直下で `build.cmd` を実行します。

```bat
build.cmd Release
```

必要に応じて Visual Studio Build Tools などの環境を用意してください。

## 設定ファイル

`setting.ini` は `csend.exe` と同じフォルダに置きます。
初回起動時に自動で作成されます。

主な設定:

- `[category]` カテゴリ一覧と前回選択したカテゴリ
- `[font]` 表示フォント
- `[Window]` ウィンドウ位置
- `[notification]` 通知方法
- `[message]` 表示文言

通知について:

- アプリ内 tip は既定で 3 秒表示します
- `setting.ini` の `notification.tip_ms` で表示時間を変えられます
- `setting.ini` の `notification.toast=on` にすると toast を表示します
- `notification.toast=off` の場合は toast を表示しません

## 補足

- 詳しい設定仕様は [setting.ini 仕様](docs/setting-ini-spec.md) を参照してください
- データファイルの形式は [データファイル仕様](docs/data-file-spec.md) にまとめています

## ライセンス

このプロジェクトは [LICENSE](LICENSE) の条件に従って利用できます。
