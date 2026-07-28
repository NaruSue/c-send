#include "stdafx.h"
#include "GeminiApi.h"

#include <wincred.h>
#include <wininet.h>
#include <string>
#include <vector>

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

static bool ExtractResponseText(const std::string& body, CString& result)
{
    size_t candidates = body.find("\"candidates\"");
    if (candidates == std::string::npos) return false;
    size_t textPosition = candidates;
    result.Empty();
    bool found = false;
    while ((textPosition = body.find("\"text\"", textPosition)) != std::string::npos) {
        size_t colon = body.find(':', textPosition + 6);
        if (colon == std::string::npos) break;
        ++colon;
        while (colon < body.size() && (body[colon] == ' ' || body[colon] == '\t' || body[colon] == '\r' || body[colon] == '\n')) ++colon;
        CString part;
        size_t cursor = colon;
        if (ReadJsonString(body, cursor, part)) {
            result += part;
            found = true;
        }
        textPosition = colon + 1;
    }
    return found;
}

bool ReadGeminiApiKey(CString& apiKey)
{
    apiKey.Empty();
    PCREDENTIALW credential = NULL;
    if (!CredReadW(kGeminiApiCredentialTarget, CRED_TYPE_GENERIC, 0, &credential)) {
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

bool WriteGeminiApiKey(const CString& apiKey)
{
    std::string utf8;
    if (apiKey.IsEmpty() || !CStringToUtf8(apiKey, utf8)) return false;

    CREDENTIALW credential = {};
    credential.Type = CRED_TYPE_GENERIC;
    credential.TargetName = const_cast<LPWSTR>(kGeminiApiCredentialTarget);
    credential.UserName = const_cast<LPWSTR>(L"C-Send");
    credential.CredentialBlobSize = (DWORD)utf8.size();
    credential.CredentialBlob = (LPBYTE)utf8.data();
    credential.Persist = CRED_PERSIST_LOCAL_MACHINE;
    return CredWriteW(&credential, 0) == TRUE;
}

bool ExecuteGeminiGenerateContent(const CString& apiKey, const CString& prompt,
    DWORD timeoutMs, CString& result, CString& error)
{
    result.Empty();
    error.Empty();

    std::string promptJson;
    AppendJsonString(promptJson, prompt);
    std::string requestBody = "{\"contents\":[{\"parts\":[{\"text\":" + promptJson + "}]}]}";

    HINTERNET session = InternetOpen(_T("C-Send/2.0"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (session == NULL) {
        error = _T("InternetOpen failed.");
        return false;
    }
    InternetSetOption(session, INTERNET_OPTION_CONNECT_TIMEOUT, &timeoutMs, sizeof(timeoutMs));
    InternetSetOption(session, INTERNET_OPTION_SEND_TIMEOUT, &timeoutMs, sizeof(timeoutMs));
    InternetSetOption(session, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeoutMs, sizeof(timeoutMs));

    HINTERNET connection = InternetConnect(session, _T("generativelanguage.googleapis.com"), INTERNET_DEFAULT_HTTPS_PORT,
        NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    if (connection == NULL) {
        InternetCloseHandle(session);
        error = _T("InternetConnect failed.");
        return false;
    }

    HINTERNET request = HttpOpenRequest(connection, _T("POST"),
        _T("/v1beta/models/gemini-3.5-flash-lite:generateContent"), NULL, NULL,
        NULL, INTERNET_FLAG_SECURE | INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, 0);
    if (request == NULL) {
        InternetCloseHandle(connection);
        InternetCloseHandle(session);
        error = _T("HttpOpenRequest failed.");
        return false;
    }
    InternetSetOption(request, INTERNET_OPTION_CONNECT_TIMEOUT, &timeoutMs, sizeof(timeoutMs));
    InternetSetOption(request, INTERNET_OPTION_SEND_TIMEOUT, &timeoutMs, sizeof(timeoutMs));
    InternetSetOption(request, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeoutMs, sizeof(timeoutMs));

    CString headers;
    headers.Format(_T("Content-Type: application/json\r\nx-goog-api-key: %s\r\n"), apiKey.GetString());
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
        error.Format(_T("Gemini API returned HTTP status %lu."), status);
        return false;
    }
    if (!ExtractResponseText(response, result) || result.IsEmpty()) {
        error = _T("Gemini API response did not contain generated text.");
        return false;
    }
    return true;
}
