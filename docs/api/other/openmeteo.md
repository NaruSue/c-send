# Open-Meteo Weather API と c-send

最終確認日: 2026-07-28

対象設定は`api/open-meteo.json`です。Open-MeteoのForecast APIを使用し、API keyなしで東京の現在気温と今日の予報最高気温を取得します。

## 公式資料

- [Open-Meteo Weather Forecast API](https://open-meteo.com/en/docs)
- [Open-Meteo API概要](https://open-meteo.com/)

Forecast APIでは緯度・経度が必須です。`current`や`daily`で返す項目を指定し、レスポンスJSONから値を取り出します。

## 使い方

1. `api/open-meteo.json`を`csend.exe`と同じフォルダの`api`へ置く。
2. c-sendを再起動し、システムメニューの「API」を開く。
3. Open-Meteo Weather APIが表示されることを確認する。認証情報は不要である。
4. カテゴリへAPI項目を追加し、東京の現在気温を取得または東京の今日の最高気温を取得を選択する。
5. 実行結果は数値だけがクリップボードへ入る。単位は摂氏（°C）。

場所を変更するときはAction URLの`latitude`と`longitude`を変更する。返却する値を変えるときは、公式資料に従ってクエリとResponse JSONを対にして変更する。

## AIへの依頼文

```text
Open-Meteo公式資料、添付したc-sendのdocs/api/config-spec.md、api/open-meteo.jsonを読んでください。
次の場所の天気を取得するActionへ変更または追加してください。

- 場所: （都市名または緯度・経度）
- 欲しい情報: （現在気温、降水確率、週間最高気温など）

GETなのでrequestは省略してください。公式資料に従い、URLのクエリとresponse JSONの{{value}}位置を対応させてください。
```
