# API mock test specification

## Purpose

Native API behavior is tested without sending requests to an external service or storing a real API credential. The executable uses the environment variable `CSEND_API_MOCK` to return deterministic responses from the local mock path.

## Environment variables

- `CSEND_API_MOCK=success`: 設定JSONの`response`パターンを元に、`{{value}}`へ`API_TEST_OK`を設定した有効なJSONレスポンスを返す。サービス固有のレスポンス形式へ依存しない。
- `CSEND_API_MOCK=malformed`: returns an invalid response and must exit with code `10`.
- `CSEND_API_MOCK=http401`: returns an authentication error and must exit with code `10`.
- `CSEND_API_CONFIG=<path>`: selects a temporary API configuration instead of the installed API definitions.

## Virtual configurations

`tests/api/configs/` contains one configuration for each supported authentication type:

- `none.json`
- `api-key-header.json`
- `api-key-query.json`
- `bearer.json`
- `basic.json`
- `structured-none.json`（`request`／`response`と`{{value}}`を使用する現行形式）

`tests/api/invalid/`には、必須`id`の欠落と未対応HTTPメソッドを含む設定を置く。CLIの`key-status`はこれらを設定選択失敗として終了コード`6`で拒否する。

The CI build runs `/api ping` against every configuration with `CSEND_API_MOCK=success`. The test confirms that configuration validation, request expansion, response extraction, and clipboard output work without contacting the configured endpoint.

Authentication transport details are covered by the request-building implementation; no credential is read when the mock path is active.

CI also runs `/api key-status` without storing credentials. `none` must be ready, while API-key, Bearer, and Basic configurations must report missing credentials with exit code `3`. This verifies the same preflight used to mark native list items as unavailable before execution.

Template expansion also supports `{{clipboard|fallback text}}`; when the captured clipboard is empty, `fallback text` is used.
