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
        if (point.x >= client.right - 96) {
            SetCurSel(index);
            CWnd* parent = GetParent();
            if (parent != NULL) {
                UINT message = point.x < client.right - 50 ?
                    WM_USER_API_COPY_RESULT : WM_USER_API_RUN_ITEM;
                LRESULT handled = parent->SendMessage(message, (WPARAM)index, 0);
                if (handled != 0) return;
            }
        }
    }
    CListBox::OnLButtonUp(nFlags, point);
}

void CActionListBox::MeasureItem(LPMEASUREITEMSTRUCT measureItemStruct)
{
    if (measureItemStruct == NULL) return;
    CClientDC dc(this);
    CFont* font = GetFont();
    CFont* oldFont = font != NULL ? dc.SelectObject(font) : NULL;
    TEXTMETRIC metrics = {};
    dc.GetTextMetrics(&metrics);
    if (oldFont != NULL) dc.SelectObject(oldFont);
    measureItemStruct->itemHeight = max(28, metrics.tmHeight + 8);
}

void CActionListBox::DrawItem(LPDRAWITEMSTRUCT drawItemStruct)
{
    if (drawItemStruct == NULL || drawItemStruct->itemID == (UINT)-1) return;
    CDC dc;
    dc.Attach(drawItemStruct->hDC);
    CRect rect(drawItemStruct->rcItem);
    BOOL selected = (drawItemStruct->itemState & ODS_SELECTED) != 0;
    COLORREF background = selected ? GetSysColor(COLOR_HIGHLIGHT) : GetSysColor(COLOR_WINDOW);
    COLORREF foreground = selected ? GetSysColor(COLOR_HIGHLIGHTTEXT) : GetSysColor(COLOR_WINDOWTEXT);
    CBrush brush(background);
    dc.FillRect(&rect, &brush);
    CString text;
    GetText((int)drawItemStruct->itemID, text);
    CRect textRect(rect.left + 6, rect.top, rect.right - 100, rect.bottom);
    dc.SetBkMode(TRANSPARENT);
    dc.SetTextColor(foreground);
    dc.DrawText(text, &textRect, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);

    LRESULT state = -1;
    CWnd* parent = GetParent();
    if (parent != NULL) state = parent->SendMessage(WM_USER_API_GET_STATE,
        (WPARAM)drawItemStruct->itemID, 0);
    if (state >= 0) {
        int centerX = rect.right - 25;
        int centerY = rect.CenterPoint().y;
        COLORREF stateColor = RGB(38, 158, 74);
        if (state == 1) {
            stateColor = RGB(224, 168, 0);
        }
        else if (state == 3) {
            stateColor = RGB(210, 45, 45);
        }
        else if (state == 4) {
            stateColor = RGB(128, 128, 128);
        }
        CPen pen(PS_SOLID, 2, stateColor);
        CPen* oldPen = dc.SelectObject(&pen);
        if (state == 1) {
            CBrush stateBrush(stateColor);
            CBrush* oldBrush = dc.SelectObject(&stateBrush);
            dc.Ellipse(centerX - 10, centerY - 2, centerX - 6, centerY + 2);
            dc.Ellipse(centerX - 2, centerY - 2, centerX + 2, centerY + 2);
            dc.Ellipse(centerX + 6, centerY - 2, centerX + 10, centerY + 2);
            dc.SelectObject(oldBrush);
        }
        else if (state == 3) {
            dc.MoveTo(centerX - 8, centerY - 8);
            dc.LineTo(centerX + 8, centerY + 8);
            dc.MoveTo(centerX + 8, centerY - 8);
            dc.LineTo(centerX - 8, centerY + 8);
        }
        else if (state == 4) {
            dc.Ellipse(centerX - 9, centerY - 9, centerX + 9, centerY + 9);
            dc.MoveTo(centerX - 7, centerY + 7);
            dc.LineTo(centerX + 7, centerY - 7);
        }
        else {
            CBrush stateBrush(stateColor);
            CBrush* oldBrush = dc.SelectObject(&stateBrush);
            POINT triangle[3] = {
                { centerX - 5, centerY - 9 },
                { centerX - 5, centerY + 9 },
                { centerX + 8, centerY }
            };
            dc.Polygon(triangle, 3);
            dc.SelectObject(oldBrush);
        }
        dc.SelectObject(oldPen);

        LRESULT hasResult = parent != NULL ? parent->SendMessage(WM_USER_API_HAS_RESULT,
            (WPARAM)drawItemStruct->itemID, 0) : 0;
        if (hasResult != 0 && state != 1) {
            int resultX = rect.right - 70;
            CPen resultPen(PS_SOLID, 2, RGB(45, 105, 200));
            oldPen = dc.SelectObject(&resultPen);
            dc.Rectangle(resultX - 8, centerY - 8, resultX + 8, centerY + 9);
            dc.MoveTo(resultX - 4, centerY - 11);
            dc.LineTo(resultX + 4, centerY - 11);
            dc.MoveTo(resultX - 4, centerY - 3);
            dc.LineTo(resultX + 4, centerY - 3);
            dc.MoveTo(resultX - 4, centerY + 2);
            dc.LineTo(resultX + 4, centerY + 2);
            dc.SelectObject(oldPen);
        }
    }
    dc.Detach();
}
