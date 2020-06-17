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
// CDlgAudioSetting.cpp : implementation file
// --------------------------------------------------

#include "stdafx.h"
#include "ginsidemfc-test.h"
#include "CDlgAudioSetting.h"
#include "afxdialogex.h"
#include "Util.h"

#pragma warning(disable:4996)

// CDlgAudioSetting dialog

IMPLEMENT_DYNAMIC(CDlgAudioSetting, CDialogEx)

CDlgAudioSetting::CDlgAudioSetting(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_AUDIO, pParent)
{

}

CDlgAudioSetting::~CDlgAudioSetting()
{
}

void CDlgAudioSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_MIC, m_comboMic);
	DDX_Control(pDX, IDC_COMBO_SPEAKER, m_comboSpeaker);
}

BEGIN_MESSAGE_MAP(CDlgAudioSetting, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgAudioSetting::OnBnClickedOk)
END_MESSAGE_MAP()

// CDlgAudioSetting message handlers
void CDlgAudioSetting::SetAudioList(CStringArray &micArray, CStringArray &speakerArray)
{
	m_micArray.Copy(micArray);
	m_speakerArray.Copy(speakerArray);
	m_nMicSelected = -1;
	m_nSpeakerSelected = -1;
}

void CDlgAudioSetting::SetSelectedAudio(int micSelected, int speakerSelected)
{
	m_nMicSelected = micSelected;
	m_nSpeakerSelected = speakerSelected;
}

void CDlgAudioSetting::WriteAudioSetting()
{
	FILE *fp;
	char	buf[1024];

	fp = fopen(AUDIO_SETTING_FILE, "wt");
	if (fp != NULL)
	{
		ConvertWideStringToString(m_strMic, buf, sizeof(buf));
		fprintf(fp, "%d=%s\n", m_comboMic.GetCurSel(), buf);
		ConvertWideStringToString(m_strSpeaker, buf, sizeof(buf));
		fprintf(fp, "%d=%s\n", m_comboSpeaker.GetCurSel(), buf);
		fclose(fp);
	}
}

BOOL CDlgAudioSetting::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	int i;
	for (i = 0; i < m_micArray.GetSize(); i++)
	{
		m_comboMic.AddString(m_micArray.GetAt(i).GetString());
	}
	for (i = 0; i < m_speakerArray.GetSize(); i++)
	{
		m_comboSpeaker.AddString(m_speakerArray.GetAt(i).GetString());
	}
	if (m_nMicSelected != -1)
	{
		m_comboMic.SetCurSel(m_nMicSelected);
	}
	if (m_nSpeakerSelected != -1)
	{
		m_comboSpeaker.SetCurSel(m_nSpeakerSelected);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgAudioSetting::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	m_nSpeakerSelected = m_comboSpeaker.GetCurSel();
	m_nMicSelected = m_comboMic.GetCurSel();

	m_comboMic.GetLBText(m_nMicSelected, m_strMic);
	m_comboSpeaker.GetLBText(m_nSpeakerSelected, m_strSpeaker);

	WriteAudioSetting();

	CDialogEx::OnOK();
}
