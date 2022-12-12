// timeDlg.h : 头文件
//
#pragma once

#include <string>
#include "afxwin.h"
using namespace std;

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
	afx_msg void OnBnClickedTop();
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
	CButton m_objButtonTop;
	CEdit m_objCEdit_Second_Info;
	BOOL m_bIsUTCTime;				//是否点击了CST时间
	BOOL m_bIsCuted;				//是否裁剪过
	CString m_strBeforeCuted;		//裁剪前的原始数据
	long m_nUTCTime;				//系统中正在计算的时间
	bool m_nCleaned;				//点击过清理

private:
	void ChangeInputTime();
	void ChangeOutputTime();
public:
	CButton m_objCButtonCut;
};	
