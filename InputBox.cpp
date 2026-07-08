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
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInputBox, CDialog)
	//{{AFX_MSG_MAP(CInputBox)
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
		const UINT ids[] = { IDC_EDIT1, IDC_EDIT2, IDOK, IDCANCEL };
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

	SetWindowText(m_WindowName);


	if( m_bViewOnly ){
		m_Edit.SetReadOnly(TRUE);
		m_ETitle.SetReadOnly(TRUE);
		CWnd* pOK = GetDlgItem(IDOK);
		if( pOK != NULL ){
			pOK->EnableWindow(FALSE);
		}
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

	CDialog::OnOK();
}

