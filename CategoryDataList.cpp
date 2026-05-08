#include "stdafx.h"
#include "CategoryDataList.h"

CCategoryDataList::CCategoryDataList() {}
CCategoryDataList::~CCategoryDataList() {}

// 全件読み込み
void CCategoryDataList::LoadAll(LPCTSTR iniPath) {
    m_arr.RemoveAll();
    int count = GetPrivateProfileInt(_T("category"), _T("count"), 0, iniPath);

    for (int i = 0; i < count; i++) {
        CategoryData data;
        TCHAR buf[256];
        CString k, p;

        k.Format(_T("item%03d"), i);
        p.Format(_T("path%03d"), i);

        GetPrivateProfileString(_T("category"), k, _T(""), buf, 256, iniPath);
        data.name = buf;

        GetPrivateProfileString(_T("category"), p, _T(""), buf, 256, iniPath);
        data.path = buf;

        if (!data.name.IsEmpty()) {
            m_arr.Add(data);
        }
    }

    if (m_arr.GetSize() == 0) {
        CategoryData def;
        def.name = _T("default");
        def.path = _T("default.txt");
        m_arr.Add(def);

        // その場ですぐ保存して整合性を取る
        SaveAll(iniPath);
    }
}

// 全件保存
void CCategoryDataList::SaveAll(LPCTSTR iniPath) {
    int count = (int)m_arr.GetSize();
    CString strCount;
    strCount.Format(_T("%d"), count);
    WritePrivateProfileString(_T("category"), _T("count"), strCount, iniPath);

    for (int i = 0; i < count; i++) {
        CString k, p;
        k.Format(_T("item%03d"), i );
        p.Format(_T("path%03d"), i );

        WritePrivateProfileString(_T("category"), k, m_arr[i].name, iniPath);
        WritePrivateProfileString(_T("category"), p, m_arr[i].path, iniPath);
    }
}

// インデックスアクセス
CategoryData& CCategoryDataList::Datas(int i) {
    ASSERT(i >= 0 && i < m_arr.GetSize());
    return m_arr.ElementAt(i);
}

int CCategoryDataList::GetCount() const {
    return (int)m_arr.GetSize();
}

// 上へ移動
void CCategoryDataList::MoveUp(int i) {
    if (i > 0 && i < m_arr.GetSize()) {
        CategoryData temp = m_arr[i];
        m_arr.RemoveAt(i);
        m_arr.InsertAt(i - 1, temp);
    }
}

// 下へ移動
void CCategoryDataList::MoveDown(int i) {
    if (i >= 0 && i < m_arr.GetSize() - 1) {
        CategoryData temp = m_arr[i];
        m_arr.RemoveAt(i);
        m_arr.InsertAt(i + 1, temp);
    }
}

// ???f?[?^?{?^?C?g
void CCategoryDataList::Add(CategoryData data) {
    m_arr.Add(data);
}
