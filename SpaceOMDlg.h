
// SpaceOMDlg.h : header file
//

#pragma once
#include "CSP_Group.h"

#define M_NCALCFIND		5


// CSpaceOMDlg dialog
class CSpaceOMDlg : public CDialogEx
{
// Construction
public:
	CSpaceOMDlg(CWnd* pParent = nullptr);	// standard constructor
	~CSpaceOMDlg();	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SPACEOM_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	void DeleteAll();

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	SP_Calc m_Calc_1;
	SP_CalcFind m_Calc_FIND[M_NCALCFIND];

	virtual BOOL DestroyWindow();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClicked_RefreshTrace();
	afx_msg void OnBnClicked_ShowTrace();
	afx_msg void OnBnClicked_StartFind();
	afx_msg void OnBnClicked_CalcSimple();
	afx_msg void OnBnClicked_StopCalc();
};
