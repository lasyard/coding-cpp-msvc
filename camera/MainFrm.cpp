#include "stdafx.h"

#include <dbt.h>
#include <dshow.h>

#include "MainFrm.h"

#include "Camera.h"
#include "CameraApp.h"
#include "CameraManager.h"
#include "ChildFrm.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
ON_WM_CREATE()
ON_WM_DESTROY()
ON_WM_SETFOCUS()
ON_COMMAND(ID_WINDOW_MANAGER, &CMainFrame::OnWindowManager)
ON_COMMAND(ID_VIEW_CAPTION_BAR, &CMainFrame::OnViewCaptionBar)
ON_UPDATE_COMMAND_UI(ID_VIEW_CAPTION_BAR, &CMainFrame::OnUpdateViewCaptionBar)
ON_WM_DEVICECHANGE()
ON_COMMAND(ID_OPEN_CAM, &CMainFrame::OnOpenCam)
ON_UPDATE_COMMAND_UI(ID_OPEN_CAM, &CMainFrame::OnUpdateOpenCam)
ON_COMMAND(ID_CLOSE_CAM, &CMainFrame::OnCloseCam)
ON_UPDATE_COMMAND_UI(ID_CLOSE_CAM, &CMainFrame::OnUpdateCloseCam)
ON_COMMAND(ID_PLAY_CAM, &CMainFrame::OnPlayCam)
ON_UPDATE_COMMAND_UI(ID_PLAY_CAM, &CMainFrame::OnUpdatePlayCam)
ON_COMMAND(ID_PAUSE_CAM, &CMainFrame::OnPauseCam)
ON_UPDATE_COMMAND_UI(ID_PAUSE_CAM, &CMainFrame::OnUpdatePauseCam)
END_MESSAGE_MAP()

static UINT indicators[] = {
    ID_SEPARATOR,
};

CMainFrame::CMainFrame() : m_hDevNotify(NULL)
{
}

CMainFrame::~CMainFrame()
{
}

void CMainFrame::selectCamViewItemByCamId(int camId)
{
    HTREEITEM hItem = m_wndCamView.getItemByCamId(camId);
    if (hItem != NULL) {
        m_wndCamView.selectItem(hItem);
    }
}

void CMainFrame::setSelectedCamViewItemImage(int index)
{
    m_wndCamView.setSelectedItemImage(index);
}

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
    CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext &dc) const
{
    CMDIFrameWndEx::Dump(dc);
}
#endif

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1) {
        return -1;
    }

    CString strTemp, strTemp1;

    CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
    EnableDocking(CBRS_ALIGN_ANY);
    CDockingManager::SetDockingMode(DT_SMART);
    EnableAutoHidePanes(CBRS_ALIGN_ANY);

    if (!m_wndMenuBar.Create(this)) {
        ATLTRACE2("Failed to create menubar.\n");
        return -1;
    }
    m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);
    m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
    DockPane(&m_wndMenuBar);
    // prevent the menu bar from taking the focus on activation
    CMFCPopupMenu::SetForceMenuFocus(FALSE);

    if (!m_wndToolBar.CreateEx(
            this,
            TBSTYLE_FLAT,
            WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC
        ) ||
        !m_wndToolBar.LoadToolBar(IDR_MAINFRAME)) {
        ATLTRACE2("Failed to create toolbar.\n");
        return -1;
    }
    strTemp.LoadString(IDS_TOOLBAR_STANDARD);
    m_wndToolBar.SetWindowText(strTemp);
    m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
    DockPane(&m_wndToolBar);

    if (!m_wndStatusBar.Create(this)) {
        ATLTRACE2("Failed to create status bar.\n");
        return -1;
    }
    m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT));

    if (!m_wndCaptionBar.Create(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, this, ID_VIEW_CAPTION_BAR, -1, TRUE)) {
        ATLTRACE2("Failed to create caption bar.\n");
        return -1;
    }
    m_wndCaptionBar.SetBitmap(IDB_INFO, RGB(255, 255, 255), FALSE, CMFCCaptionBar::ALIGN_LEFT);
    strTemp.LoadString(IDS_CAPTION_IMAGE_TIP);
    strTemp1.LoadString(IDS_CAPTION_IMAGE_TEXT);
    m_wndCaptionBar.SetImageToolTip(strTemp, strTemp1);

    strTemp.LoadString(IDS_CAM_VIEW);
    if (!m_wndCamView.Create(
            strTemp,
            this,
            CRect(0, 0, 200, 200),
            TRUE,
            ID_VIEW_CAM_VIEW,
            WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI
        )) {
        ATLTRACE2("Failed to create Cam View window.\n");
        return -1;
    }
    HICON hCamViewIcon = (HICON)::LoadImage(
        ::AfxGetResourceHandle(),
        MAKEINTRESOURCE(IDI_CAM_VIEW),
        IMAGE_ICON,
        ::GetSystemMetrics(SM_CXSMICON),
        ::GetSystemMetrics(SM_CYSMICON),
        0
    );
    m_wndCamView.SetIcon(hCamViewIcon, FALSE);
    m_wndCamView.EnableDocking(CBRS_ALIGN_ANY);
    DockPane(&m_wndCamView);

    strTemp.LoadString(IDS_OUTPUT_WND);
    if (!m_wndOutput.Create(
            strTemp,
            this,
            CRect(0, 0, 100, 100),
            TRUE,
            ID_VIEW_OUTPUTWND,
            WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI
        )) {
        ATLTRACE2("Failed to create Output window.\n");
        return -1;
    }
    HICON hOutputBarIcon = (HICON)::LoadImage(
        ::AfxGetResourceHandle(),
        MAKEINTRESOURCE(IDI_OUTPUT_WND),
        IMAGE_ICON,
        ::GetSystemMetrics(SM_CXSMICON),
        ::GetSystemMetrics(SM_CYSMICON),
        0
    );
    m_wndOutput.SetIcon(hOutputBarIcon, FALSE);
    m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
    DockPane(&m_wndOutput);

    EnableWindowsDialog(ID_WINDOW_MANAGER, IDS_WINDOWS_MANAGER, TRUE);
    // enable toolbar and docking window menu replacement
    EnablePaneMenu(TRUE, 0, _T(""), ID_VIEW_TOOLBAR);

    DEV_BROADCAST_DEVICEINTERFACE dev;
    ZeroMemory(&dev, sizeof(DEV_BROADCAST_DEVICEINTERFACE));
    dev.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    dev.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    dev.dbcc_classguid = AM_KSCATEGORY_CAPTURE;
    if ((m_hDevNotify = RegisterDeviceNotification(m_hWnd, &dev, DEVICE_NOTIFY_WINDOW_HANDLE)) == NULL) {
        ATLTRACE2("RegisterDeviceNotification failed.\n");
        return -1;
    }

    showCaptionBar(IDS_WELCOME);
    return 0;
}

void CMainFrame::OnDestroy()
{
    if (m_hDevNotify != NULL) {
        UnregisterDeviceNotification(m_hDevNotify);
    }
}

void CMainFrame::OnSetFocus(CWnd *pOldWnd)
{
}

void CMainFrame::OnWindowManager()
{
    ShowWindowsDialog();
}

void CMainFrame::OnViewCaptionBar()
{
    m_wndCaptionBar.ShowWindow(m_wndCaptionBar.IsVisible() ? SW_HIDE : SW_SHOW);
    RecalcLayout(FALSE);
}

void CMainFrame::OnUpdateViewCaptionBar(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(m_wndCaptionBar.IsVisible());
}

BOOL CMainFrame::OnDeviceChange(UINT nEventType, DWORD_PTR dwData)
{
    if (nEventType != DBT_DEVICEARRIVAL && nEventType != DBT_DEVICEREMOVECOMPLETE) {
        return TRUE;
    }
    PDEV_BROADCAST_HDR pHdr = (PDEV_BROADCAST_HDR)dwData;
    if (pHdr->dbch_devicetype != DBT_DEVTYP_DEVICEINTERFACE) {
        return TRUE;
    }
    PDEV_BROADCAST_DEVICEINTERFACE pDev = (PDEV_BROADCAST_DEVICEINTERFACE)dwData;
    if (pDev->dbcc_classguid != AM_KSCATEGORY_CAPTURE) {
        return TRUE;
    }
    CString temp;
    if (nEventType == DBT_DEVICEARRIVAL) {
        showCaptionBar(IDS_NEW_CAM_COME);
    } else {
        showCaptionBar(IDS_CAM_LEAVE);
    }
    theApp.m_pCamManager->notifyDevChanged();
    m_wndCamView.showCamList();
    return TRUE;
}

void CMainFrame::OnOpenCam()
{
    CCamera *pCam = getSelectedCam();
    if (pCam == NULL) {
        return;
    }
    if (!pCam->hasWindow()) {
        CChildFrame *pChild = new CChildFrame;
        if (!pChild->Create(NULL, pCam->name, WS_CHILD | WS_VISIBLE | WS_OVERLAPPEDWINDOW, rectDefault, this, NULL)) {
            ATLTRACE2("Failed to create child window.\n");
            return;
        }
        pChild->m_pCam = pCam;
        pCam->setWindow(pChild->m_hWnd);
    }
    pCam->startCapture();
    pCam->setWindowFocus();
    m_wndCamView.setSelectedItemImage(2);
}

void CMainFrame::OnUpdateOpenCam(CCmdUI *pCmdUI)
{
    CCamera *pCam = getSelectedCam();
    if (pCam != NULL && !pCam->hasWindow()) {
        pCmdUI->Enable(TRUE);
    } else {
        pCmdUI->Enable(FALSE);
    }
}

void CMainFrame::OnCloseCam()
{
    CCamera *pCam = getSelectedCam();
    if (pCam == NULL) {
        return;
    }
    if (pCam->hasWindow()) {
        pCam->closeWindow();
    }
}

void CMainFrame::OnUpdateCloseCam(CCmdUI *pCmdUI)
{
    CCamera *pCam = getSelectedCam();
    if (pCam != NULL && pCam->hasWindow()) {
        pCmdUI->Enable(TRUE);
    } else {
        pCmdUI->Enable(FALSE);
    }
}

void CMainFrame::OnPlayCam()
{
    OnOpenCam();
}

void CMainFrame::OnUpdatePlayCam(CCmdUI *pCmdUI)
{
    CCamera *pCam = getSelectedCam();
    if (pCam != NULL && pCam->hasWindow() && !pCam->isRunning()) {
        pCmdUI->Enable(TRUE);
    } else {
        pCmdUI->Enable(FALSE);
    }
}

void CMainFrame::OnPauseCam()
{
    CCamera *pCam = getSelectedCam();
    if (pCam == NULL) {
        return;
    }
    pCam->stopCapture();
}

void CMainFrame::OnUpdatePauseCam(CCmdUI *pCmdUI)
{
    CCamera *pCam = getSelectedCam();
    if (pCam != NULL && pCam->hasWindow() && pCam->isRunning()) {
        pCmdUI->Enable(TRUE);
    } else {
        pCmdUI->Enable(FALSE);
    }
}

void CMainFrame::showCaptionBar(LPCTSTR text)
{
    m_wndCaptionBar.SetText(text, CMFCCaptionBar::ALIGN_LEFT);
    if (!m_wndCaptionBar.IsVisible()) {
        m_wndCaptionBar.ShowWindow(SW_SHOW);
        RecalcLayout(FALSE);
    }
    m_wndOutput << text;
}

void CMainFrame::showCaptionBar(UINT resId)
{
    CString temp;
    temp.LoadString(resId);
    showCaptionBar(temp);
}

CCamera *CMainFrame::getSelectedCam()
{
    return m_wndCamView.getCamFromSelected();
}
