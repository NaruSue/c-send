#pragma once
#include <afxtempl.h>

// データの器
struct ItemData {
    CString name;
    CString value;
    int type;
    ItemData() : type(0) {}
};

class CDataValueList
{
private:
    CArray<ItemData, ItemData&> m_arr;

public:
    CDataValueList();
    ~CDataValueList();

    // 主要機能
    void LoadAll(CString txtPath);
    void SaveAll(CString txtPath);

    // データアクセス
    ItemData& Datas(int i);
    int GetCount() const;

    // 並び替え補助（管理画面用）
    void MoveUp(int i);
    void MoveDown(int i);
    void ClearAll() { m_arr.RemoveAll(); }

    // CDataValueList.h の public: 内に追加
    void Add(CString name, CString value) {
        ItemData data;
        data.name = name;
        data.value = value;
        m_arr.Add(data);
    }

    // 指定したインデックスのデータを削除
    void Remove(int i) {
        if (i >= 0 && i < m_arr.GetSize()) {
            m_arr.RemoveAt(i);
        }
    }
};

