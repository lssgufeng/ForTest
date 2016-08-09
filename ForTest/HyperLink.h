// HyperLink.h : header file
//
//
// HyperLink static control.
//
// Copyright Giancarlo Iovino, 1997 (giancarlo@saria.com)
// This code is based on CHyperlink by Chris Maunder.
// Feel free to use and distribute. May not be sold for profit. 

#if !defined(AFX_HYPERLINK_H_04ET323B01_023500_0204251998_ENG_INCLUDED_)
#define AFX_HYPERLINK_H_04ET323B01_023500_0204251998_ENG_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Structure used to get/set hyperlink colors

/////////////////////////////////////////////////////////////////////////////
// CHyperLink window

class CHyperLink : public CStatic
{
	DECLARE_DYNAMIC(CHyperLink)

public:
// Link styles

// Construction/destruction
	CHyperLink();
	virtual ~CHyperLink();

// Attributes
public:

// Operations
public:	

	void SetURL(CString strURL);
    CString GetURL() const;
    void SetColor(COLORREF clr,BYTE clrItem);//设置颜色
	COLORREF GetColor(BYTE clrItem);//得到颜色
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHyperLink)
    protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
protected:
	int GotoURL(LPCTSTR url, int showcmd);
// Protected attributes
protected:
    COLORREF m_clrHot;		// Link normal color
    COLORREF m_clrNor;	    // Link active color
    COLORREF m_clrBG;	    // Link active color
	BOOL	 m_bHot;				// Is the link active?
	CString  m_strURL;					// Hyperlink URL string

	// Generated message map functions
protected:
	//{{AFX_MSG(CHyperLink)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg LRESULT  OnMouseHover(WPARAM wParam,LPARAM lParam);
    afx_msg LRESULT  OnMouseLeave(WPARAM wParam,LPARAM lParam);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line

#endif // !defined(AFX_HYPERLINK_H_04ET323B01_023500_0204251998_ENG_INCLUDED_)
