#pragma once

class CChildView : public CWnd
{
public:
    CChildView();
    virtual ~CChildView();

protected:
    DECLARE_MESSAGE_MAP()

    virtual BOOL PreCreateWindow(CREATESTRUCT &cs);

    afx_msg void OnPaint();
};
