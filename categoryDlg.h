#pragma once
#include "afxdialogex.h"
#include "CategoryDataList.h"

// categoryDlg ダイアログ

class categoryDlg : public CDialogEx
{
	DECLARE_DYNAMIC(categoryDlg)

public:
	categoryDlg(CWnd* pParent = nullptr);   // 標準コンストラクター
	virtual ~categoryDlg();


    // CListBox m_listCat; (unused) removed to avoid confusion
	CString  m_strName;           // IDC_EDIT_CAT_NAME
	CString  m_strPath;           // IDC_EDIT_CAT_PATH

	virtual BOOL OnInitDialog();   // 初期化用

private:
	CCategoryDataList m_catList;
	CString m_iniPath;
	int m_currentIndex;

	void UpdateCategoryList();

	// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = ID_CATEGORY };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonCatUp();
	void SetIniPath(CString path) { m_iniPath = path; }
	afx_msg void OnBnClickedButtonCatDown();
	afx_msg void OnBnClickedButtonCatDel();
	afx_msg void OnLbnSelchangeListCategory();
	CListBox m_ListCategory;
	CEdit m_c_catName;
	CEdit m_c_catPath;
	afx_msg void OnBnClickedButtonCatSave();
};
