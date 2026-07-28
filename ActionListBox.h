#pragma once

#include <afxwin.h>

#define WM_USER_API_RUN_ITEM (WM_APP + 612)

class CActionListBox : public CListBox
{
protected:
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    DECLARE_MESSAGE_MAP()
};
