#pragma once

#include <afx.h>

// The credential target is intentionally stable so the value can be shared
// by future API settings UI without putting the secret in a data file.
static const wchar_t kGeminiApiCredentialTarget[] = L"C-Send/GeminiAPIKey";

struct GeminiApiConfig {
    CString endpoint;
    CString path;
    CString method;
    CString model;
    DWORD port;
    BOOL secure;
    DWORD timeoutMs;
};

bool LoadGeminiApiConfig(GeminiApiConfig& config);

bool ReadGeminiApiKey(CString& apiKey);
bool WriteGeminiApiKey(const CString& apiKey);
bool ExecuteGeminiGenerateContent(const CString& apiKey, const CString& prompt,
    DWORD timeoutMs, CString& result, CString& error);
