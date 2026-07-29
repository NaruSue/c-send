#pragma once

bool EvaluateTemplate(const CString& source, CString& output, CString& error);
bool ExpandClipboardTags(const CString& source, const CString& clipboard, CString& output);

