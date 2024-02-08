
// RunServerDlg.h: 헤더 파일
//

#pragma once

#include "stdAfx.h"

// CRunServerDlg 대화 상자
class CRunServerDlg : public CDialogEx
{
// 생성입니다.
public:
	CRunServerDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

	CSocServer m_socServer;
	CSocCom* m_socCom[MAX_CLIENT_COUNT];
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RUNSERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LPARAM OnAccept(WPARAM wParam, LPARAM lParam);
	afx_msg LPARAM OnReceive(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	CString m_strSend;
	CListBox m_list;
	CString m_strStatus;

	int best;
	int comp;

	std::list<int> m_using;
};
