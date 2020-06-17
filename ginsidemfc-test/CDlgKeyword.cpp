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
// CDlgKeyword.cpp : implementation file
// --------------------------------------------------

#include "stdafx.h"
#include "ginsidemfc-test.h"
#include "CDlgKeyword.h"
#include "afxdialogex.h"


// CDlgKeyword dialog

IMPLEMENT_DYNAMIC(CDlgKeyword, CDialogEx)

CDlgKeyword::CDlgKeyword(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_SET_KEYWORD, pParent)
{

}

CDlgKeyword::~CDlgKeyword()
{
}

void CDlgKeyword::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgKeyword, CDialogEx)
	ON_BN_CLICKED(IDC_RADIO1_GIGA, &CDlgKeyword::OnBnClickedRadio1Giga)
	ON_BN_CLICKED(IDC_RADIO2_GENIE, &CDlgKeyword::OnBnClickedRadio2Genie)
	ON_BN_CLICKED(IDC_RADIO3_FRIEND, &CDlgKeyword::OnBnClickedRadio3Friend)
	ON_BN_CLICKED(IDC_RADIO4_HONEY, &CDlgKeyword::OnBnClickedRadio4Honey)
END_MESSAGE_MAP()


// CDlgKeyword message handlers


BOOL CDlgKeyword::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here

	if (m_nKeyword == 0)
		((CButton*)GetDlgItem(IDC_RADIO1_GIGA))->SetCheck(TRUE);
	else
		((CButton*)GetDlgItem(IDC_RADIO1_GIGA))->SetCheck(FALSE);
	if (m_nKeyword == 1)
		((CButton*)GetDlgItem(IDC_RADIO2_GENIE))->SetCheck(TRUE);
	else
		((CButton*)GetDlgItem(IDC_RADIO2_GENIE))->SetCheck(FALSE);
	if (m_nKeyword == 2)
		((CButton*)GetDlgItem(IDC_RADIO3_FRIEND))->SetCheck(TRUE);
	else
		((CButton*)GetDlgItem(IDC_RADIO3_FRIEND))->SetCheck(FALSE);
	if (m_nKeyword == 3)
		((CButton*)GetDlgItem(IDC_RADIO4_HONEY))->SetCheck(TRUE);
	else
		((CButton*)GetDlgItem(IDC_RADIO4_HONEY))->SetCheck(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgKeyword::setCurrentKeyword(int keyword)
{
	m_nKeyword = keyword;
}

int CDlgKeyword::getNewKeyword()
{
	return m_nKeyword;
}


void CDlgKeyword::OnBnClickedRadio1Giga()
{
	m_nKeyword = 0;
}


void CDlgKeyword::OnBnClickedRadio2Genie()
{
	m_nKeyword = 1;
}


void CDlgKeyword::OnBnClickedRadio3Friend()
{
	m_nKeyword = 2;
}


void CDlgKeyword::OnBnClickedRadio4Honey()
{
	m_nKeyword = 3;
}
