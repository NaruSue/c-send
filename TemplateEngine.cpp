#include "stdafx.h"
#include "TemplateEngine.h"

#include <algorithm>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <string>
#include <vector>

static bool CStringToUtf8Text(const CString& input, std::string& output)
{
    int wideLength = MultiByteToWideChar(CP_ACP, 0, input, input.GetLength(), NULL, 0);
    if (!input.IsEmpty() && wideLength <= 0) return false;
    std::vector<wchar_t> wide((size_t)wideLength);
    if (wideLength > 0 &&
        MultiByteToWideChar(CP_ACP, 0, input, input.GetLength(), wide.data(), wideLength) <= 0) return false;

    int utf8Length = WideCharToMultiByte(CP_UTF8, 0,
        wide.empty() ? L"" : wide.data(), wideLength, NULL, 0, NULL, NULL);
    if (wideLength > 0 && utf8Length <= 0) return false;
    output.resize((size_t)utf8Length);
    return utf8Length == 0 ||
        WideCharToMultiByte(CP_UTF8, 0, wide.data(), wideLength, &output[0], utf8Length, NULL, NULL) > 0;
}

static bool Utf8TextToCString(const std::string& input, CString& output)
{
    int wideLength = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
        input.data(), (int)input.size(), NULL, 0);
    if (!input.empty() && wideLength <= 0) return false;
    std::vector<wchar_t> wide((size_t)wideLength);
    if (wideLength > 0 &&
        MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
            input.data(), (int)input.size(), wide.data(), wideLength) <= 0) return false;

    int ansiLength = WideCharToMultiByte(CP_ACP, 0,
        wide.empty() ? L"" : wide.data(), wideLength, NULL, 0, NULL, NULL);
    if (wideLength > 0 && ansiLength <= 0) return false;
    std::vector<char> ansi((size_t)ansiLength);
    if (ansiLength > 0 &&
        WideCharToMultiByte(CP_ACP, 0, wide.data(), wideLength,
            ansi.data(), ansiLength, NULL, NULL) <= 0) return false;
    output.SetString(ansi.empty() ? "" : ansi.data(), ansiLength);
    return true;
}

static bool ReadClipboardText(CString& text)
{
    text.Empty();
    bool opened = false;
    for (int attempt = 0; attempt < 5; ++attempt) {
        if (OpenClipboard(NULL)) {
            opened = true;
            break;
        }
        Sleep(10);
    }
    if (!opened) return false;

    HANDLE handle = GetClipboardData(CF_UNICODETEXT);
    if (handle != NULL) {
        LPCWSTR value = (LPCWSTR)GlobalLock(handle);
        if (value != NULL) {
            int length = (int)wcslen(value);
            if (length == 0) {
                GlobalUnlock(handle);
                CloseClipboard();
                return true;
            }
            int ansiLength = WideCharToMultiByte(CP_ACP, 0, value, length, NULL, 0, NULL, NULL);
            if (ansiLength > 0) {
                std::vector<char> ansi((size_t)ansiLength);
                if (WideCharToMultiByte(CP_ACP, 0, value, length, ansi.data(), ansiLength, NULL, NULL) > 0) {
                    text.SetString(ansi.data(), ansiLength);
                    GlobalUnlock(handle);
                    CloseClipboard();
                    return true;
                }
            }
            GlobalUnlock(handle);
        }
    }

    handle = GetClipboardData(CF_TEXT);
    if (handle != NULL) {
        LPCSTR value = (LPCSTR)GlobalLock(handle);
        if (value != NULL) {
            text = value;
            GlobalUnlock(handle);
            CloseClipboard();
            return true;
        }
    }

    CloseClipboard();
    return true;
}

static bool ParseArguments(const CString& text, std::vector<CString>& args)
{
    args.clear();
    CString value(text);
    value.Trim();
    if (value.IsEmpty()) return true;

    int pos = 0;
    while (pos < value.GetLength()) {
        while (pos < value.GetLength() && _istspace((unsigned char)value[pos])) ++pos;
        if (pos >= value.GetLength()) return false;

        CString arg;
        if (value[pos] == '"') {
            ++pos;
            bool closed = false;
            while (pos < value.GetLength()) {
                TCHAR c = value[pos++];
                if (c == '"') {
                    closed = true;
                    break;
                }
                if (c == '\\' && pos < value.GetLength()) {
                    TCHAR escaped = value[pos++];
                    if (escaped == '"' || escaped == '\\') arg += escaped;
                    else return false;
                }
                else {
                    arg += c;
                }
            }
            if (!closed) return false;
        }
        else {
            int start = pos;
            while (pos < value.GetLength() && value[pos] != ',') ++pos;
            arg = value.Mid(start, pos - start);
            arg.Trim();
            if (arg.IsEmpty()) return false;
        }

        while (pos < value.GetLength() && _istspace((unsigned char)value[pos])) ++pos;
        args.push_back(arg);
        if (pos == value.GetLength()) break;
        if (value[pos++] != ',') return false;
        int next = pos;
        while (next < value.GetLength() && _istspace((unsigned char)value[next])) ++next;
        if (next == value.GetLength()) return false;
    }
    return true;
}

static bool IsValidDateFormat(const CString& format)
{
    for (int i = 0; i < format.GetLength();) {
        if (format.Mid(i, 4) == _T("yyyy")) i += 4;
        else if (format.Mid(i, 2) == _T("MM") || format.Mid(i, 2) == _T("dd") ||
            format.Mid(i, 2) == _T("HH") || format.Mid(i, 2) == _T("mm") ||
            format.Mid(i, 2) == _T("ss")) i += 2;
        else if (format[i] == _T('y') || format[i] == _T('M') || format[i] == _T('d') ||
            format[i] == _T('H') || format[i] == _T('m') || format[i] == _T('s')) return false;
        else ++i;
    }
    return true;
}

static CString FormatDateTime(const CTime& time, const CString& format)
{
    CString result;
    for (int i = 0; i < format.GetLength();) {
        CString part;
        if (format.Mid(i, 4) == _T("yyyy")) {
            part.Format(_T("%04d"), time.GetYear());
            i += 4;
        }
        else if (format.Mid(i, 2) == _T("MM")) {
            part.Format(_T("%02d"), time.GetMonth());
            i += 2;
        }
        else if (format.Mid(i, 2) == _T("dd")) {
            part.Format(_T("%02d"), time.GetDay());
            i += 2;
        }
        else if (format.Mid(i, 2) == _T("HH")) {
            part.Format(_T("%02d"), time.GetHour());
            i += 2;
        }
        else if (format.Mid(i, 2) == _T("mm")) {
            part.Format(_T("%02d"), time.GetMinute());
            i += 2;
        }
        else if (format.Mid(i, 2) == _T("ss")) {
            part.Format(_T("%02d"), time.GetSecond());
            i += 2;
        }
        else {
            part = format.Mid(i++, 1);
        }
        result += part;
    }
    return result;
}

static bool EvaluateDate(const std::vector<CString>& args, CString& result)
{
    if (args.size() > 3) return false;
    CString format = args.empty() ? _T("yyyy/MM/dd HH:mm:ss") : args[0];
    if (!IsValidDateFormat(format)) return false;
    if (args.size() >= 2 && args[1] != _T("current")) return false;

    __time64_t offsetSeconds = 0;
    if (args.size() == 3) {
        CString offset(args[2]);
        if (offset.GetLength() < 3 || (offset[0] != '+' && offset[0] != '-')) return false;
        CString number = offset.Mid(1, offset.GetLength() - 2);
        for (int i = 0; i < number.GetLength(); ++i) {
            if (number[i] < '0' || number[i] > '9') return false;
        }
        TCHAR* end = NULL;
        __int64 amount = _tcstoi64(number, &end, 10);
        if (number.IsEmpty() || end == NULL || end == (LPCTSTR)number || *end != _T('\0') || amount < 0) return false;
        __int64 unit = 0;
        switch (offset[offset.GetLength() - 1]) {
        case 's': unit = 1; break;
        case 'm': unit = 60; break;
        case 'h': unit = 60 * 60; break;
        case 'd': unit = 24 * 60 * 60; break;
        case 'w': unit = 7 * 24 * 60 * 60; break;
        default: return false;
        }
        if (amount > LLONG_MAX / unit) return false;
        offsetSeconds = amount * unit;
        if (offset[0] == '-') offsetSeconds = -offsetSeconds;
    }

    try {
        CTime value(CTime::GetCurrentTime().GetTime() + offsetSeconds);
        result = FormatDateTime(value, format);
        return true;
    }
    catch (CException* error) {
        if (error != NULL) error->Delete();
        return false;
    }
}

static CString SortClipboardLines(const CString& clipboard, bool descending)
{
    CString normalized(clipboard);
    normalized.Replace(_T("\r\n"), _T("\n"));
    normalized.Replace(_T("\r"), _T("\n"));

    std::vector<CString> lines;
    int start = 0;
    for (int i = 0; i <= normalized.GetLength(); ++i) {
        if (i == normalized.GetLength() || normalized[i] == '\n') {
            lines.push_back(normalized.Mid(start, i - start));
            start = i + 1;
        }
    }
    if (!normalized.IsEmpty() && normalized[normalized.GetLength() - 1] == '\n' && !lines.empty()) {
        lines.pop_back();
    }

    std::sort(lines.begin(), lines.end(), [descending](const CString& left, const CString& right) {
        int compare = left.Compare(right);
        return descending ? compare > 0 : compare < 0;
    });

    CString result;
    for (size_t i = 0; i < lines.size(); ++i) {
        if (i > 0) result += _T("\r\n");
        result += lines[i];
    }
    return result;
}

class ExpressionParser
{
public:
    explicit ExpressionParser(const CString& text) : m_text(text), m_pos(0) {}

    bool Parse(double& value)
    {
        if (!ParseExpression(value)) return false;
        SkipSpace();
        return m_pos == m_text.GetLength() && std::isfinite(value);
    }

private:
    const CString& m_text;
    int m_pos;

    void SkipSpace()
    {
        while (m_pos < m_text.GetLength() && _istspace((unsigned char)m_text[m_pos])) ++m_pos;
    }

    bool ParseExpression(double& value)
    {
        if (!ParseTerm(value)) return false;
        while (true) {
            SkipSpace();
            if (m_pos >= m_text.GetLength() || (m_text[m_pos] != '+' && m_text[m_pos] != '-')) return true;
            TCHAR op = m_text[m_pos++];
            double right = 0;
            if (!ParseTerm(right)) return false;
            value = op == '+' ? value + right : value - right;
        }
    }

    bool ParseTerm(double& value)
    {
        if (!ParseFactor(value)) return false;
        while (true) {
            SkipSpace();
            if (m_pos >= m_text.GetLength() || (m_text[m_pos] != '*' && m_text[m_pos] != '/')) return true;
            TCHAR op = m_text[m_pos++];
            double right = 0;
            if (!ParseFactor(right) || (op == '/' && right == 0.0)) return false;
            value = op == '*' ? value * right : value / right;
        }
    }

    bool ParseFactor(double& value)
    {
        SkipSpace();
        if (m_pos < m_text.GetLength() && (m_text[m_pos] == '+' || m_text[m_pos] == '-')) {
            TCHAR sign = m_text[m_pos++];
            if (!ParseFactor(value)) return false;
            if (sign == '-') value = -value;
            return true;
        }
        if (m_pos < m_text.GetLength() && m_text[m_pos] == '(') {
            ++m_pos;
            if (!ParseExpression(value)) return false;
            SkipSpace();
            return m_pos < m_text.GetLength() && m_text[m_pos++] == ')';
        }

        SkipSpace();
        LPCTSTR start = (LPCTSTR)m_text + m_pos;
        TCHAR* end = NULL;
        value = _tcstod(start, &end);
        if (end == start) return false;
        m_pos += (int)(end - start);
        return std::isfinite(value);
    }
};

class JsonPrettyPrinter
{
public:
    explicit JsonPrettyPrinter(const std::string& text) : m_text(text), m_pos(0) {}

    bool Format(std::string& output)
    {
        output.clear();
        if (!ParseValue(output, 0)) return false;
        SkipSpace();
        return m_pos == m_text.size();
    }

private:
    const std::string& m_text;
    size_t m_pos;

    void SkipSpace()
    {
        while (m_pos < m_text.size() &&
            (m_text[m_pos] == ' ' || m_text[m_pos] == '\t' || m_text[m_pos] == '\r' || m_text[m_pos] == '\n')) ++m_pos;
    }

    static void Indent(std::string& output, int level)
    {
        output.append((size_t)level * 2, ' ');
    }

    bool ParseString(std::string& output)
    {
        SkipSpace();
        if (m_pos >= m_text.size() || m_text[m_pos] != '"') return false;
        size_t start = m_pos++;
        while (m_pos < m_text.size()) {
            unsigned char c = (unsigned char)m_text[m_pos++];
            if (c == '"') {
                output.append(m_text, start, m_pos - start);
                return true;
            }
            if (c < 0x20) return false;
            if (c == '\\') {
                if (m_pos >= m_text.size()) return false;
                char escaped = m_text[m_pos++];
                if (escaped == 'u') {
                    for (int i = 0; i < 4; ++i) {
                        if (m_pos >= m_text.size() || !isxdigit((unsigned char)m_text[m_pos++])) return false;
                    }
                }
                else if (strchr("\"\\/bfnrt", escaped) == NULL) return false;
            }
        }
        return false;
    }

    bool ParseNumber(std::string& output)
    {
        SkipSpace();
        size_t start = m_pos;
        if (m_pos < m_text.size() && m_text[m_pos] == '-') ++m_pos;
        if (m_pos >= m_text.size()) return false;
        if (m_text[m_pos] == '0') ++m_pos;
        else if (m_text[m_pos] >= '1' && m_text[m_pos] <= '9') {
            while (m_pos < m_text.size() && isdigit((unsigned char)m_text[m_pos])) ++m_pos;
        }
        else return false;
        if (m_pos < m_text.size() && m_text[m_pos] == '.') {
            ++m_pos;
            size_t digits = m_pos;
            while (m_pos < m_text.size() && isdigit((unsigned char)m_text[m_pos])) ++m_pos;
            if (digits == m_pos) return false;
        }
        if (m_pos < m_text.size() && (m_text[m_pos] == 'e' || m_text[m_pos] == 'E')) {
            ++m_pos;
            if (m_pos < m_text.size() && (m_text[m_pos] == '+' || m_text[m_pos] == '-')) ++m_pos;
            size_t digits = m_pos;
            while (m_pos < m_text.size() && isdigit((unsigned char)m_text[m_pos])) ++m_pos;
            if (digits == m_pos) return false;
        }
        output.append(m_text, start, m_pos - start);
        return true;
    }

    bool ParseArray(std::string& output, int level)
    {
        ++m_pos;
        output += '[';
        SkipSpace();
        if (m_pos < m_text.size() && m_text[m_pos] == ']') {
            ++m_pos;
            output += ']';
            return true;
        }
        output += '\n';
        while (true) {
            Indent(output, level + 1);
            if (!ParseValue(output, level + 1)) return false;
            SkipSpace();
            if (m_pos >= m_text.size()) return false;
            if (m_text[m_pos] == ']') {
                ++m_pos;
                output += '\n';
                Indent(output, level);
                output += ']';
                return true;
            }
            if (m_text[m_pos++] != ',') return false;
            output += ",\n";
        }
    }

    bool ParseObject(std::string& output, int level)
    {
        ++m_pos;
        output += '{';
        SkipSpace();
        if (m_pos < m_text.size() && m_text[m_pos] == '}') {
            ++m_pos;
            output += '}';
            return true;
        }
        output += '\n';
        while (true) {
            Indent(output, level + 1);
            if (!ParseString(output)) return false;
            SkipSpace();
            if (m_pos >= m_text.size() || m_text[m_pos++] != ':') return false;
            output += ": ";
            if (!ParseValue(output, level + 1)) return false;
            SkipSpace();
            if (m_pos >= m_text.size()) return false;
            if (m_text[m_pos] == '}') {
                ++m_pos;
                output += '\n';
                Indent(output, level);
                output += '}';
                return true;
            }
            if (m_text[m_pos++] != ',') return false;
            output += ",\n";
        }
    }

    bool ParseValue(std::string& output, int level)
    {
        SkipSpace();
        if (m_pos >= m_text.size()) return false;
        if (m_text[m_pos] == '"') return ParseString(output);
        if (m_text[m_pos] == '{') return ParseObject(output, level);
        if (m_text[m_pos] == '[') return ParseArray(output, level);
        if (m_text.compare(m_pos, 4, "true") == 0) { output += "true"; m_pos += 4; return true; }
        if (m_text.compare(m_pos, 5, "false") == 0) { output += "false"; m_pos += 5; return true; }
        if (m_text.compare(m_pos, 4, "null") == 0) { output += "null"; m_pos += 4; return true; }
        return ParseNumber(output);
    }
};

static bool UriEncode(const CString& input, CString& output)
{
    std::string utf8;
    if (!CStringToUtf8Text(input, utf8)) return false;
    static const char hex[] = "0123456789ABCDEF";
    std::string encoded;
    for (size_t i = 0; i < utf8.size(); ++i) {
        unsigned char c = (unsigned char)utf8[i];
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') || c == '-' || c == '.' || c == '_' || c == '~') {
            encoded += (char)c;
        }
        else {
            encoded += '%';
            encoded += hex[c >> 4];
            encoded += hex[c & 0x0F];
        }
    }
    output = encoded.c_str();
    return true;
}

static int HexDigit(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

static bool UriDecode(const CString& input, CString& output)
{
    std::string encoded((LPCTSTR)input);
    std::string utf8;
    for (size_t i = 0; i < encoded.size(); ++i) {
        if (encoded[i] != '%') {
            if ((unsigned char)encoded[i] > 0x7F) return false;
            utf8 += encoded[i];
            continue;
        }
        if (i + 2 >= encoded.size()) return false;
        int high = HexDigit(encoded[i + 1]);
        int low = HexDigit(encoded[i + 2]);
        if (high < 0 || low < 0) return false;
        utf8 += (char)((high << 4) | low);
        i += 2;
    }
    return Utf8TextToCString(utf8, output);
}

static bool EvaluateCommand(const CString& expression, CString& result, CString& error)
{
    CString command(expression);
    command.Trim();
    int fallbackSeparator = command.Find(_T('|'));
    if (fallbackSeparator >= 0 && command.Left(fallbackSeparator).Trim() == _T("clipboard")) {
        CString fallback = command.Mid(fallbackSeparator + 1);
        fallback.TrimLeft();
        fallback.TrimRight();
        if (!ReadClipboardText(result)) {
            error = _T("Clipboard could not be read.");
            return false;
        }
        if (result.IsEmpty()) result = fallback;
        return true;
    }
    CString argumentText;
    int open = command.Find('(');
    if (open >= 0) {
        if (command.IsEmpty() || command[command.GetLength() - 1] != ')') {
            error = _T("テンプレートの引数形式が正しくありません。");
            return false;
        }
        argumentText = command.Mid(open + 1, command.GetLength() - open - 2);
        command = command.Left(open);
        command.Trim();
    }

    std::vector<CString> args;
    if (!ParseArguments(argumentText, args)) {
        error = _T("テンプレートの引数形式が正しくありません。");
        return false;
    }

    if (command == _T("date")) {
        if (!EvaluateDate(args, result)) {
            error = _T("dateの引数または書式が正しくありません。");
            return false;
        }
        return true;
    }

    if (!args.empty()) {
        error = _T("このテンプレートコマンドは引数を受け付けません。");
        return false;
    }

    CString clipboard;
    if (command == _T("clipboard") || command == _T("sort") || command == _T("sort_desc") ||
        command == _T("calc") || command == _T("json") || command == _T("uri_encode") ||
        command == _T("uri_decode")) {
        if (!ReadClipboardText(clipboard)) {
            error = _T("クリップボードを読み込めませんでした。");
            return false;
        }
    }

    if (command == _T("clipboard")) {
        result = clipboard;
        return true;
    }
    if (command == _T("sort")) {
        result = SortClipboardLines(clipboard, false);
        return true;
    }
    if (command == _T("sort_desc")) {
        result = SortClipboardLines(clipboard, true);
        return true;
    }
    if (command == _T("calc")) {
        double value = 0;
        ExpressionParser parser(clipboard);
        if (!parser.Parse(value)) {
            error = _T("クリップボードの数式を計算できませんでした。");
            return false;
        }
        result.Format(_T("%.15g"), value == 0.0 ? 0.0 : value);
        return true;
    }
    if (command == _T("json")) {
        std::string utf8;
        std::string formatted;
        if (!CStringToUtf8Text(clipboard, utf8) || !JsonPrettyPrinter(utf8).Format(formatted) ||
            !Utf8TextToCString(formatted, result)) {
            error = _T("クリップボードのJSONを解析できませんでした。");
            return false;
        }
        result.Replace(_T("\n"), _T("\r\n"));
        return true;
    }
    if (command == _T("uri_encode")) {
        if (!UriEncode(clipboard, result)) {
            error = _T("URIエンコードに失敗しました。");
            return false;
        }
        return true;
    }
    if (command == _T("uri_decode")) {
        if (!UriDecode(clipboard, result)) {
            error = _T("URIデコードに失敗しました。");
            return false;
        }
        return true;
    }

    error.Format(_T("未定義のテンプレートコマンドです: %s"), command);
    return false;
}

bool EvaluateTemplate(const CString& source, CString& output, CString& error)
{
    output.Empty();
    error.Empty();
    int pos = 0;
    while (pos < source.GetLength()) {
        int open = source.Find(_T("{{"), pos);
        if (open < 0) {
            output += source.Mid(pos);
            return true;
        }
        output += source.Mid(pos, open - pos);
        int close = source.Find(_T("}}"), open + 2);
        if (close < 0) {
            error = _T("テンプレートの閉じ括弧がありません。");
            output.Empty();
            return false;
        }
        CString expression = source.Mid(open + 2, close - open - 2);
        CString replacement;
        if (!EvaluateCommand(expression, replacement, error)) {
            output.Empty();
            return false;
        }
        output += replacement;
        pos = close + 2;
    }
    return true;
}

bool ExpandClipboardTags(const CString& source, const CString& clipboard, CString& output)
{
    output.Empty();
    int pos = 0;
    while (pos < source.GetLength()) {
        int open = source.Find(_T("{{"), pos);
        if (open < 0) {
            output += source.Mid(pos);
            return true;
        }
        output += source.Mid(pos, open - pos);
        int close = source.Find(_T("}}"), open + 2);
        if (close < 0) return false;
        CString expression = source.Mid(open + 2, close - open - 2);
        CString command(expression);
        command.Trim();
        int separator = command.Find(_T('|'));
        if (command == _T("clipboard")) {
            output += clipboard;
        }
        else if (separator >= 0 && command.Left(separator).Trim() == _T("clipboard")) {
            CString fallback = command.Mid(separator + 1);
            fallback.TrimLeft();
            fallback.TrimRight();
            output += clipboard.IsEmpty() ? fallback : clipboard;
        }
        else {
            output += source.Mid(open, close - open + 2);
        }
        pos = close + 2;
    }
    return true;
}