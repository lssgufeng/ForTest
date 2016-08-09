#include "StdAfx.h"
#include "Morphology.h"


Morphology::Morphology(void)
{
}


Morphology::~Morphology(void)
{
}
//形态学结构元素的复制
IplConvKernel* Morphology::lhStructuringElementCopy(IplConvKernel* se)
{

	IplConvKernel* copy = cvCreateStructuringElementEx( se->nCols, se->nRows, 
		se->anchorX, se->anchorY, 0, NULL );

	copy->nShiftR = se->nShiftR;

	memcpy( copy->values, se->values, sizeof(int) * se->nRows * se->nCols );

	return copy;
}

//形态学结构元素的取反
IplConvKernel* Morphology::lhStructuringElementNot(IplConvKernel* se)
{

	IplConvKernel* temp = cvCreateStructuringElementEx( se->nCols, se->nRows, 
		se->anchorX, se->anchorY, 0, NULL );

	temp->nShiftR = se->nShiftR;

	memcpy( temp->values, se->values, sizeof(int) * se->nRows * se->nCols );

	for(int i=0; i<temp->nRows * temp->nCols ; i++)
	{
		temp->values[i] = !temp->values[i] ;
	}

	return temp;
}
//形态学结构元素的基数
int Morphology::lhStructuringElementCard(IplConvKernel* se)
{
	assert(se != NULL);
	int i, cnt = 0;

	for (i=0; i<se->nCols*se->nRows; i++)
	{
		cnt += se->values[i];
	}
	return cnt;

}

//形态学结构元素的映射
IplConvKernel* Morphology::lhStructuringElementMap(IplConvKernel* se)
{
	CvMat *mat = cvCreateMat( se->nRows,  se->nCols, CV_32SC1);
	memcpy(mat->data.i, se->values, sizeof(int) * se->nRows * se->nCols );
	cvFlip(mat, NULL, -1);

	IplConvKernel* semap = cvCreateStructuringElementEx( se->nCols, se->nRows, 
		se->nCols-se->anchorX-1, se->nRows-se->anchorY-1, 0, NULL );

	semap->nShiftR = se->nShiftR;

	memcpy( semap->values, mat->data.i, sizeof(int) * se->nRows * se->nCols );

	cvReleaseMat(&mat);

	return semap;
}

//形态学线性结构元素的创建，常用于形态学方向分析
IplConvKernel* Morphology::lhStructuringElementLine(unsigned int angle, unsigned int len)
{
	assert(len > 2);
	angle = angle%180;

	CvPoint p1 = {0};
	CvPoint p2 = {0};
	int width = cvRound(len*cos((float)angle*CV_PI/180.0));
	int height = cvRound(len*sin((float)angle*CV_PI/180.0));

	height = height >= 1 ? height : 1;

	if (width < 0)
	{
		width = width <= -1 ? width : -1;
		p1.x = 0;
		p1.y = 0;
		p2.x = -width -1;
		p2.y = height -1;
	}
	else
	{
		width = width >= 1 ? width : 1;
		p1.x = 0;
		p1.y = height -1;
		p2.x = width -1;
		p2.y = 0;
	}
	CvMat *temp = cvCreateMat(height, width, CV_32SC1);
	cvZero(temp);
	cvLine(temp, p1, p2, cvScalar(1,1,1), 1, 4, 0);

	IplConvKernel* se = cvCreateStructuringElementEx( width, height, 
		(width-1)/2, (height-1)/2,  CV_SHAPE_CUSTOM, temp->data.i );

	cvReleaseMat(&temp);
	return se;

}


//形态学开运算
void Morphology::lhMorpOpen(const IplImage* src, IplImage* dst, IplConvKernel* se, int iterations)
{

	cvErode( src, dst, se, iterations );

	IplConvKernel* semap = lhStructuringElementMap(se);

	cvDilate( dst, dst, semap, iterations );

	cvReleaseStructuringElement(&semap);

}

//形态学闭运算
void Morphology::lhMorpClose(const IplImage* src, IplImage* dst, IplConvKernel* se, int iterations)
{

	cvDilate( src, dst, se, iterations );

	IplConvKernel* semap = lhStructuringElementMap(se);

	cvErode( dst, dst, semap, iterations );

	cvReleaseStructuringElement(&semap);

}

//形态学基本梯度运算
void Morphology::lhMorpGradient(const IplImage* src, IplImage* dst, IplConvKernel* se, int iterations)
{
	assert(src != NULL && dst != NULL && src != dst);
	IplImage*  temp = cvCloneImage(src);
	cvErode( src, temp, se, iterations );
	cvDilate( src, dst, se, iterations );
	cvSub( dst, temp, dst );
	cvReleaseImage(&temp);
}

//形态学内梯度运算
void Morphology::lhMorpGradientIn(const IplImage* src, IplImage* dst, IplConvKernel* se, int iterations)
{
	assert(src != NULL && dst != NULL && src != dst);
	cvErode( src, dst, se, iterations );
	cvSub( src, dst, dst );
}

//形态学外梯度运算
void Morphology::lhMorpGradientOut(const IplImage* src, IplImage* dst, IplConvKernel* se, int iterations)
{
	assert(src != NULL && dst != NULL && src != dst);
	cvDilate( src, dst, se, iterations );
	cvSub(dst, src, dst );
}

//形态学方向梯度
void Morphology::lhMorpGradientDir(const IplImage* src, IplImage* dst, unsigned int angle, unsigned int len )
{
	assert(src != NULL && dst != NULL && src != dst);
	IplConvKernel* se = lhStructuringElementLine(angle, len);
	lhMorpGradient(src, dst, se);
	cvReleaseStructuringElement(&se);
}

//形态学白顶帽运算
void Morphology::lhMorpWhiteTopHat(const IplImage* src, IplImage* dst, IplConvKernel* se, int iterations)
{
	assert(src != NULL && dst != NULL && src != dst);
	lhMorpOpen(src, dst, se, iterations );
	cvSub( src, dst, dst );
}


//形态学黑顶帽运算
void Morphology::lhMorpBlackTopHat(const IplImage* src, IplImage* dst, IplConvKernel* se, int iterations)
{
	assert(src != NULL && dst != NULL && src != dst);
	lhMorpClose(src, dst, se, iterations );
	cvSub(dst, src, dst );

}

//形态学自补顶帽运算
void Morphology::lhMorpQTopHat(const IplImage* src, IplImage* dst, IplConvKernel* se, int iterations)
{
	assert(src != NULL && dst != NULL && src != dst);
	IplImage*  temp = cvCloneImage(src);
	lhMorpClose( src, temp, se, iterations );
	lhMorpOpen( src, dst, se, iterations );
	cvSub(temp, dst, dst );
	cvReleaseImage(&temp);
}

//形态学对比度增强运算
void Morphology::lhMorpEnhance(const IplImage* src, IplImage* dst, IplConvKernel* se, int iterations)
{
	assert(src != NULL && dst != NULL && src != dst);
	IplImage*  temp = cvCloneImage(src);
	lhMorpWhiteTopHat( src, temp, se, iterations );
	lhMorpBlackTopHat( src, dst, se, iterations );
	cvAdd(src, temp, temp);
	cvSub(temp, dst, dst );
	cvReleaseImage(&temp);
}

//形态学二值击中-击不中变换
void Morphology::lhMorpHMTB(const IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg)
{
	assert(src != NULL && dst != NULL && src != dst && sefg!= NULL && sefg!=sebg);

	if (sebg == NULL)
	{
		sebg = lhStructuringElementNot(sefg);

	}
	IplImage*  temp1 = cvCreateImage(cvGetSize(src), 8, 1);
	IplImage*  temp2 = cvCreateImage(cvGetSize(src), 8, 1);

	//P104 (5.2)
	cvErode( src, temp1, sefg);
	cvNot(src, temp2);
	cvErode( temp2, temp2, sebg);
	cvAnd(temp1, temp2, dst);


	cvReleaseImage(&temp1);
	cvReleaseImage(&temp2);

	cvReleaseStructuringElement(&sebg);

}


//形态学非约束击中-击不中变换 针对二值和灰度图像
void Morphology::lhMorpHMTU(const IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg)
{
	assert(src != NULL && dst != NULL && src != dst && sefg!= NULL && sefg!=sebg);

	if (sebg == NULL)
	{
		sebg = lhStructuringElementNot(sefg);

	}

	IplImage*  temp = cvCreateImage(cvGetSize(src), 8, 1);
	IplImage*  mask = cvCreateImage(cvGetSize(src), 8, 1);
	cvZero(mask);

	//P106 (5.4)
	cvErode( src, temp, sefg);
	cvDilate(src, dst, sebg);
	cvCmp(temp, dst, mask, CV_CMP_GT);

	cvSub(temp, dst, dst, mask);
	cvNot(mask, mask);
	cvSet(dst, cvScalar(0), mask);

	//cvCopy(dst, mask);
	//cvSet(dst, cvScalar(255), mask);
	cvReleaseImage(&mask);
	cvReleaseImage(&temp);

	cvReleaseStructuringElement(&sebg);
}

//形态学约束击中-击不中变换 针对二值和灰度图像
void Morphology::lhMorpHMTC(const IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg)
{
	assert(src != NULL && dst != NULL && src != dst && sefg!= NULL && sefg!=sebg);

	if (sebg == NULL)
	{
		sebg = lhStructuringElementNot(sefg);

	}

	IplImage*  temp1 = cvCreateImage(cvGetSize(src), 8, 1);
	IplImage*  temp2 = cvCreateImage(cvGetSize(src), 8, 1);
	IplImage*  temp3 = cvCreateImage(cvGetSize(src), 8, 1);
	IplImage*  temp4 = cvCreateImage(cvGetSize(src), 8, 1);

	IplImage*  mask1 = cvCreateImage(cvGetSize(src), 8, 1);
	IplImage*  mask2 = cvCreateImage(cvGetSize(src), 8, 1);
	IplImage*  mask3 = cvCreateImage(cvGetSize(src), 8, 1);
	IplImage*  mask4 = cvCreateImage(cvGetSize(src), 8, 1);

	cvZero(mask1);
	cvZero(mask2);
	cvZero(mask3);
	cvZero(mask4);

	cvZero(dst);

	//P107 (5.5)
	cvErode( src, temp1, sebg);
	cvDilate( src, temp2, sebg);
	cvErode( src, temp3, sefg);
	cvDilate( src, temp4, sefg);

	cvCmp(src, temp3, mask1, CV_CMP_EQ);
	cvCmp(temp2, src,  mask2, CV_CMP_LT);
	cvAnd(mask1, mask2, mask2);

	cvCmp(src, temp4, mask3 , CV_CMP_EQ);
	cvCmp(temp1, src, mask4 , CV_CMP_GT);
	cvAnd(mask3, mask4, mask4);

	cvSub(src, temp2, dst, mask2);
	cvSub(temp1, src, dst, mask4);




	cvReleaseImage(&mask1);
	cvReleaseImage(&mask2);
	cvReleaseImage(&mask3);
	cvReleaseImage(&mask4);

	cvReleaseImage(&temp1);
	cvReleaseImage(&temp2);
	cvReleaseImage(&temp3);
	cvReleaseImage(&temp4);

	cvReleaseStructuringElement(&sebg);

}

//形态学约束击中-击不中变换
void Morphology::lhMorpHMT(const IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg, int type)
{
	switch(type)
	{
	case LH_MORP_TYPE_BINARY:
		{
			lhMorpHMTB(src, dst, sefg, sebg);
			break;
		}

	case LH_MORP_TYPE_UNCONSTRAIN:
		{
			lhMorpHMTU(src, dst, sefg, sebg);
			break;
		}

	case LH_MORP_TYPE_CONSTRAIN:
		{
			lhMorpHMTC(src, dst, sefg, sebg);
			break;
		}

	default:
		break;

	}

}

//形态学击中-击不中开变换 
void Morphology::lhMorpHMTOpen(const IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg, int type)
{
	assert(src != NULL && dst != NULL && src != dst && sefg!= NULL && sefg!=sebg);

	IplConvKernel* semap = lhStructuringElementMap(sefg);

	IplImage*  temp = cvCreateImage(cvGetSize(src), 8, 1);

	//P110 (5.8)
	lhMorpHMT(src, temp, sefg, sebg, type);
	cvDilate(temp, dst, semap);

	cvReleaseImage(&temp);
	cvReleaseStructuringElement(&semap);

}

//形态学细化运算
void Morphology::lhMorpThin(const IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg, int type)
{

	assert(src != NULL && dst != NULL && src != dst && sefg!= NULL && sefg!=sebg);


	lhMorpHMT(src, dst, sefg, NULL, type);
	cvSub(src, dst, dst);

}

//形态学细化匹配运算
void Morphology::lhMorpThinFit(const IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg, int type)
{

	assert(src != NULL && dst != NULL && src != dst && sefg!= NULL && sefg!=sebg);

	lhMorpHMTOpen(src, dst, sefg, NULL, type);
	cvSub(src, dst, dst);
}

//形态学粗化运算
void Morphology::lhMorpThick(const IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg, int type)
{

	assert(src != NULL && dst != NULL && src != dst && sefg!= NULL && sefg!=sebg);


	lhMorpHMT(src, dst, sefg, NULL, type);
	cvAdd(src, dst, dst);

}

//形态学粗化不匹配运算
void Morphology::lhMorpThickMiss(const IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg, int type)
{

	assert(src != NULL && dst != NULL && src != dst && sefg!= NULL && sefg!=sebg);

	lhMorpHMTOpen(src, dst, sefg, NULL, type);
	cvAdd(src, dst, dst);
}


//比较两个图像是否相同， 0 相同
int  Morphology::lhImageCmp(const IplImage* img1, const IplImage* img2)
{
	assert(img1->width == img2->width && img1->height == img2->height && img1->imageSize == img2->imageSize );
	return memcmp(img1->imageData, img2->imageData, img1->imageSize);
}

//形态学测地膨胀和膨胀重建运算
void Morphology::lhMorpRDilate(const IplImage* src, const IplImage* msk, IplImage* dst, IplConvKernel* se, int iterations)
{

	assert(src != NULL && msk != NULL && dst != NULL && src != dst );

	if(iterations < 0)
	{
		//膨胀重建
		cvMin(src, msk, dst);
		cvDilate(dst, dst, se);
		cvMin(dst, msk, dst);

		IplImage*  temp1 = cvCreateImage(cvGetSize(src), 8, 1);
		//IplImage*  temp2 = cvCreateImage(cvGetSize(src), 8, 1);

		do
		{
			//record last result
			cvCopy(dst, temp1);
			cvDilate(dst, dst, se);
			cvMin(dst, msk, dst);
			//cvCmp(temp1, dst, temp2, CV_CMP_NE );

		}
		//while(cvSum(temp2).val[0] != 0);
		while(lhImageCmp(temp1, dst)!= 0);

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

		//普通测地膨胀 p136(6.1)
		cvMin(src, msk, dst);
		cvDilate(dst, dst, se);
		cvMin(dst, msk, dst);

		for(int i=1; i<iterations; i++)
		{
			cvDilate(dst, dst, se);
			cvMin(dst, msk, dst);

		}

	}
}

//形态学测地腐蚀和腐蚀重建运算
void Morphology::lhMorpRErode(const IplImage* src,  const IplImage* msk, IplImage* dst, IplConvKernel* se, int iterations)
{

	assert(src != NULL  && msk != NULL && dst != NULL && src != dst );

	if(iterations < 0)
	{
		//腐蚀重建
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
		while(lhImageCmp(temp1, dst)!= 0);

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
		//普通测地腐蚀 p137(6.2)
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

//形态学开重建
void Morphology::lhMorpROpen(const IplImage* src, IplImage* dst, IplConvKernel* se, int iterations)
{
	assert(src != NULL  && dst != NULL && src != dst );

	//p155(6.16)
	IplImage*  temp = cvCreateImage(cvGetSize(src), 8, 1);
	cvErode(src, temp, se, iterations);
	lhMorpRDilate(temp, src, dst, se, -1);
	cvReleaseImage(&temp);

}

//形态学闭重建
void Morphology::lhMorpRClose(const IplImage* src, IplImage* dst, IplConvKernel* se, int iterations)
{
	assert(src != NULL  && dst != NULL && src != dst );

	//p155(6.17)
	IplImage*  temp = cvCreateImage(cvGetSize(src), 8, 1);
	cvDilate(src, temp, se, iterations);
	lhMorpRErode(temp, src, dst, se, -1);
	cvReleaseImage(&temp);

}

//形态学白帽重建
void Morphology::lhMorpRWTH(const IplImage* src, IplImage* dst, IplConvKernel* se, int iterations)
{
	assert(src != NULL  && dst != NULL && src != dst );
	//p156
	lhMorpROpen(src, dst, se, iterations);
	cvSub(src, dst, dst);
}

//形态学黑帽重建
void Morphology::lhMorpRBTH(const IplImage* src, IplImage* dst, IplConvKernel* se, int iterations)
{
	assert(src != NULL  && dst != NULL && src != dst );
	//p156
	lhMorpRClose(src, dst, se, iterations);
	cvSub(dst, src, dst);
}


//形态学测地自对偶和自对偶重建运算
void Morphology::lhMorpRSelfDual(const IplImage* src, const IplImage* msk, IplImage* dst, IplConvKernel* se, int iterations)
{
	assert(src != NULL  && msk != NULL && dst != NULL && src != dst );

	//p140(6.7) p142 (6.10)
	IplImage*  temp1 = cvCreateImage(cvGetSize(src), 8, 1);
	IplImage*  temp2 = cvCreateImage(cvGetSize(src), 8, 1);

	cvZero(temp2);

	lhMorpRDilate(src, msk, temp1, se, iterations);

	lhMorpRErode(src, msk, dst, se, iterations);

	cvCmp(src, msk, temp2, CV_CMP_LE);

	cvCopy(temp1, dst, temp2);

	cvReleaseImage(&temp1);
	cvReleaseImage(&temp2);
}



//形态学区域极小值
void Morphology::lhMorpRMin(const IplImage* src, IplImage* dst, IplConvKernel* se)
{
	assert(src != NULL  &&  dst != NULL && src != dst );

	//p149 (6.14)
	IplImage*  temp = cvCreateImage(cvGetSize(src), 8, 1);

	cvAddS(src, cvScalar(1), temp);

	lhMorpRErode(temp, src, dst, se);

	cvSub(dst, src, dst);

	cvReleaseImage(&temp);

}

//形态学区域极大值
void Morphology::lhMorpRMax(const IplImage* src, IplImage* dst, IplConvKernel* se)
{
	assert(src != NULL  &&  dst != NULL && src != dst );

	//p149 (6.13)
	IplImage*  temp = cvCreateImage(cvGetSize(src), 8, 1);

	cvSubS(src, cvScalar(1), temp);

	lhMorpRDilate(temp, src, dst, se);

	cvSub(src, dst, dst);

	cvReleaseImage(&temp);

}

//形态学H极大值
void Morphology::lhMorpHMax(const IplImage* src, IplImage* dst, unsigned char h, IplConvKernel* se)
{
	assert(src != NULL  &&  dst != NULL && src != dst );

	//p150
	IplImage*  temp = cvCreateImage(cvGetSize(src), 8, 1);

	cvSubS(src, cvScalar(h), temp);

	lhMorpRDilate(temp, src, dst, se);

	cvReleaseImage(&temp);

}


//形态学H极小值
void Morphology::lhMorpHMin(const IplImage* src, IplImage* dst, unsigned char h, IplConvKernel* se)
{
	assert(src != NULL  &&  dst != NULL && src != dst );

	//p150
	IplImage*  temp = cvCreateImage(cvGetSize(src), 8, 1);

	cvAddS(src, cvScalar(h), temp);

	lhMorpRErode(temp, src, dst, se);

	cvReleaseImage(&temp);

}

//形态学H凹变换
void Morphology::lhMorpHConcave(const IplImage* src, IplImage* dst, unsigned char h, IplConvKernel* se)
{
	assert(src != NULL  &&  dst != NULL && src != dst );

	//p150

	lhMorpHMin(src, dst, h, se);
	cvSub(dst, src, dst);

}

//形态学H凸变换
void Morphology::lhMorpHConvex(const IplImage* src, IplImage* dst, unsigned char h, IplConvKernel* se )
{
	assert(src != NULL  &&  dst != NULL && src != dst );

	//p150

	lhMorpHMax(src, dst, h, se);
	cvSub(src, dst, dst);

}

//形态学扩展极大值
void Morphology::lhMorpEMax(const IplImage* src, IplImage* dst, unsigned char h, IplConvKernel* se )
{
	assert(src != NULL  &&  dst != NULL && src != dst );

	//p150
	IplImage*  temp = cvCreateImage(cvGetSize(src), 8, 1);

	lhMorpHMax(src, temp, h, se);
	lhMorpRMax(temp, dst, se);

	cvReleaseImage(&temp);

}

//形态学扩展极小值
void Morphology::lhMorpEMin(const IplImage* src, IplImage* dst, unsigned char h, IplConvKernel* se)
{
	assert(src != NULL  &&  dst != NULL && src != dst );

	//p150
	IplImage*  temp = cvCreateImage(cvGetSize(src), 8, 1);

	lhMorpHMin(src, temp, h, se);
	lhMorpRMin(temp, dst, se);

	cvReleaseImage(&temp);

}


//形态学等级滤波器（二值,默认SE为矩形3*3）
void Morphology::lhMorpRankFilterB(const IplImage* src, IplImage* dst, IplConvKernel* se, unsigned int rank)
{
	assert(src != NULL  &&  dst != NULL && src != dst );

	bool defaultse = false;
	int card;
	if (se == NULL)
	{
		card = 3*3;
		assert(rank >= 0 && rank <= card);
		se = cvCreateStructuringElementEx(3, 3, 1, 1, CV_SHAPE_RECT);
		defaultse = true;
	}
	else
	{
		card = lhStructuringElementCard(se);
		assert(rank >= 0 && rank <= card);
	}

	//default rank is median
	if (rank == 0)
		rank = card/2+1;

	IplConvKernel* semap =	lhStructuringElementMap(se);

	CvMat *semat = cvCreateMat(semap->nRows, semap->nCols, CV_32FC1);

	int i;
	for (i=0; i<semap->nRows*semap->nCols; i++)
	{
		semat->data.fl[i] = semap->values[i];
	}

	cvThreshold(src, dst, 0, 1, CV_THRESH_BINARY);
	IplImage *temp = cvCreateImage(cvGetSize(dst), 8, 1);

	cvFilter2D(dst, temp, semat, cvPoint(semap->anchorX, semap->anchorY));

	cvThreshold(temp, dst, card-rank, 255, CV_THRESH_BINARY);

	cvReleaseMat(&semat);
	cvReleaseStructuringElement(&semap);

	if (defaultse)
		cvReleaseStructuringElement(&se);	

	cvReleaseImage(&temp);

}

//形态学重建应用1：去除边界的连通区域
void Morphology::lhMorpRemoveBoderObj(const IplImage* src, IplImage* dst)
{
	IplImage *temp = cvCloneImage(src);
	//double min, max;
	//cvMinMaxLoc(src, &min, &max);

	//标记图像
	cvRectangle(temp, cvPoint(3,3), cvPoint(temp->width-7, temp->height-7), CV_RGB(0,0,0), -1);

	//将原图像作为掩模图像
	lhMorpRDilate(temp, src, dst);

	cvReleaseImage(&temp);
	//cvSet((CvArr*)src, cvScalar(min), dst);
	//cvCopy(src, dst);
	cvSub(src, dst, dst);
}


//形态学重建应用2：空洞的填充
void Morphology::lhMorpFillHole(const IplImage* src, IplImage* dst)
{
	IplImage *temp = cvCloneImage(src);
	double min, max;
	cvMinMaxLoc(src, &min, &max);
	//标记图像
	cvRectangle(temp, cvPoint(3,3), cvPoint(temp->width-7, temp->height-7), CV_RGB(max,max,max), -1);

	//将原图像作为掩模图像
	lhMorpRErode(temp, src, dst);

	cvReleaseImage(&temp);
	//cvSub(src, dst, dst);
}

//形态学极小值强加
void Morphology::lhMorpMinimaImpose(const IplImage* src, IplImage* markers,  IplImage* dst, IplConvKernel* se)
{
	assert(src != NULL  && markers != NULL && dst != NULL && src != dst );

	IplImage*  temp = cvCreateImage(cvGetSize(src), 8, 1);

	cvAddS(src, cvScalar(1), temp);

	cvMin(temp , markers, temp);

	lhMorpRErode(markers, temp, dst, se);
}

//形态学极大值强加
void Morphology::lhMorpMaximaImpose(const IplImage* src, IplImage* markers,  IplImage* dst, IplConvKernel* se)
{
	assert(src != NULL  && markers != NULL && dst != NULL && src != dst );

	IplImage*  temp = cvCreateImage(cvGetSize(src), 8, 1);

	cvSubS(src, cvScalar(1), temp);

	cvMax(temp , markers, temp);

	lhMorpRDilate(markers, temp, dst, se);

}


