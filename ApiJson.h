#pragma once

#include <string>
#include <vector>

struct ApiJsonValue
{
    enum Type {
        TYPE_NULL,
        TYPE_BOOL,
        TYPE_NUMBER,
        TYPE_STRING,
        TYPE_OBJECT,
        TYPE_ARRAY
    };

    Type type;
    CString scalar;
    std::vector<CString> keys;
    std::vector<ApiJsonValue> children;

    ApiJsonValue(Type valueType = TYPE_NULL);
    static ApiJsonValue Object();
    static ApiJsonValue Array();
    static ApiJsonValue String(const CString& value);
    ApiJsonValue* Find(const CString& key);
    const ApiJsonValue* Find(const CString& key) const;
    void Set(const CString& key, const ApiJsonValue& value);
    void Remove(size_t index);
};

bool ParseApiJsonUtf8(const std::string& text, ApiJsonValue& value, CString& error);
bool SerializeApiJsonUtf8(const ApiJsonValue& value, std::string& text, BOOL pretty = TRUE);
bool ParseApiJsonText(const CString& text, ApiJsonValue& value, CString& error);
bool SerializeApiJsonText(const ApiJsonValue& value, CString& text, BOOL pretty = TRUE);
bool ApiJsonContainsValueMarker(const ApiJsonValue& value);
void ApiJsonReplaceValueMarker(ApiJsonValue& value, const CString& replacement);
bool ApiJsonExtractValues(const ApiJsonValue& pattern, const ApiJsonValue& actual,
    std::vector<CString>& values);
CString ApiJsonTypeName(ApiJsonValue::Type type);
