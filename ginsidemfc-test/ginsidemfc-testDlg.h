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
// ginsidemfc-testDlg.h : header file
// --------------------------------------------------

#pragma once

#include "AudioCtrl.h"
#include "MicInput.h"


#define  WM_STOP_SOUND (WM_USER+1)
#define  WM_ENABLE_BUTTONS (WM_USER+2)
#define  WM_PROGRESSIVE_BAR (WM_USER+3)


// CginsidemfctestDlg dialog
class CginsidemfctestDlg : public CDialogEx
{
private:
	REGISTER_CODE m_rc;
	std::string m_CLIENT_ID;
	std::string m_CLIENT_KEY;
	std::string m_CLIENT_SECRET;
	CStringArray	m_micList, m_speakerList;
	void ReadAudioSetting();
	CString	m_strMic, m_strSpeaker;
	int	m_nSpeakerSelected;
	CString	m_strLongitude, m_strLatitude, m_strAddress;
	CString m_strServer, m_strGrpcPort, m_strRestPort;
	char	m_szModelDir[255];

public:
	static int m_nMicSelected;
	static DWORD	m_dwStartTickCount;
	static bool		m_bEndBuffer;
	static double	m_dVolume;

private:
	void sendDSSP(char *pTtsTransport);

public:
	void sendTmEvent(const char *pReqAct, const char *pActionTrx);

public:
	static void AddSendLog(CString &sData);
	static void AddRecvLog(CString &sData);
	static void AddLog(CEdit &ctrl, CString &sData);
	static void onEvent(int eventMask, std::string opt);
	static void onCommand(std::string actionType, std::string payload);
	static int ConvertUTF8_to_Unicode(const char *inbuf, wchar_t *outwin);

	static void sendHwEvent(std::string target, std::string event, std::string opt);
	static bool processReqSTTM(std::string cmdPayload);
	static std::string m_sReqAct;
	static std::string m_sActionTrx;
	static int m_nSetTimer;

	static void VolumeDn(char *pLog);
	static void VolumeUp(char *pLog);
	static void VolumeLog(char *pLog);

private:
	int ConvertUnicode_to_UTF8(const wchar_t *win, char *outbuf);
	void EnableButtons(bool enable);

public:
	CAudioCtrl	*m_pPlayCtrl;
	CMicInput	*m_pRecordCtrl;
	void SetAudioControl();

// Construction
public:
	CginsidemfctestDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GINSIDEMFCTEST_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT	OnStopSound(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnEnableButtons(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnProgressiveBar(WPARAM wParam, LPARAM lParam);

	afx_msg void OnBnClickedButtonInit();
	afx_msg void OnBnClickedButtonRegister();
	afx_msg void OnBnClickedButtonUnregister();
	afx_msg void OnBnClickedButtonReset();
	afx_msg void OnBnClickedButtonStartvoice();
	afx_msg void OnBnClickedButtonStopvoice();
	afx_msg void OnBnClickedButtonSendtext();
	afx_msg void OnBnClickedButtonDebugmode();
	afx_msg void OnDestroy();
	static CEdit m_ctrlSend;
	afx_msg void OnBnClickedButtonClearLog();

public:
	static CEdit m_ctrlRecv;
	afx_msg void OnBnClickedButtonAudioSetting();
	CString	m_strVoiceStream;
	CString m_strSendText;
	CString m_strGetTts;
	afx_msg void OnBnClickedButtonGettts();
	afx_msg void OnBnClickedButtonKwsInit();
	afx_msg void OnBnClickedButtonKwsSetKeyword();
	afx_msg void OnBnClickedButtonKwsGetKeyword();
	afx_msg void OnBnClickedButtonKwsGetversion();
	afx_msg void OnBnClickedButtonKwsDetect();
	afx_msg void OnBnClickedButtonVolumeDn();
	afx_msg void OnBnClickedButtonVolumeUp();
	afx_msg void OnBnClickedButtonServicelogin();
	afx_msg void OnBnClickedButtonPrev();
	afx_msg void OnBnClickedButtonNext();
	afx_msg void OnBnClickedButtonVersion();
	afx_msg void OnBnClickedButtonPlay();
	CProgressCtrl m_ctrlSpeechIndicator;
	afx_msg void OnBnClickedButtonSetServerInfo();
	afx_msg void OnBnClickedButtonSetLocation();
	afx_msg void OnBnClickedButtonKwsSetmodelpath();
	afx_msg void OnBnClickedButtonServicelogin2();
	afx_msg void OnBnClickedButtonServiceloginstatus();
	afx_msg void OnBnClickedButtonServicelogout();
};
