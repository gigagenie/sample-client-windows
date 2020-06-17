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

#ifndef  __MICINPUT_H__
#define  __MICINPUT_H__

typedef HRESULT	(*REC_CALLBACK)(BYTE *buffer, long size);

/////////////////////////////////////////////////////////////////////////////
// CMicInput thread
class CAudioCtrl;
class CMicInput
{
protected:
	BYTE *m_RecordData;
	int m_RecordSize;
	int m_SleepTime;
	int m_nBit;
	UINT m_uDeviceID;
	BOOL DetectSilence();
// Attributes
public:
	CMicInput();      
	virtual ~CMicInput();
	
	CAudioCtrl *m_pAudioCtrl;
	REC_CALLBACK m_CallBack;
	BOOL m_bRecord;
	// Operations
public:
	void Record(UINT uDeviceID = WAVE_MAPPER);
	void Stop();
	BOOL RecordProc();
	void SetCallBackFunc(REC_CALLBACK callback);
	void SetMode(int nMono, int nSample, int nBit, int bufferCount, int bufferSize);
	void EnableDetectSilence(BOOL bUse);
protected:
	BOOL m_bDetectSilence;
};



#endif		// end of __MICINPUT_H__
