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
	m_ForeColor = RGB(0,0,0);			//文字颜色
	m_BkColor = RGB(177,195,217);		//背景色
	m_butStatus = BUT_STATUS_DOWN;		//按钮状态
	b_InRect = false;			//鼠标进入标志
	b_Click = false;			//单击标志
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

void CTitleBox::PreSubclassWindow()		//由ClassWizard生成
{
	CButton::PreSubclassWindow();
	ModifyStyle( 0, BS_OWNERDRAW );		//设置按钮属性为自画式
}

void CTitleBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)		//由ClassWizard生成
{
	CDC *pDC = CDC::FromHandle( lpDrawItemStruct->hDC );
	CRect m_butRect = lpDrawItemStruct->rcItem;		//获取按钮尺寸
	GetWindowText( m_strText );						//获取按钮文本
	CPoint m_Center = m_butRect.CenterPoint();		//求按钮中心点
	int nSavedDC = pDC->SaveDC();
	VERIFY( pDC );

	pDC->FillSolidRect( &m_butRect, m_BkColor );	//画按钮背景
	CPen bkPen;
	bkPen.CreatePen( PS_SOLID, 2, RGB(227,233,241) );
	pDC->SelectObject( &bkPen );
	pDC->MoveTo( m_butRect.left, m_butRect.top+2 );
	pDC->LineTo( m_butRect.right, m_butRect.top+2 );
	pDC->MoveTo( m_butRect.left, m_butRect.bottom-2 );
	pDC->LineTo( m_butRect.right, m_butRect.bottom-2 );

	CBitmap m_Bmp;									//画按钮左边的位图
	CPoint pt;
	m_Bmp.LoadBitmap( IDB_BITMAP1 );
	pt.x = m_butRect.left;
	pt.y = m_butRect.top+(m_butRect.Height()-16)/2;
	CDC dcMem;
	dcMem.CreateCompatibleDC( pDC );
	dcMem.SelectObject(&m_Bmp[0]);
	pDC->BitBlt( pt.x, pt.y, 16, 16, &dcMem, 0, 0, SRCAND );
	dcMem.DeleteDC();

	POINT  m_pt[3];		//箭头坐标
	pt.x = m_butRect.left+22;						//画箭头
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

	CSize Extent = pDC->GetTextExtent( m_strText );		//画按钮文字
	pDC->SetTextColor( m_ForeColor );
	pDC->SetBkMode( TRANSPARENT );
	CRect m_textRect;	//文本位置
	m_textRect.SetRect( m_butRect.left+34, m_Center.y-Extent.cy/2,
			m_butRect.left+34+Extent.cx, m_Center.y+Extent.cy/2 );
	pDC->TextOut( m_textRect.left, m_textRect.top, m_strText );
	m_InvalRect.SetRect( m_textRect.left-17, m_textRect.top,
		m_textRect.right, m_textRect.bottom+2 );	//有效区：包括箭头和文字
	
	if( b_InRect )										//鼠标进入时，在文字下加划线
	{
		CPen bkPen;
		bkPen.CreatePen( PS_SOLID, 1, m_ForeColor );
		pDC->SelectObject( &bkPen );
		pDC->MoveTo( m_textRect.left, m_textRect.bottom+1 );
		pDC->LineTo( m_textRect.right, m_textRect.bottom+1 );
	}
	
	pDC->RestoreDC( nSavedDC );
}
//鼠标移动消息函数
void CTitleBox::OnMouseMove(UINT nFlags, CPoint point) 		//由ClassWizard生成
{
	CButton::OnMouseMove(nFlags, point);
	if( !b_InRect )
	{
		if ( m_InvalRect.PtInRect(point) )		//鼠标进入有效区
		{
			b_InRect = true;
			InvalidateRect( &m_InvalRect, false );	//重绘按钮
		}
	}
	if( b_InRect )
	{
		if ( !m_InvalRect.PtInRect(point) )		//鼠标离开有效区
		{
			b_InRect = false;
			InvalidateRect( &m_InvalRect, false );	//重绘按钮
		}
	}
}
//鼠标单击消息函数
void CTitleBox::OnLButtonDown(UINT nFlags, CPoint point) 		//由ClassWizard生成
{
	CButton::OnLButtonDown(nFlags, point);
	if( b_InRect )		//只有在有效区单击才响应
	{
		switch( m_butStatus )		//更改按钮状态
		{
		case BUT_STATUS_DOWN:
			m_butStatus = BUT_STATUS_RIGHT;
			break;
		case BUT_STATUS_RIGHT:
			m_butStatus = BUT_STATUS_DOWN;
			break;
		}
		InvalidateRect( &m_InvalRect, false );	//重绘按钮
		b_Click = true;			//单击标志
	}
}

/////////////////////////////////////////////////////////////////////////
//接口函数：

//设置文本颜色
void CTitleBox::SetForeColor(COLORREF color)
{
	m_ForeColor = color;
}

//设置背景颜色
void CTitleBox::SetBkColor(COLORREF color)
{
	m_BkColor = color;
}

//获取按钮状态
int CTitleBox::GetButStatus()
{
	return m_butStatus;
}

//设置按钮状态
void CTitleBox::SetButStatus(int Status)
{
	m_butStatus = Status;
	InvalidateRect( &m_InvalRect, false );
}

//是否单击按钮有效区
BOOL CTitleBox::ClickBut()
{
	if( b_Click )
	{
		b_Click = false;
		return true;
	}
	return false;
}