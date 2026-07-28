// InputBox.h : ヘッダー ファイル
//
#include "ApiCatalog.h"

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
	void SetApiEnabled(BOOL enabled);
	void SetPasswordMode(BOOL enabled);
	void SetApiSelection(const CString& apiId, const CString& actionId);
	void GetApiSelection(CString& apiId, CString& actionId) const;

// ダイアログ データ
	//{{AFX_DATA(CInputBox)
	enum { IDD = IDD_DIALOG1 };
	CEdit	m_Edit;
	CEdit	m_ETitle;
	CButton	m_RadioPlain;
	CButton	m_RadioTemplate;
	CButton	m_RadioApi;
	CStatic m_ApiLabel;
	CStatic m_ActionLabel;
	CComboBox m_ApiCombo;
	CComboBox m_ActionCombo;
	CStatic m_ApiInfo;
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
	afx_msg void OnModeChanged();
	afx_msg void OnApiSelectionChanged();
	afx_msg void OnActionSelectionChanged();
	//}}AFX_MSG
	void ApplyFontAndLayout();
	void LoadApiChoices();
	void LoadActionChoices();
	void UpdateApiInfo();
	void UpdateApiControls();
	DECLARE_MESSAGE_MAP()
private:
	CString m_InputText;
	CString m_InputTitle;
	CString m_WindowName;
	CString m_Mode;
	CFont m_dialogFont;
	BOOL m_bViewOnly;
	BOOL m_bTemplateEnabled;
	BOOL m_bApiEnabled;
	BOOL m_bPasswordMode;
	CString m_ApiId;
	CString m_ActionId;
	CRect m_bodyNormalRect;
	std::vector<ApiDefinitionSummary> m_apiDefinitions;
};
