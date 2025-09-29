// CFindDlg.cpp : implementation file
//

#include "pch.h"
#include "SpaceOM.h"
#include "afxdialogex.h"
#include "CFindDlg.h"

#include "CSpParticle.h"
#include "CSP_Group.h"
#include "CSDataArray.h"
#include "CDrawDlg.h"



// CFindDlg dialog

IMPLEMENT_DYNAMIC(CFindDlg, CDialogEx)

CFindDlg::CFindDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOGFINDSP, pParent)
{
	m_pData = NULL;
	m_hDCBitmap = NULL;

	m_BMP_Size.X = 2000;
	m_BMP_Size.Y = 1000;
	m_BMP_Size.Z = 500;

	m_uRedrawFlag = 1;
}

CFindDlg::~CFindDlg()
{
	if (m_hDCBitmap) DeleteObject(m_hDCBitmap);
	m_hDCBitmap = NULL;
}

void CFindDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFindDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_BUTTONSHOWTRACE, &CFindDlg::OnBnClickedButtonshowtrace)
	ON_BN_CLICKED(IDC_BUTTONSHOWTRACE2, &CFindDlg::OnBnClickedButtonshowtrace2)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDCANCEL, &CFindDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTONSHOWTRACE3, &CFindDlg::OnBnClickedButtonshowtrace3)
	ON_BN_CLICKED(IDC_BUTTONREFRESH2, &CFindDlg::OnBnClickedButtonrefresh2)
END_MESSAGE_MAP()


// CFindDlg message handlers


void CFindDlg::OnPaint()
{
	CPaintDC dc(this);
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
		m_hDCBitmap = ::CreateCompatibleBitmap(dc.m_hDC, (int)m_BMP_Size.X, (int)m_BMP_Size.Y);
		m_uRedrawFlag = 1;
		///////////////	   ::BitBlt (DCMem.m_hDC, re.left,re.top,re.Width(),re.Height(), dc.m_hDC , re.left,re.top, SRCCOPY);
	}

	// Select the new bitmap object into the memory device context. 
	hOldBitmap = (HBITMAP)DCMem.SelectObject(m_hDCBitmap);
	//if (m_uRedrawFlag)
	{
		PaintToCDC(&DCMem);
		m_uRedrawFlag = 0;
	}

	::BitBlt(dc.m_hDC, re.left, re.top, re.Width(), re.Height(), DCMem.m_hDC, re.left, re.top, SRCCOPY);

	// Select the old bitmap back into the device context.
	DCMem.SelectObject(hOldBitmap);
}


void CFindDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
}


void CFindDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CFindDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default

	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}


BOOL CFindDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetTimer(1, m_pData ? m_pData->m_FIND_REFRESH : 5000, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}



void CFindDlg::PaintToCDC(CDC* DCMem)
{
	if (DCMem && m_pData && m_pData->curCalc.Group)
	{
		EnterCriticalSection(&m_pData->curCalc.Group->m_cs);

		DCMem->SetBkMode(TRANSPARENT);
		DCMem->FillSolidRect(0, 0, (int)m_BMP_Size.X, (int)m_BMP_Size.Y, RGB(255, 255, 255));

		DCMem->SetBkColor(RGB(255, 255, 255));
		DCMem->SetTextColor(RGB(0, 0, 0));


		try
		{
			HDC hDC = DCMem->m_hDC;

			SDrawSettings oldDrawSet = m_pData->curCalc.DrawSet;

			m_pData->curCalc.DrawSet.iShowFlags = SP_SHOWFLAG_TRACE;

			RECT re = { 0 };
			GetClientRect(&re);


			SPos minPos = { 0 };
			SPos maxPos = { 0 };
			SPos TraceSize = { 0 };
			int cx = re.right - re.left;
			int cy = re.bottom - re.top;
			int BMP_Size = cx > cy ? cy: cx;

			m_pData->curCalc.DrawSet.BMP_Size.X = BMP_Size;
			m_pData->curCalc.DrawSet.BMP_Size.Y = BMP_Size;

			m_pData->curCalc.Group->GetMinMaxTracePos(minPos, maxPos);


			TraceSize.X = maxPos.X - minPos.X;
			TraceSize.Y = maxPos.Y - minPos.Y;
			TraceSize.Z = maxPos.Z - minPos.Z;

			if (TraceSize.X > TraceSize.Y && TraceSize.X > TraceSize.Z) 
				m_pData->curCalc.DrawSet.ViewMltpl = BMP_Size / TraceSize.X;
			else
			if (TraceSize.Y > TraceSize.X && TraceSize.Y > TraceSize.Z) 
				m_pData->curCalc.DrawSet.ViewMltpl = BMP_Size / TraceSize.Y;
			else
				m_pData->curCalc.DrawSet.ViewMltpl = BMP_Size / TraceSize.Z;

			m_pData->curCalc.DrawSet.ViewMltpl *= 0.95;

			m_pData->curCalc.DrawSet.m_00_Pos = { 0 };

			SPos delta00 = { 0 };

			if (cx > cy) 
				delta00.X = cx - cy;
			else
				delta00.Y = cy - cx;

			m_pData->curCalc.DrawSet.PosMin = minPos;
			m_pData->curCalc.DrawSet.PosMax = maxPos;

			PaintTraceToCDC(DCMem, &m_pData->curCalc, SP_DRAW_XYZ, delta00);

			m_pData->curCalc.DrawSet = oldDrawSet;

			char tmps[300];
			int posY = 10;
			int txtlen = sprintf_s(tmps, 300, "nParticle = %d", m_pData->curCalc.Group->m_Count);
			TextOutA(hDC, 1, posY, tmps, txtlen);
			
			
			txtlen = sprintf_s(tmps, 300, "Current Iteration = %d Reported %d", m_pData->stat.Count, m_pData->stat.Reported);
			posY += 20;TextOutA(hDC, 1, posY, tmps, txtlen);

			txtlen = sprintf_s(tmps, 300, "CurStep %d   Time %g    Step %g", m_pData->curCalc.CurStep, m_pData->curCalc.Time.CurTime, m_pData->curCalc.Time.CurStep);
			posY += 20; TextOutA(hDC, 1, posY, tmps, txtlen);

			txtlen = sprintf_s(tmps, 300, "MinRelDist %g  MaxRelDist % g     MinRelSpeed %g  MaxRelSpeed %g", m_pData->curCalc.Stat.Rel_Min.Distance.MMValue, m_pData->curCalc.Stat.Rel_Max.Distance.MMValue, m_pData->curCalc.Stat.Rel_Min.Speed.MMValue, m_pData->curCalc.Stat.Rel_Max.Speed.MMValue);
			posY += 20; TextOutA(hDC, 1, posY, tmps, txtlen);

			txtlen = sprintf_s(tmps, 300, "DltMinRelDistance %g  DltMinRelSpeed %g  DltMaxRelSpeed %g", m_pData->curCalc.Stat.Rel_Min.Distance.MMValue - m_pData->curCalc.Stat.Prev_Rel_Min.Distance.MMValue, m_pData->curCalc.Stat.Rel_Min.Speed.MMValue - m_pData->curCalc.Stat.Prev_Rel_Min.Speed.MMValue, m_pData->curCalc.Stat.Rel_Max.Speed.MMValue - m_pData->curCalc.Stat.Prev_Rel_Max.Speed.MMValue);
			posY += 20; TextOutA(hDC, 1, posY, tmps, txtlen);

			posY += 10;
			for (int i = 0; i < m_pData->curCalc.Group->m_Count; i++)
			{
				SPos tmp = m_pData->curCalc.Group->m_Array[i]->GetCurPosition();
				posY += 20; 
				DCMem->SetTextColor(mRGB[(i) % 16]);
				TextOutA(hDC, 1, posY, "Position", 8);
				DCMem->SetTextColor(RGB(0, 0, 0));

				txtlen = sprintf_s(tmps, 300, "X=%g  Y=%g  Z=%g", tmp.X, tmp.Y, tmp.Z);
				TextOutA(hDC, 60, posY, tmps, txtlen);
			}
			posY += 10;
			for (int i = 0; i < m_pData->curCalc.Group->m_Count; i++)
			{
				SPos tmp = m_pData->curCalc.Group->m_Array[i]->GetCurData()->Speed;
				posY += 20;
				DCMem->SetTextColor(mRGB[(i) % 16]);
				TextOutA(hDC, 1, posY, "Speed", 5);
				DCMem->SetTextColor(RGB(0, 0, 0));

				txtlen = sprintf_s(tmps, 300, "%g --  X=%g  Y=%g  Z=%g", CSpParticle::Get3D_Pwr1(tmp), tmp.X, tmp.Y, tmp.Z);
				TextOutA(hDC, 60, posY, tmps, txtlen);
			}



//				txtlen = sprintf_s(tmps, 300, "Time = %15.15g  | TimeStep = %15.15g ", pData[nPoints - 1].TimePoint, m_pData->curCalc.Group->m_pCalc->Time.CurStep);
//				TextOutA(hDC, 1, 30, tmps, txtlen);
		}
		catch (...) { ; }


		LeaveCriticalSection(&m_pData->curCalc.Group->m_cs);
	}


}

void CFindDlg::OnBnClickedButtonshowtrace2()
{
	Invalidate();
	RedrawWindow();
}


void CFindDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case 1:
		Invalidate();
		RedrawWindow();
		break;
	default:
		break;
	}


	CDialogEx::OnTimer(nIDEvent);
}


void CFindDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	if (m_pData)
	{
		m_pData->FindState = 0;
		m_pData->curCalc.State = 0;
	}

	CDialogEx::OnCancel();
}


void CFindDlg::OnBnClickedButtonshowtrace()
{
	m_pData->FindState |= M_STATE_DO_NEXT;
}

void CFindDlg::OnBnClickedButtonshowtrace3()
{
	m_pData->FindState |= M_STATE_DO_NEXT| M_STATE_DO_REPORT;
}



///void SaveBitmapTest(char* in_FileName, HDC in_hdc, HBITMAP in_hBMP, HWND hWnd);

void CFindDlg::OnBnClickedButtonrefresh2()
{
	if (m_pData && m_hDCBitmap)
	{
		EnterCriticalSection(&m_pData->curCalc.Group->m_cs);

		CPaintDC dc(this);
		CDC DCMem;
		DCMem.CreateCompatibleDC(&dc);

		if (m_hDCBitmap)
		{
			HBITMAP hOldBitmap;       // Handle to the old bitmap
			hOldBitmap = (HBITMAP)DCMem.SelectObject(m_hDCBitmap);

			char fileMane[500];
			sprintf_s(fileMane, "C:\\SParticle\\BMP\\FIND_NP%d_C%d_ADD%d_Wind%d_DCT%d_ACT%d_DL%d_nc%0.3d_PT%g_%d.bmp", 
				m_pData->curCalc.Group->GetCount(), m_pData->curCalc.ClassicType, m_pData->curCalc.AdditiveType, 
				m_pData->curCalc.WindType, m_pData->curCalc.DensityCalcType, m_pData->curCalc.AccCalcType, m_pData->curCalc.DensityDerivLevel,
				m_pData->stat.Count, m_pData->curCalc.ParentSpaceThickness, GetTickCount());
			SaveBitmap(fileMane, DCMem, m_hDCBitmap);

			DCMem.SelectObject(hOldBitmap);
		}
		LeaveCriticalSection(&m_pData->curCalc.Group->m_cs);
	}
}
