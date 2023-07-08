#pragma once

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

class CCameraManager;

class CCameraApp : public CWinAppEx
{
public:
    CCameraApp();
    virtual ~CCameraApp();

    CCameraManager *m_pCamManager;

    virtual BOOL InitInstance();
    virtual void PreLoadState();
    virtual void LoadCustomState();
    virtual void SaveCustomState();

protected:
    DECLARE_MESSAGE_MAP()

    afx_msg void OnAppAbout();
};

extern CCameraApp theApp;
