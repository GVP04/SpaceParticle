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


	m_WindowPos.X = 0;
	m_WindowPos.Y = 0;
	m_WindowPos.Z = 0;

	m_uRedrawFlag = 1;
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
	DDX_Control(pDX, IDC_SLIDERSTARTX, m_SliderStartX);
	DDX_Control(pDX, IDC_SLIDERSTARTY, m_SliderStartY);
}


BEGIN_MESSAGE_MAP(CDrawDlg, CDialogEx)
	ON_WM_PAINT()
    ON_BN_CLICKED(IDCANCEL, &CDrawDlg::OnBnClickedCancel)
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDERMGNF, &CDrawDlg::OnReleasedcaptureSlidermgnf)
	ON_NOTIFY(TRBN_THUMBPOSCHANGING, IDC_SLIDERMGNF, &CDrawDlg::OnThumbposchangingSlidermgnf)
	ON_BN_CLICKED(IDC_BUTTONREFRESH, &CDrawDlg::OnBnClickedButtonrefresh)
	ON_BN_CLICKED(IDC_BUTTONREFRESH2, &CDrawDlg::OnBnClickedButtonrefresh2)
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
			//PaintToCDC(&DCMem);
			SPos tmp = { 0 };
			PaintTraceToCDC(&DCMem, m_pData, SP_DRAW_XY, tmp);
			m_uRedrawFlag = 0;
		}

		::BitBlt(dc.m_hDC, re.left, re.top, re.Width(), re.Height(), DCMem.m_hDC, re.left + (int)m_WindowPos.X, re.top + (int)m_WindowPos.Y, SRCCOPY);

		// Select the old bitmap back into the device context.
		DCMem.SelectObject(hOldBitmap);
	}

}




void CDrawDlg::PaintToCDC(CDC* DCMem)
{
	if (DCMem && m_pData && m_pData->Group)
	{
		EnterCriticalSection(&m_pData->Group->m_cs);

		DCMem->SetBkMode(TRANSPARENT);
		DCMem->FillSolidRect(0,0,(int)m_pData->DrawSet.BMP_Size.X, (int)m_pData->DrawSet.BMP_Size.Y,RGB(255,255,255));

		DCMem->SetBkColor(RGB(255,255,255));
		DCMem->SetTextColor(RGB(0, 0, 0));


		try
		{

			int nTrace = m_pData->Group->m_Count;
			if (nTrace > 1 && m_pData->Group->m_Array[0] && m_pData->Group->m_Array[0]->m_Data)
			{
				HDC hDC = DCMem->m_hDC;

				int nPoints = m_pData->Group->m_Array[0]->m_Data->CurPos + 1;
				SData* pData = m_pData->Group->m_Array[0]->m_Data->m_Array;
				SData VP_Data = { 0 };

				char tmps[300];
				int txtlen = sprintf_s(tmps, 300, "nPoints = %d", nPoints);
				TextOutA(hDC, 1, 10, tmps, txtlen);
				txtlen = sprintf_s(tmps, 300, "Time = %15.15g  | TimeStep = %15.15g ", pData[nPoints-1].TimePoint, m_pData->Group->m_pCalc->Time.CurStep);
				TextOutA(hDC, 1, 30, tmps, txtlen);

				int iLastY = 0;

				for (int i = 0; i < nTrace; i++)
				{
					pData = m_pData->Group->m_Array[i]->m_Data->m_Array;

					DCMem->SetTextColor(mRGB[(i) % 16]);
					TextOutA(hDC, 1, 50 + i * 50, "Speed", 5);
					DCMem->SetTextColor(RGB(0, 0, 0));


					txtlen = sprintf_s(tmps, 300, "%g | X=%g Y=%g Z=%g", 
						sqrt(pData[nPoints - 1].Speed.X * pData[nPoints - 1].Speed.X + pData[nPoints - 1].Speed.Y * pData[nPoints - 1].Speed.Y + pData[nPoints - 1].Speed.Z * pData[nPoints - 1].Speed.Z)
						, pData[nPoints - 1].Speed.X, pData[nPoints - 1].Speed.Y, pData[nPoints - 1].Speed.Z);
					TextOutA(hDC, 60, 50 + i * 50, tmps, txtlen);

					DCMem->SetTextColor(mRGB[(i) % 16]);
					TextOutA(hDC, 1, 70 + i * 50, "Density", 7);
					DCMem->SetTextColor(RGB(0, 0, 0));

					txtlen = sprintf_s(tmps, 300, "Density %g | X=%g Y=%g Z=%g", 
						sqrt(pData[nPoints - 1].Accel3D.X * pData[nPoints - 1].Accel3D.X + pData[nPoints - 1].Accel3D.Y * pData[nPoints - 1].Accel3D.Y + pData[nPoints - 1].Accel3D.Z * pData[nPoints - 1].Accel3D.Z)
						, pData[nPoints - 1].Accel3D.X, pData[nPoints - 1].Accel3D.Y, pData[nPoints - 1].Accel3D.Z);
					TextOutA(hDC, 60, iLastY = 70 + i * 50, tmps, txtlen);
				}

				SMinMaxRelation out_Min, out_Max;
				m_pData->Group->GetMinMaxRelation(&out_Min, &out_Max);

				txtlen = sprintf_s(tmps, 300, "MIN RELATIVE   Distance = %g | Speed = %g", out_Min.Distance.MMValue, sqrt(out_Min.Speed.MMValue));
				TextOutA(hDC, 1, iLastY + 50, tmps, txtlen);
				txtlen = sprintf_s(tmps, 300, "MAX RELATIVE   Distance = %g | Speed = %g", out_Max.Distance.MMValue, sqrt(out_Max.Speed.MMValue));
				TextOutA(hDC, 1, iLastY + 70, tmps, txtlen);

				for (int idPrt = 0; idPrt < nTrace; idPrt++)
				{
					int Pwr2Id = 1 << idPrt;

					if ((m_pData->DrawSet.ShowItems & Pwr2Id) && (Pwr2Id & m_pData->DrawSet.ShowLinksFrom))
					{
						nPoints = m_pData->Group->m_Array[idPrt]->m_Data->CurPos + 1;
						HPEN PenLink = CreatePen(PS_SOLID, 1, mRGB[(idPrt + 8) % 16]);
						SelectObject(hDC, PenLink);

						pData = m_pData->Group->m_Array[idPrt]->m_Data->m_Array;
						if (m_pData->DrawSet.nLinks == 0) m_pData->DrawSet.nLinks = 40;

						int ViewStep = nPoints / m_pData->DrawSet.nLinks;
						if (ViewStep < 1) ViewStep = 1;

						for (int i = 0; i < nPoints; i+= ViewStep)
						{
							int posX = (int)(m_pData->DrawSet.m_00_Pos.X + (((double)pData[i].position.X) * m_pData->DrawSet.ViewMltpl));
							int posY = (int)(m_pData->DrawSet.m_00_Pos.Y + (((double)pData[i].position.Y) * m_pData->DrawSet.ViewMltpl));

							MoveToEx(hDC, posX, posY, NULL);

							for (int j = 0; j < nTrace; j++)
								if (i != j && ((1 << j) & m_pData->DrawSet.ShowLinksTo))
								{
									m_pData->Group->m_Array[j]->FindViewPoint(&VP_Data, pData + i, pData[i].TimePoint, NULL);

									LineTo(hDC, (int)(m_pData->DrawSet.m_00_Pos.X + (((double)VP_Data.position.X) * m_pData->DrawSet.ViewMltpl)), (int)(m_pData->DrawSet.m_00_Pos.Y + (((double)VP_Data.position.Y) * m_pData->DrawSet.ViewMltpl)));
									MoveToEx(hDC, posX, posY, NULL);
								}
						}
						DeleteObject(PenLink);
					}
				}

				for (int idPrt = 0; idPrt < nTrace; idPrt++)
				{
					int Pwr2Id = 1 << idPrt;

					if ((m_pData->DrawSet.ShowItems & Pwr2Id))
					{
						nPoints = m_pData->Group->m_Array[idPrt]->m_Data->CurPos + 1;
						HPEN PenPtr = CreatePen(PS_SOLID, 1, mRGB[idPrt % 16]);

						pData = m_pData->Group->m_Array[idPrt]->m_Data->m_Array;

						int iStep = nPoints >> 13;
						if (iStep < 1) iStep = 1;
						SelectObject(hDC, PenPtr);

						int posX = (int)(m_pData->DrawSet.m_00_Pos.X + (((double)pData[0].position.X) * m_pData->DrawSet.ViewMltpl));
						int posY = (int)(m_pData->DrawSet.m_00_Pos.Y + (((double)pData[0].position.Y) * m_pData->DrawSet.ViewMltpl));

						MoveToEx(hDC, posX + 2, posY + 2, NULL);
						LineTo(hDC, posX - 2, posY + 2);
						MoveToEx(hDC, posX - 2, posY + 2, NULL);
						LineTo(hDC, posX + 2, posY - 2);
						MoveToEx(hDC, posX + 2, posY - 2, NULL);
						LineTo(hDC, posX - 2, posY - 2);
						MoveToEx(hDC, posX - 2, posY - 2, NULL);
						LineTo(hDC, posX + 2, posY + 2);

						for (int i = 1; i < nPoints; i += iStep)
						{
							posX = (int)(m_pData->DrawSet.m_00_Pos.X + (((double)pData[i].position.X) * m_pData->DrawSet.ViewMltpl));
							posY = (int)(m_pData->DrawSet.m_00_Pos.Y + (((double)pData[i].position.Y) * m_pData->DrawSet.ViewMltpl));

							LineTo(hDC, posX, posY);
							MoveToEx(hDC, posX, posY, NULL);
						}
						DeleteObject(PenPtr);
					}
				}
			}
		}
		catch (...) { ; }

		
		LeaveCriticalSection(&m_pData->Group->m_cs);
	}


}

void CDrawDlg::OnBnClickedCancel()
{
    // TODO: Add your control notification handler code here
    CDialogEx::OnCancel();
}

//void CDrawDlg::OnDeltaposSpinmagnific(NMHDR* pNMHDR, LRESULT* pResult)
//{
//	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
//	if (pNMUpDown)
//	{
//		int tmpPos = pNMUpDown->iPos + pNMUpDown->iDelta;
//		m_SpinMagn.SetPos(tmpPos);
//		m_Mltpl = 1000.0;
//		double dlt = tmpPos >= 0 ? 1.5 : 0.7;
//		if (tmpPos < 0) tmpPos = -tmpPos;
//
//		while (tmpPos-- > 0) m_Mltpl *= dlt;
//
//		RedrawMe();
//	}
//
//	*pResult = 0;
//}

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

double CDrawDlg::SetMltpl(double in_Mltpl)
{
	double ret = m_pData->DrawSet.ViewMltpl;

	m_pData->DrawSet.ViewMltpl = in_Mltpl;

	m_SliderMgnf.SetRange(0, 140);

	int posMgn;
	GetMgnPos(&posMgn, NULL, m_pData->DrawSet.ViewMltpl, 140);
	m_SliderMgnf.SetPos(posMgn);

	return ret;
}

void CDrawDlg::Set00Point(SPos &in_00, bool bRedraw)
{
	m_pData->DrawSet.m_00_Pos = in_00;
	m_SliderStartX.SetPos((int)m_pData->DrawSet.m_00_Pos.X);
	m_SliderStartY.SetPos((int)m_pData->DrawSet.m_00_Pos.Y);

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
	si.nPos = 0;
	si.nTrackPos = 0;
	SetScrollInfo(SB_HORZ, &si, TRUE);

	si.nMax = (int)m_pData->DrawSet.BMP_Size.Y;
	SetScrollInfo(SB_VERT, &si, TRUE);

	SetMltpl(m_pData->DrawSet.ViewMltpl);

	m_SliderStartX.SetRange(0, (int)m_pData->DrawSet.BMP_Size.X);
	m_SliderStartY.SetRange(0, (int)m_pData->DrawSet.BMP_Size.Y);

	m_SliderStartX.SetPos((int)m_pData->DrawSet.m_00_Pos.X);
	m_SliderStartY.SetPos((int)m_pData->DrawSet.m_00_Pos.Y);


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
		GetMgnPos(&PosValue, NULL, m_pData->DrawSet.ViewMltpl, 140);
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
			GetMgnPos(NULL, &m_pData->DrawSet.ViewMltpl, nPos, 140);
			RedrawMe();
		}
	}
	else if (WndId == IDC_SLIDERSTARTY)
	{
		bool bRedraw = false;
		nPos = m_SliderStartY.GetPos();
		if (nPos != m_pData->DrawSet.m_00_Pos.Y)
		{
			bRedraw = true;
			m_pData->DrawSet.m_00_Pos.Y = nPos;
		}

		switch (nSBCode)
		{
		case SB_PAGEUP:				break;
		case SB_PAGEDOWN:			break;
		case SB_LINEUP:				break;
		case SB_LINEDOWN:			break;
		case SB_THUMBPOSITION:		bRedraw = true; break;
		case SB_ENDSCROLL:			break;
		case SB_TOP:				break;
		case SB_BOTTOM:				break;
		case SB_THUMBTRACK:			bRedraw = true; 	break;
		default:		break;
		}
		if (bRedraw)
		{
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
	else if (WndId == IDC_SLIDERSTARTX)
	{
		bool bRedraw = false;
		nPos = m_SliderStartX.GetPos();
		if (nPos != m_pData->DrawSet.m_00_Pos.X)
		{
			bRedraw = true;
			m_pData->DrawSet.m_00_Pos.X = nPos;
		}

		switch (nSBCode)
		{
		case SB_PAGEUP:				break;
		case SB_PAGEDOWN:			break;
		case SB_LINEUP:				break;
		case SB_LINEDOWN:			break;
		case SB_THUMBPOSITION:		bRedraw = true; break;
		case SB_ENDSCROLL:			break;
		case SB_TOP:				break;
		case SB_BOTTOM:				break;
		case SB_THUMBTRACK:			bRedraw = true;	break;
		default:		break;
		}
		if (bRedraw)
		{
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


void CDrawDlg::OnBnClickedButtonrefresh()
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




///void SaveBitmapTest(char* in_FileName, HDC in_hdc, HBITMAP in_hBMP, HWND hWnd);

void CDrawDlg::OnBnClickedButtonrefresh2()
{
	if (m_pData)
	{
		SDrawSettings oldDrawSet = m_pData->DrawSet;

		SPos minPos = { 0 };
		SPos maxPos = { 0 };
		SPos TraceSize = { 0 };
		int BMP_Size = 1000;

		m_pData->DrawSet.BMP_Size.X = BMP_Size;
		m_pData->DrawSet.BMP_Size.Y = BMP_Size;

		m_pData->Group->GetMinMaxTracePos(minPos, maxPos);


		TraceSize.X = maxPos.X - minPos.X;
		TraceSize.Y = maxPos.Y - minPos.Y;

		if (TraceSize.X > TraceSize.Y) m_pData->DrawSet.ViewMltpl = BMP_Size / TraceSize.X;
		else m_pData->DrawSet.ViewMltpl = BMP_Size / TraceSize.Y;

		m_pData->DrawSet.ViewMltpl *= 0.95;

		m_pData->DrawSet.PosMax = maxPos;
		m_pData->DrawSet.PosMin = minPos;

		m_pData->DrawSet.m_00_Pos.X = (BMP_Size - TraceSize.X * m_pData->DrawSet.ViewMltpl) * 0.5 - minPos.X * m_pData->DrawSet.ViewMltpl;
		m_pData->DrawSet.m_00_Pos.Y = (BMP_Size - TraceSize.Y * m_pData->DrawSet.ViewMltpl) * 0.5 - minPos.Y * m_pData->DrawSet.ViewMltpl;

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
			PaintTraceToCDC(&DCMem, m_pData, SP_DRAW_XY, tmp);

			// Select the old bitmap back into the device context.

			//SaveBitmapTest("C:\\SParticle\\test.bmp", DCMem, hBitmap, m_hWnd);
			SaveBitmap("C:\\SParticle\\test.bmp", DCMem, hBitmap);

			DCMem.SelectObject(hOldBitmap);
			DeleteObject(hBitmap);
			hBitmap = NULL;
		}

		m_pData->DrawSet = oldDrawSet;

	}


}
