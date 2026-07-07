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
