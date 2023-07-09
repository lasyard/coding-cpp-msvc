// ImageConverterView.cpp : implementation of the CImageConverterView class
//

#include "stdafx.h"

#include "ImageConverterView.h"

#include "ImageConverter.h"
#include "ImageConverterDoc.h"

#pragma comment(lib, "opengl32")
#pragma comment(lib, "glu32")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImageConverterView

IMPLEMENT_DYNCREATE(CImageConverterView, CView)

BEGIN_MESSAGE_MAP(CImageConverterView, CView)
//{{AFX_MSG_MAP(CImageConverterView)
ON_WM_CREATE()
ON_WM_DESTROY()
ON_WM_SIZE()
ON_WM_ERASEBKGND()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageConverterView construction/destruction

CImageConverterView::CImageConverterView() : m_data(NULL)
{
}

CImageConverterView::~CImageConverterView()
{
    if (m_data) {
        free(m_data);
    }
}

BOOL CImageConverterView::PreCreateWindow(CREATESTRUCT &cs)
{
    return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CImageConverterView drawing

void CImageConverterView::OnDraw(CDC *pDC)
{
    CImageConverterDoc *pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    wglMakeCurrent(m_hdc, m_hrc);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawPixels(m_vw, m_vh, GL_RGBA, GL_UNSIGNED_SHORT, m_data);
    glFlush();
    SwapBuffers(m_hdc);
}

/////////////////////////////////////////////////////////////////////////////
// CImageConverterView diagnostics

#ifdef _DEBUG
void CImageConverterView::AssertValid() const
{
    CView::AssertValid();
}

void CImageConverterView::Dump(CDumpContext &dc) const
{
    CView::Dump(dc);
}

CImageConverterDoc *CImageConverterView::GetDocument() // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CImageConverterDoc)));
    return (CImageConverterDoc *)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CImageConverterView message handlers

int CImageConverterView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CView::OnCreate(lpCreateStruct) == -1) {
        return -1;
    }

    // TODO: Add your specialized creation code here
    PIXELFORMATDESCRIPTOR pfd;
    m_hdc = GetDC()->GetSafeHdc();
    /* set pixel format and create opengl rendering context */
    memset(&pfd, 0, sizeof(pfd));
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_GENERIC_ACCELERATED;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 32;
    SetPixelFormat(m_hdc, ChoosePixelFormat(m_hdc, &pfd), &pfd);
    m_hrc = wglCreateContext(m_hdc);
    /* set clear color */
    wglMakeCurrent(m_hdc, m_hrc);
    glClearColor(0.0, 0.0, 0.0, 1.0);

    return 0;
}

void CImageConverterView::OnDestroy()
{
    CView::OnDestroy();

    // TODO: Add your message handler code here
    ::ReleaseDC(m_hWnd, m_hdc);
    wglDeleteContext(m_hrc);
}

GLshort *CImageConverterView::render_image(jas_image_t *image)
{
    int i, j, k;
    int iw, ih; // image width & height
    int ho, vo; // grid horizontal & vertical offset
    int hs, vs; // grid horizontal & vertical space
    int cmptlut[3];
    int x, y, v;
    GLshort *data, *datap;
    jas_image_t *alt_image;
    jas_cmprof_t *outprof;
    if (!(outprof = jas_cmprof_createfromclrspc(JAS_CLRSPC_SRGB))) {
        return NULL;
    }
    if (!(alt_image = jas_image_chclrspc(image, outprof, JAS_CMXFORM_INTENT_PER))) {
        jas_cmprof_destroy(outprof);
        return NULL;
    }
    if ((cmptlut[0] = jas_image_getcmptbytype(alt_image, JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_R))) < 0 ||
        (cmptlut[1] = jas_image_getcmptbytype(alt_image, JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_G))) < 0 ||
        (cmptlut[2] = jas_image_getcmptbytype(alt_image, JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_B))) < 0) {
        jas_cmprof_destroy(outprof);
        jas_image_destroy(alt_image);
        return NULL;
    }
    iw = jas_image_cmptwidth(alt_image, cmptlut[0]);
    ih = jas_image_cmptheight(alt_image, cmptlut[0]);
    for (i = 1; i < 3; i++) {
        if (jas_image_cmptwidth(alt_image, cmptlut[i]) != iw || jas_image_cmptheight(alt_image, cmptlut[i]) != ih) {
            jas_cmprof_destroy(outprof);
            jas_image_destroy(alt_image);
            return NULL;
        }
    }
    ho = jas_image_cmpttlx(alt_image, cmptlut[0]);
    vo = jas_image_cmpttly(alt_image, cmptlut[0]);
    hs = jas_image_cmpthstep(alt_image, cmptlut[0]);
    vs = jas_image_cmptvstep(alt_image, cmptlut[0]);
    m_vw = ho + iw * hs;
    m_vh = vo + ih * vs;
    if (!(data = (GLshort *)malloc(4 * m_vw * m_vh * sizeof(GLshort)))) {
        jas_cmprof_destroy(outprof);
        jas_image_destroy(alt_image);
        return NULL;
    }
    memset(data, 0, 4 * m_vw * m_vh * sizeof(GLshort));
    for (i = 0; i < m_vh; i++) {
        datap = data + (m_vh - 1 - i) * (4 * m_vw);
        for (j = 0; j < m_vw; j++) {
            x = (j - ho) / hs;
            y = (i - vo) / vs;
            for (k = 0; k < 3; k++) {
                v = jas_image_readcmptsample(alt_image, cmptlut[k], x, y);
                v <<= 16 - jas_image_cmptprec(alt_image, cmptlut[k]);
                if (v < 0) {
                    v = 0;
                } else if (v > 0xFFFF) {
                    v = 0xFFFF;
                }
                *datap = (GLshort)v;
                datap++;
            }
            *datap = 0;
            datap++;
        }
    }
    return data;
}

void CImageConverterView::OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint)
{
    // TODO: Add your specialized code here and/or call the base class
    CImageConverterDoc *pDoc = GetDocument();
    if (m_data) {
        free(m_data);
    }
    if (!(m_data = render_image(pDoc->m_image))) {
        AfxMessageBox(_T("Render image failed!"));
    }
    CRect rect;
    GetClientRect(&rect);
    change_view(rect.Width(), rect.Height());
}

void CImageConverterView::OnSize(UINT nType, int cx, int cy)
{
    CView::OnSize(nType, cx, cy);

    // TODO: Add your message handler code here
    change_view(cx, cy);
}

BOOL CImageConverterView::OnEraseBkgnd(CDC *pDC)
{
    // TODO: Add your message handler code here and/or call default
    return TRUE;
    return CView::OnEraseBkgnd(pDC);
}

void CImageConverterView::change_view(const int cx, const int cy)
{
    wglMakeCurrent(m_hdc, m_hrc);
    glViewport(0, 0, cx, cy);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, cx, 0, cy);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRasterPos2i((cx > m_vw) ? (cx - m_vw) / 2 : 0, (cy > m_vh) ? (cy - m_vh) / 2 : 0);
}
