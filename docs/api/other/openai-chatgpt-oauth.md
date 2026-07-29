# OpenAI ChatGPT OAuth (experimental)

This is an experimental connection that uses the local `openai-oauth` proxy.
It is not an official OpenAI API integration. The standard OpenAI API
connection remains the recommended option for stable use.

## Setup

Install and run `openai-oauth` by following its GitHub README:

https://github.com/EvanZhouDev/openai-oauth

The usual local setup is:

```text
npx openai-oauth@latest
```

The proxy normally listens at:

```text
http://127.0.0.1:10531/v1
```

If the browser login is requested, complete the ChatGPT sign-in flow. The
credentials are managed by the local Codex/openai-oauth setup; do not enter a
ChatGPT email address, password, access token, or API key in the c-send JSON.

## c-send configuration

Copy `api/openai-chatgpt-oauth.json` into the c-send `api` directory and reload
the API list. The sample uses `authType: none` because the local proxy handles
authentication.

The model list and available usage depend on the ChatGPT account and plan.
Change the model in the sample only to a model shown by the local proxy's
`/v1/models` endpoint.

## Limitations

- The proxy and this connection are unofficial and may stop working after a
  ChatGPT or Codex change.
- The proxy must be running before c-send sends a request.
- The endpoint is loopback-only by default. Do not expose it to the network
  without adding access control.
- Use only your own ChatGPT account. Do not share or redistribute credentials.
