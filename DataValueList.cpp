#include "stdafx.h"
#include "DataValueList.h"
#include "IniTextUtil.h"

static bool TryParseStrictInt(const CString& text, int& value)
{
    CString s(text);
    s.Trim();
    if (s.IsEmpty()) {
        return false;
    }

    TCHAR* end = NULL;
    long parsed = _tcstol(s, &end, 10);
    if (end == s.GetString() || *end != _T('\0')) {
        return false;
    }

    value = (int)parsed;
    return true;
}

static CString EscapeValueForStorage(const CString& input)
{
    CString out;
    out.Preallocate(input.GetLength() * 2 + 4);
    for (int i = 0; i < input.GetLength(); ++i) {
        TCHAR c = input[i];
        switch (c) {
        case '\\':
            out += _T("\\\\");
            break;
        case '\r':
            out += _T("\\r");
            break;
        case '\n':
            out += _T("\\n");
            break;
        case ',':
            out += _T("\\,");
            break;
        default:
            out += c;
            break;
        }
    }
    return out;
}

static CString UnescapeValueFromStorage(const CString& input)
{
    CString out;
    out.Preallocate(input.GetLength());
    int i = 0;
    int n = input.GetLength();
    while (i < n) {
        TCHAR c = input[i];
        if (c != '\\') {
            out += c;
            ++i;
            continue;
        }

        if (i + 1 >= n) {
            out += '\\';
            break;
        }

        TCHAR nx = input[i + 1];
        switch (nx) {
        case 'r':
            out += '\r';
            break;
        case 'n':
            out += '\n';
            break;
        case '\\':
            out += '\\';
            break;
        case ',':
            out += ',';
            break;
        default:
            out += nx;
            break;
        }
        i += 2;
    }
    return out;
}

static bool IsBinaryDataFile(const CString& path, CString* pError)
{
    CFile file;
    if (!file.Open(path, CFile::modeRead | CFile::typeBinary)) {
        if (pError != NULL) {
            *pError = GetIniMessage(_T(""), _T("data_read_failed"), _T("データファイルを読み込めませんでした。"));
        }
        return true;
    }

    const ULONGLONG len = file.GetLength();
    if (len > CDataValueList::MAX_FILE_BYTES) {
        file.Close();
        if (pError != NULL) {
            CString fmt = GetIniMessage(_T(""), _T("data_too_large"), _T("データファイルが大きすぎます。上限は %d バイトです。"));
            pError->Format(fmt, CDataValueList::MAX_FILE_BYTES);
        }
        return true;
    }

    BYTE buffer[4096];
    UINT bytesRead = 0;
    while ((bytesRead = file.Read(buffer, sizeof(buffer))) > 0) {
        for (UINT i = 0; i < bytesRead; ++i) {
            if (buffer[i] == 0) {
                file.Close();
                if (pError != NULL) {
                    *pError = GetIniMessage(_T(""), _T("data_binary_ng"), _T("バイナリ形式のデータファイルは使えません。"));
                }
                return true;
            }
        }
    }

    file.Close();
    return false;
}

CDataValueList::CDataValueList()
{
}

CDataValueList::~CDataValueList()
{
    m_arr.RemoveAll();
}

// 3行1ユニット形式での全件読み込み
bool CDataValueList::LoadAll(CString txtPath, CString* pError)
{
    m_arr.RemoveAll();
    if (pError != NULL) {
        pError->Empty();
    }

    if (txtPath.IsEmpty()) {
        return false;
    }

    CString error;
    if (IsBinaryDataFile(txtPath, &error)) {
        if (pError != NULL) {
            *pError = error;
        }
        return false;
    }

    CStdioFile file;
    try {
        if (!file.Open(txtPath, CFile::modeRead | CFile::typeText)) {
            if (pError != NULL) {
                *pError = GetIniMessage(_T(""), _T("data_read_failed"), _T("データファイルを読み込めませんでした。"));
            }
            return false;
        }
    }
    catch (CFileException&) {
        if (pError != NULL) {
            *pError = GetIniMessage(_T(""), _T("data_read_failed"), _T("データファイルを読み込めませんでした。"));
        }
        return false;
    }

    CArray<ItemData, ItemData&> loaded;
    CString lineName, lineType, lineVal;

    while (file.ReadString(lineName))
    {
        lineName.Trim();
        if (lineName.IsEmpty()) {
            continue;
        }

        if (!file.ReadString(lineType) || !file.ReadString(lineVal)) {
            file.Close();
            if (pError != NULL) {
                *pError = GetIniMessage(_T(""), _T("data_format_ng"), _T("データファイルの形式が正しくありません。"));
            }
            return false;
        }

        ItemData data;
        data.name = lineName;
        if (!TryParseStrictInt(lineType, data.type)) {
            file.Close();
            if (pError != NULL) {
                *pError = GetIniMessage(_T(""), _T("data_format_ng"), _T("データファイルの形式が正しくありません。"));
            }
            return false;
        }
        data.value = UnescapeValueFromStorage(lineVal);

        if (loaded.GetSize() >= MAX_ITEMS) {
            file.Close();
            if (pError != NULL) {
                *pError = GetIniMessage(_T(""), _T("data_count_ng"), _T("データファイルは100件までです。"));
            }
            return false;
        }

        loaded.Add(data);
    }

    file.Close();

    m_arr.RemoveAll();
    for (int i = 0; i < loaded.GetSize(); ++i) {
        m_arr.Add(loaded[i]);
    }

    return true;
}

// 全件保存
void CDataValueList::SaveAll(CString txtPath)
{
    CStdioFile file;
    try {
        if (!file.Open(txtPath, CFile::modeCreate | CFile::modeWrite | CFile::typeText)) {
            CString msg;
            CString fmt = GetIniMessage(_T(""), _T("data_write_failed"), _T("データファイルを書き込めませんでした: %s"));
            msg.Format(fmt, txtPath);
            AfxMessageBox(msg);
            return;
        }

        int count = (int)m_arr.GetSize();
        for (int i = 0; i < count; i++)
        {
            ItemData& data = m_arr.ElementAt(i);

            CString strType;
            strType.Format(_T("%d"), data.type);

            file.WriteString(data.name + _T("\n"));
            file.WriteString(strType + _T("\n"));
            file.WriteString(EscapeValueForStorage(data.value) + _T("\n"));
        }

        file.Close();
    }
    catch (CFileException&)
    {
        CString msg;
        CString fmt = GetIniMessage(_T(""), _T("data_write_error"), _T("データファイルの書き込み中にエラーが発生しました: %s"));
        msg.Format(fmt, txtPath);
        AfxMessageBox(msg);
        if (file.m_pStream != NULL) {
            file.Abort();
        }
        return;
    }
}

// データアクセス
ItemData& CDataValueList::Datas(int i)
{
    ASSERT(i >= 0 && i < m_arr.GetSize());
    return m_arr.ElementAt(i);
}

int CDataValueList::GetCount() const
{
    return (int)m_arr.GetSize();
}

// 並び替え：上へ
void CDataValueList::MoveUp(int i)
{
    if (i > 0 && i < m_arr.GetSize())
    {
        ItemData temp = m_arr[i];
        m_arr.RemoveAt(i);
        m_arr.InsertAt(i - 1, temp);
    }
}

// 並び替え：下へ
void CDataValueList::MoveDown(int i)
{
    if (i >= 0 && i < (int)m_arr.GetSize() - 1)
    {
        ItemData temp = m_arr[i];
        m_arr.RemoveAt(i);
        m_arr.InsertAt(i + 1, temp);
    }
}
