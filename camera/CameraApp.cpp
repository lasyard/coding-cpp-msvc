#include "stdafx.h"

#include <dshow.h>

#include "CameraApp.h"

#include "AboutDlg.h"
#include "CameraManager.h"
#include "CameraView.h"
#include "MainFrm.h"
#include "OutputWnd.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCameraApp theApp;

BEGIN_MESSAGE_MAP(CCameraApp, CWinAppEx)
ON_COMMAND(ID_APP_ABOUT, &CCameraApp::OnAppAbout)
END_MESSAGE_MAP()

CCameraApp::CCameraApp() : CWinAppEx(TRUE), m_pCamManager(NULL)
{
}

CCameraApp::~CCameraApp()
{
    if (m_pCamManager != NULL) {
        delete m_pCamManager;
    }
}

BOOL CCameraApp::InitInstance()
{
    // InitCommonControlsEx() is required on Windows XP if an application
    // manifest specifies use of ComCtl32.dll version 6 or later to enable
    // visual styles.  Otherwise, any window creation will fail.
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    // Set this to include all the common control classes you want to use
    // in your application.
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CWinAppEx::InitInstance();

    m_pCamManager = new CCameraManager;

    if (!AfxSocketInit()) {
        AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
        return FALSE;
    }

    if (!AfxOleInit()) {
        AfxMessageBox(IDP_OLE_INIT_FAILED);
        return FALSE;
    }
    AfxEnableControlContainer();

    // Change the registry key under which our settings are stored
    // TODO: You should modify this string to be something appropriate
    // such as the name of your company or organization
    SetRegistryKey(_T("lasyard"));

    InitContextMenuManager();
    InitKeyboardManager();
    InitTooltipManager();

    CMFCToolTipInfo ttParams;
    ttParams.m_bVislManagerTheme = TRUE;
    theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL, RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

    CMainFrame *pFrame = new CMainFrame;
    if (!pFrame) {
        return FALSE;
    }
    m_pMainWnd = pFrame;

    // create and load the frame with its resources
    pFrame->LoadFrame(IDR_MAINFRAME, WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL, NULL);
    HINSTANCE hInst = AfxGetResourceHandle();
    pFrame->ShowWindow(SW_SHOW);
    pFrame->UpdateWindow();

    return TRUE;
}

void CCameraApp::PreLoadState()
{
    CString strName;
    strName.LoadString(IDR_CAM_VIEW);
    GetContextMenuManager()->AddMenu(strName, IDR_CAM_VIEW);
}

void CCameraApp::LoadCustomState()
{
}

void CCameraApp::SaveCustomState()
{
}

void CCameraApp::OnAppAbout()
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}
