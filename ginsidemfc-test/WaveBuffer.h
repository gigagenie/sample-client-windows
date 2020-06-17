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

#ifndef  __WAVEBUFFERH__
#define  __WAVEBUFFERH__


#include <MMSystem.h>

#pragma comment(lib, "winmm.lib")

typedef enum
{
	VOID_BUFFER = 0,
	WAIT_BUFFER,
	USING_BUFFER
} BUFFER_STATE;

class CWaveBuffer 
{
public:
	CWaveBuffer(int Size);
	virtual ~CWaveBuffer();

    DWORD PrepareAlreadyCopyData(HWAVEOUT hWaveOut);
    DWORD Prepare(HWAVEOUT hWaveOut, int & count);
    DWORD Prepare(HWAVEIN hWaveIn);
    DWORD Release();
	BUFFER_STATE GetBufferState();
	void SetBufferState(BUFFER_STATE eState);
	
    void PrepareCommon(int count);

    HWAVEOUT hWaveOut;
    HWAVEIN  hWaveIn;
    WAVEHDR  header;
	int GetBufferSize()
	{
		return m_Size;
	}
	char* GetDataPtr()
	{
		return m_pData;
	}
	int GetValidSize()
	{
		return m_ValidDataSize;
	}
	void CopyToBuffer(void* pData, int& count);
private:
	int m_ValidDataSize;
	int m_Size;
	char *m_pData;
	BUFFER_STATE m_eBufferState;
};

#endif		// end of __WAVEBUFFERH__
