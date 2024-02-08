
// RunServerDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "RunServer.h"
#include "RunServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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


// CRunServerDlg 대화 상자



CRunServerDlg::CRunServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_RUNSERVER_DIALOG, pParent)
	, m_strSend(_T(""))
	, m_strStatus(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	best = 0;
	comp = 0;
}

void CRunServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
}

BEGIN_MESSAGE_MAP(CRunServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(UM_ACCEPT, OnAccept)
	ON_MESSAGE(UM_RECEIVE, OnReceive)
END_MESSAGE_MAP()


// CRunServerDlg 메시지 처리기

BOOL CRunServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	for (int i = 0; i < MAX_CLIENT_COUNT; i++) {
		m_socServer.m_index.push_back(i);
	}

	m_socServer.Create(5000);
	m_socServer.Listen();
	m_socServer.Init(this->m_hWnd);
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CRunServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CRunServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CRunServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LPARAM CRunServerDlg::OnAccept(WPARAM wParam, LPARAM lParam)
{
	try {
		//통신용 소켓을 생성한 뒤
		int tmp = m_socServer.m_index.front();

		CString number;
		number.Format(L"%d", tmp);

		m_socCom[tmp] = new CSocCom();
		// 서버소켓과 통신소켓을 연결한다.
		m_socCom[tmp] = m_socServer.GetAcceptSocCom();

		m_socServer.m_index.pop_front();
		m_using.push_back(tmp);

		m_socCom[tmp]->m_index = tmp;
		m_socCom[tmp]->Init(this->m_hWnd);

		//클라이언트에게 연결 성공 메시지를 보낼때 클라이언트 번호도 같이 보냄
		char message[50]; //필요에 따라 버퍼 크기를 조정하십시오.
		sprintf_s(message, "%d%d", SOC_CLIENT_CONNECT, number);
		m_socCom[tmp]->Send(message, 256);
	}
	catch (CException* ex) {
		ex->ReportError();
	}
	UpdateData(TRUE);
	TCHAR pTmp[256];
	CString strTmp;
	strTmp.Format(L"%d", best);
	memset(pTmp, '\0', 256);
	wcscat_s(pTmp, strTmp);
	strTmp = "";
	for each (int i in m_using) {
		m_socCom[i]->Send(pTmp, 256);
	}
	UpdateData(FALSE);
	return TRUE;
}

LPARAM CRunServerDlg::OnReceive(WPARAM wParam, LPARAM lParam)
{
	TCHAR pTmp[256];
	CString strTmp;
	memset(pTmp, '\0', 256);

	m_socCom[wParam]->Receive(pTmp, 256); // wParam = 클라이언트 번호
	strTmp.Format(L"%s", pTmp);

	if (strTmp == "DISCONNECT") {
		m_socServer.m_socCom[wParam].Close();
		m_socCom[wParam]->Close();
		m_socServer.m_index.push_back(wParam);
		m_using.erase(std::remove(m_using.begin(), m_using.end(), wParam), m_using.end());
		int i = m_list.GetCount();
		m_list.InsertString(i, L"접속종료");
	}
	else {
		//리스트박스에 보여준다.
		CString id;
		id.Format(L"%d", wParam);

		CString sendId;
		sendId.Format(strTmp);
		int i = m_list.GetCount();
		m_list.InsertString(i, sendId);
		
		//클라이언트가 보낸 기록과 서버의 현재 최고기록을 비교해서 클라이언트들에게 보냄
		comp = _ttoi(sendId);
		if (best < comp)
			best = comp;
		CString str;
		str.Format(L"%d", best);
		for each (int a in m_using) {
			m_socCom[a]->Send(str, 256);
		}
	}
	return TRUE;
}

