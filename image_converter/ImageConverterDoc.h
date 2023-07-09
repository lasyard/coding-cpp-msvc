// ImageConverterDoc.h : interface of the CImageConverterDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGECONVERTERDOC_H__D5FCC17B_5EDB_474D_A2A4_23359CCA3B65__INCLUDED_)
#define AFX_IMAGECONVERTERDOC_H__D5FCC17B_5EDB_474D_A2A4_23359CCA3B65__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <jasper/jasper.h>

class CImageConverterDoc : public CDocument
{
protected: // create from serialization only
    CImageConverterDoc();
    DECLARE_DYNCREATE(CImageConverterDoc)

    // Attributes
public:
    // Operations
public:
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CImageConverterDoc)
public:
    virtual BOOL OnNewDocument();
    virtual void Serialize(CArchive &ar);
    virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
    virtual void OnCloseDocument();
    virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
    //}}AFX_VIRTUAL

    // Implementation
public:
    virtual ~CImageConverterDoc();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext &dc) const;
#endif

protected:
    // Generated message map functions
protected:
    //{{AFX_MSG(CImageConverterDoc)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

public:
    jas_image_t *m_image;
    int m_fmt;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMAGECONVERTERDOC_H__D5FCC17B_5EDB_474D_A2A4_23359CCA3B65__INCLUDED_)
