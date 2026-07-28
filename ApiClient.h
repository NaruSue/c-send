#pragma once

#include <afx.h>

struct ApiConfig {
    CString endpoint;
    CString path;
    CString method;
    CString model;
    DWORD port;
    BOOL secure;
    DWORD timeoutMs;
    CString authType;
    CString credentialTarget;
    CString authHeader;
    CString requestTemplate;
    CString responsePath;
};

bool LoadApiConfig(ApiConfig& config);

bool ReadApiCredential(CString& apiKey);
bool WriteApiCredential(const CString& apiKey);
bool ExecuteApiAction(const CString& apiKey, const CString& prompt,
    DWORD timeoutMs, CString& result, CString& error);
