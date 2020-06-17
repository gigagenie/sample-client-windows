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
// AudioCtrl.cpp: implementation of the CAudioCtrl class.
// --------------------------------------------------

#include "stdafx.h"
#include "AudioCtrl.h"
#include "Global.h"

#include <mmdeviceapi.h>
#include <endpointvolume.h>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


BOOL g_bPlayThreadRun = TRUE;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CAudioCtrl::CAudioCtrl(Direction dir, Proc_Mode Mode, CWnd *pWnd,int nMono, int nSample, int nBit)
{
	m_bufferMutex = new CMyMutex;
	m_pWnd = NULL;
	m_ibufferIndex = 0;
	m_ibufferAddedIndex = 0;
	m_iprebufferIndex = 0;
	m_firstcheck = 0;
	m_iBufferByteOffset = P_MAX_INDEX;
	m_Mode = Mode;
	m_Dir = dir;
	m_BufferNum = AUDIO_BUFFER_NUM;
	m_QSize = QSIZE;
	switch(m_Mode)
	{
	case WINDOW:
		m_pWnd = pWnd;
		break;
	case EVENT:
		m_hEventDone = CreateEvent(NULL, FALSE, FALSE, NULL);
		break;
	case CALLBACK_FUNC:
		break;
	}
	SetWaveInfo(nMono, nSample, nBit);
	wAllocMemory();
	m_hWaveInn = NULL;
	m_hWaveOut = NULL;
	m_uiDeviceID = WAVE_MAPPER;

}

CAudioCtrl::~CAudioCtrl()
{
	if (m_Dir == PLAY)
	{
		g_bPlayThreadRun = FALSE;
		Sleep(30);
	}
	m_bufferMutex->Signal();

	if (m_Dir == RECORD) 
	{
		if (m_hWaveInn != NULL)
		{
			RecordStop();
		}
	} 
	else if (m_Dir == PLAY) 
	{
		if (m_hWaveOut != NULL)
		{
			wCloseDevice();
		}
	}
	wFreeMemory();
	delete m_bufferMutex;
}

void CAudioCtrl::SetWaveInfo(int nChannel, int nSample, int nBit)
{
	int chk, byte;

	if (nBit == 8)
		chk = 1;
	else if (nBit == 16)
		chk = 2;
	else
		chk = 4;
	
	byte = chk * nChannel;

	m_WaveFormat.wf.wFormatTag		= WAVE_FORMAT_PCM;
	m_WaveFormat.wf.nChannels		= nChannel;
	m_WaveFormat.wf.nSamplesPerSec	= nSample;
	m_WaveFormat.wf.nAvgBytesPerSec	= nSample * nChannel * chk;
	m_WaveFormat.wf.nBlockAlign		= byte;
	m_WaveFormat.wBitsPerSample		= nBit;
}

BOOL CAudioCtrl::wAllocMemory()
{
	int i;
	PWaitAndSignal mutex(*m_bufferMutex);
	m_iBufferByteOffset = P_MAX_INDEX;
	m_ibufferIndex = 0;
	m_iprebufferIndex = 0;

	CWaveBuffer *pWavBuffer = NULL;
	m_ppbuffer = new CWaveBuffer * [m_BufferNum];

	for (i = 0 ; i < m_BufferNum ; i ++)
	{
		pWavBuffer = new CWaveBuffer(m_QSize);
		m_ppbuffer[i] = pWavBuffer;
	}
	
	return TRUE;
}

void CAudioCtrl::wFreeMemory()
{
	int i;
	for (i = 0; i < m_BufferNum ; i ++)
	{
		delete m_ppbuffer[i];
		m_ppbuffer[i] = NULL;
	}
	delete [] m_ppbuffer;
}


BOOL CAudioCtrl::wOpenDevice(UINT uDeviceID)
{
	PWaitAndSignal mutex(*m_bufferMutex);
	m_iBufferByteOffset = P_MAX_INDEX;
	m_ibufferIndex = 0;
	m_iprebufferIndex = 0;

	DWORD erroCode;
	if(m_Dir == RECORD) 
	{
TRACE("waveInOpen : uDeviceID = %d\n", uDeviceID);
		switch(m_Mode)
		{
		case WINDOW:
			erroCode = waveInOpen((LPHWAVEIN)&m_hWaveInn, (UINT)uDeviceID, (LPWAVEFORMATEX)&m_WaveFormat.wf, (DWORD_PTR)m_pWnd->m_hWnd, NULL, (DWORD)CALLBACK_WINDOW);
			break;
		case EVENT:
			erroCode = waveInOpen((LPHWAVEIN)&m_hWaveInn, (UINT)uDeviceID, (LPWAVEFORMATEX)&m_WaveFormat.wf, (DWORD_PTR)m_hEventDone, 0, CALLBACK_EVENT);
			break;
		case CALLBACK_FUNC:
			erroCode = waveInOpen((LPHWAVEIN)&m_hWaveInn, (UINT)uDeviceID, (LPWAVEFORMATEX)&m_WaveFormat.wf, (DWORD_PTR)waveInProc, (DWORD_PTR)this, (DWORD)CALLBACK_FUNCTION);
			break;
		}
	}
	else 
	{
		switch(m_Mode)
		{
		case WINDOW:
			erroCode = waveOutOpen(&m_hWaveOut, (UINT)uDeviceID, (LPWAVEFORMATEX)&m_WaveFormat.wf, (DWORD_PTR)m_pWnd->m_hWnd, NULL, (DWORD)CALLBACK_WINDOW);
			break;
		case EVENT:
			erroCode = waveOutOpen(&m_hWaveOut, (UINT)uDeviceID, (LPWAVEFORMATEX)&m_WaveFormat.wf, (DWORD_PTR)m_hEventDone, 0, CALLBACK_EVENT);
			break;
		case CALLBACK_FUNC:
			erroCode = waveOutOpen(&m_hWaveOut, (UINT)uDeviceID, (LPWAVEFORMATEX)&m_WaveFormat.wf, (DWORD_PTR)waveOutProc, (DWORD_PTR)this, (DWORD)CALLBACK_FUNCTION);
			break;
		}
	}

	if (erroCode != MMSYSERR_NOERROR)
	{
		ErrorProcess(erroCode,"wOpenDevice");
		return FALSE;
	}
	else 
		return TRUE;
}


BOOL CAudioCtrl::Record(UINT uDeviceID)
{
	int i;

	PWaitAndSignal mutex(*m_bufferMutex);
	if (m_iBufferByteOffset != P_MAX_INDEX)
		return TRUE;

	if (m_Dir == RECORD)
	{
		if (m_hWaveInn == NULL)
		{
			if (wOpenDevice(uDeviceID) == FALSE)
				return FALSE ;
			m_uiDeviceID = uDeviceID;
		}
		else
		{
			if(m_uiDeviceID != uDeviceID)
			{
				RecordStop();
				if (wOpenDevice(uDeviceID) == FALSE)
					return FALSE ;
				m_uiDeviceID = uDeviceID;
			}
		}
	} 
	else 
	{
		return FALSE;
	}

	DWORD erroCode;
	if (m_hWaveInn == NULL) return 0;
	for (i = 0; i < m_BufferNum; i ++)
	{
		CWaveBuffer* buffer = m_ppbuffer[i];
		buffer->Release();
	}

	for (i = 0; i < m_BufferNum; i ++) 
	{
		CWaveBuffer* buffer = m_ppbuffer[i];
		erroCode = buffer->Prepare(m_hWaveInn);
		if (erroCode != MMSYSERR_NOERROR)
		{
			ErrorProcess(erroCode, "waveInHeader prepare");
			break;
		}
		erroCode = waveInAddBuffer(m_hWaveInn, &buffer->header, sizeof(WAVEHDR));
		if (erroCode != MMSYSERR_NOERROR)
		{
			ErrorProcess(erroCode, "waveInAddbuffer");
			break;
		}
	}
	
	if(wInStart() == TRUE) 
	{
		m_iBufferByteOffset = 0;
		return TRUE;
	}
	m_iBufferByteOffset = P_MAX_INDEX;
	return FALSE;
}

BOOL CAudioCtrl::RecordStop()
{
	DWORD erroCode;

	if (m_hWaveInn == NULL) return 0;
	PWaitAndSignal mutex(*m_bufferMutex);
	int check = 0;
	while((erroCode = waveInStop(m_hWaveInn)) != MMSYSERR_NOERROR)
	{
		Sleep(1);
		check ++;
		if(check > 3000) return FALSE;
	}

	if (erroCode != MMSYSERR_NOERROR)
	{
		ErrorProcess(erroCode, "waveInStop");
	} 

    erroCode = waveInReset(m_hWaveInn);
	if (erroCode != MMSYSERR_NOERROR)
	{
		ErrorProcess(erroCode, "waveInReset");
	}

	return wCloseDevice();
}

BOOL CAudioCtrl::wCloseDevice()
{
	DWORD erroCode;
	if(m_Dir == RECORD) 
	{
		int check = 0;
		if (m_hWaveInn == NULL) return 0;
		while((erroCode = waveInClose(m_hWaveInn)) != MMSYSERR_NOERROR)  
		{

			Sleep(1);
			check ++;
			if(check > 2000) return FALSE;
		}

		if (erroCode != MMSYSERR_NOERROR) 
		{
			ErrorProcess(erroCode,"waveInClose");
			return FALSE;
		} 
		else 
		{
			m_hWaveInn = NULL;
			m_iBufferByteOffset = P_MAX_INDEX;
			return TRUE;
		}
	} 
	else if(m_Dir == PLAY) 
	{
		if(m_hWaveOut == NULL) return 0;
		int check = 0;
		while((erroCode = waveOutClose(m_hWaveOut)) != MMSYSERR_NOERROR)  
		{
			Sleep(5);
			check ++;
			if(check > 300) break;
		}
		if(erroCode != MMSYSERR_NOERROR) 
		{
			ErrorProcess(erroCode, "waveOutClose");
			return FALSE;
		} 
		else 
		{
			m_hWaveOut = NULL;
			return TRUE;
		}
	}

	return FALSE;
}

int CAudioCtrl::GetBufSize()
{
	return (int)m_QSize;
}

BOOL CAudioCtrl::wClearDataBuf()
{
	return TRUE;
}

BOOL CAudioCtrl::wInStart()
{
	DWORD erroCode;
	if (m_hWaveInn == NULL) return FALSE;

	erroCode = waveInStart(m_hWaveInn);

	if (erroCode != MMSYSERR_NOERROR)
	{
		ErrorProcess(erroCode,"waveInStart");
		return FALSE;
	} 
	
	return TRUE;
}

BOOL CAudioCtrl::ErrorProcess(DWORD erroCode, LPCSTR strPostedMessage)
{
	CString str, str1;
	waveInGetErrorText(erroCode, str1.GetBuffer(1024), 1000);
	str1.ReleaseBuffer();

	if (erroCode == MMSYSERR_INVALHANDLE) 
	{
		str = "핸들이 유효하지 않습니다.";
	}
	else if (erroCode == MMSYSERR_NODRIVER) 
	{
		str = "장치 구동기(Device Driver)가 존재하지 않습니다.";
	}
	else if (erroCode == MMSYSERR_NOMEM) 
	{
		str = "음성입력장치에서 메모리를 할당하지 못하였습니다.";
	}
	else if (erroCode == WAVERR_STILLPLAYING) 
	{
		str = "버퍼가 장치 큐에 남아있습니다.";
		return TRUE;
	}
	else if (erroCode == WAVERR_UNPREPARED) 
	{
		str = "버퍼가 준비되질 않았습니다.";
	}
	else if (erroCode == MMSYSERR_ALLOCATED) 
	{
		str = "음성입력장치가 다른 곳에서 사용중 입니다.";
	}
	else if (erroCode == MMSYSERR_BADDEVICEID) 
	{
		str = "음성입력장치 번호가 범위를 벗어났습니다.";
	}
	else if (erroCode == WAVERR_BADFORMAT) 
	{
		str = "지원되지 않는 포맷이 시도되었습니다.";
	}
	else 
	{
		str.Format(_T("음성입력장치 에러(%x:%d)"), erroCode, erroCode) ;
		return TRUE;
	}

	AfxMessageBox(str + " (" + str1 + ") " + strPostedMessage);
	TRACE(str + L"\n");
	TRACE(str1 + L"\n");
	TRACE(strPostedMessage);
	TRACE(L"\n");

	return TRUE ;
}

BOOL CAudioCtrl::WaitForAllRecordBuffersFull()
{
	while (!AreAllRecordBuffersFull()) 
	{
		if (WaitForSingleObject(m_hEventDone, INFINITE) != WAIT_OBJECT_0)
		return FALSE;
	}

	return TRUE;
}

BOOL CAudioCtrl::WaitForRecordBufferFull()
{
	if (m_iBufferByteOffset == P_MAX_INDEX)
		return FALSE;

	while (!isRecordBufferFull()) 
	{
		if (WaitForSingleObject(m_hEventDone, INFINITE) != WAIT_OBJECT_0)
			return FALSE;
	}

	return TRUE;
}

BOOL CAudioCtrl::AreAllRecordBuffersFull()
{
	PWaitAndSignal mutex(*m_bufferMutex);

	if (m_iBufferByteOffset == P_MAX_INDEX)
		return TRUE;

	for (int i = 0; i < m_BufferNum; i ++) 
	{
		if ((m_ppbuffer[i]->header.dwFlags & WHDR_DONE) == 0)
			return FALSE;
	}

	return TRUE;
}

BOOL CAudioCtrl::isRecordBufferFull()
{
	PWaitAndSignal mutex(*m_bufferMutex);
	if (m_iBufferByteOffset == P_MAX_INDEX)
		return TRUE;

    if ((m_ppbuffer[m_ibufferIndex]->header.dwFlags & WHDR_DONE) == 0)
      return FALSE;

	return TRUE;
}

BOOL CAudioCtrl::ReadAllRecordData(char *data, int& size)
{
	int i;

	if (!Record())  // Start the first read, queue all the buffers
		return FALSE;

	if (!WaitForAllRecordBuffersFull())
		return FALSE;
	PWaitAndSignal mutex(*m_bufferMutex);

	int RecordLength = 0;
	for (i = 0; i < m_BufferNum; i ++) 
	{
		CWaveBuffer *buffer = m_ppbuffer[i];
		int sz = buffer->header.dwBytesRecorded;
		if(sz<0) break;
		memcpy(data, (void*) buffer->GetDataPtr(), sz);
		data += sz;
		RecordLength +=sz;
	}
	size = RecordLength;
	DWORD erroCode;
	for (i = 0; i < m_BufferNum; i ++) 
	{
		CWaveBuffer* buffer = m_ppbuffer[i];
		erroCode = buffer->Prepare(m_hWaveInn);
		if (erroCode != MMSYSERR_NOERROR)
		{
			ErrorProcess(erroCode, "waveInHeader prepare");
			break;
		}
		erroCode = waveInAddBuffer(m_hWaveInn, &buffer->header, sizeof(WAVEHDR));
		if (erroCode != MMSYSERR_NOERROR)
		{
			ErrorProcess(erroCode, "waveInAddbuffer");
			break;
		}
	}
	return TRUE;
}

BOOL CAudioCtrl::ReadRecordData(char *data, int& size)
{
	int lastReadCount = 0;
	if (!Record(m_uRecordDeviceId))  // Start the first read, queue all the buffers
		return FALSE;

	if (!WaitForRecordBufferFull())
		return FALSE;
	PWaitAndSignal mutex(*m_bufferMutex);

	int RecordLength = 0;
	CWaveBuffer *buffer = m_ppbuffer[m_ibufferIndex];
	int sz = buffer->header.dwBytesRecorded;
	lastReadCount = sz - m_iBufferByteOffset;
	if(sz < 0) 
	{
		sz = 0;
	}

	if (lastReadCount == 0 || m_iBufferByteOffset == P_MAX_INDEX)
		return FALSE;

	memcpy(data, (void*) (buffer->GetDataPtr() + m_iBufferByteOffset), lastReadCount);
	m_iBufferByteOffset += lastReadCount;

	size = lastReadCount;
	DWORD erroCode;
	if (m_iBufferByteOffset >= buffer->GetBufferSize()) 
	{
		if ((erroCode = buffer->Prepare(m_hWaveInn)) != MMSYSERR_NOERROR)
			return FALSE;
		if ((erroCode = waveInAddBuffer(m_hWaveInn, &buffer->header, sizeof(WAVEHDR))) != MMSYSERR_NOERROR)
			return FALSE;

		m_ibufferIndex = (m_ibufferIndex + 1) % m_BufferNum;
		m_iBufferByteOffset = 0;
	}
	return TRUE;
}

UINT PlayProc(LPVOID pParam)
{
	CAudioCtrl	*pCtrl = (CAudioCtrl*) pParam;
	int erroCode;
	CWaveBuffer * prevbuffer = NULL;
	while(g_bPlayThreadRun)
	{
		CWaveBuffer * buffer = pCtrl->m_ppbuffer[pCtrl->m_ibufferIndex];
		if(buffer->GetBufferState() != WAIT_BUFFER)
		{
			if(buffer->GetBufferState() == USING_BUFFER && (buffer->header.dwFlags&WHDR_DONE) != 0)
				buffer->SetBufferState(VOID_BUFFER);
			Sleep(1);
			pCtrl->m_firstcheck = 0;
			continue;
		}
		if(buffer == NULL) break;
		if(pCtrl->m_firstcheck != 0 || prevbuffer != NULL) 
		{//현재 add 할 버퍼가 플레이 중일 경우
			if(buffer->GetBufferState() == USING_BUFFER && (buffer->header.dwFlags&WHDR_DONE) == 0) 
			{
				if (WaitForSingleObject(pCtrl->m_hEventDone, 5000) != WAIT_OBJECT_0) 
				{
					if(g_bPlayThreadRun == FALSE)
						return 0;
					return 0; // No free buffers
				}
			}
		} 
		else 
		{
			pCtrl->m_firstcheck = 1;
		}
		// Can't write more than a buffer full
		buffer->SetBufferState(USING_BUFFER);
		if ((erroCode = buffer->PrepareAlreadyCopyData(pCtrl->m_hWaveOut)) != MMSYSERR_NOERROR)
			break;
		prevbuffer = buffer;
		if ((erroCode = waveOutWrite(pCtrl->m_hWaveOut, &buffer->header, sizeof(WAVEHDR))) != MMSYSERR_NOERROR)
			break;

		pCtrl->m_ibufferIndex = (pCtrl->m_ibufferIndex + 1) % pCtrl->m_BufferNum;
	
	}

	return 0;
}

BOOL CAudioCtrl::WriteDataPlayThread(void *data, int size, UINT uDeviceID)
{
	int lastWriteCount = 0;
	m_playSize = size;
	if(m_hWaveOut == NULL) 
	{
		if(wOpenDevice(uDeviceID) == FALSE)
		{
			TRACE("wOpenDevice returns a FALSE, %d\n", uDeviceID);
			return FALSE;
		}
		AfxBeginThread(PlayProc, (LPVOID) this);
		m_uiDeviceID = uDeviceID;
	}
	else
	{
		if(m_uiDeviceID != uDeviceID)
		{
			TRACE("wOpenDevice : m_uiDeviceID(%d) != uDeviceID(%d)\n", m_uiDeviceID, uDeviceID);
			PlayStop();
			wCloseDevice();
			if(wOpenDevice(uDeviceID) == FALSE)
				return FALSE;
			AfxBeginThread(PlayProc, (LPVOID) this);
			m_uiDeviceID = uDeviceID;
		}
	}

	BYTE * ptr = new BYTE[size];
	memcpy(ptr, data, size);
	BYTE *orgPtr = ptr;
	m_bufferMutex->Wait();
	BUFFER_STATE eState;
	CWaveBuffer * buffer;
	int iAddTryCount = 0;
	while (m_playSize > 0) 
	{
		if(iAddTryCount > m_BufferNum)
			break;
		buffer = m_ppbuffer[m_ibufferAddedIndex];
		if(buffer == NULL)
			break;
		eState = buffer->GetBufferState();
		if(eState == USING_BUFFER && (buffer->header.dwFlags & WHDR_DONE) == 0 || eState == WAIT_BUFFER)
		{
			m_ibufferAddedIndex = (m_ibufferAddedIndex + 1) % m_BufferNum;
			iAddTryCount++;
			continue;
		}

		int count = (m_playSize>0) ? m_playSize : 0;
		if(count==0)
			break;
		buffer->CopyToBuffer(ptr, count);
		buffer->SetBufferState(WAIT_BUFFER);
		m_ibufferAddedIndex = (m_ibufferAddedIndex + 1) % m_BufferNum;
		lastWriteCount += count;
		m_playSize -= count;
		ptr += count;
	}
	delete[] orgPtr;
	m_bufferMutex->Signal();
	return TRUE;
}



BOOL CAudioCtrl::WriteDataPlay(void *data, int size, UINT uDeviceID)
{
	int lastWriteCount = 0;
	m_playSize = size;
	if (m_hWaveOut == NULL) 
	{
		if(wOpenDevice(uDeviceID) == FALSE) 
			return FALSE;
		m_uiDeviceID = uDeviceID;
	}
	else
	{
		if(m_uiDeviceID != uDeviceID)
		{
			PlayStop();
			wCloseDevice();
			if(wOpenDevice(uDeviceID) == FALSE) 
				return FALSE;
			m_uiDeviceID = uDeviceID;
		}
	}

	if (m_hWaveOut == NULL) 
		return FALSE;

	BYTE * ptr = new BYTE[size];
	memcpy(ptr, data, size);
	BYTE *orgPtr = ptr;
	m_bufferMutex->Wait();
	DWORD erroCode;

	while (m_playSize > 0) 
	{
		CWaveBuffer * buffer = m_ppbuffer[m_ibufferIndex];
		CWaveBuffer * prebuffer = m_ppbuffer[m_iprebufferIndex];
		if(buffer == NULL || prebuffer == NULL) break;
		if(m_firstcheck != 0) 
		{
			while ((buffer->header.dwFlags&WHDR_DONE) == 0) 
			{	// 현재 add 할 버퍼가 플레이 중일 경우 
				// No free buffers, so wait for one
				if (WaitForSingleObject(m_hEventDone, INFINITE) != WAIT_OBJECT_0)
				{
					delete ptr;
					return FALSE; // No free buffers
				}
			}
		} 
		else 
		{
			m_firstcheck = 1;
		}
		// Can't write more than a buffer full
		int count = (m_playSize > 0) ? m_playSize : 0;
		if(count == 0) break;
		if ((erroCode = buffer->Prepare(m_hWaveOut, count)) != MMSYSERR_NOERROR)
			break;
		char *tempptr = buffer->GetDataPtr();
		if(tempptr == NULL) break;
		memcpy(tempptr, ptr, count);

		if ((erroCode = waveOutWrite(m_hWaveOut, &buffer->header, sizeof(WAVEHDR))) != MMSYSERR_NOERROR)
			break;
		m_iprebufferIndex = m_ibufferIndex;

		m_ibufferIndex = (m_ibufferIndex+1)%m_BufferNum;
		lastWriteCount += count;
		m_playSize -= count;
		ptr += count;
	}
	delete[] orgPtr;
	m_bufferMutex->Signal();
	return TRUE;
}

BOOL CAudioCtrl::PlayStop()
{
	DWORD erroCode;
	int check = 0;

	m_playSize = 0;

	if (m_hWaveOut == NULL) return 0;
	PWaitAndSignal mutex(*m_bufferMutex);
	while((erroCode = waveOutPause(m_hWaveOut)) != MMSYSERR_NOERROR) 
	{
		Sleep(1);
		check ++;
		if(check > 3000) return FALSE;
	}

	if (erroCode != MMSYSERR_NOERROR)
	{
		ErrorProcess(erroCode,"waveOutPause");
	} 

    erroCode = waveOutReset(m_hWaveOut);
	if (erroCode != MMSYSERR_NOERROR)
	{
		ErrorProcess(erroCode,"waveOutReset");
	}

	return wCloseDevice();
}

BOOL CAudioCtrl::SetAllocBuffer(int buffernum, int qsize)
{
	if(m_Dir == RECORD)
	{
		if(m_hWaveInn != NULL) 
		{
			RecordStop();
		}
	}
	else if(m_Dir == PLAY) 
	{
		if(m_hWaveOut != NULL) 
		{
			m_bufferMutex->Wait();
			wCloseDevice();
		}
	}

	wFreeMemory();
	m_BufferNum = buffernum;
	m_QSize = qsize;
	wAllocMemory();
	m_iprebufferIndex = 0;
	m_ibufferIndex = 0;
	m_bufferMutex->Signal();
	return TRUE;
}

void CAudioCtrl::SetMicVolume(int level)
{
   // Open the mixer device
	HMIXER hmx;
	mixerOpen(&hmx, 0, 0, 0, 0);

	// Get the line info for the wave in destination line
	MIXERLINE mxl;
	mxl.cbStruct = sizeof(mxl);
	mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
	mixerGetLineInfo((HMIXEROBJ)hmx, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE); 

	// Now find the microphone source line connected to this wave in
	// destination
	DWORD cConnections = mxl.cConnections;
	for(DWORD j = 0; j < cConnections; j ++)
	{
		mxl.dwSource = j;
		mixerGetLineInfo((HMIXEROBJ)hmx, &mxl, MIXER_GETLINEINFOF_SOURCE);
		if (MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE == mxl.dwComponentType) break;
	}
	// Find a volume control, if any, of the microphone line
	LPMIXERCONTROL pmxctrl = (LPMIXERCONTROL)malloc(sizeof MIXERCONTROL);
	MIXERLINECONTROLS mxlctrl = {sizeof mxlctrl, mxl.dwLineID, MIXERCONTROL_CONTROLTYPE_VOLUME, 1, sizeof MIXERCONTROL, pmxctrl};

	if (!mixerGetLineControls((HMIXEROBJ) hmx, &mxlctrl, MIXER_GETLINECONTROLSF_ONEBYTYPE))
	{ 
		// Found!
		DWORD cChannels = mxl.cChannels;
		if (MIXERCONTROL_CONTROLF_UNIFORM & pmxctrl->fdwControl) cChannels = 1;

		LPMIXERCONTROLDETAILS_UNSIGNED pUnsigned = (LPMIXERCONTROLDETAILS_UNSIGNED) malloc(cChannels * sizeof MIXERCONTROLDETAILS_UNSIGNED);
		MIXERCONTROLDETAILS mxcd = {sizeof(mxcd), pmxctrl->dwControlID, cChannels, (HWND)0, sizeof MIXERCONTROLDETAILS_UNSIGNED, (LPVOID) pUnsigned};
		mixerGetControlDetails((HMIXEROBJ)hmx, &mxcd, MIXER_SETCONTROLDETAILSF_VALUE); 
		// Set the volume to the middle  (for both channels as needed)
		pUnsigned[0].dwValue = pUnsigned[cChannels - 1].dwValue = (0xffff / 100) * level;
		mixerSetControlDetails((HMIXEROBJ)hmx, &mxcd, MIXER_SETCONTROLDETAILSF_VALUE); 

		free(pmxctrl);
		free(pUnsigned);
	}
	else
		free(pmxctrl);

	mixerClose(hmx);
}

BOOL CAudioCtrl::SelectMic()
{
	// Open the mixer device
	HMIXER hmx;
	MIXERLINE mxl;
	int iMixerID = 0;
	MMRESULT hResult;
	LPMIXERCONTROLDETAILS_LISTTEXT plisttext;
	MIXERCONTROLDETAILS mxcd;

mixerOpen_pos:
	do
	{
		hResult = mixerOpen(&hmx, iMixerID, 0, 0, 0);
		if (hResult!= MMSYSERR_NOERROR)
		{
			::MessageBox(NULL, L"녹음 설정에 문제가 있습니다.", L"Error", MB_OK);
			return FALSE;
		}
		// Get the line info for the wave in destination line
		memset((void*)&mxl, NULL, sizeof(MIXERLINE));
		mxl.cbStruct = sizeof(mxl);
		mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
		hResult = mixerGetLineInfo((HMIXEROBJ)hmx, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE); 
		if (hResult!= MMSYSERR_NOERROR)
		{
			mixerClose(hmx);
			iMixerID++;
		}
	} while (hResult != MMSYSERR_NOERROR);

	// Find a LIST control, if any, for the wave in line
	LPMIXERCONTROL pmxctrl = (LPMIXERCONTROL)malloc(mxl.cControls * sizeof MIXERCONTROL); 
	MIXERLINECONTROLS mxlctrl = {sizeof mxlctrl, mxl.dwLineID, 0, mxl.cControls, sizeof MIXERCONTROL, pmxctrl};
	mixerGetLineControls((HMIXEROBJ) hmx, &mxlctrl, MIXER_GETLINECONTROLSF_ALL); 

   // Now walk through each control to find a type of LIST control. This
   // can be either Mux, Single-select, Mixer or Multiple-select.
	DWORD i = 0;
	for (i = 0; i < mxl.cControls; i ++)
	{	// Found a LIST control
		// Check if the LIST control is a Mux or Single-select type
		if (pmxctrl[i].cMultipleItems == 0)
			continue;
		BOOL bOneItemOnly = FALSE;
		switch (pmxctrl[i].dwControlType)
		{
			case MIXERCONTROL_CONTROLTYPE_MUX:
			case MIXERCONTROL_CONTROLTYPE_SINGLESELECT:
			bOneItemOnly = TRUE;
		}

		DWORD cChannels = mxl.cChannels, cMultipleItems = 0;
		if (MIXERCONTROL_CONTROLF_UNIFORM & pmxctrl[i].fdwControl)  
			cChannels = 1;
		if (MIXERCONTROL_CONTROLF_MULTIPLE & pmxctrl[i].fdwControl)
			cMultipleItems = pmxctrl[i].cMultipleItems;

		// Get the text description of each item
		plisttext = (LPMIXERCONTROLDETAILS_LISTTEXT) malloc(cChannels * cMultipleItems * sizeof MIXERCONTROLDETAILS_LISTTEXT);
		mxcd = {sizeof(mxcd), pmxctrl[i].dwControlID, cChannels, (HWND)cMultipleItems, sizeof MIXERCONTROLDETAILS_LISTTEXT, (LPVOID) plisttext};
		mixerGetControlDetails((HMIXEROBJ)hmx, &mxcd, MIXER_GETCONTROLDETAILSF_LISTTEXT);

		// Now get the value for each item
		LPMIXERCONTROLDETAILS_BOOLEAN plistbool = (LPMIXERCONTROLDETAILS_BOOLEAN) malloc(cChannels * cMultipleItems * sizeof MIXERCONTROLDETAILS_BOOLEAN);
		mxcd.cbDetails = sizeof MIXERCONTROLDETAILS_BOOLEAN;
		mxcd.paDetails = plistbool;
		mixerGetControlDetails((HMIXEROBJ)hmx, &mxcd, MIXER_GETCONTROLDETAILSF_VALUE); 
		int iCheck = 0;

		// Select the "Microphone" item
		for (DWORD j = 0; j < cMultipleItems; j += cChannels)
		{
			if (0 == wcscmp(plisttext[j].szName, L"Microphone")
				|| 0 == wcscmp(plisttext[j].szName, L"Mic")
				|| 0 == wcscmp(plisttext[j].szName, L"마이크")
				|| 0 == wcscmp(plisttext[j].szName, L"마이크로폰")
				|| 0 == wcscmp(plisttext[j].szName, L"마이크 볼륨")
				|| 0 == wcscmp(plisttext[j].szName, L"Mic Volume"))
			{
				// Select it for both left and right channels
				plistbool[j].fValue = plistbool[j + cChannels - 1].fValue = 1;
				iCheck++;
			}
			else if (bOneItemOnly)
			{
				// Mux or Single-select allows only one item to be selected
				// so clear other items as necessary
				plistbool[j].fValue = plistbool[j + cChannels - 1].fValue = 0;
			}
		}
		if (iCheck == 0)
		{
			free(pmxctrl);
			free(plisttext);
			free(plistbool);
			mixerClose(hmx);
			iMixerID ++;
			goto mixerOpen_pos;
		}
		// Now actually set the new values in
		mixerSetControlDetails((HMIXEROBJ)hmx, &mxcd, MIXER_GETCONTROLDETAILSF_VALUE); 

		free(plisttext);
		free(plistbool);
	}  
	free(pmxctrl);
	mixerClose(hmx);
	return TRUE;
} 

bool CAudioCtrl::GetCurrentVolume(double &curVolume)
{
	HRESULT hr = NULL;
	bool decibels = false;
	bool scalar = false;

	CoInitialize(NULL);
	IMMDeviceEnumerator *deviceEnumerator = NULL;
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER,
		__uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);
	IMMDevice *defaultDevice = NULL;

	hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
	deviceEnumerator->Release();
	deviceEnumerator = NULL;

	IAudioEndpointVolume *endpointVolume = NULL;
	hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume),
		CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume);
	defaultDevice->Release();
	defaultDevice = NULL;

	// -------------------------
	float currentVolume = 0;
	endpointVolume->GetMasterVolumeLevel(&currentVolume);
	//printf("Current volume in dB is: %f\n", currentVolume);

	hr = endpointVolume->GetMasterVolumeLevelScalar(&currentVolume);
	curVolume = currentVolume;
	
	endpointVolume->Release();

	CoUninitialize();

	return FALSE;
}

bool CAudioCtrl::ChangeVolume(double nVolume, bool bScalar)
{
	HRESULT hr = NULL;
	bool decibels = false;
	bool scalar = false;
	double newVolume = nVolume;

	CoInitialize(NULL);
	IMMDeviceEnumerator *deviceEnumerator = NULL;
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER,
		__uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);
	IMMDevice *defaultDevice = NULL;

	hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
	deviceEnumerator->Release();
	deviceEnumerator = NULL;

	IAudioEndpointVolume *endpointVolume = NULL;
	hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume),
		CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume);
	defaultDevice->Release();
	defaultDevice = NULL;

	// -------------------------
	float currentVolume = 0;
	endpointVolume->GetMasterVolumeLevel(&currentVolume);
	//printf("Current volume in dB is: %f\n", currentVolume);

	hr = endpointVolume->GetMasterVolumeLevelScalar(&currentVolume);
	//CString strCur = L"";
	//strCur.Format(L"%f", currentVolume);
	//AfxMessageBox(strCur);

	// printf("Current volume as a scalar is: %f\n", currentVolume);
	if (bScalar == false)
	{
		hr = endpointVolume->SetMasterVolumeLevel((float)newVolume, NULL);
	}
	else if (bScalar == true)
	{
		hr = endpointVolume->SetMasterVolumeLevelScalar((float)newVolume, NULL);
	}
	endpointVolume->Release();

	CoUninitialize();

	return FALSE;
}
