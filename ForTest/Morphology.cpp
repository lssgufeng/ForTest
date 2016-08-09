#include "StdAfx.h"
#include "Morphology.h"


Morphology::Morphology(void)
{
}


Morphology::~Morphology(void)
{
}
//��̬ѧ�ṹԪ�صĸ���
IplConvKernel* Morphology::lhStructuringElementCopy(IplConvKernel* se)
{

	IplConvKernel* copy = cvCreateStructuringElementEx( se->nCols, se->nRows, 
		se->anchorX, se->anchorY, 0, NULL );

	copy->nShiftR = se->nShiftR;

	memcpy( copy->values, se->values, sizeof(int) * se->nRows * se->nCols );

	return copy;
}

//��̬ѧ�ṹԪ�ص�ȡ��
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
//��̬ѧ�ṹԪ�صĻ���
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

//��̬ѧ�ṹԪ�ص�ӳ��
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

//��̬ѧ���ԽṹԪ�صĴ�������������̬ѧ�������
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


//��̬ѧ������
void Morphology::lhMorpOpen(const IplImage* src, IplImage* dst, IplConvKernel* se, int iterations)
{

	cvErode( src, dst, se, iterations );

	IplConvKernel* semap = lhStructuringElementMap(se);

	cvDilate( dst, dst, semap, iterations );

	cvReleaseStructuringElement(&semap);

}

//��̬ѧ������
void Morphology::lhMorpClose(const IplImage* src, IplImage* dst, IplConvKernel* se, int iterations)
{

	cvDilate( src, dst, se, iterations );

	IplConvKernel* semap = lhStructuringElementMap(se);

	cvErode( dst, dst, semap, iterations );

	cvReleaseStructuringElement(&semap);

}

//��̬ѧ�����ݶ�����
void Morphology::lhMorpGradient(const IplImage* src, IplImage* dst, IplConvKernel* se, int iterations)
{
	assert(src != NULL && dst != NULL && src != dst);
	IplImage*  temp = cvCloneImage(src);
	cvErode( src, temp, se, iterations );
	cvDilate( src, dst, se, iterations );
	cvSub( dst, temp, dst );
	cvReleaseImage(&temp);
}

//��̬ѧ���ݶ�����
void Morphology::lhMorpGradientIn(const IplImage* src, IplImage* dst, IplConvKernel* se, int iterations)
{
	assert(src != NULL && dst != NULL && src != dst);
	cvErode( src, dst, se, iterations );
	cvSub( src, dst, dst );
}

//��̬ѧ���ݶ�����
void Morphology::lhMorpGradientOut(const IplImage* src, IplImage* dst, IplConvKernel* se, int iterations)
{
	assert(src != NULL && dst != NULL && src != dst);
	cvDilate( src, dst, se, iterations );
	cvSub(dst, src, dst );
}

//��̬ѧ�����ݶ�
void Morphology::lhMorpGradientDir(const IplImage* src, IplImage* dst, unsigned int angle, unsigned int len )
{
	assert(src != NULL && dst != NULL && src != dst);
	IplConvKernel* se = lhStructuringElementLine(angle, len);
	lhMorpGradient(src, dst, se);
	cvReleaseStructuringElement(&se);
}

//��̬ѧ�׶�ñ����
void Morphology::lhMorpWhiteTopHat(const IplImage* src, IplImage* dst, IplConvKernel* se, int iterations)
{
	assert(src != NULL && dst != NULL && src != dst);
	lhMorpOpen(src, dst, se, iterations );
	cvSub( src, dst, dst );
}


//��̬ѧ�ڶ�ñ����
void Morphology::lhMorpBlackTopHat(const IplImage* src, IplImage* dst, IplConvKernel* se, int iterations)
{
	assert(src != NULL && dst != NULL && src != dst);
	lhMorpClose(src, dst, se, iterations );
	cvSub(dst, src, dst );

}

//��̬ѧ�Բ���ñ����
void Morphology::lhMorpQTopHat(const IplImage* src, IplImage* dst, IplConvKernel* se, int iterations)
{
	assert(src != NULL && dst != NULL && src != dst);
	IplImage*  temp = cvCloneImage(src);
	lhMorpClose( src, temp, se, iterations );
	lhMorpOpen( src, dst, se, iterations );
	cvSub(temp, dst, dst );
	cvReleaseImage(&temp);
}

//��̬ѧ�Աȶ���ǿ����
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

//��̬ѧ��ֵ����-�����б任
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


//��̬ѧ��Լ������-�����б任 ��Զ�ֵ�ͻҶ�ͼ��
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

//��̬ѧԼ������-�����б任 ��Զ�ֵ�ͻҶ�ͼ��
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

//��̬ѧԼ������-�����б任
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

//��̬ѧ����-�����п��任 
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

//��̬ѧϸ������
void Morphology::lhMorpThin(const IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg, int type)
{

	assert(src != NULL && dst != NULL && src != dst && sefg!= NULL && sefg!=sebg);


	lhMorpHMT(src, dst, sefg, NULL, type);
	cvSub(src, dst, dst);

}

//��̬ѧϸ��ƥ������
void Morphology::lhMorpThinFit(const IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg, int type)
{

	assert(src != NULL && dst != NULL && src != dst && sefg!= NULL && sefg!=sebg);

	lhMorpHMTOpen(src, dst, sefg, NULL, type);
	cvSub(src, dst, dst);
}

//��̬ѧ�ֻ�����
void Morphology::lhMorpThick(const IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg, int type)
{

	assert(src != NULL && dst != NULL && src != dst && sefg!= NULL && sefg!=sebg);


	lhMorpHMT(src, dst, sefg, NULL, type);
	cvAdd(src, dst, dst);

}

//��̬ѧ�ֻ���ƥ������
void Morphology::lhMorpThickMiss(const IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg, int type)
{

	assert(src != NULL && dst != NULL && src != dst && sefg!= NULL && sefg!=sebg);

	lhMorpHMTOpen(src, dst, sefg, NULL, type);
	cvAdd(src, dst, dst);
}


//�Ƚ�����ͼ���Ƿ���ͬ�� 0 ��ͬ
int  Morphology::lhImageCmp(const IplImage* img1, const IplImage* img2)
{
	assert(img1->width == img2->width && img1->height == img2->height && img1->imageSize == img2->imageSize );
	return memcmp(img1->imageData, img2->imageData, img1->imageSize);
}

//��̬ѧ������ͺ������ؽ�����
void Morphology::lhMorpRDilate(const IplImage* src, const IplImage* msk, IplImage* dst, IplConvKernel* se, int iterations)
{

	assert(src != NULL && msk != NULL && dst != NULL && src != dst );

	if(iterations < 0)
	{
		//�����ؽ�
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

		//��ͨ������� p136(6.1)
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

//��̬ѧ��ظ�ʴ�͸�ʴ�ؽ�����
void Morphology::lhMorpRErode(const IplImage* src,  const IplImage* msk, IplImage* dst, IplConvKernel* se, int iterations)
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

//��̬ѧ���ؽ�
void Morphology::lhMorpROpen(const IplImage* src, IplImage* dst, IplConvKernel* se, int iterations)
{
	assert(src != NULL  && dst != NULL && src != dst );

	//p155(6.16)
	IplImage*  temp = cvCreateImage(cvGetSize(src), 8, 1);
	cvErode(src, temp, se, iterations);
	lhMorpRDilate(temp, src, dst, se, -1);
	cvReleaseImage(&temp);

}

//��̬ѧ���ؽ�
void Morphology::lhMorpRClose(const IplImage* src, IplImage* dst, IplConvKernel* se, int iterations)
{
	assert(src != NULL  && dst != NULL && src != dst );

	//p155(6.17)
	IplImage*  temp = cvCreateImage(cvGetSize(src), 8, 1);
	cvDilate(src, temp, se, iterations);
	lhMorpRErode(temp, src, dst, se, -1);
	cvReleaseImage(&temp);

}

//��̬ѧ��ñ�ؽ�
void Morphology::lhMorpRWTH(const IplImage* src, IplImage* dst, IplConvKernel* se, int iterations)
{
	assert(src != NULL  && dst != NULL && src != dst );
	//p156
	lhMorpROpen(src, dst, se, iterations);
	cvSub(src, dst, dst);
}

//��̬ѧ��ñ�ؽ�
void Morphology::lhMorpRBTH(const IplImage* src, IplImage* dst, IplConvKernel* se, int iterations)
{
	assert(src != NULL  && dst != NULL && src != dst );
	//p156
	lhMorpRClose(src, dst, se, iterations);
	cvSub(dst, src, dst);
}


//��̬ѧ����Զ�ż���Զ�ż�ؽ�����
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



//��̬ѧ����Сֵ
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

//��̬ѧ���򼫴�ֵ
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

//��̬ѧH����ֵ
void Morphology::lhMorpHMax(const IplImage* src, IplImage* dst, unsigned char h, IplConvKernel* se)
{
	assert(src != NULL  &&  dst != NULL && src != dst );

	//p150
	IplImage*  temp = cvCreateImage(cvGetSize(src), 8, 1);

	cvSubS(src, cvScalar(h), temp);

	lhMorpRDilate(temp, src, dst, se);

	cvReleaseImage(&temp);

}


//��̬ѧH��Сֵ
void Morphology::lhMorpHMin(const IplImage* src, IplImage* dst, unsigned char h, IplConvKernel* se)
{
	assert(src != NULL  &&  dst != NULL && src != dst );

	//p150
	IplImage*  temp = cvCreateImage(cvGetSize(src), 8, 1);

	cvAddS(src, cvScalar(h), temp);

	lhMorpRErode(temp, src, dst, se);

	cvReleaseImage(&temp);

}

//��̬ѧH���任
void Morphology::lhMorpHConcave(const IplImage* src, IplImage* dst, unsigned char h, IplConvKernel* se)
{
	assert(src != NULL  &&  dst != NULL && src != dst );

	//p150

	lhMorpHMin(src, dst, h, se);
	cvSub(dst, src, dst);

}

//��̬ѧH͹�任
void Morphology::lhMorpHConvex(const IplImage* src, IplImage* dst, unsigned char h, IplConvKernel* se )
{
	assert(src != NULL  &&  dst != NULL && src != dst );

	//p150

	lhMorpHMax(src, dst, h, se);
	cvSub(src, dst, dst);

}

//��̬ѧ��չ����ֵ
void Morphology::lhMorpEMax(const IplImage* src, IplImage* dst, unsigned char h, IplConvKernel* se )
{
	assert(src != NULL  &&  dst != NULL && src != dst );

	//p150
	IplImage*  temp = cvCreateImage(cvGetSize(src), 8, 1);

	lhMorpHMax(src, temp, h, se);
	lhMorpRMax(temp, dst, se);

	cvReleaseImage(&temp);

}

//��̬ѧ��չ��Сֵ
void Morphology::lhMorpEMin(const IplImage* src, IplImage* dst, unsigned char h, IplConvKernel* se)
{
	assert(src != NULL  &&  dst != NULL && src != dst );

	//p150
	IplImage*  temp = cvCreateImage(cvGetSize(src), 8, 1);

	lhMorpHMin(src, temp, h, se);
	lhMorpRMin(temp, dst, se);

	cvReleaseImage(&temp);

}


//��̬ѧ�ȼ��˲�������ֵ,Ĭ��SEΪ����3*3��
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

//��̬ѧ�ؽ�Ӧ��1��ȥ���߽����ͨ����
void Morphology::lhMorpRemoveBoderObj(const IplImage* src, IplImage* dst)
{
	IplImage *temp = cvCloneImage(src);
	//double min, max;
	//cvMinMaxLoc(src, &min, &max);

	//���ͼ��
	cvRectangle(temp, cvPoint(3,3), cvPoint(temp->width-7, temp->height-7), CV_RGB(0,0,0), -1);

	//��ԭͼ����Ϊ��ģͼ��
	lhMorpRDilate(temp, src, dst);

	cvReleaseImage(&temp);
	//cvSet((CvArr*)src, cvScalar(min), dst);
	//cvCopy(src, dst);
	cvSub(src, dst, dst);
}


//��̬ѧ�ؽ�Ӧ��2���ն������
void Morphology::lhMorpFillHole(const IplImage* src, IplImage* dst)
{
	IplImage *temp = cvCloneImage(src);
	double min, max;
	cvMinMaxLoc(src, &min, &max);
	//���ͼ��
	cvRectangle(temp, cvPoint(3,3), cvPoint(temp->width-7, temp->height-7), CV_RGB(max,max,max), -1);

	//��ԭͼ����Ϊ��ģͼ��
	lhMorpRErode(temp, src, dst);

	cvReleaseImage(&temp);
	//cvSub(src, dst, dst);
}

//��̬ѧ��Сֵǿ��
void Morphology::lhMorpMinimaImpose(const IplImage* src, IplImage* markers,  IplImage* dst, IplConvKernel* se)
{
	assert(src != NULL  && markers != NULL && dst != NULL && src != dst );

	IplImage*  temp = cvCreateImage(cvGetSize(src), 8, 1);

	cvAddS(src, cvScalar(1), temp);

	cvMin(temp , markers, temp);

	lhMorpRErode(markers, temp, dst, se);
}

//��̬ѧ����ֵǿ��
void Morphology::lhMorpMaximaImpose(const IplImage* src, IplImage* markers,  IplImage* dst, IplConvKernel* se)
{
	assert(src != NULL  && markers != NULL && dst != NULL && src != dst );

	IplImage*  temp = cvCreateImage(cvGetSize(src), 8, 1);

	cvSubS(src, cvScalar(1), temp);

	cvMax(temp , markers, temp);

	lhMorpRDilate(markers, temp, dst, se);

}


