// ConvertDialog.h : header file
//

#if !defined(AFX_CONVERTDIALOG_H__149AF545_DC67_444E_80DC_2A96793A9EE7__INCLUDED_)
#define AFX_CONVERTDIALOG_H__149AF545_DC67_444E_80DC_2A96793A9EE7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CConvertDialog dialog

class CConvertDialog : public CDialog
{
    // Construction
public:
    CConvertDialog(CWnd *pParent = NULL); // standard constructor

    // Dialog Data
    //{{AFX_DATA(CConvertDialog)
    enum { IDD = IDD_CONVERT };

    CComboBox m_fmt_lst;
    CString m_fmtdesc;
    //}}AFX_DATA

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CConvertDialog)
protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    //}}AFX_VIRTUAL

    // Implementation
protected:
    // Generated message map functions
    //{{AFX_MSG(CConvertDialog)
    virtual BOOL OnInitDialog();
    afx_msg void OnSelchangeFmtLst();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

public:
    CString m_caption;
    int m_fmt;
    int m_sel;

    void set_current_fmt(const int fmt);
    void set_caption(LPCTSTR s);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONVERTDIALOG_H__149AF545_DC67_444E_80DC_2A96793A9EE7__INCLUDED_)
