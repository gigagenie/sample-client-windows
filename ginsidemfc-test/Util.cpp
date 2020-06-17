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
// Util.cpp
// --------------------------------------------------

#include  "stdafx.h"
#include  "Util.h"

void ConvertWideStringToString(CString &str, char *pString, int length)
{
	wchar_t* wchar_str;
	char*    char_str;
	int      char_str_len;

	wchar_str = str.GetBuffer(str.GetLength());
	char_str_len = WideCharToMultiByte(CP_ACP, 0, wchar_str, -1, NULL, 0, NULL, NULL);
	char_str = new char[char_str_len];
	WideCharToMultiByte(CP_ACP, 0, wchar_str, -1, char_str, char_str_len, 0, 0);

	if (length >= char_str_len)
		strcpy_s(pString, length, char_str);
	else
		pString = NULL;

	delete[] char_str;
}

void ConvertStringToWideString(char *pString, TCHAR *pTCHAR)
{
	USES_CONVERSION;

	_tcscpy(pTCHAR, A2T(pString));
}
