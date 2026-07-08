#pragma once

#include "afxwin.h"

inline CString GetSettingIniPathFromExe()
{
	TCHAR modulePath[MAX_PATH] = { 0 };
	if (GetModuleFileName(NULL, modulePath, MAX_PATH) == 0) {
		return _T("setting.ini");
	}

	CString path = modulePath;
	int pos = path.ReverseFind(_T('\\'));
	if (pos < 0) {
		pos = path.ReverseFind(_T('/'));
	}
	if (pos >= 0) {
		path = path.Left(pos);
	}
	path += _T("\\setting.ini");
	return path;
}

inline CString ResolveSettingIniPath(const CString& iniPath)
{
	if (!iniPath.IsEmpty()) {
		return iniPath;
	}
	return GetSettingIniPathFromExe();
}

inline void LoadFontSetting(const CString& iniPath, CString& fontName, int& fontSize)
{
	TCHAR buffer[256] = { 0 };
	CString path = ResolveSettingIniPath(iniPath);
	GetPrivateProfileString(_T("font"), _T("name"), _T("MS UI Gothic"), buffer, 256, path);
	fontName = buffer;
	fontSize = GetPrivateProfileInt(_T("font"), _T("size"), 16, path);

	fontName.Trim();
	if (fontName.IsEmpty()) {
		fontName = _T("MS UI Gothic");
	}
	if (fontSize <= 0) {
		fontSize = 16;
	}
}

inline bool CreateFontForSetting(CFont& font, const CString& fontName, int fontSize)
{
	if (fontName.IsEmpty() || fontSize <= 0) {
		return false;
	}

	HDC hdc = ::GetDC(NULL);
	if (hdc == NULL) {
		return false;
	}

	LOGFONT lf{};
	lf.lfHeight = -MulDiv(fontSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	lf.lfCharSet = SHIFTJIS_CHARSET;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	::ReleaseDC(NULL, hdc);
	_tcsncpy_s(lf.lfFaceName, fontName, LF_FACESIZE - 1);

	font.DeleteObject();
	return font.CreateFontIndirect(&lf) != FALSE;
}

inline int ScaleInt(int value, double scale)
{
	if (scale == 1.0) {
		return value;
	}
	return (int)(value * scale + (value >= 0 ? 0.5 : -0.5));
}

inline void ScaleRect(CRect& rc, double sx, double sy)
{
	rc.left = ScaleInt(rc.left, sx);
	rc.top = ScaleInt(rc.top, sy);
	rc.right = ScaleInt(rc.right, sx);
	rc.bottom = ScaleInt(rc.bottom, sy);
}

inline void ApplyScaledLayout(CWnd* pWnd, double sx, double sy, const UINT* ids, int count)
{
	if (pWnd == NULL || !::IsWindow(pWnd->m_hWnd)) {
		return;
	}

	CRect rcWnd;
	pWnd->GetWindowRect(&rcWnd);
	pWnd->SetWindowPos(NULL, 0, 0, ScaleInt(rcWnd.Width(), sx), ScaleInt(rcWnd.Height(), sy),
		SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

	for (int i = 0; i < count; i++) {
		CWnd* pChild = pWnd->GetDlgItem(ids[i]);
		if (pChild == NULL || !::IsWindow(pChild->m_hWnd)) {
			continue;
		}

		CRect rc;
		pChild->GetWindowRect(&rc);
		pWnd->ScreenToClient(&rc);
		ScaleRect(rc, sx, sy);
		pChild->MoveWindow(&rc);
	}
}
