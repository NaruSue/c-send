# ネイティブ版データファイル仕様

この文書は、ネイティブ版 c-send が `setting.ini` の `[category]` セクションから参照するデータファイルの仕様をまとめたものである。

v1.9では、従来の3行1レコード形式に加えてJSON形式を追加する。

## 1. 対象

- `setting.ini` の `[category]` セクション
- `path000`、`path001` などに指定するローカルファイルまたはURL
- 1つのカテゴリに対応する定型文データ

`itemNNN` はカテゴリ名、`pathNNN` はデータ参照先を表す。

## 2. データ形式の判定

データ形式は、`setting.ini` に追加の設定値を持たせず、参照先から自動判定する。

| データ元 | JSONとして扱う条件 | それ以外 |
| --- | --- | --- |
| ローカルファイル | ファイル拡張子が `.json`（大文字小文字を区別しない） | 従来の3行1レコード形式 |
| URL | HTTPレスポンスの `Content-Type` が `application/json` または `application/*+json` | 従来の3行1レコード形式 |
| URL（補助判定） | Content-TypeがJSON系で取得できず、URLのパス部分が `.json` で終わる | 従来の3行1レコード形式 |

URLのクエリ文字列とフラグメントは、拡張子判定の対象から除く。

例:

- `C:\Users\example\Documents\category.json` → JSON
- `samples\jp\Git.txt` → 従来形式
- `https://example.com/category.json?rev=10` → JSON
- `https://example.com/data` が `Content-Type: application/json` を返す → JSON

URLは、一時ファイルに保存した後も、取得時に判定した形式を維持して読み込む。一時ファイルの拡張子には依存しない。

## 3. JSON形式

JSONデータファイルはUTF-8（BOMあり・なしのどちらも可）で保存する。ルート要素は配列とする。

```json
[
  {
    "name": "Git Status",
    "value": "git status",
    "type": 0
  },
  {
    "name": "複数行の例",
    "value": "1行目\n2行目",
    "type": 0
  }
]
```

### 1件の項目

| プロパティ | 型 | 必須 | 内容 |
| --- | --- | --- | --- |
| `name` | 文字列 | 必須 | 一覧に表示する名前 |
| `value` | 文字列 | 必須 | クリップボードへ送る本文。複数行可 |
| `type` | 整数 | 必須 | 現時点では `0`（テキスト）のみ使用 |

- 配列は最大100件までとする
- `name` が空文字の項目は形式エラーとする
- `value` は空文字を許可する
- JSON文字列のエスケープはJSON標準に従う
- `tags` など上表以外のプロパティはネイティブ版の画面には表示しないが、読み込み後にローカルJSONを保存しても保持する。これにより、HTA版・PWA版が持つ追加情報をネイティブ版が消さないようにする

### JSON読み書き

- ローカルの `.json` ファイルは、追加・編集・削除後もJSON形式・UTF-8で上書き保存する
- URLのJSONは参照専用とし、編集・保存は行わない
- 書式不正、型不正、件数超過の場合は読み込みエラーとする
- 読み込みエラー時は、既存の一覧を空データで保存し直さない

## 4. 従来形式（互換維持）

JSON以外の参照先は、従来の3行1レコード形式として読み書きする。

```text
name
type
value
```

```text
Git Status
0
git status

Git Pull
0
git pull
```

### エスケープ仕様

従来形式の `value` は1行に保存するため、次の文字をエスケープする。

- `\` → `\\`
- 改行 → `\n`
- 復帰 → `\r`
- `,` → `\,`

JSON形式では、この独自エスケープは使用しない。

## 5. パスとURL

### ローカルファイル

アプリは次の順で参照先を解決する。

1. `csend.exe` のあるフォルダ + `\` + `pathNNN`
2. 指定された文字列を絶対パスまたは相対パスとして扱う

存在しないローカルファイルは空の一覧として扱い、保存時に参照先の形式で新規作成する。

### URL

- `http://` または `https://` で始まる参照先をURLとして扱う
- URLカテゴリは参照専用とする
- ダウンロード失敗、サイズ上限超過、JSON形式不正時はエラー表示する
- URLから取得したContent-TypeとURLパスに基づいて、ダウンロード前にデータ形式を確定する

## 6. `setting.ini` の例

```ini
[category]
count=3
item000=Git
path000=samples\en\Git.txt
item001=ローカルJSON
path001=C:\Users\example\Documents\csend\custom.json
item002=公開JSON
path002=https://example.com/c-send/category.json
```

`setting.ini` は参照先だけを管理し、JSONか従来形式かを指定するキーは持たない。

## 7. v1.9の実装範囲

- ローカル `.json` の読み込み・保存
- URLのJSON系Content-TypeによるJSON判定
- URLパス末尾 `.json` による補助判定
- JSONのUTF-8読み書き
- 従来形式との互換維持
- URLカテゴリの参照専用維持

JSON形式の追加はネイティブ版v1.9の主変更とする。
