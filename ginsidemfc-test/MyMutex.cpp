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
// MyMutex.cpp: implementation of the CMyMutex class.
// --------------------------------------------------

#include "stdafx.h"
#include "MyMutex.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMySemaphore::CMySemaphore(HANDLE h)
{
  handle = h;
}

CMySemaphore::CMySemaphore(unsigned initial, unsigned maxCount)
{
  if (initial > maxCount)
    initial = maxCount;
  handle = CreateSemaphore(NULL, initial, maxCount, NULL);
}

CMySemaphore::~CMySemaphore()
{
  ReleaseSemaphore(handle, 1, NULL);
//  if (handle != NULL)
  //  CloseHandle(handle);

}


void CMySemaphore::Wait()
{
  WaitForSingleObject(handle, 3000);
}


BOOL CMySemaphore::Wait(const DWORD & timeout)
{
  DWORD result = WaitForSingleObject(handle, timeout);
  return result != WAIT_TIMEOUT;
}


void CMySemaphore::Signal()
{
  ReleaseSemaphore(handle, 1, NULL);
}


BOOL CMySemaphore::WillBlock() const
{
  CMySemaphore * unconst = (CMySemaphore *)this;

  if (!unconst->Wait(0))
    return TRUE;

  unconst->Signal();
  return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
// PMutex

CMyMutex::CMyMutex()
  : CMySemaphore(::CreateMutex(NULL, FALSE, NULL))
{
}


void CMyMutex::Signal()
{
  ::ReleaseMutex(handle);
}

CMyMutex::~CMyMutex()
{
  ::ReleaseMutex(handle);

//  if (handle != NULL)
  //  CloseHandle(handle);

}


PWaitAndSignal::PWaitAndSignal(CMySemaphore & sem)
  : semaphore(sem) { semaphore.Wait(); }

PWaitAndSignal::~PWaitAndSignal()
  { semaphore.Signal(); }
