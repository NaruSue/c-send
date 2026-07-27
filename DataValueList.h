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
    CString mode;
    int type;
    std::vector<std::string> jsonExtraProperties;
    ItemData() : mode(_T("plain")), type(0) {}
};

struct JsonFileMetadata {
    CString createdAt;
    CString categoryName;
    std::vector<std::string> extraProperties;

    void Clear() {
        createdAt.Empty();
        categoryName.Empty();
        extraProperties.clear();
    }
};

class CDataValueList
{
private:
    CArray<ItemData, ItemData&> m_arr;
    JsonFileMetadata m_jsonMetadata;

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
    void ClearAll() {
        m_arr.RemoveAll();
        m_jsonMetadata.Clear();
    }

    bool Add(CString name, CString value, CString mode = _T("plain")) {
        if (m_arr.GetSize() >= MAX_ITEMS) {
            return false;
        }
        ItemData data;
        data.name = name;
        data.value = value;
        data.mode = mode;
        m_arr.Add(data);
        return true;
    }

    void Remove(int i) {
        if (i >= 0 && i < m_arr.GetSize()) {
            m_arr.RemoveAt(i);
        }
    }
};
