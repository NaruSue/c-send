# c-send リリース運用

## ブランチ構成

- master
  - リリース済み安定版

- develop/x.y
  - 次期バージョン開発

- feature/*
  - 個別機能開発

## バージョン管理

version.h をバージョン管理の正本とする。

例:

```cpp
#define CSEND_VERSION_MAJOR 1
#define CSEND_VERSION_MINOR 5
#define CSEND_VERSION_PATCH 0
#define CSEND_VERSION_TEXT "1.5.0"
```

利用箇所:

- About画面
- VERSIONINFO
- GitHub Release名
- ZIPファイル名

## リリース手順

1. feature ブランチを develop にマージ
2. develop を master にマージ
3. version.h を更新
4. GitHub Actions を手動実行

## GitHub Actions

実行内容:

1. version.h からバージョン取得
2. 同一タグの存在確認
3. Releaseビルド
4. ビルド成功時のみタグ作成
5. GitHub Release 作成
6. ZIP添付

ZIP に含める主なファイル:

- `csend.exe`
- `README.md`
- `LICENSE`
- `docs/usage.md`

## リリース差分

### v1.6 -> v1.7.0

ユーザー向けの変更:

- カテゴリ設定画面の見た目とラベルを調整
- カテゴリの新規作成手順を `docs/usage.md` に追加
- README を読みやすく整理
- タスクトレイのメニュー操作を分かりやすく改善
- 単一起動時の復帰動作を改善
- コピー時の通知まわりを改善し、アプリ内 tip と toast の扱いを整理
- 入力ダイアログやカテゴリ設定画面の表示崩れを調整

内部改善:

- カテゴリ選択処理を整理
- エラー状態の表示を改善
- `setting.ini` 関連のバリデーションと補完を強化
- 生成物 `csend.aps` を管理対象外に整理
- リリース ZIP に `docs/usage.md` を含めるよう変更

## 命名規則

タグ:

v1.5.0

Release:

v1.5.0

ZIP:

c-send-1.5.0.zip

## 今後の課題

- GitHub Actions自動化
- changelog自動生成
- Release Notes自動生成
- バージョン整合性チェック
