#pragma once

class COutputWnd : public CDockablePane
{
public:
    COutputWnd();
    virtual ~COutputWnd();

    COutputWnd &operator<<(LPCTSTR str);

protected:
    DECLARE_MESSAGE_MAP()

    CFont m_Font;
    int m_extMax;

    CListBox m_wndOutput;

    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
};
