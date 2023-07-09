// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__27CDA58C_DE33_47A8_BD00_44DD188775CC__INCLUDED_)
#define AFX_MAINFRM_H__27CDA58C_DE33_47A8_BD00_44DD188775CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <jasper/jasper.h>

class CMainFrame : public CMDIFrameWnd
{
    DECLARE_DYNAMIC(CMainFrame)
public:
    CMainFrame();

    // Attributes
public:
    // Operations
public:
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CMainFrame)
    virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
    //}}AFX_VIRTUAL

    // Implementation
public:
    virtual ~CMainFrame();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext &dc) const;
#endif

protected: // control bar embedded members
    CStatusBar m_wndStatusBar;
    CToolBar m_wndToolBar;

    // Generated message map functions
protected:
    //{{AFX_MSG(CMainFrame)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnUpdateConvert(CCmdUI *pCmdUI);
    afx_msg void OnConvert();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

private:
    int convert_image(jas_image_t *const image, LPCTSTR file, const int fmt);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__27CDA58C_DE33_47A8_BD00_44DD188775CC__INCLUDED_)
