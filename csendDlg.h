// csendDlg.h : �w�b�_�[ �t�@�C��
//

/////////////////////////////////////////////////////////////////////////////
// CCsendDlg dialog

class CCsendDlg : public CDialog
{
// �\�z
public:
	void SaveData( void );
	CCsendDlg(CWnd* pParent = NULL);	// �W���̃R���X�g���N�^

// Dialog Data
	//{{AFX_DATA(CCsendDlg)
	enum { IDD = IDD_CSEND_DIALOG };
	CListBox	m_CList;
	//}}AFX_DATA

	// ClassWizard �͉��z�֐��𐶐����I�[�o�[���C�h���܂��B
	//{{AFX_VIRTUAL(CCsendDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �̃T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:
	void ChangeMessage( void );
	void DeleteString(void);
	void SendClipBoard( CString& text );
	HICON m_hIcon;
	int m_fontSize = 0;
	CString m_fontName;
	CFont m_listFont;

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
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
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	NOTIFYICONDATA m_stNtfyIcon;
	RECT rect;
	CString m_SelText;
};

