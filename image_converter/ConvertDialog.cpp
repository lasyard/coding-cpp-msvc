// ConvertDialog.cpp : implementation file
//

#include "stdafx.h"

#include <jasper/jasper.h>

#include "ConvertDialog.h"

#include "ImageConverter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConvertDialog dialog

CConvertDialog::CConvertDialog(CWnd *pParent /*=NULL*/) : CDialog(CConvertDialog::IDD, pParent), m_caption(_T(""))
{
    //{{AFX_DATA_INIT(CConvertDialog)
    m_fmtdesc = _T("");
    //}}AFX_DATA_INIT
}

void CConvertDialog::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CConvertDialog)
    DDX_Control(pDX, IDC_FMT_LST, m_fmt_lst);
    DDX_Text(pDX, IDC_FMT, m_fmtdesc);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CConvertDialog, CDialog)
//{{AFX_MSG_MAP(CConvertDialog)
ON_CBN_SELCHANGE(IDC_FMT_LST, OnSelchangeFmtLst)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConvertDialog message handlers

void CConvertDialog::set_current_fmt(const int fmt)
{
    m_fmt = fmt;
    const jas_image_fmtinfo_t *fmtinfo = jas_image_lookupfmtbyid(m_fmt);
    m_fmtdesc = fmtinfo->desc;
}

BOOL CConvertDialog::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO: Add extra initialization here
    const jas_image_fmtinfo_t *fmtinfo;
    if (m_caption) {
        SetWindowText(m_caption);
    }
    for (int i = 0; i < JAS_IMAGE_MAXFMTS; i++) {
        if (i != m_fmt) {
            if (!(fmtinfo = jas_image_lookupfmtbyid(i))) {
                break;
            }
            m_fmt_lst.AddString(CA2T(fmtinfo->desc));
        }
    }
    m_fmt_lst.SetCurSel(m_sel = 0);

    return TRUE; // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CConvertDialog::set_caption(LPCTSTR s)
{
    m_caption = s;
}

void CConvertDialog::OnSelchangeFmtLst()
{
    // TODO: Add your control notification handler code here
    m_sel = m_fmt_lst.GetCurSel();
    if (m_sel >= m_fmt) {
        m_sel++;
    }
}
