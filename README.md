# c-send

定型文を登録して、ワンクリックでクリップボードに送信する Windows 向けツールです。

## 機能

- カテゴリごとにデータファイルを分けられます
- データ参照先はローカルファイルと URL を使えます
- URL のカテゴリは参照専用です
- 読み取り専用のカテゴリは `RO` として表示されます
- 一覧の `(追加)` から定型文を登録できます
- クリップボード転送時に、カーソル付近へアプリ内 tip を表示します
- 必要に応じて toast（Windows の通知）も使えます

## 使い方

1. `csend.exe` を任意のフォルダに配置します
2. `csend.exe` を起動します
3. カテゴリを選びます
4. `(追加)` を使って文字列を登録します
5. 一覧の項目をクリックするとクリップボードへ送信します

## 通知

クリップボードへ送信したときは、まずアプリ内の tip 吹き出しで反応を返します。

- 既定では tip を 3 秒表示します
- `setting.ini` の `notification.tip_ms` で表示時間を変えられます
- `setting.ini` の `notification.toast=on` にした場合だけ toast を使います
- ステータスバー通知は使いません

## `setting.ini`

設定ファイルは `csend.exe` と同じフォルダに置きます。

主な設定は次のとおりです。

```ini
[category]
count=2
item000=Git
path000=samples\en\Git.txt
item001=SQL
path001=https://raw.githubusercontent.com/NaruSue/c-send/feature/codex_1.6/samples/jp/SQL.txt
last=Git

[font]
name=MS UI Gothic
size=16

[Window]
window=04EE0982080C0C4F

[notification]
toast=on
tip_ms=3000
```

### `pathNNN`

- ローカルファイルのパスを指定できます
- `http://` または `https://` で始まる URL も指定できます
- URL のカテゴリは読み取り専用です
- 読み取り専用のカテゴリでは、編集画面を開いても保存はできません

### `notification.toast`

- `on` のときだけ toast を表示します
- `off` のときは toast を表示しません

### `notification.tip_ms`

- アプリ内 tip の表示時間をミリ秒で指定します
- 既定値は `3000` です

## 補足

- データ保存形式は将来拡張できるように考えています
- 現時点の `setting.ini` では、参照先の管理に集中しています

## ダウンロード

- [GitHub Releases](https://github.com/NaruSue/c-send/releases)
- [Vector](https://www.vector.co.jp/soft/win95/util/se061481.html?srsltid=AfmBOooY1phVnlrRnupG-bHEt6iyhr1YMWpCVOBYakbIfdn4s3pHxnWY#google_vignette)
