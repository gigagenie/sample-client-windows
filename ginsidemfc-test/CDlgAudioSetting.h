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


#define  AUDIO_SETTING_FILE		".\\audio_setting.cfg"


// CDlgAudioSetting dialog

class CDlgAudioSetting : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgAudioSetting)

public:
	CDlgAudioSetting(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDlgAudioSetting();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_AUDIO };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void SetAudioList(CStringArray &micArray, CStringArray &speakerArray);
	void SetSelectedAudio(int micSelected, int speakerSelected);
	void WriteAudioSetting();

	int	m_nMicSelected, m_nSpeakerSelected;
	CString	m_strMic, m_strSpeaker;

	CComboBox m_comboMic;
	CComboBox m_comboSpeaker;
	CStringArray m_micArray, m_speakerArray;
	afx_msg void OnBnClickedOk();
};
