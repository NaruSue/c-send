// csendDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "csend.h"
#include "csendDlg.h"

// Make -->
#include "InputBox.h"	// 文字列登録用ダイアログのヘッダファイル
// <--Make

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Make-->
#define WM_USER_NTFYICON	(WM_USER+531)	// タスクトレイアイコンからのメッセージ用
// <--Make


/////////////////////////////////////////////////////////////////////////////
// アプリケーションのバージョン情報で使われている CAboutDlg ダイアログ

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

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
	//}}AFX_DATA_MAP
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
	ON_COMMAND(ID_HELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCsendDlg メッセージ ハンドラ

// ダイアログボックスが表示される前に呼ばれます
BOOL CCsendDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

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
	CString l_tmp;	// 行を読み込むための作業領域

	CString adds;	// リストの最後の（追加）メッセージ用です。この文字はリソースで変更できます
	adds.LoadString( IDS_LISTADD );	// 「(追加)」

	CStdioFile savedata;	// セーブされているデータファイルを扱います
	CString csSaveFileName;	// セーブされているファイル名を指定します、リソースで変更できます
	csSaveFileName.LoadString( IDS_SAVEFILE );	// リソースからファイル名を取得します
	if( savedata.Open( csSaveFileName,
			CFile::modeRead | CFile::typeText ) ){	// 読み込み、テキストモードで開きます


		if (savedata.ReadString(l_tmp)) {


			// "15,MS UI Gothic" のような形式を分解
			int commaPos = l_tmp.Find(_T(","));
			if (commaPos > 0) {
				m_fontSize = _ttoi(l_tmp.Left(commaPos));
				m_fontName = l_tmp.Mid(commaPos + 1);
			}
			

			if (m_fontSize > 0 && !m_fontName.IsEmpty()) {
				LOGFONT lf{};
				lf.lfHeight = -MulDiv(m_fontSize, GetDeviceCaps(::GetDC(NULL), LOGPIXELSY), 72);
				_tcsncpy_s(lf.lfFaceName, m_fontName, LF_FACESIZE - 1);

				m_listFont.DeleteObject();              // 既存フォントがあれば削除
				m_listFont.CreateFontIndirect(&lf);     // メンバ変数に作成
				m_CList.SetFont(&m_listFont);           // 有効なフォントを関連付け
			}
		}

		if( savedata.ReadString( l_tmp ) ){	// 一行読み込み
			// 一行目はサイズと位置が入っています
			sscanf( l_tmp, "%04X%04X%04X%04X", &rect.top, &rect.left,
								&rect.bottom, &rect.right );
			MoveWindow( &rect );	// 取得した情報に従ってサイズと位置を変更します
		}			
		while( savedata.ReadString( l_tmp ) ){	// 行が有る間読み込みます
			if( l_tmp != adds ){		// （追加）という文字で無ければ
				m_CList.AddString(l_tmp);	// リストに追加します
			}
		}
		savedata.Close();		// ファイルを閉じます
	}
	
	m_CList.InsertString(-1,adds);	// リストの最後に（追加）を加えます

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
	else if( nID == ID_HELP ){		// 「ヘルプ」
		AfxGetApp()->WinHelp(0,HELP_CONTENTS);	// ヘルプを呼び出します 
		return;
	}
	else if( nID == ID_DELETESTRING ){	// 削除
		DeleteString();					// 削除関数を呼び出します
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
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
// Make-->
	CString text;	// リスト文字列取得用
	CString adds;	// （追加）用

	int i=m_CList.GetCurSel();	// 現在選択されているリスト
	m_CList.GetText( i, text );	// 文字列を取得する
	
	adds.LoadString( IDS_LISTADD );	// （追加）を取得する
	if( text != adds ){			// 選択されているリストが（追加）でなければ
		SendClipBoard( text );	// クリップボードに文字列を転送します
	}
	else{		// 選択されているリストが（追加）ならばtextを初期化します
		text.LoadString( IDS_TITLE );
	}
	SetWindowText( text );	// 現在のtextの内容をキャプションに表示します

	// Ctrlキーが押されていたら最小化
	if ((GetKeyState(VK_CONTROL) & 0x8000) != 0) {
		ShowWindow(SW_MINIMIZE);
	}
// <--Make
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

	m_CList.MoveWindow( 0, 0, cx, cy);	// リストのサイズをダイアログのクライアントサイズに変更します
// <--Make
}

// クリップボードに転送する関数です
// Make-->
void CCsendDlg::SendClipBoard( CString& text )
{
	CString csMessage;	// エラーメッセージ表示用
	if( !OpenClipboard() ){	// クリップボードのオープン
		// 失敗した場合はメッセージを表示して処理を終了します
		csMessage.LoadString( IDS_CBD_NOOPEN );
		AfxMessageBox( csMessage );
		return;
	}
	if( !EmptyClipboard() ){	// クリップボードの内容を初期化します
		// 失敗した場合はメッセージを表示して処理を終了します
		csMessage.LoadString( IDS_CBD_NOEMPTY );
		AfxMessageBox( csMessage );
		CloseClipboard();
		return;
	}

	char* buff;	// クリップボードへ転送するための領域を作ります
	// GlobalAllocを使い、GMEM_DDESHAREの属性で作成します
	// ここで作ったメモリ領域はあとで解放する必要はありません
	buff = (char*) GlobalAlloc( GMEM_DDESHARE, text.GetLength()+1 );

	lstrcpy( buff, (LPCTSTR)text );	// 作成した領域に文字列を転送します

	if ( ::SetClipboardData( CF_TEXT, (HANDLE)buff ) == NULL ){ // クリップボードに転送します
		// 失敗した場合はメッセージを表示して処理を終了します
		csMessage.LoadString( IDS_CBD_NOSEND );
		AfxMessageBox( csMessage );
	}
	CloseClipboard();	// クリップボードを閉じます
}
// <--Make

// リストがダブルクリックされたときに呼ばれます
void CCsendDlg::OnDblclkClist() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください

// Make-->
	ChangeMessage();
// <--Make
}

// ウインドウを閉じるときに呼ばれます
void CCsendDlg::OnClose() 
{
	// TODO: この位置にメッセージ ハンドラ用のコードを追加するかまたはデフォルトの処理を呼び出してください
// Make-->
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
	
	// TODO: この位置にメッセージ ハンドラ用のコードを追加してください
// Make-->

	::Shell_NotifyIcon( NIM_DELETE, &m_stNtfyIcon );	//タスクトレイのアイコンを削除します。
	AfxGetApp()->WinHelp(0,HELP_QUIT);	// ヘルプを閉じます

// ver 1.1-->
	SaveData();	// リストを保存します。
// -->ver 1.1

// <--Make
}

// タスクトレイのアイコンがクリックされたときに呼ばれます
// Make-->
LRESULT CCsendDlg::OnNotifyIconIvents( WPARAM wParam, LPARAM lParam ){

	switch( lParam ){
	case WM_LBUTTONDOWN:	// 左クリック
		int st;	// ウインドウのモードを取得
	
		if( IsWindowVisible() ){	// ウインドウが表示されていたら
			st = SW_HIDE;
		}
		else{	// ウインドウが非表示なら
			st = SW_SHOW;	// ウインドウを表示して元の大きさに戻します
			ShowWindow( SW_RESTORE );
			// ツールが使いやすいように常に手前に表示します
			SetWindowPos( &wndTopMost, 0,0,0,0, SWP_NOSIZE | SWP_NOMOVE );
		}

		ShowWindow( st );	// ウインドウの表示／非表示を切り替えます

		break;
	case WM_RBUTTONDOWN:	//右クリック
// Make 1.1-->
		POINT point;
		GetCursorPos( &point );	// 現在のカーソルの位置を取得

		SetForegroundWindow();	//ウィンドウをフォアグラウンドに持ってきます。
		SetFocus();	//これをしないと、メニューが消えなくなります。

		CMenu cMenu;	// メニュー
		if( !cMenu.CreatePopupMenu() ){	// ポップアップメニューを作成します
			break;
		}

		CString strMenu;	// メニューに表示する文字列用
		strMenu.LoadString(IDS_ABOUTBOX);	// メニューに「バージョン情報」を表示します
		cMenu.AppendMenu( MF_STRING, ID_ABOUT, strMenu); 
		strMenu.LoadString(IDS_EXIT);	// メニューに「終了」を表示します
		cMenu.AppendMenu( MF_STRING, ID_EXIT, strMenu); 

		cMenu.TrackPopupMenu( TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this );	//ポップアップメニューを表示します。

		PostMessage( WM_NULL );	//これをしないと、２度目のメニューがすぐ消えちゃいます。
// -->Make 1.1
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

	m_CList.DeleteString(j);	// リストから文字列を削除します
}
// <--Make

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

// メッセージの変更と登録
// Make-->
void CCsendDlg::ChangeMessage()
{
	CInputBox cInput;	// メッセージ編集用のダイアログ
	CString text;		// リストの文字列
	
	// 現在選択されている文字列を取得します
	int i=m_CList.GetCurSel();
	m_CList.GetText( i, text );

	// （追加）をリソースから取得します
	CString l_tmp;
	l_tmp.LoadString( IDS_LISTADD );


	BOOL flag = FALSE;	// このフラグで、登録と変更を区別します。TRUE--登録  FALSE--変更
	CString InputWindowName;	// メッセージ編集用のダイアログのキャプション用
	if( text == l_tmp ){	// 現在の文字列が（追加）の時
		InputWindowName.LoadString( IDS_REGIST );	// キャプションは「登録」を選びます
		flag = TRUE;	// フラグを「登録」(TRUE)にします
	}
	else{
		cInput.SetInputText( text );	// メッセージ編集用のダイアログに現在選択されている文字列を設定します
		InputWindowName.LoadString( IDS_CHANGE );	// キャプションは「変更」を選びます
	}
	cInput.SetWindowName( InputWindowName );	// キャプションを設定します

	if( cInput.DoModal()==IDOK ){	// ダイアログを表示します
		// OKボタンが押されて終了した場合以下の処理を行います
		CString text;	// メッセージ編集ダイアログからの文字列取得用
		cInput.GetInputText(text);	// メッセージ編集用ダイアログから文字列を取得します
		if( !flag ){	// フラグが「変更」(FALSE)なら、現在のリストの文字列を削除します
			m_CList.DeleteString(i);
		}
		if( text.GetLength() ){	// 取得した文字列がある場合
			m_CList.InsertString( i, (LPCTSTR) text );	// リストに文字列を追加します
		}
	}
// Make 1.1-->
	SaveData();	// リストを保存します
// -->Make 1.1

}
// <--Make

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
	CDialog::OnOK();	// ウインドウを閉じます
// <--Make 1.1
}

void CCsendDlg::OnHelp() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
// Make 1.1-->
	AfxGetApp()->WinHelp(0,HELP_CONTENTS);	// ヘルプを呼び出します 
// <--Make 1.1
}


// リストデータの保存
// Make 1.1-->
void CCsendDlg::SaveData()
{
	//////////////////リストを保存します//////////////////
	CString l_tmp;	// 作業用

	int j = m_CList.GetCount();	// リストの数を取得します

	CStdioFile savedata;	// 保存用ファイル
	CString csSaveFileName;	// 保存用ファイル名
	csSaveFileName.LoadString( IDS_SAVEFILE );	// 保存用ファイルをリソースから取得します
	if( savedata.Open( csSaveFileName,	// テキストモード、ライト属性で、無い場合は新規作成します
		CFile::modeCreate | CFile::modeWrite | CFile::typeText ) ){

		int fontSize = 16;              // デフォルト値
		CString fontName = _T("MS UI Gothic");

		if (m_fontSize != 0) {
			fontSize = m_fontSize;
			fontName = m_fontName;
		}

		// --- 1行目: フォント情報 ---
		CString fontLine;
		fontLine.Format(_T("%d,%s\n"), fontSize, (LPCTSTR)fontName);
		savedata.WriteString(fontLine);


		char buff[17];	// 2行目用
		if( !IsIconic() && !IsZoomed() ){	// ウインドウがアイコン化や最大化されていなければ
			GetWindowRect( &rect );	// ウインドウサイズを取得します
		}
		// １行目はサイズと位置を書き込みます
		wsprintf( buff, "%04X%04X%04X%04X\n", rect.top, rect.left,
						rect.bottom, rect.right );
		savedata.WriteString(buff);

		for(int i=0; i<j; i++){	// リストを１つづつ取り込んで１行ずつ書き込みます
			m_CList.GetText( i, l_tmp );
			l_tmp += "\n";
			savedata.WriteString(l_tmp);
		}
		savedata.Close();	// ファイルを閉じます
	}
}
// <--Make 1.1
