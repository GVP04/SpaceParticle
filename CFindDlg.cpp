// CFindDlg.cpp : implementation file
//

#include "pch.h"
#include "CommonFunc.h"
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
	m_pData = nullptr;
	m_hDCBitmap = nullptr;
	m_LiveCntr = 0;

	m_BMP_Size.X = 2000;
	m_BMP_Size.Y = 1000;
	m_BMP_Size.Z = 500;

	m_uRedrawFlag = 1;
}

CFindDlg::~CFindDlg()
{
	if (m_hDCBitmap) DeleteObject(m_hDCBitmap);
	m_hDCBitmap = nullptr;
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
	ON_WM_TIMER()
	ON_BN_CLICKED(IDCANCEL, &CFindDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CFindDlg::OnBnClicked_Refresh)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CFindDlg::OnBnClicked_Save)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, &CFindDlg::OnBnClicked_Next)
	ON_BN_CLICKED(IDC_BUTTON_NEXTANDREPORT, &CFindDlg::OnBnClicked_NextAndReport)
	ON_BN_CLICKED(IDC_BUTTON_REFRESHLIVE, &CFindDlg::OnBnClickedButtonRefreshlive)
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
	if (m_hDCBitmap == nullptr)
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

	SetTimer(1, 100, nullptr);

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
			int m_PtrCount = m_pData->curCalc.Group->GetCount();


			SDrawSettings oldDrawSet = m_pData->curCalc.DrawSet;

			m_pData->curCalc.DrawSet.iShowFlags = 0;

			RECT re = { 0 };
			GetClientRect(&re);

			int cx = re.right - re.left;
			int cy = re.bottom - re.top;
			int BMP_Size = cx > cy ? cy: cx;

			m_pData->curCalc.DrawSet.BMP_Size.X = BMP_Size;
			m_pData->curCalc.DrawSet.BMP_Size.Y = BMP_Size;

			m_pData->curCalc.DrawSet.ViewPoint.TimeInPc = 100.0;
			m_pData->curCalc.DrawSet.ViewPoint.Spread = 100.0;

			m_pData->curCalc.Group->SetViewPoint(100.0, 100.0);

			m_pData->curCalc.Group->GetMinMaxTracePos(m_pData->curCalc.DrawSet.PosMin, m_pData->curCalc.DrawSet.PosMax, 0, SP_DRAW_MAX_ID);
			SPos TraceSize;
			TraceSize.SET_AS_DELTA(m_pData->curCalc.DrawSet.PosMax, m_pData->curCalc.DrawSet.PosMin);

			if (TraceSize.X > TraceSize.Y && TraceSize.X > TraceSize.Z) 
				m_pData->curCalc.DrawSet.ViewMltpl = BMP_Size / TraceSize.X;
			else
			if (TraceSize.Y > TraceSize.X && TraceSize.Y > TraceSize.Z) 
				m_pData->curCalc.DrawSet.ViewMltpl = BMP_Size / TraceSize.Y;
			else
				m_pData->curCalc.DrawSet.ViewMltpl = BMP_Size / TraceSize.Z;

			m_pData->curCalc.DrawSet.ViewMltpl *= 0.98;

			m_pData->curCalc.DrawSet.m_00_Pos = { 0 };

			SPos delta00 = { 0 };

			if (cx > cy) 
				delta00.X = cx - cy;
			else
				delta00.Y = cy - cx;

			PaintTraceToCDC(DCMem, &m_pData->curCalc, SP_DRAW_XYZ | SP_DRAW_CLEARBKG | SP_DRAW_GROUPINFO, delta00);

			m_pData->curCalc.DrawSet = oldDrawSet;

			char tmps[300];
			int posY = 10;
			int txtlen = sprintf_s(tmps, 300, "nParticle = %d Elapsed = ", m_PtrCount);
			txtlen += m_pData->curCalc.Time.GetElapsedCur(tmps + txtlen);

			TextOutA(hDC, 1, posY, tmps, txtlen);
			
			
			txtlen = sprintf_s(tmps, 300, "Current Iteration = %d Reported %d", m_pData->stat.Count, m_pData->stat.Reported);
			posY += 20;TextOutA(hDC, 1, posY, tmps, txtlen);

			txtlen = sprintf_s(tmps, 300, "CurStep %d_%d   Time %4.4g    Step %5.5g", m_pData->FindCounter.GetCurLoop(), m_pData->FindCounter.GetCurStep(), m_pData->curCalc.Time.GetCurTime(), m_pData->curCalc.Time.GetStep());
			posY += 20; TextOutA(hDC, 1, posY, tmps, txtlen);

			txtlen = sprintf_s(tmps, 300, "MinRelDistR %5.5g  MaxRelDistR %5.5g     MinRelDistP %5.5g  MaxRelDistP %5.5g     MinRelSpeed %5.5g  MaxRelSpeed %5.5g", m_pData->curCalc.Stat.Rel_Min.Real_Distance.MMValue, m_pData->curCalc.Stat.Rel_Max.Real_Distance.MMValue, m_pData->curCalc.Stat.Rel_Min.Phantom_Distance.MMValue, m_pData->curCalc.Stat.Rel_Max.Phantom_Distance.MMValue, m_pData->curCalc.Stat.Rel_Min.Velocity.MMValue, m_pData->curCalc.Stat.Rel_Max.Velocity.MMValue);
			posY += 20; TextOutA(hDC, 1, posY, tmps, txtlen);

//			txtlen = sprintf_s(tmps, 300, "DltMinRelDistance %5.5g  DltMinRelSpeed %5.5g  DltMaxRelSpeed %5.5g", m_pData->curCalc.Stat.Rel_Min.Distance.MMValue - m_pData->curCalc.Stat.Prev_Rel_Min.Distance.MMValue, m_pData->curCalc.Stat.Rel_Min.Velocity.MMValue - m_pData->curCalc.Stat.Prev_Rel_Min.Velocity.MMValue, m_pData->curCalc.Stat.Rel_Max.Velocity.MMValue - m_pData->curCalc.Stat.Prev_Rel_Max.Velocity.MMValue);
//			posY += 20; TextOutA(hDC, 1, posY, tmps, txtlen);

			GroupSummaryInfo GrInfo_Cur;
			m_pData->curCalc.Group->GetGroupSummaryInfo(&GrInfo_Cur, m_pData->curCalc.Time.GetCurTime());

			txtlen = sprintf_s(tmps, 300, "Group Velocity %3.3g | X=%3.3g Y=%3.3g Z=%3.3g", GrInfo_Cur.Velocity, GrInfo_Cur.Speed_3D.X, GrInfo_Cur.Speed_3D.Y, GrInfo_Cur.Speed_3D.Z);
			TextOutA(hDC, 1, posY += 20, tmps, txtlen);
			txtlen = sprintf_s(tmps, 300, "Group Accel %3.3g | X=%3.3g Y=%3.3g Z=%3.3g", GrInfo_Cur.Acceleration, GrInfo_Cur.Acceleration_3D.X, GrInfo_Cur.Acceleration_3D.Y, GrInfo_Cur.Acceleration_3D.Z);
			TextOutA(hDC, 1, posY += 20, tmps, txtlen);
			txtlen = sprintf_s(tmps, 300, "Group Pos X=%3.3g Y=%3.3g Z=%3.3g", GrInfo_Cur.Position.X, GrInfo_Cur.Position.Y, GrInfo_Cur.Position.Z);
			TextOutA(hDC, 1, posY += 20, tmps, txtlen);


			posY += 10;
			for (int i = 0; i < m_PtrCount; i++)
			{
				SPos tmp = m_pData->curCalc.Group->m_Array[i]->GetCurPosition();
				posY += 20; 
				DCMem->SetTextColor(mRGB[(i) % 16]);
				txtlen = sprintf_s(tmps, 300, "%0.2d Pos/Velocity", i);
				TextOutA(hDC, 1, posY, tmps, txtlen);

				DCMem->SetTextColor(RGB(0, 0, 0));
				txtlen = sprintf_s(tmps, 300, "X%3.3g", tmp.X);
				int tmpPosX = 100;
				TextOutA(hDC, tmpPosX, posY, tmps, txtlen);
				tmpPosX += txtlen * 8 + 5;
				txtlen = sprintf_s(tmps, 300, "Y%3.3g", tmp.Y);
				TextOutA(hDC, tmpPosX, posY, tmps, txtlen);
				tmpPosX += txtlen * 8 + 5;
				txtlen = sprintf_s(tmps, 300, "Z%3.3g", tmp.Z);
				TextOutA(hDC, tmpPosX, posY, tmps, txtlen);

				m_pData->curCalc.Group->m_Array[i]->GetCurSpeed(tmp);

				tmpPosX += txtlen * 8 + 5;
				txtlen = sprintf_s(tmps, 300, "S%3.3g", tmp.LEN());
				TextOutA(hDC, tmpPosX, posY, tmps, txtlen);
				tmpPosX += txtlen * 8 + 5;
				txtlen = sprintf_s(tmps, 300, "X%3.3g", tmp.X);
				TextOutA(hDC, tmpPosX, posY, tmps, txtlen);
				tmpPosX += txtlen * 8 + 5;
				txtlen = sprintf_s(tmps, 300, "Y%3.3g", tmp.Y);
				TextOutA(hDC, tmpPosX, posY, tmps, txtlen);
				tmpPosX += txtlen * 8 + 5;
				txtlen = sprintf_s(tmps, 300, "Z%3.3g", tmp.Z);
				TextOutA(hDC, tmpPosX, posY, tmps, txtlen);
			}
		}
		catch (...) { ; }
		LeaveCriticalSection(&m_pData->curCalc.Group->m_cs);
	}


}

void CFindDlg::OnBnClicked_Refresh()
{
	::RedrawWindow(m_hWnd, nullptr, nullptr, RDW_NOERASE | RDW_UPDATENOW | RDW_INVALIDATE);
}


void CFindDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case 1:
		if (m_pData)
		{
			KillTimer(nIDEvent);
			if (m_pData->m_FIND_REFRESH < 500)	m_pData->m_FIND_REFRESH = 500;

			if (m_LiveCntr >= m_pData->m_FIND_REFRESH)
			{
				::RedrawWindow(m_hWnd, nullptr, nullptr, RDW_NOERASE| RDW_UPDATENOW| RDW_INVALIDATE);
			}
			m_LiveCntr -= 100;
			if (m_LiveCntr <= 0) 
				m_LiveCntr = m_pData->m_FIND_REFRESH;

			SetTimer(nIDEvent, 100, nullptr);
		}
		break;
	default:
		break;
	}


	CDialogEx::OnTimer(nIDEvent);
}


void CFindDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	if (m_pData && m_pData->curCalc.Group)
	{
		m_pData->curCalc.Group->SetCalcState(0);
	}

	CDialogEx::OnCancel();
}


void CFindDlg::OnBnClicked_Next()
{
	if (m_pData && m_pData->curCalc.Group)
		m_pData->curCalc.Group->SetCalcState(M_STATE_DO_NEXT);
}

void CFindDlg::OnBnClicked_NextAndReport()
{
	if (m_pData && m_pData->curCalc.Group)
		m_pData->curCalc.Group->SetCalcState(M_STATE_DO_NEXT | M_STATE_DO_REPORT);
}


///void SaveBitmapTest(char* in_FileName, HDC in_hdc, HBITMAP in_hBMP, HWND hWnd);

void CFindDlg::OnBnClicked_Save()
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
			sprintf_s(fileMane, "C:\\SParticle\\BMP\\FIND_NP%d_C%d_ADD%d_Wind%d_Clls%X_DCT%d_ACT%d_DL%d_nc%0.3d_PT%g_%I64d.bmp", 
				m_pData->curCalc.Group->GetCount(), m_pData->curCalc.ClassicType, m_pData->curCalc.AdditiveType, 
				m_pData->curCalc.WindType, m_pData->curCalc.CollisionType, m_pData->curCalc.DensityCalcType, m_pData->curCalc.AccCalcType, m_pData->curCalc.DensityDerivLevel,
				m_pData->stat.Count, m_pData->curCalc.ParentSpaceThickness, GetTickCount64());
			SaveBitmap(fileMane, DCMem, m_hDCBitmap);

			DCMem.SelectObject(hOldBitmap);
		}
		LeaveCriticalSection(&m_pData->curCalc.Group->m_cs);
	}
}

void CFindDlg::OnBnClickedButtonRefreshlive()
{
	m_LiveCntr = 10000;
}
