// csend.cpp : アプリケーション用クラスの定義を行います。
//

#include "stdafx.h"
#include "csend.h"
#include "csendDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCsendApp

BEGIN_MESSAGE_MAP(CCsendApp, CWinApp)
	//{{AFX_MSG_MAP(CCsendApp)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
		//        この位置に生成されるコードを編集しないでください。
	//}}AFX_MSG
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCsendApp クラスの構築

CCsendApp::CCsendApp()
{
	// TODO: この位置に構築用のコードを追加してください。
	// ここに InitInstance 中の重要な初期化処理をすべて記述してください。
}

/////////////////////////////////////////////////////////////////////////////
// 唯一の CCsendApp オブジェクト

CCsendApp theApp;

static const TCHAR kSingleInstanceMutexName[] = _T("Local\\C-Send-SingleInstance");
static const TCHAR kMainWindowMappingName[] = _T("Local\\C-Send-MainWindow");
static const UINT kNotifyIconMessage = WM_USER + 531;
static const UINT kRestoreMainWindowMessage = WM_APP + 531;

static void ActivateMainWindow(HWND hWnd)
{
	if (!::IsWindow(hWnd)) {
		return;
	}

	::ShowWindow(hWnd, SW_SHOW);
	if (::IsIconic(hWnd) || !::IsWindowVisible(hWnd)) {
		::ShowWindow(hWnd, SW_RESTORE);
	}

	::SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
	::BringWindowToTop(hWnd);
	::SetForegroundWindow(hWnd);
	::SetActiveWindow(hWnd);
}
BOOL CCsendApp::RegisterMainWindow(HWND hWnd)
{
	if (m_hMainWindowMapping == NULL) {
		m_hMainWindowMapping = ::CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(HWND), kMainWindowMappingName);
	}

	if (m_hMainWindowMapping == NULL) {
		return FALSE;
	}

	if (m_pSharedMainWnd == NULL) {
		m_pSharedMainWnd = reinterpret_cast<HWND*>(::MapViewOfFile(m_hMainWindowMapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(HWND)));
	}

	if (m_pSharedMainWnd == NULL) {
		return FALSE;
	}

	*m_pSharedMainWnd = hWnd;
	return TRUE;
}

void CCsendApp::UnregisterMainWindow()
{
	if (m_pSharedMainWnd != NULL) {
		*m_pSharedMainWnd = NULL;
	}
}

BOOL CCsendApp::RestoreExistingInstance()
{
	if (m_hMainWindowMapping == NULL) {
		m_hMainWindowMapping = ::CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(HWND), kMainWindowMappingName);
	}

	if (m_hMainWindowMapping == NULL) {
		return FALSE;
	}

	if (m_pSharedMainWnd == NULL) {
		m_pSharedMainWnd = reinterpret_cast<HWND*>(::MapViewOfFile(m_hMainWindowMapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(HWND)));
	}

	if (m_pSharedMainWnd == NULL) {
		return FALSE;
	}

	HWND hWnd = NULL;
	for (int i = 0; i < 40; ++i) {
		hWnd = *m_pSharedMainWnd;
		if (::IsWindow(hWnd)) {
			break;
		}
		::Sleep(50);
	}

	if (!::IsWindow(hWnd)) {
		return FALSE;
	}

	DWORD_PTR result = 0;
	if (::SendMessageTimeout(hWnd, kNotifyIconMessage, 0, kRestoreMainWindowMessage, SMTO_ABORTIFHUNG | SMTO_BLOCK, 1000, &result) != 0) {
		return TRUE;
	}

	ActivateMainWindow(hWnd);
	return TRUE;
}

int CCsendApp::ExitInstance()
{
	UnregisterMainWindow();

	if (m_pSharedMainWnd != NULL) {
		::UnmapViewOfFile(m_pSharedMainWnd);
		m_pSharedMainWnd = NULL;
	}

	if (m_hMainWindowMapping != NULL) {
		::CloseHandle(m_hMainWindowMapping);
		m_hMainWindowMapping = NULL;
	}

	if (m_hSingleInstanceMutex != NULL) {
		::CloseHandle(m_hSingleInstanceMutex);
		m_hSingleInstanceMutex = NULL;
	}

	return CWinApp::ExitInstance();
}
/////////////////////////////////////////////////////////////////////////////
// CCsendApp クラスの初期化

BOOL CCsendApp::InitInstance()
{
	// 標準的な初期化処理
	// もしこれらの機能を使用せず、実行ファイルのサイズを小さくしたけ
	//  れば以下の特定の初期化ルーチンの中から不必要なものを削除して
	//  ください。

#ifdef _AFXDLL
	Enable3dControls();			// 共有 DLL 内で MFC を使う場合はここをコールしてください。
#endif

	m_hSingleInstanceMutex = ::CreateMutex(NULL, FALSE, kSingleInstanceMutexName);
	if (m_hSingleInstanceMutex != NULL && ::GetLastError() == ERROR_ALREADY_EXISTS) {
		RestoreExistingInstance();
		return FALSE;
	}

	CCsendDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: ダイアログが <OK> で消された時のコードを
		//       記述してください。
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: ダイアログが <ｷｬﾝｾﾙ> で消された時のコードを
		//       記述してください。
	}
	
	// ダイアログが閉じられてからアプリケーションのメッセージ ポンプを開始するよりは、
	// アプリケーションを終了するために FALSE を返してください。
	return FALSE;
}













