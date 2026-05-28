#include "stdafx.h"
#include "CategoryDataList.h"

CCategoryDataList::CCategoryDataList() {}
CCategoryDataList::~CCategoryDataList() {}

// 全件読み込み
void CCategoryDataList::LoadAll(LPCTSTR iniPath) {
    m_arr.RemoveAll();
    int count = GetPrivateProfileInt(_T("category"), _T("count"), 0, iniPath);

    if (count < 0) count = 0; // sanity

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
    // read previous count so we can remove any leftover keys
    int oldCount = GetPrivateProfileInt(_T("category"), _T("count"), 0, iniPath);

    CString strCount;
    strCount.Format(_T("%d"), count);
    BOOL ok = WritePrivateProfileString(_T("category"), _T("count"), strCount, iniPath);
    if (!ok) {
        AfxMessageBox(_T("Failed to write category count to INI."));
    }

    for (int i = 0; i < count; i++) {
        CString k, p;
        k.Format(_T("item%03d"), i );
        p.Format(_T("path%03d"), i );

        if (!WritePrivateProfileString(_T("category"), k, m_arr[i].name, iniPath)) {
            AfxMessageBox(_T("Failed to write category name to INI."));
        }
        if (!WritePrivateProfileString(_T("category"), p, m_arr[i].path, iniPath)) {
            AfxMessageBox(_T("Failed to write category path to INI."));
        }
    }

    // remove any leftover keys from previous larger count
    for (int i = count; i < oldCount; i++) {
        CString k, p;
        k.Format(_T("item%03d"), i );
        p.Format(_T("path%03d"), i );
        // passing NULL removes the key
        WritePrivateProfileString(_T("category"), k, NULL, iniPath);
        WritePrivateProfileString(_T("category"), p, NULL, iniPath);
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
