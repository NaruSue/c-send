#pragma once

#include "ApiJson.h"
#include <vector>

struct ApiActionDocument
{
    CString id;
    CString name;
    CString method;
    CString url;
    CString description;
    ApiJsonValue request;
    ApiJsonValue response;
};

struct ApiConfigDocument
{
    CString id;
    CString name;
    CString baseUrl;
    DWORD timeoutMs;
    CString authType;
    CString credentialId;
    ApiJsonValue keyConfig;
    std::vector<ApiActionDocument> actions;

    ApiConfigDocument();
};

bool LoadApiConfigDocument(const CString& filePath, ApiConfigDocument& document, CString& error);
bool SaveApiConfigDocument(const CString& filePath, const ApiConfigDocument& document, CString& error);
