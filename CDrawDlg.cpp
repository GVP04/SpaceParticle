// CDrawDlg.cpp : implementation file
//

#include "pch.h"
#include "SpaceOM.h"
#include "afxdialogex.h"
#include "CDrawDlg.h"
#include "CSpParticle.h"
#include "CSP_Group.h"
#include "CSDataArray.h"


// CDrawDlg dialog

IMPLEMENT_DYNAMIC(CDrawDlg, CDialogEx)

CDrawDlg::CDrawDlg(SP_Calc* in_pData, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOGDRAW, pParent)
{
    m_pData = in_pData;
	m_hDCBitmap = NULL;
	m_ViewMltpl = 0.1;
	m_ViewPrc = 100.0;
	m_ViewSpread = 100.0;

	m_WindowPos = { 0 };

	m_uRedrawFlag = 1;

	m_XYZ = SP_DRAW_XY;
}

CDrawDlg::~CDrawDlg()
{
	if (m_hDCBitmap) DeleteObject(m_hDCBitmap);
	m_hDCBitmap = NULL;
}

void CDrawDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDERMGNF, m_SliderMgnf);
	DDX_Control(pDX, IDC_SLIDERVIEWSPREAD, m_SliderSpread);
	DDX_Control(pDX, IDC_SLIDERVIEWPRC, m_SliderViewPrc);
}


BEGIN_MESSAGE_MAP(CDrawDlg, CDialogEx)
	ON_WM_PAINT()
    ON_BN_CLICKED(IDCANCEL, &CDrawDlg::OnBnClickedCancel)
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDERMGNF, &CDrawDlg::OnReleasedcaptureSlidermgnf)
	ON_NOTIFY(TRBN_THUMBPOSCHANGING, IDC_SLIDERMGNF, &CDrawDlg::OnThumbposchangingSlidermgnf)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CDrawDlg::OnBnClicked_Refresh)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CDrawDlg::OnBnClicked_Save)
	ON_BN_CLICKED(IDC_BUTTON_UPLEFT, &CDrawDlg::OnBnClicked_UpLeft)
	ON_BN_CLICKED(IDC_BUTTON_CENTER, &CDrawDlg::OnBnClicked_Center)
	ON_BN_CLICKED(IDC_BUTTON_XYZ, &CDrawDlg::OnBnClickedButtonXyz)
END_MESSAGE_MAP()


// CDrawDlg message handlers


void CDrawDlg::OnPaint()
{
	CPaintDC dc(this);

	if (m_pData)
	{
		HBITMAP hOldBitmap;       // Handle to the old bitmap

		// Create a memory device context compatible with the device. 
		CRect re;
		dc.GetClipBox(&re);
		CDC DCMem;
		DCMem.CreateCompatibleDC(&dc);
		if (m_hDCBitmap == NULL)
		{
			///////////CRect ClientRe;
			///////////GetClientRect(&ClientRe);
			//////////m_hDCBitmap = ::CreateCompatibleBitmap(dc.m_hDC, ClientRe.Width(), ClientRe.Height());
			m_hDCBitmap = ::CreateCompatibleBitmap(dc.m_hDC, (int)m_pData->DrawSet.BMP_Size.X, (int)m_pData->DrawSet.BMP_Size.Y);
			m_uRedrawFlag = 1;
			///////////////	   ::BitBlt (DCMem.m_hDC, re.left,re.top,re.Width(),re.Height(), dc.m_hDC , re.left,re.top, SRCCOPY);
		}

		// Select the new bitmap object into the memory device context. 
		hOldBitmap = (HBITMAP)DCMem.SelectObject(m_hDCBitmap);
		if (m_uRedrawFlag)
		{

			//SPos tmp = { 0 };
			//PaintTraceToCDC(&DCMem, m_pData, SP_DRAW_XY, tmp);

			HDC hDC = DCMem.m_hDC;

			SDrawSettings oldDrawSet = m_pData->DrawSet;

			//m_pData->DrawSet.iShowFlags = SP_SHOWFLAG_TRACE;

			SPos TraceSize = { 0 };
			int BMP_Size = (int) (m_pData->DrawSet.BMP_Size.X > m_pData->DrawSet.BMP_Size.Y ? m_pData->DrawSet.BMP_Size.Y : m_pData->DrawSet.BMP_Size.X);

			m_pData->Group->SetViewPoint(m_pData->DrawSet.ViewPoint.TimeInPc, m_pData->DrawSet.ViewPoint.Spread);
			m_pData->Group->GetMinMaxTracePos(m_pData->DrawSet.PosMin, m_pData->DrawSet.PosMax, m_pData->DrawSet.ViewPoint.StartIdx, m_pData->DrawSet.ViewPoint.EndIdx);

			TraceSize.SET_AS_DELTA(m_pData->DrawSet.PosMax, m_pData->DrawSet.PosMin);

			if (TraceSize.X > TraceSize.Y && TraceSize.X > TraceSize.Z)
				m_pData->DrawSet.ViewMltpl = BMP_Size / TraceSize.X;
			else
				if (TraceSize.Y > TraceSize.X && TraceSize.Y > TraceSize.Z)
					m_pData->DrawSet.ViewMltpl = BMP_Size / TraceSize.Y;
				else
					m_pData->DrawSet.ViewMltpl = BMP_Size / TraceSize.Z;

			m_pData->DrawSet.ViewMltpl *= 0.98;

			//m_pData->DrawSet.m_00_Pos = m_pData->DrawSet.BMP_Size;
			//m_pData->DrawSet.m_00_Pos.MULT(0.5);

			m_pData->DrawSet.m_00_Pos = { 0 };

			SPos delta00 = { 0 };

			if (m_pData->DrawSet.BMP_Size.X > m_pData->DrawSet.BMP_Size.Y)
				delta00.X = m_pData->DrawSet.BMP_Size.X - m_pData->DrawSet.BMP_Size.Y;
			else
				delta00.Y = m_pData->DrawSet.BMP_Size.Y - m_pData->DrawSet.BMP_Size.X;


			m_pData->DrawSet.m_00_Pos.X = (m_pData->DrawSet.BMP_Size.X - (m_pData->DrawSet.PosMax.X - m_pData->DrawSet.PosMin.X) * m_pData->DrawSet.ViewMltpl * m_ViewMltpl) * 0.5 - m_pData->DrawSet.PosMin.X * m_pData->DrawSet.ViewMltpl * m_ViewMltpl;
			m_pData->DrawSet.m_00_Pos.Y = (m_pData->DrawSet.BMP_Size.Y - (m_pData->DrawSet.PosMax.Y - m_pData->DrawSet.PosMin.Y) * m_pData->DrawSet.ViewMltpl * m_ViewMltpl) * 0.5 - m_pData->DrawSet.PosMin.Y * m_pData->DrawSet.ViewMltpl * m_ViewMltpl;

			m_pData->DrawSet.ViewMltpl *= m_ViewMltpl;
			
			PaintTraceToCDC(&DCMem, m_pData, m_XYZ | SP_DRAW_CLEARBKG | SP_DRAW_TEXT, delta00);

			m_pData->DrawSet = oldDrawSet;

			m_uRedrawFlag = 0;
		}

		::BitBlt(dc.m_hDC, re.left, re.top, re.Width(), re.Height(), DCMem.m_hDC, re.left + (int)m_WindowPos.X, re.top + (int)m_WindowPos.Y, SRCCOPY);

		// Select the old bitmap back into the device context.
		DCMem.SelectObject(hOldBitmap);
	}
}



void CDrawDlg::OnBnClickedCancel()
{
    // TODO: Add your control notification handler code here
    CDialogEx::OnCancel();
}

void GetMgnPos(int* out_Pos, double* out_Mgnf, double in_Data, int in_FullRange)
{
	double mgnfRange[8] = {  0.000000001, 0.001, 0.01, 0.1, 1.0, 10.0, 100.0, 1000.0 };
	int iPos;
	int dStep = in_FullRange / 7;

	if (out_Pos)
	{
		if (in_Data <= mgnfRange[0]) *out_Pos = 0;
		else if (in_Data >= mgnfRange[7]) *out_Pos = in_FullRange;
		else
		{
			for (iPos = 1; iPos < 6 && in_Data > mgnfRange[iPos]; iPos++);

			*out_Pos = (int) ((iPos - 1) * dStep + dStep * (in_Data - mgnfRange[iPos-1]) / (mgnfRange[iPos] - mgnfRange[iPos-1]));
		}
	}
	else if (out_Mgnf)
	{
		int PosIn = (int)in_Data;
		if (PosIn < 0) *out_Mgnf = mgnfRange[0];
		else if (PosIn > 140) *out_Mgnf = mgnfRange[7];
		else
		{
			int idxTmp = PosIn / dStep;
			*out_Mgnf = mgnfRange[idxTmp] + (mgnfRange[idxTmp + 1] - mgnfRange[idxTmp]) * ((double)(PosIn % dStep)) / (double)dStep;
		}
	}
}

void GetPrcPos(int* out_Pos, double* out_Mgnf, double in_Data, int in_FullRange)
{
	double mgnfRange[8] = { 0.000001, 0.01, 0.1, 1.01, 10.0, 20.0, 50.0, 100.0 };
	int iPos;
	int dStep = in_FullRange / 7;

	if (out_Pos)
	{
		if (in_Data <= mgnfRange[0]) *out_Pos = 0;
		else if (in_Data >= mgnfRange[7]) *out_Pos = in_FullRange;
		else
		{
			for (iPos = 1; iPos < 6 && in_Data > mgnfRange[iPos]; iPos++);

			*out_Pos = (int)((iPos - 1) * dStep + dStep * (in_Data - mgnfRange[iPos - 1]) / (mgnfRange[iPos] - mgnfRange[iPos - 1]));
		}
	}
	else if (out_Mgnf)
	{
		int PosIn = (int)in_Data;
		if (PosIn < 0) *out_Mgnf = mgnfRange[0];
		else if (PosIn > 140) *out_Mgnf = mgnfRange[7];
		else
		{
			int idxTmp = PosIn / dStep;
			*out_Mgnf = mgnfRange[idxTmp] + (mgnfRange[idxTmp + 1] - mgnfRange[idxTmp]) * ((double)(PosIn % dStep)) / (double)dStep;
		}
	}
}


double CDrawDlg::SetMltpl(double in_Mltpl)
{
	double ret = m_ViewMltpl;

	m_ViewMltpl = in_Mltpl;

	m_SliderMgnf.SetRange(0, 140);

	int posMgn;
	GetMgnPos(&posMgn, NULL, m_ViewMltpl, 140);
	m_SliderMgnf.SetPos(posMgn);

	return ret;
}


double CDrawDlg::SetViewPrc(double in_Prc)
{
	double ret = m_ViewPrc;
	m_ViewPrc = in_Prc;

	if (m_ViewPrc < 0.0 || m_ViewPrc > 100.0) m_ViewPrc = 100.0;
	m_SliderViewPrc.SetRange(0, 140);

	int posMgn;
	GetPrcPos(&posMgn, NULL, m_ViewPrc, 140);
	m_SliderViewPrc.SetPos(posMgn);

	return ret;
}


double CDrawDlg::SetViewSpread(double in_Mlin_Spreadtpl)
{
	double ret = m_ViewSpread;
	m_ViewSpread = in_Mlin_Spreadtpl;
	if (m_ViewSpread < 0.0 || m_ViewSpread > 100.0) m_ViewSpread = 100.0;

	m_SliderSpread.SetRange(0, 140);

	int posMgn;
	GetPrcPos(&posMgn, NULL, m_ViewSpread, 140);
	m_SliderSpread.SetPos(posMgn);

	return ret;
}


void CDrawDlg::Set00Point(SPos &in_00, bool bRedraw)
{
	m_pData->DrawSet.m_00_Pos = in_00;

	if (bRedraw) RedrawMe();
}

void CDrawDlg::SetWindowPos(SPos & in_WPos, bool bRedraw)
{

	SCROLLINFO si = { 0 };
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_POS;

	m_WindowPos.Y = in_WPos.Y;
	if (m_WindowPos.Y < 0)	m_WindowPos.Y = 0;
	if (m_WindowPos.Y > m_pData->DrawSet.BMP_Size.Y)	m_WindowPos.Y = m_pData->DrawSet.BMP_Size.Y - 200;

	si.nPos = (int)m_WindowPos.Y;
	SetScrollInfo(SB_VERT, &si, TRUE);

	m_WindowPos.X = in_WPos.X;
	if (m_WindowPos.X < 0)	m_WindowPos.X = 0;
	if (m_WindowPos.X > m_pData->DrawSet.BMP_Size.X)	m_WindowPos.X = m_pData->DrawSet.BMP_Size.X - 200;

	si.nPos = (int)m_WindowPos.X;
	SetScrollInfo(SB_HORZ, &si, TRUE);

	if (bRedraw) Invalidate();
}

BOOL CDrawDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SCROLLINFO si = { 0 };
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL;
	si.nMin = 0;
	si.nMax = (int)m_pData->DrawSet.BMP_Size.X;
	si.nPage = 200;
	si.nTrackPos = 0;
	SetScrollInfo(SB_HORZ, &si, TRUE);

	si.nMax = (int)m_pData->DrawSet.BMP_Size.Y;
	SetScrollInfo(SB_VERT, &si, TRUE);

	OnBnClicked_Center();

	SetMltpl(1.0);
	SetViewPrc(m_pData->DrawSet.ViewPoint.TimeInPc);
	SetViewSpread(m_pData->DrawSet.ViewPoint.Spread);


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDrawDlg::RedrawMe()
{
	m_uRedrawFlag = 1;
	Invalidate();
	UpdateWindow();
}

void CDrawDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	
	SCROLLINFO si = { 0 };
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL;
	si.nMin = 0;
	si.nMax = (int)m_pData->DrawSet.BMP_Size.X;
	si.nPage = cx;
	si.nPos = (int)m_WindowPos.X;

	SetScrollInfo(SB_HORZ, &si, TRUE);

	si.nPage = cy;
	si.nMax = (int)m_pData->DrawSet.BMP_Size.Y;

	SetScrollInfo(SB_VERT, &si, TRUE);

	// TODO: Add your message handler code here
}


void CDrawDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int WndId = 0;
	if (pScrollBar) WndId = GetWindowLong(pScrollBar->m_hWnd, GWL_ID);

	if (WndId == 0)
	{
		SCROLLINFO si = { 0 };
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_POS;
		if (pScrollBar) si.nPos = 0;
		switch (nSBCode)
		{
		case SB_PAGEUP:			m_WindowPos.Y -= 400; break;
		case SB_PAGEDOWN:		m_WindowPos.Y += 400; break;
		case SB_LINEUP:			m_WindowPos.Y -= 20; break;
		case SB_LINEDOWN:		m_WindowPos.Y += 20; break;
		case SB_THUMBPOSITION:	m_WindowPos.Y = nPos; break;
		case SB_ENDSCROLL:		break;
		case SB_TOP:			m_WindowPos.Y = 0; break;
		case SB_BOTTOM:			m_WindowPos.Y = nPos - 200; break;
		case SB_THUMBTRACK:		m_WindowPos.Y = nPos; break;
		default:
			nPos = nPos;
			break;
		}
		if (m_WindowPos.Y < 0)
			m_WindowPos.Y = 0;
		if (m_WindowPos.Y > m_pData->DrawSet.BMP_Size.Y)
			m_WindowPos.Y = m_pData->DrawSet.BMP_Size.Y - 200;

		si.nPos = (int)m_WindowPos.Y;

		SetScrollInfo(SB_VERT, &si, TRUE);
		Invalidate();
	}
	else if (WndId == IDC_SLIDERMGNF)
	{
		bool bRedraw = false;
		nPos = m_SliderMgnf.GetPos();

		int PosValue = m_SliderMgnf.GetPos();
		GetMgnPos(&PosValue, NULL, m_ViewMltpl, 140);
		if (nPos != PosValue)
			bRedraw = true;
		switch (nSBCode)
		{
		case SB_PAGEUP:				break;
		case SB_PAGEDOWN:			break;
		case SB_LINEUP:				break;
		case SB_LINEDOWN:			break;
		case SB_THUMBPOSITION:		break;
		case SB_ENDSCROLL:			break;
		case SB_TOP:				break;
		case SB_BOTTOM:				break;
		case SB_THUMBTRACK:			bRedraw = false;	break;
		default:		break;
		}
		if (bRedraw)
		{
			GetMgnPos(NULL, &m_ViewMltpl, nPos, 140);
			RedrawMe();
		}
	}

	//CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CDrawDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int WndId = 0;
	if (pScrollBar) WndId = GetWindowLong(pScrollBar->m_hWnd, GWL_ID);

	if (WndId == 0)
	{
		SCROLLINFO si = { 0 };
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_POS;

		switch (nSBCode)
		{
		case SB_PAGEUP:			m_WindowPos.X -= 400; break;
		case SB_PAGEDOWN:		m_WindowPos.X += 400; break;
		case SB_LINEUP:			m_WindowPos.X -= 20; break;
		case SB_LINEDOWN:		m_WindowPos.X += 20; break;
		case SB_THUMBPOSITION:	m_WindowPos.X = nPos; break;
		case SB_ENDSCROLL:		break;
		case SB_TOP:			m_WindowPos.X = 0; break;
		case SB_BOTTOM:			m_WindowPos.X = nPos - 200; break;
		case SB_THUMBTRACK:		m_WindowPos.X = nPos; break;
		default:
			nPos = nPos;
			break;
		}
		if (m_WindowPos.X < 0)
			m_WindowPos.X = 0;
		if (m_WindowPos.X > m_pData->DrawSet.BMP_Size.X)
			m_WindowPos.X = m_pData->DrawSet.BMP_Size.X - 200;

		si.nPos = (int)m_WindowPos.X;

		SetScrollInfo(SB_HORZ, &si, TRUE);
		Invalidate();
	}
	else if (WndId == IDC_SLIDERVIEWPRC)
	{
		bool bRedraw = false;
		nPos = m_SliderViewPrc.GetPos();

		int PosValue = m_SliderViewPrc.GetPos();
		GetPrcPos(&PosValue, NULL, m_ViewPrc, 140);
		if (nPos != PosValue)
			bRedraw = true;
		switch (nSBCode)
		{
		case SB_PAGEUP:				break;
		case SB_PAGEDOWN:			break;
		case SB_LINEUP:				break;
		case SB_LINEDOWN:			break;
		case SB_THUMBPOSITION:		break;
		case SB_ENDSCROLL:			break;
		case SB_TOP:				break;
		case SB_BOTTOM:				break;
		case SB_THUMBTRACK:			bRedraw = false;	break;
		default:		break;
		}
		if (bRedraw)
		{
			GetPrcPos(NULL, &m_ViewPrc, nPos, 140);
			m_pData->DrawSet.ViewPoint.TimeInPc = m_ViewPrc;
			RedrawMe();
		}
	}
	else if (WndId == IDC_SLIDERVIEWSPREAD)
	{
		bool bRedraw = false;
		nPos = m_SliderSpread.GetPos();

		int PosValue = m_SliderSpread.GetPos();
		GetPrcPos(&PosValue, NULL, m_ViewSpread, 140);
		if (nPos != PosValue)
			bRedraw = true;
		switch (nSBCode)
		{
		case SB_PAGEUP:				break;
		case SB_PAGEDOWN:			break;
		case SB_LINEUP:				break;
		case SB_LINEDOWN:			break;
		case SB_THUMBPOSITION:		break;
		case SB_ENDSCROLL:			break;
		case SB_TOP:				break;
		case SB_BOTTOM:				break;
		case SB_THUMBTRACK:			bRedraw = false;	break;
		default:		break;
		}
		if (bRedraw)
		{
			GetPrcPos(NULL, &m_ViewSpread, nPos, 140);
			m_pData->DrawSet.ViewPoint.Spread = m_ViewSpread;
			RedrawMe();
		}
	}

	//CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


BOOL CDrawDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: Add your specialized code here and/or call the base class

	return CDialogEx::OnNotify(wParam, lParam, pResult);
}


void CDrawDlg::OnReleasedcaptureSlidermgnf(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
}


void CDrawDlg::OnThumbposchangingSlidermgnf(NMHDR* pNMHDR, LRESULT* pResult)
{
	// This feature requires Windows Vista or greater.
	// The symbol _WIN32_WINNT must be >= 0x0600.
	NMTRBTHUMBPOSCHANGING* pNMTPC = reinterpret_cast<NMTRBTHUMBPOSCHANGING*>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}


void CDrawDlg::OnBnClicked_Refresh()
{
	RedrawMe();
}





/*

	HDC hdcScreen;
	HDC hdcWindow;
	HDC hdcMemDC = NULL;
	HBITMAP hbmScreen = NULL;
	BITMAP bmpScreen;
	DWORD dwBytesWritten = 0;
	DWORD dwSizeofDIB = 0;
	HANDLE hFile = NULL;
	char* lpbitmap = NULL;
	HANDLE hDIB = NULL;
	DWORD dwBmpSize = 0;

	// Retrieve the handle to a display device context for the client
	// area of the window.
	hdcScreen = GetDC(NULL);
	hdcWindow = GetDC(hWnd);

	// Create a compatible DC, which is used in a BitBlt from the window DC.
	hdcMemDC = CreateCompatibleDC(hdcWindow);

	if (!hdcMemDC)
	{
		MessageBox(hWnd, L"CreateCompatibleDC has failed", L"Failed", MB_OK);
		goto done;
	}

	// Get the client area for size calculation.
	RECT rcClient;
	GetClientRect(hWnd, &rcClient);

	// This is the best stretch mode.
	SetStretchBltMode(hdcWindow, HALFTONE);

	// The source DC is the entire screen, and the destination DC is the current window (HWND).



if (!StretchBlt(hdcWindow,
		0, 0,
		rcClient.right, rcClient.bottom,
		hdcScreen,
		0, 0,
		GetSystemMetrics(SM_CXSCREEN),
		GetSystemMetrics(SM_CYSCREEN),
		SRCCOPY))
	{
		MessageBox(hWnd, L"StretchBlt has failed", L"Failed", MB_OK);
		goto done;
	}

	// Create a compatible bitmap from the Window DC.
	hbmScreen = CreateCompatibleBitmap(hdcWindow, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);

	if (!hbmScreen)
	{
		MessageBox(hWnd, L"CreateCompatibleBitmap Failed", L"Failed", MB_OK);
		goto done;
	}

	// Select the compatible bitmap into the compatible memory DC.
	SelectObject(hdcMemDC, hbmScreen);

	// Bit block transfer into our compatible memory DC.
	if (!BitBlt(hdcMemDC,
		0, 0,
		rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
		hdcWindow,
		0, 0,
		SRCCOPY))
	{
		MessageBox(hWnd, L"BitBlt has failed", L"Failed", MB_OK);
		goto done;
	}

	// Get the BITMAP from the HBITMAP.
	GetObject(hbmScreen, sizeof(BITMAP), &bmpScreen);

	BITMAPFILEHEADER   bmfHeader;
	BITMAPINFOHEADER   bi;

	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bmpScreen.bmWidth;
	bi.biHeight = bmpScreen.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;

	// Starting with 32-bit Windows, GlobalAlloc and LocalAlloc are implemented as wrapper functions that
	// call HeapAlloc using a handle to the process's default heap. Therefore, GlobalAlloc and LocalAlloc
	// have greater overhead than HeapAlloc.
	hDIB = GlobalAlloc(GHND, dwBmpSize);
	lpbitmap = (char*)GlobalLock(hDIB);

	// Gets the "bits" from the bitmap, and copies them into a buffer
	// that's pointed to by lpbitmap.
	GetDIBits(hdcWindow, hbmScreen, 0,
		(UINT)bmpScreen.bmHeight,
		lpbitmap,
		(BITMAPINFO*)&bi, DIB_RGB_COLORS);

	// A file is created, this is where we will save the screen capture.
	hFile = CreateFile(L"captureqwsx.bmp",
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);

	// Add the size of the headers to the size of the bitmap to get the total file size.
	dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	// Offset to where the actual bitmap bits start.
	bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);

	// Size of the file.
	bmfHeader.bfSize = dwSizeofDIB;

	// bfType must always be BM for Bitmaps.
	bmfHeader.bfType = 0x4D42; // BM.

	WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);

	// Unlock and Free the DIB from the heap.
	GlobalUnlock(hDIB);
	GlobalFree(hDIB);

	// Close the handle for the file that was created.
	CloseHandle(hFile);

*/


void CDrawDlg::OnBnClicked_Save()
{
	if (m_pData)
	{
		SDrawSettings oldDrawSet = m_pData->DrawSet;

		SPos TraceSize = { 0 };
		int BMP_Size = 1000;

		m_pData->DrawSet.BMP_Size.X = BMP_Size;
		m_pData->DrawSet.BMP_Size.Y = BMP_Size;

		m_pData->DrawSet.ViewPoint.TimeInPc = m_ViewPrc;
		m_pData->DrawSet.ViewPoint.Spread = m_ViewSpread;

		m_pData->Group->SetViewPoint(m_ViewPrc, m_ViewSpread);
		m_pData->Group->GetMinMaxTracePos(m_pData->DrawSet.PosMin, m_pData->DrawSet.PosMax, m_pData->DrawSet.ViewPoint.StartIdx, m_pData->DrawSet.ViewPoint.EndIdx);

		TraceSize.SET_AS_DELTA(m_pData->DrawSet.PosMax, m_pData->DrawSet.PosMin);

		if (TraceSize.X > TraceSize.Y) m_pData->DrawSet.ViewMltpl = BMP_Size / TraceSize.X;
		else m_pData->DrawSet.ViewMltpl = BMP_Size / TraceSize.Y;

		m_pData->DrawSet.ViewMltpl *= 0.98;

		m_pData->DrawSet.m_00_Pos.X = (m_pData->DrawSet.BMP_Size.X - (m_pData->DrawSet.PosMax.X - m_pData->DrawSet.PosMin.X) * m_pData->DrawSet.ViewMltpl * m_ViewMltpl) * 0.5 - m_pData->DrawSet.PosMin.X * m_pData->DrawSet.ViewMltpl * m_ViewMltpl;
		m_pData->DrawSet.m_00_Pos.Y = (m_pData->DrawSet.BMP_Size.Y - (m_pData->DrawSet.PosMax.Y - m_pData->DrawSet.PosMin.Y) * m_pData->DrawSet.ViewMltpl * m_ViewMltpl) * 0.5 - m_pData->DrawSet.PosMin.Y * m_pData->DrawSet.ViewMltpl * m_ViewMltpl;


		CPaintDC dc(this);
		HBITMAP hBitmap;       // Handle to the old bitmap

		// Create a memory device context compatible with the device. 
		CDC DCMem;
		DCMem.CreateCompatibleDC(&dc);

		hBitmap = ::CreateCompatibleBitmap(dc.m_hDC, (int)m_pData->DrawSet.BMP_Size.X, (int)m_pData->DrawSet.BMP_Size.Y);
		if (hBitmap)
		{
			HBITMAP hOldBitmap;       // Handle to the old bitmap
			// Select the new bitmap object into the memory device context. 
			hOldBitmap = (HBITMAP)DCMem.SelectObject(hBitmap);

			SPos tmp = { 0 };

			m_pData->DrawSet.ViewMltpl *= m_ViewMltpl;

			PaintTraceToCDC(&DCMem, m_pData, m_XYZ | SP_DRAW_CLEARBKG | SP_DRAW_TEXT, tmp);

			char fileMane[500];
			sprintf_s(fileMane, "C:\\SParticle\\BMP\\CALC_NP%d_C%d_ADD%d_Wind%d_Clls%X_DCT%d_ACT%d_DL%d_PT%g_%d.bmp",
				m_pData->Group->GetCount(), m_pData->ClassicType, m_pData->AdditiveType,
				m_pData->WindType, m_pData->CollisionType, m_pData->DensityCalcType, m_pData->AccCalcType, m_pData->DensityDerivLevel,
				m_pData->ParentSpaceThickness, GetTickCount());

			SaveBitmap(fileMane, DCMem, m_hDCBitmap);


			SaveBitmap(fileMane, DCMem, hBitmap);

			DCMem.SelectObject(hOldBitmap);
			DeleteObject(hBitmap);
			hBitmap = NULL;
		}

		m_pData->DrawSet = oldDrawSet;

	}


}

void CDrawDlg::OnBnClicked_UpLeft()
{
	SCROLLINFO si = { 0 };
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_POS;


	m_WindowPos.X = 0.0; 
	si.nPos = 0;
	SetScrollInfo(SB_HORZ, &si, TRUE);

	m_WindowPos.Y = 0.0;
	si.nPos = 0;
	SetScrollInfo(SB_VERT, &si, TRUE);

	Invalidate();

}

void CDrawDlg::OnBnClicked_Center()
{
	RECT re; 
	GetClientRect(&re);

	SCROLLINFO si = { 0 };
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_POS;

	m_WindowPos.X =  (((int)m_pData->DrawSet.BMP_Size.X) >> 1) - re.right / 2;
	si.nPos = (int)m_WindowPos.X;

	SetScrollInfo(SB_HORZ, &si, TRUE);

	m_WindowPos.Y = (((int)m_pData->DrawSet.BMP_Size.Y) >> 1) - re.bottom / 2;
	si.nPos = (int)m_WindowPos.Y;
	SetScrollInfo(SB_VERT, &si, TRUE);

	Invalidate();
}

void CDrawDlg::OnBnClickedButtonXyz()
{
	if (m_XYZ == SP_DRAW_XY) 
		m_XYZ = SP_DRAW_XYZ;
	else
		m_XYZ = SP_DRAW_XY;

	RedrawMe();
	OnBnClicked_Center();
}
