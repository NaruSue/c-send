// csendDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "csend.h"
#include "csendDlg.h"
#include "version.h"

// Make -->
#include "InputBox.h"	// 文字列登録用ダイアログのヘッダファイル
#include "categoryDlg.h"
#include "ToastNotificationHelper.h"
#include "IniTextUtil.h"
#include <wininet.h>

#pragma comment(lib, "wininet.lib")

static bool IsHttpUrl(const CString& path)
{
	return path.Left(7).CompareNoCase(_T("http://")) == 0 ||
		path.Left(8).CompareNoCase(_T("https://")) == 0;
}

static bool IsReadOnlyFilePath(const CString& path)
{
	DWORD attr = GetFileAttributes(path);
	if (attr == INVALID_FILE_ATTRIBUTES) {
		return false;
    }
	return (attr & FILE_ATTRIBUTE_READONLY) != 0;
}

static bool IsSettingOn(const CString& value)
{
	CString v(value);
	v.Trim();
	return v.CompareNoCase(_T("on")) == 0 || v.CompareNoCase(_T("1")) == 0 ||
		v.CompareNoCase(_T("true")) == 0 || v.CompareNoCase(_T("yes")) == 0;
}


static bool DownloadUrlToTempFile(const CString& url, CString& tempPath)
{
	TCHAR tempDir[MAX_PATH] = { 0 };
	if (GetTempPath(MAX_PATH, tempDir) == 0) {
		return false;
    }

	TCHAR tempFile[MAX_PATH] = { 0 };
	if (GetTempFileName(tempDir, _T("csd"), 0, tempFile) == 0) {
		return false;
    }

	CFile file;
	if (!file.Open(tempFile, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary)) {
		DeleteFile(tempFile);
		return false;
    }

	HINTERNET hSession = InternetOpen(_T("C-Send"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (hSession == NULL) {
		file.Close();
		DeleteFile(tempFile);
		return false;
    }

	HINTERNET hFile = InternetOpenUrl(
		hSession,
		url,
		NULL,
		0,
		INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_PRAGMA_NOCACHE,
		0);

	if (hFile == NULL) {
		InternetCloseHandle(hSession);
		file.Close();
		DeleteFile(tempFile);
		return false;
    }

	char buffer[4096];
	DWORD bytesRead = 0;
	BOOL ok = TRUE;
	ULONGLONG totalBytes = 0;
	while ((ok = InternetReadFile(hFile, buffer, sizeof(buffer), &bytesRead)) && bytesRead > 0) {
		if (totalBytes + bytesRead > CDataValueList::MAX_FILE_BYTES) {
			ok = FALSE;
			break;
	    }
		file.Write(buffer, bytesRead);
		totalBytes += bytesRead;
    }

	InternetCloseHandle(hFile);
	InternetCloseHandle(hSession);
	file.Close();

	if (!ok) {
		DeleteFile(tempFile);
		return false;
    }

	tempPath = tempFile;
	return true;
}

static CString MakeWindowTitle(const CString& base, BOOL isReadOnly)
{
	CString title = base;
	if (title.IsEmpty()) {
		title = _T("C-Send");
    }
	if (isReadOnly) {
		title += _T(" [RO]");
    }
	return title;
}
// <--Make
static CString MakeCategoryLabel(const CString& name, const CString& path)
{
    CString label = name;
    if (IsHttpUrl(path)) {
        label += _T(" [URL]");
    }
    else if (IsReadOnlyFilePath(path)) {
        label += _T(" [RO]");
    }
    return label;
}

static CString MakeListStatusMessage(const CString& categoryName, const CString& pathText, const CString& error)
{
    CString msg;
    if (!categoryName.IsEmpty()) {
        msg += categoryName;
    }
    if (!pathText.IsEmpty()) {
        if (!msg.IsEmpty()) {
            msg += _T("\r\n");
        }
        msg += pathText;
    }
    if (!error.IsEmpty()) {
        if (!msg.IsEmpty()) {
            msg += _T("\r\n");
        }
        msg += error;
    }
    return msg;
}
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Make-->
#define WM_USER_NTFYICON	(WM_USER+531)	// タスクトレイアイコンからのメッセージ用
#define WM_USER_RESTORE_MAINWINDOW	(WM_APP+531)	// 単一起動時の再表示要求メッセージ用
// <--Make


/////////////////////////////////////////////////////////////////////////////
// アプリケーションのバージョン情報で使われている CAboutDlg ダイアログ

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
    virtual BOOL OnInitDialog();

// ダイアログ データ
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard は仮想関数を生成しオーバーライドします
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV のサポート
	//}}AFX_VIRTUAL

// インプリメンテーション
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
		// メッセージ ハンドラがありません。
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCsendDlg ダイアログ

CCsendDlg::CCsendDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCsendDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCsendDlg)
	//}}AFX_DATA_INIT
	// メモ: LoadIcon は Win32 の DestroyIcon のサブシーケンスを要求しません。
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCsendDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCsendDlg)
	DDX_Control(pDX, IDC_CLIST, m_CList);
	DDX_Control(pDX, IDC_COMBO_CATEGORY, m_CCombo);
	//}}AFX_DATA_MAP
}

void CCsendDlg::UpdateLayout()
{
    if (!::IsWindow(m_CList.m_hWnd) || !::IsWindow(m_CCombo.m_hWnd)) {
        return;
    }

    CRect rcClient;
    GetClientRect(&rcClient);

    CFont* pFont = m_CCombo.GetFont();
    if (pFont == NULL) {
        pFont = m_CList.GetFont();
    }
    CClientDC dc(this);
    CFont* pOldFont = NULL;
    if (pFont != NULL) {
        pOldFont = dc.SelectObject(pFont);
    }

    TEXTMETRIC tm{};
    dc.GetTextMetrics(&tm);

    if (pOldFont != NULL) {
        dc.SelectObject(pOldFont);
    }

    int comboHeight = tm.tmHeight + tm.tmExternalLeading + 8;
    if (comboHeight < 20) {
        comboHeight = 20;
    }

    int gap = comboHeight / 8;
    if (gap < 2) {
        gap = 2;
    }

    int listTop = comboHeight + gap;
    int listHeight = rcClient.Height() - listTop;
    if (listHeight < 0) {
        listHeight = 0;
    }

    m_CCombo.MoveWindow(0, 0, rcClient.Width(), comboHeight);
    m_CList.MoveWindow(0, listTop, rcClient.Width(), listHeight);
    if (::IsWindow(m_StatusText.m_hWnd)) {
        CRect rcStatus = rcClient;
        rcStatus.top = listTop;
        if (rcStatus.Height() > 0) {
            rcStatus.DeflateRect(8, 8);
        }
        CString statusText;
        m_StatusText.GetWindowText(statusText);

        CClientDC dc(this);
        CFont* pOldFont = NULL;
        if (m_StatusText.GetFont() != NULL) {
            pOldFont = dc.SelectObject(m_StatusText.GetFont());
        }

        TEXTMETRIC tm{};
        dc.GetTextMetrics(&tm);

        int maxWidth = 0;
        int lineCount = 0;
        int start = 0;
        while (start <= statusText.GetLength()) {
            int end = statusText.Find(_T("\n"), start);
            CString line = (end >= 0) ? statusText.Mid(start, end - start) : statusText.Mid(start);
            line.TrimRight(_T("\r"));
            CSize sz = dc.GetTextExtent(line);
            if (sz.cx > maxWidth) {
                maxWidth = sz.cx;
            }
            ++lineCount;
            if (end < 0) {
                break;
            }
            start = end + 1;
        }
        if (lineCount < 1) {
            lineCount = 1;
        }

        int textHeight = lineCount * (tm.tmHeight + tm.tmExternalLeading);
        if (textHeight < tm.tmHeight) {
            textHeight = tm.tmHeight;
        }

        m_StatusText.MoveWindow(&rcStatus);

        CRect rcText;
        m_StatusText.GetClientRect(&rcText);
        int top = max(0, (rcText.Height() - textHeight) / 2);
        rcText.DeflateRect(0, top, 0, 0);
        m_StatusText.SendMessage(EM_SETRECT, 0, (LPARAM)(LPRECT)&rcText);

        if (pOldFont != NULL) {
            dc.SelectObject(pOldFont);
        }
    }
}


void CCsendDlg::LoadNotificationSettings()
{
	TCHAR buf[64] = { 0 };
	GetPrivateProfileString(_T("notification"), _T("toast"), _T("off"), buf, 64, m_iniPath);
	m_bToastEnabled = IsSettingOn(buf);

	m_tipTimeoutMs = GetPrivateProfileInt(_T("notification"), _T("tip_ms"), 3000, m_iniPath);
	if (m_tipTimeoutMs == 0) {
		m_tipTimeoutMs = 3000;
    }
}

void CCsendDlg::ShowCopyFeedback(const CString& itemName)
{
	CString tipText = itemName;
	if (!tipText.IsEmpty()) {
		tipText += _T("\r\n");
    }
	tipText += _T("Copied to clipboard");

	CPoint pt;
	GetCursorPos(&pt);
	m_tipWnd.ShowTip(tipText, m_tipTimeoutMs, pt);

	if (m_bToastEnabled) {
		ShowClipboardToast(itemName);
    }
}
void CCsendDlg::ShowListStatus(const CString& message, BOOL isError)
{
	m_statusMessage = message;
	m_bStatusErrorMode = isError;

	if (::IsWindow(m_StatusText.m_hWnd)) {
		m_StatusText.SetWindowText(message);
		m_StatusText.ShowWindow(isError ? SW_SHOW : SW_HIDE);
        UpdateLayout();
    }

	if (::IsWindow(m_CList.m_hWnd)) {
		m_CList.ShowWindow(isError ? SW_HIDE : SW_SHOW);
    }
}

BEGIN_MESSAGE_MAP(CCsendDlg, CDialog)
	//{{AFX_MSG_MAP(CCsendDlg)
	ON_WM_SYSCOMMAND()		// システムメニュー等のメッセージを処理します
	ON_WM_PAINT()			// WM_PAINT はウインドウの描画が必要時に呼ばれます
	ON_WM_QUERYDRAGICON()	// ほとんど意識しなくても良いです
	ON_LBN_SELCHANGE(IDC_CLIST, OnSelchangeClist)	// リストボックスが選択されたとき呼ばれます
	ON_WM_SIZE()			// ウインドウサイズが変更されたときに呼ばれます
	ON_LBN_DBLCLK(IDC_CLIST, OnDblclkClist)	// リストボックスの項目がダブルクリックされたときに呼ばれます
	ON_WM_CLOSE()	// ウインドウを閉じるときに呼ばれます
	ON_WM_DESTROY()	// ウインドウが破棄される前に呼ばれます
	ON_MESSAGE( WM_USER_NTFYICON, OnNotifyIconIvents )	// タスクトレイアイコンからのメッセージを処理します
	ON_WM_CONTEXTMENU()	// ウインドウ内で右クリックが押されたときに呼ばれます
	ON_COMMAND(ID_ADDSTRING, OnAddstring)	// ON_COMMANDはメニューに対応します
	ON_COMMAND(ID_CHANGE, OnChange)
	ON_COMMAND(ID_DELETESTRING, OnDeletestring)
	ON_COMMAND(ID_ABOUT, OnAbout)
	ON_COMMAND(ID_EXIT, OnExit)
	ON_COMMAND_RANGE(ID_TRAY_ITEM_BASE, ID_TRAY_ITEM_MAX, OnTrayItemSelect)
 ON_CBN_SELCHANGE(IDC_COMBO_CATEGORY, &CCsendDlg::OnCbnSelchangeComboCategory)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString versionText;
	versionText.Format(_T("C-Send Version %s"), CSEND_VERSION_TEXT);
	SetDlgItemText(IDC_ABOUT_VERSION, versionText);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCsendDlg メッセージ ハンドラ

// ダイアログボックスが表示される前に呼ばれます
BOOL CCsendDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CCsendApp* pApp = static_cast<CCsendApp*>(AfxGetApp());
	if (pApp != NULL) {
		pApp->RegisterMainWindow(m_hWnd);
    }

	// IDM_ABOUTBOX はコマンド メニューの範囲でなければなりません。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);	// システムメニューを取得します
	CString strMenu;
// Make-->
	pSysMenu->AppendMenu(MF_SEPARATOR);	//セパレータ

	strMenu.LoadString(IDS_ADDSTRING);		// 登録
		pSysMenu->AppendMenu(MF_STRING, ID_ADDSTRING, strMenu);
	strMenu.LoadString(IDS_CHANGESTRING);	// 変更
		pSysMenu->AppendMenu(MF_STRING, ID_CHANGE, strMenu);
	strMenu.LoadString(IDS_DELETESTRING);	// 削除
		pSysMenu->AppendMenu(MF_STRING, ID_DELETESTRING, strMenu);

	pSysMenu->AppendMenu(MF_SEPARATOR);	// セパレータ

	strMenu.LoadString(IDS_CATEGORY);	// カテゴリー
		pSysMenu->AppendMenu(MF_STRING, IDS_CATEGORY, strMenu);

	pSysMenu->AppendMenu(MF_SEPARATOR);	// セパレータ

	strMenu.LoadString(IDS_ABOUTBOX);		// AboutBox
		pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strMenu);
// <--Make
	
	// このダイアログ用のアイコンを設定します。フレームワークはアプリケーションのメイン
	// ウィンドウがダイアログでない時は自動的に設定しません。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンを設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンを設定
	
	// TODO: 特別な初期化を行う時はこの場所に追加してください。
// Make-->
	
	// ファイルからウインドウのサイズと位置、リストを取得します

	TCHAR szPath[MAX_PATH];
	GetModuleFileName(NULL, szPath, MAX_PATH); // C:\MyApp\MyApp.exe
	PathRemoveFileSpec(szPath);                 // C:\MyApp
	m_appPath = szPath;
	m_iniPath.Format(_T("%s\\setting.ini"), szPath);

    EnsureDefaultIniMessages(m_iniPath);


	TCHAR buffer[256];
	GetPrivateProfileString(_T("font"), _T("name"), _T("MS UI Gothic"), buffer, 256, m_iniPath);
	m_fontName = buffer;
	m_fontSize = GetPrivateProfileInt(_T("font"), _T("size"), 16, m_iniPath);
	GetPrivateProfileString(_T("Window"), _T("window"), _T("0000000001000100"), buffer, 256, m_iniPath);
	_stscanf_s(buffer, _T("%04X%04X%04X%04X"), &rect.top, &rect.left,
		&rect.bottom, &rect.right);
	MoveWindow(&rect);	// 取得した情報に従ってサイズと位置を変更します

	if (m_fontSize > 0 && !m_fontName.IsEmpty()) {
		if (CreateFontForSetting(m_listFont, m_fontName, m_fontSize)) {
			m_CList.SetFont(&m_listFont);
			m_CCombo.SetFont(&m_listFont);
	    }
    }

	CRect rcStatus;
	m_CList.GetWindowRect(&rcStatus);
	ScreenToClient(&rcStatus);
	if (m_StatusText.GetSafeHwnd() == NULL) {
			m_StatusText.Create(WS_CHILD | ES_MULTILINE | ES_READONLY | ES_CENTER, rcStatus, this, 0x5001);
    }
	if (m_listFont.GetSafeHandle() != NULL) {
		m_StatusText.SetFont(&m_listFont);
    }
	m_StatusText.SetWindowText(_T(""));
	m_StatusText.ShowWindow(SW_HIDE);

	CategoryUpdate();

	UpdateList();
    UpdateLayout();
	LoadNotificationSettings();

	// ツールが使いやすいように常に手前に表示します
	SetWindowPos( &wndTopMost, 0,0,0,0, SWP_NOSIZE | SWP_NOMOVE );

	//////////////////////////////////	//　アイコンをタスクトレイに表示します。
	m_stNtfyIcon.cbSize = sizeof( NOTIFYICONDATA );	// 構造体のサイズです。
	m_stNtfyIcon.uID = 0;							// アイコンの識別ナンバーです。
	m_stNtfyIcon.hWnd = m_hWnd;						// イベントと関連づ	けるウィンドウです。
	m_stNtfyIcon.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;			// 各種設定です。
	m_stNtfyIcon.hIcon = AfxGetApp()->LoadIcon( IDR_MAINFRAME );	// アプリケーションのアイコンを取得して、それをタスクトレイに表示します。
	m_stNtfyIcon.uCallbackMessage = WM_USER_NTFYICON;	// アイコンがクリックされたときに送り出すメッセージです
	lstrcpy( m_stNtfyIcon.szTip, _T( "C-Send" ) );		// チップの文字列です。
	::Shell_NotifyIcon( NIM_ADD, &m_stNtfyIcon );		// タスクトレイに表示します。

// <--Make

	return TRUE;  // TRUE を返すとコントロールに設定したフォーカスは失われません。
}

// ここではシステムコマンドに対応する為の処理を行います
void CCsendDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
// Make-->
	if( nID == ID_ADDSTRING ){		// 「登録」が選択された場合
		int i = m_CList.GetCount();	// 現在のリストの数を取得
		m_CList.SetCurSel( i-1 );	// リストの最後を選択
		ChangeMessage();			// 文字列追加／変更関数を呼びます
    }
	else if( nID == ID_CHANGE ){	// 「変更」が選択されたら
		int j = m_CList.GetCurSel();// 現在の選択を調べます
		if( j == LB_ERR ){			// 何も選択されていない場合は終了します
	        return;
	    }
		ChangeMessage();			// 文字列追加／変更関数を呼びます
    }
	else if( nID == ID_DELETESTRING ){	// 削除
		DeleteString();					// 削除関数を呼び出します
    }
	else if (nID == IDS_CATEGORY) {
		CategoryDlg();
    }
	else
// <--Make
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)	// バージョン情報(AboutBox)
	{
		CAboutDlg dlgAbout;		// ダイアログを作成して
		dlgAbout.DoModal();		// 表示します
    }
	else
	{	// その他デフォルトの処理を行います
		CDialog::OnSysCommand(nID, lParam);
    }
}

// もしダイアログボックスに最小化ボタンを追加するならば、アイコンを描画する
// コードを以下に記述する必要があります。MFC アプリケーションは document/view
// モデルを使っているので、この処理はフレームワークにより自動的に処理されます。

// ウインドウが描画されるときに呼ばれます
void CCsendDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画用のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// クライアントの矩形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンを描画します。
		dc.DrawIcon(x, y, m_hIcon);
    }
	else
	{
		CDialog::OnPaint();
    }
}

// システムは、ユーザーが最小化ウィンドウをドラッグしている間、
// カーソルを表示するためにここを呼び出します。
HCURSOR CCsendDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

// リストを選択したときに呼ばれます
void CCsendDlg::OnSelchangeClist()
{
	int i = m_CList.GetCurSel();
	if (i == LB_ERR) return;

	CString text;

	// 1. まずインデックスがデータの範囲内かチェック
	if (i < m_dataList.GetCount()) {
		// 範囲内なら安全に .value を取得
		text = m_dataList.Datas(i).value;
	    if (SendClipBoard(text)) {
			ShowCopyFeedback(m_dataList.Datas(i).name);
	    }

		// キャプションには名前（name）を出した方が分かりやすいかも？
		SetWindowText(MakeWindowTitle(m_dataList.Datas(i).name, m_bCurrentCategoryIsReadOnly));
    }
	else {
		// 範囲外 ＝ リストの最後にある「(追加)」を選択したとみなす
		text.LoadString(IDS_TITLE);
		SetWindowText(MakeWindowTitle(text, m_bCurrentCategoryIsReadOnly));

		// 通常ファイルでは「(追加)」を選んだらそのまま登録画面を開く
		if (!m_bCurrentCategoryIsReadOnly) {
			ChangeMessage();
	    }
    }

    if ((GetKeyState(VK_CONTROL) & 0x8000) != 0) {
        ShowWindow(SW_MINIMIZE);
    }
}

// ダイアログのサイズが変更されるときに呼ばれます
void CCsendDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: この位置にメッセージ ハンドラ用のコードを追加してください
// Make-->
	if( IsIconic() ){	// アイコン化されたら
		ShowWindow( SW_HIDE );	// ウインドウを非表示にします
        return;
    }
	
	if( !IsZoomed() ){	// 最大化でなければ
		GetWindowRect( &rect );		// 現在のウインドウのサイズを取得しておきます
    }

    UpdateLayout();
// <--Make
}

// クリップボードに転送する関数です
// Make-->
BOOL CCsendDlg::SendClipBoard( CString& text )
{
	CString csMessage;	// エラーメッセージ表示用
	if( !OpenClipboard() ){	// クリップボードのオープン
		// 失敗した場合はメッセージを表示して処理を終了します
		csMessage.LoadString( IDS_CBD_NOOPEN );
		AfxMessageBox( csMessage );
		return FALSE;
    }
	if( !EmptyClipboard() ){	// クリップボードの内容を初期化します
		// 失敗した場合はメッセージを表示して処理を終了します
		csMessage.LoadString( IDS_CBD_NOEMPTY );
		AfxMessageBox( csMessage );
		CloseClipboard();
		return FALSE;
    }

	char* buff;	// クリップボードへ転送するための領域を作ります
	// GlobalAllocを使い、GMEM_DDESHAREの属性で作成します
	// ここで作ったメモリ領域はあとで解放する必要はありません
	buff = (char*) GlobalAlloc( GMEM_DDESHARE, text.GetLength()+1 );
	if (buff == NULL) {
		CloseClipboard();
		return FALSE;
    }

	lstrcpy( buff, (LPCTSTR)text );	// 作成した文字列を転送します

	if ( ::SetClipboardData( CF_TEXT, (HANDLE)buff ) == NULL ){ // クリップボードに転送します
		// 失敗した場合はメッセージを表示して処理を終了します
		csMessage.LoadString( IDS_CBD_NOSEND );
		AfxMessageBox( csMessage );
		CloseClipboard();
		return FALSE;
    }
	CloseClipboard();	// クリップボードを閉じます
	return TRUE;
}
// <--Make

// リストがダブルクリックされたときに呼ばれます
void CCsendDlg::OnDblclkClist() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください

	int i = m_CList.GetCurSel();
	if( i < 0 || i >= m_dataList.GetCount() ){
        return;
    }

	CInputBox cInput;
	CString InputWindowName(_T("内容確認"));

	cInput.SetInputText( m_dataList.Datas(i).name, m_dataList.Datas(i).value );
	cInput.SetViewOnly( TRUE );
	cInput.SetWindowName( InputWindowName );
	cInput.DoModal();
}

BOOL CCsendDlg::ConfirmExit()
{
	return (MessageBox(_T("終了しますか？"), _T("確認"), MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES);
}

// ウインドウを閉じるときに呼ばれます
void CCsendDlg::OnClose() 
{
	// TODO: この位置にメッセージ ハンドラ用のコードを追加するかまたはデフォルトの処理を呼び出してください
// Make-->
	if (!ConfirmExit()) {
        return;
    }
	CDialog::OnOK();	// ウインドウを閉じます
// <--Make
}

// OKボタンが押されたときに呼ばれます
void CCsendDlg::OnOK() 
{
	// TODO: この位置にその他の検証用のコードを追加してください
// Make-->
	return;	// 今回は OKボタンの処理は何もしないので処理をせずに抜けます
// <--Make

	CDialog::OnOK();
}

// キャンセルボタンが押されたときに呼ばれます（場合によってはESCが押されたときも呼ばれたりします）
void CCsendDlg::OnCancel() 
{
	// TODO: この位置に特別な後処理を追加してください。
// Make-->
	return;	// 今回はESCで消えないように処理をせずに抜けます
// <--Make
	CDialog::OnCancel();
}

void CCsendDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	CCsendApp* pApp = static_cast<CCsendApp*>(AfxGetApp());
	if (pApp != NULL) {
		pApp->UnregisterMainWindow();
    }
	
	// TODO: この位置にメッセージ ハンドラ用のコードを追加してください
// Make-->

	::Shell_NotifyIcon( NIM_DELETE, &m_stNtfyIcon );	//タスクトレイのアイコンを削除します。

// ver 1.1-->
	SaveData();	// リストを保存します。
// -->ver 1.1

// <--Make
}

// タスクトレイのアイコンがクリックされたときに呼ばれます
// Make-->
LRESULT CCsendDlg::OnNotifyIconIvents( WPARAM wParam, LPARAM lParam )
{
	switch( lParam ){
	case WM_LBUTTONDOWN:    // 左クリック
		int st;

		if( IsWindowVisible() ){
			st = SW_HIDE;
		}
		else{
			st = SW_SHOW;
			ShowWindow( SW_RESTORE );
			SetWindowPos( &wndTopMost, 0,0,0,0, SWP_NOSIZE | SWP_NOMOVE );
		}

		ShowWindow( st );
		break;
	case WM_USER_RESTORE_MAINWINDOW:
		ShowWindow( SW_RESTORE );
		ShowWindow( SW_SHOW );
		SetWindowPos( &wndTopMost, 0,0,0,0, SWP_NOSIZE | SWP_NOMOVE );
		SetForegroundWindow();
		SetFocus();
		break;
	case WM_RBUTTONDOWN:    //右クリック
	{
		POINT point;
		GetCursorPos( &point );

		SetForegroundWindow();
		SetFocus();

		CMenu cMenu;
		if( !cMenu.CreatePopupMenu() ){
			break;
		}

		CString strMenu;
		int itemCount = m_dataList.GetCount();
		if (itemCount > 0) {
			for (int i = 0; i < itemCount && i <= (ID_TRAY_ITEM_MAX - ID_TRAY_ITEM_BASE); i++) {
				CString itemName = m_dataList.Datas(i).name;
				itemName.Replace(_T("&"), _T("&&"));
				if (itemName.IsEmpty()) {
					itemName = _T("(no items)");
				}
				cMenu.AppendMenu(MF_STRING, ID_TRAY_ITEM_BASE + i, itemName);
			}
		}
		else {
			CString noItems = _T("(no items)");
			noItems.Replace(_T("&"), _T("&&"));
			cMenu.AppendMenu(MF_STRING | MF_GRAYED | MF_DISABLED, ID_TRAY_ITEM_BASE, noItems);
		}

		cMenu.AppendMenu(MF_SEPARATOR);
		strMenu.LoadString(IDS_ABOUTBOX);
		cMenu.AppendMenu( MF_STRING, ID_ABOUT, strMenu);
		strMenu.LoadString(IDS_EXIT);
		cMenu.AppendMenu( MF_STRING, ID_EXIT, strMenu);

		cMenu.TrackPopupMenu( TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this );
		PostMessage( WM_NULL );
	}
		break;
	}

	return 0;
}
// ダイアログの中で右クリックしたときに呼ばれます
// ここでContextMenu（ポップアップのメニュー）の処理を行いましょう
void CCsendDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: この位置にメッセージ ハンドラ用のコードを追加してください

/*
	解説
	  CWnd* pWnd は右クリックされたときのマウスカーソルのあるウインドウです。
		ダイアログボックスの場合中に配置されているコントロールを識別するのに
		使いましょう。
	  CPoint point はマウスカーソルの位置です。
		Screen座標です。
		これを使ってリストの項目を選択させるような場合は座標変換を行う必要が
		あります。
*/	
// Make-->
	// 現在のマウスカーソルがリストにあるかどうかをチェックしています
	if( pWnd != (CWnd*)GetDlgItem( IDC_CLIST ) ){
        return;
    }

	if (m_bCurrentCategoryIsReadOnly) {
        return;
    }

	BOOL tmp;	// ItemFromPointの為の作業用変数です（今回は未使用）
	m_CList.ScreenToClient( &point );	// 右クリックでリストの選択を行うため、座標を変換します
	int nSelect = m_CList.ItemFromPoint( point, tmp );	// マウスカーソルの位置のリストを番号を返します
	m_CList.SetCurSel( nSelect );	// リストを選択します

	CMenu cMenu;	// メニュー
	if( !cMenu.CreatePopupMenu() ){	// ポップアップメニューを作成します
        return;
    }

	CString text;	// 文字列取得用
	int i=m_CList.GetCurSel();	// 現在の選択されているリストを取得します
	m_CList.GetText( i, text );	// 現在の文字列を取得します

	CString l_tmp;	// （追加）用
	l_tmp.LoadString( IDS_LISTADD );

	CString strMenu;	// メニューに表示する文字列用
	if( l_tmp == text ){	// 現在（追加）が選ばれているとき
		strMenu.LoadString(IDS_ADDSTRING);	// メニューには「登録」を表示します
		cMenu.AppendMenu( MF_STRING, ID_ADDSTRING, strMenu); 
    }
	else{	// （追加）以外が選ばれているとき
		strMenu.LoadString(IDS_CHANGESTRING);	// 変更
		cMenu.AppendMenu( MF_STRING, ID_CHANGE, strMenu ); 
		strMenu.LoadString(IDS_DELETESTRING);	// 削除
		cMenu.AppendMenu( MF_STRING, ID_DELETESTRING, strMenu ); 
    }

	// クライアント座標からスクリーン座標に戻します
	m_CList.ClientToScreen( &point );
	// ポップアップメニューの表示はスクリーン座標で指定する必要があります
	cMenu.TrackPopupMenu( TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this );	//ポップアップメニューを表示します。

// <--Make
}

// 文字列を削除します
// Make-->
void CCsendDlg::DeleteString()
{
	if (m_bCurrentCategoryIsReadOnly) {
        return;
    }

	int i = m_CList.GetCount();	// 現在のリストの数を数えます
	if( i<1 ){	// リストに一つもなければ何もしません
        return;
    }

	int j = m_CList.GetCurSel();	// 現在選択されている項目を調べます
	if( j == LB_ERR ){	// 何も選択されていなければ何もしません
        return;
    }

	if( j == i-1 ){	// 選択されている項目が最後（追加）での場合
		CString csMessage;
		csMessage.LoadString( IDS_NODELETE ); // エラーメッセージを出します
		AfxMessageBox( csMessage );
        return;
    }

	CString cst;	// 現在の文字列を取得
	CString output;	// 確認メッセージ用の

	m_CList.GetText( j, cst );	// 現在選択されている文字列を取得します
	output.LoadString( IDS_INFOMESSAGE );	// 問い合わせ用のメッセージをリソースから取得します
	// 内容
	// 　「XXXXXXXXXXX」
	//のようにします
	output += "\x0d\x0a「";	// メッセージを加工します
	output += cst;
	output += "」";

	CString csMessage;	// 削除をすることを促すメッセージ
	csMessage.LoadString( IDS_MESSAGE_DEL );
	if( MessageBox( output, csMessage,	// 確認ダイアログを表示します
				MB_ICONQUESTION | MB_OKCANCEL | MB_DEFBUTTON2 )	// このときデフォルトのボタンをキャンセルにします
		== IDCANCEL ){
        return;	// キャンセルが選択された場合何もせずに終了します
    }

    // remove the selected item (j), not the total count (i)
	m_dataList.Remove(j);

	// 2. ファイルに保存
	SaveData();

	// 3. 表示を更新
	UpdateList();
}

// 登録メニューが選択され時に呼ばれます
void CCsendDlg::OnAddstring() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
// Make-->
	ChangeMessage();
// <--Make
}
// 変更メニューが選択されたときに呼ばれます
void CCsendDlg::OnChange() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
// Make-->
	ChangeMessage();
// <--Make
}

// 削除メニューが選択されたときに呼ばれます
void CCsendDlg::OnDeletestring() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
// Make-->
	DeleteString();
// <--Make
}

void CCsendDlg::CategoryDlg()
{
	categoryDlg dlg;
	dlg.SetIniPath(m_iniPath);

	dlg.DoModal();
	// ダイアログが閉じられたら常にカテゴリを再読み込みしてプルダウンを更新
	CategoryUpdate();
}

void CCsendDlg::OnCbnSelchangeComboCategory()
{
    // カテゴリを切り替えたら新しいカテゴリの内容を読み込む。
	// 保存が不要な現在の設計のため SaveData() は呼ばない。
	// 将来、自動保存を入れる場合の誤追加を避けるため一旦コメントとして残す：
	// SaveData(); // <-- intentionally commented out to avoid implicit saves on category change
	UpdateList();
}
// メッセージの変更と登録
// Make-->
void CCsendDlg::ChangeMessage()
{
	if (m_bCurrentCategoryIsReadOnly) {
        return;
    }

	CInputBox cInput;	// メッセージ編集用のダイアログ
	
	// 現在選択されている文字列を取得します
	int i=m_CList.GetCurSel();

	BOOL flag = FALSE;	// このフラグで、登録と変更を区別します。TRUE--登録  FALSE--変更
	CString InputWindowName;	// メッセージ編集用のダイアログのキャプション用
	if( i >= m_dataList.GetCount() ){	// 現在の文字列が（追加）の時
		InputWindowName.LoadString( IDS_REGIST );	// キャプションは「登録」を選びます
		flag = TRUE;	// フラグを「登録」(TRUE)にします
    }
	else{
		cInput.SetInputText( m_dataList.Datas(i).name, m_dataList.Datas(i).value);	// メッセージ編集用のダイアログに現在選択されている文字列を設定します
		InputWindowName.LoadString( IDS_CHANGE );	// キャプションは「変更」を選びます
    }
	cInput.SetWindowName( InputWindowName );	// キャプションを設定します

	if (cInput.DoModal() == IDOK) {
		CString title, text;
		cInput.GetInputText(title, text);

		// 1. メモリ（m_dataList）を更新
		if (flag) {
			// 新規追加
			if (!m_dataList.Add(title, text)) {
			    AfxMessageBox(GetIniMessage(_T(""), _T("data_add_limit"), _T("データは100件までです。")));
			    return;
		    }
	    }
		else {
			// 既存編集
			m_dataList.Datas(i).name = title;
			m_dataList.Datas(i).value = text;
	    }

		// 2. 確定したメモリの内容をファイルへ物理保存
		// ここで m_dataListPath が使われます
		SaveData();

		// 3. リスト表示を最新状態（ファイルの状態）に同期
		// これにより、追加した項目も即座に画面に反映されます
		UpdateList();

		// 編集した位置にカーソルを戻すと親切
		m_CList.SetCurSel(i);
    }
}
// <--Make

void CCsendDlg::ActivateListItem(int index)
{
    if (index < 0 || index >= m_dataList.GetCount()) {
        return;
    }

    CString text = m_dataList.Datas(index).value;
    if (SendClipBoard(text)) {
        ShowCopyFeedback(m_dataList.Datas(index).name);
    }

    SetWindowText(MakeWindowTitle(m_dataList.Datas(index).name, m_bCurrentCategoryIsReadOnly));

    if ((GetKeyState(VK_CONTROL) & 0x8000) != 0) {
        ShowWindow(SW_MINIMIZE);
    }
}

void CCsendDlg::OnTrayItemSelect(UINT nID)
{
    int index = (int)(nID - ID_TRAY_ITEM_BASE);
    ActivateListItem(index);
}
// タスクトレイで「ﾊﾞｰｼﾞｮﾝ情報」が選択されたときに呼ばれます
void CCsendDlg::OnAbout() 
{
// Make 1.1-->
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	CAboutDlg dlgAbout;		// ダイアログを作成して
	dlgAbout.DoModal();		// 表示します
// <--Make 1.1
}

// タスクトレイで「終了」が選択されたときに呼ばれます
void CCsendDlg::OnExit() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
// Make 1.1-->
	if (!ConfirmExit()) {
        return;
    }
	CDialog::OnOK();	// ウインドウを閉じます
// <--Make 1.1
}

// リストデータの保存
// Make 1.1-->
void CCsendDlg::SaveData()
{
	//////////////////リストを保存します//////////////////
	CString l_tmp;	// 作業用

	int j = m_CList.GetCount();	// リストの数を取得します

	TCHAR szPath[MAX_PATH];
	GetModuleFileName(NULL, szPath, MAX_PATH); // C:\MyApp\MyApp.exe
	PathRemoveFileSpec(szPath);                 // C:\MyApp

	int fontSize = 16;              // デフォルト値
	CString fontName = _T("MS UI Gothic");

	if (m_fontSize != 0) {
		fontSize = m_fontSize;
		fontName = m_fontName;
    }

	CString strSize;
	strSize.Format(_T("%d"), fontSize);

	WritePrivateProfileString(_T("font"), _T("size"), strSize, m_iniPath);
	WritePrivateProfileString(_T("font"), _T("name"), fontName, m_iniPath);

	CString strWindow;
	if (!IsIconic() && !IsZoomed()) {	// ウインドウがアイコン化や最大化されていなければ
		GetWindowRect(&rect);	// ウインドウサイズを取得します
    }
	strWindow.Format("%04X%04X%04X%04X", rect.top, rect.left,
		rect.bottom, rect.right);
	WritePrivateProfileString(_T("Window"), _T("window"), strWindow, m_iniPath);

	// --- 2. リストデータ本体の保存 ---
	int nSel = m_CCombo.GetCurSel();
	if (nSel == CB_ERR) return;

	if (!m_SavePath.IsEmpty() && !m_bCurrentCategoryIsReadOnly) {
		// 構造化されたデータを一括保存
		m_dataList.SaveAll(m_SavePath);
    }

	// 終了時に選択中のカテゴリ名を保存（起動時に復元するため）
	if (nSel != CB_ERR) {
		CString selName;
		DWORD_PTR itemData = m_CCombo.GetItemData(nSel);
		if (itemData != CB_ERR && itemData < (DWORD_PTR)m_categorys.GetCount()) {
			selName = m_categorys.Datas((int)itemData).name;
	    }
		else {
			m_CCombo.GetLBText(nSel, selName);
	    }
		WritePrivateProfileString(_T("category"), _T("last"), selName, m_iniPath);
    }
}
// <--Make 1.1

CString CCsendDlg::Escape(const CString& input)
{
	CString out = input;

	// ① まず \ をエスケープ（最重要）
	out.Replace(_T("\\"), _T("\\\\"));

	// ② 改行（CRLF → \n）
	out.Replace(_T("\r\n"), _T("\\n"));
	out.Replace(_T("\n"), _T("\\n")); // 念のため

	// ③ カンマ
	out.Replace(_T(","), _T("\\,"));

	return out;
}

CString CCsendDlg::Unescape(const CString& input)
{
	CString out = input;

	// 逆順で戻すのがポイント

	// ① \n → 改行
	out.Replace(_T("\\n"), _T("\r\n"));

	// ② \, → ,
	out.Replace(_T("\\,"), _T(","));

    // 3) unescape backslashes
	out.Replace(_T("\\\\"), _T("\\"));

	return out;
}

void CCsendDlg::CategoryUpdate()
{
	int lastSel = m_CCombo.GetCurSel();
	if (lastSel < 0) {
		lastSel = 0;
    }

	m_CCombo.SetRedraw(FALSE);
	m_CCombo.ResetContent();
	m_categorys.LoadAll(m_iniPath);

	for (int i = 0; i < m_categorys.GetCount(); i++) {
		int pos = m_CCombo.AddString(MakeCategoryLabel(m_categorys.Datas(i).name, m_categorys.Datas(i).path));
		if (pos != CB_ERR) {
			m_CCombo.SetItemData(pos, i);
	    }
    }

	// 起動時に前回選択していたカテゴリを復元する
	CString lastName;
	TCHAR buf[256];
	GetPrivateProfileString(_T("category"), _T("last"), _T(""), buf, 256, m_iniPath);
	lastName = buf;

	int selectIndex = 0;
	if (!lastName.IsEmpty()) {
		// 名前で検索して一致するインデックスを選択
		for (int i = 0; i < m_categorys.GetCount(); i++) {
			if (m_categorys.Datas(i).name == lastName) {
				selectIndex = i;
				break;
		    }
	    }
    }

	if (selectIndex < 0) selectIndex = 0;
	m_CCombo.SetCurSel(selectIndex);
	m_CCombo.SetRedraw(TRUE);
}

CString CCsendDlg::GetValidFilePath(CString fileName) {
	// 1. フルパス（m_appPath込み）をチェック
	CString fullPath = m_appPath + _T("\\") + fileName;
	CFileStatus status;
	if (CFile::GetStatus(fullPath, status)) {
		return fullPath;
    }

	// 2. 失敗したらファイル名単体（カレントディレクトリ）をチェック
	if (CFile::GetStatus(fileName, status)) {
		return fileName;
    }

	// どちらにもなければ空を返す
	return _T("");
}

void CCsendDlg::UpdateList() {
	m_CList.SetRedraw(FALSE);
	m_CList.ResetContent();
	ShowListStatus(_T(""), FALSE);
	m_bCurrentCategoryIsReadOnly = FALSE;

	int crrCatIndex = m_CCombo.GetCurSel();
	if (crrCatIndex < 0) {
		m_CList.SetRedraw(TRUE);
        return;
    }

	CString categoryName = m_categorys.Datas(crrCatIndex).name;
	CString fileName = m_categorys.Datas(crrCatIndex).path;
	CString errorText;
    CString errorPath;
	BOOL loadError = FALSE;

	if (IsHttpUrl(fileName)) {
		m_bCurrentCategoryIsReadOnly = TRUE;
		CString tempPath;
		if (DownloadUrlToTempFile(fileName, tempPath)) {
			if (m_dataList.LoadAll(tempPath, &errorText)) {
				m_SavePath = fileName;
		    }
			else {
				loadError = TRUE;
                m_SavePath.Empty();
                errorPath = fileName;
				m_dataList.ClearAll();
		    }
			DeleteFile(tempPath);
	    }
		else {
			loadError = TRUE;
                errorPath = fileName;
			m_SavePath.Empty();
			m_dataList.ClearAll();
			errorText = GetIniMessage(_T(""), _T("url_read_failed"), _T("URLから読み込めませんでした。"));
	    }
    }
	else {
		CString targetPath = GetValidFilePath(fileName);
		if (!targetPath.IsEmpty()) {
			if (m_dataList.LoadAll(targetPath, &errorText)) {
				m_SavePath = targetPath;
				m_bCurrentCategoryIsReadOnly = IsReadOnlyFilePath(targetPath);
		    }
			else {
				loadError = TRUE;
                m_SavePath.Empty();
                errorPath = fileName;
				m_bCurrentCategoryIsReadOnly = TRUE;
				m_dataList.ClearAll();
		    }
	    }
		else {
			CString candidate = m_appPath + _T("\\") + fileName;
			m_SavePath = candidate;
			m_dataList.ClearAll();
	    }
    }

	if (loadError) {
		if (errorText.IsEmpty()) {
			errorText = GetIniMessage(_T(""), _T("read_failed"), _T("読み込みできませんでした。"));
	    }
        ShowListStatus(MakeListStatusMessage(categoryName, errorPath, errorText), TRUE);
		SetWindowText(MakeWindowTitle(categoryName, TRUE));
		m_CList.SetRedraw(TRUE);
		m_CList.Invalidate();
        return;
    }

	for (int i = 0; i < m_dataList.GetCount(); i++) {
		m_CList.AddString(m_dataList.Datas(i).name);
    }

	SetWindowText(MakeWindowTitle(categoryName, m_bCurrentCategoryIsReadOnly));

	if (!m_bCurrentCategoryIsReadOnly) {
		CString adds;
		adds.LoadString(IDS_LISTADD);
		m_CList.AddString(adds);
    }

	m_CList.SetRedraw(TRUE);
	m_CList.Invalidate();
}





















