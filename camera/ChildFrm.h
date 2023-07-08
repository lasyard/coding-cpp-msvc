#pragma once

#include "ChildView.h"

class CCamera;

class CChildFrame : public CMDIChildWndEx
{
public:
    CChildFrame();
    virtual ~CChildFrame();

    CCamera *m_pCam;

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext &dc) const;
#endif

protected:
    DECLARE_DYNCREATE(CChildFrame)
    DECLARE_MESSAGE_MAP()

    CChildView m_wndView;

    virtual BOOL PreCreateWindow(CREATESTRUCT &cs);

    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnDestroy();
    afx_msg void OnSetFocus(CWnd *pOldWnd);
    afx_msg void OnMDIActivate(BOOL bActivate, CWnd *pActivateWnd, CWnd *pDeactivateWnd);
};
