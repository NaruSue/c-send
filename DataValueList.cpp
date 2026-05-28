#include "stdafx.h"
#include "DataValueList.h"

// helper: escape/unescape value strings for single-line storage
static CString EscapeValueForStorage(const CString& input) {
    CString out;
    out.Preallocate(input.GetLength() * 2 + 4);
    for (int i = 0; i < input.GetLength(); ++i) {
        TCHAR c = input[i];
        switch (c) {
        case '\\':
            out += _T("\\\\"); // store single backslash as double backslash
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

static CString UnescapeValueFromStorage(const CString& input) {
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
        // c == '\\' and there is possibly an escape sequence
        if (i + 1 >= n) {
            // trailing backslash, treat as literal
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
            // unknown escape, keep the character as-is
            out += nx;
            break;
        }
        i += 2;
    }
    return out;
}

CDataValueList::CDataValueList()
{
}

CDataValueList::~CDataValueList()
{
    m_arr.RemoveAll();
}

// 3行1ユニット形式での全件読み込み
void CDataValueList::LoadAll(CString txtPath)
{
    m_arr.RemoveAll();

    CStdioFile file;
    // 読み込みモードでオープン（ファイルが存在しない場合はそのまま戻る）
    try {
        if (!file.Open(txtPath, CFile::modeRead | CFile::typeText)) {
            return;
        }
    }
    catch (CFileException& ex) {
        CString msg;
        msg.Format(_T("Failed to open data file for read: %s"), txtPath);
        AfxMessageBox(msg);
        return;
    }

    CString lineName, lineType, lineVal;

    // 1行目(name)が読み込める間ループ
    while (file.ReadString(lineName))
    {
        ItemData data;
        data.name = lineName.Trim();
        if (data.name.IsEmpty()) continue; // 空行はスキップ

        // 2行目(type)の読み込み
        if (file.ReadString(lineType))
        {
            data.type = _ttoi(lineType.Trim());
        }

        // 3行目(value)の読み込み
        if (file.ReadString(lineVal))
        {
            // do not Trim() value because leading/trailing spaces may be significant
            data.value = UnescapeValueFromStorage(lineVal);
        }

        // --- typeに応じた拡張ポイント ---
        switch (data.type)
        {
        case 0: // 通常テキスト
        default:
            break;
        }

        m_arr.Add(data);
    }

    file.Close();
}

// 全件保存
void CDataValueList::SaveAll(CString txtPath)
{
    CStdioFile file;
    // 書き込みモード（新規作成・上書き）
    try {
        if (!file.Open(txtPath, CFile::modeCreate | CFile::modeWrite | CFile::typeText)) {
            CString msg;
            msg.Format(_T("Failed to open data file for write: %s"), txtPath);
            AfxMessageBox(msg);
            return;
        }

        int count = (int)m_arr.GetSize();
        for (int i = 0; i < count; i++)
        {
            ItemData& data = m_arr.ElementAt(i);

            // 3行書き出し（最後に改行を付与）
            CString strType;
            strType.Format(_T("%d"), data.type);

            file.WriteString(data.name + _T("\n"));
            file.WriteString(strType + _T("\n"));
            file.WriteString(EscapeValueForStorage(data.value) + _T("\n"));
        }

        file.Close();
    }
    catch (CFileException& ex) {
        CString msg;
        msg.Format(_T("Error writing data file: %s"), txtPath);
        AfxMessageBox(msg);
        // try to close if open
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

