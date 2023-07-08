#include "stdafx.h"

#include "OutputWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(COutputWnd, CDockablePane)
ON_WM_CREATE()
ON_WM_SIZE()
END_MESSAGE_MAP()

COutputWnd::COutputWnd() : m_extMax(0)
{
}

COutputWnd::~COutputWnd()
{
}

COutputWnd &COutputWnd::operator<<(LPCTSTR str)
{
    CClientDC dc(this);
    m_wndOutput.AddString(str);
    m_extMax = max(m_extMax, dc.GetTextExtent(str).cx);
    m_wndOutput.SetHorizontalExtent(m_extMax);
    return *this;
}

int COutputWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDockablePane::OnCreate(lpCreateStruct) == -1) {
        return -1;
    }

    m_Font.CreateStockObject(DEFAULT_GUI_FONT);

    CRect rectDummy;
    rectDummy.SetRectEmpty();

    if (!m_wndOutput
             .Create(LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL, rectDummy, this, 1)) {
        ATLTRACE2("Failed to create output list.\n");
        return -1;
    }
    m_wndOutput.SetFont(&m_Font);

    return 0;
}

void COutputWnd::OnSize(UINT nType, int cx, int cy)
{
    CDockablePane::OnSize(nType, cx, cy);
    m_wndOutput.SetWindowPos(NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}
