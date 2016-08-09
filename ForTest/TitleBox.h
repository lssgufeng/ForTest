/////////////////////////////////////////////////////////////////////////////
//CTitleBox:标题框
//
//在Dreamweaver、Flash等软件中的工具窗口上有一种标题框，点击后会使工具窗口
//打开或关闭，本按钮就是仿造它的形状做出来的。
//
//用法：
//①在对话框中放置一个按钮控件，把大小调整合适；
//②为按钮定义变量，类型改为CTitleBox;
//③在对话框的OnInitDialog()函数中设置标题框颜色和初始风格。
//
//作者：风林
//                   2004-12-18
////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_TITLEBOX_H__1A63B251_18D3_4999_BD5B_6421A9838A4D__INCLUDED_)
#define AFX_TITLEBOX_H__1A63B251_18D3_4999_BD5B_6421A9838A4D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TitleBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTitleBox window
//按钮状态
#define BUT_STATUS_DOWN   1		//箭头向下的标题框
#define BUT_STATUS_RIGHT  2		//箭头向右的标题框

class CTitleBox : public CButton
{
// Construction
public:
	CTitleBox();

private:
	int      m_butStatus;	//按钮状态
	COLORREF m_ForeColor;	//文本颜色
	COLORREF m_BkColor;		//背景色
	CString  m_strText;		//按钮文本
	BOOL     b_InRect;		//鼠标进入标志
	BOOL     b_Click;		//是否单击过
	CRect    m_InvalRect;	//有效区尺寸

// Attributes
public:
	void SetForeColor(COLORREF color);		//设置文本颜色
	void SetBkColor(COLORREF color);		//设置背景颜色
	int  GetButStatus();					//获取按钮状态
	void SetButStatus(int Status);			//设置按钮状态
	BOOL ClickBut();						//是否单击过按钮文字区

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTitleBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTitleBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTitleBox)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TITLEBOX_H__1A63B251_18D3_4999_BD5B_6421A9838A4D__INCLUDED_)
