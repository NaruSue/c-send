#pragma once

#include "ApiConfigDocument.h"

class CApiActionDlg : public CDialog
{
public:
    CApiActionDlg(const ApiActionDocument& action, CWnd* parent = NULL);
    const ApiActionDocument& GetAction() const;

    enum { IDD = IDD_DIALOG1 };

protected:
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    DECLARE_MESSAGE_MAP()

private:
    ApiActionDocument m_action;
    CStatic m_nameLabel;
    CStatic m_idLabel;
    CStatic m_urlLabel;
    CStatic m_methodLabel;
    CStatic m_descriptionLabel;
    CStatic m_requestLabel;
    CStatic m_responseLabel;
    CEdit m_nameEdit;
    CEdit m_idEdit;
    CEdit m_urlEdit;
    CComboBox m_methodCombo;
    CEdit m_descriptionEdit;
    CEdit m_requestEdit;
    CEdit m_responseEdit;
    CFont m_dialogFont;

    void HideTemplateControls();
};
