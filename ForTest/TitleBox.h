/////////////////////////////////////////////////////////////////////////////
//CTitleBox:�����
//
//��Dreamweaver��Flash������еĹ��ߴ�������һ�ֱ���򣬵�����ʹ���ߴ���
//�򿪻�رգ�����ť���Ƿ���������״�������ġ�
//
//�÷���
//���ڶԻ����з���һ����ť�ؼ����Ѵ�С�������ʣ�
//��Ϊ��ť������������͸�ΪCTitleBox;
//���ڶԻ����OnInitDialog()���������ñ������ɫ�ͳ�ʼ���
//
//���ߣ�����
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
//��ť״̬
#define BUT_STATUS_DOWN   1		//��ͷ���µı����
#define BUT_STATUS_RIGHT  2		//��ͷ���ҵı����

class CTitleBox : public CButton
{
// Construction
public:
	CTitleBox();

private:
	int      m_butStatus;	//��ť״̬
	COLORREF m_ForeColor;	//�ı���ɫ
	COLORREF m_BkColor;		//����ɫ
	CString  m_strText;		//��ť�ı�
	BOOL     b_InRect;		//�������־
	BOOL     b_Click;		//�Ƿ񵥻���
	CRect    m_InvalRect;	//��Ч���ߴ�

// Attributes
public:
	void SetForeColor(COLORREF color);		//�����ı���ɫ
	void SetBkColor(COLORREF color);		//���ñ�����ɫ
	int  GetButStatus();					//��ȡ��ť״̬
	void SetButStatus(int Status);			//���ð�ť״̬
	BOOL ClickBut();						//�Ƿ񵥻�����ť������

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
