#pragma once

#include <afx.h>
#include "ApiJson.h"

struct ApiConfig {
    CString endpoint;
    CString path;
    CString method;
    DWORD port;
    BOOL secure;
    DWORD timeoutMs;
    CString authType;
    CString credentialId;
    CString authHeader;
    CString queryName;
    CString prefix;
    CString staticHeaders;
    BOOL hasRequestBody;
    ApiJsonValue requestJson;
    ApiJsonValue responseJson;

    ApiConfig() : port(443), secure(TRUE), timeoutMs(120000), hasRequestBody(FALSE) {}
};

bool LoadApiConfig(ApiConfig& config);
bool LoadApiConfigSelection(const CString& apiId, const CString& actionId, ApiConfig& config);

bool ApiRequiresCredential(const ApiConfig& config);
bool ValidateApiCredentials(const ApiConfig& config, CString& error);
CString GetApiCredentialTarget(const ApiConfig& config, const CString& field);
bool ReadApiCredential(CString& apiKey);
bool WriteApiCredential(const CString& apiKey);
bool ReadApiCredentialValue(const CString& target, CString& value);
bool WriteApiCredentialValue(const CString& target, const CString& value);
bool ExecuteApiAction(const CString& apiKey, const CString& prompt,
    DWORD timeoutMs, CString& result, CString& error, DWORD* httpStatus = NULL);
bool ExecuteApiAction(const ApiConfig& config, const CString& apiKey, const CString& prompt,
    DWORD timeoutMs, CString& result, CString& error, DWORD* httpStatus = NULL);
