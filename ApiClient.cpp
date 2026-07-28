#include "stdafx.h"
#include "ApiClient.h"

#include <wincred.h>
#include <wininet.h>
#include <string>
#include <vector>
#include <fstream>

#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "wininet.lib")

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

static void AppendJsonString(std::string& output, const CString& value)
{
    std::string utf8;
    if (!CStringToUtf8(value, utf8)) return;
    output += '"';
    for (size_t i = 0; i < utf8.size(); ++i) {
        unsigned char c = (unsigned char)utf8[i];
        switch (c) {
        case '"': output += "\\\""; break;
        case '\\': output += "\\\\"; break;
        case '\b': output += "\\b"; break;
        case '\f': output += "\\f"; break;
        case '\n': output += "\\n"; break;
        case '\r': output += "\\r"; break;
        case '\t': output += "\\t"; break;
        default: output += (char)c; break;
        }
    }
    output += '"';
}

static bool ReadJsonString(const std::string& text, size_t& position, CString& value)
{
    if (position >= text.size() || text[position] != '"') return false;
    ++position;
    std::string decoded;
    while (position < text.size()) {
        unsigned char c = (unsigned char)text[position++];
        if (c == '"') return Utf8ToCString(decoded, value);
        if (c != '\\') {
            decoded += (char)c;
            continue;
        }
        if (position >= text.size()) return false;
        char escaped = text[position++];
        switch (escaped) {
        case '"': decoded += '"'; break;
        case '\\': decoded += '\\'; break;
        case '/': decoded += '/'; break;
        case 'b': decoded += '\b'; break;
        case 'f': decoded += '\f'; break;
        case 'n': decoded += '\n'; break;
        case 'r': decoded += '\r'; break;
        case 't': decoded += '\t'; break;
        default: return false;
        }
    }
    return false;
}

static bool ExtractResponseText(const std::string& body, const CString& responsePath, CString& result)
{
    CString path = responsePath;
    int separator = path.ReverseFind(_T('.'));
    if (separator >= 0) path = path.Mid(separator + 1);
    path.Replace(_T("[]"), _T(""));
    CStringA pathA(path);
    std::string key = "\"";
    key += pathA.GetString();
    key += "\"";
    size_t textPosition = body.find(key);
    if (textPosition == std::string::npos) return false;
    result.Empty();
    bool found = false;
    while (textPosition != std::string::npos) {
        size_t colon = body.find(':', textPosition + key.size());
        if (colon == std::string::npos) break;
        ++colon;
        while (colon < body.size() && (body[colon] == ' ' || body[colon] == '\t' || body[colon] == '\r' || body[colon] == '\n')) ++colon;
        CString part;
        size_t cursor = colon;
        if (ReadJsonString(body, cursor, part)) {
            result += part;
            found = true;
        }
        textPosition = body.find(key, colon + 1);
    }
    return found;
}

static bool ReadConfigString(const std::string& text, const char* name, CString& value)
{
    std::string key = "\"";
    key += name;
    key += "\"";
    size_t position = text.find(key);
    if (position == std::string::npos) return false;
    position = text.find(':', position + key.size());
    if (position == std::string::npos) return false;
    ++position;
    while (position < text.size() && (text[position] == ' ' || text[position] == '\t' || text[position] == '\r' || text[position] == '\n')) ++position;
    return ReadJsonString(text, position, value);
}

static bool ReadConfigNumber(const std::string& text, const char* name, DWORD& value)
{
    std::string key = "\"";
    key += name;
    key += "\"";
    size_t position = text.find(key);
    if (position == std::string::npos) return false;
    position = text.find(':', position + key.size());
    if (position == std::string::npos) return false;
    ++position;
    while (position < text.size() && (text[position] == ' ' || text[position] == '\t' || text[position] == '\r' || text[position] == '\n')) ++position;
    char* end = NULL;
    unsigned long parsed = strtoul(text.c_str() + position, &end, 10);
    if (end == text.c_str() + position || parsed > 0xFFFFFFFFUL) return false;
    value = (DWORD)parsed;
    return true;
}

static bool ReadConfigBool(const std::string& text, const char* name, BOOL& value)
{
    std::string key = "\"";
    key += name;
    key += "\"";
    size_t position = text.find(key);
    if (position == std::string::npos) return false;
    position = text.find(':', position + key.size());
    if (position == std::string::npos) return false;
    ++position;
    while (position < text.size() && (text[position] == ' ' || text[position] == '\t' || text[position] == '\r' || text[position] == '\n')) ++position;
    if (text.compare(position, 4, "true") == 0) { value = TRUE; return true; }
    if (text.compare(position, 5, "false") == 0) { value = FALSE; return true; }
    return false;
}

static CString GetConfigFilePath()
{
    TCHAR modulePath[MAX_PATH] = {};
    ::GetModuleFileName(NULL, modulePath, _countof(modulePath));
    CString path(modulePath);
    int separator = path.ReverseFind(_T('\\'));
    if (separator >= 0) path = path.Left(separator);
    return path + _T("\\api\\gemini.json");
}

bool LoadApiConfig(ApiConfig& config)
{
    config = ApiConfig();
    CStringA configPath(GetConfigFilePath());
    std::ifstream file(configPath.GetString(), std::ios::binary);
    if (!file) return false;
    std::string text((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    if (!ReadConfigString(text, "endpoint", config.endpoint) ||
        !ReadConfigString(text, "path", config.path) ||
        !ReadConfigString(text, "method", config.method) ||
        !ReadConfigString(text, "model", config.model) ||
        !ReadConfigNumber(text, "port", config.port) ||
        !ReadConfigBool(text, "secure", config.secure) ||
        !ReadConfigNumber(text, "timeoutMs", config.timeoutMs) ||
        !ReadConfigString(text, "authType", config.authType) ||
        !ReadConfigString(text, "credentialTarget", config.credentialTarget) ||
        !ReadConfigString(text, "authHeader", config.authHeader) ||
        !ReadConfigString(text, "requestTemplate", config.requestTemplate) ||
        !ReadConfigString(text, "responsePath", config.responsePath)) return false;
    config.path.Replace(_T("{model}"), config.model);
    return !config.endpoint.IsEmpty() && !config.path.IsEmpty() && !config.method.IsEmpty() &&
        !config.authType.IsEmpty() && !config.credentialTarget.IsEmpty() && !config.authHeader.IsEmpty() &&
        !config.requestTemplate.IsEmpty() && !config.responsePath.IsEmpty() && config.port != 0;
}

bool ReadApiCredential(CString& apiKey)
{
    apiKey.Empty();
    ApiConfig config;
    if (!LoadApiConfig(config)) return false;
    CStringW targetW(config.credentialTarget);
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

bool WriteApiCredential(const CString& apiKey)
{
    ApiConfig config;
    if (!LoadApiConfig(config)) return false;
    std::string utf8;
    if (apiKey.IsEmpty() || !CStringToUtf8(apiKey, utf8)) return false;

    CREDENTIALW credential = {};
    credential.Type = CRED_TYPE_GENERIC;
    CStringW targetW(config.credentialTarget);
    credential.TargetName = const_cast<LPWSTR>(targetW.GetString());
    credential.UserName = const_cast<LPWSTR>(L"C-Send");
    credential.CredentialBlobSize = (DWORD)utf8.size();
    credential.CredentialBlob = (LPBYTE)utf8.data();
    credential.Persist = CRED_PERSIST_LOCAL_MACHINE;
    return CredWriteW(&credential, 0) == TRUE;
}

bool ExecuteApiAction(const CString& apiKey, const CString& prompt,
    DWORD timeoutMs, CString& result, CString& error)
{
    result.Empty();
    error.Empty();

    ApiConfig config;
    if (!LoadApiConfig(config)) {
        error = _T("API configuration could not be loaded.");
        return false;
    }
    DWORD effectiveTimeoutMs = config.timeoutMs != 0 ? config.timeoutMs : timeoutMs;

    TCHAR mockMode[64] = {};
    DWORD mockLength = ::GetEnvironmentVariable(_T("CSEND_GEMINI_MOCK"), mockMode, _countof(mockMode));
    if (mockLength > 0 && mockLength < _countof(mockMode)) {
        CString mode(mockMode, (int)mockLength);
        std::string response;
        if (mode.CompareNoCase(_T("success")) == 0) {
            response = "{\"candidates\":[{\"content\":{\"parts\":[{\"text\":\"API_TEST_OK\"}]}}]}";
        }
        else if (mode.CompareNoCase(_T("malformed")) == 0) {
            response = "{\"candidates\":[{\"content\":{\"parts\":[{\"text\":}]}]}";
        }
        else if (mode.CompareNoCase(_T("http401")) == 0) {
            error = _T("Gemini API returned HTTP status 401.");
            return false;
        }
        else if (mode.CompareNoCase(_T("timeout")) == 0) {
            error = _T("Gemini API request timed out.");
            return false;
        }
        else {
            error = _T("Unknown Gemini API mock mode.");
            return false;
        }
        if (!ExtractResponseText(response, config.responsePath, result) || result.IsEmpty()) {
            error = _T("API response did not contain the configured result.");
            return false;
        }
        return true;
    }

    std::string promptJson;
    AppendJsonString(promptJson, prompt);
    std::string requestBody;
    if (!CStringToUtf8(config.requestTemplate, requestBody)) {
        error = _T("API request template could not be encoded.");
        return false;
    }
    const std::string promptToken = "{{prompt_json}}";
    size_t tokenPosition = requestBody.find(promptToken);
    if (tokenPosition == std::string::npos) {
        error = _T("API request template does not contain {{prompt_json}}.");
        return false;
    }
    requestBody.replace(tokenPosition, promptToken.size(), promptJson);

    if (config.authType.CompareNoCase(_T("credential-header")) != 0) {
        error = _T("The configured API authentication type is not supported yet.");
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
    HINTERNET request = HttpOpenRequestA(connection, methodA.GetString(), pathA.GetString(), NULL, NULL,
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

    CString headers;
    headers.Format(_T("Content-Type: application/json\r\n%s: %s\r\n"), config.authHeader.GetString(), apiKey.GetString());
    CStringA headerA(headers);
    BOOL sent = HttpSendRequestA(request, headerA.GetString(), -1,
        (LPVOID)requestBody.data(), (DWORD)requestBody.size());
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
    if (!ExtractResponseText(response, config.responsePath, result) || result.IsEmpty()) {
        error = _T("API response did not contain the configured result.");
        return false;
    }
    return true;
}
