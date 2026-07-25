#pragma once
#include <afxtempl.h>
#include <string>
#include <vector>

enum DataFileFormat {
    DATA_FILE_FORMAT_TEXT = 0,
    DATA_FILE_FORMAT_JSON = 1
};

struct ItemData {
    CString name;
    CString value;
    int type;
    std::vector<std::string> jsonExtraProperties;
    ItemData() : type(0) {}
};

class CDataValueList
{
private:
    CArray<ItemData, ItemData&> m_arr;

public:
    enum { MAX_ITEMS = 100, MAX_FILE_BYTES = 10 * 1024 * 1024 };

    CDataValueList();
    ~CDataValueList();

    bool LoadAll(CString dataPath, DataFileFormat format, CString* pError = NULL);
    void SaveAll(CString dataPath, DataFileFormat format);

    ItemData& Datas(int i);
    int GetCount() const;

    void MoveUp(int i);
    void MoveDown(int i);
    void ClearAll() { m_arr.RemoveAll(); }

    bool Add(CString name, CString value) {
        if (m_arr.GetSize() >= MAX_ITEMS) {
            return false;
        }
        ItemData data;
        data.name = name;
        data.value = value;
        m_arr.Add(data);
        return true;
    }

    void Remove(int i) {
        if (i >= 0 && i < m_arr.GetSize()) {
            m_arr.RemoveAt(i);
        }
    }
};
