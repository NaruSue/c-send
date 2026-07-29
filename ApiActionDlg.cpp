#include "stdafx.h"
#include "csend.h"
#include "ApiActionDlg.h"
#include "DialogFontUtil.h"

BEGIN_MESSAGE_MAP(CApiActionDlg, CDialog)
END_MESSAGE_MAP()

CApiActionDlg::CApiActionDlg(const ApiActionDocument& action, CWnd* parent)
    : CDialog(IDD_DIALOG1, parent), m_action(action)
{
    if (m_action.method.IsEmpty()) m_action.method = _T("POST");
    if (m_action.response.type == ApiJsonValue::TYPE_NULL) {
        m_action.response = ApiJsonValue::Object();
        m_action.response.Set(_T("result"), ApiJsonValue::String(_T("{{value}}")));
    }
}

const ApiActionDocument& CApiActionDlg::GetAction() const { return m_action; }

void CApiActionDlg::HideTemplateControls()
{
    const int ids[] = { IDC_EDIT1, IDC_EDIT2, IDC_RADIO_PLAIN, IDC_RADIO_TEMPLATE,
        IDC_RADIO_API, IDC_LABEL_API, IDC_COMBO_API, IDC_LABEL_ACTION, IDC_COMBO_ACTION };
    for (int index = 0; index < _countof(ids); ++index) {
        CWnd* control = GetDlgItem(ids[index]);
        if (control != NULL) control->ShowWindow(SW_HIDE);
    }
}

BOOL CApiActionDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    HideTemplateControls();
    SetWindowText(_T("API Action"));
    SetWindowPos(NULL, 0, 0, 860, 680, SWP_NOMOVE | SWP_NOZORDER);
    CRect client;
    GetClientRect(&client);
    const int labelWidth = 124;
    const int left = 14;
    const int editLeft = left + labelWidth;
    const int right = client.right - 14;

    m_nameLabel.Create(_T("Action名"), WS_CHILD | WS_VISIBLE,
        CRect(left, 18, editLeft - 6, 42), this);
    m_nameEdit.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
        CRect(editLeft, 14, right, 42), this, IDC_ACTION_NAME);
    m_idLabel.Create(_T("Action ID"), WS_CHILD | WS_VISIBLE,
        CRect(left, 56, editLeft - 6, 80), this);
    m_idEdit.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
        CRect(editLeft, 52, right, 80), this, IDC_ACTION_ID);
    m_urlLabel.Create(_T("URL"), WS_CHILD | WS_VISIBLE,
        CRect(left, 94, editLeft - 6, 118), this);
    m_urlEdit.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
        CRect(editLeft, 90, right - 210, 118), this, IDC_ACTION_URL);
    m_methodLabel.Create(_T("Method"), WS_CHILD | WS_VISIBLE,
        CRect(right - 200, 94, right - 118, 118), this);
    m_methodCombo.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | CBS_DROPDOWNLIST,
        CRect(right - 110, 90, right, 260), this, IDC_ACTION_METHOD);
    const TCHAR* methods[] = { _T("GET"), _T("POST"), _T("PUT"), _T("PATCH"), _T("DELETE") };
    for (int index = 0; index < _countof(methods); ++index) m_methodCombo.AddString(methods[index]);
    m_descriptionLabel.Create(_T("Description"), WS_CHILD | WS_VISIBLE,
        CRect(left, 132, editLeft - 6, 156), this);
    m_descriptionEdit.Create(WS_CHILD | WS_VISIBLE | WS_BORDER |
        ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL,
        CRect(editLeft, 128, right, 198), this, IDC_ACTION_DESCRIPTION);

    int paneTop = 230;
    int paneBottom = client.bottom - 72;
    int middle = client.right / 2;
    m_requestLabel.Create(_T("Request JSON"), WS_CHILD | WS_VISIBLE,
        CRect(left, paneTop - 26, middle - 14, paneTop), this);
    m_requestEdit.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL |
        WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_WANTRETURN,
        CRect(left, paneTop, middle - 14, paneBottom), this, IDC_ACTION_REQUEST_JSON);
    m_responseLabel.Create(_T("Response JSON"), WS_CHILD | WS_VISIBLE,
        CRect(middle + 14, paneTop - 26, right, paneTop), this);
    m_responseEdit.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL |
        WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_WANTRETURN,
        CRect(middle + 14, paneTop, right, paneBottom), this, IDC_ACTION_RESPONSE_JSON);
    m_requestEdit.SetLimitText(1024 * 1024);
    m_responseEdit.SetLimitText(1024 * 1024);

    m_nameEdit.SetWindowText(m_action.name);
    m_idEdit.SetWindowText(m_action.id);
    m_urlEdit.SetWindowText(m_action.url);
    m_descriptionEdit.SetWindowText(m_action.description);
    int method = m_methodCombo.FindStringExact(-1, m_action.method);
    m_methodCombo.SetCurSel(method == CB_ERR ? 1 : method);
    CString requestText;
    CString responseText;
    if (m_action.request.type != ApiJsonValue::TYPE_NULL) SerializeApiJsonText(m_action.request, requestText, TRUE);
    SerializeApiJsonText(m_action.response, responseText, TRUE);
    requestText.Replace(_T("\n"), _T("\r\n"));
    responseText.Replace(_T("\n"), _T("\r\n"));
    m_requestEdit.SetWindowText(requestText);
    m_responseEdit.SetWindowText(responseText);
    CWnd* ok = GetDlgItem(IDOK);
    CWnd* cancel = GetDlgItem(IDCANCEL);
    if (ok != NULL) {
        ok->SetWindowText(_T("保存"));
        ok->SetWindowPos(NULL, client.right - 204, client.bottom - 54,
            92, 40, SWP_NOZORDER);
    }
    if (cancel != NULL) {
        cancel->SetWindowText(_T("閉じる"));
        cancel->SetWindowPos(NULL, client.right - 102, client.bottom - 54,
            92, 40, SWP_NOZORDER);
    }
    ApplyConfiguredDialogFontAndLayout(this, m_dialogFont);
    return TRUE;
}

void CApiActionDlg::OnOK()
{
    m_nameEdit.GetWindowText(m_action.name);
    m_idEdit.GetWindowText(m_action.id);
    m_urlEdit.GetWindowText(m_action.url);
    m_descriptionEdit.GetWindowText(m_action.description);
    CString requestText;
    CString responseText;
    m_requestEdit.GetWindowText(requestText);
    m_responseEdit.GetWindowText(responseText);
    int method = m_methodCombo.GetCurSel();
    if (method != CB_ERR) m_methodCombo.GetLBText(method, m_action.method);
    if (m_action.name.IsEmpty() || m_action.id.IsEmpty() || m_action.url.IsEmpty()) {
        AfxMessageBox(_T("Action名、Action ID、URLを入力してください。"), MB_OK | MB_ICONERROR);
        return;
    }
    CString error;
    requestText.Trim();
    if (!requestText.IsEmpty() && !ParseApiJsonText(requestText, m_action.request, error)) {
        AfxMessageBox(_T("Request JSONの形式が正しくありません。\n") + error,
            MB_OK | MB_ICONERROR);
        m_requestEdit.SetFocus();
        return;
    }
    if (!ParseApiJsonText(responseText, m_action.response, error)) {
        AfxMessageBox(_T("Response JSONの形式が正しくありません。\n") + error,
            MB_OK | MB_ICONERROR);
        m_responseEdit.SetFocus();
        return;
    }
    if (requestText.IsEmpty()) {
        m_action.request = ApiJsonValue();
    }
    if (m_action.request.type != ApiJsonValue::TYPE_NULL && !ApiJsonContainsValueMarker(m_action.request)) {
        AfxMessageBox(_T("Request JSONを指定する場合は{{value}}を設定してください。"), MB_OK | MB_ICONERROR);
        return;
    }
    if (!ApiJsonContainsValueMarker(m_action.response)) {
        AfxMessageBox(_T("Response JSONに{{value}}を設定してください。"), MB_OK | MB_ICONERROR);
        return;
    }
    CDialog::OnOK();
}
