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
// SelectInputDevice.cpp: implementation of the CAudioDeviceProperty class.
// --------------------------------------------------

#include "stdafx.h"
#include "AudioDeviceProperty.h"

#include <MMSystem.h>
#pragma comment(lib, "winmm.lib")

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAudioDeviceProperty::CAudioDeviceProperty()
{

}

CAudioDeviceProperty::~CAudioDeviceProperty()
{

}

BOOL CAudioDeviceProperty::GetDeviceList(CStringArray& micList, CStringArray& spkList)
{
    UINT		u, cMixerDevs;
	UINT		uWaveInGetNumDevs, uWaveOutGetNumDevs;
	WAVEINCAPS	incaps;
	WAVEOUTCAPS	outcaps;

	micList.RemoveAll();
	spkList.RemoveAll();
	cMixerDevs = mixerGetNumDevs();
	uWaveInGetNumDevs = waveInGetNumDevs();
	uWaveOutGetNumDevs = waveOutGetNumDevs();
	TRACE("Total device count : %d\n", cMixerDevs);
	TRACE("Input device count : %d\n", uWaveInGetNumDevs);
	TRACE("Output device count : %d\n", uWaveOutGetNumDevs);

	for (u = 0; u < uWaveInGetNumDevs; u++)
	{
		waveInGetDevCaps(u, &incaps, sizeof(incaps));
		micList.Add(incaps.szPname);
	}
	for (u = 0; u < uWaveOutGetNumDevs; u++)
	{
		waveOutGetDevCaps(u, &outcaps, sizeof(outcaps));
		spkList.Add(outcaps.szPname);
	}

	return TRUE;
}
