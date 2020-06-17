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

#ifndef  __MYMUTEX_H__
#define  __MYMUTEX_H__


#if defined(_WIN32)
const int P_MAX_INDEX = 0x7fffffff;
#else
const int P_MAX_INDEX = 0x7fff;
#endif

class  CMySemaphore 
{
public:
	CMySemaphore(HANDLE h);
	CMySemaphore(unsigned initial, unsigned maxCount);

	virtual ~CMySemaphore();
	HANDLE handle;
	void Wait();
	BOOL Wait(const DWORD & timeout);
	virtual void Signal();
	BOOL WillBlock() const;
};

class  CMyMutex: public CMySemaphore
{
public:
	CMyMutex();
	virtual ~CMyMutex();
	virtual void Signal();

};

class  PWaitAndSignal
{
  public:
    /**Create the semaphore wait instance.
       This will wait on the specified semaphore using teh #Wait()# function
       before returning.
      */
   PWaitAndSignal(
      CMySemaphore & sem    /// Semaphore descendent to wait/signal.
    );
    /** Signal the semaphore.
        This will execute the Signal() function on the semaphore that was used
        in the construction of this instance.
     */
    ~PWaitAndSignal();

  protected:
    CMySemaphore & semaphore;
};


#endif	// end of __MYMUTEX_H__
