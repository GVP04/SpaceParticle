
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

	m_Calc_1 = { 0 };
	m_Calc_1.maxRelativeSpeed = 1.0;
	m_Calc_1.maxAbsSpeed = 1.0;
	m_Calc_1.CalcParams = new char [SD_PARAMSLEN];
	m_Calc_1.DrawSet.iShowFlags = SP_SHOWFLAG_TEXT| SP_SHOWFLAG_TRACE | SP_SHOWFLAG_CLEARBKG;

	
	m_Calc_1.DrawSet.ShowItems = 0xFFFFFFFF;
	m_Calc_1.DrawSet.ShowLinksTo = 0xFFFFFFFF;
	m_Calc_1.DrawSet.ShowLinksFrom = 0x00000001;
	m_Calc_1.DrawSet.nLinks = 40;
	m_Calc_1.DrawSet.ViewMltpl = 1000.0;
	m_Calc_1.DrawSet.m_00_Pos.X = 500;
	m_Calc_1.DrawSet.m_00_Pos.Y = 500;
	m_Calc_1.DrawSet.m_00_Pos.Z = 500;
	m_Calc_1.DrawSet.BMP_Size.X = 5000;
	m_Calc_1.DrawSet.BMP_Size.Y = 5000;
	m_Calc_1.DrawSet.BMP_Size.Z = 5000;
	
	m_Calc_1.Group = new CSP_Group;
	m_Calc_1.Group->m_pCalc = &m_Calc_1;

	ZeroMemory(m_Calc_FIND, sizeof(SP_Calc) * M_NCALCFIND);
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
	ON_BN_CLICKED(IDC_BUTTON1, &CSpaceOMDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTONSTOPCALC, &CSpaceOMDlg::OnBnClickedButtonstopcalc)
	ON_BN_CLICKED(IDC_BUTTONSHOWTRACE, &CSpaceOMDlg::OnBnClickedButtonshowtrace)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTONSHOWTRACE2, &CSpaceOMDlg::OnBnClickedButtonshowtrace2)
	ON_BN_CLICKED(IDC_BUTTON2, &CSpaceOMDlg::OnBnClickedButton2)
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


void CSpaceOMDlg::OnBnClickedButtonshowtrace()
{
	if (m_Calc_1.DlgTrace == NULL)
	{
		m_Calc_1.DlgTrace = new CDrawDlg(&m_Calc_1);
		m_Calc_1.DlgTrace->Create(IDD_DIALOGDRAW, GetDesktopWindow());
	}
	m_Calc_1.DlgTrace->m_pData = &m_Calc_1;
	OnBnClickedButtonshowtrace2();
}

void CSpaceOMDlg::OnBnClickedButton1()
{
	if (!(m_Calc_1.State & M_STATE_FULL))
	{
		if (!m_Calc_1.Group) m_Calc_1.Group = new CSP_Group;
		m_Calc_1.Group->m_pCalc = &m_Calc_1;

		m_Calc_1.State = M_STATE_ON;
		*m_Calc_1.CalcParams = 0;
		::GetWindowTextA(::GetDlgItem(m_hWnd, IDC_EDIT1), m_Calc_1.CalcParams, SD_PARAMSLEN);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON1), 0);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTONSTOPCALC), 1);

		OnBnClickedButtonshowtrace();

		AfxBeginThread(DoCalc_1, (LPVOID)&m_Calc_1);

	}
}

void CSpaceOMDlg::OnBnClickedButtonstopcalc()
{
	if ((m_Calc_1.State & M_STATE_ON) == M_STATE_ON)
	{
		m_Calc_1.State ^= M_STATE_ON;
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON1), 1);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTONSTOPCALC), 0);
	}
}

void ReadCalcParam(char* in_Src, char* in_Mask, SPos& data, double defValue)
{
	char* pos = in_Src;
	int lenMask = (int)strlen(in_Mask);
	data.X = defValue;
	data.Y = defValue;
	data.Z = defValue;

	while ((pos = strstr(pos, in_Mask)))
	{
		pos++;
		char* posEQ = strstr(pos, "=");
		if (posEQ++)
		{
			if (pos[lenMask] == 'X') data.X = atof(posEQ);
			else	if (pos[lenMask] == 'Y') data.Y = atof(posEQ);
			else	if (pos[lenMask] == 'Z') data.Z = atof(posEQ);
		}
	}
}


void ReadCalcParam(char* in_Src, char* in_Mask, int& data, int defValue)
{
	char* pos = in_Src;
	int lenMask = (int)strlen(in_Mask);
	data = defValue;

	if ((pos = strstr(pos, in_Mask)))
	{
		pos++;
		char* posEQ = strstr(pos, "=");
		if (posEQ++)
			data = atoi(posEQ);
	}
}

void ReadCalcParam(char* in_Src, char* in_Mask, double& data, double defValue)
{
	char* pos = in_Src;
	int lenMask = (int)strlen(in_Mask);
	data = defValue;

	if ((pos = strstr(pos, in_Mask)))
	{
		pos++;
		char* posEQ = strstr(pos, "=");
		if (posEQ++)
			data = atof(posEQ);
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
		if ((m_Calc_1.State & M_STATE_CHANGED) || ((m_Calc_1.State & M_STATE_ON) == M_STATE_ON && m_Calc_1.refresh.Delta > 0 && (m_Calc_1.refresh.Cntr % m_Calc_1.refresh.Delta) == 0))
		{
			m_Calc_1.State &= (0xFFFFFFFF ^ M_STATE_CHANGED);
			::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTONSTOPCALC), (m_Calc_1.State & M_STATE_CALC));
			::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON1), !(m_Calc_1.State & M_STATE_CALC));
			*m_Calc_1.CalcParams = 0;
			::GetWindowTextA(::GetDlgItem(m_hWnd, IDC_EDIT1), m_Calc_1.CalcParams, SD_PARAMSLEN);

			ReadCalcParam(m_Calc_1.CalcParams, "TRACE_SHOWITEMS", m_Calc_1.DrawSet.ShowItems, 0xFFFFF);
			ReadCalcParam(m_Calc_1.CalcParams, "TRACE_SHOWLINKSFROM", m_Calc_1.DrawSet.ShowLinksFrom, 1);
			ReadCalcParam(m_Calc_1.CalcParams, "TRACE_SHOWLINKSTO", m_Calc_1.DrawSet.ShowLinksTo, 0xFFFFF);
			ReadCalcParam(m_Calc_1.CalcParams, "TRACE_LINKS", m_Calc_1.DrawSet.nLinks, 20);
			ReadCalcParam(m_Calc_1.CalcParams, "TRACE_REFRESH", m_Calc_1.refresh.Delta, 5);
			ReadCalcParam(m_Calc_1.CalcParams, "TIMESTEP", m_Calc_1.Time.BaseStep, 0.001);
			ReadCalcParam(m_Calc_1.CalcParams, "MAXNSTEPS", m_Calc_1.Group->m_maxCountLimit, 5000000);

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

void CSpaceOMDlg::OnBnClickedButtonshowtrace2()
{

	if (m_Calc_1.DlgTrace)
	{
		EnterCriticalSection(&m_Calc_1.Group->m_cs);
		*m_Calc_1.CalcParams = 0;
		::GetWindowTextA(::GetDlgItem(m_hWnd, IDC_EDIT1), m_Calc_1.CalcParams, SD_PARAMSLEN);

		SPos tmpSPos = { 0 };
		ReadCalcParam(m_Calc_1.CalcParams, "TRACE_00_POS", tmpSPos, 0.0);
		m_Calc_1.DlgTrace->Set00Point(tmpSPos);

		tmpSPos = { 0 };
		ReadCalcParam(m_Calc_1.CalcParams, "TRACE_WPOS", tmpSPos, 0.0);
		m_Calc_1.DlgTrace->SetWindowPos(tmpSPos);

		double tmpDbl = 10.0;
		ReadCalcParam(m_Calc_1.CalcParams, "TRACE_MAGN", tmpDbl, 5.0);
		m_Calc_1.DlgTrace->SetMltpl(tmpDbl);

		ReadCalcParam(m_Calc_1.CalcParams, "TRACE_SHOWITEMS", m_Calc_1.DrawSet.ShowItems, 0xFFFFF);
		ReadCalcParam(m_Calc_1.CalcParams, "TRACE_SHOWLINKSFROM", m_Calc_1.DrawSet.ShowLinksFrom, 1);
		ReadCalcParam(m_Calc_1.CalcParams, "TRACE_SHOWLINKSTO", m_Calc_1.DrawSet.ShowLinksTo, 0xFFFFF);
		ReadCalcParam(m_Calc_1.CalcParams, "TRACE_LINKS", m_Calc_1.DrawSet.nLinks, 20);
		ReadCalcParam(m_Calc_1.CalcParams, "TRACE_REFRESH", m_Calc_1.refresh.Delta, 5);
		ReadCalcParam(m_Calc_1.CalcParams, "TIMESTEP", m_Calc_1.Time.BaseStep, 0.001);
		ReadCalcParam(m_Calc_1.CalcParams, "MAXNSTEPS", m_Calc_1.Group->m_maxCountLimit, 5000000);

		LeaveCriticalSection(&m_Calc_1.Group->m_cs);

		m_Calc_1.DlgTrace->ShowWindow(SW_SHOW);
		m_Calc_1.DlgTrace->RedrawMe();
	}
}

void CSpaceOMDlg::OnBnClickedButton2()
{
	int i;

	for (i = 0; i < M_NCALCFIND && m_Calc_FIND[i].curCalc.Group; i++)
	{
		if (!m_Calc_FIND[i].FindState)
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

			m_Calc_FIND[i].curCalc.Stat = { 0 };
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
		m_Calc_FIND[i].curCalc.Group->m_pCalcFind = &m_Calc_FIND[i];
		m_Calc_FIND[i].curCalc.Group->m_pCalc = &m_Calc_FIND[i].curCalc;

		ReadCalcParam(m_Calc_FIND[i].curCalc.CalcParams, "TRACE_SHOWITEMS", m_Calc_FIND[i].curCalc.DrawSet.ShowItems, 0xFFFFF);
		ReadCalcParam(m_Calc_FIND[i].curCalc.CalcParams, "TRACE_SHOWLINKSFROM", m_Calc_FIND[i].curCalc.DrawSet.ShowLinksFrom, 1);
		ReadCalcParam(m_Calc_FIND[i].curCalc.CalcParams, "TRACE_SHOWLINKSTO", m_Calc_FIND[i].curCalc.DrawSet.ShowLinksTo, 0xFFFFF);
		ReadCalcParam(m_Calc_FIND[i].curCalc.CalcParams, "TRACE_LINKS", m_Calc_FIND[i].curCalc.DrawSet.nLinks, 20);

		m_Calc_FIND[i].curCalc.DrawSet.iShowFlags |= SP_SHOWFLAG_TEXT | SP_SHOWFLAG_TRACE | SP_SHOWFLAG_CLEARBKG;

		if (m_Calc_FIND[i].DlgFind == NULL)
		{
			m_Calc_FIND[i].DlgFind = new CFindDlg;
			m_Calc_FIND[i].DlgFind->Create(IDD_DIALOGFINDSP, GetDesktopWindow());
		}
		m_Calc_FIND[i].DlgFind->m_pData = &m_Calc_FIND[i];

		m_Calc_FIND[i].FindState = M_STATE_ON;


		m_Calc_FIND[i].DlgFind->ShowWindow(SW_SHOW);

		AfxBeginThread(DoCalc_Find, (LPVOID)&m_Calc_FIND[i]);
	}
}

