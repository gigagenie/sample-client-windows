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
// CDlgSetServerInfo.cpp : implementation file
// --------------------------------------------------

#include "stdafx.h"
#include "ginsidemfc-test.h"
#include "CDlgSetServerInfo.h"
#include "afxdialogex.h"


// CDlgSetServerInfo dialog

IMPLEMENT_DYNAMIC(CDlgSetServerInfo, CDialogEx)

CDlgSetServerInfo::CDlgSetServerInfo(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_SET_SERVER_INFO, pParent)
	, m_strServer(_T(""))
	, m_strGrpcPort(_T(""))
	, m_strRestPort(_T(""))
{

}

CDlgSetServerInfo::~CDlgSetServerInfo()
{
}

void CDlgSetServerInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SERVER, m_strServer);
	DDX_Text(pDX, IDC_EDIT_GRPC_PORT, m_strGrpcPort);
	DDX_Text(pDX, IDC_EDIT_REST_PORT, m_strRestPort);
}


BEGIN_MESSAGE_MAP(CDlgSetServerInfo, CDialogEx)
END_MESSAGE_MAP()


// CDlgSetServerInfo message handlers
void CDlgSetServerInfo::setServerInfo(CString &strServer, CString &strGrpcPort, CString &strRestPort)
{
	m_strServer = strServer;
	m_strGrpcPort = strGrpcPort;
	m_strRestPort = strRestPort;
}

BOOL CDlgSetServerInfo::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}
