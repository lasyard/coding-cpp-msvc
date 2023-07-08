#pragma once

#include "CameraView.h"
#include "OutputWnd.h"

class CMainFrame : public CMDIFrameWndEx
{
public:
    CMainFrame();
    virtual ~CMainFrame();

    void selectCamViewItemByCamId(int camId);
    void setSelectedCamViewItemImage(int index);

protected:
    DECLARE_DYNAMIC(CMainFrame)
    DECLARE_MESSAGE_MAP()

    CMFCMenuBar m_wndMenuBar;
    CMFCToolBar m_wndToolBar;
    CMFCStatusBar m_wndStatusBar;
    CCameraView m_wndCamView;
    COutputWnd m_wndOutput;
    CMFCCaptionBar m_wndCaptionBar;

    HDEVNOTIFY m_hDevNotify;

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext &dc) const;
#endif

    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
    afx_msg void OnSetFocus(CWnd *pOldWnd);
    afx_msg void OnWindowManager();
    afx_msg void OnViewCaptionBar();
    afx_msg void OnUpdateViewCaptionBar(CCmdUI *pCmdUI);
    afx_msg BOOL OnDeviceChange(UINT nEventType, DWORD_PTR dwData);
    afx_msg void OnOpenCam();
    afx_msg void OnUpdateOpenCam(CCmdUI *pCmdUI);
    afx_msg void OnCloseCam();
    afx_msg void OnUpdateCloseCam(CCmdUI *pCmdUI);
    afx_msg void OnPlayCam();
    afx_msg void OnUpdatePlayCam(CCmdUI *pCmdUI);
    afx_msg void OnPauseCam();
    afx_msg void OnUpdatePauseCam(CCmdUI *pCmdUI);

    void showCaptionBar(LPCTSTR text);
    void showCaptionBar(UINT resId);

    CCamera *getSelectedCam();
};
