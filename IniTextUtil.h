#pragma once

#include "DialogFontUtil.h"

inline CString GetIniMessage(const CString& iniPath, LPCTSTR key, LPCTSTR defaultText)
{
	CString path = ResolveSettingIniPath(iniPath);
	TCHAR buffer[1024] = { 0 };
	GetPrivateProfileString(_T("message"), key, defaultText, buffer, _countof(buffer), path);
	if (buffer[0] == _T('\0')) {
		return defaultText;
	}
	return buffer;
}

inline void EnsureIniMessageKey(const CString& iniPath, LPCTSTR key, LPCTSTR defaultText)
{
	CString path = ResolveSettingIniPath(iniPath);
	TCHAR buffer[8] = { 0 };
	if (GetPrivateProfileString(_T("message"), key, _T(""), buffer, _countof(buffer), path) == 0 || buffer[0] == _T('\0')) {
		WritePrivateProfileString(_T("message"), key, defaultText, path);
	}
}

inline void EnsureDefaultIniMessages(const CString& iniPath)
{
	EnsureIniMessageKey(iniPath, _T("data_read_failed"), _T("データファイルを読み込めませんでした。"));
	EnsureIniMessageKey(iniPath, _T("data_too_large"), _T("データファイルが大きすぎます。上限は %d バイトです。"));
	EnsureIniMessageKey(iniPath, _T("data_binary_ng"), _T("バイナリ形式のデータファイルは使えません。"));
	EnsureIniMessageKey(iniPath, _T("data_format_ng"), _T("データファイルの形式が正しくありません。"));
	EnsureIniMessageKey(iniPath, _T("data_count_ng"), _T("データファイルは100件までです。"));
	EnsureIniMessageKey(iniPath, _T("data_write_failed"), _T("データファイルを書き込めませんでした: %s"));
	EnsureIniMessageKey(iniPath, _T("data_write_error"), _T("データファイルの書き込み中にエラーが発生しました: %s"));
	EnsureIniMessageKey(iniPath, _T("data_add_limit"), _T("データは100件までです。"));
	EnsureIniMessageKey(iniPath, _T("url_read_failed"), _T("URLから読み込めませんでした。"));
	EnsureIniMessageKey(iniPath, _T("read_failed"), _T("読み込みできませんでした。"));
}
