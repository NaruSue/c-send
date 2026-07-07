#pragma once
#include <afxtempl.h>

// ƒf[ƒ^‚ÌŠí
struct CategoryData {
    CString name;
    CString path;
    int type;
    CategoryData() : type(0) {}
};

class CCategoryDataList {
private:
    CArray<CategoryData, CategoryData&> m_arr;

public:
    CCategoryDataList();
    ~CCategoryDataList();

    // Core operations
    void LoadAll(LPCTSTR iniPath);
    void SaveAll(LPCTSTR iniPath);

    // Data access
    CategoryData& Datas(int i);
    int GetCount() const;

    // Reorder helpers (admin UI)
    void MoveUp(int i);
    void MoveDown(int i);

    void Remove(int i) {
        if (i >= 0 && i < m_arr.GetSize()) {
            m_arr.RemoveAt(i);
        }
    }

    // Data operations
    void Add(CategoryData data);
};
