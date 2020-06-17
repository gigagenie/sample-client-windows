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

#pragma once


// CDlgKeyword dialog

class CDlgKeyword : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgKeyword)

public:
	void setCurrentKeyword(int keyword);
	int getNewKeyword();

private:
	int m_nKeyword;

public:
	CDlgKeyword(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDlgKeyword();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SET_KEYWORD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRadio1Giga();
	afx_msg void OnBnClickedRadio2Genie();
	afx_msg void OnBnClickedRadio3Friend();
	afx_msg void OnBnClickedRadio4Honey();
};
