// timeDlg.cpp : 实现文件
//
#include "stdafx.h"
#include "time.h"
#include "timeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CtimeDlg 对话框

CtimeDlg::CtimeDlg(CWnd* pParent /*=NULL*/)
: CDialog(CtimeDlg::IDD, pParent)
,m_bTop(false)
,m_bIsUTCTime(true)
,m_bIsCuted(false)
,m_nUTCTime(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
}

void CtimeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_INPUT, m_objEditInput);
	DDX_Control(pDX, IDC_EDIT_OUTPUT, m_objEditOutput);
	DDX_Control(pDX, IDC_EDIT_COPYINPUT_INFO, m_objEditCopyInputInfo);
	DDX_Control(pDX, IDC_EDIT_COPYOUTPUT_INFO, m_objEditCopyOutputInfo);
	DDX_Control(pDX, IDC_TOP, m_objButtonTop);
	DDX_Control(pDX, IDC_EDIT_SECOND_INFO, m_objCEdit_Second_Info);
	DDX_Control(pDX, IDC_CUT, m_objCButtonCut);
}

BEGIN_MESSAGE_MAP(CtimeDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_TOP, &CtimeDlg::OnBnClickedTop)
	ON_EN_CHANGE(IDC_EDIT_INPUT, &CtimeDlg::OnEnChangeInput)
	ON_EN_CHANGE(IDC_EDIT_OUTPUT, &CtimeDlg::OnEnChangeOutput)
	ON_BN_CLICKED(IDC_COPYINPUT, &CtimeDlg::OnBnClickedCopyinput)
	ON_BN_CLICKED(IDC_COPYOUTPUT, &CtimeDlg::OnBnClickedCopyoutput)
	ON_BN_CLICKED(IDC_PASTE_INPUT, &CtimeDlg::OnBnClickedPasteInput)
	ON_BN_CLICKED(IDC_PASTE_OUTPUT, &CtimeDlg::OnBnClickedPasteOutput)
	ON_BN_CLICKED(IDC_BUTTON_CLEAN_INPUT, &CtimeDlg::OnBnClickedButtonCleanInput)
	ON_BN_CLICKED(IDC_BUTTON_CLEAN_OUTPUT, &CtimeDlg::OnBnClickedButtonCleanOutput)
	ON_BN_CLICKED(IDC_CUT, &CtimeDlg::OnBnClickedCut)
	ON_WM_SYSCOMMAND()
	ON_BN_CLICKED(IDC_RADIO_UTC, &CtimeDlg::OnBnClickedRadioUTC)
	ON_BN_CLICKED(IDC_RADIO_CST, &CtimeDlg::OnBnClickedRadioCST)
END_MESSAGE_MAP()

// CtimeDlg 消息处理程序

BOOL CtimeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	
	m_FontInput.DeleteObject();
	m_FontInput.CreatePointFont(220, _T("新宋体"));
	m_objEditInput.SetFont(&m_FontInput);

	m_FontOutput.DeleteObject();
	m_FontOutput.CreatePointFont(156, _T("新宋体"));
	m_objEditOutput.SetFont(&m_FontOutput);

	m_objEditCopyInputInfo.SetWindowText(_T("已复制"));
	m_objEditCopyOutputInfo.SetWindowText(_T("已复制"));
	m_objCEdit_Second_Info.SetWindowText(_T("秒"));
	m_objCButtonCut.SetWindowText(_T("截取"));

	CButton* objRadio=(CButton*)GetDlgItem(IDC_RADIO_CST);
	objRadio->SetCheck(1);
	m_bIsUTCTime = false;
	m_nCleaned = false;

	CTime tm = CTime::GetCurrentTime();
	CString timeStamp;
	timeStamp.Format(_T("%lld"), tm.GetTime());
	m_objEditInput.SetWindowText(timeStamp);
	ChangeInputTime();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。
void CtimeDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CtimeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

bool CtimeDlg::getClipText(string& p_strText)
{
	if (OpenClipboard())
	{
		HANDLE hData = GetClipboardData(CF_TEXT);
		if (hData == NULL)
		{
			return false;
		}
		char * pszBuffer = (char*)GlobalLock(hData);
		p_strText = pszBuffer;
		GlobalUnlock(hData);
		CloseClipboard();
		return true;
	}
	return false;
}

void CtimeDlg::ChangeInputTime()
{
	CString strInput;
	CString strOutput;
	m_objEditInput.GetWindowText(strInput);

	if (strInput.IsEmpty())
	{
		//MessageBox(_T("不能为空！"));
		m_objEditOutput.SetWindowText(strOutput);
		return;
	}

	if (strInput.SpanIncluding(_T("-0123456789")) != strInput)
	{
		//MessageBox(_T("非法符号！"));
		m_objEditOutput.SetWindowText(strOutput);
		return;
	}
	
	if (strInput.GetLength() > 10)
	{
		strInput = strInput.Left(10);
		m_objCEdit_Second_Info.SetWindowText(_T("微秒"));
	}
	else
	{
		m_objCEdit_Second_Info.SetWindowText(_T("秒"));
	}

	m_nUTCTime = _ttoi(strInput);
	long nTime = m_nUTCTime;
	if (!m_bIsUTCTime)
	{
		nTime += 3600 * 8;
	}
	CTime objTime(nTime);
	strOutput = objTime.FormatGmt(_T("%Y-%m-%d %H:%M:%S"));
	m_objEditOutput.SetWindowText(strOutput);

	CEdit *pobjEditCopyInputInfo = (CEdit*)GetDlgItem(IDC_EDIT_COPYINPUT_INFO);
	pobjEditCopyInputInfo->ShowWindow(FALSE);
	CEdit *pobjEditCopyOutputInfo = (CEdit*)GetDlgItem(IDC_EDIT_COPYOUTPUT_INFO);
	pobjEditCopyOutputInfo->ShowWindow(FALSE);
}

void CtimeDlg::OnEnChangeInput()
{
	if (GetFocus() != GetDlgItem(IDC_EDIT_INPUT))
	{
		return;
	}

	m_nCleaned = false;

	ChangeInputTime();
}

void CtimeDlg::ChangeOutputTime()
{
	CString strInput;
	CString strOutput;
	m_objEditOutput.GetWindowText(strOutput);

	if (strOutput.IsEmpty())
	{
		//MessageBox(_T("不能为空！"));
		m_objEditInput.SetWindowText(strInput);
		return;
	}

	if (strOutput.SpanIncluding(_T(": -0123456789")) != strOutput)
	{
		//MessageBox(_T("非法符号！"));
		m_objEditInput.SetWindowText(strInput);
		return;
	}

	char p[128] = {0}; 
	::wsprintfA(p, "%ls", (LPCTSTR)strOutput);

	int nYear,nMouth,nDay,nHour,nMinute,nSecond;
	int nNum = sscanf(p, "%04d-%02d-%02d %02d:%02d:%02d", &nYear, &nMouth, &nDay, &nHour, &nMinute, &nSecond);
	if (nNum != 6)
	{
		m_objEditInput.SetWindowText(strInput);
		return;
	}

	//校验合法性
	if (nYear < 1900 || nYear > 2100)
	{
		m_objEditInput.SetWindowText(strInput);
		return;
	}
	if (nMouth < 0 || nMouth > 12)
	{
		m_objEditInput.SetWindowText(strInput);
		return;
	}
	if (nDay < 0 || nDay > 31)
	{
		m_objEditInput.SetWindowText(strInput);
		return;
	}
	if (nHour < 0 || nHour >= 24)
	{
		m_objEditInput.SetWindowText(strInput);
		return;
	}
	if (nMinute < 0 || nMinute >= 60)
	{
		m_objEditInput.SetWindowText(strInput);
		return;
	}
	if (nSecond < 0 || nSecond >= 60)
	{
		m_objEditInput.SetWindowText(strInput);
		return;
	}

	long nTime = mktime(nYear, nMouth, nDay, nHour, nMinute, nSecond);
	if (m_bIsUTCTime)
	{
		m_nUTCTime = nTime;
	}
	else
	{
		m_nUTCTime = nTime - 8 * 3600;
	}

	strInput.Format(_T("%ld"), m_nUTCTime);
	m_objEditInput.SetWindowText(strInput);
	m_objCEdit_Second_Info.SetWindowText(_T("秒"));

	CEdit *pobjEditCopyInputInfo = (CEdit*)GetDlgItem(IDC_EDIT_COPYINPUT_INFO);
	pobjEditCopyInputInfo->ShowWindow(FALSE);
	CEdit *pobjEditCopyOutputInfo = (CEdit*)GetDlgItem(IDC_EDIT_COPYOUTPUT_INFO);
	pobjEditCopyOutputInfo->ShowWindow(FALSE);
}

void CtimeDlg::OnEnChangeOutput()
{
	if (GetFocus() != GetDlgItem(IDC_EDIT_OUTPUT))
	{
		return;
	}

	m_nCleaned = false;

	ChangeOutputTime();
}

void CtimeDlg::OnBnClickedTop()
{
	if(m_bTop)
	{
		SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		m_bTop = false;
		m_objButtonTop.SetWindowText(_T("置顶"));
	}
	else
	{
		SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		m_bTop = true;
		m_objButtonTop.SetWindowText(_T("取消置顶"));
	}
}
unsigned long CtimeDlg::mktime(const unsigned int year0, const unsigned int mon0,
							   const unsigned int day,   const unsigned int hour,
							   const unsigned int min,   const unsigned int sec)
{
	unsigned int mon = mon0;
	unsigned int year = year0;

	/* 1..12 -> 11,12,1..10 */
	if (0 >= (int) (mon -= 2))
	{
		mon += 12;	/* Puts Feb last since it has leap day */
		year -= 1;
	}

	return ((((unsigned long)
		(year/4 - year/100 + year/400 + 367*mon/12 + day) +
		year*365 - 719499
		)*24 + hour /* now have hours */
		)*60 + min /* now have minutes */
		)*60 + sec; /* finally seconds */
}

void CtimeDlg::OnBnClickedCopyinput()
{
	string strInput;
	CString objstrInput;
	m_objEditInput.GetWindowText(objstrInput);
	if (objstrInput.IsEmpty())
	{
		return;
	}

	strInput = CStringA(objstrInput);
	SetClipText(strInput);

	CEdit *pobjEditCopyInputInfo = (CEdit*)GetDlgItem(IDC_EDIT_COPYINPUT_INFO);
	pobjEditCopyInputInfo->ShowWindow(TRUE);
	CEdit *pobjEditCopyOutputInfo = (CEdit*)GetDlgItem(IDC_EDIT_COPYOUTPUT_INFO);
	pobjEditCopyOutputInfo->ShowWindow(FALSE);
}

void CtimeDlg::OnBnClickedCopyoutput()
{
	string strOutput;
	CString objstrOutput;
	m_objEditOutput.GetWindowText(objstrOutput);
	if (objstrOutput.IsEmpty())
	{
		return;
	}

	strOutput = CStringA(objstrOutput);
	SetClipText(strOutput);

	CEdit *pobjEditCopyInputInfo = (CEdit*)GetDlgItem(IDC_EDIT_COPYINPUT_INFO);
	pobjEditCopyInputInfo->ShowWindow(FALSE);
	CEdit *pobjEditCopyOutputInfo = (CEdit*)GetDlgItem(IDC_EDIT_COPYOUTPUT_INFO);
	pobjEditCopyOutputInfo->ShowWindow(TRUE);
}

void CtimeDlg::SetClipText(string& p_strText)
{
	if(OpenClipboard())
	{
		HGLOBAL clipbuffer;
		char * buffer;
		EmptyClipboard();
		clipbuffer = GlobalAlloc(GMEM_DDESHARE, p_strText.length() + 1);
		buffer = (char*)GlobalLock(clipbuffer);
		strcpy(buffer, p_strText.c_str());
		GlobalUnlock(clipbuffer);
		SetClipboardData(CF_TEXT, clipbuffer);
		CloseClipboard();
	}
}

void CtimeDlg::OnBnClickedPasteInput()
{
	m_strBeforeCuted = "";
	m_bIsCuted = false;
	m_objCButtonCut.SetWindowText(_T("截取"));
	
	m_nCleaned = false;

	string strText;
	getClipText(strText);
	CString objStrText;
	objStrText.Format(_T("%s"), CStringW(strText.c_str()));
	m_objEditInput.SetFocus();
	m_objEditInput.SetWindowText(objStrText);
}

void CtimeDlg::OnBnClickedPasteOutput()
{
	m_strBeforeCuted = "";
	m_bIsCuted = false;
	m_objCButtonCut.SetWindowText(_T("截取"));

	m_nCleaned = false;

	string strText;
	getClipText(strText);
	CString objStrText;
	objStrText.Format(_T("%s"), CStringW(strText.c_str()));
	m_objEditOutput.SetFocus();
	m_objEditOutput.SetWindowText(objStrText);
}

void CtimeDlg::OnBnClickedButtonCleanInput()
{
	m_objEditInput.SetWindowText(_T(""));
	m_objEditOutput.SetWindowText(_T(""));
	m_strBeforeCuted = "";
	m_bIsCuted = false;
	m_objCButtonCut.SetWindowText(_T("截取"));
	m_nUTCTime = 0;
	m_nCleaned = true;
}

void CtimeDlg::OnBnClickedButtonCleanOutput()
{
	m_objEditInput.SetWindowText(_T(""));
	m_objEditOutput.SetWindowText(_T(""));
	m_strBeforeCuted = "";
	m_bIsCuted = false;
	m_objCButtonCut.SetWindowText(_T("截取"));
	m_nUTCTime = 0;
	m_nCleaned = true;
}

void CtimeDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID == SC_MAXIMIZE)
	{
		//MessageBox(_T("max"));
		return;
	}
	CDialog::OnSysCommand(nID, lParam);
}

void CtimeDlg::OnBnClickedCut()
{
	if (m_bIsCuted)
	{
		m_bIsCuted = false;
		m_objCButtonCut.SetWindowText(_T("截取"));

		if (m_strBeforeCuted.GetLength() > 10)
		{
			m_objEditInput.SetWindowText(m_strBeforeCuted);
			m_objCEdit_Second_Info.SetWindowText(_T("微秒"));
			OnEnChangeInput();
		}
	}
	else
	{
		m_bIsCuted = true;
		m_objCButtonCut.SetWindowText(_T("还原"));
		m_objEditInput.GetWindowText(m_strBeforeCuted);
		if (m_strBeforeCuted.GetLength() > 10)
		{
			CString strCuted = m_strBeforeCuted.Left(10);
			m_objEditInput.SetWindowText(strCuted);
			m_objCEdit_Second_Info.SetWindowText(_T("微秒"));
			OnEnChangeInput();
		}
	}
}

void CtimeDlg::OnBnClickedRadioUTC()
{
	m_bIsUTCTime = true;

	if (m_nCleaned)
	{
		return;
	}

	long nTime = m_nUTCTime;
	CTime objTime(nTime);
	CString strOutput = objTime.FormatGmt(_T("%Y-%m-%d %H:%M:%S"));
	m_objEditOutput.SetWindowText(strOutput);

	CEdit *pobjEditCopyInputInfo = (CEdit*)GetDlgItem(IDC_EDIT_COPYINPUT_INFO);
	pobjEditCopyInputInfo->ShowWindow(FALSE);
	CEdit *pobjEditCopyOutputInfo = (CEdit*)GetDlgItem(IDC_EDIT_COPYOUTPUT_INFO);
	pobjEditCopyOutputInfo->ShowWindow(FALSE);
}

void CtimeDlg::OnBnClickedRadioCST()
{
	m_bIsUTCTime = false;

	if (m_nCleaned)
	{
		return;
	}

	long nTime = m_nUTCTime + 8 * 3600;
	CTime objTime(nTime);
	CString strOutput = objTime.FormatGmt(_T("%Y-%m-%d %H:%M:%S"));
	m_objEditOutput.SetWindowText(strOutput);

	CEdit *pobjEditCopyInputInfo = (CEdit*)GetDlgItem(IDC_EDIT_COPYINPUT_INFO);
	pobjEditCopyInputInfo->ShowWindow(FALSE);
	CEdit *pobjEditCopyOutputInfo = (CEdit*)GetDlgItem(IDC_EDIT_COPYOUTPUT_INFO);
	pobjEditCopyOutputInfo->ShowWindow(FALSE);
}
