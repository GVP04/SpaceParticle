#pragma once
#include "afxdialogex.h"
#include "CSpParticle.h"


// CFindDlg dialog

class CFindDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFindDlg)

public:
	CFindDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CFindDlg();
	SP_CalcFind* m_pData;

	int m_uRedrawFlag;



// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOGFINDSP };
#endif

private:
	HBITMAP m_hDCBitmap;
	SPos m_BMP_Size;  //размер картинки
	void PaintToCDC(CDC* DCMem);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonshowtrace();
	afx_msg void OnBnClickedButtonshowtrace2();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonshowtrace3();
	afx_msg void OnBnClickedButtonrefresh2();
};
