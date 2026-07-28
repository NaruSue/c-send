#include "stdafx.h"
#include "ApiCatalog.h"
#include "ApiConfigDocument.h"

bool LoadApiDefinitions(std::vector<ApiDefinitionSummary>& definitions)
{
    definitions.clear();
    TCHAR modulePath[MAX_PATH] = {};
    ::GetModuleFileName(NULL, modulePath, _countof(modulePath));
    CString directory(modulePath);
    int separator = directory.ReverseFind(_T('\\'));
    if (separator >= 0) directory = directory.Left(separator);
    CString apiDirectory = directory + _T("\\api");
    WIN32_FIND_DATA findData = {};
    HANDLE findHandle = ::FindFirstFile(apiDirectory + _T("\\*.json"), &findData);
    if (findHandle == INVALID_HANDLE_VALUE) return false;
    do {
        CString filePath = apiDirectory + _T("\\") + findData.cFileName;
        ApiConfigDocument document;
        CString error;
        if (!LoadApiConfigDocument(filePath, document, error)) continue;
        bool duplicate = false;
        for (size_t index = 0; index < definitions.size(); ++index) {
            if (definitions[index].id.CompareNoCase(document.id) == 0) {
                duplicate = true;
                break;
            }
        }
        if (duplicate) continue;
        ApiDefinitionSummary definition;
        definition.filePath = filePath;
        definition.id = document.id;
        definition.name = document.name;
        definition.baseUrl = document.baseUrl;
        for (size_t index = 0; index < document.actions.size(); ++index) {
            ApiActionSummary action;
            action.id = document.actions[index].id;
            action.name = document.actions[index].name;
            action.method = document.actions[index].method;
            action.path = document.actions[index].url;
            action.description = document.actions[index].description;
            definition.actions.push_back(action);
        }
        if (!definition.id.IsEmpty() && !definition.actions.empty()) definitions.push_back(definition);
    } while (::FindNextFile(findHandle, &findData));
    ::FindClose(findHandle);
    return !definitions.empty();
}
