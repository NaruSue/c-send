#include "stdafx.h"
#include "ActionListBox.h"

BEGIN_MESSAGE_MAP(CActionListBox, CListBox)
    ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

void CActionListBox::OnLButtonUp(UINT nFlags, CPoint point)
{
    BOOL outside = FALSE;
    int index = ItemFromPoint(point, outside);
    if (!outside && point.x >= 0) {
        CRect client;
        GetClientRect(&client);
        if (point.x >= client.right - 56) {
            SetCurSel(index);
            CWnd* parent = GetParent();
            if (parent != NULL) {
                LRESULT handled = parent->SendMessage(WM_USER_API_RUN_ITEM, (WPARAM)index, 0);
                if (handled != 0) return;
            }
        }
    }
    CListBox::OnLButtonUp(nFlags, point);
}
