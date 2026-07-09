# c-send

定型文をカテゴリごとに管理して、ワンクリックでクリップボードへ送る Windows 向けツールです。

## 特徴

- タスクトレイに常駐します
- タスクトレイから操作すると、すばやく使えて便利です
- カテゴリごとに定型文を分けられます
- ローカルファイルと URL の両方をデータ元にできます
- URL のカテゴリは参照専用です
- 読み取り専用のカテゴリは `RO` と表示されます
- コピー時にカーソル付近へアプリ内 tip を表示します
- 必要に応じて Windows の toast 通知も使えます

## 使い方

基本手順は [使い方](docs/usage.md) にまとめています。

1. `csend.exe` を任意のフォルダに置きます
2. `csend.exe` を起動します
3. タスクトレイのアイコンを使ってアプリを開きます
4. カテゴリを選びます
5. 一覧から定型文をクリックすると、クリップボードに送られます

カテゴリの新規作成や並べ替えの説明は、[使い方](docs/usage.md) の「カテゴリの作成」を見てください。

## 設定ファイル

`setting.ini` は `csend.exe` と同じフォルダに置きます。
初回起動時自動で作成されます。

主な設定:

- `[category]` カテゴリ一覧と前回選択したカテゴリ
- `[font]` 表示フォント
- `[Window]` ウィンドウ位置
- `[notification]` 通知方法
- `[message]` 表示文言

## 通知について

- アプリ内 tip は既定で 3 秒表示します
- `setting.ini` の `notification.tip_ms` で表示時間を変えられます
- `setting.ini` の `notification.toast=on` にすると toast を表示します
- `notification.toast=off` の場合は toast を表示しません

## 配布

- [GitHub Releases](https://github.com/NaruSue/c-send/releases)
- [Vector](https://www.vector.co.jp/soft/win95/util/se061481.html?srsltid=AfmBOooY1phVnlrRnupG-bHEt6iyhr1YMWpCVOBYakbIfdn4s3pHxnWY#google_vignette)
