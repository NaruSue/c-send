// categoryDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "csend.h"
#include "afxdialogex.h"
#include "categoryDlg.h"
#include "CategoryDataList.h"


// categoryDlg ダイアログ

IMPLEMENT_DYNAMIC(categoryDlg, CDialogEx)

categoryDlg::categoryDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(ID_CATEGORY, pParent)
{

}

categoryDlg::~categoryDlg()
{
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
	m_ListCategory.SetItemData(addPos, (DWORD_PTR)-2);
	m_ListCategory.SetRedraw(TRUE);
	m_ListCategory.Invalidate();
	m_ListCategory.UpdateWindow();
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
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
}

void categoryDlg::OnBnClickedButtonCatDown()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
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


 if (m_currentIndex < 0) {	// 新規追加
		CategoryData data;
		data.name = m_strName;
		data.path = m_strPath;
		m_catList.Add(data);
		m_catList.SaveAll(m_iniPath);
		{
			CString msg;
			msg.Format(_T("Saved to:\r\n%s"), m_iniPath);
			AfxMessageBox(msg, MB_OK);
		}
        // デバッグ: メモリ上のリスト状態を確認
		{
			CString dbg;
			dbg.Format(_T("In-memory count=%d\r\n"), m_catList.GetCount());
			for (int j = 0; j < m_catList.GetCount(); j++) {
				dbg.AppendFormat(_T("%d: %s\r\n"), j, m_catList.Datas(j).name);
			}
			AfxMessageBox(dbg, MB_OK);
		}
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
		{
			CString msg;
			msg.Format(_T("Saved to:\r\n%s"), m_iniPath);
			AfxMessageBox(msg, MB_OK);
		}
        // デバッグ: メモリ上のリスト状態を確認
		{
			CString dbg;
			dbg.Format(_T("In-memory count=%d\r\n"), m_catList.GetCount());
			for (int j = 0; j < m_catList.GetCount(); j++) {
				dbg.AppendFormat(_T("%d: %s\r\n"), j, m_catList.Datas(j).name);
			}
			AfxMessageBox(dbg, MB_OK);
		}
		UpdateCategoryList();
		if (m_currentIndex >= 0 && m_currentIndex < m_ListCategory.GetCount()) {
			m_ListCategory.SetCurSel(m_currentIndex);
		}
		UpdateData(FALSE);
	}
}
