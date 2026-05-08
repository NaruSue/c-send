#pragma once
#include <afxtempl.h>

// データの器
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

    // 主要機能
    void LoadAll(LPCTSTR iniPath);
    void SaveAll(LPCTSTR iniPath);

    // データアクセス
    CategoryData& Datas(int i);
    int GetCount() const;

    // 並び替え補助（管理画面用）
    void MoveUp(int i);
    void MoveDown(int i);

    void Remove(int i) {
        if (i >= 0 && i < m_arr.GetSize()) {
            m_arr.RemoveAt(i);
        }
    }

    // ???f?[?^?{?^?C?g
    void Add(CategoryData data);
};
