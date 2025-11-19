#pragma once
#include "afxdialogex.h"
#include "CSpParticle.h"

// CDrawDlg dialog

class CDrawDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDrawDlg)

public:
	CDrawDlg(SP_Calc* in_pData, CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDrawDlg();

	double SetMltpl(double in_Mltpl);


	double CDrawDlg::SetViewPrc(double in_Prc);
	double CDrawDlg::SetViewSpread(double in_Spread);

	void Set00Point(SPos& in_00, bool bRedraw = false);
	void SetWindowPos(SPos& in_WPos, bool bRedraw = false);

	SP_Calc* m_pData;
	int m_uRedrawFlag;
	double m_ViewMltpl;
	double m_ViewPrc;
	double m_ViewSpread;


// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOGDRAW };
#endif

private:
	SPos m_WindowPos; //сдвиг окна просмотра от вершины картинки
	HBITMAP m_hDCBitmap;
	int m_XYZ;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnBnClickedCancel();
//	afx_msg void OnDeltaposSpinmagnific(NMHDR* pNMHDR, LRESULT* pResult);

	virtual BOOL OnInitDialog();
	void RedrawMe();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	afx_msg void OnReleasedcaptureSlidermgnf(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnThumbposchangingSlidermgnf(NMHDR* pNMHDR, LRESULT* pResult);
	CSliderCtrl m_SliderMgnf;
	afx_msg void OnBnClicked_Refresh();
	afx_msg void OnBnClicked_Save();
	afx_msg void OnBnClicked_UpLeft();
	afx_msg void OnBnClicked_Center();
	CSliderCtrl m_SliderSpread;
	CSliderCtrl m_SliderViewPrc;
	afx_msg void OnBnClickedButtonXyz();
};
