#pragma once
#include <afxtempl.h>

// ƒf[ƒ^‚ÌŠí
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

    // Core operations
    void LoadAll(CString txtPath);
    void SaveAll(CString txtPath);

    // Data access
    ItemData& Datas(int i);
    int GetCount() const;

    // Reorder helpers (admin UI)
    void MoveUp(int i);
    void MoveDown(int i);
    void ClearAll() { m_arr.RemoveAll(); }

    // Added inside the public section of CDataValueList
    void Add(CString name, CString value) {
        ItemData data;
        data.name = name;
        data.value = value;
        m_arr.Add(data);
    }

    // Remove data at the specified index
    void Remove(int i) {
        if (i >= 0 && i < m_arr.GetSize()) {
            m_arr.RemoveAt(i);
        }
    }
};

