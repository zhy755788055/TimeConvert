// timeDlg.cpp : 实现文件
//
#include "stdafx.h"
#include "time.h"
#include "timeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 查询标题栏各按钮位置（自命名，避免与 SDK 头文件冲突）
#ifndef WM_GETTITLEBARINFOEX_DEF
#define WM_GETTITLEBARINFOEX_DEF 0x033F
#endif
struct TITLEBARINFOEX_DEF
{
	DWORD cbSize;
	RECT  rcTitleBar;
	DWORD rgstate[6];
	RECT  rgrect[6];	// [0]标题栏 [1]保留 [2]最小化 [3]最大化 [4]关闭 [5]保留
};

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
	DDX_Control(pDX, IDC_EDIT_SECOND_INFO, m_objCEdit_Second_Info);
	DDX_Control(pDX, IDC_CUT, m_objCButtonCut);
}

BEGIN_MESSAGE_MAP(CtimeDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CURRENT, &CtimeDlg::OnBnClickedCurrent)
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
	ON_WM_DESTROY()
	ON_WM_WINDOWPOSCHANGED()
	ON_MESSAGE(WM_PIN_TOGGLE, &CtimeDlg::OnPinToggle)
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

	// 创建标题栏上的图钉覆盖层（位置在对话框显示/移动时同步）
	m_pinButton.Create(this);

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

// 点击“当前”按钮：把系统当前时间戳填入输入框
void CtimeDlg::OnBnClickedCurrent()
{
	m_strBeforeCuted = "";
	m_bIsCuted = false;
	m_objCButtonCut.SetWindowText(_T("截取"));
	m_nCleaned = false;

	CTime tm = CTime::GetCurrentTime();
	CString timeStamp;
	timeStamp.Format(_T("%lld"), tm.GetTime());
	m_objEditInput.SetWindowText(timeStamp);
	ChangeInputTime();
}

// 切换窗口置顶状态
void CtimeDlg::ToggleTopMost()
{
	if (m_bTop)
	{
		SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		m_bTop = false;
	}
	else
	{
		SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		m_bTop = true;
	}
	UpdatePinOverlay();
}

// 计算图钉按钮矩形（窗口坐标）：紧挨在系统“最小化”按钮左侧
CRect CtimeDlg::GetPinButtonRect()
{
	CRect rcWnd;
	GetWindowRect(&rcWnd);

	// 通过 WM_GETTITLEBARINFOEX 取得系统标题栏各按钮的精确位置
	TITLEBARINFOEX_DEF tbi;
	ZeroMemory(&tbi, sizeof(tbi));
	tbi.cbSize = sizeof(tbi);
	::SendMessage(m_hWnd, WM_GETTITLEBARINFOEX_DEF, 0, (LPARAM)&tbi);

	CRect rcMin(tbi.rgrect[2]);	// rgrect[2] 为最小化按钮（屏幕坐标）
	bool bValid = !rcMin.IsRectEmpty()
		&& rcMin.left >= rcWnd.left && rcMin.right <= rcWnd.right
		&& rcMin.top >= rcWnd.top && rcMin.bottom <= rcWnd.bottom;

	if (!bValid)
	{
		// 兜底：按系统度量从右往左推算（关闭、最大化、最小化）
		int nBtnW = GetSystemMetrics(SM_CXSIZE);
		int nBtnH = GetSystemMetrics(SM_CYSIZE);
		int nBorder = GetSystemMetrics(SM_CXDLGFRAME);
		int nRight = rcWnd.left + rcWnd.Width() - nBorder;
		int nTop = rcWnd.top + GetSystemMetrics(SM_CYDLGFRAME)
			+ (GetSystemMetrics(SM_CYCAPTION) - nBtnH) / 2;
		rcMin.SetRect(nRight - 3 * nBtnW, nTop, nRight - 2 * nBtnW, nTop + nBtnH);
	}

	rcMin.OffsetRect(-rcWnd.left, -rcWnd.top);

	CRect rcPin(rcMin.left - rcMin.Width(), rcMin.top, rcMin.left, rcMin.bottom);
	return rcPin;
}

// 把图钉覆盖层移动到系统“最小化”按钮左边
void CtimeDlg::UpdatePinOverlay()
{
	if (m_pinButton.GetSafeHwnd() == NULL)
	{
		return;
	}

	if (!IsWindowVisible() || IsIconic())
	{
		m_pinButton.ShowWindow(SW_HIDE);
		return;
	}

	CRect rcPin = GetPinButtonRect();	// 窗口坐标
	CRect rcWnd;
	GetWindowRect(&rcWnd);

	m_pinButton.SetPinned(m_bTop ? TRUE : FALSE);
	m_pinButton.SetWindowPos(NULL, rcWnd.left + rcPin.left, rcWnd.top + rcPin.top,
		rcPin.Width(), rcPin.Height(),
		SWP_NOACTIVATE | SWP_NOZORDER | SWP_SHOWWINDOW);
}

void CtimeDlg::OnDestroy()
{
	if (m_pinButton.GetSafeHwnd() != NULL)
	{
		m_pinButton.DestroyWindow();
	}
	CDialog::OnDestroy();
}

void CtimeDlg::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CDialog::OnWindowPosChanged(lpwndpos);
	UpdatePinOverlay();
}

LRESULT CtimeDlg::OnPinToggle(WPARAM wParam, LPARAM lParam)
{
	ToggleTopMost();
	return 0;
}

// ==================== 图钉覆盖层窗口 ====================

BEGIN_MESSAGE_MAP(CPinButtonWnd, CWnd)
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_SETCURSOR()
	ON_WM_WINDOWPOSCHANGED()
END_MESSAGE_MAP()

CPinButtonWnd::CPinButtonWnd()
	: m_bHover(FALSE)
	, m_bPinned(FALSE)
	, m_bPressed(FALSE)
{
}

BOOL CPinButtonWnd::Create(CWnd* pOwner)
{
	LPCTSTR lpszClass = AfxRegisterWndClass(0, ::LoadCursor(NULL, IDC_HAND), NULL, NULL);
	return CreateEx(WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
		lpszClass, _T("PinButton"), WS_POPUP, 0, 0, 16, 16,
		pOwner->GetSafeHwnd(), NULL);
}

void CPinButtonWnd::SetPinned(BOOL bPinned)
{
	if (m_bPinned != bPinned)
	{
		m_bPinned = bPinned;
		Render();
	}
}

void CPinButtonWnd::SetHover(BOOL bHover)
{
	if (m_bHover != bHover)
	{
		m_bHover = bHover;
		Render();
	}
}

// 用 GDI 把字形画到 32 位 DIB 上，再按灰度生成带透明通道的图层
void CPinButtonWnd::Render()
{
	if (m_hWnd == NULL)
	{
		return;
	}

	CRect rc;
	GetWindowRect(&rc);
	int nW = rc.Width();
	int nH = rc.Height();
	if (nW <= 0 || nH <= 0)
	{
		return;
	}

	BITMAPINFO bmi;
	ZeroMemory(&bmi, sizeof(bmi));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = nW;
	bmi.bmiHeader.biHeight = -nH;	// 负值表示自上而下
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	HDC hdcScreen = ::GetDC(NULL);
	HDC hdcMem = ::CreateCompatibleDC(hdcScreen);

	void* pBits = NULL;
	HBITMAP hBmp = ::CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
	HGDIOBJ hOldBmp = ::SelectObject(hdcMem, hBmp);
	::ZeroMemory(pBits, (size_t)nW * nH * 4);

	// 先用白色画出图钉字形，像素灰度值即为覆盖度
	int nFontH = nH * 55 / 100;
	if (nFontH < 8)
	{
		nFontH = 8;
	}
	CFont font;
	font.CreateFont(-nFontH, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, _T("Segoe MDL2 Assets"));

	// 未置顶用“图钉”，已置顶用“取消图钉”
	CString strGlyph;
	strGlyph = (wchar_t)(m_bPinned ? 0xE77A : 0xE840);

	HGDIOBJ hOldFont = ::SelectObject(hdcMem, (HFONT)font.GetSafeHandle());
	::SetBkMode(hdcMem, TRANSPARENT);
	::SetTextColor(hdcMem, RGB(255, 255, 255));
	RECT rcText = { 0, 0, nW, nH };
	::DrawText(hdcMem, strGlyph, -1, &rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	::SelectObject(hdcMem, hOldFont);

	// 悬停或已置顶时用主题蓝，否则跟随系统标题栏文字颜色
	COLORREF crGlyph = (m_bHover || m_bPinned || m_bPressed)
		? RGB(0, 120, 215)
		: ::GetSysColor(COLOR_CAPTIONTEXT);
	int nR = GetRValue(crGlyph);
	int nG = GetGValue(crGlyph);
	int nB = GetBValue(crGlyph);

	BYTE* p = (BYTE*)pBits;
	int nCount = nW * nH;
	for (int i = 0; i < nCount; ++i, p += 4)
	{
		int nCover = p[0];	// 覆盖度（灰度）
		if (nCover > 0)
		{
			p[0] = (BYTE)(nB * nCover / 255);	// B（预乘 alpha）
			p[1] = (BYTE)(nG * nCover / 255);	// G
			p[2] = (BYTE)(nR * nCover / 255);	// R
			p[3] = (BYTE)nCover;				// A
		}
		else
		{
			// alpha 置 1：整块按钮区域都能响应鼠标（alpha 为 0 会被系统穿透）
			p[3] = 1;
		}
	}

	POINT ptDst = { rc.left, rc.top };
	SIZE sizeWnd = { nW, nH };
	POINT ptSrc = { 0, 0 };
	BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
	::UpdateLayeredWindow(m_hWnd, hdcScreen, &ptDst, &sizeWnd, hdcMem, &ptSrc, 0, &bf, ULW_ALPHA);

	::SelectObject(hdcMem, hOldBmp);
	::DeleteObject(hBmp);
	::DeleteDC(hdcMem);
	::ReleaseDC(NULL, hdcScreen);
}

void CPinButtonWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	SetHover(TRUE);

	TRACKMOUSEEVENT tme;
	ZeroMemory(&tme, sizeof(tme));
	tme.cbSize = sizeof(tme);
	tme.dwFlags = TME_LEAVE;
	tme.hwndTrack = m_hWnd;
	::TrackMouseEvent(&tme);

	CWnd::OnMouseMove(nFlags, point);
}

void CPinButtonWnd::OnMouseLeave()
{
	SetHover(FALSE);
}

void CPinButtonWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bPressed = TRUE;
	SetCapture();
	Render();
	CWnd::OnLButtonDown(nFlags, point);
}

void CPinButtonWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bPressed)
	{
		m_bPressed = FALSE;
		if (GetCapture() == this)
		{
			ReleaseCapture();
		}

		CRect rc;
		GetClientRect(&rc);
		if (rc.PtInRect(point))
		{
			CWnd* pOwner = GetOwner();
			if (pOwner != NULL)
			{
				pOwner->PostMessage(WM_PIN_TOGGLE);
			}
		}
		else
		{
			Render();
		}
		return;
	}
	CWnd::OnLButtonUp(nFlags, point);
}

BOOL CPinButtonWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	::SetCursor(::LoadCursor(NULL, IDC_HAND));
	return TRUE;
}

void CPinButtonWnd::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CWnd::OnWindowPosChanged(lpwndpos);
	Render();
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
