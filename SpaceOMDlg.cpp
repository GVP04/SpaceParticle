
// SpaceOMDlg.cpp : implementation file
//

#include "pch.h"
#include "SpaceOM.h"
#include "SpaceOMDlg.h"
#include "afxdialogex.h"

#include "CSP_Group.h"
#include "CDrawDlg.h"
#include "CFindDlg.h"

//////////


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
public:
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CSpaceOMDlg dialog



CSpaceOMDlg::CSpaceOMDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SPACEOM_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_Calc_1.CalcParams = new char [SD_PARAMSLEN];
	
	m_Calc_1.Group = new CSP_Group;
	m_Calc_1.Group->m_pCalc = &m_Calc_1;
	m_Calc_1.Group->m_pCalcFind = &m_CalcFind_1;
}

CSpaceOMDlg::~CSpaceOMDlg()
{
	DeleteAll();
}

void CSpaceOMDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSpaceOMDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()

	ON_BN_CLICKED(IDC_BUTTON_STOPCALC, &CSpaceOMDlg::OnBnClicked_StopCalc)
	ON_BN_CLICKED(IDC_BUTTON_SHOWTRACE, &CSpaceOMDlg::OnBnClicked_ShowTrace)
	ON_BN_CLICKED(IDC_BUTTON_REFRESHTRACE, &CSpaceOMDlg::OnBnClicked_RefreshTrace)
	ON_BN_CLICKED(IDC_BUTTON_STARTFIND, &CSpaceOMDlg::OnBnClicked_StartFind)
	ON_BN_CLICKED(IDC_BUTTON_CALCSIMPLE, &CSpaceOMDlg::OnBnClicked_CalcSimple)
END_MESSAGE_MAP()


// CSpaceOMDlg message handlers

BOOL CSpaceOMDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	SetTimer(1, 1000, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSpaceOMDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSpaceOMDlg::OnPaint()
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
HCURSOR CSpaceOMDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CSpaceOMDlg::OnBnClicked_ShowTrace()
{
	if (m_Calc_1.DlgTrace == NULL)
	{
		m_Calc_1.DlgTrace = new CDrawDlg(&m_Calc_1);
		m_Calc_1.DlgTrace->Create(IDD_DIALOGDRAW, GetDesktopWindow());
	}
	m_Calc_1.DlgTrace->m_pData = &m_Calc_1;
	OnBnClicked_RefreshTrace();
}

void CSpaceOMDlg::OnBnClicked_CalcSimple()
{
	if (!m_Calc_1.Group)
	{
		m_Calc_1.Group = new CSP_Group;
		m_Calc_1.Group->m_pCalc = &m_Calc_1;
		m_Calc_1.Group->m_pCalcFind = &m_CalcFind_1;
	}


	if (!(m_Calc_1.Group->GetCalcState() & M_STATE_CALC))
	{
		*m_Calc_1.CalcParams = 0;
		::GetWindowTextA(::GetDlgItem(m_hWnd, IDC_EDIT1), m_Calc_1.CalcParams, SD_PARAMSLEN);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON_CALCSIMPLE), 0);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON_STOPCALC), 1);

		OnBnClicked_ShowTrace();

		AfxBeginThread(DoCalc_1, (LPVOID)&m_Calc_1);
	}
}

void CSpaceOMDlg::OnBnClicked_StopCalc()
{
	if (!m_Calc_1.Group)
	{
		m_Calc_1.Group = new CSP_Group;
		m_Calc_1.Group->m_pCalc = &m_Calc_1;
		m_Calc_1.Group->m_pCalcFind = &m_CalcFind_1;
	}

	if ((m_Calc_1.Group->GetCalcState() & M_STATE_CALC) == M_STATE_CALC)
	{
		m_Calc_1.Group->ClearCalcState(M_STATE_CALC);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON_CALCSIMPLE), 1);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON_STOPCALC), 0);
	}
}

void CSpaceOMDlg::DeleteAll()
{
	if (m_Calc_1.DlgTrace) delete m_Calc_1.DlgTrace;
	m_Calc_1.DlgTrace = NULL;

	if (m_Calc_1.Group) delete m_Calc_1.Group;
	m_Calc_1.Group = NULL;

	if (m_Calc_1.CalcParams) delete[] m_Calc_1.CalcParams;
	m_Calc_1.CalcParams = NULL;


	for (int i = 0; i < M_NCALCFIND; i++)
	{
		if (m_Calc_FIND[i].curCalc.DlgTrace) delete m_Calc_FIND[i].curCalc.DlgTrace;
		m_Calc_FIND[i].curCalc.DlgTrace = NULL;

		if (m_Calc_FIND[i].DlgFind) delete m_Calc_FIND[i].DlgFind;
		m_Calc_FIND[i].DlgFind = NULL;

		if (m_Calc_FIND[i].curCalc.Group) delete m_Calc_FIND[i].curCalc.Group;
		m_Calc_FIND[i].curCalc.Group = NULL;

		if (m_Calc_FIND[i].curCalc.CalcParams) delete m_Calc_FIND[i].curCalc.CalcParams;
		m_Calc_FIND[i].curCalc.CalcParams = NULL;

		if (m_Calc_FIND[i].FindParams) delete m_Calc_FIND[i].FindParams;
		m_Calc_FIND[i].FindParams = NULL;
	}
}

BOOL CSpaceOMDlg::DestroyWindow()
{
	// TODO: Add your specialized code here and/or call the base class

	DeleteAll();

	return CDialogEx::DestroyWindow();
}

void CSpaceOMDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case 1:
		EnterCriticalSection(&m_Calc_1.Group->m_cs);

		m_Calc_1.refresh.Cntr++;
		if ((m_Calc_1.Group->GetCalcState() & M_STATE_CHANGED) || 
			((m_Calc_1.Group->GetCalcState() & M_STATE_CALC) == M_STATE_CALC
				&& !(m_Calc_1.Group->GetCalcState() & M_STATE_BREAK)
				&& m_Calc_1.refresh.Delta > 0 && (m_Calc_1.refresh.Cntr % m_Calc_1.refresh.Delta) == 0))
		{
			m_Calc_1.Group->ClearCalcState(M_STATE_CHANGED);
			::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON_STOPCALC), (m_Calc_1.Group->GetCalcState() & M_STATE_CALC));
			::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON_CALCSIMPLE), !(m_Calc_1.Group->GetCalcState() & M_STATE_CALC));
			*m_Calc_1.CalcParams = 0;
			::GetWindowTextA(::GetDlgItem(m_hWnd, IDC_EDIT1), m_Calc_1.CalcParams, SD_PARAMSLEN);

			m_Calc_1.DlgTrace->ShowWindow(SW_SHOW);
			m_Calc_1.DlgTrace->RedrawMe();
		}
		LeaveCriticalSection(&m_Calc_1.Group->m_cs);
		break;
	default:
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CSpaceOMDlg::OnBnClicked_RefreshTrace()
{

	if (m_Calc_1.DlgTrace)
	{
		EnterCriticalSection(&m_Calc_1.Group->m_cs);
		*m_Calc_1.CalcParams = 0;
		::GetWindowTextA(::GetDlgItem(m_hWnd, IDC_EDIT1), m_Calc_1.CalcParams, SD_PARAMSLEN);

		m_Calc_1.Group->ReadSettingsGroup(m_Calc_1.CalcParams, SGROUP_STEP| SGROUP_TRACE, READPARAMS_NOT_USE_DEFS);

		LeaveCriticalSection(&m_Calc_1.Group->m_cs);

		m_Calc_1.DlgTrace->ShowWindow(SW_SHOW);
		m_Calc_1.DlgTrace->RedrawMe();
	}
}

void CSpaceOMDlg::OnBnClicked_StartFind()
{
	int i;

	for (i = 0; i < M_NCALCFIND && m_Calc_FIND[i].curCalc.Group; i++)
	{
		if (!(m_Calc_FIND[i].curCalc.Group->GetCalcState() & M_STATE_CALC))
		{
			if (m_Calc_FIND[i].FindParams) delete[] m_Calc_FIND[i].FindParams;
			m_Calc_FIND[i].FindParams = NULL;

			if (m_Calc_FIND[i].curCalc.CalcParams) delete[] m_Calc_FIND[i].curCalc.CalcParams;
			m_Calc_FIND[i].curCalc.CalcParams = NULL;

			if (m_Calc_FIND[i].DlgFind) delete m_Calc_FIND[i].DlgFind;
			m_Calc_FIND[i].DlgFind = NULL;

			if (m_Calc_FIND[i].curCalc.DlgTrace) delete m_Calc_FIND[i].curCalc.DlgTrace;
			m_Calc_FIND[i].curCalc.DlgTrace = NULL;

			if (m_Calc_FIND[i].curCalc.Group) delete m_Calc_FIND[i].curCalc.Group;
			m_Calc_FIND[i].curCalc.Group = NULL;

			m_Calc_FIND[i].curCalc.Stat.CLEAR();
			i--;
		}
	}

	if ( i < M_NCALCFIND)
	{
		m_Calc_FIND[i].FindParams = new char[SD_PARAMSLEN];
		*m_Calc_FIND[i].FindParams = 0;
		::GetWindowTextA(::GetDlgItem(m_hWnd, IDC_EDIT1), m_Calc_FIND[i].FindParams, SD_PARAMSLEN);

		m_Calc_FIND[i].curCalc.CalcParams = new char[SD_PARAMSLEN];
		*m_Calc_FIND[i].curCalc.CalcParams = 0;
		::GetWindowTextA(::GetDlgItem(m_hWnd, IDC_EDIT1), m_Calc_FIND[i].curCalc.CalcParams, SD_PARAMSLEN);

		m_Calc_FIND[i].curCalc.Group = new CSP_Group;
		m_Calc_FIND[i].curCalc.Group->m_pCalc = &m_Calc_FIND[i].curCalc;
		m_Calc_FIND[i].curCalc.Group->m_pCalcFind = &m_Calc_FIND[i];

		m_Calc_FIND[i].curCalc.Group->ReadSettingsGroup(m_Calc_FIND[i].curCalc.CalcParams, SGROUP_STEP | SGROUP_TRACE, READPARAMS_NOT_USE_DEFS);

		m_Calc_FIND[i].curCalc.DrawSet.iShowFlags |= SP_DRAW_TEXT | SP_DRAW_CLEARBKG | SP_DRAW_GROUPINFO;

		if (m_Calc_FIND[i].DlgFind == NULL)
		{
			m_Calc_FIND[i].DlgFind = new CFindDlg;
			m_Calc_FIND[i].DlgFind->Create(IDD_DIALOGFINDSP, GetDesktopWindow());
		}
		m_Calc_FIND[i].DlgFind->m_pData = &m_Calc_FIND[i];
		m_Calc_FIND[i].DlgFind->ShowWindow(SW_SHOW);

		AfxBeginThread(DoCalc_Find, (LPVOID)&m_Calc_FIND[i]);
	}
}

