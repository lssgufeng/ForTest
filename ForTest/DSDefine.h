// --------------------------------------------------------------------------
// All or portions of this software are copyrighted by Dothinkey.
// Copyright 1996-2008 Dothinkey Corporation.  
// Company proprietary.
// --------------------------------------------------------------------------
//******************************************************************************
/**
*  \file           DSCAMAPI.h
*  \brief          Defines for the CM500 DLL application
*  \author         Mike 
*  \version        \$ Revision: 0.1 \$         
*  \arg            first implemetation    
*  \date           2007/12/08 10:52:00     
*/
#ifndef _DSDEFINE_H_
#define _DSDEFINE_H_

//parameter
typedef enum tagDS_RUNMODE
{
		RUNMODE_PLAY=0,
		RUNMODE_PAUSE,
		RUNMODE_STOP,
}DS_RUNMODE;

//parameter
typedef enum tagDS_RESOLUTION
{
		R2048_1536=0,
		R1024_768=1,
		R640_480=2,
		R_ROI=3,
}DS_RESOLUTION;

typedef enum tagDS_CAMERA_STATUS
{
		STATUS_OK = 1,                         //动作成功
		STATUS_INTERNAL_ERROR = 0,             //内部错误
		STATUS_NO_DEVICE_FIND = -1,            //没有发现相机
		STATUS_NOT_ENOUGH_SYSTEM_MEMORY = -2,  //没有足够系统内存  
		STATUS_HW_IO_ERROR = -3,               //硬件IO错误
		STATUS_PARAMETER_INVALID = -4,         //参数无效
		STATUS_PARAMETER_OUT_OF_BOUND = -5,    //参数越界
		STATUS_FILE_CREATE_ERROR = -6,         //创建文件失败
		STATUS_FILE_INVALID = -7,              //文件格式无效
		
}DS_CAMERA_STATUS;

typedef enum tagDS_MIRROR_DIRECTION
{
		MIRROR_DIRECTION_HORIZONTAL = 0,
		MIRROR_DIRECTION_VERTICAL = 1,
}DS_MIRROR_DIRECTION;

typedef enum tagDS_FRAME_SPEED
{
		FRAME_SPEED_NORMAL = 0,
		FRAME_SPEED_HIGH = 1,
}DS_FRAME_SPEED;

typedef enum tagDS_POLARITY { 
		POLARITY_LOW	 = 0,
		POLARITY_HIGH = 1
} DS_POLARITY;

typedef enum tagDS_FILE_TYPE
{
		FILE_JPG = 1,
		FILE_BMP = 2,
		FILE_RAW = 4,
		FILE_PNG = 8,
}DS_FILE_TYPE;

typedef enum tagDS_DATA_TYPE
{
		DATA_TYPE_RAW = 0,
		DATA_TYPE_RGB24 = 1,
}DS_DATA_TYPE;

typedef enum tagDS_SNAP_MODE { 
		SNAP_MODE_CONTINUATION	= 0,
		SNAP_MODE_SOFT_TRIGGER	= 1,
		SNAP_MODE_EXTERNAL_TRIGGER	= 2,//external
} DS_SNAP_MODE;

typedef enum tagDS_LIGHT_FREQUENCY{
		LIGHT_FREQUENCY_50HZ = 0,
		LIGHT_FREQUENCY_60HZ = 1,
}DS_LIGHT_FREQUENCY;

typedef enum tagDS_SUBSAMPLE_MODE{
		SUBSAMPLE_MODE_BIN = 0,
		SUBSAMPLE_MODE_SKIP = 1,
}DS_SUBSAMPLE_MODE;

typedef enum tagDS_COLOR_CH{
		COLOR_CH_G1 = 0,
		COLOR_CH_G2 = 1,
		COLOR_CH_R = 2,
		COLOR_CH_B = 3,
}DS_COLOR_CH;

typedef enum tagDS_PARAMETER_TEAM{
		PARAMETER_TEAM_A = 0,
		PARAMETER_TEAM_B = 1,
		PARAMETER_TEAM_C = 2,
		PARAMETER_TEAM_D = 3,
}DS_PARAMETER_TEAM;

typedef enum tagDS_STROBE_MODE{
	STROBE_MODE_AUTO = 0,
	STROBE_MODE_OFF  = 1,
}DS_STROBE_MODE;

typedef enum tagDS_B2RGB_MODE{
	B2RGB_MODE_LINE	 = 0,
		B2RGB_MODE_HAMILTON = 1,
		B2RGB_MODE_LAROCHE= 2,
}DS_B2RGB_MODE;

typedef enum tagDS_CAMERA_MSG{
	CAMERA_MSG_VIDEO = 0,
}DS_CAMERA_MSG;
typedef int (CALLBACK* DS_SNAP_PROC)(BYTE *pImageBuffer);

#endif