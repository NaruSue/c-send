#include "stdafx.h"
#include "ApiClient.h"
#include "ApiCatalog.h"
#include "ApiConfigDocument.h"

#include <wincred.h>
#include <wininet.h>
#include <string>
#include <vector>
#include <fstream>
#include <wincrypt.h>

#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "crypt32.lib")

static bool CStringToUtf8(const CString& input, std::string& output)
{
#ifdef _UNICODE
    const wchar_t* wide = input.GetString();
    int wideLength = input.GetLength();
#else
    int wideLength = MultiByteToWideChar(CP_ACP, 0, input.GetString(), input.GetLength(), NULL, 0);
    if (wideLength < 0) return false;
    std::vector<wchar_t> buffer((size_t)wideLength);
    if (wideLength > 0 && MultiByteToWideChar(CP_ACP, 0, input.GetString(), input.GetLength(), buffer.data(), wideLength) <= 0) return false;
    const wchar_t* wide = buffer.empty() ? L"" : buffer.data();
#endif
    if (wideLength == 0) {
        output.clear();
        return true;
    }
    int utf8Length = WideCharToMultiByte(CP_UTF8, 0, wide, wideLength, NULL, 0, NULL, NULL);
    if (utf8Length <= 0) return false;
    output.resize((size_t)utf8Length);
    return WideCharToMultiByte(CP_UTF8, 0, wide, wideLength, &output[0], utf8Length, NULL, NULL) > 0;
}

static bool Utf8ToCString(const std::string& input, CString& output)
{
    if (input.empty()) {
        output.Empty();
        return true;
    }
    int wideLength = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, input.data(), (int)input.size(), NULL, 0);
    if (wideLength <= 0) return false;
    std::vector<wchar_t> wide((size_t)wideLength);
    if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, input.data(), (int)input.size(), wide.data(), wideLength) <= 0) return false;
#ifdef _UNICODE
    output.SetString(wide.data(), wideLength);
#else
    int ansiLength = WideCharToMultiByte(CP_ACP, 0, wide.data(), wideLength, NULL, 0, NULL, NULL);
    if (ansiLength <= 0) return false;
    std::vector<char> ansi((size_t)ansiLength);
    if (WideCharToMultiByte(CP_ACP, 0, wide.data(), wideLength, ansi.data(), ansiLength, NULL, NULL) <= 0) return false;
    output.SetString(ansi.data(), ansiLength);
#endif
    return true;
}

static bool GetConfigFilePath(CString& filePath)
{
    filePath.Empty();
    TCHAR overridePath[MAX_PATH] = {};
    DWORD overrideLength = ::GetEnvironmentVariable(_T("CSEND_API_CONFIG"), overridePath, _countof(overridePath));
    if (overrideLength > 0 && overrideLength < _countof(overridePath)) {
        filePath = overridePath;
        return true;
    }

    std::vector<ApiDefinitionSummary> definitions;
    if (!LoadApiDefinitions(definitions) || definitions.size() != 1) return false;
    filePath = definitions[0].filePath;
    return !filePath.IsEmpty();
}

static bool LoadApiConfigFile(const CString& filePath, const CString& actionId, ApiConfig& config)
{
    config = ApiConfig();
    ApiConfigDocument document;
    CString loadError;
    if (!LoadApiConfigDocument(filePath, document, loadError)) return false;
    const ApiActionDocument* action = NULL;
    for (size_t index = 0; index < document.actions.size(); ++index) {
        if ((actionId.IsEmpty() && index == 0) ||
            document.actions[index].id.CompareNoCase(actionId) == 0) {
            action = &document.actions[index];
            break;
        }
    }
    if (action == NULL) return false;

    URL_COMPONENTS components = {};
    components.dwStructSize = sizeof(components);
    TCHAR host[512] = {};
    TCHAR urlPath[2048] = {};
    components.lpszHostName = host;
    components.dwHostNameLength = _countof(host);
    components.lpszUrlPath = urlPath;
    components.dwUrlPathLength = _countof(urlPath);
    if (!InternetCrackUrl(document.baseUrl, 0, 0, &components)) return false;
    config.endpoint.SetString(host, components.dwHostNameLength);
    config.secure = components.nScheme == INTERNET_SCHEME_HTTPS;
    config.port = components.nPort;
    CString basePath;
    basePath.SetString(urlPath, components.dwUrlPathLength);
    basePath.TrimRight(_T('/'));
    CString actionPath(action->url);
    if (!actionPath.IsEmpty() && actionPath[0] != _T('/')) actionPath = _T("/") + actionPath;
    config.path = basePath + actionPath;
    if (config.path.IsEmpty()) config.path = _T("/");
    config.method = action->method;
    config.timeoutMs = document.timeoutMs;
    config.authType = document.authType;
    config.credentialId = document.credentialId;
    const ApiJsonValue* header = document.keyConfig.Find(_T("headerName"));
    const ApiJsonValue* query = document.keyConfig.Find(_T("queryName"));
    const ApiJsonValue* prefix = document.keyConfig.Find(_T("prefix"));
    if (header != NULL && header->type == ApiJsonValue::TYPE_STRING) config.authHeader = header->scalar;
    if (query != NULL && query->type == ApiJsonValue::TYPE_STRING) config.queryName = query->scalar;
    if (prefix != NULL && prefix->type == ApiJsonValue::TYPE_STRING) config.prefix = prefix->scalar;
    const CString staticHeaderPrefix = _T("staticHeader.");
    for (size_t index = 0; index < document.keyConfig.keys.size(); ++index) {
        const CString& key = document.keyConfig.keys[index];
        const ApiJsonValue& value = document.keyConfig.children[index];
        if (key.Left(staticHeaderPrefix.GetLength()).CompareNoCase(staticHeaderPrefix) == 0 &&
            value.type == ApiJsonValue::TYPE_STRING) {
            CString headerName = key.Mid(staticHeaderPrefix.GetLength());
            config.staticHeaders += headerName + _T(": ") + value.scalar + _T("\r\n");
        }
    }
    config.requestJson = action->request;
    config.hasRequestBody = action->request.type != ApiJsonValue::TYPE_NULL;
    config.responseJson = action->response;
    if (config.authType.CompareNoCase(_T("api-key-header")) == 0) {
        if (config.credentialId.IsEmpty()) return false;
        if (config.authHeader.IsEmpty()) return false;
    }
    else if (config.authType.CompareNoCase(_T("api-key-query")) == 0) {
        if (config.credentialId.IsEmpty()) return false;
        if (config.queryName.IsEmpty()) return false;
    }
    else if (config.authType.CompareNoCase(_T("bearer")) == 0) {
        if (config.credentialId.IsEmpty()) return false;
        if (config.authHeader.IsEmpty()) config.authHeader = _T("Authorization");
        if (config.prefix.IsEmpty()) config.prefix = _T("Bearer ");
    }
    else if (config.authType.CompareNoCase(_T("basic")) == 0) {
        if (config.credentialId.IsEmpty()) return false;
    }
    else if (config.authType.CompareNoCase(_T("none")) != 0) return false;
    return !config.endpoint.IsEmpty() && !config.path.IsEmpty() && !config.method.IsEmpty() &&
        !config.authType.IsEmpty() && (!config.hasRequestBody || ApiJsonContainsValueMarker(config.requestJson)) &&
        ApiJsonContainsValueMarker(config.responseJson) && config.port != 0;
}

bool LoadApiConfig(ApiConfig& config)
{
    CString filePath;
    return GetConfigFilePath(filePath) && LoadApiConfigFile(filePath, CString(), config);
}

bool LoadApiConfigSelection(const CString& apiId, const CString& actionId, ApiConfig& config)
{
    if (apiId.IsEmpty()) return LoadApiConfig(config);
    std::vector<ApiDefinitionSummary> definitions;
    if (!LoadApiDefinitions(definitions)) return false;
    for (size_t index = 0; index < definitions.size(); ++index) {
        if (definitions[index].id.CompareNoCase(apiId) == 0) {
            return LoadApiConfigFile(definitions[index].filePath, actionId, config);
        }
    }
    return false;
}

bool ApiRequiresCredential(const ApiConfig& config)
{
    return config.authType.CompareNoCase(_T("none")) != 0;
}

CString GetApiCredentialTarget(const ApiConfig& config, const CString& field)
{
    CString target;
    target.Format(_T("C-Send/API/%s/%s"), config.credentialId.GetString(), field.GetString());
    return target;
}

bool ValidateApiCredentials(const ApiConfig& config, CString& error)
{
    error.Empty();
    if (!ApiRequiresCredential(config)) return true;
    if (config.credentialId.IsEmpty()) {
        error = _T("ƒNƒŒƒfƒ“ƒVƒƒƒ‹ID‚ªÝ’è‚³‚ê‚Ä‚¢‚Ü‚¹‚ñB");
        return false;
    }
    if (config.authType.CompareNoCase(_T("basic")) == 0) {
        CString username;
        CString password;
        const bool haveUsername =
            ReadApiCredentialValue(GetApiCredentialTarget(config, _T("username")), username) &&
            !username.IsEmpty();
        const bool havePassword =
            ReadApiCredentialValue(GetApiCredentialTarget(config, _T("password")), password) &&
            !password.IsEmpty();
        if (!haveUsername && !havePassword) {
            error = _T("Basic”FØ‚ÌID‚ÆPASS‚ª“o˜^‚³‚ê‚Ä‚¢‚Ü‚¹‚ñB");
            return false;
        }
        if (!haveUsername) {
            error = _T("Basic”FØ‚ÌID‚ª“o˜^‚³‚ê‚Ä‚¢‚Ü‚¹‚ñB");
            return false;
        }
        if (!havePassword) {
            error = _T("Basic”FØ‚ÌPASS‚ª“o˜^‚³‚ê‚Ä‚¢‚Ü‚¹‚ñB");
            return false;
        }
        return true;
    }
    CString token;
    if (!ReadApiCredentialValue(GetApiCredentialTarget(config, _T("token")), token) ||
        token.IsEmpty()) {
        error = _T("API token‚ª“o˜^‚³‚ê‚Ä‚¢‚Ü‚¹‚ñB");
        return false;
    }
    return true;
}

bool ReadApiCredentialValue(const CString& target, CString& apiKey)
{
    apiKey.Empty();
    CStringW targetW(target);
    PCREDENTIALW credential = NULL;
    if (!CredReadW(targetW.GetString(), CRED_TYPE_GENERIC, 0, &credential)) {
        return false;
    }

    if (credential->CredentialBlobSize > 0 && credential->CredentialBlob != NULL) {
        std::string utf8((const char*)credential->CredentialBlob,
            (size_t)credential->CredentialBlobSize);
        Utf8ToCString(utf8, apiKey);
    }
    CredFree(credential);
    return !apiKey.IsEmpty();
}

bool WriteApiCredentialValue(const CString& target, const CString& apiKey)
{
    std::string utf8;
    if (apiKey.IsEmpty() || !CStringToUtf8(apiKey, utf8)) return false;

    CREDENTIALW credential = {};
    credential.Type = CRED_TYPE_GENERIC;
    CStringW targetW(target);
    credential.TargetName = const_cast<LPWSTR>(targetW.GetString());
    credential.UserName = const_cast<LPWSTR>(L"C-Send");
    credential.CredentialBlobSize = (DWORD)utf8.size();
    credential.CredentialBlob = (LPBYTE)utf8.data();
    credential.Persist = CRED_PERSIST_LOCAL_MACHINE;
    return CredWriteW(&credential, 0) == TRUE;
}

bool ReadApiCredential(CString& apiKey)
{
    ApiConfig config;
    return LoadApiConfig(config) &&
        ReadApiCredentialValue(GetApiCredentialTarget(config, _T("token")), apiKey);
}

bool WriteApiCredential(const CString& apiKey)
{
    ApiConfig config;
    return LoadApiConfig(config) &&
        WriteApiCredentialValue(GetApiCredentialTarget(config, _T("token")), apiKey);
}

static CString UrlEncodeCredential(const CString& value)
{
    CStringA input(value);
    CStringA encoded;
    const char hex[] = "0123456789ABCDEF";
    for (int i = 0; i < input.GetLength(); ++i) {
        unsigned char c = (unsigned char)input[i];
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' || c == '~') {
            encoded += (char)c;
        }
        else {
            encoded += '%';
            encoded += hex[(c >> 4) & 0x0F];
            encoded += hex[c & 0x0F];
        }
    }
    return CString(encoded);
}

static bool MakeBasicCredential(const CString& username, const CString& password, CString& encoded)
{
    CStringA plain(username + _T(":" ) + password);
    DWORD required = 0;
    if (!CryptBinaryToStringA((const BYTE*)plain.GetString(), plain.GetLength(),
        CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, NULL, &required)) return false;
    std::vector<char> buffer(required + 1, 0);
    if (!CryptBinaryToStringA((const BYTE*)plain.GetString(), plain.GetLength(),
        CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, buffer.data(), &required)) return false;
    encoded = CString(buffer.data());
    return true;
}

static bool ExtractStructuredResponse(const std::string& response,
    const ApiJsonValue& pattern, CString& result)
{
    ApiJsonValue actual;
    CString parseError;
    if (!ParseApiJsonUtf8(response, actual, parseError)) return false;
    std::vector<CString> values;
    if (!ApiJsonExtractValues(pattern, actual, values) || values.empty()) return false;
    result.Empty();
    for (size_t index = 0; index < values.size(); ++index) result += values[index];
    return !result.IsEmpty();
}

bool ExecuteApiAction(const ApiConfig& config, const CString& apiKey, const CString& prompt,
    DWORD timeoutMs, CString& result, CString& error, DWORD* httpStatus)
{
    result.Empty();
    error.Empty();
    if (httpStatus != NULL) *httpStatus = 0;
    DWORD effectiveTimeoutMs = config.timeoutMs != 0 ? config.timeoutMs : timeoutMs;

    TCHAR mockMode[64] = {};
    DWORD mockLength = ::GetEnvironmentVariable(_T("CSEND_API_MOCK"), mockMode, _countof(mockMode));
    if (mockLength > 0 && mockLength < _countof(mockMode)) {
        CString mode(mockMode, (int)mockLength);
        std::string response;
        if (mode.CompareNoCase(_T("success")) == 0) {
            ApiJsonValue mockResponse = config.responseJson;
            ApiJsonReplaceValueMarker(mockResponse, _T("API_TEST_OK"));
            if (!SerializeApiJsonUtf8(mockResponse, response, FALSE)) {
                error = _T("API mock response could not be encoded.");
                return false;
            }
        }
        else if (mode.CompareNoCase(_T("malformed")) == 0) {
            response = "{\"candidates\":[{\"content\":{\"parts\":[{\"text\":}]}]}";
        }
        else if (mode.CompareNoCase(_T("http401")) == 0) {
            if (httpStatus != NULL) *httpStatus = 401;
            error = _T("API returned HTTP status 401.");
            return false;
        }
        else if (mode.CompareNoCase(_T("timeout")) == 0) {
            error = _T("API request timed out.");
            return false;
        }
        else {
            error = _T("Unknown API mock mode.");
            return false;
        }
        bool extracted = ExtractStructuredResponse(response, config.responseJson, result);
        if (!extracted || result.IsEmpty()) {
            error = _T("API response did not contain the configured result.");
            return false;
        }
        if (httpStatus != NULL) *httpStatus = 200;
        return true;
    }

    std::string requestBody;
    if (config.hasRequestBody) {
        ApiJsonValue requestJson = config.requestJson;
        ApiJsonReplaceValueMarker(requestJson, prompt);
        if (!SerializeApiJsonUtf8(requestJson, requestBody, FALSE)) {
            error = _T("API request JSON could not be encoded.");
            return false;
        }
    }

    CString credential = apiKey;
    CString headers;
    CString query;
    if (config.authType.CompareNoCase(_T("none")) == 0) {
        // No authentication header or query is added.
    }
    else if (config.authType.CompareNoCase(_T("api-key-header")) == 0 ||
        config.authType.CompareNoCase(_T("bearer")) == 0) {
        if (credential.IsEmpty() && !ReadApiCredentialValue(GetApiCredentialTarget(config, _T("token")), credential)) {
            error = _T("API credential is not configured.");
            return false;
        }
        headers.Format(_T("%s: %s%s\r\n"), config.authHeader.GetString(), config.prefix.GetString(), credential.GetString());
    }
    else if (config.authType.CompareNoCase(_T("api-key-query")) == 0) {
        if (credential.IsEmpty() && !ReadApiCredentialValue(GetApiCredentialTarget(config, _T("token")), credential)) {
            error = _T("API credential is not configured.");
            return false;
        }
        query.Format(_T("%s=%s"), config.queryName.GetString(), UrlEncodeCredential(config.prefix + credential).GetString());
    }
    else if (config.authType.CompareNoCase(_T("basic")) == 0) {
        CString username;
        CString password;
        if (!ReadApiCredentialValue(GetApiCredentialTarget(config, _T("username")), username) ||
            !ReadApiCredentialValue(GetApiCredentialTarget(config, _T("password")), password)) {
            error = _T("Basic authentication credentials are not configured.");
            return false;
        }
        CString encoded;
        if (!MakeBasicCredential(username, password, encoded)) {
            error = _T("Basic authentication credentials could not be encoded.");
            return false;
        }
        headers.Format(_T("Authorization: Basic %s\r\n"), encoded.GetString());
    }
    else {
        error = _T("The configured API authentication type is not supported.");
        return false;
    }

    HINTERNET session = InternetOpen(_T("C-Send/2.0"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (session == NULL) {
        error = _T("InternetOpen failed.");
        return false;
    }
    InternetSetOption(session, INTERNET_OPTION_CONNECT_TIMEOUT, &effectiveTimeoutMs, sizeof(effectiveTimeoutMs));
    InternetSetOption(session, INTERNET_OPTION_SEND_TIMEOUT, &effectiveTimeoutMs, sizeof(effectiveTimeoutMs));
    InternetSetOption(session, INTERNET_OPTION_RECEIVE_TIMEOUT, &effectiveTimeoutMs, sizeof(effectiveTimeoutMs));

    CStringA endpointA(config.endpoint);
    HINTERNET connection = InternetConnectA(session, endpointA.GetString(), (INTERNET_PORT)config.port,
        NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    if (connection == NULL) {
        InternetCloseHandle(session);
        error = _T("InternetConnect failed.");
        return false;
    }

    DWORD requestFlags = INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE;
    if (config.secure) requestFlags |= INTERNET_FLAG_SECURE;
    CStringA methodA(config.method);
    CStringA pathA(config.path);
    CString requestPath(config.path);
    if (!query.IsEmpty()) requestPath += _T("?") + query;
    CStringA requestPathA(requestPath);
    HINTERNET request = HttpOpenRequestA(connection, methodA.GetString(), requestPathA.GetString(), NULL, NULL,
        NULL, requestFlags, 0);
    if (request == NULL) {
        InternetCloseHandle(connection);
        InternetCloseHandle(session);
        error = _T("HttpOpenRequest failed.");
        return false;
    }
    InternetSetOption(request, INTERNET_OPTION_CONNECT_TIMEOUT, &effectiveTimeoutMs, sizeof(effectiveTimeoutMs));
    InternetSetOption(request, INTERNET_OPTION_SEND_TIMEOUT, &effectiveTimeoutMs, sizeof(effectiveTimeoutMs));
    InternetSetOption(request, INTERNET_OPTION_RECEIVE_TIMEOUT, &effectiveTimeoutMs, sizeof(effectiveTimeoutMs));

    headers = config.staticHeaders + headers;
    if (config.hasRequestBody) headers = _T("Content-Type: application/json\r\n") + headers;
    CStringA headerA(headers);
    BOOL sent = HttpSendRequestA(request, headerA.GetString(), -1,
        config.hasRequestBody ? (LPVOID)requestBody.data() : NULL,
        config.hasRequestBody ? (DWORD)requestBody.size() : 0);
    if (!sent) {
        error = _T("HttpSendRequest failed.");
        InternetCloseHandle(request);
        InternetCloseHandle(connection);
        InternetCloseHandle(session);
        return false;
    }

    DWORD status = 0;
    DWORD statusSize = sizeof(status);
    HttpQueryInfo(request, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &status, &statusSize, NULL);
    if (httpStatus != NULL) *httpStatus = status;

    std::string response;
    char buffer[4096];
    DWORD read = 0;
    while (InternetReadFile(request, buffer, sizeof(buffer), &read) && read > 0) {
        response.append(buffer, read);
    }

    InternetCloseHandle(request);
    InternetCloseHandle(connection);
    InternetCloseHandle(session);

    if (status < 200 || status >= 300) {
        error.Format(_T("API returned HTTP status %lu."), status);
        return false;
    }
    bool extracted = ExtractStructuredResponse(response, config.responseJson, result);
    if (!extracted || result.IsEmpty()) {
        error = _T("API response did not contain the configured result.");
        return false;
    }
    return true;
}

bool ExecuteApiAction(const CString& apiKey, const CString& prompt,
    DWORD timeoutMs, CString& result, CString& error, DWORD* httpStatus)
{
    ApiConfig config;
    if (!LoadApiConfig(config)) {
        error = _T("API configuration could not be loaded.");
        return false;
    }
    return ExecuteApiAction(config, apiKey, prompt, timeoutMs, result, error, httpStatus);
}
