#pragma once
#include "windows.h"
#include "cv.h"
#include "highgui.h"
#include "CvvImage.h"
#include <iostream>

using namespace cv;
using namespace std;

class Morphology
{
public:
	Morphology(void);
	~Morphology(void);


#define LH_MORP_TYPE_BINARY			0
#define LH_MORP_TYPE_UNCONSTRAIN	1
#define LH_MORP_TYPE_CONSTRAIN		2
	//��̬ѧ�ṹԪ�صĸ���
	IplConvKernel* lhStructuringElementCopy(IplConvKernel* se);

	//��̬ѧ�ṹԪ�ص�ȡ��
	IplConvKernel* lhStructuringElementNot(IplConvKernel* se);
	//��̬ѧ�ṹԪ�صĻ���
	int lhStructuringElementCard(IplConvKernel* se);

	//��̬ѧ�ṹԪ�ص�ӳ��
	IplConvKernel* lhStructuringElementMap(IplConvKernel* se);

	//��̬ѧ���ԽṹԪ�صĴ�������������̬ѧ�������
	IplConvKernel* lhStructuringElementLine(unsigned int angle, unsigned int len);


	//��̬ѧ������
	void lhMorpOpen(const IplImage* src, IplImage* dst, IplConvKernel* se=NULL, int iterations=1);
	//��̬ѧ������
	void lhMorpClose(const IplImage* src, IplImage* dst, IplConvKernel* se=NULL, int iterations=1);

	//��̬ѧ�����ݶ�����
	void lhMorpGradient(const IplImage* src, IplImage* dst, IplConvKernel* se=NULL, int iterations=1);
	//��̬ѧ���ݶ�����
	void lhMorpGradientIn(const IplImage* src, IplImage* dst, IplConvKernel* se=NULL, int iterations=1);

	//��̬ѧ���ݶ�����
	void lhMorpGradientOut(const IplImage* src, IplImage* dst, IplConvKernel* se=NULL, int iterations=1);

	//��̬ѧ�����ݶ�
	void lhMorpGradientDir(const IplImage* src, IplImage* dst, unsigned int angle, unsigned int len );

	//��̬ѧ�׶�ñ����
	void lhMorpWhiteTopHat(const IplImage* src, IplImage* dst, IplConvKernel* se=NULL, int iterations=1);


	//��̬ѧ�ڶ�ñ����
	void lhMorpBlackTopHat(const IplImage* src, IplImage* dst, IplConvKernel* se=NULL, int iterations=1);
	//��̬ѧ�Բ���ñ����
	void lhMorpQTopHat(const IplImage* src, IplImage* dst, IplConvKernel* se=NULL, int iterations=1);
	//��̬ѧ�Աȶ���ǿ����
	void lhMorpEnhance(const IplImage* src, IplImage* dst, IplConvKernel* se=NULL, int iterations=1);

	//��̬ѧ��ֵ����-�����б任
	void lhMorpHMTB(const IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg =NULL);


	//��̬ѧ��Լ������-�����б任 ��Զ�ֵ�ͻҶ�ͼ��
	void lhMorpHMTU(const IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg =NULL);

	//��̬ѧԼ������-�����б任 ��Զ�ֵ�ͻҶ�ͼ��
	void lhMorpHMTC(const IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg =NULL);

	//��̬ѧԼ������-�����б任
	void lhMorpHMT(const IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg =NULL, int type=LH_MORP_TYPE_BINARY);

	//��̬ѧ����-�����п��任 
	void lhMorpHMTOpen(const IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg =NULL, int type=LH_MORP_TYPE_BINARY);

	//��̬ѧϸ������
	void lhMorpThin(const IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg =NULL, int type=LH_MORP_TYPE_BINARY);

	//��̬ѧϸ��ƥ������
	void lhMorpThinFit(const IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg =NULL, int type=LH_MORP_TYPE_BINARY);

	//��̬ѧ�ֻ�����
	void lhMorpThick(const IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg =NULL, int type=LH_MORP_TYPE_BINARY);

	//��̬ѧ�ֻ���ƥ������
	void lhMorpThickMiss(const IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg =NULL, int type=LH_MORP_TYPE_BINARY);


	//�Ƚ�����ͼ���Ƿ���ͬ�� 0 ��ͬ
	int  lhImageCmp(const IplImage* img1, const IplImage* img2);

	//��̬ѧ������ͺ������ؽ�����
	void lhMorpRDilate(const IplImage* src, const IplImage* msk, IplImage* dst, IplConvKernel* se = NULL, int iterations=-1);

	//��̬ѧ��ظ�ʴ�͸�ʴ�ؽ�����
	void lhMorpRErode(const IplImage* src,  const IplImage* msk, IplImage* dst, IplConvKernel* se = NULL, int iterations=-1);

	//��̬ѧ���ؽ�
	void lhMorpROpen(const IplImage* src, IplImage* dst, IplConvKernel* se = NULL, int iterations=1);

	//��̬ѧ���ؽ�
	void lhMorpRClose(const IplImage* src, IplImage* dst, IplConvKernel* se = NULL, int iterations=1);

	//��̬ѧ��ñ�ؽ�
	void lhMorpRWTH(const IplImage* src, IplImage* dst, IplConvKernel* se = NULL, int iterations=1);

	//��̬ѧ��ñ�ؽ�
	void lhMorpRBTH(const IplImage* src, IplImage* dst, IplConvKernel* se = NULL, int iterations=1);


	//��̬ѧ����Զ�ż���Զ�ż�ؽ�����
	void lhMorpRSelfDual(const IplImage* src, const IplImage* msk, IplImage* dst, IplConvKernel* se = NULL, int iterations=-1);



	//��̬ѧ����Сֵ
	void lhMorpRMin(const IplImage* src, IplImage* dst, IplConvKernel* se = NULL);

	//��̬ѧ���򼫴�ֵ
	void lhMorpRMax(const IplImage* src, IplImage* dst, IplConvKernel* se = NULL);

	//��̬ѧH����ֵ
	void lhMorpHMax(const IplImage* src, IplImage* dst, unsigned char h, IplConvKernel* se = NULL);


	//��̬ѧH��Сֵ
	void lhMorpHMin(const IplImage* src, IplImage* dst, unsigned char h, IplConvKernel* se = NULL);

	//��̬ѧH���任
	void lhMorpHConcave(const IplImage* src, IplImage* dst, unsigned char h, IplConvKernel* se = NULL);

	//��̬ѧH͹�任
	void lhMorpHConvex(const IplImage* src, IplImage* dst, unsigned char h, IplConvKernel* se = NULL);

	//��̬ѧ��չ����ֵ
	void lhMorpEMax(const IplImage* src, IplImage* dst, unsigned char h, IplConvKernel* se = NULL);

	//��̬ѧ��չ��Сֵ
	void lhMorpEMin(const IplImage* src, IplImage* dst, unsigned char h, IplConvKernel* se = NULL);


	//��̬ѧ�ȼ��˲�������ֵ,Ĭ��SEΪ����3*3��
	void lhMorpRankFilterB(const IplImage* src, IplImage* dst, IplConvKernel* se = NULL, unsigned int rank = 0);

	//��̬ѧ�ؽ�Ӧ��1��ȥ���߽����ͨ����
	void lhMorpRemoveBoderObj(const IplImage* src, IplImage* dst);


	//��̬ѧ�ؽ�Ӧ��2���ն������
	void lhMorpFillHole(const IplImage* src, IplImage* dst);

	//��̬ѧ��Сֵǿ��
	void lhMorpMinimaImpose(const IplImage* src, IplImage* markers,  IplImage* dst, IplConvKernel* se = NULL);

	//��̬ѧ����ֵǿ��
	void lhMorpMaximaImpose(const IplImage* src, IplImage* markers,  IplImage* dst, IplConvKernel* se = NULL);

};

