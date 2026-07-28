#include "stdafx.h"
#include "ApiCommand.h"
#include "ApiClient.h"

namespace {
BOOL ReadClipboardText(CString& text)
{
    text.Empty();
    if (!::OpenClipboard(NULL)) return FALSE;
    HANDLE handle = ::GetClipboardData(CF_UNICODETEXT);
    if (handle != NULL) {
        LPCWSTR value = static_cast<LPCWSTR>(::GlobalLock(handle));
#ifdef _UNICODE
        if (value != NULL) text = value;
#else
        if (value != NULL) text = CString(CW2A(value));
#endif
        if (value != NULL) ::GlobalUnlock(handle);
        ::CloseClipboard();
        return TRUE;
    }
    handle = ::GetClipboardData(CF_TEXT);
    if (handle != NULL) {
        LPCSTR value = static_cast<LPCSTR>(::GlobalLock(handle));
        if (value != NULL) text = value;
        if (value != NULL) ::GlobalUnlock(handle);
    }
    ::CloseClipboard();
    return TRUE;
}

BOOL WriteClipboardText(const CString& text)
{
    if (!::OpenClipboard(NULL)) return FALSE;
    ::EmptyClipboard();
#ifdef _UNICODE
    const UINT format = CF_UNICODETEXT;
    const SIZE_T bytes = (static_cast<SIZE_T>(text.GetLength()) + 1) * sizeof(wchar_t);
#else
    const UINT format = CF_TEXT;
    const SIZE_T bytes = static_cast<SIZE_T>(text.GetLength()) + 1;
#endif
    HGLOBAL memory = ::GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (memory == NULL) { ::CloseClipboard(); return FALSE; }
#ifdef _UNICODE
    LPWSTR target = static_cast<LPWSTR>(::GlobalLock(memory));
    if (target == NULL) { ::GlobalFree(memory); ::CloseClipboard(); return FALSE; }
    memcpy(target, text.GetString(), bytes);
#else
    LPSTR target = static_cast<LPSTR>(::GlobalLock(memory));
    if (target == NULL) { ::GlobalFree(memory); ::CloseClipboard(); return FALSE; }
    memcpy(target, text.GetString(), bytes);
#endif
    ::GlobalUnlock(memory);
    if (::SetClipboardData(format, memory) == NULL) {
        ::GlobalFree(memory); ::CloseClipboard(); return FALSE;
    }
    ::CloseClipboard();
    return TRUE;
}

CString Argument(LPWSTR* argv, int index)
{
    return argv != NULL && argv[index] != NULL ? CString(argv[index]) : CString();
}
}

BOOL IsApiCommandLine()
{
    int argc = 0;
    LPWSTR* argv = ::CommandLineToArgvW(::GetCommandLineW(), &argc);
    BOOL result = argv != NULL && argc >= 2 && Argument(argv, 1).CompareNoCase(_T("/api")) == 0;
    if (argv != NULL) ::LocalFree(argv);
    return result;
}

int RunApiCommandLine()
{
    int argc = 0;
    LPWSTR* argv = ::CommandLineToArgvW(::GetCommandLineW(), &argc);
    if (argv == NULL || argc < 3) {
        if (argv != NULL) ::LocalFree(argv);
        return 2;
    }

    const CString operation = Argument(argv, 2);
    if (operation.CompareNoCase(_T("key-status")) == 0) {
        CString key;
        BOOL configured = ReadApiCredential(key) && !key.IsEmpty();
        ::LocalFree(argv);
        return configured ? 0 : 3;
    }

    CString prompt;
    if (operation.CompareNoCase(_T("ping")) == 0) {
        prompt = _T("Reply with exactly: API_TEST_OK");
    }
    else if (operation.CompareNoCase(_T("request")) == 0 && argc >= 4) {
        prompt = Argument(argv, 3);
    }
    else if (operation.CompareNoCase(_T("clipboard")) == 0 && argc >= 4) {
        CString clipboard;
        if (!ReadClipboardText(clipboard)) { ::LocalFree(argv); return 4; }
        prompt = Argument(argv, 3);
        prompt.Replace(_T("{{clipboard}}"), clipboard);
        prompt.Replace(_T("{{input}}"), clipboard);
    }
    else {
        ::LocalFree(argv);
        return 2;
    }

    CString key;
    CString result;
    CString error;
    TCHAR mockMode[64] = {};
    BOOL mock = ::GetEnvironmentVariable(_T("CSEND_GEMINI_MOCK"), mockMode, _countof(mockMode)) > 0;
    BOOL success = (mock || ReadApiCredential(key)) &&
        ExecuteApiAction(key, prompt, 0, result, error);
    if (success && !WriteClipboardText(result)) {
        ::LocalFree(argv);
        return 5;
    }
    ::LocalFree(argv);
    return success ? 0 : 10;
}
