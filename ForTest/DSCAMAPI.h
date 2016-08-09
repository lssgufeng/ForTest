// --------------------------------------------------------------------------
// All or portions of this software are copyrighted by Dothinkey.
// Copyright 1996-2008 Dothinkey Corporation.  
// Company proprietary.
// --------------------------------------------------------------------------
//******************************************************************************
/**
 *  \file           DSCAMAPI.h
 *  \brief          Defines the API for the CM300 DLL application
 *  \author         Mike 
 *  \version        \$ Revision: 0.1 \$         
 *  \arg            first implemetation    
 *  \date           2007/12/08 10:52:00     
 */
#ifndef _DSCAMAPI_H_
#define _DACAMAPI_H_
#include "stdafx.h"
#include "DSDefine.h"
// DECLDIR will perform an export for us

#define _DTCALL_ 

#ifdef DLL_EXPORT
	#define DT_API extern "C" __declspec(dllexport) 
#else
//	#define DT_API extern "C" _DTCALL_   __declspec(dllimport) 
#define DT_API extern "C" __declspec(dllimport) 
#endif


/*==============================================================
Name:	CameraGetCameraNum
Desc:  Get the numeber of MultiCamera that connected to the PC.

Param:	*CamAllNum      Camera numeber 
        *pResolution    the resolution of the previous work 
Return: Call returns a STATUS_OK on success,otherwise returns an error code
Note:   
  --------------------------------------------------------------*/

DT_API DS_CAMERA_STATUS _DTCALL_  CameraGetMultiCameraNumber(BYTE *CamAllNum, BYTE *pResolution, BYTE *pCapResolution);

/*==============================================================
Name:	CameraInit
Desc:   Initialize video equipment

Param:   pCallbackFunction  Callback fuction pointer,called by SDK,users can add the image analysis in callback fuction.	
		uiResolution  Resolution index
        hWndDisplay	  Video display control handle(Set it to NULL,when don't need to display an image )
		CamNum        One in MultiCamera
Return: Call returns a STATUS_OK on success,otherwise returns an error code
Note:   
  --------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_   CameraInit(DS_SNAP_PROC pCallbackFunction,
								   IN DS_RESOLUTION uiResolution, 
								   IN HWND hWndDisplay, 
								   BYTE CamNum,
								   HWND MsHWND
								   //CWnd* pParent
								  );
/*==============================================================
Name:	CameraUnInit
Desc:	Anti-initialization equipment
Param:   
Return: Call returns a STATUS_OK on success,otherwise returns an error code
Note:   It must call when exit the program for releasing the memory allocation space.
  --------------------------------------------------------------*/

DT_API DS_CAMERA_STATUS _DTCALL_   CameraUnInit(void);
/*==============================================================
Name:	CameraPlay
Desc:	Open the video stream
Param: 
Return: Call returns a STATUS_OK on success,otherwise returns an error code 
Note:   
  --------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_   CameraPlay();

/*==============================================================
Name:	CameraPlay
Desc:	Stop the video stream
Param:   
Return: Call returns a STATUS_OK on success,otherwise returns an error code
Note:   
  --------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_   CameraStop(void);

/*==============================================================
Name:	CameraSaveCurrentImage
Desc:	Save current image.
Param:   strFileName  file name(include directory path)
        FileType     File type,specific definition refer FILE_TYPE 
Return: Call returns a STATUS_OK on success,otherwise returns an error code    
Note:   The last image will be saved when the video stream stops , .
  --------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_   CameraSaveCurrentImage(LPCTSTR strFileName, BYTE FileType,IN BYTE Quality);
/*==============================================================
Name:	CameraCaptureFile
Desc:	Capture an image to a file, the file format will change according to FileType
Param:   strFileName  file name(include directory path)
        FileType     File type,specific definition refer FILE_TYPE
		Quality      JEPG compression parameters，range：1 - 100，value larger，image quality better，file memory bigger.
		uiCapResolution resolution， refer DS_RESOLUTION definition
Return: Call returns a STATUS_OK on success,otherwise returns an error code  
Note:  
  --------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_   CameraCaptureFile(IN LPCTSTR strFileName, IN BYTE FileType, IN BYTE Quality,
										  IN DS_RESOLUTION uiCapResolution);
/*==============================================================
Name:	CameraGetImageSize
Desc:	Read current image size
Param:   *pWidth image width pointer
		*pHeight image height pointer
Return: Call returns a STATUS_OK on success,otherwise returns an error code
Note:   
--------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_   CameraGetImageSize(int *pWidth, int *pHeight);
/*==============================================================
Name:	CameraSetAeState
Desc:	Set AE mode
Param:   bState - TRUE automatic exposure
               - FALSE manual exposure
		
Return: Call returns a STATUS_OK on success,otherwise returns an error code
Note:   
--------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_   CameraSetAeState(IN BOOL bState);
/*==============================================================
Name:	CameraGetAeState
Desc:	Read AE mode
Param:   *pAeState save state pointer
		
Return: Call returns a STATUS_OK on success,otherwise returns an error code
Note:   
--------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_   CameraGetAeState(BOOL *pAeState);
/*==============================================================
Name:	CameraSetAWBState
Desc:   Set white balance mode
Prame:   bAWBState - TRUE automatic white balance
                  - FALSE mu manual white balance
		
Return: Call returns a STATUS_OK on success,otherwise returns an error code
Note:   
--------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_  CameraSetAWBState(BOOL bAWBState);
/*==============================================================
Name:	CameraGetAWBState
Desc:   Read white balance mode
Param:   *pAWBState save state pointer
		
Return: Call returns a STATUS_OK on success,otherwise returns an error code
Note:   
--------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_  CameraGetAWBState(BOOL *pAWBState);
/*==============================================================
Name:	CameraSetAeTarget
Desc:   Set AE target
Param:   uiAeTarget -AE target
		
Return: Call returns a STATUS_OK on success,otherwise returns an error code
Note:   
--------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_  CameraSetAeTarget(IN BYTE uiAeTarget);
/*==============================================================
Name:	CameraGetAeTarget
Desc:   Read AE target
Param:   *pAeTarget -save AE target pointer
		
Return: Call returns a STATUS_OK on success,otherwise returns an error code
Note:   
--------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_  CameraGetAeTarget(IN OUT BYTE *pAeTarget);
/*==============================================================
Name:	CameraSetExposureTime
Desc:   Set exposure time
Param:   uiExposureTime -exposure time
		
Return: Call returns a STATUS_OK on success,otherwise returns an error code
Note:   
--------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_  CameraSetExposureTime(IN int uiExposureTime);
/*==============================================================
Name:	CameraGetExposureTime
Desc:   Read exposure time
Param:   *pExposureTime -save exposure time pointer
		
Return: Call returns a STATUS_OK on success,otherwise returns an error code
Note:   
--------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_  CameraGetExposureTime(IN int *pExposureTime);
DT_API DS_CAMERA_STATUS _DTCALL_  CameraGetMaxExposureTime(IN USHORT *pMaxExposureTime);
/*==============================================================
Name:	CameraSetAnalogGain
Desc:   Set gain
Param:   usAnalogGain gain
		
Return: Call returns a STATUS_OK on success,otherwise returns an error code
Note:   
--------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_  CameraSetAnalogGain(IN USHORT usAnalogGain);
/*==============================================================
Name:	CameraGetAnalogGain
Desc:   Set gain
Param:   *pAnalogGain save the gain pointer
		
Return: Call returns a STATUS_OK on success,otherwise returns an error code
Note:   
--------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_  CameraGetAnalogGain(IN USHORT *pAnalogGain);
/*==============================================================
Name:	CameraSetGain
Desc:   Set each color channel gain
Param:   RGain red channel gain
		GGain green channel gain
		BGain blue channel gain
		
Return: Call returns a STATUS_OK on success,otherwise returns an error code
Note:   
--------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_  CameraSetGain(IN USHORT RGain, USHORT GGain, USHORT BGain);
/*==============================================================
Name:	CameraGetGain
Desc:   Read each color channel gain 
Param:   *pRGain save red channel gain pointer
		*pGGain save green channel gain pointer
		*pBGain save blue channel gain pointer
		
Return: Call returns a STATUS_OK on success,otherwise returns an error code
Note:   
--------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_  CameraGetGain(IN int *pRGain, int *pGGain, int *pBGain);
/*==============================================================
Name:	CameraSetGamma
Desc:   set GAMMA
Param:   uiGamma 
		
Return: Call returns a STATUS_OK on success,otherwise returns an error code
Note:   
--------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_  CameraSetGamma(IN BYTE uiGamma);
/*==============================================================
Name:	CameraGetGamma
Desc:   Read GAMMA
Param:   *pGamma save the GAMMA pointer
		
Return: Call returns a STATUS_OK on success,otherwise returns an error code
Note:   
--------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_  CameraGetGamma(IN BYTE *pGamma);
/*==============================================================
Name:	CameraSetColorEnhancement
Desc:   Color enhancement
Param:   bEnable - TRUE enable 
                - FALSE 
		
Return: Call returns a STATUS_OK on success,otherwise returns an error code
Note:   
--------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_  CameraSetColorEnhancement(IN BOOL bEnable);
/*==============================================================
Name:	CameraGetColorEnhancement
Desc:   
Param:    
		
Return: Call returns a STATUS_OK on success,otherwise returns an error code
Note:   
--------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_  CameraGetColorEnhancement(IN BOOL *pEnable);
/*==============================================================
Name:	CameraSetSaturation
Desc:   Set saturation
Param:   uiSaturation 
		
Return: Call returns a STATUS_OK on success,otherwise returns an error code
Note:   
--------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_  CameraSetSaturation(IN BYTE uiSaturation);
/*==============================================================
Name:	CameraGetSaturation
Desc:   Read saturation
Param:   *pSaturation save the saturation pointer
		
Return: Call returns a STATUS_OK on success,otherwise returns an error code
Note:   
--------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_  CameraGetSaturation(IN BYTE *pSaturation);
/*==============================================================
Name:	CameraSetContrast
Desc:   Set contrast
Param:   uiContrast 
		
Return: Call returns a STATUS_OK on success,otherwise returns an error code
Note:   
--------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_  CameraSetContrast(IN BYTE uiContrast);
/*==============================================================
Name:	CameraGetContrast
Desc:   Read contrast
Param:   *pContrast 
		
Return: Call returns a STATUS_OK on success,otherwise returns an error code
Note:   
--------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_  CameraGetContrast(IN BYTE *pContrast);
/*==============================================================
Name:	CameraSetMirror
Desc:   Set image mirror
Parem:   uiDir direction specified,refer DS_MIRROR_DIRECTION definition,
        bEnable - TRUE mirror
		        - FALSE cancel mirror
		
Return: Call returns a STATUS_OK on success,otherwise returns an error code
Note:   
--------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_  CameraSetMirror(IN DS_MIRROR_DIRECTION uiDir, IN BOOL bEnable);
/*==============================================================
Name:	CameraGetMirror
Desc:   Set image mirror
Param:   uiDir direction specified,refer DS_MIRROR_DIRECTION definition,
        *bEnable - return information
		        
Return: Call returns a STATUS_OK on success,otherwise returns an error code
Note:   
--------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_  CameraGetMirror(IN DS_MIRROR_DIRECTION uiDir, IN BOOL *bEnable);
/*==============================================================
Name:	CameraSetMonochrome
Desc:   Set monochrome
Param:   bEnable - TRUE monochrome
                - FALSE cancel monochrome
		
Return: Call returns a STATUS_OK on success,otherwise returns an error code
Note:   
--------------------------------------------------------------*/

DT_API DS_CAMERA_STATUS _DTCALL_  CameraSetMonochrome(IN BOOL bEnable);
/*==============================================================
Name:	CameraGetMonochrome
Desc:   Read color settings
Param:   *pEnable save the state pointer
		
Return: Call returns a STATUS_OK on success,otherwise returns an error code
Note:   
--------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_  CameraGetMonochrome(IN BOOL *pEnable);
/*==============================================================
Name:	CameraSetFrameSpeed
Desc:   Set frame speed
Param:   FrameSpeed index,Specific definitions refer DS_FRAME_SPEED
		
Return: Call returns a STATUS_OK on success,otherwise returns an error code
Note:   
--------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_  CameraSetFrameSpeed(IN DS_FRAME_SPEED FrameSpeed);
/*==============================================================
Name:	CameraGetFrameSpeed
Desc:   Set frame speed
Param:   FrameSpeed index,Specific definitions refer DS_FRAME_SPEED
		
Return: Call returns a STATUS_OK on success,otherwise returns an error code
Note:   
--------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_  CameraGetFrameSpeed(IN BYTE *pFrameSpeed);

/*==============================================================
Name:	CameraSetLightFrquency
Desc:   Set light frquency
Param:   
		
Return: Call returns a STATUS_OK on success,otherwise returns an error code
Note:   It is effective in automatic expoture.
--------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_  CameraSetLightFrquency(IN DS_LIGHT_FREQUENCY LightFrequency );
DT_API DS_CAMERA_STATUS _DTCALL_  CameraGetLightFrquency(IN BYTE *pLightFrequency );
/*==============================================================
Name:	CameraSaveParameter
Desc:   Save parameters to team ?
Param:   Team -team
		
Return: Call returns a STATUS_OK on success,otherwise returns an error code
Note:   
--------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_  CameraSaveParameter(IN DS_PARAMETER_TEAM Team);
DT_API DS_CAMERA_STATUS _DTCALL_  CameraReadParameter(IN DS_PARAMETER_TEAM Team);
/*==============================================================
Name:	CameraGetCurrentParameterTeam
Desc:   Read current parameter team
Param:   
		
Return: Call returns a STATUS_OK on success,otherwise returns an error code
Note:   
--------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_  CameraGetCurrentParameterTeam(IN BYTE *pTeam);
/*==============================================================
Name:	CameraSetGpio
Desc:   Configure the input and output of GPIO
Param:   bValue －－bit0 to IO3
               －－bit1 to IO2
		
Return: Call returns a STATUS_OK on success,otherwise returns an error code
Note:   input is 0，output is 1；
        such as：CameraSetGpio(0x01)--configure IO3 as output，IO2 as input.
        
--------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_  CameraSetGpio(IN BYTE Value);
/*==============================================================
Name:	CameraSetGpio
Desc:   Read GPIO
Param:   bValue －－bit0 to IO3
               －－bit1 to IO2
		
Return: Call returns a STATUS_OK on success,otherwise returns an error code
Note:   Configure the appropriate IO port as output before read.
        
--------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_  CameraReadGpio(IN BYTE *pbValue);
/*==============================================================
Name:	CameraSetGpio
Desc:   Write GPIO
Param:   bValue －－bit0 to IO3
               －－bit1 to IO2
		
Return: Call returns a STATUS_OK on success,otherwise returns an error code
Note:   Configure the appropriate IO port as output before read.
        
--------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_  CameraWriteGpio(IN BYTE Value);
/*==============================================================
Name:	CameraGetRowTime
Desc:   Read the line cycle at current
Param:   
		
Return: Call returns a STATUS_OK on success,otherwise returns an error code
Note: Get the current setting line time,units is microseconds(us);
      Can be used to count exposure time.
--------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_  CameraGetRowTime(UINT *pRowTime);

/*==============================================================
Name:	CameraSetROI
Desc:   Set ROI area
Param:   HOff Line offset
        VOff Offset
		Width area width
		Height area height
		
Return: Call returns a STATUS_OK on success,otherwise returns an error code
Note: 
--------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_  CameraSetROI(USHORT HOff, USHORT VOff, USHORT Width, USHORT Height);
DT_API DS_CAMERA_STATUS _DTCALL_  CameraGetROI(USHORT *pHOff, USHORT *pVOff, USHORT *pWidth, USHORT *pHeight);

//20111207 added...
/*==============================================================
/*
Name:	CameraSetWBWindow
Desc:   Set AE caculation window
Param:   HOff Line offset
		 VOff Offset
		 Width  area width
		 Height area height
Return:Call returns a STATUS_OK on success,otherwise returns an error code
Note: Default regional is center,a quarter of full-screen.
*/
DT_API DS_CAMERA_STATUS _DTCALL_  CameraSetAEWindow(USHORT HOff, USHORT VOff, USHORT Width, USHORT Height);
/*==============================================================
Name:	CameraSetWBWindow
Desc:   Set white balance window
Param:   HOff Line offset
        VOff Offset
		Width  area width
		Height area height
		
Return:Call returns a STATUS_OK on success,otherwise returns an error code
Note: Default regional is center,a quarter of full-screen.
Notice：The function is effective when call CameraInit（）.
--------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_  CameraSetWBWindow(USHORT HOff, USHORT VOff, USHORT Width, USHORT Height);
/*==============================================================
Name:	CameraSetDisplayWindow
Desc:   Set display window
Param:   HOff Line offset
        VOff Offset
		Width area width
		Height area height
		
Return: Call returns a STATUS_OK on success,otherwise returns an error code
Note: full resolution；
Notice：The function is effective when call CameraInit（）.
--------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_  CameraSetDisplayWindow(USHORT HOff, USHORT VOff, USHORT Width, USHORT Height);

/*==============================================================
Name:	CameraEnableDeadPixelCorrection
Desc:   initializatize dead pixel correction.
Param:   no

  Return: Call returns a STATUS_OK on success,otherwise returns an error code
  Note:   When need to dead pixel correction,first cover the lens with shade cap,turn off auto-exposure ,set the exposure time and gain to appropriate value.
  And then call the fuction to initialize,the program willautomatically find and record the position of dead pixel.
--------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_  CameraEnableDeadPixelCorrection(void);

/*==============================================================
Name:	CameraSetB2RGBMode
Desc:   Set the transfer mode from BAYER to RGB24.
Param:   Mode --B2RGB_MODE_LAROCHE  base on  Gradient and color interpolation
			--B2RGB_MODE_HAMILTON  Adaptive interpolation
			--B2RGB_MODE_LINE  Linear interpolation
  Return: Call returns a STATUS_OK on success,otherwise returns an error code
  Note:   
--------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_  CameraSetB2RGBMode(DS_B2RGB_MODE Mode);
DT_API DS_CAMERA_STATUS _DTCALL_  CameraGetB2RGBMode(byte *pMode);

/*==============================================================
Name:	CameraWriteSN
Desc:   Write product number.
Param:   

  Return: Call returns a STATUS_OK on success,otherwise returns an error code
  Note: make sure eeprom is writeable
        the SNCnt max value is 32  
--------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_  CameraWriteSN(char *pSN, BYTE SNCnt);
/*==============================================================
Name:	CameraReadSN
Desc:   read product number
Param:   

Return: Call returns a STATUS_OK on success,otherwise returns an error code
  Note:   
--------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_  CameraReadSN(char *pSN, BYTE SNCnt);

/*==============================================================
Name:	CameraLoadDefault
Desc:   load default camera setting
Param:   

Return: Call returns a STATUS_OK on success,otherwise returns an error code
  Note:   
--------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_  CameraLoadDefault(void);

/*==============================================================
Name:	CameraSetVideoMessage
Desc:   .
Param:   

Return: Call returns a STATUS_OK on success,otherwise returns an error code
  Note:   
--------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_  CameraSetVideoMessage(UINT Message);

DT_API DS_CAMERA_STATUS _DTCALL_  CameraReadBN(USHORT *pYear, BYTE *pMonth, BYTE *pDay);

/*==============================================================
Name:	CameraCaptureToBuf
Desc:   .
Param:   

  Return: Call returns a STATUS_OK on success,otherwise returns an error code
  Note:   捕获图像数据到内存
  比如：
  BYTE* pBuf = new BYTE[nImageSize];
  CameraCaptureToBuf（BYTE* pBuf);
  将当前的图像数据拷贝到pBuf的内存中。
  
--------------------------------------------------------------*/
DT_API DS_CAMERA_STATUS _DTCALL_  CameraCaptureToBuf(BYTE *pBuffer, IN DS_DATA_TYPE DataType);

/*==============================================================
Name:	CameraCaptureToBuf
==============================================================*/
DT_API DS_CAMERA_STATUS _DTCALL_  SaveBmpFile(CString sfilename, BYTE *pBuffer, UINT width, UINT height);

/*
==============================================================
Name:	CameraInitDeadPixel
//find dead pixel and init...
=============================================================*/
DT_API DS_CAMERA_STATUS _DTCALL_  CameraInitDeadPixel(void);

/*==============================================================
Name:	CameraISP
just do image isp. transfer from raw to rgb24 and do gamma/contrast/color matrix correction....
=============================================================*/
//pData is raw data buffer pointer.
//return is rgb24 buffer pointer..
//
DT_API BYTE* _DTCALL_  CameraISP(BYTE *pData);


/*
==============================================================
Name:	CameraDisplayRGB24
show the rgb24 to video....
=============================================================*/
//pBmp24 is rgb24 buffer pointer to show the image...
//
DT_API void _DTCALL_  CameraDisplayRGB24(BYTE *pBmp24);

#endif