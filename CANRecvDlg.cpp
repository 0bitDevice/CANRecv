
// CANRecvDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "CANRecv.h"
#include "CANRecvDlg.h"
#include "afxdialogex.h"
#include "ControlCAN.h"
#include <locale.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

constexpr auto CANREVMAXPACKNUM = 200;
#define	REFRESHRECVEDITEVET		1

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CCANRecvDlg 对话框



CCANRecvDlg::CCANRecvDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CANRECV_DIALOG, pParent)
	, m_strRecvEdit(_T(""))
	, m_strUTCEdit(_T(""))
	, m_strNavstateEdit(_T(""))
	, m_strLongitudeEdit(_T(""))
	, m_strLatitudeEdit(_T(""))
	, m_strHeightEdit(_T(""))
	, m_strFlightangleEdit(_T(""))
	, m_strSkyspeedEdit(_T(""))
	, m_strGroundspeedEdit(_T(""))
	, m_strGPSnumEdit(_T(""))
	, m_strBDnumEdit(_T(""))
{
	m_bDevOpended = FALSE;
	m_DevIndex = 0;
	m_pReceiveThread = NULL;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCANRecvDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_OPENDEV, m_OpenDevBut);
	DDX_Control(pDX, IDC_EDIT_RECV, m_RecvEdit);
	DDX_Control(pDX, IDC_CHECK_SAVEDATA, m_RecordRecvChk);
	DDX_Text(pDX, IDC_EDIT_RECV, m_strRecvEdit);
	DDX_Text(pDX, IDC_EDIT_UTC, m_strUTCEdit);
	DDX_Text(pDX, IDC_EDIT_NAVSTATE, m_strNavstateEdit);
	DDX_Text(pDX, IDC_EDIT_LONGITUDE, m_strLongitudeEdit);
	DDX_Text(pDX, IDC_EDIT_LATITUDE, m_strLatitudeEdit);
	DDX_Text(pDX, IDC_EDIT_HEIGHT, m_strHeightEdit);
	DDX_Text(pDX, IDC_EDIT_FLIGHTANGLE, m_strFlightangleEdit);
	DDX_Text(pDX, IDC_EDIT_SKYSPEED, m_strSkyspeedEdit);
	DDX_Text(pDX, IDC_EDIT_GROUNDSPPED, m_strGroundspeedEdit);
	DDX_Text(pDX, IDC_EDIT_GPSNUM, m_strGPSnumEdit);
	DDX_Text(pDX, IDC_EDIT_BDNUM, m_strBDnumEdit);
	DDX_Control(pDX, IDC_EDIT_FILTERID, m_FilterEdit);
}

BEGIN_MESSAGE_MAP(CCANRecvDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OPENDEV, &CCANRecvDlg::OnBnClickedButtonOpendev)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHECK_SAVEDATA, &CCANRecvDlg::OnBnClickedCheckSavedata)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CCANRecvDlg 消息处理程序

BOOL CCANRecvDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	HFONT  hFont = (HFONT)::GetStockObject(SYSTEM_FIXED_FONT);
	CFont* pFont = CFont::FromHandle(hFont);
	m_RecvEdit.SetFont(pFont);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CCANRecvDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CCANRecvDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CCANRecvDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CCANRecvDlg::OnBnClickedButtonOpendev()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_bDevOpended)
	{
		DWORD Reserved = 0;
		//打开设备
		if (VCI_OpenDevice(VCI_USBCAN2, m_DevIndex, Reserved) != 1)
		{
			MessageBox(_T("打开设备失败"));
			return;
		}
		VCI_INIT_CONFIG InitInfo[1];
		InitInfo->Timing0 = 0x00;		//暂时默认按1M接收
		InitInfo->Timing1 = 0x14;
		InitInfo->Filter = 2;
		InitInfo->AccCode = 0x80000008;
		InitInfo->AccMask = 0xFFFFFFFF;
		InitInfo->Mode = 0;
		//初始化通道0
		if (VCI_InitCAN(VCI_USBCAN2, m_DevIndex, 0, InitInfo) != 1)	//can-0
		{
			MessageBox(_T("初始化CAN失败"));
			return;
		}
		Sleep(100);
		//初始化通道0
		if (VCI_StartCAN(VCI_USBCAN2, m_DevIndex, 0) != 1)	//can-0
		{
			MessageBox(_T("打开通道失败"));
			return;
		}

		MessageBox(_T("打开设备成功"));

		m_bDevOpended = TRUE;
		m_OpenDevBut.SetWindowText(_T("关闭CAN分析仪"));

		m_pReceiveThread = AfxBeginThread((AFX_THREADPROC)ReceiveThread, this);
		SetTimer(REFRESHRECVEDITEVET, 50, NULL);
	}
	else
	{
		if (VCI_CloseDevice(VCI_USBCAN2, m_DevIndex) != 1)
		{
			MessageBox(_T("关闭设备失败"));
			return;

		}
		m_bDevOpended = FALSE;
		m_OpenDevBut.SetWindowText(_T("打开CAN分析仪"));

		Sleep(10);
		m_pReceiveThread = NULL;
		KillTimer(REFRESHRECVEDITEVET);
		MessageBox(_T("关闭设备成功"));
	}
}
void CCANRecvDlg::DisplayNMEAMsg(CString& strMsg)
{
	CString strLine = strMsg;
	CString strHour, strMin, strSec, strMillisec;
	double fLongtitude, fLatitude;
	short skySpeed, height, groundSpeed;
	double fSkySpeed, fHeight, fGroundSpeed;

	CString msgBuff[32];

	for (int i = 0; i < 32; ++i)
	{
		msgBuff[i] = strLine.Left(2);
		strLine.Delete(0, 3);
	}

	strHour.Format(_T("%u"), _tcstol(msgBuff[2], NULL, 16));
	strMin.Format(_T("%u"), _tcstol(msgBuff[3], NULL, 16));
	strSec.Format(_T("%u"), _tcstol(msgBuff[4], NULL, 16));
	strMillisec.Format(_T("%u00"), _tcstol(msgBuff[1], NULL, 16));
	m_strUTCEdit = strHour + _T(":") + strMin + _T(":") + strSec + _T(".") + strMillisec;

	if (msgBuff[5] == _T("00"))
		m_strNavstateEdit = _T("不定位");
	else
		m_strNavstateEdit = _T("定位");

	fLongtitude = (double)_tcstol(msgBuff[10] + msgBuff[11] + msgBuff[12] + msgBuff[13], NULL, 16) / 10000000;
	fLatitude = (double)_tcstol(msgBuff[18] + msgBuff[19] + msgBuff[20] + msgBuff[21], NULL, 16) / 10000000;
	m_strLongitudeEdit.Format(_T("%.7f"), fLongtitude);
	m_strLatitudeEdit.Format(_T("%.7f"), fLatitude);

	m_strGPSnumEdit.Format(_T("%u"), _tcstol(msgBuff[30], NULL, 16) / 16);
	m_strBDnumEdit.Format(_T("%u"), _tcstol(msgBuff[30], NULL, 16) % 16);

	height = (short)_tcstol(msgBuff[28] + msgBuff[29], NULL, 16);
	if ((height & 0x8000) == 0x8000)
	{
		fHeight = -(float)(((~height) + 1)) * 0.5;
	}
	else
	{
		fHeight = (float)(height) * 0.5;
	}
	m_strHeightEdit.Format(_T("%.1f"), fHeight);

	skySpeed = (short)_tcstol(msgBuff[22] + msgBuff[23], NULL, 16);
	if ((skySpeed & 0x8000) == 0x8000)
	{
		fSkySpeed = -(float)(((~skySpeed) + 1)) * 0.1;
	}
	else
	{
		fSkySpeed = (float)(skySpeed) * 0.1;
	}
	m_strSkyspeedEdit.Format(_T("%.1f"), fSkySpeed);

	groundSpeed = (short)_tcstol(msgBuff[14] + msgBuff[15], NULL, 16);
	if ((groundSpeed & 0x8000) == 0x8000)
	{
		fGroundSpeed = -(float)(((~groundSpeed) + 1)) * 0.1;
	}
	else
	{
		fGroundSpeed = (float)(groundSpeed) * 0.1;
	}
	m_strGroundspeedEdit.Format(_T("%.1f"), fGroundSpeed);
	m_strFlightangleEdit.Format(_T("%.1f"), (float)_tcstol(msgBuff[26] + msgBuff[27], NULL, 16) * 0.1);
}
DWORD CCANRecvDlg::ReceiveThread(LPVOID pParam)
{
	CCANRecvDlg* pCCANRecvDlg = (CCANRecvDlg*)pParam;
	VCI_CAN_OBJ pCanObj[CANREVMAXPACKNUM];
	CList<VCI_CAN_OBJ, VCI_CAN_OBJ&> listVCI_CAN_OBJ;
	int NumValue = 0;
	
	while (pCCANRecvDlg->m_bDevOpended)
	{
		//调用动态链接看接收函数
		NumValue = VCI_Receive(VCI_USBCAN2, pCCANRecvDlg->m_DevIndex, 0, pCanObj, CANREVMAXPACKNUM, 0);
		if (NumValue > 0)
		{
			for (int num = 0; num < NumValue; ++num)
			{
				listVCI_CAN_OBJ.AddHead(pCanObj[num]);
				if (listVCI_CAN_OBJ.GetSize() > 3)
				{
					POSITION pos = listVCI_CAN_OBJ.GetTailPosition();
					BYTE CANframe01 = listVCI_CAN_OBJ.GetTail().Data[0];
					BYTE CANframe02 = listVCI_CAN_OBJ.GetPrev(pos).Data[0];
					BYTE CANframe03 = listVCI_CAN_OBJ.GetPrev(pos).Data[0];
					BYTE CANframe04 = listVCI_CAN_OBJ.GetPrev(pos).Data[0];
					if ((CANframe01 == CANframe02) 
						&& (CANframe02 == CANframe03) 
						&& (CANframe03 == CANframe04))
					{
						CString strTemp, strPack;
						while (listVCI_CAN_OBJ.GetSize() > 0)
						{
							for (int i = 0; i < 8; ++i)	//数据信息
							{
								strTemp.Format(_T("%02X "), listVCI_CAN_OBJ.GetTail().Data[i]);
								strPack += strTemp;
							}
							listVCI_CAN_OBJ.RemoveTail();
						}
						strPack += _T("\r\n");

						pCCANRecvDlg->m_strRecvEdit += strPack;
						pCCANRecvDlg->DisplayNMEAMsg(strPack);

						if (pCCANRecvDlg->m_bRecordRecv && pCCANRecvDlg->m_fileRecv.m_pStream)
						{
							int buffLen = WideCharToMultiByte(CP_ACP, 0, strPack, -1, NULL, 0, NULL, NULL);
							char* MultiByteBuff = (char*)calloc(buffLen, sizeof(char));
							WideCharToMultiByte(CP_ACP, 0, strPack, -1, MultiByteBuff, buffLen, NULL, NULL);

							pCCANRecvDlg->m_fileRecv.Write(MultiByteBuff, buffLen - 1);
							pCCANRecvDlg->m_fileRecv.Flush();

							free(MultiByteBuff);
						}
					}
					else
					{
						listVCI_CAN_OBJ.RemoveAt(listVCI_CAN_OBJ.GetTailPosition());
					}
				}
			}
		}
		Sleep(10);
	}
	
	return 0;
}

void CCANRecvDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nIDEvent)
	{
		case REFRESHRECVEDITEVET:			//接收区显示刷新的定时器
		{
			UpdateData(FALSE);
			int RecvStrLen = m_RecvEdit.GetWindowTextLength();
			m_RecvEdit.SetSel(RecvStrLen, RecvStrLen);
		}
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CCANRecvDlg::OnBnClickedCheckSavedata()
{
	// TODO: 在此添加控件通知处理程序代码
	BOOL bFLag = m_RecordRecvChk.GetCheck();
	setlocale(LC_CTYPE, ("chs"));
	LPCTSTR szFilter = _T("文件 (*.txt)|*.txt|所有文件 (*.*)|*.*||");
	CFileDialog fd(FALSE, _T("*.txt"), NULL, OFN_HIDEREADONLY, szFilter);

	if (m_fileRecv.m_pStream && !bFLag)
	{
		m_fileRecv.Close();
		MessageBox(_T("保存成功！"));
	}
	else
	{
		if (IDCANCEL == fd.DoModal())
			bFLag = FALSE;

		if (!m_fileRecv.Open(fd.GetPathName(), CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyNone | CFile::typeBinary))
		{
			MessageBox(_T("保存失败！"));
			bFLag = FALSE;
		}
		else
		{
			bFLag = TRUE;
		}
	}
	
	m_RecordRecvChk.SetCheck(bFLag);
	m_bRecordRecv = bFLag;
}


void CCANRecvDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_bDevOpended = FALSE;
	VCI_CloseDevice(VCI_USBCAN2, m_DevIndex);

	if (m_fileRecv.m_pStream)
	{
		m_fileRecv.Close();
	}
	CDialogEx::OnClose();
}
