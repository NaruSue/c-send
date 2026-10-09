# c-send PWA

c-send PWAは、よく使う文章を登録して、スマートフォンからすぐにコピーできる定型文コピーアプリです。

Windows用のネイティブ版c-sendとは別に、iPhoneなどのスマートフォンで使うことを主目的にしています。バイナリのインストールは不要で、HTTPSで公開されたページをSafariなどで開くだけで利用できます。

## 公開URL

本番版はGitHub Pagesで公開します。

- [c-send PWA](https://narusue.github.io/c-send/)

このURLをiPhoneのSafariで開き、ホーム画面へ追加して利用します。

## 主な機能

- 一覧をタップして本文をコピー
- コピーした本文をその場で確認
- タイトル・本文・タグ検索
- タグによる絞り込み
- 最後に使った項目をリストの先頭に表示
- 定型文の追加・編集・削除
- 共通JSONファイルの追加、バックアップ、リストア
- テンプレート項目の評価とコピー
- `＋` ボタンからのクリップボード取り込み
- IndexedDBへの端末内保存
- iPhoneのホーム画面へ追加してPWAとして利用
- API定義を使ったAI・外部APIの実行
- メニューからリリース情報を確認

## iPhoneへの追加

1. Safariで [c-send PWA](https://narusue.github.io/c-send/) を開きます。
2. 画面右上の家アイコンをタップします。
3. Safariの共有ボタンをタップします。
4. `ホーム画面に追加` を選び、`追加` をタップします。

追加後はホーム画面のc-sendアイコンから起動できます。PWAとして起動した場合、右上の追加アイコンは表示されません。

## 使い方

### 定型文を登録する

1. 登録したい文章を他のアプリやWebページでコピーします。
2. c-sendを開いて、右下の `＋` をタップします。
3. 本文にコピー済みの内容が入っていることを確認します。
4. 必要に応じてタイトルとタグを入力します。
5. 本文入力欄の下で `plain text` または `template` を選びます。
6. `保存` をタップします。

クリップボードの読み取りがiPhone側で許可されない場合は、本文欄へ手動で貼り付けてください。

### 定型文を使う

1. 一覧から使いたい項目をタップします。
2. 本文がコピーされ、コピーした内容が確認画面に表示されます。
3. 確認画面をタップして閉じます。
4. 元のアプリへ戻って貼り付けます。

同じようなタイトルの項目を誤って選んでも、確認画面で内容を見てから別の項目を選び直せます。
使った項目はリストの先頭へ移動します。リストを下へスクロールした後は、画面上部のアプリタイトルをタップするとリスト先頭へ戻ります。

テンプレート項目は一覧で `[T]` と表示され、コピー時に日時やクリップボードの内容を展開します。詳しくは [テンプレートの使い方](../../docs/template-usage.md) を参照してください。

### APIを使う

1. 左上のアイコンから `API設定` を開きます。
2. API設定画面でAPI定義を新規作成するか、API設定JSONをファイル・URL・IndexedDBから読み込みます。
3. APIの認証方式を確認し、API token、Bearer token、またはBasic認証のID／PASSを登録します。
4. 定型文の追加・編集画面でAPIモードを選び、APIとActionを指定します。
5. 入力本文と、Request JSON内の`{{value}}`へ入力が渡ることを確認して保存します。
6. 一覧からAPI項目を実行し、結果を確認してからクリップボードへコピーします。

API定義JSONにはAPI tokenなどの機密情報を含めないでください。資格情報はこのPWAを利用している端末・ブラウザ内のIndexedDBへ保存されます。URLからの読み込みは取得先がCORSを許可している必要があります。

### JSONファイルを追加する

左上のアイコンからメニューを開き、`ファイルから追加` を選びます。[共通JSONデータ形式仕様](../../docs/data-file-spec.md) に適合するファイルを1つ以上選択すると、既存データへ追加されます。タイトルと本文が既存項目と同じ場合は重複追加せず、タグを統合して今回のファイル名で更新します。

GitHubの公開カタログを使う場合は、メニューの `URLから追加` にJSONのRaw URLを入力します。取得先がCORSを許可している必要があります。タイトルと本文が既存項目と同じ場合は重複追加せず、タグを統合して今回のURLで更新します。取り込み後の項目は端末内のデータとして自由に編集できます。

URLデータの使い方は、更新しないスナップショットと、参照元に追随する自動更新から選べます。取り込んだ後も同じ内容を使いたい場合は `URLから追加` を選んでください。その時点のJSONを一度だけ取り込み、URL上のデータが後から更新されても端末内のデータは変わりません。URL入力欄には前回のURLが表示され、×を押すと入力欄を空にできます。

URL上のデータを自動更新したい場合は、メニューの `URLを参照` を使います。HTTPSのJSON Raw URLを登録すると初回データを取り込み、次回以降の起動時にバックグラウンドで最新データを確認します。アプリを開いている間は約10分ごとにも参照URLを確認し、同じタイミングでリリース情報も確認します。画面をバックグラウンドから戻したとき、前回の確認から約10分以上経っていれば再確認します。ブラウザーはバックグラウンド中の処理を遅らせたり止めたりすることがあるため、厳密に10分ごとの実行ではありません。通信に失敗した場合は、端末内の定型文とリリース情報の未読状態を維持します。

参照URLの確認では、まずサーバーへ `HEAD` を送り、前回のETagと一致すればJSON本文を再取得せず「最新」とします。ETagが変わった場合や取得できない場合は `GET` でJSONを取得して検証・反映します。ETagは参照URLごとに端末内へ保存されます。ETagを使うには配布元のCORS応答で `Access-Control-Expose-Headers: ETag` が必要です。設定されていない配布元でもGETにフォールバックするため利用できますが、本文取得の節約にはなりません。loose.bzの公開JSONでは同ヘッダーがサーバーに設定されており、ETagをブラウザーから参照できます。サーバー設定・デプロイ状況の詳細はc-send.web側の運用記録を参照してください。

参照一覧には複数のURLを登録でき、不要になったURLは一覧から削除できます。各URLの行にはURL、現在の項目数、最終確認日時、`更新`、`詳細`、`削除` ボタンが表示されます。行の `更新` はそのURLだけを再取得し、一覧上部の `すべて更新` は登録したURLをまとめて再取得します。長いURLは入力欄を横にスクロールして確認できます。`詳細` をタップすると、そのURLから参照中の項目タイトル一覧が開き、`詳細を閉じる` で閉じます。参照中の項目はそのURLの最新データで置き換わり、参照元から削除された項目も反映されます。別のURLから取り込んだスナップショットや手入力の定型文には影響しません。参照URLを削除しても、最後に取り込んだデータは端末内にスナップショットとして残ります。

### リリース情報

メニューの `リリース情報` から、公開ページ `news.html` をアプリ内で確認できます。ページの内容が前回閲覧した時から更新されていると、メニューアイコン左側に青いバッジが表示されます。アプリ自体の更新がある場合の赤いバッジは右側に表示されます。リリース情報ページはキャッシュされるため、通信できない時も最後に取得した内容を確認できます。

次回のPWAリリースでは、メニューを開いた時にも、未読のお知らせがある `リリース情報` に青い `NEW`、新版が確認された `バージョンチェック` に赤い `更新あり` を項目名の横へ表示する予定です。既読化またはアプリ更新後に該当表示を消します。これは次回リリースで実装する予定の内容で、現在のバージョンにはまだ含まれていません。

URL参照には、配布元がブラウザからの取得を許可するCORS設定であることが必要です。オフラインや取得失敗時は現在のデータをそのまま利用できます。参照元の更新を次回取得時に反映するため、参照中の項目を端末で編集しても、そのURLの次回同期で参照元の内容に置き換わります。

### 公開サンプルカタログ

サンプルJSONはGitHubで公開しています。PWAの `URLから追加` では、通常のGitHubページではなく、必ず `Raw` のURLを使用してください。

- [日本語サンプル一覧（GitHub）](https://github.com/NaruSue/c-send/tree/master/samples/jp)
- [英語サンプル一覧（GitHub）](https://github.com/NaruSue/c-send/tree/master/samples/en)

### ライブラリ

メニューの`ライブラリ`から、独立公開されたAPI設定・データセットを検索できます。ライブラリ画面では、検索欄とタグチップを組み合わせて絞り込み、JSONを開くかPWAへインストールできます。

- [c-sendライブラリ](https://narusue.github.io/c-send-library/)
- カタログ: `https://narusue.github.io/c-send-library/catalog.json`

#### 日本語サンプル

| サンプル | Raw URL |
| --- | --- |
| Git | [Raw](https://raw.githubusercontent.com/NaruSue/c-send/master/samples/jp/Git.json) |
| HTML | [Raw](https://raw.githubusercontent.com/NaruSue/c-send/master/samples/jp/HTML.json) |
| JSON | [Raw](https://raw.githubusercontent.com/NaruSue/c-send/master/samples/jp/JSON.json) |
| Markdown | [Raw](https://raw.githubusercontent.com/NaruSue/c-send/master/samples/jp/Markdown.json) |
| PowerShell | [Raw](https://raw.githubusercontent.com/NaruSue/c-send/master/samples/jp/PowerShell.json) |
| SQL | [Raw](https://raw.githubusercontent.com/NaruSue/c-send/master/samples/jp/SQL.json) |
| URL | [Raw](https://raw.githubusercontent.com/NaruSue/c-send/master/samples/jp/URL.json) |
| Windowsコマンド | [Raw](https://raw.githubusercontent.com/NaruSue/c-send/master/samples/jp/Windows%E3%82%B3%E3%83%9E%E3%83%B3%E3%83%89.json) |
| テンプレート | [Raw](https://raw.githubusercontent.com/NaruSue/c-send/master/samples/jp/%E3%83%86%E3%83%B3%E3%83%97%E3%83%AC%E3%83%BC%E3%83%88.json) |
| メール・チャット | [Raw](https://raw.githubusercontent.com/NaruSue/c-send/master/samples/jp/%E3%83%A1%E3%83%BC%E3%83%AB%E3%83%BB%E3%83%81%E3%83%A3%E3%83%83%E3%83%88.json) |
| 挨拶 | [Raw](https://raw.githubusercontent.com/NaruSue/c-send/master/samples/jp/%E6%8C%A8%E6%8B%B6.json) |
| 記号 | [Raw](https://raw.githubusercontent.com/NaruSue/c-send/master/samples/jp/%E8%A8%98%E5%8F%B7.json) |

#### 英語サンプル

| サンプル | Raw URL |
| --- | --- |
| Email and Chat | [Raw](https://raw.githubusercontent.com/NaruSue/c-send/master/samples/en/Email%20and%20Chat.json) |
| Git | [Raw](https://raw.githubusercontent.com/NaruSue/c-send/master/samples/en/Git.json) |
| Greetings | [Raw](https://raw.githubusercontent.com/NaruSue/c-send/master/samples/en/Greetings.json) |
| HTML | [Raw](https://raw.githubusercontent.com/NaruSue/c-send/master/samples/en/HTML.json) |
| JSON | [Raw](https://raw.githubusercontent.com/NaruSue/c-send/master/samples/en/JSON.json) |
| Markdown | [Raw](https://raw.githubusercontent.com/NaruSue/c-send/master/samples/en/Markdown.json) |
| PowerShell | [Raw](https://raw.githubusercontent.com/NaruSue/c-send/master/samples/en/PowerShell.json) |
| SQL | [Raw](https://raw.githubusercontent.com/NaruSue/c-send/master/samples/en/SQL.json) |
| Symbols | [Raw](https://raw.githubusercontent.com/NaruSue/c-send/master/samples/en/Symbols.json) |
| Templates | [Raw](https://raw.githubusercontent.com/NaruSue/c-send/master/samples/en/Templates.json) |
| URLs | [Raw](https://raw.githubusercontent.com/NaruSue/c-send/master/samples/en/URLs.json) |
| Windows Commands | [Raw](https://raw.githubusercontent.com/NaruSue/c-send/master/samples/en/Windows%20Commands.json) |

URLをコピーしてPWAのメニューから `URLから追加` を開き、入力欄へ貼り付けてください。Raw URLの例:

```text
https://raw.githubusercontent.com/NaruSue/c-send/master/samples/jp/テンプレート.json
```

### 探す

- 上部の検索欄では、タイトル・本文・タグを検索できます。
- タグチップをタップすると、そのタグを持つ項目だけを表示します。最後に選択したタグが先頭に並び、未選択のタグは件数順で表示されます。
- 項目はコピー回数が多い順に並びます。

## 初期サンプル

初回起動時には、仕事・友人への連絡文、位置情報を使う検索文、AI用プロンプト、テンプレート各コマンドのサンプルが登録されます。日本語テンプレートには `テンプレ` / `ja`、英語テンプレートには `template` / `en` タグが付きます。

初期サンプルは [samples.json](./samples.json) で管理しています。既に登録済みの定型文がある場合、サンプルの更新で利用者のデータが上書きされることはありません。

## 公開・更新方法

本番版の公開先はGitHub Pagesです。

- 公開URL: [https://narusue.github.io/c-send/](https://narusue.github.io/c-send/)
- 公開対象: `scripts-ver/pwa` 配下

GitHub Pagesへのデプロイ後、公開URLからアプリを開けます。

更新時は少なくとも次を同時に配置します。

- `index.html`
- `service-worker.js`
- `samples.json`

## API定義・AIプロンプトの配布

ネイティブ版と共通のGemini API定義をダウンロードして、PWAの「API設定」からJSON読み込みできます。

- [Gemini API設定JSON](./api/gemini.json)
- [AI APIプロンプトサンプル（日本語）](./samples/jp/AI%20API.json)

手順は、API設定JSONをダウンロードし、PWAの「メニュー」→「API設定」→「JSON読込」で読み込みます。続けてAPI tokenを入力して保存し、AI APIプロンプトサンプルを「URLから追加」またはファイル読み込みで登録します。
- `manifest.webmanifest`
- `apple-touch-icon.png`

PWAはService Workerによってファイルをキャッシュします。起動時に新版が見つかると、左上のメニューアイコン右側に赤いバッジが表示されます。リリース情報が更新されている場合は、同じアイコンの左側に青いバッジが表示されます。メニュー最下部の `バージョンチェック` を選ぶとアプリを更新できます。c-send PWA v2.5ではURL参照と起動時の更新通知に対応しました。v2.6では参照URLごとの項目数・最終更新日時・詳細タイトル一覧を表示し、v2.7では赤い更新バッジの表示を修正、v2.8ではアプリ内リリース情報ページと青い更新バッジを追加、v2.9では参照URLの個別・一括手動更新と約10分ごとの自動更新、リリース情報の定期確認に対応し、v2.10では参照URLのETag確認で変更のないJSON本文を再取得しないようにしました。

## 制約

- 保存データは利用中の端末・ブラウザ内に保存され、他端末とは同期されません。
- c-sendから他アプリを自動起動したり、自動貼り付けしたりはできません。
- クリップボード読み取りはiPhoneやブラウザの制約で失敗する場合があります。
- ネイティブ版およびHTA版のカテゴリ機能とは互換ではありません。

## ファイル構成

```text
scripts-ver/pwa/
├─ index.html
├─ news.html
├─ samples.json
├─ manifest.webmanifest
├─ service-worker.js
├─ apple-touch-icon.png
├─ c-send.ico
├─ .nojekyll
├─ readme.md
└─ docs/design.md
```
