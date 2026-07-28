#pragma once

#include "ApiConfigDocument.h"
#include <afxcmn.h>

class CApiDetailDlg : public CDialog
{
public:
    enum { IDD = IDD_DIALOG1 };
    CApiDetailDlg(const CString& filePath, CWnd* parent = NULL);

protected:
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnAuthChanged();
    afx_msg void OnKeyAdd();
    afx_msg void OnKeyEdit();
    afx_msg void OnKeyDelete();
    afx_msg void OnActionAdd();
    afx_msg void OnActionEdit();
    afx_msg void OnActionDelete();
    afx_msg void OnActionDoubleClick(NMHDR* header, LRESULT* result);
    DECLARE_MESSAGE_MAP()

private:
    CString m_filePath;
    ApiConfigDocument m_document;
    BOOL m_existing;
    CStatic m_nameLabel;
    CStatic m_urlLabel;
    CStatic m_timeoutLabel;
    CStatic m_authLabel;
    CStatic m_credentialLabel;
    CStatic m_tokenLabel;
    CStatic m_usernameLabel;
    CStatic m_passwordLabel;
    CStatic m_keyLabel;
    CStatic m_actionLabel;
    CEdit m_nameEdit;
    CEdit m_urlEdit;
    CEdit m_timeoutEdit;
    CComboBox m_authCombo;
    CEdit m_credentialEdit;
    CEdit m_tokenEdit;
    CEdit m_usernameEdit;
    CEdit m_passwordEdit;
    CListCtrl m_keyList;
    CListCtrl m_actionList;
    CButton m_keyAdd;
    CButton m_keyEdit;
    CButton m_keyDelete;
    CButton m_actionAdd;
    CButton m_actionEdit;
    CButton m_actionDelete;
    CFont m_dialogFont;

    void HideTemplateControls();
    void RefreshKeyList();
    void RefreshActionList();
    void UpdateAuthControls();
    CString CurrentAuthType() const;
    bool SaveCredentials(CString& error);
};
