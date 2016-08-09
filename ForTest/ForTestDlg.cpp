
// ForTestDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ForTest.h"
#include "ForTestDlg.h"
#include "afxdialogex.h"
#include <iostream>
#include <stdio.h>
using namespace std;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TDIBWIDTHBYTES(bits)	(((bits) + 31) / 32 * 4)
/*************************************************
*һЩ��Ҫ��ȫ�ֱ���
/***************************************************/

Galil *m_pGalil = NULL;//�˶�����
CString sImagePath, sPath;
int orthogonal = 1;//�Ƿ��������
bool ImageOpenFlag = false;//��ͼƬ��־
int nWidth = 0, nHeight = 0;//����ֱ���
double m_Row = 0, m_Col = 0;//ƴ��ʱ����ɨ�貽��
UINT RawTime;//��ǰ�趨����ʱ�䣬���������ع�ʱ��
int nWBFlag = 0;//��ƽ���־
int nSFlag = 0;//�ٶ����ñ�־
int nStep = 0;//�����������ñ�־
int nDetect = 0;//��������־
int nScale = 0;//��߱�� 
int m_StepMode = 1;//�˶�ģʽ(����or������
int lens = 4;//�ﾵ��־
int nSnap = 0;//���ձ��
int nScan = 0;//ɨ���־
int nCur_row = 0, nCur_col = 0;//��ǰɨ����С�����
//int counts = 1;//��ɨ������
int nAutoScan = 0;//�Զ�ɨ���̱߳�־
int num = 0;//ʶ�����Բ�ĸ���
int removeBg = 0;

DNA_IMAGE* Save_image = NULL;
int spA = 5000, spB = 5000, acA = 500000, acB = 500000;//�˶���������
char* filename = NULL;//����ͼƬ�ļ���������·����
BYTE* m_pImageData = NULL;


//����ʶ�����Բ
vector<CIRCLE> vCircles;

//�����ļ�·��
void GetGlobalPath()
{
	GetModuleFileName(NULL, sPath.GetBuffer(256), 256);
	sPath.ReleaseBuffer(256);

	sPath = sPath.Left( sPath.ReverseFind('\\') + 1 );
	sImagePath = sPath;
	sPath += "path.ini";
}

//����ͼ���ڴ�ռ�ͱ���ͼ��
DNA_IMAGE* DNA_CreatImage(int Channel, int Height, int Width)
{
	DNA_IMAGE *pImage = NULL;
	if((pImage = (DNA_IMAGE *)malloc(sizeof(DNA_IMAGE))) == NULL)
	{
		return 0;
	}

	if((pImage->pixels = (unsigned char *)malloc(Height * Width * Channel * sizeof(unsigned char))) == NULL)
	{
		return 0;
	}
	memset(pImage->pixels, 0, Height * Width * Channel * sizeof(unsigned char));
	pImage->width = Width;
	pImage->height = Height;
	pImage->Channel = Channel;
	return pImage;
}

void DNA_SaveImage(DNA_IMAGE* Image, char* filename)
{
	IplImage  *TempImage = NULL;
	int width = Image->width;
	int height = Image->height;
	int channel = Image->Channel;

	TempImage = cvCreateImage(cvSize(width, height), 8, channel);
	if(!TempImage)
	{
		return;
	}
	if(channel == 3)
	{
		TempImage->origin = 1;
	}
	else
	{
		TempImage->origin = 0;
	}
	TempImage->imageData = (char *)Image->pixels;	    
	cvSaveImage(filename, TempImage);
	cvReleaseImage(&TempImage);
}

int DNA_freeImage(DNA_IMAGE* pImage)
{
	if(pImage)
	{
		free(pImage->pixels);
		pImage->pixels = NULL;
		free(pImage);
		pImage = NULL;
	}	
	return 1;
}
//Ϊrgb���ݣ�����Ϊbmp�ļ���
void SavePictureFile(BYTE* pBmp24,char* filename, int wide, int high)
{
	//�ֱ�Ϊrgb���ݣ�Ҫ�����bmp�ļ�����ͼƬ���� http://blog.csdn.net/chyxwzn/article/details/8443883
	int size = wide * high * 3 * sizeof(char); // ÿ�����ص�3���ֽ�
	// λͼ��һ���֣��ļ���Ϣ
	BITMAPFILEHEADER bfh;
	bfh.bfType = (WORD)0x4d42;  //bm
	bfh.bfSize = size  // data size
		+ sizeof( BITMAPFILEHEADER ) // first section size
		+ sizeof( BITMAPINFOHEADER ) // second section size
		;
	bfh.bfReserved1 = 0; // reserved
	bfh.bfReserved2 = 0; // reserved
	bfh.bfOffBits = sizeof( BITMAPFILEHEADER )+ sizeof( BITMAPINFOHEADER );//���������ݵ�λ��

	// λͼ�ڶ����֣�������Ϣ
	BITMAPINFOHEADER bih;
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biWidth = wide;
	//bih.biHeight = -height;//BMPͼƬ�����һ���㿪ʼɨ�裬��ʾʱͼƬ�ǵ��ŵģ�������-height������ͼƬ������
	bih.biHeight = high;
	bih.biPlanes = 1;//Ϊ1�����ø�
	bih.biBitCount = 24;
	bih.biCompression = 0;//��ѹ��
	bih.biSizeImage = size;
	bih.biXPelsPerMeter = 2835 ;//����ÿ��
	bih.biYPelsPerMeter = 2835 ;
	bih.biClrUsed = 0;//���ù�����ɫ��24λ��Ϊ0
	bih.biClrImportant = 0;//ÿ�����ض���Ҫ
	FILE * fp = fopen( filename,"wb" );
	if( !fp ) return;

	fwrite( &bfh, 8, 1,  fp );//����linux��4�ֽڶ��룬����Ϣͷ��СΪ54�ֽڣ���һ����14�ֽڣ��ڶ�����40�ֽڣ����ԻὫ��һ���ֲ���Ϊ16�Լ���ֱ����sizeof����ͼƬʱ�ͻ�����premature end-of-file encountered����
	fwrite(&bfh.bfReserved2, sizeof(bfh.bfReserved2), 1, fp);
	fwrite(&bfh.bfOffBits, sizeof(bfh.bfOffBits), 1, fp);
	fwrite( &bih, sizeof(BITMAPINFOHEADER),1,fp );
	fwrite(pBmp24,size,1,fp);
	fclose( fp );

	//Save_image = DNA_CreatImage(3, Height, Width);
	//filename1 = (LPSTR)pp;	
	//Save_image->pixels = BmpBuffer;
	//DNA_SaveImage(Save_image, filename1);
	//Save_image = NULL;
	//DNA_freeImage(Save_image);
}

//��ʾλ������
void CForTestDlg::DisplayPos()
{
	m_curX = (double)m_pGalil->commandValue("TPA");
	m_curY = (double)m_pGalil->commandValue("TPB");
	CString curx;
	curx.Format(_T("%0.3f"), m_curX / 2000);//ת��Ϊstring���ͣ�����3λС��
	GetDlgItem(IDC_CURX)-> SetWindowText(curx);//������������༭��
	CString cury;
	cury.Format(_T("%0.3f"), m_curY / 2000);//ת��Ϊstring���ͣ�����3λС��
	GetDlgItem(IDC_CURY)-> SetWindowText(cury);//������������༭��

}
UINT MyThreadProc(LPVOID lpvoid)
{
	CForTestDlg* m_form = (CForTestDlg*)lpvoid;
	while(TRUE)
	{
		m_form->DisplayPos();
		Sleep(60);
	}
}

int Orthogonal(BYTE *pBmp, int wide, int high)
{
	int i, j;
	int strideWidth = TDIBWIDTHBYTES(wide * 24);
	for (i = high / 4; i < high * 3 / 4; i++)
	{
		j = (wide / 2);
		//pBmp[i * wide * 3 + j * 3] = 0;
		//pBmp[i * wide * 3 + j * 3 + 1] = 255;
		//pBmp[i * wide * 3 + j * 3 + 2] = 0;
		pBmp[i * strideWidth + j * 3] = 0;//�����ֽڶ���
		pBmp[i * strideWidth + j * 3 + 1] = 255;
		pBmp[i * strideWidth + j * 3 + 2] = 0;
	}
	for (j = wide / 4; j < wide * 3 / 4; j++)
	{
		i = (high / 2);
		//pBmp[i * wide * 3 + j * 3] = 0;
		//pBmp[i * wide * 3 + j * 3 + 1] = 255;
		//pBmp[i * wide * 3 + j * 3 + 2] = 0;
		pBmp[i * strideWidth + j * 3] = 0;
		pBmp[i * strideWidth + j * 3 + 1] = 255;
		pBmp[i * strideWidth + j * 3 + 2] = 0;
	}
	return 1;
}

//unicode�ַ����ļ�·��ת��Ϊ���ֽ��ַ������ļ�·��
char* transToMultibyte(CString name)
{
	//unicode�ַ���ת��Ϊ���ֽ��ַ���
	char* filename1 = NULL;
	int len = WideCharToMultiByte(CP_OEMCP, NULL, name, -1, NULL, 0, NULL, NULL);
	char* pfileName = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, name, name.GetLength()+1, pfileName, len + 1, NULL, NULL);
	pfileName[len] = '\0';
	LPCTSTR pp = (LPCTSTR)pfileName;
	filename1 = (LPSTR)pp;

	return filename1;
}


//���û��ṩ��SDKʹ�ã�ͼ�������ⲿ�����
//�û��ڻص�������ʵ�ֶԲɼ����ݵĴ���
//pBuffer is RGB24BIT DATA 
//here is GBGB....RGRG....
#define B_OFFSET    0
#define G_OFFSET    1
#define R_OFFSET    2
int CALLBACK SnapThreadCallback(BYTE *pBuffer)
{
	BYTE *pBmp24 = CameraISP(pBuffer);
	if (orthogonal == 0)
	{
		Orthogonal(pBmp24, 1024, 768);
	}
	if (nScale == 1)
	{
		IplImage* tempImage = cvCreateImage(cvSize(nWidth, nHeight), 8, 3);
		tempImage->origin = 1;
		tempImage->imageData = (char *)pBmp24;
		CvFont font = cvFont(1.1, 1);
		if (lens == 4)
		{
			cvPutText(tempImage, "200um", cvPoint(934, 25), &font, CV_RGB(255, 0, 0));
			cvLine(tempImage, cvPoint(960 - 9, 15), cvPoint(960 + 9, 15), CV_RGB(255, 0, 0));
		}
		if (lens == 10)
		{
			cvPutText(tempImage, "50um", cvPoint(934, 25), &font, CV_RGB(255, 0, 0));
			cvLine(tempImage, cvPoint(960 - 28, 15), cvPoint(960 + 28, 15), CV_RGB(255, 0, 0));
		}
		if (lens == 20)
		{
			cvPutText(tempImage, "50um", cvPoint(934, 25), &font, CV_RGB(255, 0, 0));
			cvLine(tempImage, cvPoint(960 - 56, 15), cvPoint(960 + 56, 15), CV_RGB(255, 0, 0));
		}
		if (lens == 40)
		{
			cvPutText(tempImage, "25um", cvPoint(934, 25), &font, CV_RGB(255, 0, 0));
			cvLine(tempImage, cvPoint(960 - 56, 15), cvPoint(960 + 56, 15), CV_RGB(255, 0, 0));
		}
		cvReleaseImage(&tempImage);
	}
	if (nSnap == 1)
	{
		//*m_pImageData = *pBmp24;
		nSnap = 0;
		//if (nAutoScan == 1)
		//{
		//	CString sfilename; 
		//	sfilename.Format(_T("%s\\P%d_%d"), sImagePath, nCur_row, nCur_col);
		//	filename = transToMultibyte(sfilename);
		//}
		//else
		//{
		//	CTime time = CTime::GetCurrentTime();
		//	sfilename.Format(_T("%s\\P%02d%02d%02d%02d%02d.bmp"), sImagePath, time.GetMonth(), time.GetDay(), 
		//		time.GetHour(), time.GetMinute(), time.GetSecond());
		//}

		//filename = transToMultibyte(sfilename);

		Save_image = DNA_CreatImage(3, nHeight, nWidth);
		Save_image->pixels = pBmp24;
		DNA_SaveImage(Save_image, filename);
		Save_image = NULL;
		DNA_freeImage(Save_image);

	}
	if(pBmp24)
	{
		CameraDisplayRGB24(pBmp24);
	}
	return TRUE;
}
// CForTestDlg �Ի���

CForTestDlg::CForTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CForTestDlg::IDD, pParent)
	, m_SpeedX(0)
	, m_SpeedY(0)
	, m_ImagePath(_T(""))
	, m_curX(0)
	, m_curY(0)
	, m_targetX(0)
	, m_targetY(0)
	, m_stepX(0.5)
	, m_stepY(0.5)
	, m_minRadius(8)
	, m_maxRadius(18)
	, m_scanCols(3)
	, m_scanRows(3)
	, nThreashold(41)
	, m_point(1.00)
	, m_xiao(17)
	, m_da(25)
	, m_acc(200)
	, m_hough(50)
	, m_erodeNum(100)
	, m_xiangsidu(125)
	, m_iThresholdValue(191)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pImageData = NULL;
	m_Thread = NULL;
	m_ScanThread = NULL;
	DetectImage = NULL;
	dstImage = NULL;
	displayImage = NULL;
	storage = NULL;
	results = NULL;
	image_edge = NULL;
	circleImage = NULL;
	m_Thread = NULL;
	m_ScanThread = NULL;
	
	nOpen = 0;
	nClose = 0;
	nErode = 0;
	nDilate = 0;
	nThin = 0;
	nNum = 0;

	memset(&m_header, 0, sizeof(m_header));
	m_header.biSize = sizeof(m_header);
	m_header.biPlanes = 1;
	m_header.biBitCount = 24;	

	try
	{
		m_pGalil = new Galil("10.0.0.100");
	}
	catch(string e)
	{
		MessageBox(_T("����������ʧ�ܣ�"));
		m_pGalil = NULL;		
	}
	//m_pGalil = NULL;	
}

void CForTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OPEN_CAM, m_openCam);
	DDX_Control(pDX, IDC_SNAP, m_snap);
	DDX_Control(pDX, IDC_EXIT, m_exit);
	DDX_Control(pDX, IDC_UP, m_up);
	DDX_Control(pDX, IDC_DOWN, m_down);
	DDX_Control(pDX, IDC_LEFT, m_left);
	DDX_Control(pDX, IDC_RIGHT, m_right);
	DDX_Control(pDX, IDC_Orthogonal, m_orthogonal);
	DDX_Control(pDX, IDC_WHITEBALANCE, m_whitebalance);
	DDX_Text(pDX, IDC_SPEEDX, m_SpeedX);
	DDX_Text(pDX, IDC_SPEEDY, m_SpeedY);
	DDX_Control(pDX, IDC_CONFIRM, m_SpdSet);
	DDX_Control(pDX, IDC_ScanImgPath, m_ImagePathSet);
	DDX_Text(pDX, IDC_ScanPath, m_ImagePath);
	DDX_Control(pDX, IDC_GOTO, m_goto);
	DDX_Text(pDX, IDC_CURX, m_curX);
	DDX_Text(pDX, IDC_CURY, m_curY);
	DDX_Text(pDX, IDC_TARGETX, m_targetX);
	DDX_Text(pDX, IDC_TARGETY, m_targetY);
	DDX_Control(pDX, IDC_SEL_CAM, m_MultiCamList);
	DDX_Control(pDX, IDC_RESOLUTION, m_resolution);
	DDX_Control(pDX, IDC_SLIDER1, m_Contrast);
	DDX_Control(pDX, IDC_SLIDER2, m_target);
	DDX_Control(pDX, IDC_SLIDER3, m_exposureTime);
	DDX_Control(pDX, IDC_DEFAULT, m_default);
	DDX_Text(pDX, IDC_STEPX, m_stepX);
	DDX_Text(pDX, IDC_STEPY, m_stepY);
	DDX_Control(pDX, IDC_STEPSET, m_stepSet);
	DDX_Control(pDX, IDC_STOP, m_stop);
	DDX_Control(pDX, IDC_StitchPathSet, m_stitchPathSet);
	DDX_Text(pDX, IDC_StitchPath, m_stitchPath);
	DDX_Control(pDX, IDC_OBJECTLENS, m_objectlens);
	//	DDX_Control(pDX, IDC_TEXT, m_hyperLink);
	DDX_Control(pDX, IDC_DETECT, m_detect);
	DDX_Control(pDX, IDC_GOTO2, m_goto2);
	DDX_Control(pDX, IDC_SLIDER4, m_threashold);
	DDX_Control(pDX, IDC_COUNTDEFAULT, m_countDefault);
	DDX_Text(pDX, IDC_MINRADIUS, m_minRadius);
	DDX_Text(pDX, IDC_MAXRADIUS, m_maxRadius);
	DDX_Control(pDX, IDC_BEGIN, m_begin);
	DDX_Text(pDX, IDC_SCANCOLS, m_scanCols);
	DDX_Text(pDX, IDC_SCANROWS, m_scanRows);
	DDX_Control(pDX, IDC_URL, m_url);
	DDX_Control(pDX, IDC_MATCH, m_match);
	DDX_Control(pDX, IDC_LIST1, m_tableWidget);
	DDX_Control(pDX, IDC_RECOGNITION, m_recognition);
	DDX_Control(pDX, IDC_RELOAD, m_reload);
	DDX_Control(pDX, IDC_EXPORT, m_export);
	DDX_Text(pDX, IDC_point, m_point);
	DDX_Text(pDX, IDC_xiao, m_xiao);
	DDX_Text(pDX, IDC_da, m_da);
	DDX_Text(pDX, IDC_ACC, m_acc);
	DDX_Text(pDX, IDC_HOUGH, m_hough);
	DDX_Text(pDX, IDC_erodeNum, m_erodeNum);
	DDX_Text(pDX, IDC_erodeNum2, m_xiangsidu);
	DDX_Control(pDX, IDC_SLIDER5, m_threshold);
	DDX_Slider(pDX, IDC_SLIDER5, m_iThresholdValue);
	DDX_Control(pDX, IDC_SELIMAGE, m_selImage);
}

BEGIN_MESSAGE_MAP(CForTestDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_OPEN_CAM, &CForTestDlg::OnBnClickedOpenCam)
	ON_WM_CTLCOLOR()
	ON_WM_HSCROLL()
	ON_CBN_SELCHANGE(IDC_RESOLUTION, &CForTestDlg::OnCbnSelchangeResolution)
	ON_BN_CLICKED(IDC_DEFAULT, &CForTestDlg::OnBnClickedDefault)
	ON_BN_CLICKED(IDC_EXIT, &CForTestDlg::OnBnClickedExit)
	ON_BN_CLICKED(IDC_ScanImgPath, &CForTestDlg::OnBnClickedScanimgpath)
	ON_BN_CLICKED(IDC_Orthogonal, &CForTestDlg::OnBnClickedOrthogonal)
	ON_BN_CLICKED(IDC_AutoExp, &CForTestDlg::OnBnClickedAutoexp)
	ON_BN_CLICKED(IDC_SNAP, &CForTestDlg::OnBnClickedSnap)
	ON_BN_CLICKED(IDC_WHITEBALANCE, &CForTestDlg::OnBnClickedWhitebalance)
	ON_BN_CLICKED(IDC_CONFIRM, &CForTestDlg::OnBnClickedConfirm)
	ON_BN_CLICKED(IDC_RIGHT, &CForTestDlg::OnBnClickedRight)
	ON_BN_CLICKED(IDC_LEFT, &CForTestDlg::OnBnClickedLeft)
	ON_BN_CLICKED(IDC_UP, &CForTestDlg::OnBnClickedUp)
	ON_BN_CLICKED(IDC_DOWN, &CForTestDlg::OnBnClickedDown)
	ON_BN_CLICKED(IDC_GOTO, &CForTestDlg::OnBnClickedGoto)
	ON_BN_CLICKED(IDC_CHECK3, &CForTestDlg::OnBnClickedCheck3)
	ON_BN_CLICKED(IDC_STEPSET, &CForTestDlg::OnBnClickedStepset)
	ON_BN_CLICKED(IDC_STOP, &CForTestDlg::OnBnClickedStop)
	ON_CBN_SELCHANGE(IDC_OBJECTLENS, &CForTestDlg::OnCbnSelchangeObjectlens)
	ON_BN_CLICKED(IDC_StitchPathSet, &CForTestDlg::OnBnClickedStitchpathset)
	ON_BN_CLICKED(IDC_DETECT, &CForTestDlg::OnBnClickedDetect)
	ON_BN_CLICKED(IDC_GOTO2, &CForTestDlg::OnBnClickedGoto2)
	ON_BN_CLICKED(IDC_BEGIN, &CForTestDlg::OnBnClickedBegin)
	ON_BN_CLICKED(IDC_MATCH, &CForTestDlg::OnBnClickedMatch)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RECOGNITION, &CForTestDlg::OnBnClickedRecognition)
	ON_BN_CLICKED(IDC_RELOAD, &CForTestDlg::OnBnClickedReload)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &CForTestDlg::OnNMClickList1)
	ON_BN_CLICKED(IDC_open, &CForTestDlg::OnBnClickedopen)
	ON_BN_CLICKED(IDC_close, &CForTestDlg::OnBnClickedclose)
	ON_BN_CLICKED(IDC_erode, &CForTestDlg::OnBnClickederode)
	ON_BN_CLICKED(IDC_dilate, &CForTestDlg::OnBnClickeddilate)
	ON_BN_CLICKED(IDC_dilate2, &CForTestDlg::OnBnClickeddilate2)
	ON_BN_CLICKED(IDC_BUTTON2, &CForTestDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CForTestDlg::OnBnClickedButton3)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_rmBackgroud, &CForTestDlg::OnBnClickedrmbackgroud)
//	ON_BN_CLICKED(IDC_CHECK4, &CForTestDlg::OnBnClickedCheck4)
	ON_BN_CLICKED(IDC_edgeEnhance, &CForTestDlg::OnBnClickededgeenhance)
	ON_BN_CLICKED(IDC_BUTTON5, &CForTestDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_EXPORT, &CForTestDlg::OnBnClickedExport)
	ON_BN_CLICKED(IDC_COUNTDEFAULT, &CForTestDlg::OnBnClickedCountdefault)
	ON_BN_CLICKED(IDC_CHECK4, &CForTestDlg::OnBnClickedCheck4)
	ON_BN_CLICKED(IDC_BUTTON4, &CForTestDlg::OnBnClickedButton4)
	ON_CBN_SELCHANGE(IDC_SELIMAGE, &CForTestDlg::OnCbnSelchangeSelimage)
	ON_BN_CLICKED(IDC_BUTTON6, &CForTestDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CForTestDlg::OnBnClickedButton7)
END_MESSAGE_MAP()


// CForTestDlg ��Ϣ�������

BOOL CForTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_B1.CreateSolidBrush(RGB(45, 45, 45));
	m_B2.CreateSolidBrush(RGB(70, 70, 70));
	m_B3.CreateSolidBrush(RGB(75, 75, 75));
	m_B4.CreateSolidBrush(RGB(75, 75, 75));

	//�����ڱ���
	CString text ;
	text.Format(_T("�й���ѧԺ�Ϻ�΢ϵͳ����Ϣ�����о���"));
	this->SetWindowText(text);
	
	//��ť�ؼ�������
	m_openCam.SetShade(CShadeButtonST::SHS_HSHADE, 5, 20, 30, RGB(70, 70, 70));
	m_openCam.DrawFlatFocus(TRUE);
	m_snap.SetShade(CShadeButtonST::SHS_HSHADE, 5, 20, 30, RGB(70, 70, 70));
	m_snap.DrawFlatFocus(TRUE);
	m_exit.SetShade(CShadeButtonST::SHS_HSHADE, 5, 20, 30, RGB(70, 70, 70));
	m_exit.DrawFlatFocus(TRUE);
	m_up.SetShade(CShadeButtonST::SHS_HSHADE, 5, 20, 30, RGB(70, 70, 70));
	m_up.DrawFlatFocus(TRUE);
	m_down.SetShade(CShadeButtonST::SHS_HSHADE, 5, 20, 30, RGB(70, 70, 70));
	m_down.DrawFlatFocus(TRUE);
	m_left.SetShade(CShadeButtonST::SHS_HSHADE, 5, 20, 30, RGB(70, 70, 70));
	m_left.DrawFlatFocus(TRUE);
	m_right.SetShade(CShadeButtonST::SHS_HSHADE, 5, 20, 30, RGB(70, 70, 70));
	m_right.DrawFlatFocus(TRUE);
	m_orthogonal.SetShade(CShadeButtonST::SHS_HSHADE, 5, 20, 30, RGB(70, 70, 70));
	m_orthogonal.DrawFlatFocus(TRUE);
	m_whitebalance.SetShade(CShadeButtonST::SHS_HSHADE, 5, 20, 30, RGB(70, 70, 70));
	m_whitebalance.DrawFlatFocus(TRUE);
	m_SpdSet.SetShade(CShadeButtonST::SHS_HSHADE, 5, 20, 30, RGB(70, 70, 70));
	m_SpdSet.DrawFlatFocus(TRUE);
	m_ImagePathSet.SetShade(CShadeButtonST::SHS_HSHADE, 5, 20, 30, RGB(70, 70, 70));
	m_ImagePathSet.DrawFlatFocus(TRUE);
	m_goto.SetShade(CShadeButtonST::SHS_HSHADE, 5, 20, 30, RGB(70, 70, 70));
	m_goto.DrawFlatFocus(TRUE);
	m_goto2.SetShade(CShadeButtonST::SHS_HSHADE, 5, 20, 30, RGB(70, 70, 70));
	m_goto2.DrawFlatFocus(TRUE);
	m_default.SetShade(CShadeButtonST::SHS_HSHADE, 5, 20, 30, RGB(70, 70, 70));
	m_default.DrawFlatFocus(TRUE);
	m_stepSet.SetShade(CShadeButtonST::SHS_HSHADE, 5, 20, 30, RGB(70, 70, 70));
	m_stepSet.DrawFlatFocus(TRUE);
	m_stop.SetShade(CShadeButtonST::SHS_HSHADE, 5, 20, 30, RGB(70, 70, 70));
	m_stop.DrawFlatFocus(TRUE);
	m_stitchPathSet.SetShade(CShadeButtonST::SHS_HSHADE, 5, 20, 30, RGB(70, 70, 70));
	m_stitchPathSet.DrawFlatFocus(TRUE);
	m_detect.SetShade(CShadeButtonST::SHS_HSHADE, 5, 20, 30, RGB(70, 70, 70));
	m_detect.DrawFlatFocus(TRUE);
	m_countDefault.SetShade(CShadeButtonST::SHS_HSHADE, 5, 20, 30, RGB(70, 70, 70));
	m_countDefault.DrawFlatFocus(TRUE);
	m_begin.SetShade(CShadeButtonST::SHS_HSHADE, 5, 20, 30, RGB(70, 70, 70));
	m_begin.DrawFlatFocus(TRUE);
	m_match.SetShade(CShadeButtonST::SHS_HSHADE, 5, 20, 30, RGB(70, 70, 70));
	m_match.DrawFlatFocus(TRUE);
	m_recognition.SetShade(CShadeButtonST::SHS_HSHADE, 5, 20, 30, RGB(70, 70, 70));
	m_recognition.DrawFlatFocus(TRUE);
	m_reload.SetShade(CShadeButtonST::SHS_HSHADE, 5, 20, 30, RGB(70, 70, 70));
	m_reload.DrawFlatFocus(TRUE);
	m_export.SetShade(CShadeButtonST::SHS_HSHADE, 5, 20, 30, RGB(70, 70, 70));
	m_export.DrawFlatFocus(TRUE);

	//�ؼ�λ�ÿ���
	SetDisplayWindow();

	SetDlgItemText(IDC_TEXT, _T("    �п�Ժ�Ϻ�΢ϵͳ����Ϣ�����о�����ӭ���� "));
	//������
	SetDlgItemText(IDC_URL, _T("  ��ѯ���¼: http://sim.cas.cn"));
	m_url.SetURL(_T("http://sim.cas.cn"));
	SetTimer(1, 10000,NULL);//10s�Ӻ��Ϊ���ɼ�

	if (m_pGalil != NULL)
	{
		m_curX = (double)m_pGalil->commandValue("TPA");
		m_curY = (double)m_pGalil->commandValue("TPB");
		m_curX = m_curX / 2000;
		m_curY = m_curY / 2000;

		m_SpeedX = (double)m_pGalil->commandValue("SPA=?");
		m_SpeedX = m_SpeedX / 10000;
		m_SpeedY = (double)m_pGalil->commandValue("SPB=?");
		m_SpeedY = m_SpeedY / 10000;

		CString spx, spy, curx, cury;
		spx.Format(_T("%0.3f"), m_SpeedX);//ת��Ϊstring���ͣ�����3λС��
		GetDlgItem(IDC_SPEEDX)-> SetWindowText(spx);//������������༭��
		spy.Format(_T("%0.3f"), m_SpeedX);
		GetDlgItem(IDC_SPEEDY)-> SetWindowText(spy);
		curx.Format(_T("%0.3f"), m_curX);
		GetDlgItem(IDC_CURX)-> SetWindowText(curx);
		cury.Format(_T("%0.3f"), m_curY);
		GetDlgItem(IDC_CURY)-> SetWindowText(cury);

		m_pGalil->command("ST");
		Sleep(200);
		m_pGalil->command("MO");
		m_Thread = AfxBeginThread((AFX_THREADPROC)MyThreadProc, this);
		//m_Thread = CreateThread(NULL, 0, MyThreadProc, this, 0, NULL);
	}
	//����galil�ؼ���ʹ��
	else if (NULL == m_pGalil)
	{
		//GetDlgItem(IDC_CHECK2)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK3)->EnableWindow(FALSE);
		GetDlgItem(IDC_UP)->EnableWindow(FALSE);
		GetDlgItem(IDC_DOWN)->EnableWindow(FALSE);
		GetDlgItem(IDC_RIGHT)->EnableWindow(FALSE);
		GetDlgItem(IDC_LEFT)->EnableWindow(FALSE);
		GetDlgItem(IDC_CURY)->EnableWindow(FALSE);
		GetDlgItem(IDC_CURX)->EnableWindow(FALSE);
		GetDlgItem(IDC_TARGETX)->EnableWindow(FALSE);
		GetDlgItem(IDC_TARGETY)->EnableWindow(FALSE);
		GetDlgItem(IDC_CONFIRM)->EnableWindow(FALSE);
		GetDlgItem(IDC_STEPSET)->EnableWindow(FALSE);
		GetDlgItem(IDC_GOTO)->EnableWindow(FALSE);
		GetDlgItem(IDC_STOP)->EnableWindow(FALSE);
		GetDlgItem(IDC_BEGIN)->EnableWindow(FALSE);
	}

	//-------��ȡ�����ļ� http://www.doc88.com/p-740821465722.html
	GetGlobalPath();
	GetPrivateProfileString(_T("PathInfo"), _T("ImageDataPath"), _T("E:\\test"), m_ImagePath.GetBuffer(256), 256, sPath);
	GetDlgItem(IDC_ScanPath)-> SetWindowText(m_ImagePath);//������������༭��
	GetPrivateProfileString(_T("PathInfo"), _T("ImageStitchPath"), _T("E:\\test"), m_stitchPath.GetBuffer(256), 256, sPath);
	GetDlgItem(IDC_StitchPath)-> SetWindowText(m_stitchPath);//������������༭��
	sImagePath = m_ImagePath;
	CString targetX, targetY;
	targetX.Format(_T("%0.3f"), m_targetX);//ת��Ϊstring���ͣ�����3λС��
	targetY.Format(_T("%0.3f"), m_targetY);//ת��Ϊstring���ͣ�����3λС��	
	GetPrivateProfileString(_T("TargetData"), _T("X"), _T("0"), targetX.GetBuffer(256), 256, sPath);
	GetPrivateProfileString(_T("TargetData"), _T("Y"), _T("0"), targetY.GetBuffer(256), 256, sPath);
	//CString scanRow, scanCol;
	//scanRow.Format(_T("%d"), m_scanRows);
	//GetPrivateProfileString(_T("ScanInfo"), _T("ScanRow"),  _T("4"), scanRow.GetBuffer(256), 256, sPath);
	//scanCol.Format(_T("%d"), m_scanCols);
	//GetPrivateProfileString(_T("ScanInfo"), _T("ScanCol"),  _T("4"), scanCol.GetBuffer(256), 256, sPath);

	//����combobox�ĳ�ʼ������
	BYTE CamAllNum = 0;
	BYTE Resolution = 0;
	BYTE CapResolution = 0;
	CameraGetMultiCameraNumber(&CamAllNum, &Resolution, &CapResolution);
	//û�з������
	if (CamAllNum == 0)
	{
		GetDlgItem(IDC_OPEN_CAM)->EnableWindow(FALSE);
		GetDlgItem(IDC_SLIDER1)->EnableWindow(FALSE);
		GetDlgItem(IDC_SLIDER2)->EnableWindow(FALSE);
		GetDlgItem(IDC_SLIDER3)->EnableWindow(FALSE);
		GetDlgItem(IDC_EXIT)->EnableWindow(FALSE);
	}
	else
	{
		for(int i=0; i<CamAllNum; i++)
		{
			CString xtmp;
			xtmp.Format(_T("%s #%d"), _T(MODEL_NAME),i+1);
			m_MultiCamList.AddString(xtmp);
		}

		m_MultiCamList.SetCurSel(0);	
		m_resolution.AddString(_T(RESOLUTION_A_TEXT));
		m_resolution.AddString(_T(RESOLUTION_B_TEXT));
		m_resolution.AddString(_T(RESOLUTION_C_TEXT));
		m_resolution.SetCurSel(1);

		m_objectlens.AddString(_T("4X"));
		m_objectlens.AddString(_T("10X"));
		m_objectlens.AddString(_T("20X"));
		m_objectlens.AddString(_T("40X"));
		m_objectlens.SetCurSel(0);
		if (m_pGalil != NULL)
		{
			m_Row = 2.37;
			m_Col = 1.7825;
		}

		nWidth = 1024;
		nHeight = 768;
		m_Resolution = R1024_768;

		//3���������ĳ�ʼ������
		m_Contrast.SetRange(0, 255);
		m_target.SetRange(0, 255);
		ushort uiMaxexpTime = 1000;
		CameraGetMaxExposureTime(&uiMaxexpTime);
		m_exposureTime.SetRange(1, uiMaxexpTime);
		CameraLoadDefault();

		//�����������
		CameraSetFrameSpeed(FRAME_SPEED_HIGH);
	}
	m_threshold.SetRange(1, 255);
	m_threshold.SetPos(191);
	SetDlgItemInt(IDC_EDIT1, 191, TRUE);

	//�ؼ���ʹ��
	GetDlgItem(IDC_SNAP)->EnableWindow(FALSE);
	GetDlgItem(IDC_EXIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_Orthogonal)->EnableWindow(FALSE);
	GetDlgItem(IDC_SPEEDX)->EnableWindow(FALSE);
	GetDlgItem(IDC_SPEEDY)->EnableWindow(FALSE);
	GetDlgItem(IDC_STEPX)->EnableWindow(FALSE);
	GetDlgItem(IDC_STEPY)->EnableWindow(FALSE);
	GetDlgItem(IDC_WHITEBALANCE)->EnableWindow(FALSE);
	GetDlgItem(IDC_DEFAULT)->EnableWindow(FALSE);
	GetDlgItem(IDC_AutoExp)->EnableWindow(FALSE);

	////��Ƶ����
	//GetDlgItem(IDC_VIDEO)->SetWindowPos(NULL, 30,65, 944,708,SWP_NOZORDER | SWP_SHOWWINDOW);
	//�Զ�ɨ���ʼ��
	CString note;
	note.Format(_T("��ʾ��%d���ﾵ��һ��ͼƬ��ΧΪ%0.3f * %0.3f mm"), lens, m_Row, m_Col);
	GetDlgItem(IDC_NOTE)->SetWindowText(note);
	GetDlgItem(IDC_SCANROWS)->EnableWindow(FALSE);
	GetDlgItem(IDC_SCANCOLS)->EnableWindow(FALSE);	

	//��ͼƬ�ĳ�ʼ��
	CvSize ImgSize;
	ImgSize.height = 708;
	ImgSize.width = 944;
	displayImage = cvCreateImage(ImgSize, IPL_DEPTH_8U, 3);
	//����������
	m_threashold.SetRange(1,255);
	m_threashold.SetPos(51);
	nThreashold = 51;
	SetDlgItemInt(IDC_STATIC4, 51);

	//ͳ�Ʊ��ĳ�ʼ��
	//CRect rect;
	//m_tableWidget.GetClientRect(&rect);
	//m_tableWidget.SetExtendedStyle(m_tableWidget.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	//m_tableWidget.InsertColumn(0, _T("���"), LVCFMT_CENTER, rect.Width()/4, 0);
	//m_tableWidget.InsertColumn(1, _T("����"), LVCFMT_CENTER, rect.Width()/4, 1);
	//m_tableWidget.InsertColumn(2, _T("���"), LVCFMT_CENTER, rect.Width()/4, 2);
	//m_tableWidget.InsertColumn(3, _T("Բ��"), LVCFMT_CENTER, rect.Width()/4, 3);//Ϊ�˷�����ʾ��飬����Բ������ᣬ��ʾ����
	//m_tableWidget.InsertColumn(3, _T("Բ����"), LVCFMT_CENTER, rect.Width()/4, 3);
	//m_tableWidget.InsertItem(0, _T("Java"));
	//m_tableWidget.SetItemText(0, 1, _T("1"));
	//m_tableWidget.SetItemText(0, 2, _T("1"));
	//m_tableWidget.SetItemText(0, 3, _T("1"));
	m_tableWidget.SetExtendedStyle(m_tableWidget.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_tableWidget.SetHeadings(_T("���,90;Բ��,105;ƽ���Ҷ�,90;�뾶,90"));
	m_tableWidget.LoadColumnInfo();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�
void CForTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		//-----------------start-------���ñ���ɫ
		CRect rect;
		CPaintDC dc(this);
		GetClientRect(rect);
		dc.FillSolidRect(rect, RGB(83, 83, 83));//(60, 60, 60)
		dc.FillPath();/**/
		//-----------------end---------���ñ���ɫ

		CDialogEx::OnPaint();

		//����Windows���ڣ�������ⲽ���ã�ͼƬ��ʾ�����������
		UpdateWindow();
		//ShowImage(DetectImage, IDC_VIDEO);
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CForTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



//�Զ����һЩ����
void CForTestDlg::UpdateExposurteTimeText()
{
	UINT RawTime;
	CString sTemp;

	CameraGetRowTime(&RawTime);
	sTemp.Format(_T("%.3f ms"),((float)m_expTime*RawTime/1000));//����3λС������msΪ��λ���ع�ʱ��
	GetDlgItem(IDC_SLIDER3)->SetWindowText(sTemp);
}


void CForTestDlg::StartDevice()
{

	DS_CAMERA_STATUS st = CameraGetImageSize(&nWidth, &nHeight);
	if (st != STATUS_OK)
	{
		m_RunMode = STATUS_INTERNAL_ERROR;
		return;
	}

	m_header.biWidth = nWidth;
	m_header.biHeight = nHeight;
	m_header.biSizeImage = TDIBWIDTHBYTES(nWidth * 24) * nHeight;
	if (m_pImageData)
	{
		free(m_pImageData);
		m_pImageData = NULL;
	}
	m_pImageData = (BYTE *)malloc(m_header.biSizeImage);

	bool bEnableAutoExpo = true;
	CameraSetAeState(bEnableAutoExpo);
	CheckDlgButton(IDC_AutoExp, bEnableAutoExpo ? 1:0);
	GetDlgItem(IDC_SLIDER3)->EnableWindow(!bEnableAutoExpo);

	UpdateSetupWindow();
	//UpdateExposurteTimeText();
	//GetDlgItem(IDC_VIDEO)->UpdateWindow();
}

void CForTestDlg::UpdateSetupWindow()
{
	int iTemp;
	BYTE bTemp, contrast;
	CString sTemp;
	CameraGetExposureTime(&iTemp);
	CameraGetRowTime(&RawTime);
	m_expTime = iTemp;
	CameraGetAeTarget(&bTemp);
	CameraGetContrast(&contrast);
	m_exposureTime.SetPos(m_expTime);
	m_target.SetPos(bTemp);
	m_Contrast.SetPos(contrast);

	sTemp.Format(_T("%.3f ms"),((float)m_expTime*RawTime/1000));//����3λС������msΪ��λ���ع�ʱ��
	GetDlgItem(IDC_STATIC3)->SetWindowText(sTemp);

	//CString stTemp;
	//stTemp.Format(_T("%.3f"), m_expTime);
	//GetDlgItem(IDC_STATIC3)->SetWindowText(stTemp);
	sTemp.Format(_T("%d"), bTemp);
	GetDlgItem(IDC_STATIC1)->SetWindowText(sTemp);
	sTemp.Format(_T("%d"), contrast);
	GetDlgItem(IDC_STATIC2)->SetWindowText(sTemp);
}

//ͳһ����Ϊ944*708
void CForTestDlg::ResizeImage(IplImage* img)
{
	int w = img->width;
	int h = img->height;
	//int max = (w > h)? w: h;

	float scalew = (float) ((float)w / 944.0f);
	float scaleh = (float) ((float)h / 708.0f);
	float scale = (scalew > scaleh)? scalew : scaleh;
	//float scale = (float) ( (float) max / 256.0f );
	int nw = (int)( w / scale );//���ź�Ŀ�͸�
	int nh = (int)( h / scale );

	// Ϊ�˽����ź��ͼƬ���� TheImage �����в�λ�������ͼƬ�� TheImage ���Ͻǵ���������ֵ
	//int tlx = (nw > nh)? 0: (int)(944-nw)/2;
	//int tly = (nw > nh)? (int)(708-nh)/2: 0;

	cvSetImageROI( displayImage, cvRect( 0, 0, nw, nh) );
	cvResize( img, displayImage );
	cvResetImageROI( displayImage );

}
Point2i CForTestDlg::ReverseSize(Point2i point)
{
	Point2i pt;
	float scaleX = (float)((float)point.x / 944.0f);
	float scaleY = (float) ((float)point.y	/ 708.0f);
	pt.x  = (int)(dstImage->width * scaleX);
	pt.y  = (int)(dstImage->height * scaleY);

	return pt;
}

void CForTestDlg::ShowImage(IplImage* img, UINT ID)
{
	//�����ʾ�ؼ���DC
	CDC* pDC = GetDlgItem(ID)->GetDC();
	//��ȡHDC���豸����������л�ͼ����
	HDC hDC = pDC->GetSafeHdc();

	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);
	//����ؼ���ͼƬ�Ŀ�͸�
	int rw = rect.right - rect.left;			
	int rh = rect.bottom - rect.top;
	int iw = img->width;	
	int ih = img->height;

	// ʹͼƬ����ʾλ�������ڿؼ�������
	int tx = (int)(rw - iw)/2;	
	int ty = (int)(rh - ih)/2;
	SetRect( rect, tx, ty, tx+iw, ty+ih );
	// ����ͼƬ
	CvvImage cimg;
	cimg.CopyOf( img );
	// ��ͼƬ���Ƶ���ʾ�ؼ���ָ��������	
	cimg.DrawToHDC( hDC, &rect );	

	ReleaseDC( pDC );

}
void CForTestDlg::ShowImage(IplImage* img)
{
	if (displayImage)
	{
		cvZero(displayImage);
	}
	cvReleaseImage(&savedImage);
	savedImage = cvCreateImage(cvGetSize(img), img->depth, 3);
	if (img->nChannels == 1)
	{
		cvCvtColor(img, savedImage, CV_GRAY2RGB);
	}
	else if (img->nChannels == 3)
	{
		cvCopy(img, savedImage);
	}
	ResizeImage(savedImage);
	ShowImage(displayImage, IDC_VIDEO);

}

HBRUSH CForTestDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����

	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	switch (pWnd->GetDlgCtrlID())
	{
	case IDC_STATIC:
	case IDC_STATIC5:
	case IDC_STATIC6:
	case IDC_STATIC7:
	//case IDC_STITCH8:
	case IDC_STATIC9:
	case IDC_STATIC10:
	case IDC_STATIC11:
	case IDC_STATIC12:
	case IDC_STATIC13:
	case IDC_STATIC14:
	case IDC_STATIC15:
	case IDC_STATIC16:
	case IDC_STATIC17:
	case IDC_STATIC18:
	case IDC_STATIC19:
	case IDC_STATIC20:
	case IDC_STATIC21:
	case IDC_STATIC22:
	case IDC_STATIC23:
	case IDC_STATIC24:
	case IDC_STATIC25:
	case IDC_STATIC26:
	case IDC_STATIC27:
	case IDC_STATIC28:
	case IDC_STATIC29:
	case IDC_STATIC30:
	case IDC_STATIC31:
	case IDC_STATIC32:
	case IDC_STATIC33:
	case IDC_STITCH:
	case IDC_STATIC1:
	case IDC_STATIC2:
	case IDC_STATIC3:
	case IDC_STATIC4:
	case IDC_TEXT:
	case IDC_NOTE:
	case IDC_CHECK3:
	case IDC_SLIDER1:
	case IDC_SLIDER2:
	case IDC_SLIDER3:
	case IDC_SLIDER4:
	case IDC_AutoExp:
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(RGB(255, 255, 255));
			return (HBRUSH)m_B4.GetSafeHandle();
			break;
		}
	case IDC_SPEEDX:
	case IDC_SPEEDY:
	case IDC_STEPX:
	case IDC_STEPY:
	case IDC_CURX:
	case IDC_CURY:
	case IDC_TARGETX:
	case IDC_TARGETY:
	case IDC_ScanPath:
	case IDC_StitchPath:
	case IDC_MINRADIUS:
	case IDC_MAXRADIUS:
	case IDC_SCANROWS:
	case IDC_SCANCOLS:
//	case IDC_URL:
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(RGB(230, 230, 230));
			return (HBRUSH)m_B1.GetSafeHandle();
			break;
		}
	case IDC_RESOLUTION:
	case IDC_OBJECTLENS:
	case IDC_SEL_CAM:
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(RGB(230, 230, 230));
			return (HBRUSH)m_B3.GetSafeHandle();
			break;			
		}
	//case IDC_URL:
	//	{
	//		pDC->SetBkMode(TRANSPARENT);
	//		pDC->SetTextColor(RGB(230, 230, 230));
	//		return (HBRUSH)m_B2.GetSafeHandle();
	//		break;			
	//	}
	default:
		return hbr;

	}

	return hbr;
}


void CForTestDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (pScrollBar == GetDlgItem(IDC_SLIDER3))
	{
		int nTime = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER3))->GetPos();
		CameraSetExposureTime(nTime);
		CString sTemp;
		sTemp.Format(_T("%.3f ms"),((float)nTime*RawTime/1000));//����3λС������msΪ��λ���ع�ʱ��
		GetDlgItem(IDC_STATIC3)->SetWindowText(sTemp);

	}
	if (pScrollBar == GetDlgItem(IDC_SLIDER2))
	{
		int contrastValue = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER2))->GetPos();
		CameraSetContrast(contrastValue);
		SetDlgItemInt(IDC_STATIC2, contrastValue, TRUE);
	}
	if (pScrollBar == GetDlgItem(IDC_SLIDER1))
	{
		int nTemp = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER1))->GetPos();
		CameraSetAeTarget(nTemp);
		SetDlgItemInt(IDC_STATIC1, nTemp, TRUE);
	}
	if (pScrollBar == GetDlgItem(IDC_SLIDER4))
	{
		nThreashold = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER4))->GetPos();
		if (nThreashold%2 == 1)
		{
			SetDlgItemInt(IDC_STATIC4, nThreashold, TRUE);
			OnBnClickedRecognition();
		}
	}
	if (pScrollBar == GetDlgItem(IDC_SLIDER5))
	{
		int n = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER5))->GetPos();
		SetDlgItemInt(IDC_EDIT1, n, TRUE);
		m_iThresholdValue = n;
		OnBnClickedrmbackgroud();
	}
	
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CForTestDlg::OnBnClickedOpenCam()
{
	//CRect rect1, rect2, rect3; 
	//GetDlgItem(IDC_URL )->GetWindowRect(&rect1);
	//GetDlgItem(IDC_STATIC32 )->GetWindowRect(&rect2);
	//GetDlgItem(IDC_STATIC33 )->GetWindowRect(&rect3);
	//CString text;
	//text.Format(_T("����=%d,%d;cx=%d;cy=%d,����=%d,%d;cx=%d;cy=%d,����=%d,%d;cx=%d;cy=%d"),
	//			rect1.TopLeft().x, rect1.TopLeft().y, rect1.Width(), rect1.Height(),
	//			rect2.TopLeft().x, rect2.TopLeft().y, rect2.Width(), rect2.Height(),
	//			rect3.TopLeft().x, rect3.TopLeft().y, rect3.Width(), rect3.Height());
	//SetDlgItemText(IDC_TEXT, text);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_RunMode == RUNMODE_PLAY)
	{
		CameraStop();
		CameraUnInit();
		m_RunMode = RUNMODE_STOP;

		SetDlgItemText(IDC_OPEN_CAM, _T("�����"));
		GetDlgItem(IDC_OPEN_CAM)->SetFocus();
		GetDlgItem(IDC_SNAP)->EnableWindow(FALSE);
		GetDlgItem(IDC_Orthogonal)->EnableWindow(FALSE);
		GetDlgItem(IDC_WHITEBALANCE)->EnableWindow(FALSE);
		GetDlgItem(IDC_DEFAULT)->EnableWindow(FALSE);
		GetDlgItem(IDC_AutoExp)->EnableWindow(FALSE);

		return;
	}
	//else
	//{
	BYTE CamNum = m_MultiCamList.GetCurSel();

	if (m_MultiCamList.GetCount() == 0)
	{
		CamNum = 0;
	}
	//if (R_ROI == m_Resolution)
	//{
	//	CameraSetROI(nWidth, nHeight, nWidth, nHeight);
	//}
	if (STATUS_OK != CameraInit(SnapThreadCallback, 
		R1024_768,
		GetDlgItem(IDC_VIDEO)->m_hWnd, //NULL,//
		CamNum+1,
		this->m_hWnd) )
	{
		MessageBox(_T("�����ʼ��ʧ��"), _T("����"), MB_OK | MB_ICONSTOP);
		return ;
	}

	//	StartDevice();
	//}
	DS_CAMERA_STATUS op =  CameraPlay();
	if (op == STATUS_NO_DEVICE_FIND)
	{
		AfxMessageBox(_T("û�з������"));
		return;
	}
	m_RunMode = RUNMODE_PLAY;	
	ImageOpenFlag = false;
	SetDlgItemText(IDC_OPEN_CAM, _T("�ر����"));
	GetDlgItem(IDC_SNAP)->EnableWindow(TRUE);
	GetDlgItem(IDC_Orthogonal)->EnableWindow(TRUE);
	GetDlgItem(IDC_WHITEBALANCE)->EnableWindow(TRUE);
	GetDlgItem(IDC_DEFAULT)->EnableWindow(TRUE);
	GetDlgItem(IDC_AutoExp)->EnableWindow(TRUE);
	GetDlgItem(IDC_EXIT)->EnableWindow(TRUE);
	StartDevice();
}

void CForTestDlg::OnCbnSelchangeResolution()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int n = m_resolution.GetCurSel();
	if (n == 0)
	{
		nWidth = 2048;
		nHeight = 1536;
		m_Resolution = R2048_1536;
	}
	if (n == 1)
	{
		nWidth = 1024;
		nHeight = 768;
		m_Resolution = R1024_768;
	}
	if (n == 2)
	{
		nWidth = 640;
		nHeight = 480;
		m_Resolution = R640_480;
	}
	if (m_RunMode == STATUS_OK)
	{
		DS_CAMERA_STATUS st = CameraSetROI(nWidth, nHeight, nWidth, nHeight);
		if (st != STATUS_OK)
		{
			return;
		}
		StartDevice();
	}
}


void CForTestDlg::OnBnClickedDefault()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CameraLoadDefault();
	UpdateSetupWindow();
}


void CForTestDlg::OnBnClickedExit()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString sDisplay;
	sDisplay.Format(_T("��ǰ��΢������%d���ﾵ��,��ȷ����ӱ����"), lens);
	(MessageBox(sDisplay, _T("ѡ���ﾵ"), MB_OKCANCEL) == IDOK)? (nScale = 1):(nScale = 0);
	
}


void CForTestDlg::OnBnClickedScanimgpath()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	// MFC����ļ��в�����·�� http://blog.csdn.net/wutaozhao/article/details/7057402 

	CString    strFolderPath;  
	BROWSEINFO broInfo = {0};  
	TCHAR      szDisName[MAX_PATH] = {0};  

	broInfo.hwndOwner = this->m_hWnd;  
	broInfo.pidlRoot  = NULL;  //�ļ��жԻ����Ŀ¼
	broInfo.pszDisplayName = szDisName;  
	broInfo.lpszTitle = _T("ѡ�񱣴�·��");  
	broInfo.ulFlags   = BIF_NEWDIALOGSTYLE | BIF_DONTGOBELOWDOMAIN | BIF_BROWSEFORCOMPUTER | BIF_RETURNONLYFSDIRS | BIF_RETURNFSANCESTORS;  
	broInfo.lpfn      = NULL;  
	broInfo.lParam    = NULL;  
	broInfo.iImage    = IDR_MAINFRAME;  

	LPITEMIDLIST pIDList = SHBrowseForFolder(&broInfo);  
	if (pIDList != NULL)  
	{  
		memset(szDisName, 0, sizeof(szDisName));  
		SHGetPathFromIDList(pIDList, szDisName);  
		strFolderPath = szDisName;  
	} 
	m_ImagePath = szDisName;
	UpdateData(FALSE);

	//-------�����������ļ�
	::WritePrivateProfileString(_T("PathInfo"), _T("ImageDataPath"), m_ImagePath, sPath);
	sImagePath = m_ImagePath;
}


void CForTestDlg::OnBnClickedOrthogonal()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	(MessageBox(_T("����ʮ���߼�������XYƽ̨�����ԣ��Ƿ����ʮ���ߣ�"),_T("���ʮ����?"),
				MB_OKCANCEL) == IDOK)? (orthogonal = 0) : (orthogonal = 1);
}


void CForTestDlg::OnBnClickedAutoexp()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_RunMode == RUNMODE_PLAY)
		CameraSetAeState(IsDlgButtonChecked(IDC_AutoExp) ? TRUE : FALSE);
	GetDlgItem(IDC_SLIDER3)->EnableWindow(IsDlgButtonChecked(IDC_AutoExp) ? FALSE : TRUE);
}




void CForTestDlg::OnBnClickedSnap()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString sfilename; 
	//char* filename1 = NULL;
	CTime time = CTime::GetCurrentTime();
	sfilename.Format(_T("%s\\P%02d%02d%02d%02d%02d.bmp"), sImagePath, time.GetMonth(), time.GetDay(), 
		time.GetHour(), time.GetMinute(), time.GetSecond());

	filename = transToMultibyte(sfilename);

#if 0
	int Width = 0;
	int Height= 0;
	if (STATUS_OK == CameraGetImageSize(&Width, &Height))
	{
		int nImageSize = Width*Height;
		BYTE* BmpBuffer   = (BYTE *) malloc(nImageSize * 3 + 512);
		//BYTE* BmpBuffer   = (BYTE *) malloc(TDIBWIDTHBYTES(Width * 24)* Height);

		if (BmpBuffer != NULL)
		{	
			CameraCaptureToBuf(BmpBuffer, DATA_TYPE_RGB24);		
			SavePictureFile(BmpBuffer, filename1, Width, Height);
			//SaveBmpFile(pp,BmpBuffer,Width,Height);
			//Save_image = DNA_CreatImage(3, Height, Width);
			//filename1 = (LPSTR)pp;	
			//Save_image->pixels = BmpBuffer;
			//DNA_SaveImage(Save_image, filename1);
			//Save_image = NULL;
			//DNA_freeImage(Save_image);
		}
		free(BmpBuffer);
		BmpBuffer = NULL;
	}
	CameraEnableDeadPixelCorrection();
	CameraSetDisplayWindow(160,120,320,240);	
#else
	if (isCamOpen)
	{
		nSnap = 1;
	}
	else
	{
		CString Name;
		CTime time = CTime::GetCurrentTime();
		Name = time.Format(_T("ʶ��ͼ��%m%d%H%M"));
		CFileDialog dlg(FALSE, _T("bmp"), //��ʾĬ����չ��
						Name, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("*.jpg|*.jpg|*.png|*.png|*.bmp|*.bmp|*.tif|*.tif|*.jpeg|*.jpeg|�����ļ�(*.*)|*.*||"));//||��ʾ������û�к������һ������
		//dlg.m_ofn.lpstrInitialDir = _T("D:\\test");//��ס�������ȷ��Ĭ��·��
		dlg.m_ofn.lpstrTitle = _T("��ǰͼ�����Ϊ");
		if (dlg.DoModal() != IDOK)
		{
			return;
		}
		CString strFilePath;
		strFilePath = dlg.GetPathName();
		char* PathAndName = transToMultibyte(strFilePath);

		cvSaveImage(PathAndName, savedImage);
	}
#endif
}


void CForTestDlg::OnBnClickedWhitebalance()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_RunMode == RUNMODE_PLAY)
	{
		if (nWBFlag == 0)
		{
			CameraSetAWBState(1);
			nWBFlag = 1;
		}
		else
		{
			CameraLoadDefault();
			UpdateSetupWindow();
			nWBFlag = 0;
		}
	}
}


void CForTestDlg::OnBnClickedConfirm()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (nSFlag == 0)
	{
		nSFlag = 1;
		SetDlgItemText(IDC_CONFIRM, _T("ȷ��"));
		GetDlgItem(IDC_SPEEDX)->EnableWindow(TRUE);
		GetDlgItem(IDC_SPEEDY)->EnableWindow(TRUE);		
	}
	else
	{
		nSFlag = 0;
		SetDlgItemText(IDC_CONFIRM, _T("�趨"));
		GetDlgItem(IDC_SPEEDX)->EnableWindow(FALSE);
		GetDlgItem(IDC_SPEEDY)->EnableWindow(FALSE);
		//GetDlgItem(IDC_CONFIRM)->SetFocus();
	
		//��դ�ֱ���500nm,TM=500(ȡ��ʱ��),10000countss/s = 1mm/s
		char str1[30] = { 0 };
		char str2[30] = { 0 };
		char str_xspeed[30] = { 0 };
		char str_yspeed[30] = { 0 };
		char str11[30] = { 0 };
		char str21[30] = { 0 };
		UpdateData(TRUE);

		spA = m_SpeedX * 10000.0;
		acA = m_SpeedX * 100000;

		spB = m_SpeedY * 10000.0;
		acB = m_SpeedY * 100000;

		sprintf(str1, "SPA=%d", spA);
		m_pGalil->command(str1);
		sprintf(str11, "ACA=%d", acA);
		m_pGalil->command(str11);
		sprintf(str11, "DCA=%d", acA);
		m_pGalil->command(str11);

		sprintf(str2, "SPB=%d", spB);
		m_pGalil->command(str2);
		sprintf(str21, "ACB=%d", acB);
		m_pGalil->command(str21);
		sprintf(str21, "DCB=%d", acB);
		m_pGalil->command(str21);

		m_pGalil->command("BN");

		AfxMessageBox(_T("�ٶ����óɹ�"));
	}
}

BOOL CForTestDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	int buID;
	char strxadd[40] = { 0 }, strxsub[40] = { 0 }, stryadd[40] = { 0 }, strysub[40] = { 0 };

	//�ɴ��ھ�����ID�ţ�GetWindowLongΪ��ô��ڵ�ID�š�
	buID = GetWindowLong(pMsg->hwnd, GWL_ID);

	if (!m_pGalil)
	{
		return CDialogEx::PreTranslateMessage(pMsg);
	}
	if (WM_LBUTTONDOWN == pMsg->message)
	{
		if (IDC_RIGHT == buID)
		{
			if (0 == m_StepMode)
			{
				sprintf(strxadd, "JG-%d", spA);
				m_pGalil->command(strxadd);
				m_pGalil->command("SHA");
				m_pGalil->command("BGA");
			}
		}
		else if (buID == IDC_LEFT)
		{
			if (0 == m_StepMode)
			{
				sprintf(strxsub, "JG%d", spA);
				m_pGalil->command(strxsub);
				m_pGalil->command("SHA");
				m_pGalil->command("BGA");
			}
		}
		else if (IDC_UP == buID)
		{
			if (0 == m_StepMode)
			{
				sprintf(stryadd, "JG,-%d", spB);
				m_pGalil->command(stryadd);
				m_pGalil->command("SHB");
				m_pGalil->command("BGB");
			}
		}
		else if (buID == IDC_DOWN)
		{
			if (0 == m_StepMode)
			{
				sprintf(strysub, "JG,%d", spB);
				m_pGalil->command(strysub);
				m_pGalil->command("SHB");
				m_pGalil->command("BGB");
			}
		}
	}
	else if (pMsg->message == WM_LBUTTONUP)
	{
		if ((IDC_RIGHT == buID) || (buID == IDC_LEFT))
		{
			if(0 == m_StepMode)
			{
				m_pGalil->command("STA");
				m_pGalil->command("SHA");

			}
		}
		if ((IDC_UP == buID) || (buID == IDC_DOWN))
		{
			if(0 == m_StepMode)
			{
				m_pGalil->command("STB");
				m_pGalil->command("SHB");

			}
		}
	}
	else if (pMsg->message == WM_KEYDOWN )
	{
		switch(pMsg->wParam)
		{
		case VK_UP: //up
			sprintf(stryadd, "JG,-%d", spB);
			m_pGalil->command(stryadd);
			m_pGalil->command("SHB");
			m_pGalil->command("BGB");
			break;
		case VK_DOWN: //down
			sprintf(strysub, "JG,%d", spB);
			m_pGalil->command(strysub);
			m_pGalil->command("SHB");
			m_pGalil->command("BGB");
			break;
		case VK_LEFT: //left
			sprintf(strxsub, "JG%d", spA);
			m_pGalil->command(strxsub);
			m_pGalil->command("SHA");
			m_pGalil->command("BGA");
			break;
		case VK_RIGHT: //right
			sprintf(strxadd, "JG-%d", spA);
			m_pGalil->command(strxadd);
			m_pGalil->command("SHA");
			m_pGalil->command("BGA");
			break;
		case VK_DELETE:
			if (m_pGalil == NULL)
			{
				AfxMessageBox(_T("������δ����"));
				break;
			}
			/*if (nAutoScan == 1)
			{*/
			if (m_ScanThread != NULL)
			{
				DWORD dwExit = 0;
				GetExitCodeThread(m_ScanThread->m_hThread, &dwExit);
				if (dwExit == STILL_ACTIVE)
				{
					TerminateThread(m_ScanThread->m_hThread, 0);
				}
				delete m_ScanThread;
				MessageBox(_T("ɨ�豻��ֹ��"));
			}
			//}
			m_pGalil->command("ST");
			Sleep(200);
			m_pGalil->command("MO");
			break;
		default: break;
		}
	}
	else if (pMsg->message == WM_KEYUP)
	{
		switch(pMsg->wParam)
		{
		case VK_LEFT:
		case VK_RIGHT:
			m_pGalil->command("STA");
			m_pGalil->command("SHA");

			m_curX = (double)m_pGalil->commandValue("TPA");
			break;
		case VK_UP:
		case VK_DOWN:
			m_pGalil->command("STB");
			m_pGalil->command("SHB");

			m_curY = (double)m_pGalil->commandValue("TPB");
			break;
		default: break;			
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CForTestDlg::OnBnClickedRight()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	char strxadd[40] = { 0 };
	if(1 == m_StepMode)
	{
		GetDlgItem(IDC_LEFT)->EnableWindow(FALSE);
		GetDlgItem(IDC_RIGHT)->EnableWindow(FALSE);
		double curX = (double)m_pGalil->commandValue("TPA");
		sprintf(strxadd,  "pPOSA=%f-%f", curX/2000.0, m_stepX);
		m_pGalil->command(strxadd);
		m_pGalil->command("XQ#MOVEA");
		GetDlgItem(IDC_LEFT)->EnableWindow(TRUE);
		GetDlgItem(IDC_RIGHT)->EnableWindow(TRUE);

	}
}

void CForTestDlg::OnBnClickedLeft()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	char strxsub[40] = { 0 };
	if(1 == m_StepMode)
	{
		GetDlgItem(IDC_LEFT)->EnableWindow(FALSE);
		GetDlgItem(IDC_RIGHT)->EnableWindow(FALSE);
		double curX = (double)m_pGalil->commandValue("TPA");
		sprintf(strxsub,  "pPOSA=%f+%f", curX/2000.0, m_stepX);
		m_pGalil->command(strxsub);
		m_pGalil->command("XQ#MOVEA");
		GetDlgItem(IDC_LEFT)->EnableWindow(TRUE);
		GetDlgItem(IDC_RIGHT)->EnableWindow(TRUE);

	}
}

void CForTestDlg::OnBnClickedUp()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	char stryadd[40] = { 0 };
	if(1 == m_StepMode)
	{
		GetDlgItem(IDC_UP)->EnableWindow(FALSE);
		GetDlgItem(IDC_DOWN)->EnableWindow(FALSE);
		double curY = (double)m_pGalil->commandValue("TPB");
		sprintf(stryadd, "pPOSB=%f-%f", curY / 2000.0, m_stepY);
		m_pGalil->command(stryadd);
		m_pGalil->command("XQ#MOVEB");
		GetDlgItem(IDC_UP)->EnableWindow(TRUE);
		GetDlgItem(IDC_DOWN)->EnableWindow(TRUE);

	}
}


void CForTestDlg::OnBnClickedDown()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	char strysub[40] = { 0 };
	if(1 == m_StepMode)
	{
		GetDlgItem(IDC_UP)->EnableWindow(FALSE);
		GetDlgItem(IDC_DOWN)->EnableWindow(FALSE);
		double curY = (double)m_pGalil->commandValue("TPB");
		sprintf(strysub, "pPOSB=%f+%f", curY / 2000.0, m_stepY);
		m_pGalil->command(strysub);
		m_pGalil->command("XQ#MOVEB");
		GetDlgItem(IDC_UP)->EnableWindow(TRUE);
		GetDlgItem(IDC_DOWN)->EnableWindow(TRUE);

	}
}


void CForTestDlg::OnBnClickedGoto()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//m_pGalil->command("XQ#AUTO");
	UpdateData(TRUE);
	char strx[40] = { 0 }, stry[40] = { 0 };

	if (m_targetX * 2000  > 80000)
	{
		AfxMessageBox(_T("����X������λ��������ѡ��X������Ŀ��㣡"));
		return;
	}
	else if (m_targetX * 2000 < -80000)
	{
		AfxMessageBox(_T("����X�Ḻ��λ��������ѡ��X�Ḻ��Ŀ��㣡"));
	}
	if (m_targetY * 2000  > 60000)
	{
		AfxMessageBox(_T("����Y������λ��������ѡ��Y������Ŀ��㣡"));
		return;
	}
	else if (m_targetY * 2000 < -60000)
	{
		AfxMessageBox(_T("����Y�Ḻ��λ��������ѡ��Y�Ḻ��Ŀ��㣡"));
		return;
	}

	sprintf(strx, "pPOSA=%f", m_targetX);
	sprintf(stry, "pPOSB=%f", m_targetY);
	m_pGalil->command(strx);
	m_pGalil->command(stry);
	m_pGalil->command("XQ#MOVEAB");

}


void CForTestDlg::OnBnClickedCheck3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_StepMode = IsDlgButtonChecked(IDC_CHECK3)? 0 : 1;
}


void CForTestDlg::OnBnClickedStepset()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (nStep == 0)
	{
		nStep = 1;
		SetDlgItemText(IDC_STEPSET, _T("ȷ��"));
		GetDlgItem(IDC_STEPX)->EnableWindow(TRUE);
		GetDlgItem(IDC_STEPY)->EnableWindow(TRUE);		
	}
	else
	{
		nStep = 0;
		SetDlgItemText(IDC_STEPSET, _T("�趨"));
		GetDlgItem(IDC_STEPX)->EnableWindow(FALSE);
		GetDlgItem(IDC_STEPY)->EnableWindow(FALSE);
		//GetDlgItem(IDC_CONFIRM)->SetFocus();

		UpdateData(TRUE);

		AfxMessageBox(_T("�����������óɹ�"));
	}
}


void CForTestDlg::OnBnClickedStop()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_pGalil == NULL)
	{
		AfxMessageBox(_T("������δ����"));
		return;
	}
	if (m_ScanThread != NULL)
	{
		DWORD dwExit = 0;
		GetExitCodeThread(m_ScanThread->m_hThread, &dwExit);
		if (dwExit == STILL_ACTIVE)
		{
			TerminateThread(m_ScanThread->m_hThread, 0);
		}
		delete m_ScanThread;
		MessageBox(_T("ɨ�豻��ֹ��"));
	}
	m_pGalil->command("ST");
	Sleep(200);
	m_pGalil->command("MO");

}


void CForTestDlg::OnCbnSelchangeObjectlens()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int nSel = m_objectlens.GetCurSel();
	if (nSel == 0)
	{
		lens = 4;
		m_Row = 2.37;
		m_Col = 1.7825;
	}
	if (nSel == 1)
	{
		lens = 10;
		m_Row = 0.948;
		m_Col = 0.713;
	}
	if (nSel == 2)
	{
		lens = 20;
		m_Row = 0.474;
		m_Col = 0.3565;
	}
	if (nSel == 3)
	{
		lens = 40;
		m_Row = 0.237;
		m_Col = 0.17825;
	}
	//�Զ�ɨ����ʾ��Ϣ
	CString note;
	note.Format(_T("��ʾ��%d���ﾵ��һ��ͼƬ��ΧΪ%0.3f * %0.3f mm"), lens, m_Row, m_Col);
	GetDlgItem(IDC_NOTE)->SetWindowText(note);
}


void CForTestDlg::OnBnClickedStitchpathset()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (nDetect == 0)
	{
		CString    strFolderPath;  
		BROWSEINFO broInfo = {0};  
		TCHAR      szDisName[MAX_PATH] = {0};  

		broInfo.hwndOwner = this->m_hWnd;  
		broInfo.pidlRoot  = NULL;  //�ļ��жԻ����Ŀ¼
		broInfo.pszDisplayName = szDisName;  
		broInfo.lpszTitle = _T("ѡ�񱣴�·��");  
		broInfo.ulFlags   = BIF_NEWDIALOGSTYLE | BIF_DONTGOBELOWDOMAIN | BIF_BROWSEFORCOMPUTER | BIF_RETURNONLYFSDIRS | BIF_RETURNFSANCESTORS;  
		broInfo.lpfn      = NULL;  
		broInfo.lParam    = NULL;  
		broInfo.iImage    = IDR_MAINFRAME;  

		LPITEMIDLIST pIDList = SHBrowseForFolder(&broInfo);  
		if (pIDList != NULL)  
		{  
			memset(szDisName, 0, sizeof(szDisName));  
			SHGetPathFromIDList(pIDList, szDisName);  
			strFolderPath = szDisName;  
		} 
		m_stitchPath = szDisName;
		UpdateData(FALSE);

		//-------�����������ļ�
		::WritePrivateProfileString(_T("PathInfo"), _T("ImageStitchPath"), m_stitchPath, sPath);
	}
	//����������ļ�
	else if (nDetect == 1)
	{
		try
		{
			ImageOpenFlag = true;
			//BOOL isOpen = TRUE;     //�Ƿ��(����Ϊ����)  
			CString defaultDir = m_stitchPath;   //Ĭ�ϴ򿪵��ļ���չ�� 
			CString fileName = L"";         //Ĭ�ϴ򿪵��ļ���  
			CString filter = _T("�ļ� (*.jpg; *.png; *.bmp; *.tif; *.jpeg)|*.jpg;*.png;*.bmp;*.tif;*.jpeg||");   //�ļ����ǵ�����  
			CFileDialog openFileDlg(TRUE, _T("D:"), fileName, OFN_HIDEREADONLY|OFN_READONLY, filter, NULL);  
			//openFileDlg.GetOFN().lpstrInitialDir = _T("E:\\FileTest");  
			INT_PTR result = openFileDlg.DoModal();  
			CString filePath ;  
			if(result != IDOK) 
			{  
				return;

			}  
			filePath = openFileDlg.GetPathName();  
			char * mPath = transToMultibyte(filePath);
			CWnd::SetDlgItemTextW(IDC_StitchPath, filePath); 

			//��ʼ��һ�±���
			num = 0;

			DetectImage = cvLoadImage(mPath);//ԭʼͼƬ
			savedImage = cvCreateImage(cvGetSize(DetectImage), DetectImage->depth, 3);

			IplImage* ipl = cvLoadImage(mPath);
			if (ipl == NULL)
			{
				return;
			}

			ShowImage(ipl);

			cvReleaseImage(&ipl);
			ipl = NULL;

			RecogFlag = false;
			GetDlgItem(IDC_SNAP)->EnableWindow(TRUE);
			GetDlgItem(IDC_SNAP)->SetWindowText(_T("����ͼƬ"));
			m_selImage.ResetContent();
			m_selImage.AddString(_T("ԭͼ"));
			m_selImage.SetCurSel(0);
			RecogFlag = false;

			GetDlgItem(IDC_RECOGNITION)->EnableWindow(TRUE);
			GetDlgItem(IDC_COUNTDEFAULT)->EnableWindow(TRUE);
			m_tableWidget.DeleteAllItems();
		}
		catch (CMemoryException* e)
		{
			
		}
		catch (CFileException* e)
		{
		}
		catch (CException* e)
		{
		}
	
	}
}


void CForTestDlg::OnBnClickedDetect()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (nDetect == 0)
	{
		nDetect = 1;
		SetDlgItemText(IDC_DETECT, _T("ƴ��"));
		GetDlgItem(IDC_SCANROWS)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCANCOLS)->EnableWindow(TRUE);	
	}
	else if (nDetect == 1)
	{
		nDetect = 0;
		SetDlgItemText(IDC_DETECT, _T("���"));
		GetDlgItem(IDC_SCANROWS)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCANCOLS)->EnableWindow(FALSE);	
		UpdateData(FALSE);
	}
}


void CForTestDlg::OnBnClickedGoto2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	CString targetX, targetY;
	targetX.Format(_T("%0.3f"), m_targetX);//ת��Ϊstring���ͣ�����3λС��
	targetY.Format(_T("%0.3f"), m_targetY);//ת��Ϊstring���ͣ�����3λС��	
	::WritePrivateProfileString(_T("TargetData"), _T("X"), targetX, sPath);
	::WritePrivateProfileString(_T("TargetData"), _T("Y"), targetY, sPath);
}

//�Զ�ɨ���������߳�
void CForTestDlg::AutoScan()
{
	CString sfilename; 
	char strXMove[40] = { 0 };//�ƶ�Ŀ����ַ�����ʽ
	char strYMove[40] = { 0 };
	double curX = 0;
	double curY = 0; 
	double curYNext = 0;
	double curXNext = 0;
	int rowmax = m_scanRows;
	int colmax = m_scanCols;

	SetTimer(2, 100, NULL);
	for (int i = 0;i < rowmax; i ++)
	{
		nCur_row = i;
		sfilename.Format(_T("%s\\P%d_0.bmp"), sImagePath, nCur_row);
		filename = transToMultibyte(sfilename);
		Sleep(200);
		nSnap = 1;
		Sleep(100);
		for (int j = 1;j < colmax; j ++)
		{
			nCur_col = j;
			//SetTimer(2, 500, NULL);

			curX = (double)m_pGalil->commandValue("TPA");
			if ((i % 2) == 0)
			{
				curXNext = curX / 2000.0 + m_Row;
			}
			else
			{
				curXNext = curX / 2000.0 - m_Row;					
			}
			Sleep(700);
			sprintf(strXMove, "pPOSA=%f", curXNext);
			m_pGalil->command(strXMove);
			m_pGalil->command("XQ#MOVEA");
			//�ȴ��˶�ֹͣ
			while(TRUE)
			{
				Sleep(200);
				if (((int)m_pGalil->commandValue("_BGA=?")) == 0)
				{
					break;
				}
			}
			sfilename.Format(_T("%s\\P%d_%d.bmp"), sImagePath, nCur_row, nCur_col);
			filename = transToMultibyte(sfilename);
			Sleep(200);
			nSnap = 1;
			Sleep(100);
		}
		//if (i != (nCur_row - 1))
		while (((int)m_pGalil->commandValue("_BGA=?")) == 0 && (int)m_pGalil->commandValue("_BGB=?") == 0)//�ȴ�ȫ���˶�ֹͣ�ڿ�ʼ�˶�
		{
			Sleep(700);
			curY = (double)m_pGalil->commandValue("TPB");
			curYNext = curY - m_Col * 2000;
			sprintf(strYMove,  "pPOSB=%f", curYNext/2000.0);
			m_pGalil->command(strYMove);
			m_pGalil->command("XQ#MOVEB");

			//�ȴ��˶�ֹͣ
			while(TRUE)
			{
				Sleep(200);
				if((int)m_pGalil->commandValue("_BGB=?") == 0);
				{
					break;
				}
			}
			break;
		}
	}
	//nAutoScan = 0;
	MessageBox(_T("ɨ���Ѿ���ɣ�"));
}

//DWORD WINAPI AutoScanThread(LPVOID lpvoid)
UINT AutoScanThread(LPVOID lpvoid)
{
	CForTestDlg* m_form = (CForTestDlg*)lpvoid;
	m_form->AutoScan();
	return 0;
}



void CForTestDlg::OnBnClickedBegin()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (nScan == 0)
	{
		nScan = 1;
		SetDlgItemText(IDC_BEGIN, _T("��ʼɨ��"));
		GetDlgItem(IDC_SCANCOLS)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCANROWS)->EnableWindow(TRUE);	
	}
	else if (nScan == 1)
	{
		nScan = 0;
		nAutoScan = 1;
		CString sfilename; 
		SetDlgItemText(IDC_BEGIN, _T("����ɨ��"));
		GetDlgItem(IDC_SCANROWS)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCANCOLS)->EnableWindow(FALSE);	
		UpdateData(TRUE);

		CString scanRow, scanCol, confirms;
		scanRow.Format(_T("%d"), m_scanRows);
		::WritePrivateProfileString(_T("ScanInfo"), _T("ScanRow"), scanRow, sPath);
		scanCol.Format(_T("%d"), m_scanCols);
		::WritePrivateProfileString(_T("ScanInfo"), _T("ScanCol"), scanCol, sPath);
		confirms.Format(_T("��ǰ����%d���ﾵ�£�ע���ʮ���ߺͱ��ȥ����ȷ��Ҫɨ��ͼƬ��"), lens);
		if ((MessageBox(confirms, _T("ȷ��"), MB_OKCANCEL)) == IDCANCEL)
		{
			return;
		}

		m_ScanThread = AfxBeginThread((AFX_THREADPROC)AutoScanThread, this);
		//int rowmax = m_scanRows;
		//int colmax = m_scanCols;
		//char strXMove[40] = { 0 };//�ƶ�Ŀ����ַ�����ʽ
		//char strYMove[40] = { 0 };
		//double curX = 0;
		//double curY = 0; 
		//double curYNext = 0;
		//double curXNext = 0;

		////IplImage* tempImage[] = {NULL};//ɨ��ͼƬ
		////IplImage* tempImage = NULL;
		////IplImage* resultImage = cvCreateImage(cvSize(nWidth * 3, nHeight * 3), 8, 3);//ƴ�ӽ��
		////cvZero(resultImage);
		////malloc(TDIBWIDTHBYTES(nWidth * colmax * 24) * nHeight * rowmax);
		////memset(resultImage, 0,TDIBWIDTHBYTES(nWidth * colmax * 24) * nHeight * rowmax);
		////CvRect rect;
		//
		//for (int i = 0;i < rowmax; i ++)
		//{
		//	nCur_row = i;
		//	sfilename.Format(_T("%s\\P%d_0.bmp"), sImagePath, nCur_row);
		//	filename = transToMultibyte(sfilename);
		//	Sleep(200);
		//	nSnap = 1;
		//	Sleep(100);
		//	for (int j = 1;j < colmax; j ++)
		//	{
		//		nCur_col = j;

		//		curX = (double)m_pGalil->commandValue("TPA");
		//		if ((i % 2) == 0)
		//		{
		//			curXNext = curX / 2000.0 + m_Row;
		//		}
		//		else
		//		{
		//			curXNext = curX / 2000.0 - m_Row;					
		//		}
		//		Sleep(700);
		//		sprintf(strXMove, "pPOSA=%f", curXNext);
		//		m_pGalil->command(strXMove);
		//		m_pGalil->command("XQ#MOVEA");
		//		//�ȴ��˶�ֹͣ
		//		while(TRUE)
		//		{
		//			Sleep(200);
		//			if (((int)m_pGalil->commandValue("_BGA=?")) == 0)
		//			{
		//				break;
		//			}
		//		}
		//		sfilename.Format(_T("%s\\P%d_%d.bmp"), sImagePath, nCur_row, nCur_col);
		//		filename = transToMultibyte(sfilename);
		//		Sleep(200);
		//		nSnap = 1;
		//		Sleep(100);
		//	}
		//	//if (i != (nCur_row - 1))
		//	while (((int)m_pGalil->commandValue("_BGA=?")) == 0 && (int)m_pGalil->commandValue("_BGB=?") == 0)//�ȴ�ȫ���˶�ֹͣ�ڿ�ʼ�˶�
		//	{
		//		Sleep(700);
		//		curY = (double)m_pGalil->commandValue("TPB");
		//		curYNext = curY - m_Col * 2000;
		//		sprintf(strYMove,  "pPOSB=%f", curYNext/2000.0);
		//		m_pGalil->command(strYMove);
		//		m_pGalil->command("XQ#MOVEB");

		//		//�ȴ��˶�ֹͣ
		//		while(TRUE)
		//		{
		//			Sleep(200);
		//			if((int)m_pGalil->commandValue("_BGB=?") == 0);
		//			{
		//				break;
		//			}
		//		}
		//		break;
		//	}
		//	//MessageBox(_T("has stopped"));
		//}
		//MessageBox(_T("ɨ���Ѿ���ɣ�"));
		////CString stitchName;
		////stitchName.Format(_T("%s\\ƴ��ͼ.bmp"), m_stitchPath);
		////char* stitchFile = transToMultibyte(stitchName);
		////cvSaveImage(stitchFile, resultImage);
		////cvReleaseImage(&resultImage);
	}
}

void CForTestDlg::OnBnClickedMatch()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString sfilename; 
	CvRect rect;

	//IplImage* tempImage = NULL;
	IplImage* resultImage = cvCreateImage(cvSize((nWidth - 2) * m_scanCols, (nHeight - 2) * m_scanRows), 8, 3);//ƴ�ӽ��
	cvZero(resultImage);
	if (m_scanCols < 1 || m_scanRows < 1)
	{
		MessageBox(_T("ƴ�����ٵ�����ͼƬ��"));
		return;
	}
	for (int i = 0; i < m_scanRows; i ++)
	{
		for (int j = 0;j < m_scanCols; j ++)
		{
			sfilename.Format(_T("D:\\test\\P%d_%d.bmp"), i, j);
			filename = transToMultibyte(sfilename);

			IplImage* tempImage = cvLoadImage(filename);
			if ((i % 2) == 0)
			{
				rect = cvRect(nWidth * j - 2 * j, nHeight * i - 2 * i, nWidth - 2,  nHeight - 2);
			}
			else
			{
				rect = cvRect((nWidth - 2) * (m_scanCols -j - 1), nHeight * i - 2 * i, nWidth - 2, nHeight - 2);	
			}
			cvSetImageROI(tempImage, cvRect(1, 1, nWidth - 2, nHeight - 2));
			cvSetImageROI(resultImage, rect);//���úϲ�ͼ�����Ȥ��
			cvCopy(tempImage, resultImage);
			cvResetImageROI(resultImage);//�ͷ�ROI������ֻ��ʾROI����
			cvResetImageROI(tempImage);
			cvReleaseImage(&tempImage);
			tempImage = NULL;
		}
	}
	CString stitchName;
	//stitchName.Format(_T("%s\\ƴ��ͼ.bmp"), m_stitchPath);
	stitchName.Format(_T("D:\\test\\ƴ��ͼ.bmp"));
	char* stitchFile = transToMultibyte(stitchName);
	cvSaveImage(stitchFile, resultImage);
	cvReleaseImage(&resultImage);
	//cvReleaseImage(&tempImage);
	MessageBox(_T("ƴ����ɣ�"));
}


void CForTestDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CString scan_info;
	int counts = 0;
	switch (nIDEvent)
	{
	case 1://IDC_URL��Ϊ���ɼ�
		GetDlgItem(IDC_URL)->ShowWindow(SW_HIDE);
		KillTimer(1);
		break;
	case 2:
		UpdateData(TRUE);
		if (nCur_row < m_scanRows)
		{
			scan_info.Format(_T("��ǰɨ���%d�У���%d��"), nCur_row, nCur_col);
			SetDlgItemText(IDC_TEXT, scan_info);
		}
		counts = m_scanCols * m_scanRows;
		if (counts == ((nCur_col + 1) * (nCur_row + 1)))
		{
			GetDlgItem(IDC_BEGIN)->EnableWindow(TRUE);
			KillTimer(2);
			SetDlgItemText(IDC_TEXT, _T("�ز�Ƭɨ����ɣ�"));
		}
		break;
	default: break;
	}

	CDialogEx::OnTimer(nIDEvent);
}
 
void CForTestDlg::OnBnClickedRecognition()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_tableWidget.DeleteAllItems();//���������Ŀ
	UpdateData(TRUE);
	int histSize = 256;// ����HIST�ĸ�����Խ��Խ��ȷ
	float RGB_rangs[] = {0, 255};
	float *rangs[] = {RGB_rangs};
	//CvHistogram* hist = cvCreateHist(1,&histSize,CV_HIST_ARRAY,rangs,1);
	float max_value = 0;
	float min_value = 0;

	try
	{
	if ((m_minRadius >= 0) && (m_maxRadius > 0))
	{
		storage = cvCreateMemStorage(0);
		//DetectImage = (&(IplImage) imageMatch);
		dstImage = cvCreateImage(cvGetSize(DetectImage), 8, 1);
		image_edge = cvCreateImage(cvGetSize(DetectImage), 8, 1);
		IplImage* edge_clone = cvCreateImage(cvGetSize(DetectImage), 8, 1);
		circleImage = cvCreateImage(cvGetSize(DetectImage), DetectImage->depth, 3);
		GrayImage = cvCreateImage(cvGetSize(DetectImage), DetectImage->depth, 3);
		//results = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq), sizeof(CvPoint), storage);
		if (DetectImage->nChannels == 3)
		{
			cvCvtColor(DetectImage, image_edge, CV_RGB2GRAY);
		}
		else if (DetectImage->nChannels == 1)
		{
			cvCopy(DetectImage, image_edge);
		}
		else return;


		//����ǰ��ֱ��ͼ����������Դ��Ӱ��
		edge_clone = cvCloneImage(image_edge);
		cvEqualizeHist(edge_clone, edge_clone);//���⻯
		cvSmooth(edge_clone, edge_clone, CV_GAUSSIAN, 3, 3);//��˹�˲�
		cv::Mat edge = Mat(edge_clone);
		cv::Mat gI;
		edgeEnhance(edge, gI);//��Ե��ǿ
		*edge_clone = IplImage(gI);


		////ֻ���м䲿�ֵģ�10���ر�ԵȫΪ0
		//IplImage* pTemp = cvCloneImage(image_edge);
		//uchar* ptr = (uchar*)pTemp->imageData;
		//for (int i = 0; i < pTemp->height; i++)
		//{
		//	for (int j = 0; j < pTemp->width; j++)
		//	{
		//		if (i < 5 || i > (pTemp->height - 5) || j < 5 || j > (pTemp->width - 5))
		//		{
		//			*ptr = 0;
		//		}
		//		ptr++;
		//	}
		//	ptr += pTemp->widthStep -pTemp->width;
		//}
		//cvCopy(pTemp, image_edge);
		//cvReleaseImage(&pTemp);

		//��ֵ��ǰ�ı�Ե��չ
		int border = 20;
		Mat gray_buf(gI.rows + border * 2, gI.cols + border * 2, gI.depth());
		copyMakeBorder(gI, gray_buf, border, border, border, border, BORDER_REFLECT);

		IplImage* gray_canvas = cvCreateImage(cvSize(gray_buf.rows, gray_buf.cols), image_edge->depth, 1);
		*gray_canvas = IplImage(gray_buf);
		cvAdaptiveThreshold(gray_canvas, gray_canvas, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 41, 0);
		cvSetImageROI(gray_canvas, cvRect(border + 1, border + 1, dstImage->width, dstImage->height));
		cvCopy(gray_canvas, dstImage);
		cvResetImageROI(gray_canvas);

		gray_canvas = NULL;
		gray_buf = NULL;
		gI = NULL;
		cvReleaseImage(&gray_canvas);	


		//�ֲ���ֵ��
		//cvAdaptiveThreshold(image_edge, dstImage, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, nThreashold, 0);

		//double low = 0, high = 0;
		//AdaptiveFindThreshold(DetectImage, &low, &high, 3);
		//cvCanny(image_edge, image_edge, low, high);
		//IplConvKernel* kernel = cvCreateStructuringElementEx(3, 3, 1, 1, CV_SHAPE_ELLIPSE);
		//cvDilate(image_edge, image_edge, kernel, 3);
		//cvThreshold(image_edge, image_edge, 10, 255, CV_THRESH_BINARY_INV);

		//kernel = NULL; �����ᵼ���ڴ��ģ���ͷţ��Ӷ�����
		//delete kernel;
		//cvSub(dstImage, image_edge, dstImage);

		//������ȥ���ӵ�
		IplConvKernel* kernel_5_5 = cvCreateStructuringElementEx(5, 5, 2, 2, CV_SHAPE_ELLIPSE, 0);
		IplConvKernel* kernel_7_7 = cvCreateStructuringElementEx(7, 7, 3, 3, CV_SHAPE_ELLIPSE, 0);
		//cvErode(dstImage, dstImage, kernel_5_5, 1);
		//cvDilate(dstImage, dstImage, kernel_5_5, 1);
		

		//OnBnClickedButton4();//3��ϸ��
		//OnBnClickedButton3();//ȥ��ź��˿��
		//OnBnClickeddilate2();//ȥ��С���ӵ�

		//OnBnClickedclose();
		//OnBnClickeddilate();//
		//OnBnClickedclose();

/*

		char* imgPath = "F:/CircleCount/images/L-A2.tif";
		DetectImage = cvLoadImage(imgPath);
		cvThreshold(image_edge, dstImage, 200, 255, CV_THRESH_BINARY);
*/

		//ֻ���м䲿�ֵģ�7���ر�ԵȫΪ0
		IplImage* pTemp = cvCloneImage(dstImage);
		uchar* ptr = (uchar*)pTemp->imageData;
		for (int i = 0; i < pTemp->height; i++)
		{
			for (int j = 0; j < pTemp->width; j++)
			{
				if (i < 2 || i > (pTemp->height - 2) || j < 2 || j > (pTemp->width - 2))
				{
					*ptr = 0;
				}
				ptr++;
			}
			ptr += pTemp->widthStep -pTemp->width;
		}
		cvCopy(pTemp, dstImage);
		cvReleaseImage(&pTemp);
		distImage = cvCreateImage(cvGetSize(dstImage), dstImage->depth, dstImage->nChannels);
		cvCopy(dstImage, distImage);
/*

		cvCvtColor(DetectImage, image_edge, CV_RGB2GRAY);
		cvThreshold(dstImage, dstImage, 0.1, 255, CV_THRESH_BINARY);
*/

		ShowImage(dstImage);
		edge_clone = NULL;
		cvReleaseImage(&edge_clone);
	}
	}
	catch (Exception e)
	{
		//MessageBox(_T("������"));
	}
}
//�ж��Ƿ���Բ��
bool CForTestDlg::IsInTheCircle(CvPoint p, CvPoint center, int radius)
{
	if (((p.x - center.x)*(p.x - center.x) + (p.y - center.y) * (p.y - center.y)) <= radius * radius)
	{
		return true;
	} 
	else
	{
		return false;
	}
}
//�ж��Ƿ���Բ��
bool CForTestDlg::IsOnTheCircle(CvPoint p, CvPoint center, int radius)
{
	double distance = sqrt(double((p.x - center.x)*(p.x - center.x)) + double((p.y - center.y) * (p.y - center.y)));
	if (abs(radius - distance) <= 1)
	{
		return true;
	} 
	else
	{
		return false;
	}
}


void CForTestDlg::OnBnClickedReload()
{
	//��ʶ����Ľ���ͳ��
	vector<float> detectMean;
	int lightNum = 0;
	int darkNum = 0;

	double meanGray = 0.0, maxGray = 0.0, minGray = 0.0, SSGray = 0.0, curGray = 0.0;//ƽ��ֵ�����ֵ����Сֵ����׼���ǰֵ
	double meanRadius = 0.0, maxRadius = 0.0, minRadius = 0.0, SSRadius = 0.0, curRadius = 0.0;
	//int maxRadius = 0, minRadius = 0, curRadius = 0;
	CString strGray, strRadius;
	for (int i = 0; i < m_tableWidget.GetItemCount(); i++)
	{
		strGray = m_tableWidget.GetItemText(i, 2);
		curGray = _ttof(LPCTSTR(strGray));
		detectMean.push_back(curGray);
		strRadius = m_tableWidget.GetItemText(i, 3);
		curRadius = _ttof(LPCTSTR(strRadius));
		if (i == 0)
		{
			maxGray = curGray;
			minGray = curGray;
			maxRadius = curRadius;
			minRadius = curRadius;
		}

		meanGray += curGray;
		if ((maxGray - curGray) < 0.0001)
		{
			maxGray = curGray;
		}
		if ((minGray - curGray) > 0.0001)
		{
			minGray = curGray;
		}

		meanRadius += curRadius;
		if ((maxRadius - curRadius) < 0.0001)
		{
			maxRadius = curRadius;
		}
		if ((minRadius - curRadius) >0.0001)
		{
			minRadius = curRadius;
		}
	}
	meanGray /= m_tableWidget.GetItemCount();
	meanRadius /= m_tableWidget.GetItemCount();
	
	//kmeans����
	Mat points = Mat(detectMean.size(), 1, CV_32F, &detectMean[0]);
	Mat labels, center;
	int clusterIndex = 0;
	kmeans(points, 2, labels,
		TermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 20, 1.0),
		4, KMEANS_PP_CENTERS, center);
	for (int i = 0; i < detectMean.size(); i++)
	{
		clusterIndex = labels.at<int>(i);
		if (center.at<float>(0) > center.at<float>(1))
		{
			if (clusterIndex == 0)
			{
				lightNum++;
			}
			else
			{
				darkNum++;
			}
		}
		else
		{
			if (clusterIndex == 1)
			{
				lightNum++;
			}
			else
			{
				darkNum++;
			}
		}
	}

	//��׼��
	for (int j = 0; j < m_tableWidget.GetItemCount(); j++)
	{
		strGray = m_tableWidget.GetItemText(j, 2);
		curGray = _ttof(LPCTSTR(strGray));
		strRadius = m_tableWidget.GetItemText(j, 3);
		curRadius = _ttof(LPCTSTR(strRadius));

		SSGray += pow((curGray - meanGray), 2);
		SSRadius += pow(curRadius - meanRadius, 2);
	}
	SSGray = sqrt(SSGray / m_tableWidget.GetItemCount());
	SSRadius = sqrt(SSRadius / m_tableWidget.GetItemCount());

	CString a, b, c, d;
	b = _T("");


	a = _T("ƽ��ֵ:");
	c.Format(_T("%0.2f"), meanGray);
	d.Format(_T("%0.2f"), meanRadius);
	m_tableWidget.AddItem(a, b, c, d);

	a = _T("���ֵ:");
	c.Format(_T("%0.2f"), maxGray);
	d.Format(_T("%d"), (int)maxRadius);
	m_tableWidget.AddItem(a, b, c, d);

	a = _T("��Сֵ:");
	c.Format(_T("%0.2f"), minGray);
	d.Format(_T("%d"), (int)minRadius);
	m_tableWidget.AddItem(a, b, c, d);

	a = _T("��׼��:");
	c.Format(_T("%0.3f"), SSGray);
	d.Format(_T("%0.3f"), SSRadius);
	m_tableWidget.AddItem(a, b, c, d);

	a = _T("������:");
	c.Format(_T("%d"), lightNum);
	m_tableWidget.AddItem(a, b, c, b);

	a = _T("������:");
	c.Format(_T("%d"), darkNum);
	m_tableWidget.AddItem(a, b, c, b);
}

//��ȡԲ�Ĳ���ͼ�л���
void CForTestDlg::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;

	CString strAxisName;
	CString strOvalAxis;
	IplImage* circleImageTemp = cvCreateImage(cvGetSize(DetectImage), DetectImage->depth, 3);
	cvCopy(DetectImage, circleImageTemp);
	//cvCvtColor(dstImage, circleImageTemp, CV_GRAY2RGBA);
	NMLISTVIEW* pNMListView = (NMLISTVIEW*)pNMHDR;
	if (pNMListView->iItem != -1)
	{
		//��ȡ�б����Ϣ����ͼ����ʾ
		strAxisName = m_tableWidget.GetItemText(pNMListView->iItem, 1);
		strOvalAxis = m_tableWidget.GetItemText(pNMListView->iItem, 3);
		//��ȡԲ��x����
		CString leftStr = strAxisName.Left(strAxisName.ReverseFind(','));
		//int leftInt = atoi(transToMultibyte(leftStr));
		int leftInt = _ttoi(LPCTSTR(leftStr));
		//��ȡԲ��y����
		CString rightStr = strAxisName.Right(strAxisName.GetLength() - strAxisName.ReverseFind(',') - 1);
		//int rightInt = atoi(transToMultibyte(rightStr));
		int rightInt = _ttoi(LPCTSTR(rightStr));
		int radius = _ttoi(LPCTSTR(strOvalAxis));

		CvPoint centerPoint;
		centerPoint.x = leftInt;
		centerPoint.y = rightInt;
		//SetDlgItemInt(IDC_TARGETX, longAxisInt);
		//SetDlgItemInt(IDC_TARGETY, shortAxisInt);
		//Բ�Ļ���Ϊʮ����
		cvLine(circleImageTemp, cvPoint(leftInt-3, rightInt), cvPoint(leftInt+3, rightInt), CV_RGB(255,255, 255));
		cvLine(circleImageTemp, cvPoint(leftInt, rightInt-3), cvPoint(leftInt, rightInt+3), CV_RGB(255,255, 255));
		//cvCircle(circleImage, centerPoint, 2, CV_RGB(255, 255, 255), 2, 8, 0);
		//�����������ᡢ�����Ϊ�뾶��Բ
		//cvCircle(circleImage, centerPoint, shortAxisInt, CV_RGB(255, 0, 0), 4, 8, 0);
		//cvCircle(circleImage, centerPoint, longAxisInt, CV_RGB(254, 0, 0), 4, 8, 0);
		cvCircle(circleImageTemp, centerPoint, radius, CV_RGB(254, 0, 0), 4, 8, 0);
		ShowImage(circleImageTemp);
		cvReleaseImage(&circleImageTemp);
	}
}

void CForTestDlg::SetDisplayWindow()
{
	int xoffset = 768;

	int nWinWidth = 1420;//������ߴ�
	int nWinHeight= 850;
	//���������ڵĳ߶ȣ���Χ
	this->SetWindowPos(NULL, 0, 0, nWinWidth, nWinHeight, SWP_NOZORDER | SWP_SHOWWINDOW);
	GetDlgItem(IDC_VIDEO)->SetWindowPos(NULL, 13,65, 944,708,SWP_NOZORDER | SWP_SHOWWINDOW);//�����������

	GetDlgItem(IDC_OPEN_CAM)->SetWindowPos(NULL, 13, 26, 100, 33, SWP_NOZORDER | SWP_SHOWWINDOW);//�����
	GetDlgItem(IDC_SNAP)->SetWindowPos(NULL, 121, 26, 100, 33, SWP_NOZORDER | SWP_SHOWWINDOW);//����
	GetDlgItem(IDC_EXIT)->SetWindowPos(NULL, 227, 26, 100, 33, SWP_NOZORDER | SWP_SHOWWINDOW);//���
	GetDlgItem(IDC_Orthogonal)->SetWindowPos(NULL, 333, 59-33, 100, 33, SWP_NOZORDER | SWP_SHOWWINDOW);//�������
	GetDlgItem(IDC_STATIC5)->SetWindowPos(NULL, 437, 66-31, 56, 19, SWP_NOZORDER | SWP_SHOWWINDOW);//�ֱ���
	GetDlgItem(IDC_RESOLUTION)->SetWindowPos(NULL, 497, 59-31, 96, 27, SWP_NOZORDER | SWP_SHOWWINDOW);//�ֱ���ѡ��
	GetDlgItem(IDC_STATIC6)->SetWindowPos(NULL, 608-13, 66-31, 70, 19, SWP_NOZORDER | SWP_SHOWWINDOW);//���
	GetDlgItem(IDC_SEL_CAM)->SetWindowPos(NULL, 682-13, 59-31, 146, 27, SWP_NOZORDER | SWP_SHOWWINDOW);//���ѡ��
	GetDlgItem(IDC_STATIC7)->SetWindowPos(NULL, 834-13, 66-31, 44, 19, SWP_NOZORDER | SWP_SHOWWINDOW);//�ﾵ
	GetDlgItem(IDC_OBJECTLENS)->SetWindowPos(NULL, 882-13, 59-31, 56, 27, SWP_NOZORDER | SWP_SHOWWINDOW);//�ﾵѡ��
	GetDlgItem(IDC_DETECT       )->SetWindowPos(NULL, 974-15, 59-34, 60, 33, SWP_NOZORDER | SWP_SHOWWINDOW);//���
	GetDlgItem(IDC_STITCH   )->SetWindowPos(NULL, 1012-13, 63-34, 114, 19, SWP_NOZORDER | SWP_SHOWWINDOW);//ƴ��·��
	GetDlgItem(IDC_StitchPath   )->SetWindowPos(NULL, 1136-13, 59-34, 214, 33, SWP_NOZORDER | SWP_SHOWWINDOW);//ƴ��·��
	GetDlgItem(IDC_StitchPathSet)->SetWindowPos(NULL, 1352-13, 59-37, 62, 38, SWP_NOZORDER | SWP_SHOWWINDOW);//·��Ԥ��
	GetDlgItem(IDC_STATIC9     )->SetWindowPos(NULL, 1028-13, 116-34, 98, 19, SWP_NOZORDER | SWP_SHOWWINDOW);//����·��
	GetDlgItem(IDC_ScanPath     )->SetWindowPos(NULL, 1136-13, 111-34, 214, 33, SWP_NOZORDER | SWP_SHOWWINDOW);//����·��
	GetDlgItem(IDC_ScanImgPath  )->SetWindowPos(NULL, 1352-13, 109-34, 62, 38, SWP_NOZORDER | SWP_SHOWWINDOW);//����Ԥ��

	GetDlgItem(IDC_STATIC10 )->SetWindowPos(NULL, 994-10, 170-33, 132, 19, SWP_NOZORDER | SWP_SHOWWINDOW);//�ٶ�
	GetDlgItem(IDC_STATIC11 )->SetWindowPos(NULL, 1140-13, 170-33, 16, 19, SWP_NOZORDER | SWP_SHOWWINDOW);//�ٶ�x
	GetDlgItem(IDC_STATIC12 )->SetWindowPos(NULL, 1244-10, 170-33, 16, 19, SWP_NOZORDER | SWP_SHOWWINDOW);//�ٶ�y
	GetDlgItem(IDC_STATIC13 )->SetWindowPos(NULL, 1008-10, 213-33, 116, 19, SWP_NOZORDER | SWP_SHOWWINDOW);//����
	GetDlgItem(IDC_SPEEDX )->SetWindowPos(NULL, 1158-10, 166-33, 80, 33, SWP_NOZORDER | SWP_SHOWWINDOW);//�ٶ�x
	GetDlgItem(IDC_SPEEDY )->SetWindowPos(NULL, 1264-10, 166-33, 80, 33, SWP_NOZORDER | SWP_SHOWWINDOW);//�ٶ�y
	GetDlgItem(IDC_CONFIRM)->SetWindowPos(NULL, 1350-13, 164-32, 64, 33, SWP_NOZORDER | SWP_SHOWWINDOW);//�趨
	GetDlgItem(IDC_STEPX  )->SetWindowPos(NULL, 1158-10, 209-33, 80, 33, SWP_NOZORDER | SWP_SHOWWINDOW);//����x
	GetDlgItem(IDC_STEPY  )->SetWindowPos(NULL, 1264-10, 209-33, 80, 33, SWP_NOZORDER | SWP_SHOWWINDOW);//����y
	GetDlgItem(IDC_STATIC14 )->SetWindowPos(NULL, 1140-13, 213-33, 16, 19, SWP_NOZORDER | SWP_SHOWWINDOW);//����x
	GetDlgItem(IDC_STATIC15 )->SetWindowPos(NULL, 1246-13, 215-33, 16, 19, SWP_NOZORDER | SWP_SHOWWINDOW);//����y
	GetDlgItem(IDC_STATIC16 )->SetWindowPos(NULL, 992-13, 256-23, 70, 19, SWP_NOZORDER | SWP_SHOWWINDOW);//Ŀ��
	GetDlgItem(IDC_STEPSET)->SetWindowPos(NULL, 1350-13, 209-33, 64, 33, SWP_NOZORDER | SWP_SHOWWINDOW);//�趨
	GetDlgItem(IDC_TARGETX)->SetWindowPos(NULL, 1094-13, 254-30, 80, 33, SWP_NOZORDER | SWP_SHOWWINDOW);//Ŀ��x
	GetDlgItem(IDC_TARGETY)->SetWindowPos(NULL, 1200-13, 254-30, 80, 33, SWP_NOZORDER | SWP_SHOWWINDOW);//Ŀ��y
	GetDlgItem(IDC_GOTO   )->SetWindowPos(NULL, 1286-13, 256-29, 52, 29, SWP_NOZORDER | SWP_SHOWWINDOW);//ֱ��
	GetDlgItem(IDC_GOTO2  )->SetWindowPos(NULL, 1348-13, 256-29, 52, 29, SWP_NOZORDER | SWP_SHOWWINDOW);//����
	GetDlgItem(IDC_STATIC17 )->SetWindowPos(NULL, 1074-13, 256-23, 16, 19, SWP_NOZORDER | SWP_SHOWWINDOW);//Ŀ��x
	GetDlgItem(IDC_STATIC18 )->SetWindowPos(NULL, 1180-13, 256-23, 16, 19, SWP_NOZORDER | SWP_SHOWWINDOW);//Ŀ��y
	GetDlgItem(IDC_STATIC19 )->SetWindowPos(NULL, 994-13, 310-33, 176, 195, SWP_NOZORDER | SWP_SHOWWINDOW);//�ֶ�����

	GetDlgItem(IDC_CHECK3)->SetWindowPos(NULL, 1008-13, 349-24, 50, 24, SWP_NOZORDER | SWP_SHOWWINDOW);//����
	GetDlgItem(IDC_UP    )->SetWindowPos(NULL, 1064-13, 339-33, 48, 59, SWP_NOZORDER | SWP_SHOWWINDOW);//��
	GetDlgItem(IDC_LEFT  )->SetWindowPos(NULL, 1014-13, 399-33, 48, 59, SWP_NOZORDER | SWP_SHOWWINDOW);//��
	GetDlgItem(IDC_DOWN  )->SetWindowPos(NULL, 1064-13, 399-33, 48, 59, SWP_NOZORDER | SWP_SHOWWINDOW);//��
	GetDlgItem(IDC_RIGHT )->SetWindowPos(NULL, 1114-13, 399-33, 48, 59, SWP_NOZORDER | SWP_SHOWWINDOW);//��
	GetDlgItem(IDC_STOP  )->SetWindowPos(NULL, 1042-13, 468-33, 84, 33, SWP_NOZORDER | SWP_SHOWWINDOW);//ֹͣ

	GetDlgItem(IDC_STATIC20 )->SetWindowPos(NULL, 1220-13, 363-33, 44, 19, SWP_NOZORDER | SWP_SHOWWINDOW);//baoguang
	GetDlgItem(IDC_STATIC21 )->SetWindowPos(NULL, 1224-13, 408-28, 34, 19, SWP_NOZORDER | SWP_SHOWWINDOW);//����
	GetDlgItem(IDC_STATIC22 )->SetWindowPos(NULL, 1210-13, 458-30, 48, 19, SWP_NOZORDER | SWP_SHOWWINDOW);//�Աȶ�
	GetDlgItem(IDC_STATIC23 )->SetWindowPos(NULL, 1196-13, 507-30, 62, 19, SWP_NOZORDER | SWP_SHOWWINDOW);//�ع�ʱ��
	GetDlgItem(IDC_WHITEBALANCE)->SetWindowPos(NULL, 1210-13, 318-33, 100, 33, SWP_NOZORDER | SWP_SHOWWINDOW);//��ƽ��
	GetDlgItem(IDC_DEFAULT     )->SetWindowPos(NULL, 1318-13, 318-33, 84, 33, SWP_NOZORDER | SWP_SHOWWINDOW);//Ĭ��ֵ
	GetDlgItem(IDC_AutoExp     )->SetWindowPos(NULL, 1282-13, 363-35, 86, 24, SWP_NOZORDER | SWP_SHOWWINDOW);//�Զ��ع�
	GetDlgItem(IDC_STATIC1     )->SetWindowPos(NULL, 1298-10, 359, 112, 19, SWP_NOZORDER | SWP_SHOWWINDOW);//����ֵ
	GetDlgItem(IDC_SLIDER1     )->SetWindowPos(NULL, 1254-10, 378-2, 156, 29, SWP_NOZORDER | SWP_SHOWWINDOW);//������
	GetDlgItem(IDC_STATIC2     )->SetWindowPos(NULL, 1298-10, 409-2, 112, 19, SWP_NOZORDER | SWP_SHOWWINDOW);//�Աȶ�ֵ
	GetDlgItem(IDC_SLIDER2     )->SetWindowPos(NULL, 1254-10, 428-4, 156, 29, SWP_NOZORDER | SWP_SHOWWINDOW);//�Աȶ���
	GetDlgItem(IDC_STATIC3     )->SetWindowPos(NULL, 1298-10, 459-4, 112, 19, SWP_NOZORDER | SWP_SHOWWINDOW);//�ع�ʱ��ֵ
	GetDlgItem(IDC_SLIDER3     )->SetWindowPos(NULL, 1254-10, 478-6, 156, 29, SWP_NOZORDER | SWP_SHOWWINDOW);//�ع�ʱ����

	GetDlgItem(IDC_STATIC24 )->SetWindowPos(NULL, 1006-13, 529-33, 398, 93, SWP_NOZORDER | SWP_SHOWWINDOW);//�Զ�ɨ��
	GetDlgItem(IDC_STATIC25 )->SetWindowPos(NULL, 1018-13, 560-28, 58, 19, SWP_NOZORDER | SWP_SHOWWINDOW);//ɨ������
	GetDlgItem(IDC_STATIC26 )->SetWindowPos(NULL, 1172-11, 560-28, 58, 19, SWP_NOZORDER | SWP_SHOWWINDOW);//ɨ������
	GetDlgItem(IDC_SCANROWS)->SetWindowPos(NULL, 1084-15, 560-31, 88, 29, SWP_NOZORDER | SWP_SHOWWINDOW);//ɨ������
	GetDlgItem(IDC_SCANCOLS)->SetWindowPos(NULL, 1238-13, 560-31, 88, 29, SWP_NOZORDER | SWP_SHOWWINDOW);//ɨ������
	GetDlgItem(IDC_BEGIN   )->SetWindowPos(NULL, 1330-13, 560-29, 70, 26, SWP_NOZORDER | SWP_SHOWWINDOW);//����ɨ��
	GetDlgItem(IDC_MATCH   )->SetWindowPos(NULL, 1330-13, 591-29, 70, 26, SWP_NOZORDER | SWP_SHOWWINDOW);//ƴ��
	GetDlgItem(IDC_NOTE    )->SetWindowPos(NULL, 1010, 591-26, 300, 19, SWP_NOZORDER | SWP_SHOWWINDOW);//��ʾ

	GetDlgItem(IDC_STATIC27 )->SetWindowPos(NULL, 1006-13, 621-33, 398, 204, SWP_NOZORDER | SWP_SHOWWINDOW);//Բ����
	GetDlgItem(IDC_STATIC28 )->SetWindowPos(NULL, 1030-16, 650-28, 42, 19, SWP_NOZORDER | SWP_SHOWWINDOW);//��ֵ
	GetDlgItem(IDC_STATIC29 )->SetWindowPos(NULL, 1028-16, 686-30, 60, 19, SWP_NOZORDER | SWP_SHOWWINDOW);//��С�뾶
	GetDlgItem(IDC_STATIC30 )->SetWindowPos(NULL, 1146-9, 686-30, 60, 19, SWP_NOZORDER | SWP_SHOWWINDOW);//���뾶
	GetDlgItem(IDC_STATIC4     )->SetWindowPos(NULL, 1146-16, 602, 98, 19, SWP_NOZORDER | SWP_SHOWWINDOW);//��ֵֵ
	GetDlgItem(IDC_SLIDER4     )->SetWindowPos(NULL, 1080-16, 619, 200, 24, SWP_NOZORDER | SWP_SHOWWINDOW);//��ֵ��
	GetDlgItem(IDC_RECOGNITION )->SetWindowPos(NULL, 1286-16, 648-29, 58, 26, SWP_NOZORDER | SWP_SHOWWINDOW);//ʶ��
	GetDlgItem(IDC_RELOAD      )->SetWindowPos(NULL, 1346-16, 648-29, 58, 26, SWP_NOZORDER | SWP_SHOWWINDOW);//���⻯
	GetDlgItem(IDC_MINRADIUS   )->SetWindowPos(NULL, 1094-16, 686-32, 54, 29, SWP_NOZORDER | SWP_SHOWWINDOW);//��С�뾶
	GetDlgItem(IDC_MAXRADIUS   )->SetWindowPos(NULL, 1208-6, 686-32, 54, 29, SWP_NOZORDER | SWP_SHOWWINDOW);//���뾶
	GetDlgItem(IDC_COUNTDEFAULT)->SetWindowPos(NULL, 1286-16, 686-26, 58, 26, SWP_NOZORDER | SWP_SHOWWINDOW);//Ĭ��
	GetDlgItem(IDC_EXPORT      )->SetWindowPos(NULL, 1346-16, 686-26, 58, 26, SWP_NOZORDER | SWP_SHOWWINDOW);//����
	GetDlgItem(IDC_LIST1       )->SetWindowPos(NULL, 1012-16, 690, 394, 138, SWP_NOZORDER | SWP_SHOWWINDOW);//�б�

	GetDlgItem(IDC_STATIC31 )->SetWindowPos(NULL, 674-13, 783, 100, 19, SWP_NOZORDER | SWP_SHOWWINDOW);//��ǰ����
	GetDlgItem(IDC_STATIC32 )->SetWindowPos(NULL, 782-13, 783, 16, 19, SWP_NOZORDER | SWP_SHOWWINDOW);//x
	GetDlgItem(IDC_STATIC33 )->SetWindowPos(NULL, 888-13, 783, 16, 19, SWP_NOZORDER | SWP_SHOWWINDOW);//y
	GetDlgItem(IDC_URL )->SetWindowPos(NULL, 470-13, 776, 198, 33, SWP_NOZORDER | SWP_SHOWWINDOW);//URL
	GetDlgItem(IDC_TEXT)->SetWindowPos(NULL, 26-13, 776, 648, 33, SWP_NOZORDER | SWP_SHOWWINDOW);//״̬��
	GetDlgItem(IDC_CURX)->SetWindowPos(NULL, 802-13, 776, 80, 33, SWP_NOZORDER | SWP_SHOWWINDOW);//����x
	GetDlgItem(IDC_CURY)->SetWindowPos(NULL, 908-13, 776, 80, 33, SWP_NOZORDER | SWP_SHOWWINDOW);//����y
}

void CForTestDlg::AdaptiveFindThreshold(const CvArr* image, double *low, double *high, int aperture_size/* =3 */)
{                                                                                
	cv::Mat src = cv::cvarrToMat(image);                                     
	const int cn = src.channels();                                           
	cv::Mat dx(src.rows, src.cols, CV_16SC(cn));                             
	cv::Mat dy(src.rows, src.cols, CV_16SC(cn));                             

	cv::Sobel(src, dx, CV_16S, 1, 0, aperture_size, 1, 0);  
	cv::Sobel(src, dy, CV_16S, 0, 1, aperture_size, 1, 0);  

	CvMat _dx = dx, _dy = dy;
	_AdaptiveFindThreshold(&_dx, &_dy, low, high); 
}
void CForTestDlg::_AdaptiveFindThreshold(CvMat *dx, CvMat *dy, double *low, double *high)
{                                                                                
	CvSize size;                                                             
	IplImage *imge=0;                                                        
	int i,j;                                                                 
	CvHistogram *hist;                                                       
	int hist_size = 255;                                                     
	float range_0[]={0,256};                                                 
	float* ranges[] = { range_0 };                                           
	double PercentOfPixelsNotEdges = 0.7;                                    
	size = cvGetSize(dx);                                                    
	imge = cvCreateImage(size, IPL_DEPTH_32F, 1);                            
	// �����Ե��ǿ��, ������ͼ����                                          
	float maxv = 0;                                                          
	for(i = 0; i < size.height; i++ )                                        
	{                                                                        
		const short* _dx = (short*)(dx->data.ptr + dx->step*i);          
		const short* _dy = (short*)(dy->data.ptr + dy->step*i);          
		float* _image = (float *)(imge->imageData + imge->widthStep*i);  
		for(j = 0; j < size.width; j++)                                  
		{                                                                
			_image[j] = (float)(abs(_dx[j]) + abs(_dy[j]));          
			maxv = maxv < _image[j] ? _image[j]: maxv;  
		}  
		_dy = NULL;
		_dx = NULL;
		delete _dx;
		delete _dy;
		_image = NULL;
		delete _image; 
	}                                                                        
	if(maxv == 0){                                                           
		*high = 0;                                                       
		*low = 0;                                                        
		cvReleaseImage( &imge );  
		return;                                                          
	}                                                                        

	// ����ֱ��ͼ                                                            
	range_0[1] = maxv;                                                       
	hist_size = (int)(hist_size > maxv ? maxv:hist_size);                    
	hist = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);            
	cvCalcHist( &imge, hist, 0, NULL );                                      
	int total = (int)(size.height * size.width * PercentOfPixelsNotEdges);   
	float sum=0;                                                             
	int icount = hist->mat.dim[0].size;                                      

	float *h = (float*)cvPtr1D( hist->bins, 0 );                             
	for(i = 0; i < icount; i++)                                              
	{                                                                        
		sum += h[i];                                                     
		if( sum > total )                                                
			break;                                                   
	}                                                                        
	// ����ߵ�����                                                          
	*high = (i+1) * maxv / hist_size ;                                       
	*low = *high * 0.4;                                                      
	cvReleaseImage( &imge );                                                 
	cvReleaseHist(&hist);  
}

void CForTestDlg::OnBnClickedopen()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString opentext;
	nOpen++;
	nNum++;
	opentext.Format(_T("������ %d(%d)"), nOpen, nNum);
	IplConvKernel* kernel = cvCreateStructuringElementEx(3, 3, 1, 1, CV_SHAPE_RECT, 0);
	cvMorphologyEx(dstImage, dstImage, NULL, kernel, CV_MOP_OPEN, 1);
	GetDlgItem(IDC_open)->SetWindowText(opentext);

	cvCopy(dstImage, distImage);
	ShowImage(dstImage);
	kernel = NULL;
	delete kernel;
}


void CForTestDlg::OnBnClickedclose()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString closeText;
	nClose++;
	nNum++;
	closeText.Format(_T("������ %d(%d)"), nClose, nNum);
	IplConvKernel* kernel = cvCreateStructuringElementEx(5, 5, 2, 2, CV_SHAPE_RECT, 0);
	cvMorphologyEx(dstImage, dstImage, NULL, kernel, CV_MOP_CLOSE, 1);
	GetDlgItem(IDC_close)->SetWindowText(closeText);
	cvCopy(dstImage, distImage);
	ShowImage(dstImage);
	kernel = NULL;
	delete kernel;
}


void CForTestDlg::OnBnClickederode()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString erodeText;
	nErode++;
	nNum++;
	erodeText.Format(_T("��ʴ���� %d(%d)"), nErode, nNum);
	IplConvKernel* kernel = cvCreateStructuringElementEx(3, 3, 1, 1, CV_SHAPE_ELLIPSE, 0);
	cvErode(dstImage, dstImage, kernel, 1);
	//cvMorphologyEx(dstImage, dstImage, NULL, kernel, CV_MOP_OPEN, 2);

	//cvErode(dstImage, dstImage, kernel, 2);
	//cvMorphologyEx(dstImage, dstImage, NULL, kernel, CV_MOP_OPEN, 2);
	//cvErode(dstImage, dstImage, kernel, 1);
	//cvMorphologyEx(dstImage, dstImage, NULL, kernel, CV_MOP_OPEN, 2);
	GetDlgItem(IDC_erode)->SetWindowText(erodeText);
	cvCopy(dstImage, distImage);
	ShowImage(dstImage);
	kernel = NULL;
	delete kernel;
}


void CForTestDlg::OnBnClickeddilate()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString dilateText;
	nDilate++;
	nNum++;
	dilateText.Format(_T("�������� %d(%d)"), nDilate, nNum);
	//IplConvKernel* kernel = cvCreateStructuringElementEx(5, 5, 2, 2, CV_SHAPE_ELLIPSE);
	//cvDilate(dstImage, dstImage, kernel, 1);
	GetDlgItem(IDC_dilate)->SetWindowText(dilateText);

	//�ôֻ����������������:ȡͼ��Ĳ�������ϸ��ͼ������
	IplImage* pTemp = cvCloneImage(dstImage);
	cvThreshold(dstImage, pTemp, 68, 1, CV_THRESH_BINARY_INV);
	Mat tempMat = Mat(pTemp);
	Mat dstMat = cvCreateMat(pTemp->height, pTemp->width, CV_8UC1);
	cvThin(tempMat, dstMat, 1);
	*dstImage = IplImage(dstMat);
	cvThreshold(dstImage, dstImage, 0.5, 255, CV_THRESH_BINARY_INV);


	cvCopy(dstImage, distImage);
	ShowImage(dstImage);
	cvReleaseImage(&pTemp);
	//kernel = NULL;
	//delete kernel;
}


void CForTestDlg::OnBnClickeddilate2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	storage = cvCreateMemStorage(0);
	CvSeq* contours = NULL;
	CvScalar black = CV_RGB(0, 0, 0);
	CvScalar white = CV_RGB(255, 255, 255);
	double area, minArea, maxArea;
	minArea = m_minRadius * m_minRadius ;
	maxArea = m_maxRadius * m_maxRadius ;
	cvFindContours(dstImage, storage, &contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
	while(contours)
	{
		area = cvContourArea(contours, CV_WHOLE_SEQ);
		if (fabs(area) <= minArea || fabs(area) >= maxArea)
		{
			cvDrawContours(dstImage, contours, black, black, -1, CV_FILLED);
		} 
		else
		{
			cvDrawContours(dstImage, contours, white, white, -1, CV_FILLED);

		}
		contours = contours->h_next;
	}
	cvReleaseMemStorage(&storage);
	contours = NULL;
	delete contours;
	WatershedImg = cvCreateImage(cvGetSize(dstImage), 8, 1);
	cvSub(distImage, dstImage, WatershedImg);

	ShowImage(dstImage);
}

bool displayFlag = false;
void CForTestDlg::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!displayFlag)
	{
		ShowImage(displayImage);
		displayFlag = false;
	} 
	else
	{
		ShowImage(circleImage);
		displayFlag = true;
	}
}

//ȥ���������ź��˿��������������������С��5��
void CForTestDlg::OnBnClickedButton3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	unsigned char * ptr1 = (unsigned char *)dstImage->imageData;
	unsigned char * ptr2 = NULL;
	IplImage* tempImg = cvCreateImage(cvGetSize(dstImage), dstImage->depth, dstImage->nChannels);
	cvCopy(dstImage, tempImg);
	unsigned char prePixel = 0;
	int num = 0;
	vector<Point> delPoint;

	//ˮƽ������ɾ��������С��4��
	for (int i = 0; i < dstImage->height; i++)
	{
		for (int j = 0; j < dstImage->width; j++)
		{
			Point pt;
			if (*ptr1 > 0)
			{
				num ++;
				pt.x = j;
				pt.y = i;
				delPoint.push_back(pt);
			}
			else if (*ptr1 == 0 && prePixel > 0)//
			{
				if (num < 4)
				{
					for (int k = 0; k < delPoint.size(); k++)
					{
						int x = delPoint[k].x;
						int y = delPoint[k].y;
						ptr2 =(unsigned char *)tempImg->imageData + y *tempImg->widthStep + x;
						*ptr2 = 0;
					}
				}
				delPoint.clear();
				num = 0;
			}
			prePixel = *ptr1;
			ptr1++;
		}
		ptr1 += dstImage->widthStep -dstImage->width;
	}
	//dstImage = NULL;
	dstImage = cvCreateImage(cvGetSize(tempImg), tempImg->depth, tempImg->nChannels);
	cvCopy(tempImg, dstImage);
	ptr1 = NULL;
	ptr2 = NULL;
	num = 0;
	delPoint.clear();
	prePixel = 0;
	ptr1 =(unsigned char *)dstImage->imageData ;
	//��ֱ������ɾ��������С��4��
	for (int i = 0; i < dstImage->width; i++)
	{
		for (int j = 0; j < dstImage->height; j++)
		{
			Point pt;
			if (*ptr1 > 0)
			{
				num ++;
				pt.x = i;
				pt.y = j;
				delPoint.push_back(pt);
			}
			else if (*ptr1 == 0 && prePixel > 0)//
			{
				if (num < 4)
				{
					for (int k = 0; k < delPoint.size(); k++)
					{
						int x = delPoint[k].x;
						int y = delPoint[k].y;
						ptr2 =(unsigned char *)tempImg->imageData + y *tempImg->widthStep + x;
						*ptr2 = 0;
					}
				}
				delPoint.clear();
				num = 0;
			}
			prePixel = *ptr1;
			ptr1 += dstImage->widthStep;
		}
		ptr1 += 1 - dstImage->height * dstImage->widthStep;
	}
	ptr1 = NULL;
	ptr2 = NULL;
	delete ptr1;
	delete ptr2;
	dstImage = NULL;
	dstImage = cvCreateImage(cvGetSize(tempImg), tempImg->depth, tempImg->nChannels);
	cvCopy(tempImg, dstImage);
	cvReleaseImage(&tempImg);

	//�Ѱ�Χ��ס�ĺڵ�����
	storage = cvCreateMemStorage(0);
	CvSeq* contours = NULL;
	CvScalar white = CV_RGB(255, 255, 255);
	cvFindContours(dstImage, storage, &contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
	while(contours)
	{
		cvDrawContours(dstImage, contours, white, white, -1, CV_FILLED);
		contours = contours->h_next;
	}
	contours = NULL;
	delete contours;

	cvCopy(dstImage, distImage);
	ShowImage(dstImage);

}


void CForTestDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//if (removeBg == 0)
	//{
	//}
	//else
	{
		Point2i pt;
		Point2i TempPt;
		CRect rect;
		CIRCLE cCircle;
		CWnd *pWnd = GetDlgItem(IDC_VIDEO);
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		vector<Point> resultPt;

		double meanScale=0.0;
		int ptNum = 0;

		if((point.x>=rect.left && point.x<=rect.right) && (point.y>=rect.top &&	point.y<=rect.bottom))
		{
			// ͨ����point�Ĵ������ʵ����picture�ؼ��еĵ��λ�ã����꣩����ɡ�
			TempPt.x = point.x - rect.left;
			TempPt.y = point.y - rect.top;
			pt = ReverseSize(TempPt);
			

			if (removeBg == 1 || removeBg == 0)
			{
				//�����������㷨�������ĵ����������а�ɫ�ĵ㣬Ȼ����minenclosing����ԲȦ
				resultPt = RegionGrow(dstImage, pt, 100);

				Point2f center;
				float radius = 0;
				if (resultPt.size() > 13)//��ֹ���ֹ��ٵ�
				{
					//int aa = resultPt.size();
					minEnclosingCircle(Mat(resultPt), center, radius);
					while (!resultPt.empty())
					{
						Point pt2i = resultPt.back();
						resultPt.pop_back();
						uchar* ptr = (uchar*)image_edge->imageData + pt2i.x  + pt2i.y * image_edge->widthStep;
						meanScale += *ptr;
						ptNum++;
					}
					meanScale /= ptNum;

					CString a, b, c, d;

					cCircle.radius = cvRound(radius);
					cCircle.center.x = cvRound(center.x);
					cCircle.center.y = cvRound(center.y);
					cCircle.meanScale = meanScale;

					if (1 == removeBg)
					{
						vCircles.push_back(cCircle);

						cvCircle(circleImage, center, cvRound(radius), CV_RGB(0, 255, 0), 3);
						cvCircle(GrayImage, center, cvRound(radius), CV_RGB(0, 255, 0), 3);
						a.Format(_T("%d"), num + 1);
						b.Format(_T("%d,%d"), cvRound(center.x), cvRound(center.y));
						c.Format(_T("%0.2f"), meanScale);
						d.Format(_T("%d"), cvRound(radius));
						m_tableWidget.AddItem(a, b, c, d);
						num++;
					}
					else if (0 == removeBg)
					{
						cvCvtColor(dstImage, GrayImage, CV_GRAY2RGBA);
						cvCopy(DetectImage, circleImage);
						m_tableWidget.DeleteAllItems();
						num = 0;

						vector<CIRCLE> vTempCircles;
						CIRCLE cTempCircle;
						while(!vCircles.empty())
						{
							cTempCircle = vCircles.back();
							vCircles.pop_back();
							if (abs(cTempCircle.center.x - cCircle.center.x) < 5 && abs(cTempCircle.center.y - cCircle.center.y) < 5)//�����ЩԲ����ȥ��
							{	
								continue;
							}
							cvCircle(circleImage, cTempCircle.center, cTempCircle.radius, CV_RGB(0, 255, 0), 3);
							cvCircle(GrayImage, cTempCircle.center, cTempCircle.radius, CV_RGB(0, 255, 0), 3);
							a.Format(_T("%d"), num + 1);
							b.Format(_T("%d,%d"), cTempCircle.center.x, cTempCircle.center.y);
							c.Format(_T("%0.2f"), cTempCircle.meanScale);
							d.Format(_T("%d"), cTempCircle.radius);
							m_tableWidget.AddItem(a, b, c, d);
							num++;
							vTempCircles.push_back(cTempCircle);
						}
						vCircles = vTempCircles;
					}
				}
				resultPt.swap(vector<Point>());
				UpdateData(FALSE);
				ShowImage(circleImage);
				resultPt.clear();
			}
			else
			{
				//�ֶ�ȥ����������ת��Ϊ�Ҷȣ�Ȼ�����������������ĵ�Ϊ��ɫ�������ĵ㲻��
				//IplImage* srcImage = cvCreateImage(cvGetSize(DetectImage), DetectImage->depth, DetectImage->nChannels);
				////cvCvtColor(DetectImage, srcImage, CV_RGB2HSV);
				//cvCopy(DetectImage, srcImage);
				IplImage* grImage = cvCreateImage(cvGetSize(dstImage), 8, 1);
				cvCopy(dstImage, grImage);
				//cvCvtColor(DetectImage, grayImage, CV_RGB2GRAY);

				Mat dtImage = Mat(dstImage);
				Point piont;
				resultPt = RegionGrow(grImage, pt, 100);
				if (resultPt.size() > 4)
				{
					while(!resultPt.empty())
					{
						piont = resultPt.back(); 
						resultPt.pop_back(); 
						//dtImage.at<Vec3b>(piont.y, piont.x) = Vec3b(0, 0, 0);
						dtImage.at<uchar>(piont.y, piont.x) = 0;
						//cvSet2D(srcImage, piont.x, piont.y, cvScalarAll(0));	//ÿ�ζ������⣬why������������				
					}

				}
				*grImage = IplImage(dtImage);
				cvCopy(grImage, dstImage);
				ShowImage(dstImage);
				grImage = NULL;
				cvReleaseImage(&grImage);
			}
		}

	}

	CDialogEx::OnLButtonDown(nFlags, point);
}




void CForTestDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialogEx::OnLButtonUp(nFlags, point);
}

//���������㷨���������ĵ����������а�ɫ�ĵ�
vector<Point> CForTestDlg::RegionGrow(IplImage* image, Point2i pt, int th)
{
	vector<Point> resultPt;							//���صĵ㼯
	Point2i ptGrowing;                              //��������λ��  
	int nGrowLable = 0;                             //����Ƿ�������  
	int nSrcValue = 0;                              //�������Ҷ�ֵ  
	int nCurValue = 0;                              //��ǰ������Ҷ�ֵ   
	if (image->nChannels != 1)
	{
		cvCvtColor(image, image, CV_RGB2GRAY);
	}
	Mat src(image);
	Mat matDst = Mat(cvGetSize(image), CV_8UC1,cvScalarAll(0));//����һ���հ��������Ϊ��ɫ 
	//Mat matDst = Mat(image);
	matDst = Scalar::all(0);
	//��������˳������  
	int DIR[8][2] = {{-1,-1}, {0,-1}, {1,-1}, {1,0}, {1,1}, {0,1}, {-1,1}, {-1,0}};  
	//int DIR[8][2] = {{0,-1}, {1,0}, {0,1}, {-1,0}}; 
	Vector<Point2i> vcGrowPt;                     //������ջ  
	vcGrowPt.push_back(pt);                         
	matDst.at<uchar>(pt.y, pt.x) = 255;               
	resultPt.push_back(pt);
	nSrcValue = src.at<uchar>(pt.y, pt.x);            
	//nSrcValue = (int)cvGetReal2D(dstImage, pt.x, pt.y);

	while (!vcGrowPt.empty())                       
	{  
		if (nSrcValue == 0)
		{
			break;
		}
		pt = vcGrowPt.back();                       //ȡ��һ��������  
		vcGrowPt.pop_back();                          

		//�ֱ�԰˸������ϵĵ��������  
		for (int i = 0; i<8; ++i)  
		{  
			ptGrowing.x = pt.x + DIR[i][0];       
			ptGrowing.y = pt.y + DIR[i][1];   
			//����Ƿ��Ǳ�Ե��  
			if (ptGrowing.x < 0 || ptGrowing.y < 0 || (ptGrowing.x > (src.cols-1)) || (ptGrowing.y >(src.rows -1)))  
				continue;  

			nGrowLable = matDst.at<uchar>(ptGrowing.y, ptGrowing.x);      //��ǰ��������ĻҶ�ֵ  

			if (nGrowLable == 0)                    //�����ǵ㻹û�б�����  
			{  
				//nCurValue = (int)cvGetReal2D(dstImage, ptGrowing.x, ptGrowing.y);
				nCurValue = src.at<uchar>(ptGrowing.y, ptGrowing.x);            
				if (abs(nSrcValue - nCurValue) < th)                 //����ֵ��Χ��������  
				{  
					matDst.at<uchar>(ptGrowing.y, ptGrowing.x) = 255;     //���Ϊ��ɫ  
					resultPt.push_back(ptGrowing);
					vcGrowPt.push_back(ptGrowing);                  //����һ��������ѹ��ջ��  
				}  
			}  
		}  
	}
	return resultPt;
}

void CForTestDlg::OnBnClickedrmbackgroud()
{
	Mat gray, remain;
	gray = Mat(distImage);
	remain = Mat(WatershedImg);

	UpdateData(TRUE);
	Mat edge = gray >= 100, dist;
	distanceTransform(edge, dist, CV_DIST_L2, CV_DIST_MASK_5);
	dist *= 5000;
	pow(dist, 0.5, dist);
	//normalize(dist, dist, 0.0, 255.0, NORM_MINMAX);

	Mat dist32s, dist8u1;
	dist.convertTo(dist32s, CV_32S, 1, 0.5);//��0.5:��������
	dist32s &= Scalar::all(255);
	dist32s.convertTo(dist8u1, CV_8U, 1, 0);

	IplImage* adaptiveImg = cvCreateImage(cvGetSize(dstImage), 8, 1);
	*adaptiveImg = IplImage(dist8u1);
	IplImage* srcImg = cvCreateImage(cvGetSize(dstImage), 8, 3);
	srcImg = cvCloneImage(DetectImage);
	//cvCvtColor(adaptiveImg, DetectImage, CV_GRAY2BGR);
	cvThreshold(adaptiveImg, adaptiveImg, m_iThresholdValue, 255.0, CV_THRESH_BINARY);
	//MorpHMin(adaptiveImg, adaptiveImg, m_iThresholdValue);

	IplImage* seperateImg = NULL;
	seperateImg = cvCreateImage(cvGetSize(dstImage), 8, 1);
	CvMemStorage* storage_sample = cvCreateMemStorage(0);
	CvSeq* contours_data = NULL;
	dist32s = Scalar::all(0);

	cvFindContours(adaptiveImg, storage_sample, &contours_data, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	int compCount = 0, i, j;
	while(contours_data)
	{
		cvDrawContours(adaptiveImg, contours_data, cvScalarAll(compCount+1), cvScalarAll(compCount+1), INT_MAX, -1, 8);
		contours_data = contours_data->h_next;
		compCount++;
	}
	dist32s = Mat(adaptiveImg);
	dist32s.convertTo(dist32s, CV_32S);

	Mat sourceImg= Mat(srcImg);
	BeginWaitCursor();
	watershed(sourceImg, dist32s);
	// paint the watershed image
	EndWaitCursor();
	Mat wshed(cvGetSize(dstImage), CV_8UC1);
	for( i = 0; i < dist32s.rows; i++ )
		for( j = 0; j < dist32s.cols; j++ )
		{
			int index = dist32s.at<int>(i,j);
			if( index == -1 )
				wshed.at<uchar>(i,j) = 0;
			else
				wshed.at<uchar>(i,j) = 255;
		}	
	wshed = wshed & gray;//gray��1ͨ���ģ�wshed����ͨ���ģ���Ȼ����������
	add(wshed, remain, wshed);
	*seperateImg = IplImage(wshed);
	cvCopy(seperateImg, dstImage);

	ShowImage(dstImage);
	adaptiveImg = NULL;
	cvReleaseImage(&adaptiveImg);
	seperateImg = NULL;
	cvReleaseImage(&seperateImg);
	srcImg = NULL;
	cvReleaseImage(&srcImg);
}

//�޶��Ҷ�ͼ�����ǿ
void CForTestDlg::edgeEnhance(cv::Mat& srcImg, cv::Mat& dstImg)
{
	if (!dstImg.empty())
	{
		dstImg.release();
	}

	std::vector<cv::Mat> rgb;

	if (srcImg.channels() == 3)        // rgb image
	{
		cv::split(srcImg, rgb);
	}
	else if (srcImg.channels() == 1)   // gray image
	{
		rgb.push_back(srcImg);
	}

	// �ֱ��R��G��B����ͨ�����б�Ե��ǿ
	for (size_t i = 0; i < rgb.size(); i++)
	{
		cv::Mat sharpMat8U;
		cv::Mat sharpMat;
		cv::Mat blurMat;

		// ��˹ƽ��
		cv::GaussianBlur(rgb[i], blurMat, cv::Size(3,3), 0, 0);

		// ����������˹
		cv::Laplacian(blurMat, sharpMat, CV_16S);

		// ת������
		sharpMat.convertTo(sharpMat8U, CV_8U);
		cv::add(rgb[i], sharpMat8U, rgb[i]);
	}
	dstImg = rgb[0].clone();

	//cv::merge(rgb, dstImg);
}

void CForTestDlg::OnBnClickededgeenhance()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//����һ����׼Բ
	UpdateData(TRUE);
	IplImage* sample = cvLoadImage("stdCircle.tif", 0);
	CvMemStorage* storage_sample = cvCreateMemStorage(0);
	CvSeq* contours_data = NULL;
	cvFindContours(sample, storage_sample, &contours_data, sizeof(CvContour), CV_RETR_EXTERNAL);
	double similarity = 0.0;//������Ե����ʶ��

	storage = cvCreateMemStorage(0);
	CvSeq* contours	= NULL;
	CvScalar black = CV_RGB(0, 0, 0);
	CvScalar white = CV_RGB(255, 255, 255);
	double area;
	cvFindContours(dstImage, storage, &contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
	while(contours)
	{
		area = cvContourArea(contours, CV_WHOLE_SEQ);
		similarity = cvMatchShapes(contours_data, contours, 2);
		if (/*fabs(area) <= 250 || */similarity > m_xiangsidu)
		{
			cvDrawContours(dstImage, contours, black, black, -1, CV_FILLED);
		} 
		else
		{
			cvDrawContours(dstImage, contours, white, white, -1, CV_FILLED);

		}
		contours = contours->h_next;
	}
	cvReleaseMemStorage(&storage);
	//cvReleaseData(&contours);
	cvReleaseMemStorage(&storage_sample);
	//cvReleaseData(&contours_data);
	cvReleaseImage(&sample);
	cvCopy(dstImage, distImage);
	ShowImage(dstImage);

}

//void CForTestDlg::cvThin( IplImage* src, IplImage* dst, int iterations)
//{
//	//��ʱ��src��һ����ֵ����ͼƬ
//	CvSize size = cvGetSize(src);
//	cvCopy(src, dst);
//
//	int n = 0,i = 0,j = 0;
//	for(n=0; n<iterations; n++)//��ʼ���е���
//	{
//		IplImage* t_image = cvCloneImage(dst);
//		for(i=0; i<size.height;  i++)
//		{
//			for(j=0; j<size.width; j++)
//			{
//				if(CV_IMAGE_ELEM(t_image,byte,i,j)==1)
//				{
//					int ap=0;
//					int p2 = (i==0)?0:CV_IMAGE_ELEM(t_image,byte, i-1, j);
//					int p3 = (i==0 || j==size.width-1)?0:CV_IMAGE_ELEM(t_image,byte, i-1, j+1);
//					if (p2==0 && p3==1)
//					{
//						ap++;
//					}
//
//					int p4 = (j==size.width-1)?0:CV_IMAGE_ELEM(t_image,byte,i,j+1);
//					if(p3==0 && p4==1)
//					{
//						ap++;
//					}
//
//					int p5 = (i==size.height-1 || j==size.width-1)?0:CV_IMAGE_ELEM(t_image,byte,i+1,j+1);
//					if(p4==0 && p5==1)
//					{
//						ap++;
//					}
//
//					int p6 = (i==size.height-1)?0:CV_IMAGE_ELEM(t_image,byte,i+1,j);
//					if(p5==0 && p6==1)
//					{
//						ap++;
//					}
//
//					int p7 = (i==size.height-1 || j==0)?0:CV_IMAGE_ELEM(t_image,byte,i+1,j-1);
//					if(p6==0 && p7==1)
//					{
//						ap++;
//					}
//
//					int p8 = (j==0)?0:CV_IMAGE_ELEM(t_image,byte,i,j-1);
//					if(p7==0 && p8==1)
//					{
//						ap++;
//					}
//
//					int p9 = (i==0 || j==0)?0:CV_IMAGE_ELEM(t_image,byte,i-1,j-1);
//					if(p8==0 && p9==1)
//					{
//						ap++;
//					}
//					if(p9==0 && p2==1)
//					{
//						ap++;
//					}
//
//					if((p2+p3+p4+p5+p6+p7+p8+p9)>1 && (p2+p3+p4+p5+p6+p7+p8+p9)<7)
//					{
//						if(ap==1)
//						{
//							if(!(p2 && p4 && p6))
//							{
//								if(!(p4 && p6 && p8)) 
//								{
//									CV_IMAGE_ELEM(dst,byte,i,j)=0;//����Ŀ��ͼ��������ֵΪ0�ĵ�
//								}
//							}
//						}
//					}
//
//				}
//			}
//		}
//
//		cvReleaseImage(&t_image);
//
//		t_image = cvCloneImage(dst);
//		for(i=0; i<size.height;  i++)
//		{
//			for(int j=0; j<size.width; j++)
//			{
//				if(CV_IMAGE_ELEM(t_image,byte,i,j)==1)
//				{
//					int ap=0;
//					int p2 = (i==0)?0:CV_IMAGE_ELEM(t_image,byte, i-1, j);
//					int p3 = (i==0 || j==size.width-1)?0:CV_IMAGE_ELEM(t_image,byte, i-1, j+1);
//					if (p2==0 && p3==1)
//					{
//						ap++;
//					}
//					int p4 = (j==size.width-1)?0:CV_IMAGE_ELEM(t_image,byte,i,j+1);
//					if(p3==0 && p4==1)
//					{
//						ap++;
//					}
//					int p5 = (i==size.height-1 || j==size.width-1)?0:CV_IMAGE_ELEM(t_image,byte,i+1,j+1);
//					if(p4==0 && p5==1)
//					{
//						ap++;
//					}
//					int p6 = (i==size.height-1)?0:CV_IMAGE_ELEM(t_image,byte,i+1,j);
//					if(p5==0 && p6==1)
//					{
//						ap++;
//					}
//					int p7 = (i==size.height-1 || j==0)?0:CV_IMAGE_ELEM(t_image,byte,i+1,j-1);
//					if(p6==0 && p7==1)
//					{
//						ap++;
//					}
//					int p8 = (j==0)?0:CV_IMAGE_ELEM(t_image,byte,i,j-1);
//					if(p7==0 && p8==1)
//					{
//						ap++;
//					}
//					int p9 = (i==0 || j==0)?0:CV_IMAGE_ELEM(t_image,byte,i-1,j-1);
//					if(p8==0 && p9==1)
//					{
//						ap++;
//					}
//					if(p9==0 && p2==1)
//					{
//						ap++;
//					}
//					if((p2+p3+p4+p5+p6+p7+p8+p9)>1 && (p2+p3+p4+p5+p6+p7+p8+p9)<7)
//					{
//						if(ap==1)
//						{
//							if(p2*p4*p8==0)
//							{
//								if(p2*p6*p8==0)
//								{
//									CV_IMAGE_ELEM(dst, byte,i,j)=0;
//								}
//							}
//						}
//					}                    
//				}
//
//			}
//
//		}            
//		cvReleaseImage(&t_image);
//	}
//
//}


void CForTestDlg::cvThin(cv::Mat& src, cv::Mat& dst, int intera)
{
	if(src.type()!=CV_8UC1)
	{
		printf("ֻ�ܴ����ֵ��Ҷ�ͼ��\n");
		return;
	}
	//��ԭ�ز���ʱ��copy src��dst
	if(dst.data!=src.data)
	{
		src.copyTo(dst);
	}

	int i, j, n;
	int width, height;
	width = src.cols -1;
	//֮���Լ�1���Ƿ��㴦��8���򣬷�ֹԽ��
	height = src.rows -1;
	int step = src.step;
	int  p2,p3,p4,p5,p6,p7,p8,p9;
	uchar* img;
	bool ifEnd;
	int A1;
	cv::Mat tmpimg;
	//n��ʾ��������
	for(n = 0; n<intera; n++)
	{
		dst.copyTo(tmpimg);
		ifEnd = false;
		img = tmpimg.data;
		for(i = 1; i < height; i++)
		{
			img += step;
			for(j =1; j<width; j++)
			{
				uchar* p = img + j;
				A1 = 0;
				if( p[0] > 0)
				{
					if(p[-step]==0&&p[-step+1]>0) //p2,p3 01ģʽ
					{
						A1++;
					}
					if(p[-step+1]==0&&p[1]>0) //p3,p4 01ģʽ
					{
						A1++;
					}
					if(p[1]==0&&p[step+1]>0) //p4,p5 01ģʽ
					{
						A1++;
					}
					if(p[step+1]==0&&p[step]>0) //p5,p6 01ģʽ
					{
						A1++;
					}
					if(p[step]==0&&p[step-1]>0) //p6,p7 01ģʽ
					{
						A1++;
					}
					if(p[step-1]==0&&p[-1]>0) //p7,p8 01ģʽ
					{
						A1++;
					}
					if(p[-1]==0&&p[-step-1]>0) //p8,p9 01ģʽ
					{
						A1++;
					}
					if(p[-step-1]==0&&p[-step]>0) //p9,p2 01ģʽ
					{
						A1++;
					}
					p2 = p[-step]>0?1:0;
					p3 = p[-step+1]>0?1:0;
					p4 = p[1]>0?1:0;
					p5 = p[step+1]>0?1:0;
					p6 = p[step]>0?1:0;
					p7 = p[step-1]>0?1:0;
					p8 = p[-1]>0?1:0;
					p9 = p[-step-1]>0?1:0;
					if((p2+p3+p4+p5+p6+p7+p8+p9)>1 && (p2+p3+p4+p5+p6+p7+p8+p9)<7  &&  A1==1)
					{
						if((p2==0||p4==0||p6==0)&&(p4==0||p6==0||p8==0)) //p2*p4*p6=0 && p4*p6*p8==0
						{
							dst.at<uchar>(i,j) = 0; //����ɾ�����������õ�ǰ����Ϊ0
							ifEnd = true;
						}
					}
				}
			}
		}

		dst.copyTo(tmpimg);
		img = tmpimg.data;
		for(i = 1; i < height; i++)
		{
			img += step;
			for(j =1; j<width; j++)
			{
				A1 = 0;
				uchar* p = img + j;
				if( p[0] > 0)
				{
					if(p[-step]==0&&p[-step+1]>0) //p2,p3 01ģʽ
					{
						A1++;
					}
					if(p[-step+1]==0&&p[1]>0) //p3,p4 01ģʽ
					{
						A1++;
					}
					if(p[1]==0&&p[step+1]>0) //p4,p5 01ģʽ
					{
						A1++;
					}
					if(p[step+1]==0&&p[step]>0) //p5,p6 01ģʽ
					{
						A1++;
					}
					if(p[step]==0&&p[step-1]>0) //p6,p7 01ģʽ
					{
						A1++;
					}
					if(p[step-1]==0&&p[-1]>0) //p7,p8 01ģʽ
					{
						A1++;
					}
					if(p[-1]==0&&p[-step-1]>0) //p8,p9 01ģʽ
					{
						A1++;
					}
					if(p[-step-1]==0&&p[-step]>0) //p9,p2 01ģʽ
					{
						A1++;
					}
					p2 = p[-step]>0?1:0;
					p3 = p[-step+1]>0?1:0;
					p4 = p[1]>0?1:0;
					p5 = p[step+1]>0?1:0;
					p6 = p[step]>0?1:0;
					p7 = p[step-1]>0?1:0;
					p8 = p[-1]>0?1:0;
					p9 = p[-step-1]>0?1:0;
					if((p2+p3+p4+p5+p6+p7+p8+p9)>1 && (p2+p3+p4+p5+p6+p7+p8+p9)<7  &&  A1==1)
					{
						if((p2==0||p4==0||p8==0)&&(p2==0||p6==0||p8==0)) //p2*p4*p8=0 && p2*p6*p8==0
						{
							dst.at<uchar>(i,j) = 0; //����ɾ�����������õ�ǰ����Ϊ0
							ifEnd = true;
						}
					}
				}
			}
		}

		//��������ӵ����Ѿ�û�п���ϸ���������ˣ����˳�����
		if(!ifEnd) break;
	}
	tmpimg.release();
	img = NULL;
	delete img;
}


void CForTestDlg::OnBnClickedButton5()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	storage = cvCreateMemStorage(0);
	CvSeq* contours = NULL;
	CvScalar black = CV_RGB(0, 0, 0);
	CvScalar white = CV_RGB(255, 255, 255);
	vector<Vec3f> circles;
	IplImage* tImg = cvCreateImage(cvGetSize(dstImage), dstImage->depth, dstImage->nChannels);
	cvFindContours(dstImage, storage, &contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
	while(contours)
	{
		Mat gray = Mat(cvGetSize(dstImage), CV_8UC1, 0);//����һ���հ��������Ϊ��ɫ 
		tImg = cvCreateImage(cvGetSize(dstImage), dstImage->depth, dstImage->nChannels);
		cvDrawContours(tImg, contours, white, white, -1, CV_FILLED);
		gray = Mat(tImg);
		double low = 0.0, high = 0.0;
		AdaptiveFindThreshold(dstImage, &low, &high);
		HoughCircles(gray, circles, CV_HOUGH_GRADIENT, 2.1, 40, high, 7.5, m_xiao - 3, 45);
		IplImage* Image = cvCreateImage(cvGetSize(tImg), tImg->depth, 3);
		cvCvtColor(tImg, Image, CV_GRAY2RGBA);

		for (size_t i = 0; i < circles.size(); i++)
		{
			CvPoint pt = cvPoint(cvRound(circles[i][0]), cvRound(circles[i][1]));//Բ��
			int r = cvRound(circles[i][2]);
			double area = r * r * CV_PI;//Բ���
			unsigned char * ptr = NULL;
			int pixelNum = 0;
			int onCircleNum = 0;



			//�������Բ�Ƿ�������ǿ�Բ
			if ((pt.y - r) < 0 || (pt.x - r) < 0 || (pt.x + r) > 944 || (pt.y + r) > 708/*|| r < 100 || r > 300*/)
			{
				ShowImage(Image);
				Image = NULL;
				cvReleaseImage(&Image);
				continue;
			}
			ptr = (unsigned char *)tImg->imageData + (pt.y - r)*tImg->widthStep + (pt.x - r);
			for (int j = 0; j < r* 2; j++)
			{
				for (int k = 0; k < r * 2; k++)
				{
					CvPoint point = cvPoint(pt.x  - r + k, pt.y - r + j);
					if (IsInTheCircle(point, pt, r))
					{
						if ( *ptr > 250)//��ֵͼ�����ȡֵ
						{
							pixelNum++;
							if (IsOnTheCircle(point, pt, r))
							{
								onCircleNum++;
							}
						}
					}
					ptr++;
				}
				ptr += dstImage->widthStep - 2 * r;
			}
			//��һ���ǰ�ɫ���ж�Ϊ��ȷ��Բ
			if (pixelNum > area / 2 && onCircleNum > 5)
			{
				cvCircle(Image, pt, r, CV_RGB(255, 0, 0), 3);
			}
		}
		ShowImage(Image);
		Image = NULL;
		cvReleaseImage(&Image);


		contours = contours->h_next;
	}
	cvReleaseMemStorage(&storage);
	//ShowImage(dstImage, IDC_VIDEO);
}


void CForTestDlg::OnBnClickedExport()
{
	///ʶ�����
	vCircles.clear();
	CIRCLE cCircle;
	//������ʾ����3ͨ������������ɫ��ʶ
	UpdateData(TRUE);
	cvCvtColor(dstImage, GrayImage, CV_GRAY2RGBA);
	cvCopy(DetectImage, circleImage);
	m_tableWidget.DeleteAllItems();//���������Ŀ
	uchar* ptr = NULL;
	num = 0;
	//UpdateData(TRUE);

	storage = cvCreateMemStorage(0);
	CvSeq* contours	= NULL;
	CvScalar black = CV_RGB(0, 0, 0);
	CvScalar white = CV_RGB(255, 255, 255);
	cvFindContours(dstImage, storage, &contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
	//vector<float> detectPoint;
	double meanDetect = 0.0;
	while(contours)
	{
		CvPoint2D32f center;
		float radius = 0;
		ptr = NULL;
		int pixelNum = 0;
		int onCircleNum = 0;
		cvDrawContours(dstImage, contours, white, white, -1, CV_FILLED);
		cvMinEnclosingCircle(contours, &center, &radius);

		CString a, b, c, d;
		CvPoint pt = cvPoint(cvRound(center.x), cvRound(center.y));
		int r = cvRound(radius);
		double meanScale = 0.0;//ƽ���Ҷ�
		double mean = 0.0;
		double circleArea = r * r * CV_PI;
		double contourArea = cvContourArea(contours);
		//��ƽ���Ҷ�

/*
		//��ֹ����������ᵼ�±��ϵ�Բ����ʶ�𣬰취������������ж�
		if ((pt.y - r) < 0 || (pt.x - r) < 0 || (pt.x + r) > image_edge->width || (pt.y + r) > image_edge->height)
		{
			contours = contours->h_next;
			continue;//�������Բ�Ƿ�������ǿ�Բ
		}
*/
		int j = 0, k = 0;

		if (r > m_maxRadius || r < m_minRadius)
		{
			contours = contours->h_next;
			continue;//�������Բ�Ƿ�������ǿ�Բ
		}
		//�˴�������ȥ��Բ�ڲ��Ƿ����
		//ptr = (uchar*)image_edge->imageData + (pt.y - r)*image_edge->widthStep + (pt.x - r);
		for (j = 0; j < r* 2; j++)
		{
			for (k = 0; k < r * 2; k++)
			{
				if ((pt.y - r + j) < 0 || (pt.x - r + k) < 0 || (pt.x - r + k) > image_edge->width || (pt.y - r + j) > image_edge->height)
				{
					//��ֹ���
					continue;
				}
				ptr = (uchar*)image_edge->imageData + (pt.y - r + j)*image_edge->widthStep + (pt.x  - r + k);
				CvPoint point = cvPoint(pt.x  - r + k, pt.y - r + j);
				if (IsInTheCircle(point, pt, r))
				{
					mean += *ptr;
					pixelNum++;
				}
				//ptr++;
			}
			//ptr += image_edge->widthStep - 2 * r;
		}
		mean /= pixelNum;

		pixelNum = 0;
		//ptr = (uchar*)image_edge->imageData + (pt.y - r)*image_edge->widthStep + (pt.x - r);
		for (j = 0; j < r* 2; j++)
		{
			for (k = 0; k < r * 2; k++)
			{
				if ((pt.y - r + j) < 0 || (pt.x - r + k) < 0 || (pt.x - r + k) > image_edge->width || (pt.y - r + j) > image_edge->height)
				{
					//��ֹ���
					continue;
				}
				ptr = (uchar*)image_edge->imageData + (pt.y - r + j)*image_edge->widthStep + (pt.x  - r + k);
				CvPoint point = cvPoint(pt.x  - r + k, pt.y - r + j);
				if (IsInTheCircle(point, pt, r))
				{
					if (*ptr > (mean - 5))
					{
						meanScale += *ptr;
						pixelNum++;
					}
				}
				//ptr++;
			}
			//ptr += image_edge->widthStep - 2 * r;
		}
		meanScale /= pixelNum;
		meanDetect += meanScale;//�ó�ƽ��ֵ


		//�˴��ö�ֵͼ
		pixelNum = 0;
		//ptr = (uchar*)dstImage->imageData + (pt.y - r)*dstImage->widthStep + (pt.x - r);
		for (j = 0; j < r* 2; j++)
		{
			for (k = 0; k < r * 2; k++)
			{
				if ((pt.y - r + j) < 0 || (pt.x - r + k) < 0 || (pt.x - r + k) > dstImage->width || (pt.y - r + j) > dstImage->height)
				{
					//��ֹ���
					continue;
				}
				ptr = (uchar*)dstImage->imageData + (pt.y - r + j)*dstImage->widthStep + (pt.x  - r + k);
				CvPoint point = cvPoint(pt.x  - r + k, pt.y - r + j);
				if (IsInTheCircle(point, pt, r))
				{
					if (*ptr > 120)//��ֵͼ�����ȡֵ
					{
						pixelNum++;
						if (IsOnTheCircle(point, pt, r))
						{
							onCircleNum++;
						}
					}
				}
				//ptr++;
			}
			//ptr += dstImage->widthStep - 2 * r;
		}
		//if (((pixelNum > (4 * circleArea / 11)) && (onCircleNum > 5)) || (pixelNum > (6 * circleArea / 11)))
		if ((pixelNum > (5 * circleArea / 11)) && contourArea > 4 * circleArea/ 11)
		{
			cCircle.radius = cvRound(radius);
			cCircle.center.x = cvRound(center.x);
			cCircle.center.y = cvRound(center.y);
			cCircle.meanScale = meanScale;
			vCircles.push_back(cCircle);

			cvCircle(circleImage, pt, r, CV_RGB(0, 255, 0), 3);
			cvCircle(GrayImage, pt, r, CV_RGB(0, 255, 0), 3);
			a.Format(_T("%d"), num + 1);
			b.Format(_T("%d,%d"), cvRound(center.x), cvRound(center.y));
			if (meanScale < 10.0)
			{
				 
				c.Format(_T("0%0.2f"), meanScale);
			}
			else
			{
				c.Format(_T("%0.2f"), meanScale);
			}
			d.Format(_T("%d"), cvRound(radius));
			m_tableWidget.AddItem(a, b, c, d);
			num++;
		}

		contours = contours->h_next;
	}

	if (!RecogFlag)
	{
		m_selImage.AddString(_T("�ָ�ͼ"));
		m_selImage.AddString(_T("�Ҷ�ͼ"));
		m_selImage.AddString(_T("ʶ��ָ�ͼ"));
		m_selImage.AddString(_T("ʶ��ԭͼ"));
	}
	RecogFlag = true;

	ShowImage(circleImage);
	cvReleaseMemStorage(&storage);
	ptr = NULL;
	delete ptr;
}


void CForTestDlg::OnBnClickedCountdefault()
{
	if (m_tableWidget.GetItemCount() <= 0)
	{
		MessageBox(_T("�б�û�м�¼���棡"));
		return;
	}
	CString FileName;
	CTime time = CTime::GetCurrentTime();
	FileName = time.Format(_T("ʶ��%Y%m%d%H%M"));

	CFileDialog dlg(FALSE, _T("xls"), FileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Excel �ļ�(*.xls)|*.xls"));
	dlg.m_ofn.lpstrTitle = _T("�ļ��б����Ϊ");
	if (dlg.DoModal() != IDOK)
	{
		return;
	}
	CString strFilePath;
	strFilePath = dlg.GetPathName();
	//�ж��ļ��Ƿ���ڣ�������ɾ���ؽ�
	DWORD dw = GetFileAttributes(strFilePath);
	if (dw != (DWORD) - 1)
	{
		DeleteFile(strFilePath);
	}

	CDatabase database;
	CString sDriver = _T("MICROSOFT EXCEL DRIVER (*.XLS)"); // Excel����
	CString sSql, strInsert;
	try
	{
		sSql.Format(_T("DRIVER={%s};DSN='';FIRSTROWHASNAMES=1;READONLY=FALSE;CREATE_DB=\"%s\";DBQ=%s"),sDriver, strFilePath, strFilePath);

		//�������ݿ�(��Excel����ļ�)
		if (database.OpenEx(sSql, CDatabase::noOdbcDialog))
		{
			//��ȡ�б��������
			int iColumnNum, iRowCount;
			LVCOLUMN lvCol;
			CString strColName;//used for saving title name
			int i,j;//loop parameter

			iColumnNum = m_tableWidget.GetHeaderCtrl()->GetItemCount();
			iRowCount = m_tableWidget.GetItemCount();
			sSql = "CREATE TABLE simit (";//sheet����Ϊsimit
			strInsert = "INSERT INTO simit ( ";

			//get table title name
			lvCol.mask = LVCF_TEXT; //�������ã�˵��LVCOLUMN������pszText������Ч
			lvCol.cchTextMax = 32;//���裬pszText������ָ����ַ����Ĵ�С
			lvCol.pszText = strColName.GetBuffer(32);//���裬pszText ��ָ����ַ�����ʵ�ʴ洢λ�á�
			//���������������ú����ͨ�� GetColumn()��������б��������
			for (i = 0; i < iColumnNum; i++)
			{
				if (!(m_tableWidget.GetColumn(i, &lvCol)))
				{
					return;
				}
				if (i < iColumnNum - 1)
				{
					sSql = sSql + lvCol.pszText + _T(" TEXT,");
					strInsert = strInsert +lvCol.pszText + _T(",");
				}
				else
				{
					sSql = sSql + lvCol.pszText + _T(" TEXT)");
					strInsert = strInsert +lvCol.pszText + _T(")");
				}
			}
			strInsert = strInsert + _T(" VALUES (");
			database.ExecuteSQL(sSql);

			//������ֵ��ѭ�������¼�����뵽excel��
			sSql = strInsert;
			CString chTemp;
			for (j = 0; j < iRowCount; j ++)
			{
				for (i = 0; i < iColumnNum; i++)
				{
					chTemp = m_tableWidget.GetItemText(j, i);

					if ( i < (iColumnNum-1) )
					{
						sSql = sSql + _T("'") + chTemp + _T("',");
					}
					else
					{
						sSql = sSql + _T("'") + chTemp + _T("')");
					}
				}
				database.ExecuteSQL(sSql);
				sSql = strInsert;
			}

		}

		// �ر�Excel����ļ�
		database.Close();
		MessageBox(_T("����ʶ����ΪExcel�ļ��ɹ���"),_T("��ʾ"),0);
	}
	catch(CDBException* e)
	{
		//�������ͺܶ࣬������Ҫ���б���
		AfxMessageBox(e->m_strError);
	}
}


void CForTestDlg::OnBnClickedCheck4()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UINT ch = IsDlgButtonChecked(IDC_CHECK4);
	if (ch == BST_CHECKED)
	{
		removeBg = 2;
		GetDlgItem(IDC_CHECK4)->SetWindowText(_T("�ֶ�ȥ����"));
	}
	else if (ch == BST_INDETERMINATE)
	{
		removeBg = 1;
		GetDlgItem(IDC_CHECK4)->SetWindowText(_T("�ֶ�����Բ"));
	}
	else
	{
		removeBg = 0;
		GetDlgItem(IDC_CHECK4)->SetWindowText(_T("�޲���"));
	}
}


void CForTestDlg::OnBnClickedButton4()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString thinText;
	nThin++;
	nNum++;
	thinText.Format(_T("ϸ�� %d(%d)"), nThin, nNum);

	IplImage* pTemp = cvCloneImage(dstImage);
	cvThreshold(dstImage, pTemp, 68, 1, CV_THRESH_BINARY);
	Mat tempMat = Mat(pTemp);
	Mat dstMat = cvCreateMat(pTemp->height, pTemp->width, CV_8UC1);
	cvThin(tempMat, dstMat, 1);
	*dstImage = IplImage(dstMat);
	cvThreshold(dstImage, dstImage, 0.5, 255, CV_THRESH_BINARY);

	GetDlgItem(IDC_BUTTON4)->SetWindowTextW(thinText);
	cvCopy(dstImage, distImage);
	ShowImage(dstImage);
	cvReleaseImage(&pTemp);
	tempMat.release();
	dstMat.release();
}


void CForTestDlg::OnCbnSelchangeSelimage()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int selNum = m_selImage.GetCurSel();
	if (RecogFlag)
	{
		switch(selNum)
		{
		case 0:
			ShowImage(DetectImage);
			break;
		case  1:
			ShowImage(dstImage);
			break;
		case 2:
			ShowImage(image_edge);
			break;
		case 3:
			ShowImage(GrayImage);
			break;
		case 4:
			ShowImage(circleImage);
			break;
		default:
			ShowImage(DetectImage);
		}
	}
}


void CForTestDlg::OnBnClickedButton6()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString erodeText;
	nErode++;
	nNum++;
	erodeText.Format(_T("���� %d(%d)"), nErode, nNum);
	IplConvKernel* kernel = cvCreateStructuringElementEx(3, 3, 1, 1, CV_SHAPE_ELLIPSE, 0);
	cvDilate(dstImage, dstImage, kernel, 1);
	//cvMorphologyEx(dstImage, dstImage, NULL, kernel, CV_MOP_OPEN, 2);

	//cvErode(dstImage, dstImage, kernel, 2);
	//cvMorphologyEx(dstImage, dstImage, NULL, kernel, CV_MOP_OPEN, 2);
	//cvErode(dstImage, dstImage, kernel, 1);
	//cvMorphologyEx(dstImage, dstImage, NULL, kernel, CV_MOP_OPEN, 2);
	GetDlgItem(IDC_erode)->SetWindowText(erodeText);
	cvCopy(dstImage, distImage);
	ShowImage(dstImage);
	kernel = NULL;
	delete kernel;
}

//��̬ѧ����Сֵ
void CForTestDlg::MorpHMin(const IplImage* src, IplImage* dst, unsigned char h, IplConvKernel* se)
{
	assert(src != NULL  &&  dst != NULL && src != dst );

	//p150
	IplImage*  temp = cvCreateImage(cvGetSize(src), 8, 1);

	cvAddS(src, cvScalar(h), temp);

	MorpRErode(temp, src, dst, se);

	cvReleaseImage(&temp);
}

//��̬ѧ��ظ�ʴ�͸�ʴ�ؽ�����
void CForTestDlg::MorpRErode(const IplImage* src,  const IplImage* msk, IplImage* dst, IplConvKernel* se, int iterations)
{

	assert(src != NULL  && msk != NULL && dst != NULL && src != dst );

	if(iterations < 0)
	{
		//��ʴ�ؽ�
		cvMax(src, msk, dst);
		cvErode(dst, dst, se);
		cvMax(dst, msk, dst);

		IplImage*  temp1 = cvCreateImage(cvGetSize(src), 8, 1);
		//IplImage*  temp2 = cvCreateImage(cvGetSize(src), 8, 1);

		do
		{
			//record last result
			cvCopy(dst, temp1);
			cvErode(dst, dst, se);
			cvMax(dst, msk, dst);
			//cvCmp(temp1, dst, temp2, CV_CMP_NE);

		}
		//while(cvSum(temp2).val[0] != 0);
		while(ImageCmp(temp1, dst)!= 0);

		cvReleaseImage(&temp1);
		//cvReleaseImage(&temp2);

		return;	

	}
	else if (iterations == 0)
	{
		cvCopy(src, dst);
	}
	else
	{
		//��ͨ��ظ�ʴ p137(6.2)
		cvMax(src, msk, dst);
		cvErode(dst, dst, se);
		cvMax(dst, msk, dst);

		for(int i=1; i<iterations; i++)
		{
			cvErode(dst, dst, se);
			cvMax(dst, msk, dst);
		}
	}
}
//�Ƚ�����ͼ���Ƿ���ͬ�� 0 ��ͬ
int  CForTestDlg::ImageCmp(const IplImage* img1, const IplImage* img2)
{
	assert(img1->width == img2->width && img1->height == img2->height && img1->imageSize == img2->imageSize );
	return memcmp(img1->imageData, img2->imageData, img1->imageSize);
}


void CForTestDlg::OnBnClickedButton7()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	cvCopy(dstImage, distImage);
}
