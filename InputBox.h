// InputBox.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// CInputBox ダイアログ

class CInputBox : public CDialog
{
// コンストラクション
public:
	void SetWindowName( CString text );
	CInputBox(CWnd* pParent = NULL);   // 標準のコンストラクタ
	void GetInputText( CString& text );
	void SetInputText( CString& text );

// ダイアログ データ
	//{{AFX_DATA(CInputBox)
	enum { IDD = IDD_DIALOG1 };
	CEdit	m_Edit;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数を生成しオーバーライドします。
	//{{AFX_VIRTUAL(CInputBox)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:
	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CInputBox)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CString m_InputText;
	CString m_WindowName;
};
