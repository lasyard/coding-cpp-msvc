#include "stdafx.h"

#include <dshow.h>

#include "ChildFrm.h"

#include "Camera.h"
#include "MainFrm.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWndEx)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWndEx)
ON_WM_CREATE()
ON_WM_SIZE()
ON_WM_DESTROY()
ON_WM_SETFOCUS()
ON_WM_MDIACTIVATE()
END_MESSAGE_MAP()

CChildFrame::CChildFrame() : m_pCam(NULL)
{
}

CChildFrame::~CChildFrame()
{
}

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
    CMDIChildWndEx::AssertValid();
}

void CChildFrame::Dump(CDumpContext &dc) const
{
    CMDIChildWndEx::Dump(dc);
}
#endif

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT &cs)
{
    if (!CMDIChildWndEx::PreCreateWindow(cs)) {
        return FALSE;
    }
    cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
    cs.lpszClass =
        AfxRegisterWndClass(0, NULL, NULL, ::LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_CHILDFRAME)));
    return TRUE;
}

int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CMDIChildWndEx::OnCreate(lpCreateStruct) == -1) {
        return -1;
    }
    if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, rectDefault, this, AFX_IDW_PANE_FIRST, NULL)) {
        ATLTRACE2("Failed to create view window.\n");
        return -1;
    }
    return 0;
}

void CChildFrame::OnSize(UINT nType, int cx, int cy)
{
    CMDIChildWndEx::OnSize(nType, cx, cy);
    if (nType != SIZE_MINIMIZED && m_pCam != NULL) {
        m_pCam->ajustWindowPos(cx, cy);
    }
}

void CChildFrame::OnDestroy()
{
    if (m_pCam != NULL) {
        CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
        pMain->selectCamViewItemByCamId(m_pCam->id);
        pMain->setSelectedCamViewItemImage(1);
        m_pCam->endCapture();
        m_pCam->setWindow(NULL);
        m_pCam = NULL;
    }
}

void CChildFrame::OnSetFocus(CWnd *pOldWnd)
{
    CMDIChildWndEx::OnSetFocus(pOldWnd);
    m_wndView.SetFocus();
}

void CChildFrame::OnMDIActivate(BOOL bActivate, CWnd *pActivateWnd, CWnd *pDeactivateWnd)
{
    CMDIChildWndEx::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);
    if (!bActivate) {
        return;
    }
    if (m_pCam != NULL) {
        CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
        pMain->selectCamViewItemByCamId(m_pCam->id);
    }
}
