// timeDlg.h : 头文件
//
#pragma once

#include <string>
#include "afxwin.h"
using namespace std;

// 图钉按钮被点击时发给对话框的通知消息
#define WM_PIN_TOGGLE   (WM_APP + 101)

// 标题栏图钉按钮：一个透明的分层窗口，浮在系统标题栏上（最小化按钮左边）
class CPinButtonWnd : public CWnd
{
public:
	CPinButtonWnd();
	BOOL Create(CWnd* pOwner);
	void SetPinned(BOOL bPinned);
	void SetHover(BOOL bHover);
	void Render();						// 按当前状态重建带透明通道的图层

protected:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	DECLARE_MESSAGE_MAP()

private:
	BOOL m_bHover;
	BOOL m_bPinned;
	BOOL m_bPressed;
};

// CtimeDlg 对话框
class CtimeDlg : public CDialog
{
// 构造
public:
	CtimeDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_TIME_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnEnChangeInput();
	afx_msg void OnBnClickedCurrent();
	afx_msg void OnEnChangeOutput();
	afx_msg void OnBnClickedCopyinput();
	afx_msg void OnBnClickedCopyoutput();
	afx_msg void OnBnClickedPasteInput();
	afx_msg void OnBnClickedPasteOutput();
	afx_msg void OnBnClickedButtonCleanInput();
	afx_msg void OnBnClickedButtonCleanOutput();
	afx_msg void OnBnClickedCut();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnBnClickedRadioUTC();
	afx_msg void OnBnClickedRadioCST();
	// 标题栏图钉按钮
	afx_msg void OnDestroy();
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg LRESULT OnPinToggle(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

private:
	bool getClipText(string& p_strText);
	void SetClipText(string& p_strText);
	unsigned long mktime(const unsigned int year0, const unsigned int mon0, const unsigned int day, const unsigned int hour, const unsigned int min, const unsigned int sec);

public:
	bool m_bTop;
	CEdit m_objEditInput;
	CEdit m_objEditOutput;
	CEdit m_objEditCopyInputInfo;
	CEdit m_objEditCopyOutputInfo;
	CFont m_FontInput;
	CFont m_FontOutput;
	CEdit m_objCEdit_Second_Info;
	CPinButtonWnd m_pinButton;		//标题栏上的图钉覆盖层
	BOOL m_bIsUTCTime;				//是否点击了CST时间
	BOOL m_bIsCuted;				//是否裁剪过
	CString m_strBeforeCuted;		//裁剪前的原始数据
	long m_nUTCTime;				//系统中正在计算的时间
	bool m_nCleaned;				//点击过清理

private:
	void ChangeInputTime();
	void ChangeOutputTime();
	// 标题栏图钉按钮
	CRect GetPinButtonRect();
	void UpdatePinOverlay();
	void ToggleTopMost();
public:
	CButton m_objCButtonCut;
};	
