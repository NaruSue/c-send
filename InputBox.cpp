// InputBox.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "csend.h"
#include "InputBox.h"

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

BOOL CInputBox::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: この位置に初期化の補足処理を追加してください

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
