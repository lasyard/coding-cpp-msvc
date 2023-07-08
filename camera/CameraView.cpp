#include "stdafx.h"

#include <dshow.h>

#include "CameraView.h"

#include "Camera.h"
#include "CameraApp.h"
#include "CameraManager.h"
#include "MainFrm.h"
#include "OutputWnd.h"
#include "resource.h"

#define IDC_CAM_TREE 1

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CCameraView, CDockablePane)
ON_WM_CREATE()
ON_WM_SIZE()
ON_WM_CONTEXTMENU()
ON_WM_PAINT()
ON_WM_SETFOCUS()
ON_NOTIFY(TVN_SELCHANGED, IDC_CAM_TREE, &CCameraView::OnNotifySelChanged)
END_MESSAGE_MAP()

CCameraView::CCameraView()
{
}

CCameraView::~CCameraView()
{
}

void CCameraView::showCamList()
{
    m_wndCamView.DeleteAllItems();

    CString strTemp;
    strTemp.LoadString(IDS_LOCAL_CAM);
    HTREEITEM hRoot = m_wndCamView.InsertItem(strTemp, 0, 0);
    m_wndCamView.SetItemData(hRoot, (DWORD_PTR)-1);

    const list<CCamera *> &camList = theApp.m_pCamManager->getCamList();
    list<CCamera *>::const_iterator i;
    int count = 0;
    for (i = camList.begin(); i != camList.end(); ++i) {
        HTREEITEM hItem;
        if ((*i)->hasWindow()) {
            hItem = m_wndCamView.InsertItem((*i)->name, 2, 2, hRoot);
        } else {
            hItem = m_wndCamView.InsertItem((*i)->name, 1, 1, hRoot);
        }
        m_wndCamView.SetItemData(hItem, (DWORD_PTR)(*i)->id);
        count++;
    }
    if (count > 0) {
        m_wndCamView.Expand(hRoot, TVE_EXPAND);
    }
}

CCamera *CCameraView::getCamFromItem(HTREEITEM h)
{
    int camId = (int)m_wndCamView.GetItemData(h);
    if (camId < 0) {
        return NULL;
    }
    return theApp.m_pCamManager->findCamById(camId);
}

CCamera *CCameraView::getCamFromSelected()
{
    HTREEITEM hItem = m_wndCamView.GetSelectedItem();
    if (hItem == NULL) {
        return NULL;
    }
    return getCamFromItem(hItem);
}

HTREEITEM CCameraView::getItemByCamId(int camId)
{
    HTREEITEM hItem, h;
    hItem = m_wndCamView.GetRootItem();
    if (hItem == NULL) {
        return NULL;
    }
    for (;;) {
        if ((int)m_wndCamView.GetItemData(hItem) == camId) {
            return hItem;
        }
        h = m_wndCamView.GetChildItem(hItem);
        if (h == NULL) {
            for (;;) {
                h = m_wndCamView.GetNextSiblingItem(hItem);
                if (h != NULL) {
                    break;
                }
                h = m_wndCamView.GetParentItem(hItem);
                if (h == NULL) {
                    return NULL;
                }
                hItem = h;
            }
        }
        hItem = h;
    }
    return NULL;
}

void CCameraView::selectItem(HTREEITEM hItem)
{
    m_wndCamView.SelectItem(hItem);
}

void CCameraView::setSelectedItemImage(int index)
{
    HTREEITEM hItem = m_wndCamView.GetSelectedItem();
    if (hItem != NULL) {
        m_wndCamView.SetItemImage(hItem, index, index);
    }
}

BOOL CCameraView::OnCmdMsg(UINT nID, int nCode, void *pExtra, AFX_CMDHANDLERINFO *pHandlerInfo)
{
    if (GetParent()->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo)) {
        return TRUE;
    }
    return CDockablePane::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

int CCameraView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDockablePane::OnCreate(lpCreateStruct) == -1) {
        return -1;
    }

    CRect rectDummy;
    rectDummy.SetRectEmpty();

    if (!m_wndCamView.Create(
            WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS,
            rectDummy,
            this,
            IDC_CAM_TREE
        )) {
        ATLTRACE2("Failed to create cam view.\n");
        return -1;
    }

    m_CamViewImages.Create(IDB_CAM_VIEW, 16, 0, RGB(255, 0, 255));
    m_wndCamView.SetImageList(&m_CamViewImages, TVSIL_NORMAL);

    showCamList();

    return 0;
}

void CCameraView::OnSize(UINT nType, int cx, int cy)
{
    CDockablePane::OnSize(nType, cx, cy);
    m_wndCamView.SetWindowPos(NULL, -1, -1, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CCameraView::OnContextMenu(CWnd *pWnd, CPoint point)
{
    CTreeCtrl *pWndTree = &m_wndCamView;
    ASSERT_VALID(pWndTree);

    if (pWnd != pWndTree) {
        CDockablePane::OnContextMenu(pWnd, point);
        return;
    }

    if (point != CPoint(-1, -1)) {
        CPoint pt = point;
        pWndTree->ScreenToClient(&pt);
        UINT flags = 0;
        HTREEITEM hTreeItem = pWndTree->HitTest(pt, &flags);
        if (hTreeItem != NULL && (TVHT_ONITEM & flags)) {
            pWndTree->SelectItem(hTreeItem);
        }
    }

    pWndTree->SetFocus();
    theApp.GetContextMenuManager()->ShowPopupMenu(IDR_CAM_VIEW, point.x, point.y, this, TRUE);
}

void CCameraView::OnPaint()
{
    CPaintDC dc(this);

    CRect rectTree;
    m_wndCamView.GetWindowRect(rectTree);
    ScreenToClient(rectTree);

    rectTree.InflateRect(1, 1);
    dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CCameraView::OnSetFocus(CWnd *pOldWnd)
{
    CDockablePane::OnSetFocus(pOldWnd);
    m_wndCamView.SetFocus();
}

void CCameraView::OnNotifySelChanged(NMHDR *pNotifyStruct, LRESULT *result)
{
    LPNMTREEVIEW pNm = (LPNMTREEVIEW)pNotifyStruct;
    if (pNm == NULL) {
        return;
    }
    TVITEM tvNew = pNm->itemNew;
    TVITEM tvOld = pNm->itemOld;
    if (tvOld.hItem != NULL) {
        m_wndCamView.SetItemState(tvOld.hItem, 0, TVIS_BOLD);
    }
    if (tvNew.hItem != NULL) {
        m_wndCamView.SetItemState(tvNew.hItem, TVIS_BOLD, TVIS_BOLD);
        CCamera *pCam = getCamFromItem(tvNew.hItem);
        if (pCam != NULL) {
            pCam->setWindowFocus();
        }
    }
    *result = S_OK;
}
