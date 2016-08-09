// TitleBox.cpp : implementation file
//

#include "stdafx.h"
//#include "CurtainBox.h"
#include "TitleBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTitleBox

CTitleBox::CTitleBox()
{
	m_ForeColor = RGB(0,0,0);			//������ɫ
	m_BkColor = RGB(177,195,217);		//����ɫ
	m_butStatus = BUT_STATUS_DOWN;		//��ť״̬
	b_InRect = false;			//�������־
	b_Click = false;			//������־
}

CTitleBox::~CTitleBox()
{
}


BEGIN_MESSAGE_MAP(CTitleBox, CButton)
	//{{AFX_MSG_MAP(CTitleBox)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTitleBox message handlers

void CTitleBox::PreSubclassWindow()		//��ClassWizard����
{
	CButton::PreSubclassWindow();
	ModifyStyle( 0, BS_OWNERDRAW );		//���ð�ť����Ϊ�Ի�ʽ
}

void CTitleBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)		//��ClassWizard����
{
	CDC *pDC = CDC::FromHandle( lpDrawItemStruct->hDC );
	CRect m_butRect = lpDrawItemStruct->rcItem;		//��ȡ��ť�ߴ�
	GetWindowText( m_strText );						//��ȡ��ť�ı�
	CPoint m_Center = m_butRect.CenterPoint();		//��ť���ĵ�
	int nSavedDC = pDC->SaveDC();
	VERIFY( pDC );

	pDC->FillSolidRect( &m_butRect, m_BkColor );	//����ť����
	CPen bkPen;
	bkPen.CreatePen( PS_SOLID, 2, RGB(227,233,241) );
	pDC->SelectObject( &bkPen );
	pDC->MoveTo( m_butRect.left, m_butRect.top+2 );
	pDC->LineTo( m_butRect.right, m_butRect.top+2 );
	pDC->MoveTo( m_butRect.left, m_butRect.bottom-2 );
	pDC->LineTo( m_butRect.right, m_butRect.bottom-2 );

	CBitmap m_Bmp;									//����ť��ߵ�λͼ
	CPoint pt;
	m_Bmp.LoadBitmap( IDB_BITMAP1 );
	pt.x = m_butRect.left;
	pt.y = m_butRect.top+(m_butRect.Height()-16)/2;
	CDC dcMem;
	dcMem.CreateCompatibleDC( pDC );
	dcMem.SelectObject(&m_Bmp[0]);
	pDC->BitBlt( pt.x, pt.y, 16, 16, &dcMem, 0, 0, SRCAND );
	dcMem.DeleteDC();

	POINT  m_pt[3];		//��ͷ����
	pt.x = m_butRect.left+22;						//����ͷ
	pt.y = m_butRect.top+m_butRect.Height()/2;
	switch( m_butStatus )
	{
	case BUT_STATUS_DOWN:
		m_pt[0].x = pt.x-5;
		m_pt[0].y = pt.y-3;
		m_pt[1].x = pt.x+6;
		m_pt[1].y = pt.y-3;
		m_pt[2].x = pt.x;
		m_pt[2].y = pt.y+3;
		break;
	case BUT_STATUS_RIGHT:
		m_pt[0].x = pt.x;
		m_pt[0].y = pt.y-6;
		m_pt[1].x = pt.x+6;
		m_pt[1].y = pt.y;
		m_pt[2].x = pt.x;
		m_pt[2].y = pt.y+6;
		break;
	}
	CBrush Brush;
	Brush.CreateSolidBrush( m_ForeColor );
	pDC->SelectObject( &Brush );
	CRgn rgn;
	rgn.CreatePolygonRgn( m_pt, 3, ALTERNATE );
	pDC->PaintRgn( &rgn );

	CSize Extent = pDC->GetTextExtent( m_strText );		//����ť����
	pDC->SetTextColor( m_ForeColor );
	pDC->SetBkMode( TRANSPARENT );
	CRect m_textRect;	//�ı�λ��
	m_textRect.SetRect( m_butRect.left+34, m_Center.y-Extent.cy/2,
			m_butRect.left+34+Extent.cx, m_Center.y+Extent.cy/2 );
	pDC->TextOut( m_textRect.left, m_textRect.top, m_strText );
	m_InvalRect.SetRect( m_textRect.left-17, m_textRect.top,
		m_textRect.right, m_textRect.bottom+2 );	//��Ч����������ͷ������
	
	if( b_InRect )										//������ʱ���������¼ӻ���
	{
		CPen bkPen;
		bkPen.CreatePen( PS_SOLID, 1, m_ForeColor );
		pDC->SelectObject( &bkPen );
		pDC->MoveTo( m_textRect.left, m_textRect.bottom+1 );
		pDC->LineTo( m_textRect.right, m_textRect.bottom+1 );
	}
	
	pDC->RestoreDC( nSavedDC );
}
//����ƶ���Ϣ����
void CTitleBox::OnMouseMove(UINT nFlags, CPoint point) 		//��ClassWizard����
{
	CButton::OnMouseMove(nFlags, point);
	if( !b_InRect )
	{
		if ( m_InvalRect.PtInRect(point) )		//��������Ч��
		{
			b_InRect = true;
			InvalidateRect( &m_InvalRect, false );	//�ػ水ť
		}
	}
	if( b_InRect )
	{
		if ( !m_InvalRect.PtInRect(point) )		//����뿪��Ч��
		{
			b_InRect = false;
			InvalidateRect( &m_InvalRect, false );	//�ػ水ť
		}
	}
}
//��굥����Ϣ����
void CTitleBox::OnLButtonDown(UINT nFlags, CPoint point) 		//��ClassWizard����
{
	CButton::OnLButtonDown(nFlags, point);
	if( b_InRect )		//ֻ������Ч����������Ӧ
	{
		switch( m_butStatus )		//���İ�ť״̬
		{
		case BUT_STATUS_DOWN:
			m_butStatus = BUT_STATUS_RIGHT;
			break;
		case BUT_STATUS_RIGHT:
			m_butStatus = BUT_STATUS_DOWN;
			break;
		}
		InvalidateRect( &m_InvalRect, false );	//�ػ水ť
		b_Click = true;			//������־
	}
}

/////////////////////////////////////////////////////////////////////////
//�ӿں�����

//�����ı���ɫ
void CTitleBox::SetForeColor(COLORREF color)
{
	m_ForeColor = color;
}

//���ñ�����ɫ
void CTitleBox::SetBkColor(COLORREF color)
{
	m_BkColor = color;
}

//��ȡ��ť״̬
int CTitleBox::GetButStatus()
{
	return m_butStatus;
}

//���ð�ť״̬
void CTitleBox::SetButStatus(int Status)
{
	m_butStatus = Status;
	InvalidateRect( &m_InvalRect, false );
}

//�Ƿ񵥻���ť��Ч��
BOOL CTitleBox::ClickBut()
{
	if( b_Click )
	{
		b_Click = false;
		return true;
	}
	return false;
}