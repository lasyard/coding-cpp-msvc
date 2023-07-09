// ImageConverter.h : main header file for the IMAGECONVERTER application
//

#if !defined(AFX_IMAGECONVERTER_H__D6AAD1EC_0133_41E3_AFB9_E2C0D4742B61__INCLUDED_)
#define AFX_IMAGECONVERTER_H__D6AAD1EC_0133_41E3_AFB9_E2C0D4742B61__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h" // main symbols

/////////////////////////////////////////////////////////////////////////////
// CImageConverterApp:
// See ImageConverter.cpp for the implementation of this class
//

class CImageConverterApp : public CWinApp
{
public:
    CImageConverterApp();

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CImageConverterApp)
public:
    virtual BOOL InitInstance();
    //}}AFX_VIRTUAL

    // Implementation
    //{{AFX_MSG(CImageConverterApp)
    afx_msg void OnAppAbout();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMAGECONVERTER_H__D6AAD1EC_0133_41E3_AFB9_E2C0D4742B61__INCLUDED_)
