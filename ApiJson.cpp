#include "stdafx.h"
#include "ApiJson.h"

#include <cstdlib>

ApiJsonValue::ApiJsonValue(Type valueType) : type(valueType)
{
}

ApiJsonValue ApiJsonValue::Object() { return ApiJsonValue(TYPE_OBJECT); }
ApiJsonValue ApiJsonValue::Array() { return ApiJsonValue(TYPE_ARRAY); }
ApiJsonValue ApiJsonValue::String(const CString& value)
{
    ApiJsonValue result(TYPE_STRING);
    result.scalar = value;
    return result;
}

ApiJsonValue* ApiJsonValue::Find(const CString& key)
{
    if (type != TYPE_OBJECT) return NULL;
    for (size_t index = 0; index < keys.size(); ++index) {
        if (keys[index] == key) return &children[index];
    }
    return NULL;
}

const ApiJsonValue* ApiJsonValue::Find(const CString& key) const
{
    if (type != TYPE_OBJECT) return NULL;
    for (size_t index = 0; index < keys.size(); ++index) {
        if (keys[index] == key) return &children[index];
    }
    return NULL;
}

void ApiJsonValue::Set(const CString& key, const ApiJsonValue& value)
{
    if (type != TYPE_OBJECT) {
        type = TYPE_OBJECT;
        scalar.Empty();
        keys.clear();
        children.clear();
    }
    for (size_t index = 0; index < keys.size(); ++index) {
        if (keys[index] == key) {
            children[index] = value;
            return;
        }
    }
    keys.push_back(key);
    children.push_back(value);
}

void ApiJsonValue::Remove(size_t index)
{
    if (index >= children.size()) return;
    children.erase(children.begin() + index);
    if (type == TYPE_OBJECT && index < keys.size()) keys.erase(keys.begin() + index);
}

static bool Utf8ToCString(const std::string& input, CString& output)
{
    if (input.empty()) {
        output.Empty();
        return true;
    }
    int wideLength = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
        input.data(), (int)input.size(), NULL, 0);
    if (wideLength <= 0) return false;
    std::vector<wchar_t> wide((size_t)wideLength);
    if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
        input.data(), (int)input.size(), wide.data(), wideLength) <= 0) return false;
#ifdef _UNICODE
    output.SetString(wide.data(), wideLength);
#else
    int length = WideCharToMultiByte(CP_ACP, 0, wide.data(), wideLength,
        NULL, 0, NULL, NULL);
    if (length <= 0) return false;
    std::vector<char> ansi((size_t)length);
    if (WideCharToMultiByte(CP_ACP, 0, wide.data(), wideLength,
        ansi.data(), length, NULL, NULL) <= 0) return false;
    output.SetString(ansi.data(), length);
#endif
    return true;
}

static bool CStringToUtf8(const CString& input, std::string& output)
{
#ifdef _UNICODE
    const wchar_t* wide = input.GetString();
    int wideLength = input.GetLength();
#else
    int wideLength = MultiByteToWideChar(CP_ACP, 0, input, input.GetLength(), NULL, 0);
    if (wideLength < 0) return false;
    std::vector<wchar_t> buffer((size_t)wideLength);
    if (wideLength > 0 && MultiByteToWideChar(CP_ACP, 0, input, input.GetLength(),
        buffer.data(), wideLength) <= 0) return false;
    const wchar_t* wide = buffer.empty() ? L"" : buffer.data();
#endif
    if (wideLength == 0) {
        output.clear();
        return true;
    }
    int length = WideCharToMultiByte(CP_UTF8, 0, wide, wideLength, NULL, 0, NULL, NULL);
    if (length <= 0) return false;
    output.resize((size_t)length);
    return WideCharToMultiByte(CP_UTF8, 0, wide, wideLength,
        &output[0], length, NULL, NULL) > 0;
}

class ApiJsonParser
{
public:
    ApiJsonParser(const std::string& text) : m_text(text), m_position(0) {}

    bool Parse(ApiJsonValue& value, CString& error)
    {
        SkipSpace();
        if (!ParseValue(value)) {
            error.Format(_T("Invalid JSON near byte %u."), (UINT)m_position);
            return false;
        }
        SkipSpace();
        if (m_position != m_text.size()) {
            error.Format(_T("Unexpected data near byte %u."), (UINT)m_position);
            return false;
        }
        return true;
    }

private:
    const std::string& m_text;
    size_t m_position;

    void SkipSpace()
    {
        while (m_position < m_text.size() &&
            isspace((unsigned char)m_text[m_position])) ++m_position;
    }

    bool ParseValue(ApiJsonValue& value)
    {
        SkipSpace();
        if (m_position >= m_text.size()) return false;
        char c = m_text[m_position];
        if (c == '{') return ParseObject(value);
        if (c == '[') return ParseArray(value);
        if (c == '"') {
            value.type = ApiJsonValue::TYPE_STRING;
            return ParseString(value.scalar);
        }
        if (m_text.compare(m_position, 4, "null") == 0) {
            m_position += 4;
            value = ApiJsonValue(ApiJsonValue::TYPE_NULL);
            return true;
        }
        if (m_text.compare(m_position, 4, "true") == 0) {
            m_position += 4;
            value = ApiJsonValue(ApiJsonValue::TYPE_BOOL);
            value.scalar = _T("true");
            return true;
        }
        if (m_text.compare(m_position, 5, "false") == 0) {
            m_position += 5;
            value = ApiJsonValue(ApiJsonValue::TYPE_BOOL);
            value.scalar = _T("false");
            return true;
        }
        return ParseNumber(value);
    }

    bool ParseString(CString& value)
    {
        if (m_position >= m_text.size() || m_text[m_position++] != '"') return false;
        std::string decoded;
        while (m_position < m_text.size()) {
            char c = m_text[m_position++];
            if (c == '"') return Utf8ToCString(decoded, value);
            if (c != '\\') {
                decoded += c;
                continue;
            }
            if (m_position >= m_text.size()) return false;
            char escaped = m_text[m_position++];
            switch (escaped) {
            case '"': decoded += '"'; break;
            case '\\': decoded += '\\'; break;
            case '/': decoded += '/'; break;
            case 'b': decoded += '\b'; break;
            case 'f': decoded += '\f'; break;
            case 'n': decoded += '\n'; break;
            case 'r': decoded += '\r'; break;
            case 't': decoded += '\t'; break;
            case 'u':
                {
                    unsigned int codePoint = 0;
                    if (!ReadHex4(codePoint)) return false;
                    if (codePoint >= 0xD800 && codePoint <= 0xDBFF) {
                        if (m_position + 2 > m_text.size() ||
                            m_text[m_position] != '\\' || m_text[m_position + 1] != 'u') return false;
                        m_position += 2;
                        unsigned int low = 0;
                        if (!ReadHex4(low) || low < 0xDC00 || low > 0xDFFF) return false;
                        codePoint = 0x10000 + ((codePoint - 0xD800) << 10) + (low - 0xDC00);
                    }
                    AppendCodePoint(decoded, codePoint);
                }
                break;
            default: return false;
            }
        }
        return false;
    }

    bool ReadHex4(unsigned int& value)
    {
        if (m_position + 4 > m_text.size()) return false;
        value = 0;
        for (int index = 0; index < 4; ++index) {
            char c = m_text[m_position++];
            value <<= 4;
            if (c >= '0' && c <= '9') value += c - '0';
            else if (c >= 'a' && c <= 'f') value += c - 'a' + 10;
            else if (c >= 'A' && c <= 'F') value += c - 'A' + 10;
            else return false;
        }
        return true;
    }

    static void AppendCodePoint(std::string& output, unsigned int value)
    {
        if (value <= 0x7F) output += (char)value;
        else if (value <= 0x7FF) {
            output += (char)(0xC0 | (value >> 6));
            output += (char)(0x80 | (value & 0x3F));
        }
        else if (value <= 0xFFFF) {
            output += (char)(0xE0 | (value >> 12));
            output += (char)(0x80 | ((value >> 6) & 0x3F));
            output += (char)(0x80 | (value & 0x3F));
        }
        else {
            output += (char)(0xF0 | (value >> 18));
            output += (char)(0x80 | ((value >> 12) & 0x3F));
            output += (char)(0x80 | ((value >> 6) & 0x3F));
            output += (char)(0x80 | (value & 0x3F));
        }
    }

    bool ParseNumber(ApiJsonValue& value)
    {
        size_t start = m_position;
        if (m_text[m_position] == '-') ++m_position;
        size_t integerStart = m_position;
        while (m_position < m_text.size() && isdigit((unsigned char)m_text[m_position])) ++m_position;
        if (m_position == integerStart) return false;
        if (m_position < m_text.size() && m_text[m_position] == '.') {
            ++m_position;
            size_t fractionStart = m_position;
            while (m_position < m_text.size() && isdigit((unsigned char)m_text[m_position])) ++m_position;
            if (m_position == fractionStart) return false;
        }
        if (m_position < m_text.size() &&
            (m_text[m_position] == 'e' || m_text[m_position] == 'E')) {
            ++m_position;
            if (m_position < m_text.size() &&
                (m_text[m_position] == '+' || m_text[m_position] == '-')) ++m_position;
            size_t exponentStart = m_position;
            while (m_position < m_text.size() && isdigit((unsigned char)m_text[m_position])) ++m_position;
            if (m_position == exponentStart) return false;
        }
        std::string number = m_text.substr(start, m_position - start);
        value = ApiJsonValue(ApiJsonValue::TYPE_NUMBER);
        return Utf8ToCString(number, value.scalar);
    }

    bool ParseObject(ApiJsonValue& value)
    {
        ++m_position;
        value = ApiJsonValue::Object();
        SkipSpace();
        if (m_position < m_text.size() && m_text[m_position] == '}') {
            ++m_position;
            return true;
        }
        while (m_position < m_text.size()) {
            CString key;
            if (!ParseString(key)) return false;
            SkipSpace();
            if (m_position >= m_text.size() || m_text[m_position++] != ':') return false;
            ApiJsonValue child;
            if (!ParseValue(child)) return false;
            value.keys.push_back(key);
            value.children.push_back(child);
            SkipSpace();
            if (m_position < m_text.size() && m_text[m_position] == '}') {
                ++m_position;
                return true;
            }
            if (m_position >= m_text.size() || m_text[m_position++] != ',') return false;
            SkipSpace();
        }
        return false;
    }

    bool ParseArray(ApiJsonValue& value)
    {
        ++m_position;
        value = ApiJsonValue::Array();
        SkipSpace();
        if (m_position < m_text.size() && m_text[m_position] == ']') {
            ++m_position;
            return true;
        }
        while (m_position < m_text.size()) {
            ApiJsonValue child;
            if (!ParseValue(child)) return false;
            value.children.push_back(child);
            SkipSpace();
            if (m_position < m_text.size() && m_text[m_position] == ']') {
                ++m_position;
                return true;
            }
            if (m_position >= m_text.size() || m_text[m_position++] != ',') return false;
            SkipSpace();
        }
        return false;
    }
};

bool ParseApiJsonUtf8(const std::string& text, ApiJsonValue& value, CString& error)
{
    ApiJsonParser parser(text);
    return parser.Parse(value, error);
}

bool ParseApiJsonText(const CString& text, ApiJsonValue& value, CString& error)
{
    std::string utf8;
    if (!CStringToUtf8(text, utf8)) {
        error = _T("JSON text could not be encoded as UTF-8.");
        return false;
    }
    return ParseApiJsonUtf8(utf8, value, error);
}

static void AppendIndent(std::string& output, int indent)
{
    output.append((size_t)indent, ' ');
}

static void AppendEscaped(std::string& output, const CString& value)
{
    std::string utf8;
    CStringToUtf8(value, utf8);
    output += '"';
    for (size_t index = 0; index < utf8.size(); ++index) {
        unsigned char c = (unsigned char)utf8[index];
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

static void SerializeValue(const ApiJsonValue& value, std::string& output, BOOL pretty, int indent)
{
    switch (value.type) {
    case ApiJsonValue::TYPE_NULL:
        output += "null";
        break;
    case ApiJsonValue::TYPE_BOOL:
    case ApiJsonValue::TYPE_NUMBER:
        {
            std::string scalar;
            CStringToUtf8(value.scalar, scalar);
            output += scalar;
        }
        break;
    case ApiJsonValue::TYPE_STRING:
        AppendEscaped(output, value.scalar);
        break;
    case ApiJsonValue::TYPE_OBJECT:
        output += "{";
        if (pretty && !value.children.empty()) output += "\n";
        for (size_t index = 0; index < value.children.size(); ++index) {
            if (pretty) AppendIndent(output, indent + 2);
            AppendEscaped(output, value.keys[index]);
            output += pretty ? ": " : ":";
            SerializeValue(value.children[index], output, pretty, indent + 2);
            if (index + 1 < value.children.size()) output += ",";
            if (pretty) output += "\n";
        }
        if (pretty && !value.children.empty()) AppendIndent(output, indent);
        output += "}";
        break;
    case ApiJsonValue::TYPE_ARRAY:
        output += "[";
        if (pretty && !value.children.empty()) output += "\n";
        for (size_t index = 0; index < value.children.size(); ++index) {
            if (pretty) AppendIndent(output, indent + 2);
            SerializeValue(value.children[index], output, pretty, indent + 2);
            if (index + 1 < value.children.size()) output += ",";
            if (pretty) output += "\n";
        }
        if (pretty && !value.children.empty()) AppendIndent(output, indent);
        output += "]";
        break;
    }
}

bool SerializeApiJsonUtf8(const ApiJsonValue& value, std::string& text, BOOL pretty)
{
    text.clear();
    SerializeValue(value, text, pretty, 0);
    return true;
}

bool SerializeApiJsonText(const ApiJsonValue& value, CString& text, BOOL pretty)
{
    std::string utf8;
    if (!SerializeApiJsonUtf8(value, utf8, pretty)) return false;
    return Utf8ToCString(utf8, text);
}

bool ApiJsonContainsValueMarker(const ApiJsonValue& value)
{
    if (value.type == ApiJsonValue::TYPE_STRING && value.scalar == _T("{{value}}")) return true;
    for (size_t index = 0; index < value.children.size(); ++index) {
        if (ApiJsonContainsValueMarker(value.children[index])) return true;
    }
    return false;
}

void ApiJsonReplaceValueMarker(ApiJsonValue& value, const CString& replacement)
{
    if (value.type == ApiJsonValue::TYPE_STRING) {
        value.scalar.Replace(_T("{{value}}"), replacement);
        return;
    }
    for (size_t index = 0; index < value.children.size(); ++index) {
        ApiJsonReplaceValueMarker(value.children[index], replacement);
    }
}

static bool ScalarToText(const ApiJsonValue& value, CString& text)
{
    if (value.type == ApiJsonValue::TYPE_STRING ||
        value.type == ApiJsonValue::TYPE_NUMBER ||
        value.type == ApiJsonValue::TYPE_BOOL) {
        text = value.scalar;
        return true;
    }
    if (value.type == ApiJsonValue::TYPE_NULL) {
        text.Empty();
        return true;
    }
    return false;
}

bool ApiJsonExtractValues(const ApiJsonValue& pattern, const ApiJsonValue& actual,
    std::vector<CString>& values)
{
    if (pattern.type == ApiJsonValue::TYPE_STRING && pattern.scalar == _T("{{value}}")) {
        CString text;
        if (!ScalarToText(actual, text)) return false;
        values.push_back(text);
        return true;
    }
    if (pattern.type == ApiJsonValue::TYPE_OBJECT) {
        if (actual.type != ApiJsonValue::TYPE_OBJECT) return false;
        bool matched = false;
        for (size_t index = 0; index < pattern.children.size(); ++index) {
            const ApiJsonValue* child = actual.Find(pattern.keys[index]);
            if (child != NULL && ApiJsonExtractValues(pattern.children[index], *child, values)) {
                matched = true;
            }
        }
        return matched;
    }
    if (pattern.type == ApiJsonValue::TYPE_ARRAY) {
        if (actual.type != ApiJsonValue::TYPE_ARRAY || pattern.children.empty()) return false;
        bool matched = false;
        for (size_t actualIndex = 0; actualIndex < actual.children.size(); ++actualIndex) {
            if (ApiJsonExtractValues(pattern.children[0], actual.children[actualIndex], values)) {
                matched = true;
            }
        }
        return matched;
    }
    return pattern.type == actual.type && pattern.scalar == actual.scalar;
}

CString ApiJsonTypeName(ApiJsonValue::Type type)
{
    switch (type) {
    case ApiJsonValue::TYPE_NULL: return _T("null");
    case ApiJsonValue::TYPE_BOOL: return _T("boolean");
    case ApiJsonValue::TYPE_NUMBER: return _T("number");
    case ApiJsonValue::TYPE_STRING: return _T("string");
    case ApiJsonValue::TYPE_OBJECT: return _T("object");
    case ApiJsonValue::TYPE_ARRAY: return _T("array");
    }
    return CString();
}
