#pragma once

#include "DataValueList.h"

bool LoadJsonDataFile(const CString& dataPath, CArray<ItemData, ItemData&>& loaded, CString* pError = NULL);
void SaveJsonDataFile(const CString& dataPath, const CArray<ItemData, ItemData&>& items);
