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

#ifndef  __AUDIOCTRL_H__
#define  __AUDIOCTRL_H__

#include "mmsystem.h"

#define  SAMPLING				16000		// Sampliing Rate


#define  MONO					1			// Mono
#define  NBITAUDIO				16			// Bit per sample

/*
#define  QSIZE					24*20 //(SAMPLING/5)		// Unit Buffer Size
#define  FRAME_SIZE				(SAMPLING/50)
#define  AUDIO_BUFFER_NUM		20
*/
/*
#define  QSIZE					24*2*10*4      //(SAMPLING/5)		// Unit Buffer Size
#define  FRAME_SIZE				(SAMPLING/50)
#define  AUDIO_BUFFER_NUM		2
*/

#define  QSIZE					480*4 //(SAMPLING/5)		// Unit Buffer Size
#define  FRAME_SIZE				(SAMPLING/50)
#define  AUDIO_BUFFER_NUM		6

#include "WaveBuffer.h"
#include "mymutex.h"

class  CAudioCtrl  
{
public:
	BOOL PlayStop();
	BOOL SelectMic();
	void SetMicVolume(int level = 50);

	BOOL SetAllocBuffer(int buffernum, int qsize);

	BOOL WriteDataPlayThread(void * data, int size, UINT uDeviceID = WAVE_MAPPER);
	BOOL WriteDataPlay(void * data, int size, UINT uDeviceID = WAVE_MAPPER);
	
	BOOL ReadAllRecordData(char* data, int& size);
	BOOL ReadRecordData(char* data, int& size);
	BOOL AreAllRecordBuffersFull();
	BOOL WaitForAllRecordBuffersFull();
	BOOL isRecordBufferFull();
	BOOL WaitForRecordBufferFull();
	enum Direction {PLAY = 0, RECORD = 1};
	enum Proc_Mode {EVENT = 0, WINDOW = 1, CALLBACK_FUNC = 2};
	BOOL wClearDataBuf();
	int GetBufSize();
	BOOL wCloseDevice();
	void SetWindow(CWnd *pWnd) { m_pWnd = pWnd ; }
	BOOL RecordStop();
	void SetRecordDeviceId(UINT uDeviceID = WAVE_MAPPER) { m_uRecordDeviceId = uDeviceID; }
	BOOL Record(UINT uDeviceID = WAVE_MAPPER);
	BOOL wOpenDevice(UINT uDeviceID = WAVE_MAPPER);
	void wFreeMemory();
	BOOL wAllocMemory();
	void SetWaveInfo(int nChannel, int nSample, int nBit);

	BOOL wInStart();

	CAudioCtrl(Direction dir = PLAY, Proc_Mode Mode = EVENT, CWnd *pWnd = NULL,int nMono = MONO, int nSample = 16000, int nBit = 16);
	//mode 0: event
	//mode 1: 윈도우.
	//모드 2: 콜백함수.
	BOOL ErrorProcess(DWORD erroCode, LPCSTR strPostedMessage);
	virtual ~CAudioCtrl();

//protected:
	int m_firstcheck;
	int m_BufferNum;
	int m_QSize;
	int m_playSize;
	CMyMutex *m_bufferMutex;
	int m_iBufferByteOffset;
	CWnd	*m_pWnd ;
	HANDLE   m_hEventDone;
	Proc_Mode m_Mode;
	Direction m_Dir;
	int	m_nMono ;
	int	m_nBit ;
	int	m_nSampling ;
	int m_ibufferIndex;
	int m_ibufferAddedIndex;
	int m_iprebufferIndex;
	PCMWAVEFORMAT	m_WaveFormat;
	HWAVEIN			m_hWaveInn;
	HWAVEOUT		m_hWaveOut;
	CWaveBuffer** m_ppbuffer;
	UINT			m_uiDeviceID;

	// record device 전용
	UINT			m_uRecordDeviceId;

public:
	bool ChangeVolume(double nVolume, bool bScalar);
	bool GetCurrentVolume(double &curVolume);
};


#endif	// end of __AUDIOCTRL_H__
