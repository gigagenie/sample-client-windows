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
// WaveBuffer.cpp: implementation of the CWaveBuffer class.
// --------------------------------------------------

#include "stdafx.h"
#include "WaveBuffer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWaveBuffer::CWaveBuffer(int Size)
{
	hWaveOut = NULL;
	hWaveIn = NULL;
	header.dwFlags = WHDR_DONE;
	m_pData = new char[Size+400];
	memset(m_pData, 0, Size);
	m_Size = Size;
	m_eBufferState = VOID_BUFFER;
}

CWaveBuffer::~CWaveBuffer()
{
//	if(Release()==MMSYSERR_NOERROR) 
	Release();
	delete[] m_pData;
}

BUFFER_STATE CWaveBuffer::GetBufferState()
{
	return m_eBufferState;
}

void CWaveBuffer::SetBufferState(BUFFER_STATE eState)
{
	m_eBufferState = eState;
}

void CWaveBuffer::CopyToBuffer(void* pData, int& count)
{
	if (count > GetBufferSize())
		count = GetBufferSize();
	char* pPtr = GetDataPtr();
	memcpy((void*)pPtr, pData, count);
	m_ValidDataSize = count;
}

DWORD CWaveBuffer::PrepareAlreadyCopyData(HWAVEOUT hOut)
{

	PrepareCommon(m_ValidDataSize );
	hWaveOut = hOut;
	return waveOutPrepareHeader(hWaveOut, &header, sizeof(header));
}

DWORD CWaveBuffer::Prepare(HWAVEOUT hOut, int & count)
{
	if (count > GetBufferSize())
		count = GetBufferSize();

	PrepareCommon(count);
	m_ValidDataSize = count;
	hWaveOut = hOut;
	return waveOutPrepareHeader(hWaveOut, &header, sizeof(header));

}
DWORD CWaveBuffer::Prepare(HWAVEIN hIn)
{
	PrepareCommon(GetBufferSize());
	hWaveIn = hIn;
	return waveInPrepareHeader(hWaveIn, &header, sizeof(header));
}

DWORD CWaveBuffer::Release()
{
	DWORD err = MMSYSERR_NOERROR;

	// There seems to be some pathalogical cases where on an Abort() call the buffers
	// still are "in use", even though waveOutReset() was called. So wait until the
	// sound driver has finished with the buffer before releasing it.

	if (hWaveOut != NULL) {
		if ((err = waveOutUnprepareHeader(hWaveOut, &header, sizeof(header))) == WAVERR_STILLPLAYING)
			return err;
		hWaveOut = NULL;
	}

	if (hWaveIn != NULL) {
		if ((err = waveInUnprepareHeader(hWaveIn, &header, sizeof(header))) == WAVERR_STILLPLAYING)
			return err;
		hWaveIn = NULL;
	}

	header.dwFlags |= WHDR_DONE;
	return err;
}

void CWaveBuffer::PrepareCommon(int count)
{
	Release();

	memset(&header, 0, sizeof(header));
	header.lpData = (char *)GetDataPtr();
	header.dwBufferLength = count;
	header.dwUser = (DWORD)this;
}
