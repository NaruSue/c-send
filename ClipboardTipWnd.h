#pragma once

#include <afxwin.h>

class CClipboardTipWnd : public CWnd
{
public:
	CClipboardTipWnd();
	virtual ~CClipboardTipWnd();

	BOOL ShowTip(const CString& text, UINT timeoutMs, const CPoint& anchor);
	void HideTip();

protected:
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	DECLARE_MESSAGE_MAP()

private:
	CString m_text;
	UINT m_timeoutMs;
	CSize MeasureText(const CString& text, int maxWidth) const;
	void PositionAndShow(const CSize& size, const CPoint& anchor);
};
