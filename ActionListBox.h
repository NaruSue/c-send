#pragma once

#include <afxwin.h>

#define WM_USER_API_RUN_ITEM (WM_APP + 612)
#define WM_USER_API_GET_STATE (WM_APP + 613)
#define WM_USER_API_HAS_RESULT (WM_APP + 614)
#define WM_USER_API_COPY_RESULT (WM_APP + 615)

class CActionListBox : public CListBox
{
protected:
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    virtual void DrawItem(LPDRAWITEMSTRUCT drawItemStruct);
    virtual void MeasureItem(LPMEASUREITEMSTRUCT measureItemStruct);
    DECLARE_MESSAGE_MAP()
};
