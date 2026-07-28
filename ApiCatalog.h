#pragma once

#include <vector>

struct ApiActionSummary
{
    CString id;
    CString name;
    CString method;
    CString path;
    CString description;
};

struct ApiDefinitionSummary
{
    CString id;
    CString name;
    CString filePath;
    CString baseUrl;
    std::vector<ApiActionSummary> actions;
};

bool LoadApiDefinitions(std::vector<ApiDefinitionSummary>& definitions);
