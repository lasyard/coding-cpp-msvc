#include "stdafx.h"

#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CChildView, CWnd)
ON_WM_PAINT()
END_MESSAGE_MAP()

CChildView::CChildView()
{
}

CChildView::~CChildView()
{
}

BOOL CChildView::PreCreateWindow(CREATESTRUCT &cs)
{
    if (!CWnd::PreCreateWindow(cs)) {
        return FALSE;
    }
    cs.dwExStyle |= WS_EX_CLIENTEDGE;
    cs.style &= ~WS_BORDER;
    cs.lpszClass = AfxRegisterWndClass(
        CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
        ::LoadCursor(NULL, IDC_ARROW),
        reinterpret_cast<HBRUSH>(COLOR_BACKGROUND + 1),
        NULL
    );
    return TRUE;
}

void CChildView::OnPaint()
{
    CPaintDC dc(this);
}
