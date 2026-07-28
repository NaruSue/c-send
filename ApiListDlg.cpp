#include "stdafx.h"
#include "csend.h"
#include "ApiListDlg.h"
#include "ApiDetailDlg.h"
#include "ApiConfigDocument.h"
#include "DialogFontUtil.h"

BEGIN_MESSAGE_MAP(CApiListDlg, CDialog)
    ON_BN_CLICKED(ID_API_NEW, &CApiListDlg::OnNewApi)
    ON_BN_CLICKED(ID_API_DETAIL, &CApiListDlg::OnOpenDetail)
    ON_BN_CLICKED(ID_API_DELETE, &CApiListDlg::OnDeleteApi)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST_CATEGORY, &CApiListDlg::OnDoubleClick)
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

CApiListDlg::CApiListDlg(CWnd* parent)
    : CDialog(CApiListDlg::IDD, parent)
{
}

BOOL CApiListDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    SetRedraw(FALSE);
    SetWindowText(_T("API"));

    const int templateControlIds[] = {
        IDC_EDIT1, IDC_EDIT2, IDC_RADIO_PLAIN, IDC_RADIO_TEMPLATE,
        IDC_RADIO_API, IDC_LABEL_API, IDC_COMBO_API,
        IDC_LABEL_ACTION, IDC_COMBO_ACTION
    };
    for (int index = 0; index < _countof(templateControlIds); ++index) {
        CWnd* control = GetDlgItem(templateControlIds[index]);
        if (control != NULL) control->ShowWindow(SW_HIDE);
    }

    SetWindowPos(NULL, 0, 0, 760, 420, SWP_NOMOVE | SWP_NOZORDER);
    CRect client;
    GetClientRect(&client);
    CWnd* closeButton = GetDlgItem(IDCANCEL);
    if (closeButton != NULL) {
        closeButton->SetWindowText(_T("閉じる"));
        closeButton->SetWindowPos(NULL, client.right - 102, client.bottom - 50, 92, 40,
            SWP_NOZORDER);
        closeButton->ShowWindow(SW_SHOW);
    }

    if (!m_list.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL |
        LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS,
        CRect(10, 10, client.right - 10, client.bottom - 62), this, IDC_LIST_CATEGORY)) {
        SetRedraw(TRUE);
        RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_ERASE |
            RDW_FRAME | RDW_ALLCHILDREN | RDW_UPDATENOW);
        return TRUE;
    }
    m_list.SetFont(GetFont());
    m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    m_list.InsertColumn(0, _T("サービス名称"), LVCFMT_LEFT, 120);
    m_list.InsertColumn(1, _T("設定ID"), LVCFMT_LEFT, 100);
    m_list.InsertColumn(2, _T("基本URL"), LVCFMT_LEFT, 390);
    m_list.InsertColumn(3, _T("Action数"), LVCFMT_RIGHT, 100);
    m_newButton.Create(_T("新規作成"), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
        CRect(10, client.bottom - 50, 128, client.bottom - 10), this, ID_API_NEW);
    m_newButton.SetFont(GetFont());
    m_detailButton.Create(_T("詳細"), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
        CRect(138, client.bottom - 50, 230, client.bottom - 10), this, ID_API_DETAIL);
    m_detailButton.SetFont(GetFont());
    m_deleteButton.Create(_T("削除"), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
        CRect(240, client.bottom - 50, 332, client.bottom - 10), this, ID_API_DELETE);
    m_deleteButton.SetFont(GetFont());
    TCHAR modulePath[MAX_PATH] = {};
    ::GetModuleFileName(NULL, modulePath, _countof(modulePath));
    CString directory(modulePath);
    int separator = directory.ReverseFind(_T('\\'));
    if (separator >= 0) directory = directory.Left(separator);
    m_apiDirectory = directory + _T("\\api");
    CString searchPath = m_apiDirectory + _T("\\*.json");
    const double layoutScale = ApplyConfiguredDialogFontAndLayout(this, m_dialogFont);
    const int nameWidth = ScaleInt(120, layoutScale);
    const int idWidth = ScaleInt(100, layoutScale);
    const int actionWidth = ScaleInt(100, layoutScale);
    CRect listClient;
    m_list.GetClientRect(&listClient);
    m_list.SetColumnWidth(0, nameWidth);
    m_list.SetColumnWidth(1, idWidth);
    m_list.SetColumnWidth(2,
        max(160, listClient.Width() - nameWidth - idWidth - actionWidth - 4));
    m_list.SetColumnWidth(3, actionWidth);
    LoadApiFiles();
    SetRedraw(TRUE);
    RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_ERASE |
        RDW_FRAME | RDW_ALLCHILDREN | RDW_UPDATENOW);
    return TRUE;
}

BOOL CApiListDlg::OnEraseBkgnd(CDC* dc)
{
    if (dc == NULL) return CDialog::OnEraseBkgnd(dc);
    CRect client;
    GetClientRect(&client);
    dc->FillSolidRect(&client, GetSysColor(COLOR_3DFACE));
    return TRUE;
}

void CApiListDlg::LoadApiFiles()
{
    m_list.DeleteAllItems();
    m_filePaths.clear();
    CString searchPath = m_apiDirectory + _T("\\*.json");
    WIN32_FIND_DATA findData = {};
    HANDLE findHandle = ::FindFirstFile(searchPath, &findData);
    if (findHandle != INVALID_HANDLE_VALUE) {
        do {
            CString filePath = m_apiDirectory + _T("\\") + findData.cFileName;
            ApiConfigDocument document;
            CString error;
            CString name = findData.cFileName;
            CString id;
            CString baseUrl;
            CString actionCount = _T("-");
            if (LoadApiConfigDocument(filePath, document, error)) {
                name = document.name;
                id = document.id;
                baseUrl = document.baseUrl;
                actionCount.Format(_T("%u"), (UINT)document.actions.size());
            }
            int item = m_list.InsertItem(m_list.GetItemCount(), name);
            m_list.SetItemText(item, 1, id);
            m_list.SetItemText(item, 2, baseUrl);
            m_list.SetItemText(item, 3, actionCount);
            m_list.SetItemData(item, (DWORD_PTR)m_filePaths.size());
            m_filePaths.push_back(filePath);
        } while (::FindNextFile(findHandle, &findData));
        ::FindClose(findHandle);
    }
    if (m_list.GetItemCount() > 0) {
        m_list.SetItemState(0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
    }
}

void CApiListDlg::OnOK()
{
    OnOpenDetail();
}

void CApiListDlg::OnNewApi()
{
    CString path;
    for (int number = 1; number < 1000; ++number) {
        CString name;
        name.Format(_T("new-api-%d.json"), number);
        path = m_apiDirectory + _T("\\") + name;
        if (!PathFileExists(path)) break;
    }
    CApiDetailDlg dialog(path, this);
    if (dialog.DoModal() == IDOK) LoadApiFiles();
}

CString CApiListDlg::GetSelectedFilePath() const
{
    POSITION position = m_list.GetFirstSelectedItemPosition();
    if (position == NULL) return CString();
    int selection = m_list.GetNextSelectedItem(position);
    size_t index = (size_t)m_list.GetItemData(selection);
    return index < m_filePaths.size() ? m_filePaths[index] : CString();
}

void CApiListDlg::OnOpenDetail()
{
    CString path = GetSelectedFilePath();
    if (path.IsEmpty()) return;
    CApiDetailDlg dialog(path, this);
    dialog.DoModal();
    LoadApiFiles();
}

void CApiListDlg::OnDeleteApi()
{
    CString path = GetSelectedFilePath();
    if (path.IsEmpty()) return;
    if (AfxMessageBox(_T("選択したAPI設定を削除しますか？"),
        MB_YESNO | MB_ICONQUESTION) != IDYES) return;
    if (!::DeleteFile(path)) {
        AfxMessageBox(_T("API設定を削除できません。"), MB_OK | MB_ICONERROR);
        return;
    }
    LoadApiFiles();
}

void CApiListDlg::OnDoubleClick(NMHDR*, LRESULT* result)
{
    OnOpenDetail();
    if (result != NULL) *result = 0;
}
