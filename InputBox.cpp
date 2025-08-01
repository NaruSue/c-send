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
	//{{AFX_DATA_INIT(CInputBox)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
	//}}AFX_DATA_INIT
}


void CInputBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputBox)
	DDX_Control(pDX, IDC_EDIT1, m_Edit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInputBox, CDialog)
	//{{AFX_MSG_MAP(CInputBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputBox メッセージ ハンドラ

void CInputBox::SetInputText( CString& text )
{
	m_InputText = text;
}

void CInputBox::GetInputText( CString& text )
{
	text = m_InputText;
}

void CInputBox::SetWindowName( CString text )
{
	m_WindowName = text;
}

BOOL CInputBox::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: この位置に初期化の補足処理を追加してください

	SetWindowText(m_WindowName);

	if( m_InputText.GetLength() ){
		m_Edit.SetWindowText(m_InputText);
	}
	else{
		m_Edit.Paste();
	}
	return TRUE;  // コントロールにフォーカスを設定しないとき、戻り値は TRUE となります
	              // 例外: OCX プロパティ ページの戻り値は FALSE となります
}

void CInputBox::OnOK() 
{
	// TODO: この位置にその他の検証用のコードを追加してください
	m_Edit.GetWindowText(m_InputText);

	CDialog::OnOK();
}
