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
	//形态学结构元素的复制
	IplConvKernel* lhStructuringElementCopy(IplConvKernel* se);

	//形态学结构元素的取反
	IplConvKernel* lhStructuringElementNot(IplConvKernel* se);
	//形态学结构元素的基数
	int lhStructuringElementCard(IplConvKernel* se);

	//形态学结构元素的映射
	IplConvKernel* lhStructuringElementMap(IplConvKernel* se);

	//形态学线性结构元素的创建，常用于形态学方向分析
	IplConvKernel* lhStructuringElementLine(unsigned int angle, unsigned int len);


	//形态学开运算
	void lhMorpOpen(const IplImage* src, IplImage* dst, IplConvKernel* se=NULL, int iterations=1);
	//形态学闭运算
	void lhMorpClose(const IplImage* src, IplImage* dst, IplConvKernel* se=NULL, int iterations=1);

	//形态学基本梯度运算
	void lhMorpGradient(const IplImage* src, IplImage* dst, IplConvKernel* se=NULL, int iterations=1);
	//形态学内梯度运算
	void lhMorpGradientIn(const IplImage* src, IplImage* dst, IplConvKernel* se=NULL, int iterations=1);

	//形态学外梯度运算
	void lhMorpGradientOut(const IplImage* src, IplImage* dst, IplConvKernel* se=NULL, int iterations=1);

	//形态学方向梯度
	void lhMorpGradientDir(const IplImage* src, IplImage* dst, unsigned int angle, unsigned int len );

	//形态学白顶帽运算
	void lhMorpWhiteTopHat(const IplImage* src, IplImage* dst, IplConvKernel* se=NULL, int iterations=1);


	//形态学黑顶帽运算
	void lhMorpBlackTopHat(const IplImage* src, IplImage* dst, IplConvKernel* se=NULL, int iterations=1);
	//形态学自补顶帽运算
	void lhMorpQTopHat(const IplImage* src, IplImage* dst, IplConvKernel* se=NULL, int iterations=1);
	//形态学对比度增强运算
	void lhMorpEnhance(const IplImage* src, IplImage* dst, IplConvKernel* se=NULL, int iterations=1);

	//形态学二值击中-击不中变换
	void lhMorpHMTB(const IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg =NULL);


	//形态学非约束击中-击不中变换 针对二值和灰度图像
	void lhMorpHMTU(const IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg =NULL);

	//形态学约束击中-击不中变换 针对二值和灰度图像
	void lhMorpHMTC(const IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg =NULL);

	//形态学约束击中-击不中变换
	void lhMorpHMT(const IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg =NULL, int type=LH_MORP_TYPE_BINARY);

	//形态学击中-击不中开变换 
	void lhMorpHMTOpen(const IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg =NULL, int type=LH_MORP_TYPE_BINARY);

	//形态学细化运算
	void lhMorpThin(const IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg =NULL, int type=LH_MORP_TYPE_BINARY);

	//形态学细化匹配运算
	void lhMorpThinFit(const IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg =NULL, int type=LH_MORP_TYPE_BINARY);

	//形态学粗化运算
	void lhMorpThick(const IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg =NULL, int type=LH_MORP_TYPE_BINARY);

	//形态学粗化不匹配运算
	void lhMorpThickMiss(const IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg =NULL, int type=LH_MORP_TYPE_BINARY);


	//比较两个图像是否相同， 0 相同
	int  lhImageCmp(const IplImage* img1, const IplImage* img2);

	//形态学测地膨胀和膨胀重建运算
	void lhMorpRDilate(const IplImage* src, const IplImage* msk, IplImage* dst, IplConvKernel* se = NULL, int iterations=-1);

	//形态学测地腐蚀和腐蚀重建运算
	void lhMorpRErode(const IplImage* src,  const IplImage* msk, IplImage* dst, IplConvKernel* se = NULL, int iterations=-1);

	//形态学开重建
	void lhMorpROpen(const IplImage* src, IplImage* dst, IplConvKernel* se = NULL, int iterations=1);

	//形态学闭重建
	void lhMorpRClose(const IplImage* src, IplImage* dst, IplConvKernel* se = NULL, int iterations=1);

	//形态学白帽重建
	void lhMorpRWTH(const IplImage* src, IplImage* dst, IplConvKernel* se = NULL, int iterations=1);

	//形态学黑帽重建
	void lhMorpRBTH(const IplImage* src, IplImage* dst, IplConvKernel* se = NULL, int iterations=1);


	//形态学测地自对偶和自对偶重建运算
	void lhMorpRSelfDual(const IplImage* src, const IplImage* msk, IplImage* dst, IplConvKernel* se = NULL, int iterations=-1);



	//形态学区域极小值
	void lhMorpRMin(const IplImage* src, IplImage* dst, IplConvKernel* se = NULL);

	//形态学区域极大值
	void lhMorpRMax(const IplImage* src, IplImage* dst, IplConvKernel* se = NULL);

	//形态学H极大值
	void lhMorpHMax(const IplImage* src, IplImage* dst, unsigned char h, IplConvKernel* se = NULL);


	//形态学H极小值
	void lhMorpHMin(const IplImage* src, IplImage* dst, unsigned char h, IplConvKernel* se = NULL);

	//形态学H凹变换
	void lhMorpHConcave(const IplImage* src, IplImage* dst, unsigned char h, IplConvKernel* se = NULL);

	//形态学H凸变换
	void lhMorpHConvex(const IplImage* src, IplImage* dst, unsigned char h, IplConvKernel* se = NULL);

	//形态学扩展极大值
	void lhMorpEMax(const IplImage* src, IplImage* dst, unsigned char h, IplConvKernel* se = NULL);

	//形态学扩展极小值
	void lhMorpEMin(const IplImage* src, IplImage* dst, unsigned char h, IplConvKernel* se = NULL);


	//形态学等级滤波器（二值,默认SE为矩形3*3）
	void lhMorpRankFilterB(const IplImage* src, IplImage* dst, IplConvKernel* se = NULL, unsigned int rank = 0);

	//形态学重建应用1：去除边界的连通区域
	void lhMorpRemoveBoderObj(const IplImage* src, IplImage* dst);


	//形态学重建应用2：空洞的填充
	void lhMorpFillHole(const IplImage* src, IplImage* dst);

	//形态学极小值强加
	void lhMorpMinimaImpose(const IplImage* src, IplImage* markers,  IplImage* dst, IplConvKernel* se = NULL);

	//形态学极大值强加
	void lhMorpMaximaImpose(const IplImage* src, IplImage* markers,  IplImage* dst, IplConvKernel* se = NULL);

};

