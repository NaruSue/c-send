// InputBox.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "csend.h"
#include "InputBox.h"
#include "DialogFontUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInputBox ダイアログ


CInputBox::CInputBox(CWnd* pParent /*=NULL*/)
	: CDialog(CInputBox::IDD, pParent)
{
	m_bViewOnly = FALSE;
	m_bTemplateEnabled = TRUE;
	m_bApiEnabled = TRUE;
	m_bPasswordMode = FALSE;
	m_Mode = _T("plain");
	//{{AFX_DATA_INIT(CInputBox)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
	//}}AFX_DATA_INIT
}


void CInputBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputBox)
	DDX_Control(pDX, IDC_EDIT1, m_Edit);
	DDX_Control(pDX, IDC_EDIT2, m_ETitle);
	DDX_Control(pDX, IDC_RADIO_PLAIN, m_RadioPlain);
	DDX_Control(pDX, IDC_RADIO_TEMPLATE, m_RadioTemplate);
	DDX_Control(pDX, IDC_RADIO_API, m_RadioApi);
	DDX_Control(pDX, IDC_LABEL_API, m_ApiLabel);
	DDX_Control(pDX, IDC_LABEL_ACTION, m_ActionLabel);
	DDX_Control(pDX, IDC_COMBO_API, m_ApiCombo);
	DDX_Control(pDX, IDC_COMBO_ACTION, m_ActionCombo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInputBox, CDialog)
	//{{AFX_MSG_MAP(CInputBox)
	ON_BN_CLICKED(IDC_RADIO_PLAIN, &CInputBox::OnModeChanged)
	ON_BN_CLICKED(IDC_RADIO_TEMPLATE, &CInputBox::OnModeChanged)
	ON_BN_CLICKED(IDC_RADIO_API, &CInputBox::OnModeChanged)
	ON_CBN_SELCHANGE(IDC_COMBO_API, &CInputBox::OnApiSelectionChanged)
	ON_CBN_SELCHANGE(IDC_COMBO_ACTION, &CInputBox::OnActionSelectionChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputBox メッセージ ハンドラ

void CInputBox::SetInputText(CString& title, CString& text )
{
	m_InputText = text;
	m_InputTitle = title;
}

void CInputBox::GetInputText(CString& title, CString& text )
{
	text = m_InputText;
	title = m_InputTitle;
}

void CInputBox::SetMode(const CString& mode)
{
	m_Mode = mode;
	if (m_Mode != _T("plain") && m_Mode != _T("template") && m_Mode != _T("counter") && m_Mode != _T("api")) {
		m_Mode = _T("plain");
	}
}

void CInputBox::GetMode(CString& mode) const
{
	mode = m_Mode;
}

void CInputBox::SetApiEnabled(BOOL enabled)
{
	m_bApiEnabled = enabled;
}

void CInputBox::SetApiSelection(const CString& apiId, const CString& actionId)
{
	m_ApiId = apiId;
	m_ActionId = actionId;
}

void CInputBox::GetApiSelection(CString& apiId, CString& actionId) const
{
	apiId = m_ApiId;
	actionId = m_ActionId;
}

void CInputBox::SetPasswordMode(BOOL enabled)
{
	m_bPasswordMode = enabled;
}

void CInputBox::SetTemplateEnabled(BOOL enabled)
{
	m_bTemplateEnabled = enabled;
	if (!m_bTemplateEnabled) {
		m_Mode = _T("plain");
	}
}

void CInputBox::SetWindowName( CString text )
{
	m_WindowName = text;
}

void CInputBox::SetViewOnly( BOOL bReadOnly )
{
	m_bViewOnly = bReadOnly;
}


void CInputBox::ApplyFontAndLayout()
{
	CString fontName;
	int fontSize = 0;
	LoadFontSetting(GetSettingIniPathFromExe(), fontName, fontSize);

	if (!CreateFontForSetting(m_dialogFont, fontName, fontSize)) {
		return;
	}

	SetFont(&m_dialogFont);
	m_Edit.SetFont(&m_dialogFont);
	m_ETitle.SetFont(&m_dialogFont);
	m_RadioPlain.SetFont(&m_dialogFont);
	m_RadioTemplate.SetFont(&m_dialogFont);
	m_RadioApi.SetFont(&m_dialogFont);
	m_ApiLabel.SetFont(&m_dialogFont);
	m_ActionLabel.SetFont(&m_dialogFont);
	m_ApiCombo.SetFont(&m_dialogFont);
	m_ActionCombo.SetFont(&m_dialogFont);
	CWnd* pOK = GetDlgItem(IDOK);
	if (pOK != NULL) {
		pOK->SetFont(&m_dialogFont);
	}
	CWnd* pCancel = GetDlgItem(IDCANCEL);
	if (pCancel != NULL) {
		pCancel->SetFont(&m_dialogFont);
	}

	double scale = (fontSize > 0) ? ((double)fontSize / 18.0) : 1.0;
	if (scale < 1.0) {
		// 既定テンプレートより小さいフォントでは、レイアウトまで縮めると
		// ボタンやスクロールバーが枠外に押し出されるので縮小しない。
		scale = 1.0;
	}
	if (scale != 1.0) {
		const UINT ids[] = { IDC_EDIT1, IDC_EDIT2, IDC_RADIO_PLAIN, IDC_RADIO_TEMPLATE, IDC_RADIO_API,
			IDC_LABEL_API, IDC_COMBO_API, IDC_LABEL_ACTION, IDC_COMBO_ACTION, IDOK, IDCANCEL };
		ApplyScaledLayout(this, scale, scale, ids, _countof(ids));
	}
}


static BOOL TrySelectAllFocusedEdit(CWnd* pFocus, CWnd* pFirstEdit, CWnd* pSecondEdit)
{
	if (pFocus == NULL) {
		return FALSE;
	}

	if (pFocus->m_hWnd != pFirstEdit->m_hWnd && pFocus->m_hWnd != pSecondEdit->m_hWnd) {
		return FALSE;
	}

	CEdit* pEdit = DYNAMIC_DOWNCAST(CEdit, pFocus);
	if (pEdit == NULL) {
		return FALSE;
	}

	pEdit->SetSel(0, -1);
	return TRUE;
}


static BOOL TryInsertTabIntoBodyEdit(CWnd* pFocus, CWnd* pBodyEdit)
{
	if (pFocus == NULL || pFocus->m_hWnd != pBodyEdit->m_hWnd) {
		return FALSE;
	}

	CEdit* pEdit = DYNAMIC_DOWNCAST(CEdit, pFocus);
	if (pEdit == NULL) {
		return FALSE;
	}

	pEdit->ReplaceSel(_T("\t"));
	return TRUE;
}

BOOL CInputBox::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: この位置に初期化の補足処理を追加してください

	ApplyFontAndLayout();
	m_Edit.GetWindowRect(&m_bodyNormalRect);
	ScreenToClient(&m_bodyNormalRect);
	CRect actionRect;
	m_ActionCombo.GetWindowRect(&actionRect);
	ScreenToClient(&actionRect);
	m_ApiInfo.Create(_T(""), WS_CHILD | SS_LEFTNOWORDWRAP,
		CRect(4, actionRect.bottom + 3, m_bodyNormalRect.right, actionRect.bottom + 27),
		this, IDC_API_ACTION_INFO);
	m_ApiInfo.SetFont(GetFont());
	LoadApiChoices();

	SetWindowText(m_WindowName);

	m_RadioPlain.SetCheck(m_Mode == _T("plain") ? BST_CHECKED : BST_UNCHECKED);
	m_RadioTemplate.SetCheck(m_Mode == _T("template") ? BST_CHECKED : BST_UNCHECKED);
	m_RadioApi.SetCheck(m_Mode == _T("api") ? BST_CHECKED : BST_UNCHECKED);
	m_RadioApi.ShowWindow(m_bApiEnabled ? SW_SHOW : SW_HIDE);
	m_RadioApi.EnableWindow(m_bApiEnabled);
	m_Edit.SetPasswordChar(m_bPasswordMode ? TCHAR(42) : 0);
	UpdateApiControls();
	if (!m_bTemplateEnabled) {
		m_RadioPlain.SetCheck(BST_CHECKED);
		m_RadioTemplate.SetCheck(BST_UNCHECKED);
	m_RadioApi.SetCheck(BST_UNCHECKED);
		m_RadioTemplate.EnableWindow(FALSE);
	m_RadioApi.EnableWindow(FALSE);
	m_ApiCombo.EnableWindow(FALSE);
	m_ActionCombo.EnableWindow(FALSE);
	}

	if( m_bViewOnly ){
		m_Edit.SetReadOnly(TRUE);
		m_ETitle.SetReadOnly(TRUE);
		CWnd* pOK = GetDlgItem(IDOK);
		if( pOK != NULL ){
			pOK->EnableWindow(FALSE);
		}
		m_RadioPlain.EnableWindow(FALSE);
		m_RadioTemplate.EnableWindow(FALSE);
	m_RadioApi.EnableWindow(FALSE);
	}

	if( m_InputText.GetLength() ){
		m_Edit.SetWindowText(m_InputText);
		m_ETitle.SetWindowText(m_InputTitle);
	}
	else{
		m_Edit.Paste();
		m_ETitle.Paste();
	}
	return TRUE;  // コントロールにフォーカスを設定しないとき、戻り値は TRUE となります
	              // 例外: OCX プロパティ ページの戻り値は FALSE となります
}


BOOL CInputBox::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN) {
		if (pMsg->wParam == 'A' && (GetKeyState(VK_CONTROL) & 0x8000) != 0) {
			if (TrySelectAllFocusedEdit(GetFocus(), &m_Edit, &m_ETitle)) {
				return TRUE;
			}
		}

		if (!m_bViewOnly && pMsg->wParam == VK_TAB) {
			if (TryInsertTabIntoBodyEdit(GetFocus(), &m_Edit)) {
				return TRUE;
			}
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CInputBox::OnOK() 
{
	// TODO: この位置にその他の検証用のコードを追加してください
	if( m_bViewOnly ){
		CDialog::OnCancel();
		return;
	}
	m_Edit.GetWindowText(m_InputText);
	m_ETitle.GetWindowText(m_InputTitle);
	if (m_InputTitle.IsEmpty()) {
		m_InputTitle = m_InputText;
	}
	if (m_RadioPlain.GetCheck() == BST_CHECKED) {
		m_Mode = _T("plain");
	}
	else if (m_RadioTemplate.GetCheck() == BST_CHECKED) {
		m_Mode = _T("template");
	}
	else if (m_RadioApi.GetCheck() == BST_CHECKED) {
		m_Mode = _T("api");
		int apiSelection = m_ApiCombo.GetCurSel();
		int actionSelection = m_ActionCombo.GetCurSel();
		if (apiSelection == CB_ERR || actionSelection == CB_ERR) {
			AfxMessageBox(_T("APIとActionを選択してください。"), MB_OK | MB_ICONERROR);
			return;
		}
		int apiIndex = (int)m_ApiCombo.GetItemData(apiSelection);
		if (apiIndex < 0 || apiIndex >= (int)m_apiDefinitions.size()) return;
		m_ApiId = m_apiDefinitions[(size_t)apiIndex].id;
		int actionIndex = (int)m_ActionCombo.GetItemData(actionSelection);
		if (actionIndex < 0 || actionIndex >= (int)m_apiDefinitions[(size_t)apiIndex].actions.size()) return;
		m_ActionId = m_apiDefinitions[(size_t)apiIndex].actions[(size_t)actionIndex].id;
	}

	CDialog::OnOK();
}

void CInputBox::LoadApiChoices()
{
	m_ApiCombo.ResetContent();
	LoadApiDefinitions(m_apiDefinitions);
	int selected = CB_ERR;
	for (size_t index = 0; index < m_apiDefinitions.size(); ++index) {
		CString label = m_apiDefinitions[index].name;
		if (label.IsEmpty()) label = m_apiDefinitions[index].id;
		int position = m_ApiCombo.AddString(label);
		m_ApiCombo.SetItemData(position, (DWORD_PTR)index);
		if (m_apiDefinitions[index].id.CompareNoCase(m_ApiId) == 0) selected = position;
	}
	if (selected == CB_ERR && m_ApiCombo.GetCount() > 0) selected = 0;
	m_ApiCombo.SetCurSel(selected);
	LoadActionChoices();
}

void CInputBox::LoadActionChoices()
{
	m_ActionCombo.ResetContent();
	int apiSelection = m_ApiCombo.GetCurSel();
	if (apiSelection == CB_ERR) return;
	int apiIndex = (int)m_ApiCombo.GetItemData(apiSelection);
	if (apiIndex < 0 || apiIndex >= (int)m_apiDefinitions.size()) return;
	const ApiDefinitionSummary& definition = m_apiDefinitions[(size_t)apiIndex];
	int selected = CB_ERR;
	for (size_t index = 0; index < definition.actions.size(); ++index) {
		const ApiActionSummary& action = definition.actions[index];
		CString label = action.name;
		if (!action.method.IsEmpty()) label += _T(" [") + action.method + _T("]");
		int position = m_ActionCombo.AddString(label);
		m_ActionCombo.SetItemData(position, (DWORD_PTR)index);
		if (action.id.CompareNoCase(m_ActionId) == 0) selected = position;
	}
	if (selected == CB_ERR && m_ActionCombo.GetCount() > 0) selected = 0;
	m_ActionCombo.SetCurSel(selected);
	UpdateApiInfo();
}

void CInputBox::UpdateApiInfo()
{
	CString info;
	int apiSelection = m_ApiCombo.GetCurSel();
	int actionSelection = m_ActionCombo.GetCurSel();
	if (apiSelection != CB_ERR && actionSelection != CB_ERR) {
		int apiIndex = (int)m_ApiCombo.GetItemData(apiSelection);
		int actionIndex = (int)m_ActionCombo.GetItemData(actionSelection);
		if (apiIndex >= 0 && apiIndex < (int)m_apiDefinitions.size() &&
			actionIndex >= 0 && actionIndex < (int)m_apiDefinitions[(size_t)apiIndex].actions.size()) {
			const ApiDefinitionSummary& definition = m_apiDefinitions[(size_t)apiIndex];
			const ApiActionSummary& action = definition.actions[(size_t)actionIndex];
			CString url = definition.baseUrl;
			url.TrimRight(_T('/'));
			CString path = action.path;
			if (!path.IsEmpty() && path[0] != _T('/')) path = _T("/") + path;
			info = action.method + _T("  ") + url + path;
		}
	}
	m_ApiInfo.SetWindowText(info);
}

void CInputBox::UpdateApiControls()
{
	BOOL visible = m_bApiEnabled && m_RadioApi.GetCheck() == BST_CHECKED;
	int command = visible ? SW_SHOW : SW_HIDE;
	m_ApiLabel.ShowWindow(command);
	m_ApiCombo.ShowWindow(command);
	m_ActionLabel.ShowWindow(command);
	m_ActionCombo.ShowWindow(command);
	m_ApiInfo.ShowWindow(command);
	CRect body(m_bodyNormalRect);
	if (visible) {
		CRect infoRect;
		m_ApiInfo.GetWindowRect(&infoRect);
		ScreenToClient(&infoRect);
		body.top = infoRect.bottom + 4;
	}
	m_Edit.MoveWindow(body);
}

void CInputBox::OnModeChanged()
{
	UpdateApiControls();
}

void CInputBox::OnApiSelectionChanged()
{
	m_ActionId.Empty();
	LoadActionChoices();
	UpdateApiInfo();
}

void CInputBox::OnActionSelectionChanged()
{
	UpdateApiInfo();
}

