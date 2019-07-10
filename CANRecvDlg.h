
// CANRecvDlg.h: 头文件
//

#pragma once
#include <afxmt.h>

// CCANRecvDlg 对话框
class CCANRecvDlg : public CDialogEx
{
// 构造
public:
	CCANRecvDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
public:
	int			m_DevIndex;
	BOOL		m_bDevOpended;
	CString		m_RecvStrBuff;
	BOOL		m_bRecordRecv;
	CStdioFile	m_fileRecv;
	CEvent*		m_pThreadStopEvnt;
	CWinThread*	m_pReceiveThread;
	static DWORD ReceiveThread(LPVOID pParam);
	void DisplayNMEAMsg(CString& strMsg);
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CANRECV_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonOpendev();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedCheckSavedata();
	afx_msg void OnClose();
	CButton		m_OpenDevBut;
	CEdit		m_RecvEdit;
	CButton		m_RecordRecvChk;
	CString		m_strRecvEdit;
	CString		m_strUTCEdit;
	CString		m_strNavstateEdit;
	CString		m_strLongitudeEdit;
	CString		m_strLatitudeEdit;
	CString		m_strHeightEdit;
	CString		m_strFlightangleEdit;
	CString		m_strSkyspeedEdit;
	CString		m_strGroundspeedEdit;
	CString		m_strGPSnumEdit;
	CString		m_strBDnumEdit;
public:
	CEdit m_FilterEdit;
};
