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
// ginsidemfc-testDlg.cpp : implementation file
// --------------------------------------------------

#include "stdafx.h"
#include "ginsidemfc-test.h"
#include "ginside.h"
#include "ginsidedef.h"
#include "ginsidemfc-testDlg.h"
#include "afxdialogex.h"
#include "CDlgAudioSetting.h"
#include "CDlgKeyword.h"
#include "CDlgSetLocation.h"
#include "CDlgSetServerInfo.h"
#include "CDlgKwsModelPath.h"
#include "AudioDeviceProperty.h"
#include "Util.h"
#include "base64.h"
#include "cJSON.h"

#pragma warning(disable:4996)

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CginsidemfctestDlg dialog

std::string CginsidemfctestDlg::m_sReqAct;
std::string CginsidemfctestDlg::m_sActionTrx;
int CginsidemfctestDlg::m_nSetTimer = 0;
double CginsidemfctestDlg::m_dVolume = 0.;

CEdit CginsidemfctestDlg::m_ctrlRecv;
CEdit CginsidemfctestDlg::m_ctrlSend;
DWORD CginsidemfctestDlg::m_dwStartTickCount = 0;
int CginsidemfctestDlg::m_nMicSelected = 0;

bool CginsidemfctestDlg::m_bEndBuffer = true;

BOOL		g_bKWS;

CAudioCtrl	*g_pPlayCtrl;
CMicInput	*g_pRecordCtrl;
BOOL		g_bPlay;
BOOL		g_bRecord;
UINT		g_uPlayDeviceID;
UINT		g_uRecordDeviceID;

void Queue(char *pdata, int size);
static void log_update_time(char* pTimestr);

CginsidemfctestDlg::CginsidemfctestDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_GINSIDEMFCTEST_DIALOG, pParent)
	, m_strSendText(_T(""))
	, m_strGetTts(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pPlayCtrl = NULL;
	m_pRecordCtrl = NULL;
}

void CginsidemfctestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SEND_LOG, m_ctrlSend);
	DDX_Control(pDX, IDC_EDIT_RECV_LOG, m_ctrlRecv);
	DDX_Text(pDX, IDC_EDIT_SEND_TEXT, m_strSendText);
	DDX_Text(pDX, IDC_EDIT_GET_TTS, m_strGetTts);
	DDX_Control(pDX, IDC_PROGRESS_SPEECH, m_ctrlSpeechIndicator);
}

BEGIN_MESSAGE_MAP(CginsidemfctestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_STOP_SOUND, OnStopSound)
	ON_MESSAGE(WM_ENABLE_BUTTONS, OnEnableButtons)
	ON_MESSAGE(WM_PROGRESSIVE_BAR, OnProgressiveBar)
	ON_BN_CLICKED(IDC_BUTTON_INIT, &CginsidemfctestDlg::OnBnClickedButtonInit)
	ON_BN_CLICKED(IDC_BUTTON_REGISTER, &CginsidemfctestDlg::OnBnClickedButtonRegister)
	ON_BN_CLICKED(IDC_BUTTON_UNREGISTER, &CginsidemfctestDlg::OnBnClickedButtonUnregister)
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CginsidemfctestDlg::OnBnClickedButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_STARTVOICE, &CginsidemfctestDlg::OnBnClickedButtonStartvoice)
	ON_BN_CLICKED(IDC_BUTTON_STOPVOICE, &CginsidemfctestDlg::OnBnClickedButtonStopvoice)
	ON_BN_CLICKED(IDC_BUTTON_SENDTEXT, &CginsidemfctestDlg::OnBnClickedButtonSendtext)
	ON_BN_CLICKED(IDC_BUTTON_DEBUGMODE, &CginsidemfctestDlg::OnBnClickedButtonDebugmode)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_LOG, &CginsidemfctestDlg::OnBnClickedButtonClearLog)
	ON_BN_CLICKED(IDC_BUTTON_AUDIO_SETTING, &CginsidemfctestDlg::OnBnClickedButtonAudioSetting)
	ON_BN_CLICKED(IDC_BUTTON_GETTTS, &CginsidemfctestDlg::OnBnClickedButtonGettts)
	ON_BN_CLICKED(IDC_BUTTON_KWS_INIT, &CginsidemfctestDlg::OnBnClickedButtonKwsInit)
	ON_BN_CLICKED(IDC_BUTTON_KWS_SET_KEYWORD, &CginsidemfctestDlg::OnBnClickedButtonKwsSetKeyword)
	ON_BN_CLICKED(IDC_BUTTON_KWS_GET_KEYWORD, &CginsidemfctestDlg::OnBnClickedButtonKwsGetKeyword)
	ON_BN_CLICKED(IDC_BUTTON_KWS_GETVERSION, &CginsidemfctestDlg::OnBnClickedButtonKwsGetversion)
	ON_BN_CLICKED(IDC_BUTTON_KWS_DETECT, &CginsidemfctestDlg::OnBnClickedButtonKwsDetect)
	ON_BN_CLICKED(IDC_BUTTON_VOLUME_DN, &CginsidemfctestDlg::OnBnClickedButtonVolumeDn)
	ON_BN_CLICKED(IDC_BUTTON_VOLUME_UP, &CginsidemfctestDlg::OnBnClickedButtonVolumeUp)
	ON_BN_CLICKED(IDC_BUTTON_SERVICELOGIN, &CginsidemfctestDlg::OnBnClickedButtonServicelogin)
	ON_BN_CLICKED(IDC_BUTTON_PREV, &CginsidemfctestDlg::OnBnClickedButtonPrev)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, &CginsidemfctestDlg::OnBnClickedButtonNext)
	ON_BN_CLICKED(IDC_BUTTON_VERSION, &CginsidemfctestDlg::OnBnClickedButtonVersion)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, &CginsidemfctestDlg::OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_SET_SERVER_INFO, &CginsidemfctestDlg::OnBnClickedButtonSetServerInfo)
	ON_BN_CLICKED(IDC_BUTTON_SET_LOCATION, &CginsidemfctestDlg::OnBnClickedButtonSetLocation)
	ON_BN_CLICKED(IDC_BUTTON_KWS_SETMODELPATH, &CginsidemfctestDlg::OnBnClickedButtonKwsSetmodelpath)
	ON_BN_CLICKED(IDC_BUTTON_SERVICELOGINSTATUS, &CginsidemfctestDlg::OnBnClickedButtonServiceloginstatus)
	ON_BN_CLICKED(IDC_BUTTON_SERVICELOGOUT, &CginsidemfctestDlg::OnBnClickedButtonServicelogout)
END_MESSAGE_MAP()

int CginsidemfctestDlg::ConvertUnicode_to_UTF8(const wchar_t *win, char *outbuf)
{
	int nLen = WideCharToMultiByte(CP_UTF8, 0, win, lstrlenW(win), NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_UTF8, 0, win, lstrlenW(win), outbuf, nLen, NULL, NULL);

	return nLen;
}

int CginsidemfctestDlg::ConvertUTF8_to_Unicode(const char *inbuf, wchar_t *outwin)
{
	int nLen = MultiByteToWideChar(CP_UTF8, 0, inbuf, strlen(inbuf), NULL, NULL);
	MultiByteToWideChar(CP_UTF8, 0, inbuf, strlen(inbuf), outwin, nLen);

	return nLen;
}

void get_localtime(char *pTimestr)
{
	SYSTEMTIME	st;

	GetLocalTime(&st);
	sprintf(pTimestr, "%4d%02d%02d%02d%02d%02d",
		st.wYear,
		st.wMonth,
		st.wDay,
		st.wHour,
		st.wMinute,
		st.wSecond);
}

void PostToEnableButtons()
{
	PostMessage(((CginsidemfctestDlg *)AfxGetApp()->m_pMainWnd)->m_hWnd, WM_ENABLE_BUTTONS, 0, 0);
}

void log_update_time(char* pTimestr)
{
	SYSTEMTIME	st;

	GetLocalTime(&st);
	sprintf(pTimestr, "%4d-%02d-%02d %02d:%02d:%02d.%03d",
		st.wYear,
		st.wMonth,
		st.wDay,
		st.wHour,
		st.wMinute,
		st.wSecond,
		st.wMilliseconds);
}

void CginsidemfctestDlg::onEvent(int eventMask, std::string opt)
{
	wchar_t	w_opt[1024];
	char	szLog[1024], szTimestr[200];
	TCHAR	TLog[1024];
	CString	strLog;
	cJSON *cmdp_jsonObj;

	memset(szLog, 0x0, sizeof(szLog));
	memset(w_opt, 0x0, sizeof(w_opt));

	log_update_time(szTimestr);
	sprintf(szLog, "[%s] ", szTimestr);
	if (szLog[0] != 0x0)
	{
		ConvertStringToWideString(szLog, TLog);
		strLog = TLog;
		AddRecvLog(strLog);
		memset(szLog, 0x0, sizeof(szLog));
	}

	switch (eventMask) {
		case INSIDE_EVENT::SERVER_ERROR:
			g_pRecordCtrl->Stop();
			sprintf(szLog, "onEvent ---> SERVER_ERROR (%s) received. MIC is OFF now!\n", opt.c_str());

			cmdp_jsonObj = cJSON_Parse(opt.c_str());
			if (cmdp_jsonObj != NULL) {
				cJSON *cmdp_errorCode = cJSON_GetObjectItem(cmdp_jsonObj, "errorCode");
				cJSON *cmdp_errorMsg = cJSON_GetObjectItem(cmdp_jsonObj, "errorMsg");
				if (cmdp_errorCode != NULL && cmdp_errorMsg != NULL) {
					int errorCode = cmdp_errorCode->valueint;
					std::string	errorMsg = cmdp_errorMsg->valuestring;
					sprintf(szLog, "errorCode: %d, errorMsg: %s\n", errorCode, errorMsg.c_str());
				}
				cJSON_Delete(cmdp_jsonObj);
			}
			break;
		case INSIDE_EVENT::GRPC_INIT_SUCCESS:
			sprintf(szLog, "onEvent RESULT_GRPC_INIT_SUCCESS received.\n");
			PostToEnableButtons();
			break;
		case INSIDE_EVENT::GRPC_INIT_FAIL:
			sprintf(szLog, "onEvent RESULT_GRPC_INIT_FAIL received.\n");
			break;
		case INSIDE_EVENT::GRPC_DISCONNECTED:
			sprintf(szLog, "onEvent GRPC_DISCONNECTED received. opt : %s\n", opt.c_str());
			break;
		case INSIDE_EVENT::GO_TO_STANDBY:
			sprintf(szLog, "onEvent GO_TO_STANDBY received. opt : %s\n", opt.c_str());
			break;
		default:
			break;
	}
	if (szLog[0] != 0x0)
	{
		ConvertStringToWideString(szLog, TLog);
		strLog = TLog;
		AddRecvLog(strLog);
	}
}

UINT TimerThread(LPVOID lpvoid)
{
	int i;

	((CginsidemfctestDlg*)AfxGetApp())->m_sActionTrx;
	for (i = 0; i < ((CginsidemfctestDlg*)AfxGetApp())->m_nSetTimer; i++)
	{
		Sleep(1000);	// 1 sec sleep
	}
	((CginsidemfctestDlg*)AfxGetApp())->sendTmEvent(((CginsidemfctestDlg*)AfxGetApp())->m_sReqAct.c_str(), ((CginsidemfctestDlg*)AfxGetApp())->m_sActionTrx.c_str());

	return 0;
}

void CginsidemfctestDlg::onCommand(std::string actiontype, std::string cmd)
{
	int voiceDataSize = 0;
	const char* voiceData;

	std::string	actionType, msgPayload, strJsonMsg;;
	char	szTemp[10], szLog[1024];
	TCHAR	TLog[1024];
	wchar_t	w_opt[1024];
	CString	strLog;

	cJSON *cmdp_jsonObj = cJSON_Parse(cmd.c_str());
	if (cmdp_jsonObj == NULL)
	{
		sprintf(szLog, "parsing Error\n");
		ConvertStringToWideString(szLog, TLog);
		strLog = TLog;
		AddRecvLog(strLog);
	}
	else
	{
		memset(w_opt, 0x0, sizeof(w_opt));
		actionType = actiontype;

		if (strcmp(actionType.c_str(), "play_media") == 0)
		{
			cJSON* cmdp_cmdOpt = cJSON_GetObjectItem(cmdp_jsonObj, "cmdOpt");
			if (cmdp_cmdOpt != NULL)
			{
				cJSON* cmdp_channel = cJSON_GetObjectItem(cmdp_cmdOpt, "channel");
				cJSON* cmdp_actOnOther = cJSON_GetObjectItem(cmdp_cmdOpt, "actOnOther");
				cJSON* cmdp_metaInfo = cJSON_GetObjectItem(cmdp_cmdOpt, "metaInfo");
				cJSON* cmdp_url = cJSON_GetObjectItem(cmdp_cmdOpt, "url");

				if (cmdp_url == NULL) {
					cJSON* cmdp_mesg = cJSON_GetObjectItem(cmdp_metaInfo, "mesg");
					std::string mesg = cmdp_mesg->valuestring;

					ConvertUTF8_to_Unicode(mesg.c_str(), w_opt);
					wsprintf(TLog, L"metaInfo mesg=[%s]\n", w_opt);
					strLog = TLog;
					AddRecvLog(strLog);
				}
				else {
					sprintf(szLog, "play_media, payload: {channel=%d,actOnOther=%s,url=%s}\n", cmdp_channel->valueint, cmdp_actOnOther->valuestring, cmdp_url->valuestring);
					ConvertStringToWideString(szLog, TLog);
					strLog = TLog;
					AddRecvLog(strLog);

					if (cmdp_url->valuestring != NULL && strstr(cmdp_url->valuestring, "http") != NULL)
					{
						TCHAR player[1024], parameter[1024];
						struct _stat64 stat;
						if (0 == _stat64("C:\\Program Files (x86)\\GRETECH\\GOMPlayer\\GOM.exe", &stat))
						{
							ConvertStringToWideString("C:\\Program Files (x86)\\GRETECH\\GOMPlayer\\GOM.exe", player);
							ConvertStringToWideString(cmdp_url->valuestring, parameter);
							ShellExecute(0, NULL, player, parameter, NULL, SW_SHOWDEFAULT);
						}
						else
						{
							strLog = L"HTTP Media Player does not exit!";
							AddRecvLog(strLog);
						}
					}
				}
			}
		}
		else if (strcmp(actionType.c_str(), "media_data") == 0) {
			cJSON* cmdp_contentType = cJSON_GetObjectItem(cmdp_jsonObj, "contentType");
			if (cmdp_contentType != NULL)
			{
				if (strcmp(cmdp_contentType->valuestring, "wav") == 0)
				{
					cJSON* cmdp_mediastream = cJSON_GetObjectItem(cmdp_jsonObj, "mediastream");
					std::string media = cmdp_mediastream->valuestring;
					std::string decoded = base64_decode(media);

					voiceDataSize = (int)decoded.size();
					voiceData = decoded.c_str();

					m_dwStartTickCount = GetTickCount();
					agent_updateMediaStatus(0, "started", 0);

					strLog.Format(L"Called agent_updateMediaStatus(0, started, 0)\n");
					AddRecvLog(strLog);

					m_bEndBuffer = true;
					Queue((char*)(voiceData + 44), voiceDataSize - 44);
				}
			}
		}
		else if (strcmp(actionType.c_str(), "start_voice") == 0)
		{
			sprintf(szLog, "received action type=%s, MIC is ON now!\n", actionType.c_str());
			ConvertStringToWideString(szLog, TLog);
			strLog = TLog;
			AddRecvLog(strLog);
			g_pRecordCtrl->Record(m_nMicSelected);
		}
		else if (strcmp(actionType.c_str(), "stop_voice") == 0)
		{
			cJSON* cmdp_cmdOpt = cJSON_GetObjectItem(cmdp_jsonObj, "cmdOpt");
			if (cmdp_cmdOpt != NULL)
			{
				cJSON* cmdp_rc = cJSON_GetObjectItem(cmdp_cmdOpt, "resultCd");
				cJSON* cmdp_uword = cJSON_GetObjectItem(cmdp_cmdOpt, "uword");
				if (cmdp_rc->valueint == 200 && cmdp_uword != NULL) {
					std::string uword = cmdp_uword->valuestring;
					ConvertUTF8_to_Unicode(uword.c_str(), w_opt);
					wsprintf(TLog, L"received action type=stop_voice, MIC is OFF now! uword=%s\n", w_opt);
					strLog = TLog;
					AddRecvLog(strLog);
					g_pRecordCtrl->Stop();
					memset(szLog, 0x0, sizeof(szLog));
				}
			}
		}
		else if (strcmp(actionType.c_str(), "set_timer") == 0)
		{
			if (processReqSTTM(cmd.c_str()))
			{
				AfxBeginThread(TimerThread, NULL);
			}
		}
		else if (strcmp(actionType.c_str(), "control_media") == 0)
		{
			cJSON* cmdp_cmdOpt = cJSON_GetObjectItem(cmdp_jsonObj, "cmdOpt");
			if (cmdp_cmdOpt != NULL)
			{
				cJSON *cmdp_channel = cJSON_GetObjectItem(cmdp_cmdOpt, "channel");
				cJSON *cmdp_act = cJSON_GetObjectItem(cmdp_cmdOpt, "act");
				cJSON *cmdp_playtime = cJSON_GetObjectItem(cmdp_cmdOpt, "playtime");
				cJSON *cmdp_setDssStatus = cJSON_GetObjectItem(cmdp_cmdOpt, "setDssStatus");
				cJSON *cmdp_cleartime = cJSON_GetObjectItem(cmdp_cmdOpt, "clearDssStatus");
				sprintf(szLog, "control_media, {channel=%d,act=%s,playtime=%d, ...}\n", cmdp_channel->valueint, cmdp_act->valuestring, cmdp_playtime->valueint);
				ConvertStringToWideString(szLog, TLog);
				strLog = TLog;
				AddRecvLog(strLog);
			}
		}
		else if (strcmp(actionType.c_str(), "exec_dialogkit") == 0)
		{
			cJSON* cmdp_cmdOpt = cJSON_GetObjectItem(cmdp_jsonObj, "cmdOpt");
			if (cmdp_cmdOpt != NULL)
			{
				cJSON* cmdp_execType = cJSON_GetObjectItem(cmdp_cmdOpt, "execType");
				if (cmdp_execType != NULL && strcmp(cmdp_execType->valuestring, "dialogHandler") == 0) {
					cJSON* cmdp_execOpt = cJSON_GetObjectItem(cmdp_cmdOpt, "execOpt");
					if (cmdp_execOpt != NULL) {
						cJSON* cmdp_dssMsgOpt = cJSON_GetObjectItem(cmdp_execOpt, "dssMsg");
						if (cmdp_dssMsgOpt != NULL) {
							char* pDialogResponseStr = cJSON_Print(cmdp_dssMsgOpt);
							ConvertUTF8_to_Unicode(pDialogResponseStr, w_opt);
							wsprintf(TLog, L"dialogResponse = %s\n", w_opt);
							strLog = TLog;
							AddRecvLog(strLog);
						}
					}
				}
			}
		}
		else if (strcmp(actionType.c_str(), "control_hardware") == 0)
		{
			cJSON* cmdp_cmdOpt = cJSON_GetObjectItem(cmdp_jsonObj, "cmdOpt");
			if (cmdp_cmdOpt != NULL)
			{
				cJSON *cmdp_target = cJSON_GetObjectItem(cmdp_cmdOpt, "target");
				cJSON *cmdp_hwCmd = cJSON_GetObjectItem(cmdp_cmdOpt, "hwCmd");
				cJSON *cmdp_hwCmdOpt = cJSON_GetObjectItem(cmdp_cmdOpt, "hwCmdOpt");
				if (strcmp(cmdp_target->valuestring, "volume") == 0 && strcmp(cmdp_hwCmd->valuestring, "setVolume") == 0 && cmdp_hwCmdOpt != NULL)
				{
					cJSON *cmdp_control = cJSON_GetObjectItem(cmdp_hwCmdOpt, "control");
					cJSON *cmdp_value = cJSON_GetObjectItem(cmdp_hwCmdOpt, "value");

					sprintf(szLog, "hardware_control, Req_HWCL, volume, setVolme:(%s, %s)\n", cmdp_control->valuestring, cmdp_value->valuestring);
					ConvertStringToWideString(szLog, TLog);
					strLog = TLog;
					AddRecvLog(strLog);

					if (strcmp(cmdp_control->valuestring, "UP") == 0) {
						VolumeUp(szLog);
					}
					else {
						VolumeDn(szLog);
					}
					VolumeLog(szLog);
				}
			}
		}
		else if (strcmp(actionType.c_str(), "webview_url") == 0)
		{
			cJSON* cmdp_cmdOpt = cJSON_GetObjectItem(cmdp_jsonObj, "cmdOpt");
			if (cmdp_cmdOpt != NULL)
			{
				cJSON *cmdp_oauth_url = cJSON_GetObjectItem(cmdp_cmdOpt, "oauth_url");

				sprintf(szLog, "webview_url, Req_OAuth, payload: {oauth_url=%s}\n", cmdp_oauth_url->valuestring);
				ConvertStringToWideString(szLog, TLog);
				strLog = TLog;
				AddRecvLog(strLog);
				// Start the Player
				ConvertStringToWideString(cmdp_oauth_url->valuestring, TLog);
				ShellExecute(0, NULL, TLog, NULL, NULL, SW_SHOWDEFAULT);
			}
		}

		cJSON_Delete(cmdp_jsonObj);
	}
}

void CginsidemfctestDlg::EnableButtons(bool enable)
{
	((CButton*)GetDlgItem(IDC_BUTTON_UNREGISTER))->EnableWindow(enable);
	((CButton*)GetDlgItem(IDC_BUTTON_RESET))->EnableWindow(enable);
	((CButton*)GetDlgItem(IDC_BUTTON_GETTTS))->EnableWindow(enable);
	((CButton*)GetDlgItem(IDC_BUTTON_SENDTEXT))->EnableWindow(enable);
	((CButton*)GetDlgItem(IDC_BUTTON_SERVICELOGIN))->EnableWindow(enable);
	((CButton*)GetDlgItem(IDC_BUTTON_SERVICELOGINSTATUS))->EnableWindow(enable);
	((CButton*)GetDlgItem(IDC_BUTTON_SERVICELOGOUT))->EnableWindow(enable);
	((CButton*)GetDlgItem(IDC_BUTTON_STARTVOICE))->EnableWindow(enable);
	((CButton*)GetDlgItem(IDC_BUTTON_STOPVOICE))->EnableWindow(enable);
	((CButton*)GetDlgItem(IDC_BUTTON_NEXT))->EnableWindow(enable);
	((CButton*)GetDlgItem(IDC_BUTTON_PLAY))->EnableWindow(enable);
	((CButton*)GetDlgItem(IDC_BUTTON_PREV))->EnableWindow(enable);
	((CButton*)GetDlgItem(IDC_BUTTON_VOLUME_UP))->EnableWindow(enable);
	((CButton*)GetDlgItem(IDC_BUTTON_VOLUME_DN))->EnableWindow(enable);
	((CButton*)GetDlgItem(IDC_BUTTON_SET_LOCATION))->EnableWindow(enable);
}

bool CginsidemfctestDlg::processReqSTTM(std::string cmdPayload)
{
	char	szLog[1024];
	TCHAR	TLog[1024];
	CString	strLog;
	bool ret = false;

	cJSON *cmdp_jsonObj = cJSON_Parse(cmdPayload.c_str());
	if (cmdp_jsonObj != NULL)
	{
		cJSON *cmdp_cmdOpt = cJSON_GetObjectItem(cmdp_jsonObj, "cmdOpt");
		if (cmdp_cmdOpt != NULL)
		{
			cJSON *setOpt_obj = cJSON_GetObjectItem(cmdp_cmdOpt, "setOpt");
			cJSON *reqAct_obj = cJSON_GetObjectItem(cmdp_cmdOpt, "reqAct");
			cJSON *actionTrx_obj = cJSON_GetObjectItem(cmdp_cmdOpt, "actionTrx");
			cJSON *setTime_obj = cJSON_GetObjectItem(cmdp_cmdOpt, "setTime");
			if (setOpt_obj != NULL)
			{
				sprintf(szLog, "set_timer :setOpt=[%s], reqAct=[%s], actionTrx=[%s], setTime=[%s]\n",
					setOpt_obj->valuestring, reqAct_obj->valuestring, actionTrx_obj->valuestring, setTime_obj->valuestring);
				ConvertStringToWideString(szLog, TLog);
				strLog = TLog;
				AddRecvLog(strLog);

				if (strcmp(setOpt_obj->valuestring, "set") == 0)
				{
					if (reqAct_obj != NULL && actionTrx_obj != NULL && setTime_obj != NULL)
					{
						std::string reqAct(reqAct_obj->valuestring);
						std::string actionTrx(actionTrx_obj->valuestring);
						std::string setTime(setTime_obj->valuestring);

						m_sReqAct = reqAct;
						m_sActionTrx = actionTrx;
						m_nSetTimer = atoi(setTime.c_str());
						ret = true;
					}
				}
				else
				{	// clear....
					// remove the previous timer.
				}
			}
		}

		cJSON_Delete(cmdp_jsonObj);
	}

	return ret;
}

BOOL CginsidemfctestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Initialize Winsock
	WSADATA wsaData;
	int		ret;

	ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret != 0)
	{
		TRACE("WSAStartup failed: %d\n", ret);
		PostQuitMessage(0);
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	CAudioDeviceProperty	adp;
	adp.GetDeviceList(m_micList, m_speakerList);

	int i;

	TRACE("MIC list:\n");
	for (i = 0; i < m_micList.GetSize(); i++)
	{
		TRACE(m_micList.ElementAt(i) + L"\n");
	}
	TRACE("SPEAKER list:\n");
	for (i = 0; i < m_speakerList.GetSize(); i++)
	{
		TRACE(m_speakerList.ElementAt(i) + L"\n");
	}

	SetAudioControl();

	agent_setServerInfo("inside-dev.gigagenie.ai", "50109", "30109");

	g_bKWS = TRUE;
	m_rc.uuid = "";

	m_strLongitude = "127.027530";
	m_strLatitude = "37.4724275";
	m_strAddress = "서울특별시 서초구 우면동";

	m_pPlayCtrl->GetCurrentVolume(m_dVolume);
	m_ctrlSpeechIndicator.SetRange(0, 100);
	m_ctrlSpeechIndicator.SetPos(0);
	EnableButtons(false);

	int		count, length;
	char	szTemp[255];
	FILE *fp = fopen("key.txt", "rt");
	if (fp != NULL)
	{
		count = 0;
		while(!feof(fp))
		{
			memset(szTemp, 0x0, sizeof(szTemp));
			fgets(szTemp, sizeof(szTemp), fp);
			length = strlen(szTemp);
			if (szTemp[length - 1] == '\n') szTemp[length - 1] = 0x0;
			switch (count)
			{
			case 0:
				m_CLIENT_ID = szTemp;
				break;
			case 1:
				m_CLIENT_KEY = szTemp;
				break;
			case 2:
				m_CLIENT_SECRET = szTemp;
				break;
			}
			count++;
		}
		fclose(fp);
	}
	else
	{
		MessageBox(L"Key file (key.txt) does not exist. so exit!", L"error");
		PostQuitMessage(0);
	}

	((CButton*)GetDlgItem(IDC_BUTTON_KWS_SET_KEYWORD))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_BUTTON_KWS_GET_KEYWORD))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_BUTTON_KWS_DETECT))->EnableWindow(FALSE);

	strcpy(m_szModelDir, ".\\conf");

	return TRUE;  // return TRUE  unless you set the focus to a control
}

HRESULT __cdecl RecordCallback(BYTE* pRecordedData, int iSize)
{
	char	szLog[1024];
	TCHAR	TLog[1024];

	short		pcm_val[1280];
	unsigned long	l_pcm_val;
	unsigned short	pcm_val_avg;
	int i, ret;
	static int prev_ret = -1, hit = 0;
	CString strLog;

	for (i = 0; i < iSize; i += 2) {
		pcm_val[i / 2] = pRecordedData[i] + pRecordedData[i + 1] * 256; //2^16
	}

	// for mic wave //
	l_pcm_val = 0;
	for (i = 0; i < iSize / 2; i ++) {
		l_pcm_val += (pcm_val[i] > 0 ? pcm_val[i] : -1 * pcm_val[i]);
	}

	pcm_val_avg = (unsigned short) ((double)l_pcm_val / (iSize / 2.) / 32635. * 100.);
	PostMessage(((CginsidemfctestDlg *)AfxGetApp()->m_pMainWnd)->m_hWnd, WM_PROGRESSIVE_BAR, pcm_val_avg, 0);
	//////////////////

	if (g_bKWS)
	{
		ret = kws_detect(pcm_val, iSize / 2);
		switch (ret)
		{
		case KWS_DET_ERROR:
			strLog.Format(L"ERROR: \n");
			break;
		case KWS_DET_SILENCE:
			strLog.Format(L"Silence.\n");
			break;
		case KWS_DET_START:
			strLog.Format(L"Speech starts.\n");
			break;
		case KWS_DET_SPEECH:
			strLog.Format(L"speech.\n");
			break;
		case KWS_DET_END:
			strLog.Format(L"Speech ends.\n");
			break;
		case KWS_DET_DETECTED:
			strLog.Format(L"HIT!!!\n");
			hit = 1;
			break;
		}
		if (ret != prev_ret)
		{
			((CginsidemfctestDlg *)AfxGetApp())->AddRecvLog(strLog);
			prev_ret = ret;
			if (ret == KWS_DET_END && hit == 1)
			{	// The following code must be checked!
				g_pRecordCtrl->Stop();
				g_bKWS = FALSE;
				hit = 0;
				agent_startVoice();
			}
		}
	}
	else
	{
		agent_sendVoice(pcm_val, iSize / 2);
	}

	return 0;
}

void CginsidemfctestDlg::SetAudioControl()
{
	if (m_pPlayCtrl == NULL)
	{
		m_pPlayCtrl = new CAudioCtrl(CAudioCtrl::PLAY);
		g_pPlayCtrl = m_pPlayCtrl;
	}
	else
	{
		m_pPlayCtrl->PlayStop();
		delete m_pPlayCtrl;
		m_pPlayCtrl = new CAudioCtrl(CAudioCtrl::PLAY);
		g_pPlayCtrl = m_pPlayCtrl;
	}

	if (m_pRecordCtrl == NULL)
	{
		m_pRecordCtrl = new CMicInput;
		g_pRecordCtrl = m_pRecordCtrl;
	}
	else
	{
		m_pRecordCtrl->Stop();
		delete m_pRecordCtrl;
		m_pRecordCtrl = new CMicInput;
		g_pRecordCtrl = m_pRecordCtrl;
	}

	m_pRecordCtrl->SetCallBackFunc((REC_CALLBACK) RecordCallback);
	m_pRecordCtrl->SetMode(1, 16000, 16, 10, 1280); //모노 , 16khz, 16bit, 10개의 버퍼 , 1280(10msec) bytes의 버퍼 
}

LRESULT CginsidemfctestDlg::OnProgressiveBar(WPARAM wParam, LPARAM lParam)
{
	m_ctrlSpeechIndicator.SetPos(wParam);
	return 0;
}

LRESULT	CginsidemfctestDlg::OnEnableButtons(WPARAM wParam, LPARAM lParam)
{
	EnableButtons(true);
	return 0;
}

LRESULT CginsidemfctestDlg::OnStopSound(WPARAM wParam, LPARAM lParam)
{
	if (m_pRecordCtrl == NULL)
	{
		m_pRecordCtrl = new CMicInput;
		g_pRecordCtrl = m_pRecordCtrl;
	}
	else
	{
		m_pRecordCtrl->Stop();
		delete m_pRecordCtrl;
		m_pRecordCtrl = new CMicInput;
		g_pRecordCtrl = m_pRecordCtrl;
	}
	m_pRecordCtrl->SetCallBackFunc((REC_CALLBACK)RecordCallback);
	m_pRecordCtrl->SetMode(1, 16000, 16, 10, 1280); //모노 , 16khz, 16bit, 10개의 버퍼 , 1280(20msec) bytes의 버퍼 

	m_pPlayCtrl->PlayStop();

	char	szLog[1024];
	TCHAR	TLog[1024];
	CString strLog;

	sprintf(szLog, "OnStopSound()..........\n");
	ConvertStringToWideString(szLog, TLog);
	strLog = TLog;
	AddRecvLog(strLog);

	return 0;
}

void Queue(char *pdata, int size)
{
	char	szLog[1024];
	TCHAR	TLog[1024];
	CString strLog;

	sprintf(szLog, "Now I am playing, g_uPlayDeviceID == %d, size == %d\n", g_uPlayDeviceID, size);
	ConvertStringToWideString(szLog, TLog);
	strLog = TLog;
	((CginsidemfctestDlg *)AfxGetApp())->AddRecvLog(strLog);

	g_pPlayCtrl->WriteDataPlay(pdata, size, g_uPlayDeviceID);

	if (((CginsidemfctestDlg *)AfxGetApp())->m_bEndBuffer)
	{
		DWORD dwPlayTime = GetTickCount() - ((CginsidemfctestDlg *)AfxGetApp())->m_dwStartTickCount;
		agent_updateMediaStatus(0, "complete", dwPlayTime);

		sprintf(szLog, "Called agent_updateMediaStatus(0, complete, %d)\n", dwPlayTime);
		ConvertStringToWideString(szLog, TLog);
		strLog = TLog;
		((CginsidemfctestDlg *)AfxGetApp())->AddRecvLog(strLog);
	}
}

void CginsidemfctestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CginsidemfctestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CginsidemfctestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CginsidemfctestDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: Add your message handler code here
	WSACleanup();
}

void CginsidemfctestDlg::AddSendLog(CString &sData)
{
	AddLog(m_ctrlSend, sData);
}

void CginsidemfctestDlg::AddRecvLog(CString &sData)
{
	AddLog(m_ctrlRecv, sData);
}

void CginsidemfctestDlg::AddLog(CEdit &ctrl, CString &sData)
{
	if (ctrl.GetLineCount() > 500)
	{
		ctrl.SetReadOnly(FALSE);
		ctrl.SetSel(0, -1);
		ctrl.Clear();
		ctrl.SetReadOnly(TRUE);
		return;
	}
	ctrl.SetRedraw(FALSE);
	CRect rcClient;
	ctrl.GetClientRect(&rcClient);
	int nPrevLineCount = ctrl.GetLineCount();

	int iTotalTextLength = ctrl.GetWindowTextLength();
	ctrl.SetSel(iTotalTextLength, iTotalTextLength);
	ctrl.ReplaceSel((LPCTSTR)sData);
	int nFinalLineCount = ctrl.GetLineCount();

	ctrl.LineScroll(nFinalLineCount - nPrevLineCount);
	ctrl.SetRedraw(TRUE);
}

void CginsidemfctestDlg::sendHwEvent(std::string target, std::string event, std::string opt)
{
	char	szLog[1024];
	TCHAR	TLog[1024];
	CString strLog;
	cJSON *payload_root = cJSON_CreateObject();

	cJSON *cmdtype = cJSON_CreateString(CMD_SND_HWEV);
	cJSON *cmdOpt_target = cJSON_CreateString((const char*)target.c_str());
	cJSON *cmdOpt_event = cJSON_CreateString((const char*)event.c_str());
	cJSON *cmdOpt_opt = cJSON_CreateString((const char*)opt.c_str());

	cJSON_AddItemToObject(payload_root, "cmdType", cmdtype);
	cJSON_AddItemToObject(payload_root, "target", cmdOpt_target);
	cJSON_AddItemToObject(payload_root, "event", cmdOpt_event);
	cJSON_AddItemToObject(payload_root, "eventOpt", cmdOpt_opt);
	if (target == "volume")
	{
		cJSON *cmdOpt_volume = cJSON_CreateNumber(atoi(opt.c_str()));
		cJSON_AddItemToObject(cmdOpt_opt, "value", cmdOpt_volume);
	}
	char *msgPayloadStr = cJSON_Print(payload_root);
	agent_sendCommand(msgPayloadStr);

	sprintf(szLog, "HW Event=%s\n", msgPayloadStr);
	ConvertStringToWideString(szLog, TLog);
	strLog = TLog;
	AddSendLog(strLog);

	cJSON_Delete(payload_root);
	free(msgPayloadStr);
}

void CginsidemfctestDlg::sendTmEvent(const char *pReqAct, const char *pActionTrx)
{
	char	szLog[1024];
	TCHAR	TLog[1024];
	CString strLog;
	char	szLocalTime[20];

	get_localtime(szLocalTime);
	cJSON *payload_root = cJSON_CreateObject();
	cJSON *cmdtype = cJSON_CreateString(CMD_SND_TMEV);
	cJSON *cmdOpt_reqAct = cJSON_CreateString(pReqAct);
	cJSON *cmdOpt_actionTrx = cJSON_CreateString(pActionTrx);
	cJSON *cmdOpt_localTime = cJSON_CreateString(szLocalTime);

	cJSON_AddItemToObject(payload_root, "cmdType", cmdtype);
	cJSON_AddItemToObject(payload_root, "reqAct", cmdOpt_reqAct);
	cJSON_AddItemToObject(payload_root, "actionTrx", cmdOpt_actionTrx);
	cJSON_AddItemToObject(payload_root, "localTime", cmdOpt_localTime);

	char *msgPayloadStr = cJSON_Print(payload_root);
	agent_sendCommand(msgPayloadStr);

	sprintf(szLog, "HW Event (Timer)=%s\n", msgPayloadStr);
	ConvertStringToWideString(szLog, TLog);
	strLog = TLog;
	AddSendLog(strLog);

	cJSON_Delete(payload_root);
	free(msgPayloadStr);
}

void CginsidemfctestDlg::OnBnClickedButtonRegister()
{
	char	szLog[1024];
	TCHAR	TLog[1024];
	CString	strLog;
	REGISTER_CODE rc;

	strLog = _T("/v2/authorize\n");
	AddSendLog(strLog);
	m_rc = agent_register(m_CLIENT_ID, m_CLIENT_KEY, m_CLIENT_SECRET, "");
	if (m_rc.rc != 200)
	{
		sprintf(szLog, "Error: agent_register got rc=%d, rcmsg=%s\n", m_rc.rc, m_rc.rcmsg.c_str());
		ConvertStringToWideString(szLog, TLog);
		strLog = TLog;
	}
	else
	{
		sprintf(szLog, "agent_register successfully got rc=%d, rcmsg=%s, UUID=%s\n", m_rc.rc, m_rc.rcmsg.c_str(), m_rc.uuid.c_str());
		ConvertStringToWideString(szLog, TLog);
		strLog = TLog;
		// Write UUID to a file
		FILE *fp;
		fp = fopen("uuid.txt", "wt");
		if (fp != NULL)
		{
			fprintf(fp, "%s", m_rc.uuid.c_str());
			fclose(fp);
		}
	}
	AddRecvLog(strLog);
}

void CginsidemfctestDlg::OnBnClickedButtonInit()
{
	char	szLog[1024];
	TCHAR	TLog[1024];
	CString strLog;
	REGISTER_CODE rc;

	// Read UUID from a file
	if (m_rc.uuid.length() == 0)
	{
		FILE	*fp;
		char	szTemp[255];
		fp = fopen("uuid.txt", "rt");
		if (fp != NULL)
		{
			fread(szTemp, sizeof(char), sizeof(szTemp), fp);
			m_rc.uuid = szTemp;
			fclose(fp);
		}
		if (m_rc.uuid.length() == 0)
		{
			strLog = _T("There is no UUID. Please use agent_register(), first!\n");
			AddRecvLog(strLog);
			return;
		}
	}
	sprintf(szLog, "Connect to GRPC with UUID=%s\n", m_rc.uuid.c_str());
	ConvertStringToWideString(szLog, TLog);
	strLog = TLog;
	AddSendLog(strLog);
	rc = agent_init(m_CLIENT_ID, m_CLIENT_KEY, m_CLIENT_SECRET, m_rc.uuid.c_str());
	if (rc.rc != 200)
	{
		sprintf(szLog, "Error: agent_init got rc=%d, rcmsg=%s\n", rc.rc, rc.rcmsg.c_str());
		ConvertStringToWideString(szLog, TLog);
		strLog = TLog;
		AddRecvLog(strLog);
		return;
	}
	else
	{
		sprintf(szLog, "agent_init successfully got rc=%d, rcmsg=%s, UUID=%s\n", rc.rc, rc.rcmsg.c_str(), rc.uuid.c_str());
		ConvertStringToWideString(szLog, TLog);
		strLog = TLog;
		AddRecvLog(strLog);
	}
	agent_setCommandEventCallback(this->onCommand, this->onEvent);

	((CButton*)GetDlgItem(IDC_BUTTON_REGISTER))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_BUTTON_INIT))->EnableWindow(FALSE);
}


void CginsidemfctestDlg::OnBnClickedButtonUnregister()
{
	char	szLog[1024];
	TCHAR	TLog[1024];
	CString strLog;

	

	sprintf(szLog, "Unregister with UUID=%s\n", m_rc.uuid.c_str());
	ConvertStringToWideString(szLog, TLog);
	strLog = TLog;
	AddSendLog(strLog);

	REGISTER_CODE rc = agent_unregister();
	sprintf(szLog, "agent_unregister() returned %d\n", rc.rc);
	ConvertStringToWideString(szLog, TLog);
	strLog = TLog;
	AddRecvLog(strLog);
	if (rc.rc != 200) {
		sprintf(szLog, "agent_unregister fail. try retry.");
		ConvertStringToWideString(szLog, TLog);
		strLog = TLog;
		AddRecvLog(strLog);
	}
	else {
		m_rc.uuid = "";

		DeleteFile(L"uuid.txt");

		((CButton*)GetDlgItem(IDC_BUTTON_REGISTER))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_BUTTON_INIT))->EnableWindow(TRUE);
		EnableButtons(false);
	}
}


void CginsidemfctestDlg::OnBnClickedButtonReset()
{
	char	szLog[1024];
	TCHAR	TLog[1024];
	CString strLog;

	sprintf(szLog, "agent_reset() is called\n");
	ConvertStringToWideString(szLog, TLog);
	strLog = TLog;
	AddSendLog(strLog);
	agent_reset();

	((CButton*)GetDlgItem(IDC_BUTTON_REGISTER))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_BUTTON_INIT))->EnableWindow(TRUE);
	EnableButtons(FALSE);
}


void CginsidemfctestDlg::OnBnClickedButtonStartvoice()
{
	char	szLog[1024];
	TCHAR	TLog[1024];
	CString strLog;

	g_bKWS = FALSE;
	sprintf(szLog, "agent_startVoice() is called and a recording started.\n");
	ConvertStringToWideString(szLog, TLog);
	strLog = TLog;
	AddSendLog(strLog);

	agent_startVoice();
}


void CginsidemfctestDlg::OnBnClickedButtonStopvoice()
{
	char	szLog[1024];
	TCHAR	TLog[1024];
	CString strLog;

	sprintf(szLog, "agent_stopVoice() is called and a recording stopped.\n");
	ConvertStringToWideString(szLog, TLog);
	strLog = TLog;
	AddSendLog(strLog);

	agent_stopVoice();
	m_pRecordCtrl->Stop();
}


void CginsidemfctestDlg::OnBnClickedButtonSendtext()
{
	char szSendText[4096];
	char outbuf[4096];
	char	szLog[1024];
	TCHAR	TLog[1024];
	CString strLog;

	UpdateData(TRUE);

	memset(outbuf, 0x0, sizeof(outbuf));
	ConvertUnicode_to_UTF8(m_strSendText.operator LPCWSTR(), outbuf);
	ConvertWideStringToString(m_strSendText, szSendText, sizeof(szSendText));
	sprintf(szLog, "agent_sendText [%s]\n", szSendText);
	ConvertStringToWideString(szLog, TLog);
	strLog = TLog;
	AddSendLog(strLog);

	agent_sendText(outbuf);
}

void CginsidemfctestDlg::OnBnClickedButtonDebugmode()
{
	CString strDebug;

	GetDlgItem(IDC_BUTTON_DEBUGMODE)->GetWindowTextW(strDebug);
	if (strDebug.Find(L"OFF") > 0)
	{
		GetDlgItem(IDC_BUTTON_DEBUGMODE)->SetWindowTextW(L"DEBUGMODE ON");
		agent_debugmode(true);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_DEBUGMODE)->SetWindowTextW(L"DEBUGMODE OFF");
		agent_debugmode(false);
	}
}


void CginsidemfctestDlg::OnBnClickedButtonClearLog()
{
	m_ctrlSend.SetReadOnly(FALSE);
	m_ctrlSend.SetSel(0, -1);
	m_ctrlSend.Clear();
	m_ctrlSend.SetReadOnly(TRUE);

	m_ctrlRecv.SetReadOnly(FALSE);
	m_ctrlRecv.SetSel(0, -1);
	m_ctrlRecv.Clear();
	m_ctrlRecv.SetReadOnly(TRUE);
}


void CginsidemfctestDlg::OnBnClickedButtonAudioSetting()
{
	CDlgAudioSetting	dlg;
	dlg.SetAudioList(m_micList, m_speakerList);
	dlg.SetSelectedAudio(m_nMicSelected, m_nSpeakerSelected);
	if (dlg.DoModal() == IDOK)
	{
		m_nMicSelected = dlg.m_nMicSelected;
		g_uRecordDeviceID = m_nMicSelected;

		m_nSpeakerSelected = dlg.m_nSpeakerSelected;
		g_uPlayDeviceID = m_nSpeakerSelected;

		SetAudioControl();

		m_strMic = dlg.m_strMic;
		m_strSpeaker = dlg.m_strSpeaker;
	}
}

void CginsidemfctestDlg::OnBnClickedButtonGettts()
{
	char szGetTts[4096];
	char outbuf[4096];
	char	szLog[1024];
	TCHAR	TLog[1024];
	CString strLog;
	std::string rc;
	int voiceDataSize = 0;
	const char* voiceData;

	UpdateData(TRUE);

	memset(outbuf, 0x0, sizeof(outbuf));
	ConvertUnicode_to_UTF8(m_strGetTts.operator LPCWSTR(), outbuf);
	ConvertWideStringToString(m_strGetTts, szGetTts, sizeof(szGetTts));
	sprintf(szLog, "agent_getTTS [%s]\n", szGetTts);
	ConvertStringToWideString(szLog, TLog);
	strLog = TLog;
	AddSendLog(strLog);

	rc = agent_getTTS(outbuf);

	cJSON* cmdp_jsonObj = cJSON_Parse(rc.c_str());
	if (cmdp_jsonObj != NULL)
	{
		cJSON* cmdp_rc = cJSON_GetObjectItem(cmdp_jsonObj, "rc");

		if (cmdp_rc->valueint == 200) { //wav
			cJSON* cmdp_rcmsg = cJSON_GetObjectItem(cmdp_jsonObj, "rcmsg");
			std::string media = cmdp_rcmsg->valuestring;
			std::string decoded = base64_decode(media);

			voiceDataSize = (int)decoded.size();
			voiceData = decoded.c_str();

			m_dwStartTickCount = GetTickCount();
			agent_updateMediaStatus(0, "started", 0);

			strLog.Format(L"Called agent_updateMediaStatus(0, started, 0)\n");
			AddRecvLog(strLog);

			m_bEndBuffer = true;
			Queue((char*)(voiceData + 44), voiceDataSize - 44);
		}
		else {
			sprintf(szLog, "agent_getTTS returned %s\n", cmdp_jsonObj->valuestring);
			ConvertStringToWideString(szLog, TLog);
			strLog = TLog;
			AddRecvLog(strLog);
		}
	}
}


void CginsidemfctestDlg::OnBnClickedButtonKwsInit()
{
	int ret;
	CString str;

	str.Format(L"KWS is successfully initialized.\n");
	if ((ret = kws_init()) != KWS_ERR_SUCCESS)
	{
		str.Format(L"Failed to initialize KWS. error code=%d.\n", ret);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_BUTTON_KWS_SET_KEYWORD))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_BUTTON_KWS_GET_KEYWORD))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_BUTTON_KWS_DETECT))->EnableWindow(TRUE);
	}

	AddRecvLog(str);
}


void CginsidemfctestDlg::OnBnClickedButtonKwsSetKeyword()
{
	CDlgKeyword	dlg;

	dlg.setCurrentKeyword(kws_getKeyword());
	if (dlg.DoModal() == IDOK)
	{
		kws_setKeyword(dlg.getNewKeyword());
		OnBnClickedButtonKwsGetKeyword();
	}
}


void CginsidemfctestDlg::OnBnClickedButtonKwsGetKeyword()
{
	CString		str;

	switch (kws_getKeyword())
	{
	case 0:
		str.Format(L"Current keyword: %d = 기가지니\n", kws_getKeyword());
		break;
	case 1:
		str.Format(L"Current keyword: %d = 지니야\n", kws_getKeyword());
		break;
	case 2:
		str.Format(L"Current keyword: %d = 친구야\n", kws_getKeyword());
		break;
	case 3:
		str.Format(L"Current keyword: %d = 자기야\n", kws_getKeyword());
		break;
	}

	AddRecvLog(str);
}

void CginsidemfctestDlg::OnBnClickedButtonKwsGetversion()
{
	char	szVersion[255];
	TCHAR	TLog[1024];
	CString		str;
	sprintf(szVersion, "Current KWS vesion: %s\n", kws_getVersion());
	ConvertStringToWideString(szVersion, TLog);
	str = TLog;
	AddRecvLog(str);
}

void CginsidemfctestDlg::OnBnClickedButtonKwsDetect()
{
	g_bKWS = TRUE;
	m_pRecordCtrl->Record(m_nMicSelected);
}

void CginsidemfctestDlg::VolumeDn(char *pLog)
{
	if (m_dVolume <= 0.)
	{
		sprintf(pLog, "Send Hardware Event (Set Volume: 0 -> 0)\n");
	}
	else
	{
		m_dVolume -= 0.05;
		if (m_dVolume < 0.)
		{
			sprintf(pLog, "Send Hardware Event (Set Volume: %d -> 0)\n", (int)((m_dVolume + 0.05) * 100));
			m_dVolume = 0.;
		}
		else
		{
			sprintf(pLog, "Send Hardware Event (Set Volume: %d -> %d)\n", (int)((m_dVolume + 0.05) * 100), (int)(m_dVolume * 100));
		}
	}
}

void CginsidemfctestDlg::VolumeUp(char *pLog)
{
	if (m_dVolume >= 1.0)
	{
		sprintf(pLog, "Send Hardware Event (Set Volume: 100 -> 100)\n");
	}
	else
	{
		m_dVolume += 0.05;
		if (m_dVolume >= 1.0)
		{
			sprintf(pLog, "Send Hardware Event (Set Volume: %d -> 100)\n", (int)((m_dVolume - 0.05) * 100));
			m_dVolume = 1.0;
		}
		else
		{
			sprintf(pLog, "Send Hardware Event (Set Volume: %d -> %d)\n", (int)((m_dVolume - 0.05) * 100), (int)(m_dVolume * 100));
		}
	}
}

void CginsidemfctestDlg::VolumeLog(char *pLog)
{
	char	szLog[1024];
	TCHAR	TLog[1024];
	CString	str;

	ConvertStringToWideString(pLog, TLog);
	str = TLog;
	AddRecvLog(str);

	g_pPlayCtrl->ChangeVolume(m_dVolume, true);
	sprintf(szLog, "%d", (int)(100 * m_dVolume));
	sendHwEvent("volume", "setVolume", szLog);
}

void CginsidemfctestDlg::OnBnClickedButtonVolumeDn()
{
	char	szLog[1024];
	VolumeDn(szLog);
	VolumeLog(szLog);
}

void CginsidemfctestDlg::OnBnClickedButtonVolumeUp()
{
	char	szLog[1024];
	VolumeUp(szLog);
	VolumeLog(szLog);
}

void CginsidemfctestDlg::OnBnClickedButtonServicelogin()
{
	char	szTemp[1024];
	TCHAR	TLog[1024];
	CString		str;

	std::string strRet = agent_serviceLogin("geniemusic", "");
	if (strRet.size() > 0)
	{
		cJSON *cmdp_jsonObj = cJSON_Parse(strRet.c_str());
		if (cmdp_jsonObj == NULL)
		{
			sprintf(szTemp, "ERROR: agent_serviceLogin returns %s\n", strRet.c_str());
		}
		else
		{
			cJSON *cmdp_rc = cJSON_GetObjectItem(cmdp_jsonObj, "rc");
			cJSON *cmdp_rcmsg = cJSON_GetObjectItem(cmdp_jsonObj, "rcmsg");
			if (cmdp_rc->valueint == 200)
			{
				sprintf(szTemp, "agent_serviceLogin returned, [%s]\n\n", strRet.c_str());
				ConvertStringToWideString(szTemp, TLog);
				str = TLog;
				AddRecvLog(str);

				cJSON *cmdp_oauth_url = cJSON_GetObjectItem(cmdp_jsonObj, "oauth_url");
				sprintf(szTemp, "oauth_url=%s\n", cmdp_oauth_url->valuestring);
				ConvertStringToWideString(cmdp_oauth_url->valuestring, TLog);
				ShellExecute(0, NULL, TLog, NULL, NULL, SW_SHOWDEFAULT);
			}
			else
			{
				sprintf(szTemp, "ERROR: agent_serviceLogin returns an error, return code=%d, msg=%s\n", cmdp_rc->valueint, cmdp_rcmsg->valuestring);
			}
			cJSON_Delete(cmdp_jsonObj);
		}
	}
	else
	{
		strcpy(szTemp, "ERROR: agent_serviceLogin retrurned nothing\n");
	}

	ConvertStringToWideString(szTemp, TLog);
	str = TLog;
	AddRecvLog(str);
}


void CginsidemfctestDlg::OnBnClickedButtonPrev()
{
	CString		str;
	str = L"Send Hardware Event (Button: Previous)\n";
	AddRecvLog(str);
	sendHwEvent("button", "Btn_PV", "");
}


void CginsidemfctestDlg::OnBnClickedButtonNext()
{
	CString		str;
	str = L"Send Hardware Event (Button: Next)\n";
	AddRecvLog(str);
	sendHwEvent("button", "Btn_NX", "");
}


void CginsidemfctestDlg::OnBnClickedButtonVersion()
{
	char		szLog[255];
	TCHAR		TLog[255];
	CString		strLog;

	sprintf(szLog, "inside SDK version: %s\n", agent_getVersion());
	ConvertStringToWideString(szLog, TLog);
	strLog = TLog;
	AddRecvLog(strLog);
}


void CginsidemfctestDlg::OnBnClickedButtonPlay()
{
	CString		str;
	str = L"Send Hardware Event (Button: PU)\n";
	AddRecvLog(str);
	sendHwEvent("button", "Btn_PU", "");
}


void CginsidemfctestDlg::OnBnClickedButtonSetServerInfo()
{
	CDlgSetServerInfo	dlg;
	char	server[255], grpcport[20], restport[20];

	dlg.setServerInfo(m_strServer, m_strGrpcPort, m_strRestPort);
	if (dlg.DoModal() == IDOK)
	{
		ConvertWideStringToString(dlg.m_strServer, server, sizeof(server));
		ConvertWideStringToString(dlg.m_strGrpcPort, grpcport, sizeof(grpcport));
		ConvertWideStringToString(dlg.m_strRestPort, restport, sizeof(restport));

		agent_setServerInfo(server, grpcport, restport);

		m_strServer = dlg.m_strServer;
		m_strGrpcPort = dlg.m_strGrpcPort;
		m_strRestPort = dlg.m_strRestPort;
	}
}


void CginsidemfctestDlg::OnBnClickedButtonSetLocation()
{
	CString		str;
	CDlgSetLocation	dlg;
	char	longitude[255], latitude[255], address[1024];

	dlg.setLocation(m_strLongitude, m_strLatitude, m_strAddress);
	if (dlg.DoModal() == IDOK)
	{
		memset(longitude, 0x0, sizeof(longitude));
		memset(latitude, 0x0, sizeof(latitude));
		memset(address, 0x0, sizeof(address));

		ConvertWideStringToString(dlg.m_strLongitude, longitude, sizeof(longitude));
		ConvertWideStringToString(dlg.m_strLatitude, latitude, sizeof(latitude));
		ConvertUnicode_to_UTF8(dlg.m_strAddress, address);

		agent_setLocation(longitude, latitude, address);

		m_strLongitude = dlg.m_strLongitude;
		m_strLatitude = dlg.m_strLatitude;
		m_strAddress = dlg.m_strAddress;
	
		str.Format(L"set location, longtitue=%s, latitude=%s, address=%s!\n", m_strLongitude, m_strLatitude, m_strAddress);
		AddRecvLog(str);
	}
}


void CginsidemfctestDlg::OnBnClickedButtonKwsSetmodelpath()
{
	CString		str;
	char		szDir[255];
	CDlgKwsModelPath	dlg;

	dlg.SetModelPath(m_szModelDir);
	if (dlg.DoModal() == IDOK)
	{
		ConvertWideStringToString(dlg.m_strModelPath, szDir, sizeof(szDir));
		if (kws_setModelPath(szDir) == KWS_ERR_SUCCESS)
		{
			str.Format(L"Successfully set the KWS model path, %s\n", dlg.m_strModelPath);
			strcpy(m_szModelDir, szDir);
		}
		else
		{
			str.Format(L"Failed to set the KWS model path, %s. Maybe not a directory or does not exist!\n", dlg.m_strModelPath);
		}
		AddRecvLog(str);
	}
}

void CginsidemfctestDlg::OnBnClickedButtonServiceloginstatus()
{
	char	szTemp[1024];
	TCHAR	TLog[1024];
	CString		str;

	std::string strRet = agent_serviceLoginStatus("geniemusic");
	if (strRet.size() > 0)
	{
		cJSON* cmdp_jsonObj = cJSON_Parse(strRet.c_str());
		if (cmdp_jsonObj == NULL)
		{
			sprintf(szTemp, "ERROR: agent_serviceLoginStatus returns %s\n", strRet.c_str());
		}
		else
		{
			cJSON* cmdp_rc = cJSON_GetObjectItem(cmdp_jsonObj, "rc");
			cJSON* cmdp_rcmsg = cJSON_GetObjectItem(cmdp_jsonObj, "rcmsg");
			if (cmdp_rc->valueint == 200)
			{
				sprintf(szTemp, "agent_serviceLoginStatus returned, [%s]\n\n", strRet.c_str());
				ConvertStringToWideString(szTemp, TLog);
				str = TLog;
				AddRecvLog(str);

				cJSON* cmdp_oauth_status = cJSON_GetObjectItem(cmdp_jsonObj, "oauth_status");
				if (cmdp_oauth_status != NULL && strcmp(cmdp_oauth_status->valuestring, "valid") == 0) { //oauth 인증 상태
					cJSON* cmdp_access_token = cJSON_GetObjectItem(cmdp_jsonObj, "access_token"); //oauth_status가 "valid"인 경우 전달
					cJSON* cmdp_user_name = cJSON_GetObjectItem(cmdp_jsonObj, "user_name"); //oauth_status가 "valid"인 경우 전달 
					sprintf(szTemp, "access_token=%s, user_name=%s\n", cmdp_access_token->valuestring, cmdp_user_name->valuestring);
				}
			}
			else {
				sprintf(szTemp, "ERROR: agent_serviceLoginStatus returns an error, return code=%d, msg=%s\n", cmdp_rc->valueint, cmdp_rcmsg->valuestring);
			}
			cJSON_Delete(cmdp_jsonObj);
		}
	}
	else {
		strcpy(szTemp, "ERROR: agent_serviceLoginStatus retrurned nothing\n");
	}

	ConvertStringToWideString(szTemp, TLog);
	str = TLog;
	AddRecvLog(str);
}


void CginsidemfctestDlg::OnBnClickedButtonServicelogout()
{
	char	szTemp[1024];
	TCHAR	TLog[1024];
	CString		str;

	std::string strRet = agent_serviceLogout("geniemusic");
	if (strRet.size() > 0)
	{
		cJSON* cmdp_jsonObj = cJSON_Parse(strRet.c_str());
		if (cmdp_jsonObj == NULL)
		{
			sprintf(szTemp, "ERROR: agent_serviceLogout returns %s\n", strRet.c_str());
		}
		else
		{
			cJSON* cmdp_rc = cJSON_GetObjectItem(cmdp_jsonObj, "rc");
			cJSON* cmdp_rcmsg = cJSON_GetObjectItem(cmdp_jsonObj, "rcmsg");
			sprintf(szTemp, "agent_serviceLogout returned, [%s]\n\n", strRet.c_str());
			cJSON_Delete(cmdp_jsonObj);
		}
	}
	else {
		strcpy(szTemp, "ERROR: agent_serviceLogout retrurned nothing\n");
	}

	ConvertStringToWideString(szTemp, TLog);
	str = TLog;
	AddRecvLog(str);
}
