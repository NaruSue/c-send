# ドキュメント案内

まず普通に使ってみたい場合は、次の二つだけで大丈夫です。

- [はじめて使う人へ](first-use.md): 文章を登録して、コピーして貼り付けるまでを順番に試す
- [基本の使い方](usage.md): カテゴリ、テンプレート、サンプルの使い方を確認する

## テンプレートを使いたい人

- [テンプレートを探して使う](template-catalog.md): URLから参照する方法と、ダウンロードして編集する方法
- [テンプレートの使い方](template-usage.md): 日時やクリップボードの内容を文章へ差し込む方法

## AIや外部サービスを使いたい人

- [Geminiと連携してみる](api/gemini-tutorial.md): Geminiを使った要約・添削を試す
- [Gemini API利用メモ](note-c-send-gemini-api.txt): Gemini API keyの発行からサンプル利用まで
- [API利用者向け導入手順](api/getting-started.md): 別のAIや外部サービスを追加する

## 設定を作る人・開発者向け資料

以下は、設定JSONを作る人や、c-sendを開発・拡張する人向けの技術資料です。普通に文章やコマンドを登録して使うだけなら、読む必要はありません。

- [API設定JSON仕様](api/config-spec.md)
- [API認証方式仕様](api/auth-spec.md)
- [API機能設計書](api/design.md)
- [AI連携方針・拡張設計](ai-integration.md)
- [アプリAPI設計](app-api.md)
- [setting.ini 仕様](setting-ini-spec.md)
- [データファイル仕様](data-file-spec.md)
- [テンプレート記法仕様](template-spec.md)
- [API mock test specification](api/mock-test-spec.md)
