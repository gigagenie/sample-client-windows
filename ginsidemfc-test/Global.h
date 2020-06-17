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

#ifndef  __GLOBAL_H__
#define  __GLOBAL_H__


#include <MMSystem.h>


#pragma comment(lib, "winmm.lib")

#define WM_RECOG_DETECTED	WM_USER+3487
#define WM_WIMDATA			WM_USER+3488

#define SLIDER_RANGE	1000



#define DRAW_WAVE_ALL		0xffff
#define DRAW_WAVE_REFRESH	0x0001
#define DRAW_WAVE_LINE		0x0002
#define DRAW_WAVE_TEXT		0x0004

extern CString	gstrPath ;
extern CString	gstrCmdFile ;
extern CString	gstrCmdPath ;


extern float	gfRejection;
extern float	gfMicVol;
extern float	gfMasterVol;
extern float	gfEnergy;
extern int		gnMicMin, gnMicMax ;
extern int		gnMasterMin, gnMasterMax ;
extern BOOL		gbRejection ;
extern BOOL		gbContinue ;
extern BOOL		gbLogging ;
extern BOOL		gbSetConf ;
extern BOOL		gbTesting ;
class	CRecognitionThread ;
extern CRecognitionThread *gpRecognitionThread ;

extern "C" void CALLBACK waveInProc(HWAVEIN hwi, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
extern "C" void CALLBACK waveOutProc(HWAVEIN hwi, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);

extern BOOL		OpenMixer(UINT uMixerID);									// 오디오 믹서를 연다.
extern DWORD	GetMasterInputVolume(int & nMinValue, int & nMaxValue);		// 오디오 입력 레벨 값을 얻는다
extern BOOL		SetMasterInputVolume(DWORD dwVolume);						// 오디오 입력 레벨 값을 지정한다
extern DWORD	GetMicrophoneVolume(int & nMinValue, int & nMaxValue);		// 마이크 입력 레벨 값을 얻는다
extern BOOL		SetMicrophoneVolume(DWORD dwVolume);						// 마이크 입력 레벨 값을 지정한다

extern float	GetValueRange(float min,float max,float range,float pos);
extern int		GetPosRange(float min,float max,float range,float pos);

extern int		LoadMixerValue();
extern int		SaveMixerValue();


#endif		// end of __GLOBAL_H__
