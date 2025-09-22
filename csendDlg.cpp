// csendDlg.cpp : �C���v�������e�[�V���� �t�@�C��
//

#include "stdafx.h"
#include "csend.h"
#include "csendDlg.h"

// Make -->
#include "InputBox.h"	// ������o�^�p�_�C�A���O�̃w�b�_�t�@�C��
// <--Make

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Make-->
#define WM_USER_NTFYICON	(WM_USER+531)	// �^�X�N�g���C�A�C�R������̃��b�Z�[�W�p
// <--Make


/////////////////////////////////////////////////////////////////////////////
// �A�v���P�[�V�����̃o�[�W�������Ŏg���Ă��� CAboutDlg �_�C�A���O

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �_�C�A���O �f�[�^
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard �͉��z�֐��𐶐����I�[�o�[���C�h���܂�
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �̃T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// ���b�Z�[�W �n���h��������܂���B
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCsendDlg �_�C�A���O

CCsendDlg::CCsendDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCsendDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCsendDlg)
	//}}AFX_DATA_INIT
	// ����: LoadIcon �� Win32 �� DestroyIcon �̃T�u�V�[�P���X��v�����܂���B
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCsendDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCsendDlg)
	DDX_Control(pDX, IDC_CLIST, m_CList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCsendDlg, CDialog)
	//{{AFX_MSG_MAP(CCsendDlg)
	ON_WM_SYSCOMMAND()		// �V�X�e�����j���[���̃��b�Z�[�W���������܂�
	ON_WM_PAINT()			// WM_PAINT �̓E�C���h�E�̕`�悪�K�v���ɌĂ΂�܂�
	ON_WM_QUERYDRAGICON()	// �قƂ�ǈӎ����Ȃ��Ă��ǂ��ł�
	ON_LBN_SELCHANGE(IDC_CLIST, OnSelchangeClist)	// ���X�g�{�b�N�X���I�����ꂽ�Ƃ��Ă΂�܂�
	ON_WM_SIZE()			// �E�C���h�E�T�C�Y���ύX���ꂽ�Ƃ��ɌĂ΂�܂�
	ON_LBN_DBLCLK(IDC_CLIST, OnDblclkClist)	// ���X�g�{�b�N�X�̍��ڂ��_�u���N���b�N���ꂽ�Ƃ��ɌĂ΂�܂�
	ON_WM_CLOSE()	// �E�C���h�E�����Ƃ��ɌĂ΂�܂�
	ON_WM_DESTROY()	// �E�C���h�E���j�������O�ɌĂ΂�܂�
	ON_MESSAGE( WM_USER_NTFYICON, OnNotifyIconIvents )	// �^�X�N�g���C�A�C�R������̃��b�Z�[�W���������܂�
	ON_WM_CONTEXTMENU()	// �E�C���h�E���ŉE�N���b�N�������ꂽ�Ƃ��ɌĂ΂�܂�
	ON_COMMAND(ID_ADDSTRING, OnAddstring)	// ON_COMMAND�̓��j���[�ɑΉ����܂�
	ON_COMMAND(ID_CHANGE, OnChange)
	ON_COMMAND(ID_DELETESTRING, OnDeletestring)
	ON_COMMAND(ID_ABOUT, OnAbout)
	ON_COMMAND(ID_EXIT, OnExit)
	ON_COMMAND(ID_HELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCsendDlg ���b�Z�[�W �n���h��

// �_�C�A���O�{�b�N�X���\�������O�ɌĂ΂�܂�
BOOL CCsendDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// IDM_ABOUTBOX �̓R�}���h ���j���[�͈̔͂łȂ���΂Ȃ�܂���B
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);	// �V�X�e�����j���[���擾���܂�
	CString strMenu;
// Make-->
	pSysMenu->AppendMenu(MF_SEPARATOR);	//�Z�p���[�^

	strMenu.LoadString(IDS_ADDSTRING);		// �o�^
		pSysMenu->AppendMenu(MF_STRING, ID_ADDSTRING, strMenu);
	strMenu.LoadString(IDS_CHANGESTRING);	// �ύX
		pSysMenu->AppendMenu(MF_STRING, ID_CHANGE, strMenu);
	strMenu.LoadString(IDS_DELETESTRING);	// �폜
		pSysMenu->AppendMenu(MF_STRING, ID_DELETESTRING, strMenu);

	pSysMenu->AppendMenu(MF_SEPARATOR);	// �Z�p���[�^

	strMenu.LoadString(IDS_ABOUTBOX);		// AboutBox
		pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strMenu);
// <--Make
	
	// ���̃_�C�A���O�p�̃A�C�R����ݒ肵�܂��B�t���[�����[�N�̓A�v���P�[�V�����̃��C��
	// �E�B���h�E���_�C�A���O�łȂ����͎����I�ɐݒ肵�܂���B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R����ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R����ݒ�
	
	// TODO: ���ʂȏ��������s�����͂��̏ꏊ�ɒǉ����Ă��������B
// Make-->
	
	// �t�@�C������E�C���h�E�̃T�C�Y�ƈʒu�A���X�g���擾���܂�
	CString l_tmp;	// �s��ǂݍ��ނ��߂̍�Ɨ̈�

	CString adds;	// ���X�g�̍Ō�́i�ǉ��j���b�Z�[�W�p�ł��B���̕����̓��\�[�X�ŕύX�ł��܂�
	adds.LoadString( IDS_LISTADD );	// �u(�ǉ�)�v

	CStdioFile savedata;	// �Z�[�u����Ă���f�[�^�t�@�C���������܂�
	CString csSaveFileName;	// �Z�[�u����Ă���t�@�C�������w�肵�܂��A���\�[�X�ŕύX�ł��܂�
	csSaveFileName.LoadString( IDS_SAVEFILE );	// ���\�[�X����t�@�C�������擾���܂�
	if( savedata.Open( csSaveFileName,
			CFile::modeRead | CFile::typeText ) ){	// �ǂݍ��݁A�e�L�X�g���[�h�ŊJ���܂�


		if (savedata.ReadString(l_tmp)) {


			// "15,MS UI Gothic" �̂悤�Ȍ`���𕪉�
			int commaPos = l_tmp.Find(_T(","));
			if (commaPos > 0) {
				m_fontSize = _ttoi(l_tmp.Left(commaPos));
				m_fontName = l_tmp.Mid(commaPos + 1);
			}
			

			if (m_fontSize > 0 && !m_fontName.IsEmpty()) {
				LOGFONT lf{};
				lf.lfHeight = -MulDiv(m_fontSize, GetDeviceCaps(::GetDC(NULL), LOGPIXELSY), 72);
				_tcsncpy_s(lf.lfFaceName, m_fontName, LF_FACESIZE - 1);

				m_listFont.DeleteObject();              // �����t�H���g������΍폜
				m_listFont.CreateFontIndirect(&lf);     // �����o�ϐ��ɍ쐬
				m_CList.SetFont(&m_listFont);           // �L���ȃt�H���g���֘A�t��
			}
		}

		if( savedata.ReadString( l_tmp ) ){	// ��s�ǂݍ���
			// ��s�ڂ̓T�C�Y�ƈʒu�������Ă��܂�
			sscanf( l_tmp, "%04X%04X%04X%04X", &rect.top, &rect.left,
								&rect.bottom, &rect.right );
			MoveWindow( &rect );	// �擾�������ɏ]���ăT�C�Y�ƈʒu��ύX���܂�
		}			
		while( savedata.ReadString( l_tmp ) ){	// �s���L��ԓǂݍ��݂܂�
			if( l_tmp != adds ){		// �i�ǉ��j�Ƃ��������Ŗ������
				m_CList.AddString(l_tmp);	// ���X�g�ɒǉ����܂�
			}
		}
		savedata.Close();		// �t�@�C������܂�
	}
	
	m_CList.InsertString(-1,adds);	// ���X�g�̍Ō�Ɂi�ǉ��j�������܂�

	// �c�[�����g���₷���悤�ɏ�Ɏ�O�ɕ\�����܂�
	SetWindowPos( &wndTopMost, 0,0,0,0, SWP_NOSIZE | SWP_NOMOVE );

	//////////////////////////////////	//�@�A�C�R�����^�X�N�g���C�ɕ\�����܂��B
	m_stNtfyIcon.cbSize = sizeof( NOTIFYICONDATA );	// �\���̂̃T�C�Y�ł��B
	m_stNtfyIcon.uID = 0;							// �A�C�R���̎��ʃi���o�[�ł��B
	m_stNtfyIcon.hWnd = m_hWnd;						// �C�x���g�Ɗ֘A��	����E�B���h�E�ł��B
	m_stNtfyIcon.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;			// �e��ݒ�ł��B
	m_stNtfyIcon.hIcon = AfxGetApp()->LoadIcon( IDR_MAINFRAME );	// �A�v���P�[�V�����̃A�C�R�����擾���āA������^�X�N�g���C�ɕ\�����܂��B
	m_stNtfyIcon.uCallbackMessage = WM_USER_NTFYICON;	// �A�C�R�����N���b�N���ꂽ�Ƃ��ɑ���o�����b�Z�[�W�ł�
	lstrcpy( m_stNtfyIcon.szTip, _T( "C-Send" ) );		// �`�b�v�̕�����ł��B
	::Shell_NotifyIcon( NIM_ADD, &m_stNtfyIcon );		// �^�X�N�g���C�ɕ\�����܂��B

// <--Make

	return TRUE;  // TRUE ��Ԃ��ƃR���g���[���ɐݒ肵���t�H�[�J�X�͎����܂���B
}

// �����ł̓V�X�e���R�}���h�ɑΉ�����ׂ̏������s���܂�
void CCsendDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
// Make-->
	if( nID == ID_ADDSTRING ){		// �u�o�^�v���I�����ꂽ�ꍇ
		int i = m_CList.GetCount();	// ���݂̃��X�g�̐����擾
		m_CList.SetCurSel( i-1 );	// ���X�g�̍Ō��I��
		ChangeMessage();			// ������ǉ��^�ύX�֐����Ăт܂�
	}
	else if( nID == ID_CHANGE ){	// �u�ύX�v���I�����ꂽ��
		int j = m_CList.GetCurSel();// ���݂̑I���𒲂ׂ܂�
		if( j == LB_ERR ){			// �����I������Ă��Ȃ��ꍇ�͏I�����܂�
			return;
		}
		ChangeMessage();			// ������ǉ��^�ύX�֐����Ăт܂�
	}
	else if( nID == ID_HELP ){		// �u�w���v�v
		AfxGetApp()->WinHelp(0,HELP_CONTENTS);	// �w���v���Ăяo���܂� 
		return;
	}
	else if( nID == ID_DELETESTRING ){	// �폜
		DeleteString();					// �폜�֐����Ăяo���܂�
	}
	else
// <--Make
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)	// �o�[�W�������(AboutBox)
	{
		CAboutDlg dlgAbout;		// �_�C�A���O���쐬����
		dlgAbout.DoModal();		// �\�����܂�
	}
	else
	{	// ���̑��f�t�H���g�̏������s���܂�
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����_�C�A���O�{�b�N�X�ɍŏ����{�^����ǉ�����Ȃ�΁A�A�C�R����`�悷��
// �R�[�h���ȉ��ɋL�q����K�v������܂��BMFC �A�v���P�[�V������ document/view
// ���f�����g���Ă���̂ŁA���̏����̓t���[�����[�N�ɂ�莩���I�ɏ�������܂��B

// �E�C���h�E���`�悳���Ƃ��ɌĂ΂�܂�
void CCsendDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �`��p�̃f�o�C�X �R���e�L�X�g

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// �N���C�A���g�̋�`�̈���̒���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �A�C�R����`�悵�܂��B
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// �V�X�e���́A���[�U�[���ŏ����E�B���h�E���h���b�O���Ă���ԁA
// �J�[�\����\�����邽�߂ɂ������Ăяo���܂��B
HCURSOR CCsendDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

// ���X�g��I�������Ƃ��ɌĂ΂�܂�
void CCsendDlg::OnSelchangeClist() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
// Make-->
	CString text;	// ���X�g������擾�p
	CString adds;	// �i�ǉ��j�p

	int i=m_CList.GetCurSel();	// ���ݑI������Ă��郊�X�g
	m_CList.GetText( i, text );	// ��������擾����
	
	adds.LoadString( IDS_LISTADD );	// �i�ǉ��j���擾����
	if( text != adds ){			// �I������Ă��郊�X�g���i�ǉ��j�łȂ����
		SendClipBoard( text );	// �N���b�v�{�[�h�ɕ������]�����܂�
	}
	else{		// �I������Ă��郊�X�g���i�ǉ��j�Ȃ��text�����������܂�
		text.LoadString( IDS_TITLE );
	}
	SetWindowText( text );	// ���݂�text�̓��e���L���v�V�����ɕ\�����܂�

	// Ctrl�L�[��������Ă�����ŏ���
	if ((GetKeyState(VK_CONTROL) & 0x8000) != 0) {
		ShowWindow(SW_MINIMIZE);
	}
// <--Make
}

// �_�C�A���O�̃T�C�Y���ύX�����Ƃ��ɌĂ΂�܂�
void CCsendDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: ���̈ʒu�Ƀ��b�Z�[�W �n���h���p�̃R�[�h��ǉ����Ă�������
// Make-->
	if( IsIconic() ){	// �A�C�R�������ꂽ��
		ShowWindow( SW_HIDE );	// �E�C���h�E���\���ɂ��܂�
		return;
	}
	
	if( !IsZoomed() ){	// �ő剻�łȂ����
		GetWindowRect( &rect );		// ���݂̃E�C���h�E�̃T�C�Y���擾���Ă����܂�
	}

	m_CList.MoveWindow( 0, 0, cx, cy);	// ���X�g�̃T�C�Y���_�C�A���O�̃N���C�A���g�T�C�Y�ɕύX���܂�
// <--Make
}

// �N���b�v�{�[�h�ɓ]������֐��ł�
// Make-->
void CCsendDlg::SendClipBoard( CString& text )
{
	CString csMessage;	// �G���[���b�Z�[�W�\���p
	if( !OpenClipboard() ){	// �N���b�v�{�[�h�̃I�[�v��
		// ���s�����ꍇ�̓��b�Z�[�W��\�����ď������I�����܂�
		csMessage.LoadString( IDS_CBD_NOOPEN );
		AfxMessageBox( csMessage );
		return;
	}
	if( !EmptyClipboard() ){	// �N���b�v�{�[�h�̓��e�����������܂�
		// ���s�����ꍇ�̓��b�Z�[�W��\�����ď������I�����܂�
		csMessage.LoadString( IDS_CBD_NOEMPTY );
		AfxMessageBox( csMessage );
		CloseClipboard();
		return;
	}

	char* buff;	// �N���b�v�{�[�h�֓]�����邽�߂̗̈�����܂�
	// GlobalAlloc���g���AGMEM_DDESHARE�̑����ō쐬���܂�
	// �����ō�����������̈�͂��Ƃŉ������K�v�͂���܂���
	buff = (char*) GlobalAlloc( GMEM_DDESHARE, text.GetLength()+1 );

	lstrcpy( buff, (LPCTSTR)text );	// �쐬�����̈�ɕ������]�����܂�

	if ( ::SetClipboardData( CF_TEXT, (HANDLE)buff ) == NULL ){ // �N���b�v�{�[�h�ɓ]�����܂�
		// ���s�����ꍇ�̓��b�Z�[�W��\�����ď������I�����܂�
		csMessage.LoadString( IDS_CBD_NOSEND );
		AfxMessageBox( csMessage );
	}
	CloseClipboard();	// �N���b�v�{�[�h����܂�
}
// <--Make

// ���X�g���_�u���N���b�N���ꂽ�Ƃ��ɌĂ΂�܂�
void CCsendDlg::OnDblclkClist() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������

// Make-->
	ChangeMessage();
// <--Make
}

// �E�C���h�E�����Ƃ��ɌĂ΂�܂�
void CCsendDlg::OnClose() 
{
	// TODO: ���̈ʒu�Ƀ��b�Z�[�W �n���h���p�̃R�[�h��ǉ����邩�܂��̓f�t�H���g�̏������Ăяo���Ă�������
// Make-->
	CDialog::OnOK();	// �E�C���h�E����܂�
// <--Make
}

// OK�{�^���������ꂽ�Ƃ��ɌĂ΂�܂�
void CCsendDlg::OnOK() 
{
	// TODO: ���̈ʒu�ɂ��̑��̌��ؗp�̃R�[�h��ǉ����Ă�������
// Make-->
	return;	// ����� OK�{�^���̏����͉������Ȃ��̂ŏ����������ɔ����܂�
// <--Make

	CDialog::OnOK();
}

// �L�����Z���{�^���������ꂽ�Ƃ��ɌĂ΂�܂��i�ꍇ�ɂ���Ă�ESC�������ꂽ�Ƃ����Ă΂ꂽ�肵�܂��j
void CCsendDlg::OnCancel() 
{
	// TODO: ���̈ʒu�ɓ��ʂȌ㏈����ǉ����Ă��������B
// Make-->
	return;	// �����ESC�ŏ����Ȃ��悤�ɏ����������ɔ����܂�
// <--Make
	CDialog::OnCancel();
}

void CCsendDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: ���̈ʒu�Ƀ��b�Z�[�W �n���h���p�̃R�[�h��ǉ����Ă�������
// Make-->

	::Shell_NotifyIcon( NIM_DELETE, &m_stNtfyIcon );	//�^�X�N�g���C�̃A�C�R�����폜���܂��B
	AfxGetApp()->WinHelp(0,HELP_QUIT);	// �w���v����܂�

// ver 1.1-->
	SaveData();	// ���X�g��ۑ����܂��B
// -->ver 1.1

// <--Make
}

// �^�X�N�g���C�̃A�C�R�����N���b�N���ꂽ�Ƃ��ɌĂ΂�܂�
// Make-->
LRESULT CCsendDlg::OnNotifyIconIvents( WPARAM wParam, LPARAM lParam ){

	switch( lParam ){
	case WM_LBUTTONDOWN:	// ���N���b�N
		int st;	// �E�C���h�E�̃��[�h���擾
	
		if( IsWindowVisible() ){	// �E�C���h�E���\������Ă�����
			st = SW_HIDE;
		}
		else{	// �E�C���h�E����\���Ȃ�
			st = SW_SHOW;	// �E�C���h�E��\�����Č��̑傫���ɖ߂��܂�
			ShowWindow( SW_RESTORE );
			// �c�[�����g���₷���悤�ɏ�Ɏ�O�ɕ\�����܂�
			SetWindowPos( &wndTopMost, 0,0,0,0, SWP_NOSIZE | SWP_NOMOVE );
		}

		ShowWindow( st );	// �E�C���h�E�̕\���^��\����؂�ւ��܂�

		break;
	case WM_RBUTTONDOWN:	//�E�N���b�N
// Make 1.1-->
		POINT point;
		GetCursorPos( &point );	// ���݂̃J�[�\���̈ʒu���擾

		SetForegroundWindow();	//�E�B���h�E���t�H�A�O���E���h�Ɏ����Ă��܂��B
		SetFocus();	//��������Ȃ��ƁA���j���[�������Ȃ��Ȃ�܂��B

		CMenu cMenu;	// ���j���[
		if( !cMenu.CreatePopupMenu() ){	// �|�b�v�A�b�v���j���[���쐬���܂�
			break;
		}

		CString strMenu;	// ���j���[�ɕ\�����镶����p
		strMenu.LoadString(IDS_ABOUTBOX);	// ���j���[�Ɂu�o�[�W�������v��\�����܂�
		cMenu.AppendMenu( MF_STRING, ID_ABOUT, strMenu); 
		strMenu.LoadString(IDS_EXIT);	// ���j���[�Ɂu�I���v��\�����܂�
		cMenu.AppendMenu( MF_STRING, ID_EXIT, strMenu); 

		cMenu.TrackPopupMenu( TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this );	//�|�b�v�A�b�v���j���[��\�����܂��B

		PostMessage( WM_NULL );	//��������Ȃ��ƁA�Q�x�ڂ̃��j���[�������������Ⴂ�܂��B
// -->Make 1.1
		break;
	}
	
	return 0;
}

// �_�C�A���O�̒��ŉE�N���b�N�����Ƃ��ɌĂ΂�܂�
// ������ContextMenu�i�|�b�v�A�b�v�̃��j���[�j�̏������s���܂��傤
void CCsendDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: ���̈ʒu�Ƀ��b�Z�[�W �n���h���p�̃R�[�h��ǉ����Ă�������

/*
	���
	  CWnd* pWnd �͉E�N���b�N���ꂽ�Ƃ��̃}�E�X�J�[�\���̂���E�C���h�E�ł��B
		�_�C�A���O�{�b�N�X�̏ꍇ���ɔz�u����Ă���R���g���[�������ʂ���̂�
		�g���܂��傤�B
	  CPoint point �̓}�E�X�J�[�\���̈ʒu�ł��B
		Screen���W�ł��B
		������g���ă��X�g�̍��ڂ�I��������悤�ȏꍇ�͍��W�ϊ����s���K�v��
		����܂��B
*/	
// Make-->
	// ���݂̃}�E�X�J�[�\�������X�g�ɂ��邩�ǂ������`�F�b�N���Ă��܂�
	if( pWnd != (CWnd*)GetDlgItem( IDC_CLIST ) ){
		return;
	}

	BOOL tmp;	// ItemFromPoint�ׂ̈̍�Ɨp�ϐ��ł��i����͖��g�p�j
	m_CList.ScreenToClient( &point );	// �E�N���b�N�Ń��X�g�̑I�����s�����߁A���W��ϊ����܂�
	int nSelect = m_CList.ItemFromPoint( point, tmp );	// �}�E�X�J�[�\���̈ʒu�̃��X�g��ԍ���Ԃ��܂�
	m_CList.SetCurSel( nSelect );	// ���X�g��I�����܂�

	CMenu cMenu;	// ���j���[
	if( !cMenu.CreatePopupMenu() ){	// �|�b�v�A�b�v���j���[���쐬���܂�
		return;
	}

	CString text;	// ������擾�p
	int i=m_CList.GetCurSel();	// ���݂̑I������Ă��郊�X�g���擾���܂�
	m_CList.GetText( i, text );	// ���݂̕�������擾���܂�

	CString l_tmp;	// �i�ǉ��j�p
	l_tmp.LoadString( IDS_LISTADD );

	CString strMenu;	// ���j���[�ɕ\�����镶����p
	if( l_tmp == text ){	// ���݁i�ǉ��j���I�΂�Ă���Ƃ�
		strMenu.LoadString(IDS_ADDSTRING);	// ���j���[�ɂ́u�o�^�v��\�����܂�
		cMenu.AppendMenu( MF_STRING, ID_ADDSTRING, strMenu); 
	}
	else{	// �i�ǉ��j�ȊO���I�΂�Ă���Ƃ�
		strMenu.LoadString(IDS_CHANGESTRING);	// �ύX
		cMenu.AppendMenu( MF_STRING, ID_CHANGE, strMenu ); 
		strMenu.LoadString(IDS_DELETESTRING);	// �폜
		cMenu.AppendMenu( MF_STRING, ID_DELETESTRING, strMenu ); 
	}

	// �N���C�A���g���W����X�N���[�����W�ɖ߂��܂�
	m_CList.ClientToScreen( &point );
	// �|�b�v�A�b�v���j���[�̕\���̓X�N���[�����W�Ŏw�肷��K�v������܂�
	cMenu.TrackPopupMenu( TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this );	//�|�b�v�A�b�v���j���[��\�����܂��B

// <--Make
}

// ��������폜���܂�
// Make-->
void CCsendDlg::DeleteString()
{
	int i = m_CList.GetCount();	// ���݂̃��X�g�̐��𐔂��܂�
	if( i<1 ){	// ���X�g�Ɉ���Ȃ���Ή������܂���
		return;
	}

	int j = m_CList.GetCurSel();	// ���ݑI������Ă��鍀�ڂ𒲂ׂ܂�
	if( j == LB_ERR ){	// �����I������Ă��Ȃ���Ή������܂���
		return;
	}

	if( j == i-1 ){	// �I������Ă��鍀�ڂ��Ō�i�ǉ��j�ł̏ꍇ
		CString csMessage;
		csMessage.LoadString( IDS_NODELETE ); // �G���[���b�Z�[�W���o���܂�
		AfxMessageBox( csMessage );
		return;
	}

	CString cst;	// ���݂̕�������擾
	CString output;	// �m�F���b�Z�[�W�p��

	m_CList.GetText( j, cst );	// ���ݑI������Ă��镶������擾���܂�
	output.LoadString( IDS_INFOMESSAGE );	// �₢���킹�p�̃��b�Z�[�W�����\�[�X����擾���܂�
	// ���e
	// �@�uXXXXXXXXXXX�v
	//�̂悤�ɂ��܂�
	output += "\x0d\x0a�u";	// ���b�Z�[�W�����H���܂�
	output += cst;
	output += "�v";

	CString csMessage;	// �폜�����邱�Ƃ𑣂����b�Z�[�W
	csMessage.LoadString( IDS_MESSAGE_DEL );
	if( MessageBox( output, csMessage,	// �m�F�_�C�A���O��\�����܂�
				MB_ICONQUESTION | MB_OKCANCEL | MB_DEFBUTTON2 )	// ���̂Ƃ��f�t�H���g�̃{�^�����L�����Z���ɂ��܂�
		== IDCANCEL ){
		return;	// �L�����Z�����I�����ꂽ�ꍇ���������ɏI�����܂�
	}

	m_CList.DeleteString(j);	// ���X�g���當������폜���܂�
}
// <--Make

// �o�^���j���[���I�����ꎞ�ɌĂ΂�܂�
void CCsendDlg::OnAddstring() 
{
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������
// Make-->
	ChangeMessage();
// <--Make
}

// �ύX���j���[���I�����ꂽ�Ƃ��ɌĂ΂�܂�
void CCsendDlg::OnChange() 
{
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������
// Make-->
	ChangeMessage();
// <--Make
}

// �폜���j���[���I�����ꂽ�Ƃ��ɌĂ΂�܂�
void CCsendDlg::OnDeletestring() 
{
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������
// Make-->
	DeleteString();
// <--Make
}

// ���b�Z�[�W�̕ύX�Ɠo�^
// Make-->
void CCsendDlg::ChangeMessage()
{
	CInputBox cInput;	// ���b�Z�[�W�ҏW�p�̃_�C�A���O
	CString text;		// ���X�g�̕�����
	
	// ���ݑI������Ă��镶������擾���܂�
	int i=m_CList.GetCurSel();
	m_CList.GetText( i, text );

	// �i�ǉ��j�����\�[�X����擾���܂�
	CString l_tmp;
	l_tmp.LoadString( IDS_LISTADD );


	BOOL flag = FALSE;	// ���̃t���O�ŁA�o�^�ƕύX����ʂ��܂��BTRUE--�o�^  FALSE--�ύX
	CString InputWindowName;	// ���b�Z�[�W�ҏW�p�̃_�C�A���O�̃L���v�V�����p
	if( text == l_tmp ){	// ���݂̕����񂪁i�ǉ��j�̎�
		InputWindowName.LoadString( IDS_REGIST );	// �L���v�V�����́u�o�^�v��I�т܂�
		flag = TRUE;	// �t���O���u�o�^�v(TRUE)�ɂ��܂�
	}
	else{
		cInput.SetInputText( text );	// ���b�Z�[�W�ҏW�p�̃_�C�A���O�Ɍ��ݑI������Ă��镶�����ݒ肵�܂�
		InputWindowName.LoadString( IDS_CHANGE );	// �L���v�V�����́u�ύX�v��I�т܂�
	}
	cInput.SetWindowName( InputWindowName );	// �L���v�V������ݒ肵�܂�

	if( cInput.DoModal()==IDOK ){	// �_�C�A���O��\�����܂�
		// OK�{�^����������ďI�������ꍇ�ȉ��̏������s���܂�
		CString text;	// ���b�Z�[�W�ҏW�_�C�A���O����̕�����擾�p
		cInput.GetInputText(text);	// ���b�Z�[�W�ҏW�p�_�C�A���O���當������擾���܂�
		if( !flag ){	// �t���O���u�ύX�v(FALSE)�Ȃ�A���݂̃��X�g�̕�������폜���܂�
			m_CList.DeleteString(i);
		}
		if( text.GetLength() ){	// �擾���������񂪂���ꍇ
			m_CList.InsertString( i, (LPCTSTR) text );	// ���X�g�ɕ������ǉ����܂�
		}
	}
// Make 1.1-->
	SaveData();	// ���X�g��ۑ����܂�
// -->Make 1.1

}
// <--Make

// �^�X�N�g���C�Łu�ް�ޮݏ��v���I�����ꂽ�Ƃ��ɌĂ΂�܂�
void CCsendDlg::OnAbout() 
{
// Make 1.1-->
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������
	CAboutDlg dlgAbout;		// �_�C�A���O���쐬����
	dlgAbout.DoModal();		// �\�����܂�
// <--Make 1.1
}

// �^�X�N�g���C�Łu�I���v���I�����ꂽ�Ƃ��ɌĂ΂�܂�
void CCsendDlg::OnExit() 
{
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������
// Make 1.1-->
	CDialog::OnOK();	// �E�C���h�E����܂�
// <--Make 1.1
}

void CCsendDlg::OnHelp() 
{
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������
// Make 1.1-->
	AfxGetApp()->WinHelp(0,HELP_CONTENTS);	// �w���v���Ăяo���܂� 
// <--Make 1.1
}


// ���X�g�f�[�^�̕ۑ�
// Make 1.1-->
void CCsendDlg::SaveData()
{
	//////////////////���X�g��ۑ����܂�//////////////////
	CString l_tmp;	// ��Ɨp

	int j = m_CList.GetCount();	// ���X�g�̐����擾���܂�

	CStdioFile savedata;	// �ۑ��p�t�@�C��
	CString csSaveFileName;	// �ۑ��p�t�@�C����
	csSaveFileName.LoadString( IDS_SAVEFILE );	// �ۑ��p�t�@�C�������\�[�X����擾���܂�
	if( savedata.Open( csSaveFileName,	// �e�L�X�g���[�h�A���C�g�����ŁA�����ꍇ�͐V�K�쐬���܂�
		CFile::modeCreate | CFile::modeWrite | CFile::typeText ) ){

		int fontSize = 16;              // �f�t�H���g�l
		CString fontName = _T("MS UI Gothic");

		if (m_fontSize != 0) {
			fontSize = m_fontSize;
			fontName = m_fontName;
		}

		// --- 1�s��: �t�H���g��� ---
		CString fontLine;
		fontLine.Format(_T("%d,%s\n"), fontSize, (LPCTSTR)fontName);
		savedata.WriteString(fontLine);


		char buff[17];	// 2�s�ڗp
		if( !IsIconic() && !IsZoomed() ){	// �E�C���h�E���A�C�R������ő剻����Ă��Ȃ����
			GetWindowRect( &rect );	// �E�C���h�E�T�C�Y���擾���܂�
		}
		// �P�s�ڂ̓T�C�Y�ƈʒu���������݂܂�
		wsprintf( buff, "%04X%04X%04X%04X\n", rect.top, rect.left,
						rect.bottom, rect.right );
		savedata.WriteString(buff);

		for(int i=0; i<j; i++){	// ���X�g���P�Â�荞��łP�s���������݂܂�
			m_CList.GetText( i, l_tmp );
			l_tmp += "\n";
			savedata.WriteString(l_tmp);
		}
		savedata.Close();	// �t�@�C������܂�
	}
}
// <--Make 1.1
