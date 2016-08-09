
// ForTestDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "ButtonSet.h"
#include "ShadeButtonST.h"
#include "windows.h"
#include "cv.h"
#include "highgui.h"
#include "ForTest.h"
#include "Galil.h"
#include "afxcmn.h"
#include "HyperLink.h"
#include "CvvImage.h"
#include "SortListCtrl.h"
#include <iostream>

using namespace cv;
using namespace std;
//opencv中已经有定义了CV_PI
//#define pi 3.141592654;

// CForTestDlg 对话框
class CForTestDlg : public CDialogEx
{
public:
	BITMAPINFOHEADER m_header;
	DS_RESOLUTION m_Resolution;
	BYTE* m_pImageData;
	
	CBrush m_B1;
	CBrush m_B2;
	CBrush m_B3;
	CBrush m_B4;
	IplImage *DetectImage;//打开要识别的图片
	IplImage* dstImage;

	IplImage* displayImage;//固定要显示的图片，固定分辨率944*708
	IplImage* savedImage;//用于保存的图片
	CvMemStorage* storage;
	CvSeq* results;
	bool RecogFlag;//识别好了与否标志
	IplImage* image_edge;//灰度图片
	IplImage* circleImage;//用来显示检测后的检查的圆
	IplImage* GrayImage;//用来显示检测后的圆在二值化图像上
	IplImage* WatershedImg;
	//CvScalar color;
	int nThreashold;
	Mat imageMatch;
	Mat imageMatchShow;

	BOOL isCamOpen;
	
// 构造
public:
	CForTestDlg(CWnd* pParent = NULL);	// 标准构造函数
	BYTE m_RunMode ;
	int m_expTime;

// 对话框数据
	enum { IDD = IDD_FORTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	void StartDevice();
	void UpdateExposurteTimeText();
	void UpdateSetupWindow();
	void SetDisplayWindow();
	void ShowImage(IplImage* img, UINT ID);
	void ShowImage(IplImage* img);
	void ResizeImage(IplImage* img);
	//自适应求高低两个门限 
	void AdaptiveFindThreshold(const CvArr* image, double *low, double *high, int aperture_size=3);
	void _AdaptiveFindThreshold(CvMat *dx, CvMat *dy, double *low, double *high);
	vector<Point> RegionGrow(IplImage* src, Point2i pt, int th); //区域生长算法
	void edgeEnhance(cv::Mat& srcImg, cv::Mat& dstImg);//
	void cvThin(cv::Mat& src, cv::Mat& dst, int intera);//图像细花操作
	
	bool IsInTheCircle(CvPoint p, CvPoint center, int radius);
	bool IsOnTheCircle(CvPoint p, CvPoint center, int radius);
public:
	CShadeButtonST m_openCam;
	CShadeButtonST m_snap;
	CShadeButtonST m_exit;
	CShadeButtonST m_up;
	CShadeButtonST m_down;
	CShadeButtonST m_left;
	CShadeButtonST m_right;
	CShadeButtonST m_orthogonal;
	CShadeButtonST m_whitebalance;
	CShadeButtonST m_SpdSet;
	CShadeButtonST m_ImagePathSet;
	double m_SpeedX;
	double m_SpeedY;
	CString m_ImagePath;
	CShadeButtonST m_goto;
	double m_curX;
	double m_curY;
	double m_targetX;
	double m_targetY;
	afx_msg void OnBnClickedOpenCam();
	CComboBox m_MultiCamList;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnCbnSelchangeResolution();
	CComboBox m_resolution;
	CSliderCtrl m_Contrast;
	CSliderCtrl m_target;
	CSliderCtrl m_exposureTime;
	CShadeButtonST m_default;
	afx_msg void OnBnClickedDefault();
	afx_msg void OnBnClickedExit();
	afx_msg void OnBnClickedScanimgpath();
	afx_msg void OnBnClickedOrthogonal();
	afx_msg void OnBnClickedAutoexp();
	afx_msg void OnBnClickedSnap();
	afx_msg void OnBnClickedWhitebalance();
	afx_msg void OnBnClickedConfirm();
	afx_msg void OnBnClickedRight();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedLeft();
	afx_msg void OnBnClickedUp();
	afx_msg void OnBnClickedDown();
	afx_msg void OnBnClickedGoto();
	afx_msg void OnBnClickedCheck3();
	double m_stepX;
	double m_stepY;
	CShadeButtonST m_stepSet;
	afx_msg void OnBnClickedStepset();
	afx_msg void OnBnClickedStop();
	CShadeButtonST m_stop;
	CShadeButtonST m_stitchPathSet;
	CString m_stitchPath;
	CComboBox m_objectlens;
	afx_msg void OnCbnSelchangeObjectlens();
	afx_msg void OnBnClickedStitchpathset();
	CShadeButtonST m_detect;
	afx_msg void OnBnClickedDetect();
	afx_msg void OnBnClickedGoto2();
	CShadeButtonST m_goto2;
	afx_msg void OnEnChangeMfceditbrowse1();
	CSliderCtrl m_threashold;
	CShadeButtonST m_countDefault;
	int m_minRadius;
	int m_maxRadius;
	CShadeButtonST m_begin;
	int m_scanCols;
	int m_scanRows;
	afx_msg void OnBnClickedBegin();
	CHyperLink m_url;
	//新工作线程显示坐标
	void DisplayPos();
	CWinThread *m_Thread;
	//新工作线程自动扫描
	void AutoScan();
	CWinThread * m_ScanThread;

	//afx_msg void OnBnClickedButton1();
	CShadeButtonST m_match;
	afx_msg void OnBnClickedMatch();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CSortListCtrl m_tableWidget;
	CShadeButtonST m_recognition;
	CShadeButtonST m_reload;
	afx_msg void OnBnClickedRecognition();
	CShadeButtonST m_export;
	afx_msg void OnBnClickedReload();
	afx_msg void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	double m_point;
	int m_xiao;
	int m_da;
	double m_acc;
	double m_hough;
	int m_erodeNum;
	afx_msg void OnBnClickedopen();

	public: //临时开运算、闭运算等变量
		int nOpen;
		int nClose;
		int nErode;
		int nDilate;
		int nNum;
		int nThin;
		afx_msg void OnBnClickedclose();
		afx_msg void OnBnClickederode();
		afx_msg void OnBnClickeddilate();
		afx_msg void OnBnClickeddilate2();
		afx_msg void OnBnClickedButton2();
		afx_msg void OnBnClickedButton3();
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
		//afx_msg void OnBnClickedButton4();
		afx_msg void OnBnClickedrmbackgroud();
//		afx_msg void OnBnClickedCheck4();
		afx_msg void OnBnClickededgeenhance();
		double m_xiangsidu;
		afx_msg void OnBnClickedButton5();
		afx_msg void OnBnClickedExport();
		afx_msg void OnBnClickedCountdefault();
		CSliderCtrl m_threshold;
		int m_iThresholdValue;
		Point2i ReverseSize(Point2i point);
		afx_msg void OnBnClickedCheck4();
		afx_msg void OnBnClickedButton4();

		//临时增加一个保存距离变换前的变量
		IplImage* distImage;
		//形态学区域极小值
		void MorpHMin(const IplImage* src, IplImage* dst, unsigned char h, IplConvKernel* se = NULL);
		//形态学测地腐蚀和腐蚀重建运算
		void MorpRErode(const IplImage* src,  const IplImage* msk, IplImage* dst, IplConvKernel* se = NULL, int iterations=-1);
		//比较两个图像是否相同， 0 相同
		int  ImageCmp(const IplImage* img1, const IplImage* img2);

		CComboBox m_selImage;
		afx_msg void OnCbnSelchangeSelimage();
		afx_msg void OnBnClickedButton6();
		afx_msg void OnBnClickedButton7();
};

typedef struct DNA_IMAGE_STRUCT            // Image Struct
{
	unsigned short width;                                      
	unsigned short height;            
	unsigned short Channel;           
	unsigned char *pixels;    
}
DNA_IMAGE;

typedef struct CIRCLE_STRUCT            // Image Struct
{
	Point2i center;
	int radius;
	double meanScale;
}
CIRCLE;

DNA_IMAGE* DNA_CreatImage(int Channel, int Height, int Width);
void DNA_SaveImage(DNA_IMAGE* Image, char* filename);
int DNA_freeImage(DNA_IMAGE* pImage);

