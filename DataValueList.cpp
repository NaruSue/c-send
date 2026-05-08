#include "stdafx.h"
#include "DataValueList.h"

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
    if (!file.Open(txtPath, CFile::modeRead | CFile::typeText))
    {
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
            data.value = lineVal.Trim();
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
    if (!file.Open(txtPath, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
    {
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
        file.WriteString(data.value + _T("\n"));
    }

    file.Close();
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

