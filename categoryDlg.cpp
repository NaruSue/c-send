// categoryDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "csend.h"
#include "afxdialogex.h"
#include "categoryDlg.h"
#include "CategoryDataList.h"
#include "DialogFontUtil.h"
#include "IniTextUtil.h"
#include <wininet.h>

#pragma comment(lib, "wininet.lib")

static bool IsHttpUrl(const CString& path)
{
	return path.Left(7).CompareNoCase(_T("http://")) == 0 ||
		path.Left(8).CompareNoCase(_T("https://")) == 0;
}

static bool ValidateUrlSource(const CString& url)
{
	HINTERNET hSession = InternetOpen(_T("C-Send"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (hSession == NULL) {
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
		return false;
	}

	char buffer[16];
	DWORD bytesRead = 0;
	BOOL ok = InternetReadFile(hFile, buffer, sizeof(buffer), &bytesRead);

	InternetCloseHandle(hFile);
	InternetCloseHandle(hSession);
	return ok != FALSE;
}
// categoryDlg ダイアログ

IMPLEMENT_DYNAMIC(categoryDlg, CDialogEx)

categoryDlg::categoryDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(ID_CATEGORY, pParent)
{

}

categoryDlg::~categoryDlg()
{
}


void categoryDlg::ApplyFontAndLayout()
{
	CString fontName;
	int fontSize = 0;
	LoadFontSetting(m_iniPath, fontName, fontSize);

	if (!CreateFontForSetting(m_dialogFont, fontName, fontSize)) {
		return;
	}

	SetFont(&m_dialogFont);
	m_ListCategory.SetFont(&m_dialogFont);
	m_c_catName.SetFont(&m_dialogFont);
	m_c_catPath.SetFont(&m_dialogFont);

	CWnd* pOK = GetDlgItem(IDOK);
	if (pOK != NULL) {
		pOK->SetFont(&m_dialogFont);
	}
	CWnd* pCancel = GetDlgItem(IDCANCEL);
	if (pCancel != NULL) {
		pCancel->SetFont(&m_dialogFont);
	}
	CWnd* pSave = GetDlgItem(IDC_BUTTON_CAT_SAVE);
	if (pSave != NULL) {
		pSave->SetFont(&m_dialogFont);
	}
	CWnd* pDel = GetDlgItem(IDC_BUTTON_CAT_DEL);
	if (pDel != NULL) {
		pDel->SetFont(&m_dialogFont);
	}
	CWnd* pUp = GetDlgItem(IDC_BUTTON_CAT_UP);
	if (pUp != NULL) {
		pUp->SetFont(&m_dialogFont);
	}
	CWnd* pDown = GetDlgItem(IDC_BUTTON_CAT_DOWN);
	if (pDown != NULL) {
		pDown->SetFont(&m_dialogFont);
	}

	const double scale = (fontSize > 0) ? ((double)fontSize / 8.0) : 1.0;
	if (scale != 1.0) {
		const UINT ids[] = { IDC_LIST_CATEGORY, IDC_EDIT_CAT_NAME, IDC_EDIT_CAT_PATH, IDOK, IDCANCEL,
			IDC_BUTTON_CAT_SAVE, IDC_BUTTON_CAT_DEL, IDC_BUTTON_CAT_UP, IDC_BUTTON_CAT_DOWN };
		ApplyScaledLayout(this, scale, scale, ids, _countof(ids));
	}
}

void categoryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CATEGORY, m_ListCategory);
	DDX_Control(pDX, IDC_EDIT_CAT_NAME, m_c_catName);
	DDX_Control(pDX, IDC_EDIT_CAT_PATH, m_c_catPath);

	DDX_Text(pDX, IDC_EDIT_CAT_NAME, m_strName);
	DDX_Text(pDX, IDC_EDIT_CAT_PATH, m_strPath);
}


BEGIN_MESSAGE_MAP(categoryDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &categoryDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &categoryDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_CAT_UP, &categoryDlg::OnBnClickedButtonCatUp)
	ON_BN_CLICKED(IDC_BUTTON_CAT_DOWN, &categoryDlg::OnBnClickedButtonCatDown)
	ON_BN_CLICKED(IDC_BUTTON_CAT_DEL, &categoryDlg::OnBnClickedButtonCatDel)
	ON_LBN_SELCHANGE(IDC_LIST_CATEGORY, &categoryDlg::OnLbnSelchangeListCategory)
	ON_BN_CLICKED(IDC_BUTTON_CAT_SAVE, &categoryDlg::OnBnClickedButtonCatSave)
END_MESSAGE_MAP()

BOOL categoryDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog(); // 親クラスの呼び出し（必須！）

	if (m_iniPath.IsEmpty()) {
		m_iniPath = GetSettingIniPathFromExe();
	}

	ApplyFontAndLayout();

	m_catList.LoadAll(m_iniPath);

	UpdateCategoryList();

	return TRUE;
}

void categoryDlg::UpdateCategoryList() {
    // 既存項目をクリアしてから再構築
	m_ListCategory.SetRedraw(FALSE);
	m_ListCategory.ResetContent();
	m_currentIndex = -1;
    int count = m_catList.GetCount();
	if (count > 0) {
		for (int i = 0; i < count; i++) {
            int pos = m_ListCategory.AddString(m_catList.Datas(i).name);
			m_ListCategory.SetItemData(pos, i); // store original index to avoid issues if control is sorted
		}
	}
	CString text;
	text.LoadString(IDS_LISTADD);
    int addPos = m_ListCategory.AddString(text);
	// mark the "add" line with a sentinel value (-2) so we can detect it reliably
	if (addPos != LB_ERR) {
		m_ListCategory.SetItemData(addPos, (DWORD_PTR)-2);
	}
	m_ListCategory.SetRedraw(TRUE);
	m_ListCategory.Invalidate();
	m_ListCategory.UpdateWindow();
}

// Find the displayed list box position that corresponds to internal index
int categoryDlg::FindListPosByIndex(int idx) {
	int n = m_ListCategory.GetCount();
	for (int i = 0; i < n; i++) {
		DWORD_PTR data = m_ListCategory.GetItemData(i);
        // GetItemData may return LB_ERR; ensure comparison is robust
		if (data != LB_ERR && data == (DWORD_PTR)idx) return i;
	}
	return LB_ERR;
}

// Select by internal index (sets listbox selection and m_currentIndex and edit fields)
void categoryDlg::SelectByIndex(int idx) {
	if (idx < 0 || idx >= m_catList.GetCount()) {
		m_ListCategory.SetCurSel(LB_ERR);
		m_currentIndex = -1;
		m_strName.Empty();
		m_strPath.Empty();
		UpdateData(FALSE);
		return;
	}
	int pos = FindListPosByIndex(idx);
	if (pos == LB_ERR) {
		// Fallback: try to select by idx position
        if (idx < m_ListCategory.GetCount()) {
			m_ListCategory.SetCurSel(idx);
		}
		else if (m_ListCategory.GetCount() > 0) {
			m_ListCategory.SetCurSel(0);
		}
		else {
			m_ListCategory.SetCurSel(LB_ERR);
		}
	}
	else {
		m_ListCategory.SetCurSel(pos);
	}
	m_currentIndex = idx;
	m_strName = m_catList.Datas(idx).name;
	m_strPath = m_catList.Datas(idx).path;
	UpdateData(FALSE);
}

// categoryDlg メッセージ ハンドラー

void categoryDlg::OnBnClickedOk()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	CDialogEx::OnOK();
}

void categoryDlg::OnBnClickedButton1()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
}

void categoryDlg::OnBnClickedCancel()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	CDialogEx::OnCancel();
}

void categoryDlg::OnBnClickedButtonCatUp()
{
    // 上へ移動
	int count = m_catList.GetCount();
	if (m_currentIndex <= 0 || m_currentIndex >= count) {
		// 先頭または未選択なら何もしない
		return;
	}

	m_catList.MoveUp(m_currentIndex);
	m_catList.SaveAll(m_iniPath);

	// リストを再表示して選択を移動
    UpdateCategoryList();
	int newIndex = m_currentIndex - 1;
	if (newIndex < 0) newIndex = 0;
	// select by internal index so the displayed cursor stays near moved item
	SelectByIndex(newIndex);
}

void categoryDlg::OnBnClickedButtonCatDown()
{
    // 下へ移動
	int count = m_catList.GetCount();
	if (m_currentIndex < 0 || m_currentIndex >= count - 1) {
		// 末尾または未選択なら何もしない
		return;
	}

	m_catList.MoveDown(m_currentIndex);
	m_catList.SaveAll(m_iniPath);

	// リストを再表示して選択を移動
    UpdateCategoryList();
	int newIndex = m_currentIndex + 1;
	if (newIndex >= m_catList.GetCount()) newIndex = m_catList.GetCount() - 1;
	// select by internal index so the displayed cursor stays near moved item
	SelectByIndex(newIndex);
}

void categoryDlg::OnBnClickedButtonCatDel()
{
    // 削除処理：物理ファイルは削除せず、INI とリストから削除する
	// m_currentIndex が選択されていない（追加行など）の場合は何もしない
	if (m_currentIndex < 0) {
		CString msg;
		msg.LoadString(IDS_NODELETE);
		AfxMessageBox(msg);
		return;
	}

	// 確認ダイアログ
	CString confirm;
	confirm.Format(_T("Delete category '%s'?"), m_catList.Datas(m_currentIndex).name);
	if (MessageBox(confirm, _T("Confirm"), MB_ICONQUESTION | MB_OKCANCEL | MB_DEFBUTTON2) == IDCANCEL) {
		return;
	}

	// メモリから削除して INI に保存
	m_catList.Remove(m_currentIndex);
	m_catList.SaveAll(m_iniPath);

	// リスト再表示
	UpdateCategoryList();

	// 選択の調整: 削除した位置が存在すれば同じインデックスを、そうでなければ最後の要素を選択
	int count = m_catList.GetCount();
	if (count > 0) {
		int newIndex = m_currentIndex;
		if (newIndex >= count) newIndex = count - 1;
		m_ListCategory.SetCurSel(newIndex);
		m_currentIndex = newIndex;
		m_strName = m_catList.Datas(newIndex).name;
		m_strPath = m_catList.Datas(newIndex).path;
	}
	else {
		m_currentIndex = -1;
		m_strName.Empty();
		m_strPath.Empty();
	}

	UpdateData(FALSE);
}

void categoryDlg::OnLbnSelchangeListCategory()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
    int sel = m_ListCategory.GetCurSel();
	if (sel == LB_ERR) {
		m_strName.Empty();
		m_strPath.Empty();
		m_currentIndex = -1;
	}
	else {
     // Use item data mapping to get the real index in m_catList
		DWORD_PTR data = m_ListCategory.GetItemData(sel);
		if (data == (DWORD_PTR)-2) {
			// this is the "add" sentinel
			m_strName.Empty();
			m_strPath.Empty();
			m_currentIndex = -1;
		}
		else if (data == LB_ERR) {
			// fallback: treat sel as index
			if (sel >= m_catList.GetCount()) {
				m_strName.Empty();
				m_strPath.Empty();
				m_currentIndex = -1;
			}
			else {
				m_strName = m_catList.Datas(sel).name;
				m_strPath = m_catList.Datas(sel).path;
				m_currentIndex = sel;
			}
		}
		else {
			int idx = (int)data;
			if (idx >= 0 && idx < m_catList.GetCount()) {
				m_strName = m_catList.Datas(idx).name;
				m_strPath = m_catList.Datas(idx).path;
				m_currentIndex = idx;
			}
			else {
				m_strName.Empty();
				m_strPath.Empty();
				m_currentIndex = -1;
			}
		}
	}

	UpdateData(FALSE);

}

void categoryDlg::OnBnClickedButtonCatSave()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	UpdateData(TRUE);

	if (m_strName.IsEmpty() || m_strPath.IsEmpty()) {
		// メッセージとか入れるならここで
		return;
	}

	BOOL listCheck = TRUE;

	for (int i = 0; i < m_catList.GetCount(); i++) {
		if (i == m_currentIndex) continue;

		if (m_catList.Datas(i).name == m_strName) {
			listCheck = FALSE;
			break;
		}
		if (m_catList.Datas(i).path == m_strPath) {
			listCheck = FALSE;
			break;
		}
	}

	if (!listCheck) {
		// メッセージを入れるならここで
		return;
	}

	if (IsHttpUrl(m_strPath) && !ValidateUrlSource(m_strPath)) {
		AfxMessageBox(GetIniMessage(_T(""), _T("url_read_failed"), _T("URLから読み込めませんでした。")));
		return;
	}

 if (m_currentIndex < 0) {	// 新規追加
		CategoryData data;
		data.name = m_strName;
		data.path = m_strPath;
		m_catList.Add(data);
		m_catList.SaveAll(m_iniPath);
		// リスト再表示
		UpdateCategoryList();
		// 選択を新しく追加した最後のアイテムにする
		int newIndex = m_catList.GetCount() - 1;
		if (newIndex >= 0) {
			m_ListCategory.SetCurSel(newIndex);
			m_currentIndex = newIndex;
		}
		UpdateData(FALSE);
	}
	else {	// 既存の変更
		m_catList.Datas(m_currentIndex).name = m_strName;
		m_catList.Datas(m_currentIndex).path = m_strPath;
		m_catList.SaveAll(m_iniPath);
		UpdateCategoryList();
		if (m_currentIndex >= 0 && m_currentIndex < m_ListCategory.GetCount()) {
			m_ListCategory.SetCurSel(m_currentIndex);
		}
		UpdateData(FALSE);
	}
}




