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
// CDlgKwsModelPath.cpp : implementation file
// --------------------------------------------------

#include "stdafx.h"
#include "ginsidemfc-test.h"
#include "CDlgKwsModelPath.h"
#include "afxdialogex.h"


// CDlgKwsModelPath dialog

IMPLEMENT_DYNAMIC(CDlgKwsModelPath, CDialogEx)

CDlgKwsModelPath::CDlgKwsModelPath(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_SET_MODEL_PATH, pParent)
	, m_strModelPath(_T(""))
{

}

CDlgKwsModelPath::~CDlgKwsModelPath()
{
}

void CDlgKwsModelPath::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MODEL_PATH, m_strModelPath);
}


BEGIN_MESSAGE_MAP(CDlgKwsModelPath, CDialogEx)
END_MESSAGE_MAP()


// CDlgKwsModelPath message handlers
void CDlgKwsModelPath::SetModelPath(const char *pPath)
{
	m_strModelPath = pPath;
}