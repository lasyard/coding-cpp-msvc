// ImageConverterDoc.cpp : implementation of the CImageConverterDoc class
//

#include "stdafx.h"

#include "ImageConverterDoc.h"

#include "ImageConverter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImageConverterDoc

IMPLEMENT_DYNCREATE(CImageConverterDoc, CDocument)

BEGIN_MESSAGE_MAP(CImageConverterDoc, CDocument)
//{{AFX_MSG_MAP(CImageConverterDoc)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageConverterDoc construction/destruction

CImageConverterDoc::CImageConverterDoc() : m_image(NULL), m_fmt(-1)
{
}

CImageConverterDoc::~CImageConverterDoc()
{
}

BOOL CImageConverterDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument()) {
        return FALSE;
    }
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CImageConverterDoc serialization

void CImageConverterDoc::Serialize(CArchive &ar)
{
    if (ar.IsStoring()) {
    } else {
    }
}

/////////////////////////////////////////////////////////////////////////////
// CImageConverterDoc diagnostics

#ifdef _DEBUG
void CImageConverterDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CImageConverterDoc::Dump(CDumpContext &dc) const
{
    CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CImageConverterDoc commands

BOOL CImageConverterDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
    if (!CDocument::OnOpenDocument(lpszPathName)) {
        return FALSE;
    }

    // TODO: Add your specialized creation code here
    jas_stream_t *in_file;
    if (!(in_file = jas_stream_fopen(CT2A(lpszPathName), "rb"))) {
        AfxMessageBox(_T("Cannot open file!"));
        return FALSE;
    }
    if ((m_fmt = jas_image_getfmt(in_file)) < 0) {
        AfxMessageBox(_T("Unknow image format!"));
        return FALSE;
    }
    if (!(m_image = jas_image_decode(in_file, m_fmt, NULL))) {
        AfxMessageBox(_T("Decoding image failed!"));
        return FALSE;
    }
    jas_stream_close(in_file);

    return TRUE;
}

void CImageConverterDoc::OnCloseDocument()
{
    // TODO: Add your specialized code here and/or call the base class
    if (m_image) {
        jas_image_destroy(m_image);
    }

    CDocument::OnCloseDocument();
}

BOOL CImageConverterDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
    // TODO: Add your specialized code here and/or call the base class
    jas_stream_t *out_file;
    if (!(out_file = jas_stream_fopen(CT2A(lpszPathName), "w+b"))) {
        return FALSE;
    }
    if (jas_image_encode(m_image, out_file, m_fmt, NULL)) {
        jas_stream_close(out_file);
        return FALSE;
    }
    jas_stream_close(out_file);
    return TRUE;
    return CDocument::OnSaveDocument(lpszPathName);
}
