#pragma once

#include <afxcmn.h>
#include <vector>

class CApiListDlg : public CDialog
{
public:
    enum { IDD = IDD_DIALOG1 };
    explicit CApiListDlg(CWnd* parent = NULL);

protected:
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnNewApi();
    afx_msg void OnOpenDetail();
    afx_msg void OnDeleteApi();
    afx_msg void OnDoubleClick(NMHDR* header, LRESULT* result);
    afx_msg BOOL OnEraseBkgnd(CDC* dc);
    DECLARE_MESSAGE_MAP()

private:
    CListCtrl m_list;
    CButton m_newButton;
    CButton m_detailButton;
    CButton m_deleteButton;
    CFont m_dialogFont;
    CString m_apiDirectory;
    std::vector<CString> m_filePaths;
    void LoadApiFiles();
    CString GetSelectedFilePath() const;
};
