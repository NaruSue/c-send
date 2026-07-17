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
static const UINT kExitMainWindowMessage = WM_APP + 532;

static BOOL HasAllExitArgument()
{
	int argc = 0;
	LPWSTR* argv = ::CommandLineToArgvW(::GetCommandLineW(), &argc);
	if (argv == NULL) {
		return FALSE;
	}

	BOOL found = FALSE;
	for (int i = 1; i < argc; ++i) {
		if (::_wcsicmp(argv[i], L"/allexit") == 0) {
			found = TRUE;
			break;
		}
	}

	::LocalFree(argv);
	return found;
}

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
	m_bMainWindowRegistered = TRUE;
	return TRUE;
}

void CCsendApp::UnregisterMainWindow()
{
	if (m_bMainWindowRegistered && m_pSharedMainWnd != NULL) {
		*m_pSharedMainWnd = NULL;
	}
	m_bMainWindowRegistered = FALSE;
}

BOOL CCsendApp::NotifyExistingInstance(UINT commandMessage, BOOL waitForExit)
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

	const DWORD timeoutMs = waitForExit ? 10000 : 2000;
	const DWORD startTick = ::GetTickCount();
	HWND hWnd = NULL;
	do {
		hWnd = *m_pSharedMainWnd;
		if (::IsWindow(hWnd)) {
			break;
		}
		::Sleep(50);
	} while ((::GetTickCount() - startTick) < timeoutMs);

	if (!::IsWindow(hWnd)) {
		return FALSE;
	}

	if (!waitForExit) {
		DWORD_PTR result = 0;
		if (::SendMessageTimeout(hWnd, kNotifyIconMessage, 0, commandMessage, SMTO_ABORTIFHUNG | SMTO_BLOCK, 1000, &result) != 0) {
			return TRUE;
		}

		ActivateMainWindow(hWnd);
		return TRUE;
	}

	DWORD processId = 0;
	::GetWindowThreadProcessId(hWnd, &processId);
	HANDLE hProcess = (processId != 0) ? ::OpenProcess(SYNCHRONIZE, FALSE, processId) : NULL;
	if (!::PostMessage(hWnd, kNotifyIconMessage, 0, commandMessage)) {
		if (hProcess != NULL) {
			::CloseHandle(hProcess);
		}
		return FALSE;
	}

	DWORD elapsed = ::GetTickCount() - startTick;
	DWORD remaining = (elapsed < timeoutMs) ? timeoutMs - elapsed : 0;
	BOOL stopped = FALSE;
	if (hProcess != NULL) {
		stopped = (::WaitForSingleObject(hProcess, remaining) == WAIT_OBJECT_0);
		::CloseHandle(hProcess);
	}
	else {
		while (::IsWindow(hWnd) && (::GetTickCount() - startTick) < timeoutMs) {
			::Sleep(50);
		}
		stopped = !::IsWindow(hWnd);
	}
	return stopped;
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

	int exitCode = CWinApp::ExitInstance();
	return (m_commandExitCode != 0) ? m_commandExitCode : exitCode;
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

	BOOL allExitRequested = HasAllExitArgument();
	m_hSingleInstanceMutex = ::CreateMutex(NULL, FALSE, kSingleInstanceMutexName);
	if (m_hSingleInstanceMutex != NULL && ::GetLastError() == ERROR_ALREADY_EXISTS) {
		BOOL notified = NotifyExistingInstance(allExitRequested ? kExitMainWindowMessage : kRestoreMainWindowMessage, allExitRequested);
		if (allExitRequested && !notified) {
			m_commandExitCode = 1;
		}
		return FALSE;
	}
	if (allExitRequested) {
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













