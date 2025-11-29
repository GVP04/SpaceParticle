// CDrawDlg.cpp : implementation file
//

#include "pch.h"
#include "CommonFunc.h"
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
	m_ViewPrc = 100.0;
	m_ViewSpread = 100.0;

	m_pData->DrawSet.WindowPos = { 0 };

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
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CDrawDlg::OnBnClicked_Refresh)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CDrawDlg::OnBnClicked_Save)
	ON_BN_CLICKED(IDC_BUTTON_UPLEFT, &CDrawDlg::OnBnClicked_UpLeft)
	ON_BN_CLICKED(IDC_BUTTON_CENTER, &CDrawDlg::OnBnClicked_Center)
	ON_BN_CLICKED(IDC_BUTTON_XYZ, &CDrawDlg::OnBnClickedButtonXyz)
	ON_BN_CLICKED(IDC_BUTTONCMD, &CDrawDlg::OnBnClickedButtoncmd)
	ON_BN_CLICKED(IDC_BUTTON_GROUP, &CDrawDlg::OnBnClickedButtonGroup)
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

			m_pData->DrawSet.ViewMltpl *= m_pData->DrawSet.ViewMltpl_Ext;

			m_pData->DrawSet.m_00_Pos.X = (m_pData->DrawSet.BMP_Size.X - (m_pData->DrawSet.PosMax.X - m_pData->DrawSet.PosMin.X) * m_pData->DrawSet.ViewMltpl) * 0.5 - m_pData->DrawSet.PosMin.X * m_pData->DrawSet.ViewMltpl;
			m_pData->DrawSet.m_00_Pos.Y = (m_pData->DrawSet.BMP_Size.Y - (m_pData->DrawSet.PosMax.Y - m_pData->DrawSet.PosMin.Y) * m_pData->DrawSet.ViewMltpl) * 0.5 - m_pData->DrawSet.PosMin.Y * m_pData->DrawSet.ViewMltpl;

			
			PaintTraceToCDC(&DCMem, m_pData, m_XYZ | SP_DRAW_CLEARBKG | SP_DRAW_TEXT | SP_DRAW_GROUPINFO, delta00);

			m_pData->DrawSet = oldDrawSet;

			m_uRedrawFlag = 0;
		}

		::BitBlt(dc.m_hDC, re.left, re.top, re.Width(), re.Height(), DCMem.m_hDC, re.left + (int)m_pData->DrawSet.WindowPos.X, re.top + (int)m_pData->DrawSet.WindowPos.Y, SRCCOPY);

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


void CDrawDlg::SetMltplSliderPos()
{
	//if (m_pData)   !!!!!!!!!!!!!!
	{
		m_SliderMgnf.SetRange(0, 140);

		int posMgn;
		GetMgnPos(&posMgn, NULL, m_pData->DrawSet.ViewMltpl_Ext, 140);
		m_SliderMgnf.SetPos(posMgn);
	}
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
	//if (m_pData)   !!!!!!!!!!!!!!!!!!!!test
	{
		m_pData->DrawSet.WindowPos = in_WPos;

		SCROLLINFO si = { 0 };
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_POS;

		m_pData->DrawSet.WindowPos.Y = in_WPos.Y;
		if (m_pData->DrawSet.WindowPos.Y < 0)	m_pData->DrawSet.WindowPos.Y = 0;
		if (m_pData->DrawSet.WindowPos.Y > m_pData->DrawSet.BMP_Size.Y)	m_pData->DrawSet.WindowPos.Y = m_pData->DrawSet.BMP_Size.Y - 200;

		si.nPos = (int)m_pData->DrawSet.WindowPos.Y;
		SetScrollInfo(SB_VERT, &si, TRUE);

		m_pData->DrawSet.WindowPos.X = in_WPos.X;
		if (m_pData->DrawSet.WindowPos.X < 0)	m_pData->DrawSet.WindowPos.X = 0;
		if (m_pData->DrawSet.WindowPos.X > m_pData->DrawSet.BMP_Size.X)	m_pData->DrawSet.WindowPos.X = m_pData->DrawSet.BMP_Size.X - 200;

		si.nPos = (int)m_pData->DrawSet.WindowPos.X;
		SetScrollInfo(SB_HORZ, &si, TRUE);
	}

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

	SetMltplSliderPos();
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
	si.nPos = (int)m_pData->DrawSet.WindowPos.X;

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
		case SB_PAGEUP:			m_pData->DrawSet.WindowPos.Y -= 400; break;
		case SB_PAGEDOWN:		m_pData->DrawSet.WindowPos.Y += 400; break;
		case SB_LINEUP:			m_pData->DrawSet.WindowPos.Y -= 20; break;
		case SB_LINEDOWN:		m_pData->DrawSet.WindowPos.Y += 20; break;
		case SB_THUMBPOSITION:	m_pData->DrawSet.WindowPos.Y = nPos; break;
		case SB_ENDSCROLL:		break;
		case SB_TOP:			m_pData->DrawSet.WindowPos.Y = 0; break;
		case SB_BOTTOM:			m_pData->DrawSet.WindowPos.Y = nPos - 200; break;
		case SB_THUMBTRACK:		m_pData->DrawSet.WindowPos.Y = nPos; break;
		default:
			nPos = nPos;
			break;
		}
		if (m_pData->DrawSet.WindowPos.Y < 0)
			m_pData->DrawSet.WindowPos.Y = 0;
		if (m_pData->DrawSet.WindowPos.Y > m_pData->DrawSet.BMP_Size.Y)
			m_pData->DrawSet.WindowPos.Y = m_pData->DrawSet.BMP_Size.Y - 200;

		si.nPos = (int)m_pData->DrawSet.WindowPos.Y;

		SetScrollInfo(SB_VERT, &si, TRUE);
		Invalidate();
	}
	else if (WndId == IDC_SLIDERMGNF)
	{
		bool bRedraw = false;
		nPos = m_SliderMgnf.GetPos();

		int PosValue = m_SliderMgnf.GetPos();
		GetMgnPos(&PosValue, NULL, m_pData->DrawSet.ViewMltpl_Ext, 140);
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
			GetMgnPos(NULL, &m_pData->DrawSet.ViewMltpl_Ext, nPos, 140);
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
		case SB_PAGEUP:			m_pData->DrawSet.WindowPos.X -= 400; break;
		case SB_PAGEDOWN:		m_pData->DrawSet.WindowPos.X += 400; break;
		case SB_LINEUP:			m_pData->DrawSet.WindowPos.X -= 20; break;
		case SB_LINEDOWN:		m_pData->DrawSet.WindowPos.X += 20; break;
		case SB_THUMBPOSITION:	m_pData->DrawSet.WindowPos.X = nPos; break;
		case SB_ENDSCROLL:		break;
		case SB_TOP:			m_pData->DrawSet.WindowPos.X = 0; break;
		case SB_BOTTOM:			m_pData->DrawSet.WindowPos.X = nPos - 200; break;
		case SB_THUMBTRACK:		m_pData->DrawSet.WindowPos.X = nPos; break;
		default:
			nPos = nPos;
			break;
		}
		if (m_pData->DrawSet.WindowPos.X < 0)
			m_pData->DrawSet.WindowPos.X = 0;
		if (m_pData->DrawSet.WindowPos.X > m_pData->DrawSet.BMP_Size.X)
			m_pData->DrawSet.WindowPos.X = m_pData->DrawSet.BMP_Size.X - 200;

		si.nPos = (int)m_pData->DrawSet.WindowPos.X;

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
		case SB_THUMBTRACK:	
			bRedraw = false;	break;
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

void CDrawDlg::OnBnClicked_Refresh()
{
	RedrawMe();
}

void CDrawDlg::OnBnClicked_Save()
{
	if (m_pData)
	{
		SDrawSettings oldDrawSet = m_pData->DrawSet;

		SPos TraceSize = { 0 };
		int BMP_Size = 2000;

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
		m_pData->DrawSet.ViewMltpl *= m_pData->DrawSet.ViewMltpl_Ext;

		m_pData->DrawSet.m_00_Pos.X = (m_pData->DrawSet.BMP_Size.X - (m_pData->DrawSet.PosMax.X - m_pData->DrawSet.PosMin.X) * m_pData->DrawSet.ViewMltpl) * 0.5 - m_pData->DrawSet.PosMin.X * m_pData->DrawSet.ViewMltpl;
		m_pData->DrawSet.m_00_Pos.Y = (m_pData->DrawSet.BMP_Size.Y - (m_pData->DrawSet.PosMax.Y - m_pData->DrawSet.PosMin.Y) * m_pData->DrawSet.ViewMltpl) * 0.5 - m_pData->DrawSet.PosMin.Y * m_pData->DrawSet.ViewMltpl;


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


			PaintTraceToCDC(&DCMem, m_pData, m_XYZ | SP_DRAW_CLEARBKG | SP_DRAW_TEXT | SP_DRAW_GROUPINFO, tmp);

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


	m_pData->DrawSet.WindowPos.X = 0.0; 
	si.nPos = 0;
	SetScrollInfo(SB_HORZ, &si, TRUE);

	m_pData->DrawSet.WindowPos.Y = 0.0;
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

	m_pData->DrawSet.WindowPos.X =  (((int)m_pData->DrawSet.BMP_Size.X) >> 1) - re.right / 2;
	si.nPos = (int)m_pData->DrawSet.WindowPos.X;

	SetScrollInfo(SB_HORZ, &si, TRUE);

	m_pData->DrawSet.WindowPos.Y = (((int)m_pData->DrawSet.BMP_Size.Y) >> 1) - re.bottom / 2;
	si.nPos = (int)m_pData->DrawSet.WindowPos.Y;
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

void CDrawDlg::OnBnClickedButtoncmd()
{
	if (m_pData && m_pData->Group)
	{
		char* tmps = new char[SD_PARAMSLEN];

		::GetWindowTextA(::GetDlgItem(m_hWnd, IDC_EDITCMD), tmps, SD_PARAMSLEN);
		m_pData->Group->Cmd_Add(tmps);
		delete [] tmps;
	}
}

void CDrawDlg::OnBnClickedButtonGroup()
{

	if (m_pData && m_pData->Group)
	{
		char* tmps = new char[SD_PARAMSLEN];

		m_pData->DrawSet.ShowGroupArrows  = (m_pData->DrawSet.ShowGroupArrows & 0xFFFFFFFC) | ((m_pData->DrawSet.ShowGroupArrows & 0x3) + 1 );

		sprintf_s(tmps, SD_PARAMSLEN, "SET TRACE_GROUP_ARR = %d", m_pData->DrawSet.ShowGroupArrows);
		m_pData->Group->Cmd_Add(tmps);

		delete[] tmps;
	}
}
