#include "stdafx.h"
#include "ApiConfigDocument.h"

#include <fstream>
#include <string>

ApiConfigDocument::ApiConfigDocument()
    : timeoutMs(120000), authType(_T("none")), keyConfig(ApiJsonValue::Object())
{
}

static CString JsonString(const ApiJsonValue& root, const CString& key)
{
    const ApiJsonValue* value = root.Find(key);
    return value != NULL && value->type == ApiJsonValue::TYPE_STRING ? value->scalar : CString();
}

static bool JsonPositiveNumber(const ApiJsonValue& root, const CString& key, DWORD& value)
{
    const ApiJsonValue* property = root.Find(key);
    if (property == NULL || property->type != ApiJsonValue::TYPE_NUMBER) return false;
    DWORD parsed = (DWORD)_tcstoul(property->scalar, NULL, 10);
    if (parsed == 0) return false;
    value = parsed;
    return true;
}

static bool IsSupportedAuthType(const CString& authType)
{
    return authType == _T("none") || authType == _T("api-key-header") ||
        authType == _T("api-key-query") || authType == _T("bearer") ||
        authType == _T("basic");
}

static bool IsSupportedMethod(const CString& method)
{
    return method == _T("GET") || method == _T("POST") ||
        method == _T("PUT") || method == _T("PATCH") || method == _T("DELETE");
}

static bool HasNonEmptyString(const ApiJsonValue& object, const CString& key)
{
    const ApiJsonValue* value = object.Find(key);
    return value != NULL && value->type == ApiJsonValue::TYPE_STRING && !value->scalar.IsEmpty();
}

static bool HasUnsafeHeaderText(const CString& value)
{
    return value.Find(_T('\r')) >= 0 || value.Find(_T('\n')) >= 0;
}

static bool ValidateStaticHeaders(const ApiJsonValue& keyConfig)
{
    const CString prefix = _T("staticHeader.");
    for (size_t index = 0; index < keyConfig.keys.size(); ++index) {
        const CString& key = keyConfig.keys[index];
        if (key.Left(prefix.GetLength()).CompareNoCase(prefix) != 0) continue;
        const ApiJsonValue& value = keyConfig.children[index];
        CString name = key.Mid(prefix.GetLength());
        if (name.IsEmpty() || value.type != ApiJsonValue::TYPE_STRING ||
            value.scalar.IsEmpty() || HasUnsafeHeaderText(name) ||
            HasUnsafeHeaderText(value.scalar)) return false;
    }
    return true;
}

static bool LoadAction(const ApiJsonValue& value, ApiActionDocument& action)
{
    if (value.type != ApiJsonValue::TYPE_OBJECT) return false;
    action.id = JsonString(value, _T("id"));
    action.name = JsonString(value, _T("name"));
    action.method = JsonString(value, _T("method"));
    action.url = JsonString(value, _T("url"));
    action.description = JsonString(value, _T("description"));
    const ApiJsonValue* request = value.Find(_T("request"));
    const ApiJsonValue* response = value.Find(_T("response"));
    if (response == NULL) return false;
    if (request != NULL) action.request = *request;
    action.response = *response;
    return !action.id.IsEmpty() && !action.name.IsEmpty() &&
        IsSupportedMethod(action.method) && !action.url.IsEmpty() &&
        action.url.Find(_T("://")) < 0 && action.url.Left(2) != _T("//") &&
        (request == NULL || ApiJsonContainsValueMarker(action.request)) &&
        ApiJsonContainsValueMarker(action.response);
}

bool LoadApiConfigDocument(const CString& filePath, ApiConfigDocument& document, CString& error)
{
    document = ApiConfigDocument();
    CStringA path(filePath);
    std::ifstream file(path.GetString(), std::ios::binary);
    if (!file) {
        error = _T("API configuration file could not be opened.");
        return false;
    }
    std::string text((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    ApiJsonValue root;
    if (!ParseApiJsonUtf8(text, root, error) || root.type != ApiJsonValue::TYPE_OBJECT) return false;

    document.id = JsonString(root, _T("id"));
    document.name = JsonString(root, _T("name"));
    document.baseUrl = JsonString(root, _T("baseUrl"));
    document.authType = JsonString(root, _T("authType"));
    document.credentialId = JsonString(root, _T("credentialId"));
    if (document.id.IsEmpty() || document.name.IsEmpty() || document.baseUrl.IsEmpty() ||
        !JsonPositiveNumber(root, _T("timeoutMs"), document.timeoutMs) ||
        document.authType.IsEmpty()) {
        error = _T("Required API configuration properties are missing or invalid.");
        return false;
    }
    if (document.baseUrl.Left(7).CompareNoCase(_T("http://")) != 0 &&
        document.baseUrl.Left(8).CompareNoCase(_T("https://")) != 0) {
        error = _T("baseUrl must start with http:// or https://.");
        return false;
    }
    if (!IsSupportedAuthType(document.authType)) {
        error = _T("Unsupported authType in API configuration.");
        return false;
    }
    const ApiJsonValue* keyConfig = root.Find(_T("keyConfig"));
    if (keyConfig != NULL && keyConfig->type == ApiJsonValue::TYPE_OBJECT) {
        document.keyConfig = *keyConfig;
    }
    else {
        error = _T("keyConfig must be a JSON object.");
        return false;
    }
    if (!ValidateStaticHeaders(document.keyConfig)) {
        error = _T("staticHeader values must be non-empty strings without line breaks.");
        return false;
    }

    if (document.authType != _T("none") && document.credentialId.IsEmpty()) {
        error = _T("credentialId is required for authenticated APIs.");
        return false;
    }
    if ((document.authType == _T("api-key-header") || document.authType == _T("bearer")) &&
        !HasNonEmptyString(document.keyConfig, _T("headerName"))) {
        error = _T("headerName is required for the selected authType.");
        return false;
    }
    if (document.authType == _T("api-key-query") &&
        !HasNonEmptyString(document.keyConfig, _T("queryName"))) {
        error = _T("queryName is required for api-key-query.");
        return false;
    }

    const ApiJsonValue* actions = root.Find(_T("actions"));
    if (actions != NULL && actions->type == ApiJsonValue::TYPE_ARRAY) {
        for (size_t index = 0; index < actions->children.size(); ++index) {
            ApiActionDocument action;
            if (!LoadAction(actions->children[index], action)) return false;
            for (size_t existing = 0; existing < document.actions.size(); ++existing) {
                if (document.actions[existing].id.CompareNoCase(action.id) == 0) {
                    error = _T("Duplicate Action ID in API configuration.");
                    return false;
                }
            }
            document.actions.push_back(action);
        }
    }
    else {
        error = _T("actions must be a JSON array.");
        return false;
    }
    return !document.id.IsEmpty() && !document.name.IsEmpty() &&
        !document.baseUrl.IsEmpty() && !document.authType.IsEmpty() &&
        !document.actions.empty();
}

static ApiJsonValue StringValue(const CString& value)
{
    return ApiJsonValue::String(value);
}

static ApiJsonValue NumberValue(DWORD value)
{
    ApiJsonValue result(ApiJsonValue::TYPE_NUMBER);
    result.scalar.Format(_T("%lu"), value);
    return result;
}

bool SaveApiConfigDocument(const CString& filePath, const ApiConfigDocument& document, CString& error)
{
    ApiJsonValue root = ApiJsonValue::Object();
    root.Set(_T("id"), StringValue(document.id));
    root.Set(_T("name"), StringValue(document.name));
    root.Set(_T("baseUrl"), StringValue(document.baseUrl));
    root.Set(_T("timeoutMs"), NumberValue(document.timeoutMs));
    root.Set(_T("authType"), StringValue(document.authType));
    if (!document.credentialId.IsEmpty()) root.Set(_T("credentialId"), StringValue(document.credentialId));
    root.Set(_T("keyConfig"), document.keyConfig);
    ApiJsonValue actions = ApiJsonValue::Array();
    for (size_t index = 0; index < document.actions.size(); ++index) {
        const ApiActionDocument& source = document.actions[index];
        ApiJsonValue action = ApiJsonValue::Object();
        action.Set(_T("id"), StringValue(source.id));
        action.Set(_T("name"), StringValue(source.name));
        action.Set(_T("url"), StringValue(source.url));
        action.Set(_T("method"), StringValue(source.method));
        action.Set(_T("description"), StringValue(source.description));
        if (source.request.type != ApiJsonValue::TYPE_NULL) action.Set(_T("request"), source.request);
        action.Set(_T("response"), source.response);
        actions.children.push_back(action);
    }
    root.Set(_T("actions"), actions);

    std::string text;
    SerializeApiJsonUtf8(root, text, TRUE);
    text += "\n";
    CStringA path(filePath);
    std::ofstream file(path.GetString(), std::ios::binary | std::ios::trunc);
    if (!file) {
        error = _T("API configuration file could not be saved.");
        return false;
    }
    file.write(text.data(), (std::streamsize)text.size());
    if (!file) {
        error = _T("API configuration file could not be saved.");
        return false;
    }
    return true;
}
