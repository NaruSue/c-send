#include "stdafx.h"
#include "JsonDataFile.h"
#include "IniTextUtil.h"

#include <climits>
#include <cstdlib>
#include <string>
#include <vector>

static bool Utf8ToCString(const std::string& utf8, CString& output)
{
    if (utf8.empty()) {
        output.Empty();
        return true;
    }

    int wideLength = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8.data(), (int)utf8.size(), NULL, 0);
    if (wideLength <= 0) return false;

    std::vector<wchar_t> wide((size_t)wideLength);
    if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8.data(), (int)utf8.size(), wide.data(), wideLength) <= 0) return false;

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

static bool CStringToUtf8(const CString& input, std::string& output)
{
#ifdef _UNICODE
    const wchar_t* wideText = input.GetString();
    int wideLength = input.GetLength();
#else
    int wideLength = MultiByteToWideChar(CP_ACP, 0, input.GetString(), input.GetLength(), NULL, 0);
    if (wideLength < 0) return false;
    std::vector<wchar_t> wide((size_t)wideLength);
    if (wideLength > 0 && MultiByteToWideChar(CP_ACP, 0, input.GetString(), input.GetLength(), wide.data(), wideLength) <= 0) return false;
    const wchar_t* wideText = wide.empty() ? L"" : wide.data();
#endif

    if (wideLength == 0) {
        output.clear();
        return true;
    }

    int utf8Length = WideCharToMultiByte(CP_UTF8, 0, wideText, wideLength, NULL, 0, NULL, NULL);
    if (utf8Length <= 0) return false;
    output.resize((size_t)utf8Length);
    return WideCharToMultiByte(CP_UTF8, 0, wideText, wideLength, &output[0], utf8Length, NULL, NULL) > 0;
}

static bool IsIso8601DateTime(const std::string& value)
{
    if (value.size() < 19) return false;
    const size_t digitPositions[] = { 0, 1, 2, 3, 5, 6, 8, 9, 11, 12, 14, 15, 17, 18 };
    for (size_t i = 0; i < _countof(digitPositions); ++i) {
        char c = value[digitPositions[i]];
        if (c < '0' || c > '9') return false;
    }
    return value[4] == '-' && value[7] == '-' && (value[10] == 'T' || value[10] == 't') &&
        value[13] == ':' && value[16] == ':';
}

static void AppendUtf8CodePoint(std::string& output, unsigned int codePoint)
{
    if (codePoint <= 0x7F) {
        output += (char)codePoint;
    }
    else if (codePoint <= 0x7FF) {
        output += (char)(0xC0 | (codePoint >> 6));
        output += (char)(0x80 | (codePoint & 0x3F));
    }
    else if (codePoint <= 0xFFFF) {
        output += (char)(0xE0 | (codePoint >> 12));
        output += (char)(0x80 | ((codePoint >> 6) & 0x3F));
        output += (char)(0x80 | (codePoint & 0x3F));
    }
    else {
        output += (char)(0xF0 | (codePoint >> 18));
        output += (char)(0x80 | ((codePoint >> 12) & 0x3F));
        output += (char)(0x80 | ((codePoint >> 6) & 0x3F));
        output += (char)(0x80 | (codePoint & 0x3F));
    }
}

class JsonDataParser
{
public:
    explicit JsonDataParser(const std::string& text) : m_text(text), m_pos(0) {}

    bool Parse(CArray<ItemData, ItemData&>& loaded, JsonFileMetadata& metadata)
    {
        if (!Consume('{')) return false;

        bool hasFormat = false;
        bool hasVersion = false;
        bool hasCreatedAt = false;
        bool hasItems = false;
        bool hasCategoryName = false;
        if (Consume('}')) return false;

        while (true) {
            SkipSpace();
            size_t propertyStart = m_pos;
            std::string key;
            if (!ParseString(key) || !Consume(':')) return false;

            if (key == "format") {
                std::string value;
                if (hasFormat || !ParseString(value) || value != "c-send-pwa-backup") return false;
                hasFormat = true;
            }
            else if (key == "version") {
                int version = 0;
                if (hasVersion || !ParseInteger(version) || version != 1) return false;
                hasVersion = true;
            }
            else if (key == "createdAt") {
                std::string value;
                if (hasCreatedAt || !ParseString(value) || !IsIso8601DateTime(value) ||
                    !Utf8ToCString(value, metadata.createdAt)) return false;
                hasCreatedAt = true;
            }
            else if (key == "categoryName") {
                std::string value;
                if (hasCategoryName || !ParseString(value) || !Utf8ToCString(value, metadata.categoryName)) return false;
                hasCategoryName = true;
            }
            else if (key == "items") {
                if (hasItems || !ParseItems(loaded)) return false;
                hasItems = true;
            }
            else {
                if (!SkipValue()) return false;
                metadata.extraProperties.push_back(m_text.substr(propertyStart, m_pos - propertyStart));
            }

            if (Consume('}')) break;
            if (!Consume(',')) return false;
        }
        return hasFormat && hasVersion && hasCreatedAt && hasItems && Finish();
    }

private:
    const std::string& m_text;
    size_t m_pos;

    void SkipSpace()
    {
        while (m_pos < m_text.size()) {
            char c = m_text[m_pos];
            if (c != ' ' && c != '\t' && c != '\r' && c != '\n') break;
            ++m_pos;
        }
    }

    bool Consume(char expected)
    {
        SkipSpace();
        if (m_pos >= m_text.size() || m_text[m_pos] != expected) return false;
        ++m_pos;
        return true;
    }

    bool Finish()
    {
        SkipSpace();
        return m_pos == m_text.size();
    }

    static int HexValue(char c)
    {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        return -1;
    }

    bool ParseUnicodeEscape(unsigned int& codePoint)
    {
        if (m_pos + 4 > m_text.size()) return false;
        codePoint = 0;
        for (int i = 0; i < 4; ++i) {
            int hex = HexValue(m_text[m_pos++]);
            if (hex < 0) return false;
            codePoint = (codePoint << 4) | (unsigned int)hex;
        }
        return true;
    }

    bool ParseString(std::string& output)
    {
        SkipSpace();
        if (m_pos >= m_text.size() || m_text[m_pos++] != '"') return false;
        output.clear();

        while (m_pos < m_text.size()) {
            unsigned char c = (unsigned char)m_text[m_pos++];
            if (c == '"') return true;
            if (c < 0x20) return false;
            if (c != '\\') {
                output += (char)c;
                continue;
            }
            if (m_pos >= m_text.size()) return false;
            char escaped = m_text[m_pos++];
            switch (escaped) {
            case '"': output += '"'; break;
            case '\\': output += '\\'; break;
            case '/': output += '/'; break;
            case 'b': output += '\b'; break;
            case 'f': output += '\f'; break;
            case 'n': output += '\n'; break;
            case 'r': output += '\r'; break;
            case 't': output += '\t'; break;
            case 'u':
                {
                    unsigned int codePoint = 0;
                    if (!ParseUnicodeEscape(codePoint) || codePoint == 0) return false;
                    if (codePoint >= 0xD800 && codePoint <= 0xDBFF) {
                        if (m_pos + 2 > m_text.size() || m_text[m_pos++] != '\\' || m_text[m_pos++] != 'u') return false;
                        unsigned int low = 0;
                        if (!ParseUnicodeEscape(low) || low < 0xDC00 || low > 0xDFFF) return false;
                        codePoint = 0x10000 + ((codePoint - 0xD800) << 10) + (low - 0xDC00);
                    }
                    else if (codePoint >= 0xDC00 && codePoint <= 0xDFFF) {
                        return false;
                    }
                    AppendUtf8CodePoint(output, codePoint);
                }
                break;
            default:
                return false;
            }
        }
        return false;
    }

    bool SkipNumber()
    {
        size_t start = m_pos;
        if (m_pos < m_text.size() && m_text[m_pos] == '-') ++m_pos;
        if (m_pos >= m_text.size()) return false;
        if (m_text[m_pos] == '0') ++m_pos;
        else if (m_text[m_pos] >= '1' && m_text[m_pos] <= '9') {
            while (m_pos < m_text.size() && m_text[m_pos] >= '0' && m_text[m_pos] <= '9') ++m_pos;
        }
        else return false;

        if (m_pos < m_text.size() && m_text[m_pos] == '.') {
            ++m_pos;
            size_t digits = m_pos;
            while (m_pos < m_text.size() && m_text[m_pos] >= '0' && m_text[m_pos] <= '9') ++m_pos;
            if (digits == m_pos) return false;
        }
        if (m_pos < m_text.size() && (m_text[m_pos] == 'e' || m_text[m_pos] == 'E')) {
            ++m_pos;
            if (m_pos < m_text.size() && (m_text[m_pos] == '+' || m_text[m_pos] == '-')) ++m_pos;
            size_t digits = m_pos;
            while (m_pos < m_text.size() && m_text[m_pos] >= '0' && m_text[m_pos] <= '9') ++m_pos;
            if (digits == m_pos) return false;
        }
        return m_pos > start;
    }

    bool ParseInteger(int& value)
    {
        SkipSpace();
        size_t start = m_pos;
        if (!SkipNumber()) return false;
        std::string token = m_text.substr(start, m_pos - start);
        if (token.find_first_of(".eE") != std::string::npos) return false;
        char* end = NULL;
        long parsed = strtol(token.c_str(), &end, 10);
        if (end == token.c_str() || *end != '\0' || parsed < INT_MIN || parsed > INT_MAX) return false;
        value = (int)parsed;
        return true;
    }

    bool SkipValue()
    {
        SkipSpace();
        if (m_pos >= m_text.size()) return false;
        char c = m_text[m_pos];
        if (c == '"') {
            std::string ignored;
            return ParseString(ignored);
        }
        if (c == '{') {
            ++m_pos;
            if (Consume('}')) return true;
            while (true) {
                std::string key;
                if (!ParseString(key) || !Consume(':') || !SkipValue()) return false;
                if (Consume('}')) return true;
                if (!Consume(',')) return false;
            }
        }
        if (c == '[') {
            ++m_pos;
            if (Consume(']')) return true;
            while (true) {
                if (!SkipValue()) return false;
                if (Consume(']')) return true;
                if (!Consume(',')) return false;
            }
        }
        if (m_text.compare(m_pos, 4, "true") == 0) { m_pos += 4; return true; }
        if (m_text.compare(m_pos, 5, "false") == 0) { m_pos += 5; return true; }
        if (m_text.compare(m_pos, 4, "null") == 0) { m_pos += 4; return true; }
        return SkipNumber();
    }

    bool ParseStringArray()
    {
        if (!Consume('[')) return false;
        if (Consume(']')) return true;
        while (true) {
            std::string ignored;
            if (!ParseString(ignored)) return false;
            if (Consume(']')) return true;
            if (!Consume(',')) return false;
        }
    }

    bool ParseItems(CArray<ItemData, ItemData&>& loaded)
    {
        if (!Consume('[')) return false;
        if (Consume(']')) return true;

        while (true) {
            ItemData item;
            if (!ParseItem(item)) return false;
            if (loaded.GetSize() >= CDataValueList::MAX_ITEMS) return false;
            loaded.Add(item);

            if (Consume(']')) return true;
            if (!Consume(',')) return false;
        }
    }

    bool ParseItem(ItemData& item)
    {
        if (!Consume('{')) return false;
        bool hasName = false;
        bool hasValue = false;
        bool hasType = false;
        bool hasMode = false;
        if (Consume('}')) return false;

        while (true) {
            SkipSpace();
            size_t propertyStart = m_pos;
            std::string key;
            if (!ParseString(key) || !Consume(':')) return false;

            if (key == "name") {
                std::string value;
                if (hasName || !ParseString(value) || !Utf8ToCString(value, item.name)) return false;
                hasName = true;
            }
            else if (key == "value") {
                std::string value;
                if (hasValue || !ParseString(value) || !Utf8ToCString(value, item.value)) return false;
                hasValue = true;
            }
            else if (key == "type") {
                if (hasType || !ParseInteger(item.type) || item.type != 0) return false;
                hasType = true;
            }
            else if (key == "mode") {
                std::string value;
                if (hasMode || !ParseString(value) || !Utf8ToCString(value, item.mode)) return false;
                if (item.mode != _T("plain") && item.mode != _T("template") &&
                    item.mode != _T("counter") && item.mode != _T("api")) return false;
                hasMode = true;
            }
            else if (key == "options") {
                SkipSpace();
                if (m_pos >= m_text.size() || m_text[m_pos] != '{' || !SkipValue()) return false;
                item.jsonExtraProperties.push_back(m_text.substr(propertyStart, m_pos - propertyStart));
            }
            else if (key == "tags") {
                if (!ParseStringArray()) return false;
                item.jsonExtraProperties.push_back(m_text.substr(propertyStart, m_pos - propertyStart));
            }
            else if (key == "usageCount") {
                int usageCount = 0;
                if (!ParseInteger(usageCount) || usageCount < 0) return false;
                item.jsonExtraProperties.push_back(m_text.substr(propertyStart, m_pos - propertyStart));
            }
            else if (key == "lastUsedAt") {
                std::string value;
                if (!ParseString(value) || (!value.empty() && !IsIso8601DateTime(value))) return false;
                item.jsonExtraProperties.push_back(m_text.substr(propertyStart, m_pos - propertyStart));
            }
            else if (key == "sourceName") {
                std::string value;
                if (!ParseString(value)) return false;
                item.jsonExtraProperties.push_back(m_text.substr(propertyStart, m_pos - propertyStart));
            }
            else {
                if (!SkipValue()) return false;
                item.jsonExtraProperties.push_back(m_text.substr(propertyStart, m_pos - propertyStart));
            }

            if (Consume('}')) break;
            if (!Consume(',')) return false;
        }
        return hasName && hasValue && hasType && !item.name.IsEmpty();
    }
};

bool LoadJsonDataFile(const CString& dataPath, CArray<ItemData, ItemData&>& loaded,
    JsonFileMetadata& metadata, CString* pError)
{
    CFile file;
    try {
        if (!file.Open(dataPath, CFile::modeRead | CFile::typeBinary)) {
            if (pError != NULL) *pError = GetIniMessage(_T(""), _T("data_read_failed"), _T("Data file could not be read."));
            return false;
        }
    }
    catch (CFileException&) {
        if (pError != NULL) *pError = GetIniMessage(_T(""), _T("data_read_failed"), _T("Data file could not be read."));
        return false;
    }

    ULONGLONG length = file.GetLength();
    if (length > CDataValueList::MAX_FILE_BYTES) {
        file.Close();
        if (pError != NULL) {
            CString fmt = GetIniMessage(_T(""), _T("data_too_large"), _T("Data file is too large. Limit: %d bytes."));
            pError->Format(fmt, CDataValueList::MAX_FILE_BYTES);
        }
        return false;
    }

    std::string text((size_t)length, '\0');
    if (length > 0 && file.Read(&text[0], (UINT)length) != (UINT)length) {
        file.Close();
        if (pError != NULL) *pError = GetIniMessage(_T(""), _T("data_read_failed"), _T("Data file could not be read."));
        return false;
    }
    file.Close();

    if (text.size() >= 3 && (unsigned char)text[0] == 0xEF && (unsigned char)text[1] == 0xBB && (unsigned char)text[2] == 0xBF) {
        text.erase(0, 3);
    }

    CArray<ItemData, ItemData&> parsed;
    JsonFileMetadata parsedMetadata;
    JsonDataParser parser(text);
    if (!parser.Parse(parsed, parsedMetadata)) {
        if (pError != NULL) *pError = GetIniMessage(_T(""), _T("data_format_ng"), _T("Invalid data file format."));
        return false;
    }

    loaded.RemoveAll();
    for (int i = 0; i < parsed.GetSize(); ++i) loaded.Add(parsed[i]);
    metadata = parsedMetadata;
    return true;
}

static void AppendJsonString(std::string& output, const CString& value)
{
    std::string utf8;
    if (!CStringToUtf8(value, utf8)) utf8.clear();
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
        default:
            if (c < 0x20) {
                char buffer[7] = { 0 };
                sprintf_s(buffer, "\\u%04X", (unsigned int)c);
                output += buffer;
            }
            else output += (char)c;
            break;
        }
    }
    output += '"';
}

static CString MakeUtcTimestamp()
{
    SYSTEMTIME value = {};
    GetSystemTime(&value);
    CString timestamp;
    timestamp.Format(_T("%04u-%02u-%02uT%02u:%02u:%02u.%03uZ"),
        value.wYear, value.wMonth, value.wDay, value.wHour, value.wMinute,
        value.wSecond, value.wMilliseconds);
    return timestamp;
}

void SaveJsonDataFile(const CString& dataPath, const CArray<ItemData, ItemData&>& items,
    const JsonFileMetadata& metadata)
{
    std::string text = "{\n  \"format\": \"c-send-pwa-backup\",\n  \"version\": 1,\n  \"createdAt\": ";
    AppendJsonString(text, metadata.createdAt.IsEmpty() ? MakeUtcTimestamp() : metadata.createdAt);
    if (!metadata.categoryName.IsEmpty()) {
        text += ",\n  \"categoryName\": ";
        AppendJsonString(text, metadata.categoryName);
    }
    for (size_t extra = 0; extra < metadata.extraProperties.size(); ++extra) {
        text += ",\n  ";
        text += metadata.extraProperties[extra];
    }
    text += ",\n  \"items\": [\n";
    for (int i = 0; i < items.GetSize(); ++i) {
        const ItemData& item = items[i];
        text += "    {\n      \"name\": ";
        AppendJsonString(text, item.name);
        text += ",\n      \"value\": ";
        AppendJsonString(text, item.value);
        char typeBuffer[32] = { 0 };
        sprintf_s(typeBuffer, "%d", item.type);
        text += ",\n      \"type\": ";
        text += typeBuffer;
        text += ",\n      \"mode\": ";
        AppendJsonString(text, item.mode.IsEmpty() ? CString(_T("plain")) : item.mode);
        for (size_t extra = 0; extra < item.jsonExtraProperties.size(); ++extra) {
            text += ",\n      ";
            text += item.jsonExtraProperties[extra];
        }
        text += "\n    }";
        if (i + 1 < items.GetSize()) text += ',';
        text += '\n';
    }
    text += "  ]\n}\n";

    CFile file;
    try {
        if (!file.Open(dataPath, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary)) {
            CString message;
            CString format = GetIniMessage(_T(""), _T("data_write_failed"), _T("Data file could not be written: %s"));
            message.Format(format, dataPath);
            AfxMessageBox(message);
            return;
        }
        if (!text.empty()) file.Write(text.data(), (UINT)text.size());
        file.Close();
    }
    catch (CFileException* error) {
        CString message;
        CString format = GetIniMessage(_T(""), _T("data_write_error"), _T("An error occurred while writing data file: %s"));
        message.Format(format, dataPath);
        AfxMessageBox(message);
        if (error != NULL) error->Delete();
    }
}
