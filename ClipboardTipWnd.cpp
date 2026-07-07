#include "stdafx.h"
#include "ClipboardTipWnd.h"

namespace
{
	const UINT_PTR kTipTimerId = 1;
	const int kPaddingX = 12;
	const int kPaddingY = 8;
	const int kMaxTextWidth = 320;
}

BEGIN_MESSAGE_MAP(CClipboardTipWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

CClipboardTipWnd::CClipboardTipWnd()
	: m_timeoutMs(3000)
{
}

CClipboardTipWnd::~CClipboardTipWnd()
{
	HideTip();
}

BOOL CClipboardTipWnd::ShowTip(const CString& text, UINT timeoutMs, const CPoint& anchor)
{
	if (text.IsEmpty()) {
		return FALSE;
	}

	m_text = text;
	m_timeoutMs = (timeoutMs == 0) ? 3000 : timeoutMs;

	if (!::IsWindow(m_hWnd)) {
		CString className = AfxRegisterWndClass(
			CS_HREDRAW | CS_VREDRAW,
			::LoadCursor(NULL, IDC_ARROW),
			(HBRUSH)::GetStockObject(NULL_BRUSH),
			NULL);

		if (!CreateEx(
			WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
			className,
			_T(""),
			WS_POPUP,
			0, 0, 0, 0,
			NULL,
			0)) {
			return FALSE;
		}
	}

	CSize textSize = MeasureText(m_text, kMaxTextWidth);
	CSize windowSize(textSize.cx + kPaddingX * 2, textSize.cy + kPaddingY * 2);
	PositionAndShow(windowSize, anchor);

	KillTimer(kTipTimerId);
	SetTimer(kTipTimerId, m_timeoutMs, NULL);
	Invalidate();
	UpdateWindow();
	ShowWindow(SW_SHOWNOACTIVATE);
	return TRUE;
}

void CClipboardTipWnd::HideTip()
{
	if (!::IsWindow(m_hWnd)) {
		return;
	}

	KillTimer(kTipTimerId);
	ShowWindow(SW_HIDE);
	DestroyWindow();
}

void CClipboardTipWnd::OnPaint()
{
	CPaintDC dc(this);
	CRect rcClient;
	GetClientRect(&rcClient);

	CBrush background(RGB(32, 32, 32));
	CBrush* pOldBrush = dc.SelectObject(&background);
	CPen border(PS_SOLID, 1, RGB(96, 96, 96));
	CPen* pOldPen = dc.SelectObject(&border);
	dc.Rectangle(&rcClient);
	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldBrush);

	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(255, 255, 255));

	CFont* pFont = CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
	CFont* pOldFont = NULL;
	if (pFont != NULL) {
		pOldFont = dc.SelectObject(pFont);
	}

	CRect rcText = rcClient;
	rcText.DeflateRect(kPaddingX, kPaddingY);
	dc.DrawText(m_text, &rcText, DT_LEFT | DT_TOP | DT_WORDBREAK | DT_NOPREFIX);

	if (pOldFont != NULL) {
		dc.SelectObject(pOldFont);
	}
}

void CClipboardTipWnd::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == kTipTimerId) {
		HideTip();
		return;
	}
	CWnd::OnTimer(nIDEvent);
}

BOOL CClipboardTipWnd::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

CSize CClipboardTipWnd::MeasureText(const CString& text, int maxWidth) const
{
	CDC screenDC;
	screenDC.Attach(::GetDC(NULL));

	CFont* pFont = CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
	CFont* pOldFont = NULL;
	if (pFont != NULL) {
		pOldFont = screenDC.SelectObject(pFont);
	}

	CRect rc(0, 0, maxWidth, 0);
	screenDC.DrawText(text, &rc, DT_LEFT | DT_TOP | DT_WORDBREAK | DT_CALCRECT | DT_NOPREFIX);

	if (pOldFont != NULL) {
		screenDC.SelectObject(pOldFont);
	}
	::ReleaseDC(NULL, screenDC.Detach());

	return rc.Size();
}

void CClipboardTipWnd::PositionAndShow(const CSize& size, const CPoint& anchor)
{
	CRect workArea;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);

	int x = anchor.x + 16;
	int y = anchor.y + 16;

	if (x + size.cx > workArea.right) {
		x = workArea.right - size.cx - 8;
	}
	if (y + size.cy > workArea.bottom) {
		y = anchor.y - size.cy - 16;
	}
	if (x < workArea.left) {
		x = workArea.left + 8;
	}
	if (y < workArea.top) {
		y = workArea.top + 8;
	}

	SetWindowPos(&wndTopMost, x, y, size.cx, size.cy,
		SWP_NOACTIVATE | SWP_SHOWWINDOW);
}
