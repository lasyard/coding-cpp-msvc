// ImageConverterView.h : interface of the CImageConverterView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGECONVERTERVIEW_H__D90ABB84_1A18_4881_B522_916BFEB47610__INCLUDED_)
#define AFX_IMAGECONVERTERVIEW_H__D90ABB84_1A18_4881_B522_916BFEB47610__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <GL/gl.h>
#include <GL/glu.h>

#include "ImageConverterDoc.h"

class CImageConverterView : public CView
{
protected: // create from serialization only
    CImageConverterView();
    DECLARE_DYNCREATE(CImageConverterView)

    // Attributes
public:
    CImageConverterDoc *GetDocument();

    // Operations
public:
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CImageConverterView)
public:
    virtual void OnDraw(CDC *pDC); // overridden to draw this view
    virtual BOOL PreCreateWindow(CREATESTRUCT &cs);

protected:
    virtual void OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint);
    //}}AFX_VIRTUAL

    // Implementation
public:
    virtual ~CImageConverterView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext &dc) const;
#endif

protected:
    // Generated message map functions
protected:
    //{{AFX_MSG(CImageConverterView)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg BOOL OnEraseBkgnd(CDC *pDC);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

private:
    HDC m_hdc;
    HGLRC m_hrc;
    GLshort *m_data;
    int m_vw, m_vh;

    GLshort *render_image(jas_image_t *image);
    void change_view(const int cx, const int cy);
};

#ifndef _DEBUG // debug version in ImageConverterView.cpp
inline CImageConverterDoc *CImageConverterView::GetDocument()
{
    return (CImageConverterDoc *)m_pDocument;
}
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMAGECONVERTERVIEW_H__D90ABB84_1A18_4881_B522_916BFEB47610__INCLUDED_)
