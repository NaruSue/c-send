// csendDlg.h : ヘッダー ファイル
//
#include "CategoryDataList.h"
#include "DataValueList.h"
#include "ClipboardTipWnd.h"

/////////////////////////////////////////////////////////////////////////////
// CCsendDlg dialog

class CCsendDlg : public CDialog
{
// 構築
public:
	void SaveData( void );
	CCsendDlg(CWnd* pParent = NULL);	// 標準のコンストラクタ

	// 文字列エスケープ関連
	CString Escape(const CString& input);
	CString Unescape(const CString& input);
	CString GetValidFilePath(CString fileName);
	void CategoryUpdate();
	void UpdateList();
	void UpdateLayout();
	void LoadNotificationSettings();
	void ShowCopyFeedback(const CString& itemName);
	void ShowListStatus(const CString& message, BOOL isError);
	void RestoreMainWindow();
	void BuildTrayMenu(CMenu& menu);
	void BuildListContextMenu(CMenu& menu);

// Dialog Data
	//{{AFX_DATA(CCsendDlg)
	enum { IDD = IDD_CSEND_DIALOG };
	CListBox	m_CList;
	CComboBox m_CCombo;
	//}}AFX_DATA
	CEdit	m_StatusText;

	// ClassWizard は仮想関数を生成しオーバーライドします。
	//{{AFX_VIRTUAL(CCsendDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV のサポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:
	void ChangeMessage( void );
	void DeleteString(void);
	void ActivateListItem(int index);
	BOOL SendClipBoard( CString& text );
	BOOL ConfirmExit();
	HICON m_hIcon;
	CString m_appPath;
	CString m_iniPath;
	CString m_SavePath;

	int m_fontSize = 0;
	CString m_fontName;
	CFont m_listFont;

	CCategoryDataList m_categorys;
	CDataValueList m_dataList;

	void CategoryDlg();

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CCsendDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSelchangeClist();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDblclkClist();
	afx_msg void OnClose();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnDestroy();
	afx_msg LRESULT OnNotifyIconIvents( WPARAM wParam, LPARAM lParam );
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnAddstring();
	afx_msg void OnChange();
	afx_msg void OnDeletestring();
	afx_msg void OnAbout();
	afx_msg void OnExit();
	afx_msg void OnTrayItemSelect(UINT nID);
 afx_msg void OnCbnSelchangeComboCategory();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	enum {
		ID_TRAY_ITEM_BASE = 40000,
		ID_TRAY_ITEM_MAX = ID_TRAY_ITEM_BASE + 99
	};
	NOTIFYICONDATA m_stNtfyIcon;
	RECT rect;
	CString m_SelText;
	BOOL m_bCurrentCategoryIsUrl = FALSE;
	BOOL m_bCurrentCategoryIsReadOnly = FALSE;
	BOOL m_bToastEnabled = FALSE;
	UINT m_tipTimeoutMs = 3000;
	CClipboardTipWnd m_tipWnd;
	CString m_statusMessage;
	BOOL m_bStatusErrorMode = FALSE;
};





