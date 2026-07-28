// InputBox.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// CInputBox ダイアログ

class CInputBox : public CDialog
{
// コンストラクション
public:
	void SetWindowName( CString text );
	void SetViewOnly( BOOL bReadOnly );
	CInputBox(CWnd* pParent = NULL);   // 標準のコンストラクタ
	void GetInputText( CString& title, CString& text);
	void SetInputText( CString& title, CString& text);
	void GetMode(CString& mode) const;
	void SetMode(const CString& mode);
	void SetTemplateEnabled(BOOL enabled);

// ダイアログ データ
	//{{AFX_DATA(CInputBox)
	enum { IDD = IDD_DIALOG1 };
	CEdit	m_Edit;
	CEdit	m_ETitle;
	CButton	m_RadioPlain;
	CButton	m_RadioTemplate;
	CButton	m_RadioApi;
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
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_MSG
	void ApplyFontAndLayout();
	DECLARE_MESSAGE_MAP()
private:
	CString m_InputText;
	CString m_InputTitle;
	CString m_WindowName;
	CString m_Mode;
	CFont m_dialogFont;
	BOOL m_bViewOnly;
	BOOL m_bTemplateEnabled;
};
