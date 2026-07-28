# GitHub REST API token の取得と c-send への登録

最終確認日: 2026-07-28

対象設定は`api/github.json`です。c-send公開リポジトリの既定ブランチ名とブランチ一覧を、ボディなしGETで取得する最小サンプルです。

## 公式資料

- [GitHub REST API authentication](https://docs.github.com/en/rest/authentication/authenticating-to-the-rest-api)
- [Fine-grained personal access tokens](https://docs.github.com/en/authentication/keeping-your-account-and-data-secure/managing-your-personal-access-tokens)
- [Repository branches API](https://docs.github.com/en/rest/branches/branches)

## 登録手順

1. GitHubのSettingsからPersonal access tokenを作成する。個人利用にはfine-grained tokenを推奨し、必要最小限の対象リポジトリと権限だけを付与する。
2. 公開リポジトリだけを参照する場合でも、レート制限や将来のprivate repository利用を考慮し、用途専用のtokenを作成する。
3. c-sendのシステムメニューから「API」を開き、「GitHub REST API」の詳細を開く。
4. クレデンシャルIDが`github-rest`であることを確認し、「API token」欄へtokenを貼り付けて保存する。
5. API項目でGitHub REST API / c-send の既定ブランチ名を取得、またはブランチ一覧を取得を選択する。

`staticHeader.Accept`と`staticHeader.X-GitHub-Api-Version`はGitHub REST APIの推奨ヘッダーである。別のリポジトリに変える場合は、Action URLの`/repos/NaruSue/c-send`を`/repos/<owner>/<repo>`へ変更し、Response JSONは`default_branch`のまま利用できる。

GitHubのブランチ一覧APIは「最新ブランチ」という一意の並び順を定義しない。そのためサンプルは既定ブランチ名と全ブランチ名を別Actionで返す。更新日時などで独自に選ぶ処理が必要な場合は、API仕様と必要な判定ルールをAIへ渡して、対応可能なJSON抽出か本体の汎用機能拡張かを確認する。

## GitHub Copilotについて

GitHub CopilotのREST APIは利用状況や組織設定などの管理用で、汎用プロンプト実行APIではない。Copilotを会話／コード生成へ使う連携はSDK・CLI・OAuthを前提にするため、このJSONサンプルの対象外とする。

## AIへの依頼文

```text
GitHub公式REST API資料、添付したc-sendのdocs/api/config-spec.md、api/github.jsonを読んでください。
指定するGitHub REST APIエンドポイントをc-send用Actionへ追加してください。
GETのときはrequestを省略し、必要な固定ヘッダーはkeyConfigのstaticHeader.<ヘッダー名>で定義してください。
最小権限のfine-grained tokenに必要なrepository permissionsも示してください。tokenの実値は要求・出力しないでください。
```
