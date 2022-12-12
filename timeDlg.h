// timeDlg.h : ͷ�ļ�
//
#pragma once

#include <string>
#include "afxwin.h"
using namespace std;

// CtimeDlg �Ի���
class CtimeDlg : public CDialog
{
// ����
public:
	CtimeDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TIME_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
	BOOL m_bIsUTCTime;				//�Ƿ�����CSTʱ��
	BOOL m_bIsCuted;				//�Ƿ�ü���
	CString m_strBeforeCuted;		//�ü�ǰ��ԭʼ����
	long m_nUTCTime;				//ϵͳ�����ڼ����ʱ��
	bool m_nCleaned;				//���������

private:
	void ChangeInputTime();
	void ChangeOutputTime();
public:
	CButton m_objCButtonCut;
};	
