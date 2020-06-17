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
// MicInput.cpp : implementation file
// --------------------------------------------------

#include "stdafx.h"
#include "mmsystem.h"
#include "MicInput.h"
#include "AudioCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMicInput

UINT RecordProcThread(LPVOID lpvoid);

CMicInput::CMicInput()
{
	m_RecordData = NULL;

	m_CallBack = NULL;
	m_bRecord = 0;
	m_pAudioCtrl = NULL;
	m_SleepTime = 20;	// 현재 buffer size 20 ms기준
	m_nBit = 8;
	m_bDetectSilence = FALSE;
}

CMicInput::~CMicInput()
{
	Stop();
	if(m_pAudioCtrl != NULL) 
		delete m_pAudioCtrl;
	if(m_RecordData)
		delete m_RecordData;
}

/////////////////////////////////////////////////////////////////////////////
// CMicInput message handlers

void CMicInput::SetMode(int nMono, int nSample, int nBit, int bufferCount, int bufferSize)
{
	m_pAudioCtrl = new CAudioCtrl(CAudioCtrl::RECORD);
	m_pAudioCtrl->SetAllocBuffer(bufferCount, bufferSize); 
	m_SleepTime = (bufferSize * 1000) / (nSample*nMono*nBit / 8);
	m_nBit = nBit;
	m_pAudioCtrl->SetWaveInfo(nMono, nSample, nBit);
	if(m_RecordData!=NULL) delete m_RecordData;
	m_RecordData = new BYTE[m_pAudioCtrl->GetBufSize() + 1];
}

void CMicInput::EnableDetectSilence(BOOL bUse)
{
	m_bDetectSilence = bUse;
}

void CMicInput::SetCallBackFunc(REC_CALLBACK callback)
{
	m_CallBack = callback;
}

BOOL CMicInput::RecordProc()
{
	BOOL check = TRUE;

	//묵음 체크..
	if(m_pAudioCtrl != NULL) 
	{
		m_RecordSize = 0;
		if(m_bRecord == 1)  
		{
			m_pAudioCtrl->SetRecordDeviceId(m_uDeviceID);
			check = m_pAudioCtrl->ReadRecordData((char*) m_RecordData, m_RecordSize);
			if(check == FALSE) 
			{
				return FALSE;	// why does it come here?
			}
		} 
		else 
		{
			return FALSE;
		}
		if(m_RecordSize > 0)
		{
			if(m_bDetectSilence)
			{
				if(DetectSilence() == FALSE) 
				{
					if(m_CallBack != NULL) 
					{
						m_CallBack((BYTE*) m_RecordData, (long) m_RecordSize);
					}
				}
			}
			else
			{
				if(m_CallBack!=NULL) 
				{
					m_CallBack((BYTE*) m_RecordData, (long) m_RecordSize);
				}
			}
		}
	}

	Sleep(0.1);
	return TRUE;
}

void CMicInput::Stop()
{
	m_bRecord = 0;
	Sleep(m_SleepTime); //버퍼 사이즈 하나 만큼 기다려 주기.
	m_pAudioCtrl->RecordStop();
	Sleep(m_SleepTime);//버퍼 사이즈 하나 만큼 기다려 주기.
}

BOOL CMicInput::DetectSilence()
{
//m_RecordData, m_RecordSize
	static int loop = 0;
	static int bigloop = 0;
	static double silenceEnergy = 0;
	static double signalEnergy = 0;
	static double preEnergy[34] = {0};
	static double highEnergy = 0;
	static double lowEnergy = 100000000000;
	static double maxEnergy = 0;
	static double minEnergy = 100000000000;
	static short lowData = 10000;
	static short highData = -10000;
	int i;

	short* data = (short*) m_RecordData;
	double sum = 0;
	for(i = 0; i < m_RecordSize / 2; i ++)
	{
		sum += (double)(abs(data[i]) / 100);

		if(m_nBit == 8)
		{
			if((data[i] & 0x000000FF) < lowData) 
				lowData = (short) (data[i] & 0x000000FF);
			if((data[i] & 0x000000FF) > highData)
				highData = (short) (data[i] & 0x000000FF);
		}

		if(m_nBit == 16)
		{
			if((data[i] & 0x0000FFFF) < lowData) 
				lowData = (short) (data[i] & 0x0000FFFF);
			if((data[i] & 0x0000FFFF) > highData)
				highData = (short) (data[i] & 0x0000FFFF);
		}
	}
	loop ++;
	bigloop ++;
	if(loop > 34) 
	{
		loop = 0;
	}
	int iGap = abs(highData - lowData);
	if(m_nBit == 8)
	{
		if(iGap < 15)
		{
			minEnergy = lowEnergy;
			maxEnergy = maxEnergy * 0.5;
			bigloop = 0;
			lowData = 1000;
			highData = -1000;
			return TRUE;
		}
	}
	if(m_nBit == 16)
	{
		if(iGap < 1000)
		{
			minEnergy = lowEnergy;
			maxEnergy = maxEnergy * 0.5;
			bigloop = 0;
			lowData = 1000;
			highData = -1000;
			return TRUE;
		}
	}

	if(bigloop > 120) 
	{
		minEnergy = lowEnergy;
		maxEnergy = maxEnergy * 0.5;
		bigloop = 0;
		lowData = 1000;
		highData = -1000;
	}

	preEnergy[loop] = sum;

	for(i = 0; i < 34; i ++)
	{
		if(highEnergy < preEnergy[i]) 
			highEnergy = preEnergy[i];
		if(lowEnergy > preEnergy[i]) 
			lowEnergy = preEnergy[i];
	}
	if(minEnergy > lowEnergy) 
		minEnergy = lowEnergy;
	if(maxEnergy < highEnergy) 
		maxEnergy = highEnergy;

	if(sum < (((lowEnergy + minEnergy) / 2) + ((maxEnergy + minEnergy) / 2)) / 2) 
		return TRUE; 
	return FALSE;
}

void CMicInput::Record(UINT uDeviceID)
{
	m_uDeviceID = uDeviceID;
	m_bRecord = 1;
	AfxBeginThread(RecordProcThread, this);
}

UINT RecordProcThread(LPVOID lpvoid)
{
	CMicInput* mic = (CMicInput*) lpvoid;

	while(mic->m_bRecord == 1)
	{
		if(mic->RecordProc() == FALSE)
		{
			TRACE("I am exiting now!\n");
			break;
		}
	}

	TRACE("Am I here?\n");
	return 0;
}