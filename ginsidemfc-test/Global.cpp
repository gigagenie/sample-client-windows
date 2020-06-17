/*
 * Copyright 2020 KT AI Lab.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// --------------------------------------------------
// HanCmd.cpp : Defines the class behaviors for the application.
// --------------------------------------------------

#include "stdafx.h"
#include <direct.h>
#include <stdlib.h>
#include <stdio.h>
#include <initguid.h>
#include <MMSystem.h>


// CG: The following line was added by the Windows Multimedia component.
#pragma comment(lib, "winmm.lib")
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString	gstrPath = L"./" ;
CString	gstrCmdFile = L"hancmd" ;
CString	gstrCmdPath = L"./" ;

float gfRejection = (float)-0.8 ;
float gfMicVol = (float)30000. ;
float gfMasterVol = (float)30000. ;
float gfEnergy = (float)5. ;
int gnMicMin, gnMicMax ;
int gnMasterMin, gnMasterMax ;
BOOL gbRejection = TRUE ;
BOOL gbContinue = FALSE ;
BOOL gbLogging = FALSE ;
BOOL gbSetConf = FALSE ;
BOOL gbTesting = FALSE ;
//CRecognitionThread *gpRecognitionThread = NULL ;

#define		JS_MIXER_LINE_NUM		4	// 믹서 라인의 최대 갯수
#define		JS_MIXER_WAVEIN_LINE	0	// 믹서 wave-in 라인
#define		JS_MIXER_MIC_LINE		1	// 믹서 microphone 라인
#define		JS_MIXER_WAVEOUT_LINE	2	// 믹서 wave-out 라인
#define		JS_MIXER_SPEAKER_LINE	3	// 믹서 speaker 라인


static HMIXER	g_hMixer = NULL;					// 믹서 핸들
static UINT		g_uMixerID = 0;						// 믹서 ID
static DWORD	g_dwMixerLineID[JS_MIXER_LINE_NUM];	// 라인 ID

extern "C" void CALLBACK waveInProc(HWAVEIN hwi,UINT uMsg,DWORD dwInstance,DWORD dwParam1,DWORD dwParam2)
{
	static int i ;
	if (uMsg == WIM_DATA)
	{
//		gpRecognitionThread->WimData(dwParam1, dwParam2) ;
	}
	else if (uMsg == WIM_OPEN)
	{
		i = 1;
	}
	else if (uMsg == WIM_CLOSE)
	{
		i = 0;
	}
}

extern "C" void CALLBACK waveOutProc(HWAVEIN hwi,UINT uMsg,DWORD dwInstance,DWORD dwParam1,DWORD dwParam2)
{
	static int i;
	if (uMsg == WIM_DATA)
	{
//		gpRecognitionThread->WimData(dwParam1, dwParam2) ;
	}
	else if (uMsg == WIM_OPEN)
	{
		i = 1;
	}
	else if (uMsg == WIM_CLOSE)
	{
		i = 0;
	}
}

int LoadMixerValue()
{
	FILE *fp;
	char szTemp[2000] ;
	CString str ;

	sprintf(szTemp, "%sConf.txt", gstrPath);
	fp = fopen(szTemp, "r+t");

	if (fp == NULL) return -1;

	while (fp && fgets(szTemp, 100, fp) != NULL)
	{
		str = szTemp ;
		str.MakeUpper() ;
		if (str.Find(L"REJECTION") >= 0)
		{
			str = str.Mid(str.Find(L"[") + 1);
			gfRejection = (float)_wtoi(str.GetBuffer(100));
		}
		else if (str.Find(L"MICVOL") >= 0)
		{
			str = str.Mid(str.Find(L"[") + 1);
			gfMicVol = (float)_wtoi(str.GetBuffer(100));
		}
		else if (str.Find(L"MASTERVOL") >= 0)
		{
			str = str.Mid(str.Find(L"[") + 1);
			gfMasterVol = (float)_wtoi(str.GetBuffer(100));
		}
		else if (str.Find(L"ENERGY") >= 0)
		{
			str = str.Mid(str.Find(L"[") + 1);
			gfEnergy = (float)_wtoi(str.GetBuffer(100));
		}
		else if (str.Find(L"RJT") >= 0)
		{
			str = str.Mid(str.Find(L"[") + 1);
			gbRejection = _wtoi(str.GetBuffer(100));
		}
		else if (str.Find(L"CNT") >= 0)
		{
			str = str.Mid(str.Find(L"[") + 1);
			gbContinue = _wtoi(str.GetBuffer(100));
		}
		else if (str.Find(L"SETCONF") >= 0)
		{
			str = str.Mid(str.Find(L"[") + 1);
			gbSetConf = _wtoi(str.GetBuffer(100));
		}
		else if (str.Find(L"LOG") >= 0)
		{
			str = str.Mid(str.Find(L"[") + 1);
			gbLogging = _wtoi(str.GetBuffer(100));
			if (gbLogging)
			{
				_mkdir("PCMLOG");
			}
		}
	}
	if (fp) fclose(fp);

	return 0 ;
}

int SaveMixerValue()
{
	FILE *fp;
	char szTemp[2000] ;

	sprintf(szTemp, "%sConf.txt", gstrPath);
	fp = fopen(szTemp, "w+t");

	if ( fp == NULL ) return -1 ;

	fprintf(fp, "REJECTION=[%f]\n",gfRejection);
	fprintf(fp, "MICVOL=[%f]\n",gfMicVol);
	fprintf(fp, "MASTERVOL=[%f]\n",gfMasterVol);
	fprintf(fp, "ENERGY=[%f]\n",gfEnergy);
	fprintf(fp, "RJT=[%d]\n",gbRejection);
	fprintf(fp, "CNT=[%d]\n",gbContinue);
	fprintf(fp, "LOG=[%d]\n",gbLogging);
	fprintf(fp, "SETCONF=[%d]\n",gbSetConf);

	fclose(fp);

	return 0 ;
}

//
//	함수이름:	GetMasterInputVolume
//	함수기능:	오디오 입력 레벨 값을 얻는다
//	함수인자:	int &	nMinValue	- 최소 peak 값
//				int &	nMaxValue	- 최대 peak 값	
//	리턴값	:	입력 레벨 값 
//
DWORD GetMasterInputVolume(int & nMinValue, int & nMaxValue)
{
    MMRESULT			rc;				// Return code.
    MIXERCONTROL		mxCtrl;			// Holds the mixer control data.
    MIXERLINECONTROLS	mxLineCtrl;		// Obtains the mixer control.

    // Get the control.
    ZeroMemory(&mxLineCtrl, sizeof(mxLineCtrl));
    mxLineCtrl.cbStruct = sizeof(mxLineCtrl);
    mxLineCtrl.dwLineID = g_dwMixerLineID[JS_MIXER_WAVEIN_LINE];
    mxLineCtrl.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
    mxLineCtrl.cControls = 1;
    mxLineCtrl.cbmxctrl = sizeof(mxCtrl);
    mxLineCtrl.pamxctrl = &mxCtrl;
    ZeroMemory(&mxCtrl, sizeof(mxCtrl));
    mxCtrl.cbStruct = sizeof(mxCtrl);
    rc = mixerGetLineControls((HMIXEROBJ)g_hMixer,&mxLineCtrl, MIXER_GETLINECONTROLSF_ONEBYTYPE);
    if (MMSYSERR_NOERROR != rc)
	{
		//::MessageBox(NULL, L"Couldn't get the control.", L"Error", MB_OK);
		return 0;
    }

    // After successfully getting the peakmeter control, the volume range
    // will be specified by mxCtrl.Bounds.lMinimum to mxCtrl.Bounds.lMaximum.
    MIXERCONTROLDETAILS mxcd;				// Gets the control values.
    MIXERCONTROLDETAILS_UNSIGNED volStruct; // Gets the control values.

    // Initialize the MIXERCONTROLDETAILS structure
    ZeroMemory(&mxcd, sizeof(mxcd));
    mxcd.cbStruct = sizeof(mxcd);
    mxcd.cbDetails = sizeof(volStruct);
    mxcd.dwControlID = mxCtrl.dwControlID;
    mxcd.paDetails = &volStruct;
    mxcd.cChannels = 2;

    // Get the current value of the volume control. 
    rc = mixerGetControlDetails((HMIXEROBJ)g_hMixer, &mxcd, MIXER_GETCONTROLDETAILSF_VALUE);
    if (MMSYSERR_NOERROR != rc)
	{
		//::MessageBox(NULL, L"Couldn't get the current volume.", L"Error", MB_OK);
		return 0;
    }

	// Get the minimum & maximum value
	nMinValue = (int)mxCtrl.Bounds.lMinimum;
	nMaxValue = (int)mxCtrl.Bounds.lMaximum;

	return volStruct.dwValue;
}

//
//	함수이름:	SetMasterInputVolume
//	함수기능:	오디오 입력 레벨 값을 지정한다
//	함수인자:	DWORD	dwVolume	- 입력 레벨 값 
//	리턴값	:	TRUE,	성공하면
//				FALSE,	실패하면
//
BOOL SetMasterInputVolume(DWORD dwVolume)
{
    MMRESULT			rc;				// Return code.
    MIXERCONTROL		mxCtrl;         // Holds the mixer control data.
    MIXERLINECONTROLS	mxLineCtrl;		// Obtains the mixer control.

    // Get the control.
    ZeroMemory(&mxLineCtrl, sizeof(mxLineCtrl));
    mxLineCtrl.cbStruct = sizeof(mxLineCtrl);
    mxLineCtrl.dwLineID = g_dwMixerLineID[JS_MIXER_WAVEIN_LINE];
    mxLineCtrl.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
    mxLineCtrl.cControls = 1;
    mxLineCtrl.cbmxctrl = sizeof(mxCtrl);
    mxLineCtrl.pamxctrl = &mxCtrl;
    ZeroMemory(&mxCtrl, sizeof(mxCtrl));
    mxCtrl.cbStruct = sizeof(mxCtrl);
    rc = mixerGetLineControls((HMIXEROBJ)g_hMixer,&mxLineCtrl, MIXER_GETLINECONTROLSF_ONEBYTYPE);
    if (MMSYSERR_NOERROR != rc)
	{
		//::MessageBox(NULL, L"Couldn't get the control.", L"Error", MB_OK);
		return FALSE;
    }

    // After successfully getting the peakmeter control, the volume range
    // will be specified by mxCtrl.Bounds.lMinimum to mxCtrl.Bounds.lMaximum.

    MIXERCONTROLDETAILS mxcd;				// Gets the control values.
    MIXERCONTROLDETAILS_UNSIGNED volStruct; // Gets the control values.

    // Initialize the MIXERCONTROLDETAILS structure
	volStruct.dwValue = dwVolume;
    ZeroMemory(&mxcd, sizeof(mxcd));
    mxcd.cbStruct = sizeof(mxcd);
    mxcd.cbDetails = sizeof(volStruct);
    mxcd.dwControlID = mxCtrl.dwControlID;
    mxcd.paDetails = &volStruct;
    mxcd.cChannels = 2;

    // Set the current value of the volume control. 
    rc = mixerSetControlDetails((HMIXEROBJ)g_hMixer, &mxcd, MIXER_SETCONTROLDETAILSF_VALUE);
    if (MMSYSERR_NOERROR != rc)
	{
		//::MessageBox(NULL, L"Couldn't set the current volume.", L"Error", MB_OK);
		return FALSE;
    }

	return TRUE;
}

//
//	함수이름:	GetMicrophoneVolume
//	함수기능:	마이크 입력 레벨 값을 얻는다
//	함수인자:	int &	nMinValue	- 최소 peak 값
//				int &	nMaxValue	- 최대 peak 값	
//	리턴값	:	입력 레벨 값 
//
DWORD GetMicrophoneVolume(int & nMinValue, int & nMaxValue)
{
    MMRESULT			rc;             // Return code.
    MIXERCONTROL		mxCtrl;         // Holds the mixer control data.
    MIXERLINECONTROLS	mxLineCtrl;		// Obtains the mixer control.

    // Get the control.
    ZeroMemory(&mxLineCtrl, sizeof(mxLineCtrl));
    mxLineCtrl.cbStruct = sizeof(mxLineCtrl);
    mxLineCtrl.dwLineID = g_dwMixerLineID[JS_MIXER_MIC_LINE];
    mxLineCtrl.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
    mxLineCtrl.cControls = 1;
    mxLineCtrl.cbmxctrl = sizeof(mxCtrl);
    mxLineCtrl.pamxctrl = &mxCtrl;
    ZeroMemory(&mxCtrl, sizeof(mxCtrl));
    mxCtrl.cbStruct = sizeof(mxCtrl);
    rc = mixerGetLineControls((HMIXEROBJ)g_hMixer,&mxLineCtrl, MIXER_GETLINECONTROLSF_ONEBYTYPE);
    if (MMSYSERR_NOERROR != rc)
	{
		//::MessageBox(NULL, L"Couldn't get the control.", L"Error", MB_OK);
		return 0;
    }

    // After successfully getting the peakmeter control, the volume range
    // will be specified by mxCtrl.Bounds.lMinimum to mxCtrl.Bounds.lMaximum.
    MIXERCONTROLDETAILS mxcd;				// Gets the control values.
    MIXERCONTROLDETAILS_UNSIGNED volStruct; // Gets the control values.

    // Initialize the MIXERCONTROLDETAILS structure
    ZeroMemory(&mxcd, sizeof(mxcd));
    mxcd.cbStruct = sizeof(mxcd);
    mxcd.cbDetails = sizeof(volStruct);
    mxcd.dwControlID = mxCtrl.dwControlID;
    mxcd.paDetails = &volStruct;
    mxcd.cChannels = 1;

    // Get the current value of the volume control. 
    rc = mixerGetControlDetails((HMIXEROBJ)g_hMixer, &mxcd, MIXER_GETCONTROLDETAILSF_VALUE);
    if (MMSYSERR_NOERROR != rc)
	{
		//::MessageBox(NULL, L"Couldn't get the current volume.", L"Error", MB_OK);
		return 0;
    }

	// Get the minimum & maximum value
	nMinValue = (int)mxCtrl.Bounds.lMinimum;
	nMaxValue = (int)mxCtrl.Bounds.lMaximum;

	return volStruct.dwValue;
}

//
//	함수이름:	SetMicrophoneVolume
//	함수기능:	마이크 입력 레벨 값을 지정한다
//	함수인자:	DWORD	dwVolume	- 입력 레벨 값 
//	리턴값	:	TRUE,	성공하면
//				FALSE,	실패하면
//
BOOL SetMicrophoneVolume(DWORD dwVolume)
{
    MMRESULT			rc;             // Return code.
    MIXERCONTROL		mxCtrl;         // Holds the mixer control data.
    MIXERLINECONTROLS	mxLineCtrl;		// Obtains the mixer control.

    // Get the control.
    ZeroMemory(&mxLineCtrl, sizeof(mxLineCtrl));
    mxLineCtrl.cbStruct = sizeof(mxLineCtrl);
    mxLineCtrl.dwLineID = g_dwMixerLineID[JS_MIXER_MIC_LINE];
    mxLineCtrl.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
    mxLineCtrl.cControls = 1;
    mxLineCtrl.cbmxctrl = sizeof(mxCtrl);
    mxLineCtrl.pamxctrl = &mxCtrl;
    ZeroMemory(&mxCtrl, sizeof(mxCtrl));
    mxCtrl.cbStruct = sizeof(mxCtrl);
    rc = mixerGetLineControls((HMIXEROBJ)g_hMixer,&mxLineCtrl, MIXER_GETLINECONTROLSF_ONEBYTYPE);
    if (MMSYSERR_NOERROR != rc)
	{
		//::MessageBox(NULL, L"Couldn't get the control.", L"Error", MB_OK);
		return FALSE;
    }

    // After successfully getting the peakmeter control, the volume range
    // will be specified by mxCtrl.Bounds.lMinimum to mxCtrl.Bounds.lMaximum.
    MIXERCONTROLDETAILS mxcd;				// Gets the control values.
    MIXERCONTROLDETAILS_UNSIGNED volStruct; // Gets the control values.

    // Initialize the MIXERCONTROLDETAILS structure
	volStruct.dwValue = dwVolume;
    ZeroMemory(&mxcd, sizeof(mxcd));
    mxcd.cbStruct = sizeof(mxcd);
    mxcd.cbDetails = sizeof(volStruct);
    mxcd.dwControlID = mxCtrl.dwControlID;
    mxcd.paDetails = &volStruct;
	mxcd.cChannels = 1;

    // Set the current value of the volume control. 
    rc = mixerSetControlDetails((HMIXEROBJ)g_hMixer, &mxcd, MIXER_SETCONTROLDETAILSF_VALUE);
    if (MMSYSERR_NOERROR != rc)
	{
		//::MessageBox(NULL, L"Couldn't set the current volume.", L"Error", MB_OK);
		return FALSE;
    }

	return TRUE;
}

//
//	함수이름:	OpenMixer()
//	함수기능:	믹서를 열고 정보를 수집한다.
//	함수인자:	
//	리턴값	:	TRUE,	성공하면
//				FALSE,	실패하면
//
BOOL OpenMixer(UINT uMixerID)
{
	MMRESULT	rc;
	MIXERCAPS	mxCaps;
	MIXERLINE	mxLine;
	
	// 믹서 열기
	g_uMixerID = uMixerID;
    rc = mixerOpen(&g_hMixer, g_uMixerID,0,0,0);
    if (MMSYSERR_NOERROR != rc)
	{
		::MessageBox(NULL, L"Couldn't open the mixer", L"Error", MB_OK);
        return FALSE;
    }

	// Get the mixer capabilities.
	rc = mixerGetDevCaps(g_uMixerID, &mxCaps, sizeof(mxCaps));
    if (MMSYSERR_NOERROR != rc)
	{
		::MessageBox(NULL, L"Couldn't get the mixer capabilities", L"Error", MB_OK);
        return FALSE;
    }

	// Get the line information.
	for (int nDestIndex = 0; nDestIndex < (int)mxCaps.cDestinations; nDestIndex ++)
	{
	    // Initialize MIXERLINE structure.
		ZeroMemory(&mxLine,sizeof(mxLine));
		mxLine.dwDestination = nDestIndex;
		mxLine.cbStruct = sizeof(mxLine);
		
		rc = mixerGetLineInfo((HMIXEROBJ)g_hMixer, &mxLine, MIXER_GETLINEINFOF_DESTINATION);
	    if (MMSYSERR_NOERROR != rc)
		{
			::MessageBox(NULL, L"Couldn't get the mixer line", L"Error", MB_OK);
		    return FALSE;
		}

		// Get the wave-in line information
		if(mxLine.dwComponentType == MIXERLINE_COMPONENTTYPE_DST_WAVEIN)
		{
			g_dwMixerLineID[JS_MIXER_WAVEIN_LINE] = mxLine.dwLineID;

			// Get the microphone line information
			DWORD	cConnections = mxLine.cConnections;
			for (int nSrcIndex = 0; nSrcIndex < (int)cConnections; nSrcIndex ++)
			{
			    // Initialize MIXERLINE structure.
				ZeroMemory(&mxLine,sizeof(mxLine));
				mxLine.dwDestination = nDestIndex;
				mxLine.dwSource = nSrcIndex;
				mxLine.cbStruct = sizeof(mxLine);
		
				rc = mixerGetLineInfo((HMIXEROBJ)g_hMixer, &mxLine, MIXER_GETLINEINFOF_SOURCE);
			    if (MMSYSERR_NOERROR != rc)
				{
					::MessageBox(NULL, L"Couldn't get the mixer line", L"Error", MB_OK);
					return FALSE;
				}

				if (mxLine.dwComponentType == MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE)
				{
					g_dwMixerLineID[JS_MIXER_MIC_LINE] = mxLine.dwLineID;
				}
			}
		}

		// Get the wave-out line information
		else if(mxLine.dwComponentType == MIXERLINE_COMPONENTTYPE_DST_SPEAKERS)
		{
			g_dwMixerLineID[JS_MIXER_SPEAKER_LINE] = mxLine.dwLineID;

			// Get the microphone line information
			DWORD	cConnections = mxLine.cConnections;
			for (int nSrcIndex = 0; nSrcIndex < (int)cConnections; nSrcIndex ++)
			{
			    // Initialize MIXERLINE structure.
				ZeroMemory(&mxLine, sizeof(mxLine));
				mxLine.dwDestination = nDestIndex;
				mxLine.dwSource = nSrcIndex;
				mxLine.cbStruct = sizeof(mxLine);
		
				rc = mixerGetLineInfo((HMIXEROBJ)g_hMixer, &mxLine, MIXER_GETLINEINFOF_SOURCE);
			    if (MMSYSERR_NOERROR != rc)
				{
					::MessageBox(NULL, L"Couldn't get the mixer line", L"Error", MB_OK);
					return FALSE;
				}

				if (mxLine.dwComponentType == MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT)
				{
					g_dwMixerLineID[JS_MIXER_WAVEOUT_LINE] = mxLine.dwLineID;
				}
			}
		}
	}
	GetMicrophoneVolume(gnMicMin, gnMicMax) ;
	return TRUE;
}

float GetValueRange(float min, float max, float range, float pos)
{
	float width = max - min;
	float dpos = pos / range;
	return min + (width * dpos);
}

int GetPosRange(float min, float max, float range, float value)
{
	float width = max - min;
	float dpos = (value-min) / width;
	return (int)(range * dpos);
}
