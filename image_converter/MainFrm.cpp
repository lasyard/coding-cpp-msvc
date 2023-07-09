// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"

#include "MainFrm.h"

#include "ChildFrm.h"
#include "ConvertDialog.h"
#include "ImageConverter.h"
#include "ImageConverterDoc.h"
#include "ImageConverterView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
//{{AFX_MSG_MAP(CMainFrame)
ON_WM_CREATE()
ON_UPDATE_COMMAND_UI(ID_CONVERT, OnUpdateConvert)
ON_COMMAND(ID_CONVERT, OnConvert)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] = {
    ID_SEPARATOR, // status line indicator
    ID_INDICATOR_CAPS,
    ID_INDICATOR_NUM,
    ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1) {
        return -1;
    }

    if (!m_wndToolBar.CreateEx(
            this,
            TBSTYLE_FLAT,
            WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC
        ) ||
        !m_wndToolBar.LoadToolBar(IDR_MAINFRAME)) {
        TRACE0("Failed to create toolbar\n");
        return -1; // fail to create
    }

    if (!m_wndStatusBar.Create(this) || !m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT))) {
        TRACE0("Failed to create status bar\n");
        return -1; // fail to create
    }

    m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
    EnableDocking(CBRS_ALIGN_ANY);
    DockControlBar(&m_wndToolBar);

    if (jas_init()) {
        AfxMessageBox(_T("Init jasper image lib failed!"));
        PostQuitMessage(1);
    }
    size_t num_formats = jas_image_getnumfmts();
    for (int i = 0; i < num_formats; ++i) {
        const jas_image_fmtinfo_t *fmt;
        fmt = jas_image_getfmtbyind(i);
        TRACE("Support image format: %s\n", fmt->name);
    }

    return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT &cs)
{
    if (!CMDIFrameWnd::PreCreateWindow(cs)) {
        return FALSE;
    }
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
    CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext &dc) const
{
    CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnUpdateConvert(CCmdUI *pCmdUI)
{
    // TODO: Add your command update UI handler code here
    BOOL bMaximized;
    pCmdUI->Enable((int)(MDIGetActive(&bMaximized) != NULL));
}

void CMainFrame::OnConvert()
{
    // TODO: Add your command handler code here
    BOOL bMaximized;
    CConvertDialog cvrt_dlg;
    CChildFrame *pChildFrame = (CChildFrame *)MDIGetActive(&bMaximized);
    ASSERT(pChildFrame != NULL);
    CImageConverterView *pImageConverterView = (CImageConverterView *)(pChildFrame->GetActiveView());
    ASSERT(pImageConverterView != NULL);
    CImageConverterDoc *pImageConverterDoc = (CImageConverterDoc *)(pImageConverterView->GetDocument());
    ASSERT(pImageConverterDoc != NULL);
    cvrt_dlg.set_current_fmt(pImageConverterDoc->m_fmt);
    CString path_name = pImageConverterDoc->GetPathName();
    cvrt_dlg.set_caption(path_name);
    if (cvrt_dlg.DoModal() == IDOK) {
        int fmt = cvrt_dlg.m_sel;
        const jas_image_fmtinfo_t *fmtinfo = jas_image_lookupfmtbyid(fmt);
        int index = path_name.ReverseFind('.');
        if (index > 0)
            path_name = path_name.Left(index);
        path_name += '.';
        path_name += fmtinfo->ext;
        if (convert_image(pImageConverterDoc->m_image, path_name, fmt)) {
            AfxMessageBox(_T("Convert image failed!"));
        } else {
            POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();
            CDocTemplate *pDocTemplate = AfxGetApp()->GetNextDocTemplate(pos);
            pDocTemplate->OpenDocumentFile(path_name);
        }
    }
}

int CMainFrame::convert_image(jas_image_t *const image, LPCTSTR file, const int fmt)
{
    jas_stream_t *out_file;
    if (!(out_file = jas_stream_fopen(CT2A(file), "w+b"))) {
        return -1;
    }
    if (jas_image_encode(image, out_file, fmt, NULL)) {
        jas_stream_close(out_file);
        return -1;
    }
    jas_stream_close(out_file);
    return 0;
}
