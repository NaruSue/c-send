// InputBox.h : �w�b�_�[ �t�@�C��
//

/////////////////////////////////////////////////////////////////////////////
// CInputBox �_�C�A���O

class CInputBox : public CDialog
{
// �R���X�g���N�V����
public:
	void SetWindowName( CString text );
	CInputBox(CWnd* pParent = NULL);   // �W���̃R���X�g���N�^
	void GetInputText( CString& text );
	void SetInputText( CString& text );

// �_�C�A���O �f�[�^
	//{{AFX_DATA(CInputBox)
	enum { IDD = IDD_DIALOG1 };
	CEdit	m_Edit;
	//}}AFX_DATA


// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��𐶐����I�[�o�[���C�h���܂��B
	//{{AFX_VIRTUAL(CInputBox)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:
	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(CInputBox)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CString m_InputText;
	CString m_WindowName;
};
