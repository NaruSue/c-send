#include "stdafx.h"
#include "csend.h"
#include "ApiDetailDlg.h"
#include "ApiActionDlg.h"
#include "ApiClient.h"
#include "DialogFontUtil.h"

class CApiKeyValueDlg : public CDialog
{
public:
    CApiKeyValueDlg(const CString& key, const CString& value, CWnd* parent = NULL)
        : CDialog(IDD_DIALOG1, parent), m_key(key), m_value(value) {}
    CString GetKey() const { return m_key; }
    CString GetValue() const { return m_value; }

protected:
    BOOL OnInitDialog()
    {
        CDialog::OnInitDialog();
        SetWindowText(_T("設定key"));
        SetWindowPos(NULL, 0, 0, 460, 220, SWP_NOMOVE | SWP_NOZORDER);
        const int ids[] = { IDC_EDIT1, IDC_EDIT2, IDC_RADIO_PLAIN, IDC_RADIO_TEMPLATE,
            IDC_RADIO_API, IDC_LABEL_API, IDC_COMBO_API, IDC_LABEL_ACTION, IDC_COMBO_ACTION };
        for (int index = 0; index < _countof(ids); ++index) {
            CWnd* control = GetDlgItem(ids[index]);
            if (control != NULL) control->ShowWindow(SW_HIDE);
        }
        CRect client;
        GetClientRect(&client);
        m_keyLabel.Create(_T("設定項目"), WS_CHILD | WS_VISIBLE,
            CRect(14, 24, 104, 48), this);
        m_keyEdit.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            CRect(116, 18, client.right - 14, 48), this, IDC_JSON_NODE_KEY);
        m_valueLabel.Create(_T("値"), WS_CHILD | WS_VISIBLE,
            CRect(14, 68, 104, 92), this);
        m_valueEdit.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            CRect(116, 62, client.right - 14, 92), this, IDC_JSON_NODE_VALUE);
        m_keyEdit.SetWindowText(m_key);
        m_valueEdit.SetWindowText(m_value);
        CWnd* ok = GetDlgItem(IDOK);
        CWnd* cancel = GetDlgItem(IDCANCEL);
        if (ok != NULL) ok->SetWindowPos(NULL, client.right - 204, client.bottom - 52,
            92, 40, SWP_NOZORDER);
        if (cancel != NULL) cancel->SetWindowPos(NULL, client.right - 102, client.bottom - 52,
            92, 40, SWP_NOZORDER);
        ApplyConfiguredDialogFontAndLayout(this, m_dialogFont);
        return TRUE;
    }

    void OnOK()
    {
        m_keyEdit.GetWindowText(m_key);
        m_valueEdit.GetWindowText(m_value);
        if (m_key.IsEmpty()) {
            AfxMessageBox(_T("設定項目を入力してください。"), MB_OK | MB_ICONERROR);
            return;
        }
        CDialog::OnOK();
    }

private:
    CString m_key;
    CString m_value;
    CStatic m_keyLabel;
    CStatic m_valueLabel;
    CEdit m_keyEdit;
    CEdit m_valueEdit;
    CFont m_dialogFont;
};

BEGIN_MESSAGE_MAP(CApiDetailDlg, CDialog)
    ON_CBN_SELCHANGE(IDC_API_AUTH_TYPE, &CApiDetailDlg::OnAuthChanged)
    ON_BN_CLICKED(ID_API_KEY_ADD, &CApiDetailDlg::OnKeyAdd)
    ON_BN_CLICKED(ID_API_KEY_EDIT, &CApiDetailDlg::OnKeyEdit)
    ON_BN_CLICKED(ID_API_KEY_DELETE, &CApiDetailDlg::OnKeyDelete)
    ON_BN_CLICKED(ID_API_ACTION_ADD, &CApiDetailDlg::OnActionAdd)
    ON_BN_CLICKED(ID_API_ACTION_EDIT, &CApiDetailDlg::OnActionEdit)
    ON_BN_CLICKED(ID_API_ACTION_DELETE, &CApiDetailDlg::OnActionDelete)
    ON_NOTIFY(NM_DBLCLK, IDC_API_ACTION_LIST, &CApiDetailDlg::OnActionDoubleClick)
END_MESSAGE_MAP()

CApiDetailDlg::CApiDetailDlg(const CString& filePath, CWnd* parent)
    : CDialog(IDD_DIALOG1, parent), m_filePath(filePath),
      m_existing(PathFileExists(filePath))
{
}

void CApiDetailDlg::HideTemplateControls()
{
    const int ids[] = { IDC_EDIT1, IDC_EDIT2, IDC_RADIO_PLAIN, IDC_RADIO_TEMPLATE,
        IDC_RADIO_API, IDC_LABEL_API, IDC_COMBO_API, IDC_LABEL_ACTION, IDC_COMBO_ACTION };
    for (int index = 0; index < _countof(ids); ++index) {
        CWnd* control = GetDlgItem(ids[index]);
        if (control != NULL) control->ShowWindow(SW_HIDE);
    }
}

BOOL CApiDetailDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    HideTemplateControls();
    SetWindowText(_T("API詳細"));
    SetWindowPos(NULL, 0, 0, 900, 720, SWP_NOMOVE | SWP_NOZORDER);
    CRect client;
    GetClientRect(&client);

    CString error;
    if (m_existing && !LoadApiConfigDocument(m_filePath, m_document, error)) {
        AfxMessageBox(_T("API設定を読み込めません。\n") + error, MB_OK | MB_ICONERROR);
    }
    if (m_document.id.IsEmpty()) {
        CString fileName = PathFindFileName(m_filePath);
        int dot = fileName.ReverseFind(_T('.'));
        m_document.id = dot > 0 ? fileName.Left(dot) : fileName;
    }
    const int left = 14;
    const int labelWidth = 132;
    const int editLeft = left + labelWidth;
    const int right = client.right - 14;
    m_nameLabel.Create(_T("サービス名称"), WS_CHILD | WS_VISIBLE,
        CRect(left, 18, editLeft - 6, 44), this);
    m_nameEdit.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
        CRect(editLeft, 14, right, 44), this, IDC_API_NAME);
    m_urlLabel.Create(_T("基本URL"), WS_CHILD | WS_VISIBLE,
        CRect(left, 58, editLeft - 6, 84), this);
    m_urlEdit.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
        CRect(editLeft, 54, right - 240, 84), this, IDC_API_BASE_URL);
    m_timeoutLabel.Create(_T("Timeout(ms)"), WS_CHILD | WS_VISIBLE,
        CRect(right - 230, 58, right - 104, 84), this);
    m_timeoutEdit.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
        CRect(right - 100, 54, right, 84), this, IDC_API_TIMEOUT);
    m_authLabel.Create(_T("認証タイプ"), WS_CHILD | WS_VISIBLE,
        CRect(left, 98, editLeft - 6, 124), this);
    m_authCombo.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | CBS_DROPDOWNLIST,
        CRect(editLeft, 94, editLeft + 250, 300), this, IDC_API_AUTH_TYPE);
    const TCHAR* authTypes[] = { _T("none"), _T("api-key-header"),
        _T("api-key-query"), _T("bearer"), _T("basic") };
    for (int index = 0; index < _countof(authTypes); ++index) m_authCombo.AddString(authTypes[index]);
    m_credentialLabel.Create(_T("クレデンシャルID"), WS_CHILD | WS_VISIBLE,
        CRect(editLeft + 270, 98, editLeft + 452, 124), this);
    m_credentialEdit.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
        CRect(editLeft + 452, 94, right, 124), this, IDC_API_CREDENTIAL_ID);
    m_tokenLabel.Create(_T("API token"), WS_CHILD | WS_VISIBLE,
        CRect(left, 138, editLeft - 6, 164), this);
    m_tokenEdit.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_PASSWORD | ES_AUTOHSCROLL,
        CRect(editLeft, 134, right, 164), this, IDC_API_TOKEN);
    m_usernameLabel.Create(_T("ID"), WS_CHILD | WS_VISIBLE,
        CRect(left, 138, editLeft - 6, 164), this);
    m_usernameEdit.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
        CRect(editLeft, 134, client.right / 2 - 12, 164), this, IDC_API_USERNAME);
    m_passwordLabel.Create(_T("PASS"), WS_CHILD | WS_VISIBLE,
        CRect(client.right / 2 + 4, 138, client.right / 2 + 70, 164), this);
    m_passwordEdit.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_PASSWORD | ES_AUTOHSCROLL,
        CRect(client.right / 2 + 72, 134, right, 164), this, IDC_API_PASSWORD);

    m_keyLabel.Create(_T("設定key定義"), WS_CHILD | WS_VISIBLE,
        CRect(left, 184, 180, 210), this);
    m_keyList.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_SINGLESEL |
        LVS_SHOWSELALWAYS, CRect(left, 210, right, 332), this, IDC_API_KEY_LIST);
    m_keyList.InsertColumn(0, _T("設定項目"), LVCFMT_LEFT, 250);
    m_keyList.InsertColumn(1, _T("値"), LVCFMT_LEFT, client.Width() - 300);
    m_keyAdd.Create(_T("追加"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        CRect(right - 300, 170, right - 208, 204), this, ID_API_KEY_ADD);
    m_keyEdit.Create(_T("編集"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        CRect(right - 198, 170, right - 106, 204), this, ID_API_KEY_EDIT);
    m_keyDelete.Create(_T("削除"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        CRect(right - 96, 170, right, 204), this, ID_API_KEY_DELETE);

    m_actionLabel.Create(_T("Actionリスト"), WS_CHILD | WS_VISIBLE,
        CRect(left, 350, 180, 376), this);
    m_actionList.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_SINGLESEL |
        LVS_SHOWSELALWAYS, CRect(left, 378, right, client.bottom - 72),
        this, IDC_API_ACTION_LIST);
    m_actionList.InsertColumn(0, _T("Action名"), LVCFMT_LEFT, 210);
    m_actionList.InsertColumn(1, _T("Method"), LVCFMT_LEFT, 90);
    m_actionList.InsertColumn(2, _T("URL"), LVCFMT_LEFT, client.Width() - 350);
    m_actionAdd.Create(_T("追加"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        CRect(right - 300, 338, right - 208, 372), this, ID_API_ACTION_ADD);
    m_actionEdit.Create(_T("編集"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        CRect(right - 198, 338, right - 106, 372), this, ID_API_ACTION_EDIT);
    m_actionDelete.Create(_T("削除"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        CRect(right - 96, 338, right, 372), this, ID_API_ACTION_DELETE);

    m_nameEdit.SetWindowText(m_document.name);
    m_urlEdit.SetWindowText(m_document.baseUrl);
    CString timeout;
    timeout.Format(_T("%lu"), m_document.timeoutMs);
    m_timeoutEdit.SetWindowText(timeout);
    int auth = m_authCombo.FindStringExact(-1, m_document.authType);
    m_authCombo.SetCurSel(auth == CB_ERR ? 0 : auth);
    m_credentialEdit.SetWindowText(m_document.credentialId);
    m_keyList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    m_actionList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    RefreshKeyList();
    RefreshActionList();
    UpdateAuthControls();

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
    const double layoutScale = ApplyConfiguredDialogFontAndLayout(this, m_dialogFont);
    CRect keyClient;
    m_keyList.GetClientRect(&keyClient);
    const int keyColumnWidth = ScaleInt(250, layoutScale);
    m_keyList.SetColumnWidth(0, keyColumnWidth);
    m_keyList.SetColumnWidth(1, max(80, keyClient.Width() - keyColumnWidth - 4));
    CRect actionClient;
    m_actionList.GetClientRect(&actionClient);
    const int actionNameWidth = ScaleInt(210, layoutScale);
    const int actionMethodWidth = ScaleInt(90, layoutScale);
    m_actionList.SetColumnWidth(0, actionNameWidth);
    m_actionList.SetColumnWidth(1, actionMethodWidth);
    m_actionList.SetColumnWidth(2,
        max(100, actionClient.Width() - actionNameWidth - actionMethodWidth - 4));
    return TRUE;
}

CString CApiDetailDlg::CurrentAuthType() const
{
    CString value;
    int selection = m_authCombo.GetCurSel();
    if (selection != CB_ERR) m_authCombo.GetLBText(selection, value);
    return value;
}

void CApiDetailDlg::UpdateAuthControls()
{
    CString auth = CurrentAuthType();
    BOOL none = auth == _T("none");
    BOOL basic = auth == _T("basic");
    m_credentialLabel.ShowWindow(none ? SW_HIDE : SW_SHOW);
    m_credentialEdit.ShowWindow(none ? SW_HIDE : SW_SHOW);
    m_tokenLabel.ShowWindow(!none && !basic ? SW_SHOW : SW_HIDE);
    m_tokenEdit.ShowWindow(!none && !basic ? SW_SHOW : SW_HIDE);
    m_usernameLabel.ShowWindow(basic ? SW_SHOW : SW_HIDE);
    m_usernameEdit.ShowWindow(basic ? SW_SHOW : SW_HIDE);
    m_passwordLabel.ShowWindow(basic ? SW_SHOW : SW_HIDE);
    m_passwordEdit.ShowWindow(basic ? SW_SHOW : SW_HIDE);
}

void CApiDetailDlg::OnAuthChanged()
{
    CString auth = CurrentAuthType();
    if ((auth == _T("api-key-header") || auth == _T("bearer")) &&
        m_document.keyConfig.Find(_T("headerName")) == NULL) {
        CString name = auth == _T("bearer") ? _T("Authorization") : _T("X-API-Key");
        m_document.keyConfig.Set(_T("headerName"), ApiJsonValue::String(name));
    }
    if (auth == _T("api-key-query") &&
        m_document.keyConfig.Find(_T("queryName")) == NULL) {
        m_document.keyConfig.Set(_T("queryName"), ApiJsonValue::String(_T("api_key")));
    }
    if ((auth == _T("api-key-header") || auth == _T("api-key-query") ||
        auth == _T("bearer")) && m_document.keyConfig.Find(_T("prefix")) == NULL) {
        CString prefix = auth == _T("bearer") ? _T("Bearer ") : CString();
        m_document.keyConfig.Set(_T("prefix"), ApiJsonValue::String(prefix));
    }
    RefreshKeyList();
    UpdateAuthControls();
}

static CString KeyDisplayName(const CString& key)
{
    if (key == _T("headerName")) return _T("ヘッダー名");
    if (key == _T("queryName")) return _T("クエリ名");
    if (key == _T("prefix")) return _T("プレフィックス");
    const CString staticHeaderPrefix = _T("staticHeader.");
    if (key.Left(staticHeaderPrefix.GetLength()).CompareNoCase(staticHeaderPrefix) == 0) {
        return _T("固定ヘッダー: ") + key.Mid(staticHeaderPrefix.GetLength());
    }
    return key;
}

void CApiDetailDlg::RefreshKeyList()
{
    m_keyList.DeleteAllItems();
    if (m_document.keyConfig.type != ApiJsonValue::TYPE_OBJECT) {
        m_document.keyConfig = ApiJsonValue::Object();
    }
    for (size_t index = 0; index < m_document.keyConfig.children.size(); ++index) {
        const ApiJsonValue& value = m_document.keyConfig.children[index];
        CString shown = value.type == ApiJsonValue::TYPE_STRING ? value.scalar : ApiJsonTypeName(value.type);
        int item = m_keyList.InsertItem((int)index,
            KeyDisplayName(m_document.keyConfig.keys[index]));
        m_keyList.SetItemText(item, 1, shown);
        m_keyList.SetItemData(item, (DWORD_PTR)index);
    }
}

void CApiDetailDlg::RefreshActionList()
{
    m_actionList.DeleteAllItems();
    for (size_t index = 0; index < m_document.actions.size(); ++index) {
        const ApiActionDocument& action = m_document.actions[index];
        int item = m_actionList.InsertItem((int)index, action.name);
        m_actionList.SetItemText(item, 1, action.method);
        m_actionList.SetItemText(item, 2, action.url);
        m_actionList.SetItemData(item, (DWORD_PTR)index);
    }
}

void CApiDetailDlg::OnKeyAdd()
{
    CApiKeyValueDlg dialog(CString(), CString(), this);
    if (dialog.DoModal() != IDOK) return;
    if (m_document.keyConfig.Find(dialog.GetKey()) != NULL) {
        AfxMessageBox(_T("同じ設定項目が存在します。"), MB_OK | MB_ICONERROR);
        return;
    }
    m_document.keyConfig.Set(dialog.GetKey(), ApiJsonValue::String(dialog.GetValue()));
    RefreshKeyList();
}

void CApiDetailDlg::OnKeyEdit()
{
    POSITION position = m_keyList.GetFirstSelectedItemPosition();
    if (position == NULL) return;
    int selected = m_keyList.GetNextSelectedItem(position);
    size_t index = (size_t)m_keyList.GetItemData(selected);
    if (index >= m_document.keyConfig.children.size()) return;
    CApiKeyValueDlg dialog(m_document.keyConfig.keys[index],
        m_document.keyConfig.children[index].scalar, this);
    if (dialog.DoModal() != IDOK) return;
    for (size_t check = 0; check < m_document.keyConfig.keys.size(); ++check) {
        if (check != index && m_document.keyConfig.keys[check] == dialog.GetKey()) {
            AfxMessageBox(_T("同じ設定項目が存在します。"), MB_OK | MB_ICONERROR);
            return;
        }
    }
    m_document.keyConfig.keys[index] = dialog.GetKey();
    m_document.keyConfig.children[index] = ApiJsonValue::String(dialog.GetValue());
    RefreshKeyList();
}

void CApiDetailDlg::OnKeyDelete()
{
    POSITION position = m_keyList.GetFirstSelectedItemPosition();
    if (position == NULL) return;
    int selected = m_keyList.GetNextSelectedItem(position);
    m_document.keyConfig.Remove((size_t)m_keyList.GetItemData(selected));
    RefreshKeyList();
}

void CApiDetailDlg::OnActionAdd()
{
    ApiActionDocument action;
    CApiActionDlg dialog(action, this);
    if (dialog.DoModal() != IDOK) return;
    const ApiActionDocument& added = dialog.GetAction();
    for (size_t index = 0; index < m_document.actions.size(); ++index) {
        if (m_document.actions[index].id.CompareNoCase(added.id) == 0) {
            AfxMessageBox(_T("同じAction IDが存在します。"), MB_OK | MB_ICONERROR);
            return;
        }
    }
    m_document.actions.push_back(added);
    RefreshActionList();
}

void CApiDetailDlg::OnActionEdit()
{
    POSITION position = m_actionList.GetFirstSelectedItemPosition();
    if (position == NULL) return;
    int selected = m_actionList.GetNextSelectedItem(position);
    size_t index = (size_t)m_actionList.GetItemData(selected);
    if (index >= m_document.actions.size()) return;
    CApiActionDlg dialog(m_document.actions[index], this);
    if (dialog.DoModal() != IDOK) return;
    const ApiActionDocument& edited = dialog.GetAction();
    for (size_t check = 0; check < m_document.actions.size(); ++check) {
        if (check != index &&
            m_document.actions[check].id.CompareNoCase(edited.id) == 0) {
            AfxMessageBox(_T("同じAction IDが存在します。"), MB_OK | MB_ICONERROR);
            return;
        }
    }
    m_document.actions[index] = edited;
    RefreshActionList();
}

void CApiDetailDlg::OnActionDelete()
{
    POSITION position = m_actionList.GetFirstSelectedItemPosition();
    if (position == NULL) return;
    int selected = m_actionList.GetNextSelectedItem(position);
    size_t index = (size_t)m_actionList.GetItemData(selected);
    if (index >= m_document.actions.size()) return;
    if (AfxMessageBox(_T("選択したActionを削除しますか？"),
        MB_YESNO | MB_ICONQUESTION) != IDYES) return;
    m_document.actions.erase(m_document.actions.begin() + index);
    RefreshActionList();
}

void CApiDetailDlg::OnActionDoubleClick(NMHDR*, LRESULT* result)
{
    OnActionEdit();
    if (result != NULL) *result = 0;
}

bool CApiDetailDlg::SaveCredentials(CString& error)
{
    if (m_document.authType == _T("none")) return true;
    CString base = _T("C-Send/API/") + m_document.credentialId + _T("/");
    if (m_document.authType == _T("basic")) {
        CString username;
        CString password;
        m_usernameEdit.GetWindowText(username);
        m_passwordEdit.GetWindowText(password);
        if (!username.IsEmpty() && !WriteApiCredentialValue(base + _T("username"), username)) {
            error = _T("IDをWindows Credential Managerへ保存できません。");
            return false;
        }
        if (!password.IsEmpty() && !WriteApiCredentialValue(base + _T("password"), password)) {
            error = _T("PASSをWindows Credential Managerへ保存できません。");
            return false;
        }
    }
    else {
        CString token;
        m_tokenEdit.GetWindowText(token);
        if (!token.IsEmpty() && !WriteApiCredentialValue(base + _T("token"), token)) {
            error = _T("API tokenをWindows Credential Managerへ保存できません。");
            return false;
        }
    }
    return true;
}

void CApiDetailDlg::OnOK()
{
    m_nameEdit.GetWindowText(m_document.name);
    m_urlEdit.GetWindowText(m_document.baseUrl);
    CString timeout;
    m_timeoutEdit.GetWindowText(timeout);
    m_document.timeoutMs = (DWORD)_tcstoul(timeout, NULL, 10);
    m_document.authType = CurrentAuthType();
    m_credentialEdit.GetWindowText(m_document.credentialId);
    if (m_document.name.IsEmpty() || m_document.baseUrl.IsEmpty()) {
        AfxMessageBox(_T("サービス名称と基本URLを入力してください。"), MB_OK | MB_ICONERROR);
        return;
    }
    if (m_document.id.IsEmpty()) {
        AfxMessageBox(_T("API設定IDを決定できません。"), MB_OK | MB_ICONERROR);
        return;
    }
    if (m_document.baseUrl.Left(7).CompareNoCase(_T("http://")) != 0 &&
        m_document.baseUrl.Left(8).CompareNoCase(_T("https://")) != 0) {
        AfxMessageBox(_T("基本URLはhttp://またはhttps://から入力してください。"),
            MB_OK | MB_ICONERROR);
        return;
    }
    if (m_document.timeoutMs == 0) {
        AfxMessageBox(_T("Timeoutを入力してください。"), MB_OK | MB_ICONERROR);
        return;
    }
    if (m_document.authType != _T("none") && m_document.credentialId.IsEmpty()) {
        AfxMessageBox(_T("クレデンシャルIDを入力してください。"), MB_OK | MB_ICONERROR);
        return;
    }
    if ((m_document.authType == _T("api-key-header") ||
        m_document.authType == _T("bearer")) &&
        m_document.keyConfig.Find(_T("headerName")) == NULL) {
        AfxMessageBox(_T("設定key定義にヘッダー名を設定してください。"),
            MB_OK | MB_ICONERROR);
        return;
    }
    if (m_document.authType == _T("api-key-query") &&
        m_document.keyConfig.Find(_T("queryName")) == NULL) {
        AfxMessageBox(_T("設定key定義にクエリ名を設定してください。"),
            MB_OK | MB_ICONERROR);
        return;
    }
    if (m_document.actions.empty()) {
        AfxMessageBox(_T("Actionを1件以上登録してください。"), MB_OK | MB_ICONERROR);
        return;
    }
    CString error;
    if (!SaveCredentials(error) || !SaveApiConfigDocument(m_filePath, m_document, error)) {
        AfxMessageBox(error, MB_OK | MB_ICONERROR);
        return;
    }
    CDialog::OnOK();
}
