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
// CDlgSetLocation.cpp : implementation file
// --------------------------------------------------

#include "stdafx.h"
#include "ginsidemfc-test.h"
#include "CDlgSetLocation.h"
#include "afxdialogex.h"


// CDlgSetLocation dialog

IMPLEMENT_DYNAMIC(CDlgSetLocation, CDialogEx)

CDlgSetLocation::CDlgSetLocation(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_SET_LOCATION, pParent)
	, m_strLongitude(_T(""))
	, m_strLatitude(_T(""))
	, m_strAddress(_T(""))
{

}

CDlgSetLocation::~CDlgSetLocation()
{
}

void CDlgSetLocation::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_LONGITUDE, m_strLongitude);
	DDX_Text(pDX, IDC_EDIT_LATITUDE, m_strLatitude);
	DDX_Text(pDX, IDC_EDIT_ADDRESS, m_strAddress);
}


BEGIN_MESSAGE_MAP(CDlgSetLocation, CDialogEx)
END_MESSAGE_MAP()


// CDlgSetLocation message handlers
void CDlgSetLocation::setLocation(CString &strLongitude, CString &strLatitude, CString &strAddress)
{
	m_strLongitude = strLongitude;
	m_strLatitude = strLatitude;
	m_strAddress = strAddress;
}


BOOL CDlgSetLocation::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}
