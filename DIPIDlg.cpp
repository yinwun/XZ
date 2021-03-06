
// DIPIDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DIPI.h"
#include "DIPIDlg.h"
#include "afxdialogex.h"
#include "DpMain.h"
#include "AccountEdit.h"
#include "tlhelp32.h"
#include "smtp/zstring.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma region 变量

int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2,LPARAM lParamSort);

//排序方式，升序或降序
BOOL m_bASC=TRUE;

//当前排序的列
int m_nSordCol=0;

CALLBACK_Func f_Callback = NULL;

WPARAM f_Param = NULL;

//用于向线程传递参数
THREADPARA g_para[100];

USERINFO *g_userinfo=NULL;

int g_userNum=0;

SERVERINFO g_serverinfo;

long gametime;

CDpMain *pDp=NULL;

#pragma endregion  

#pragma region 回调函数

//设置回调函数及其参数
void Set_Callback (CALLBACK_Func func,WPARAM wParam )
{
	f_Callback = func;
	f_Param = wParam;
}
//回调函数
void Callback_func(WPARAM wParam,NOTIFYPARA *pNotifyPara)
{
	CDIPIDlg *p = (CDIPIDlg *)wParam;
	ASSERT ( p);	
	p->CallbackMsg(pNotifyPara);
}
//回调函数，由其他类调用
void Callback_Notify(NOTIFYPARA *pNotifyPara)
{
	if(f_Callback)
		f_Callback(f_Param,pNotifyPara);
}

#pragma endregion 

#pragma region 基础函数

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedGetregcode();
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)	
	ON_BN_CLICKED(ID_GETREGCODE, &CAboutDlg::OnBnClickedGetregcode)
END_MESSAGE_MAP()

// CDIPIDlg 对话框
CDIPIDlg::CDIPIDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDIPIDlg::IDD, pParent)
	, m_line(7)
	, m_autorun(FALSE)	
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	pDp=NULL;
	TimerOn=0;
	gametime=0;
}

void CDIPIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTCHAR, m_list);
	DDX_CBIndex(pDX, IDC_LINE, m_line);
	DDX_Control(pDX, IDC_TIME, m_time);
	DDX_Control(pDX, IDC_MESSAGE, m_message);
	DDX_Control(pDX, IDC_LIST2, m_charinfo);
	DDX_Check(pDX, IDC_CHK_RUNSCRIPT, m_autorun);
}

BEGIN_MESSAGE_MAP(CDIPIDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_APPEND, &CDIPIDlg::OnBnClickedAppend)
	ON_BN_CLICKED(IDC_DELETE, &CDIPIDlg::OnBnClickedDelete)
	ON_BN_CLICKED(IDC_CLEAR, &CDIPIDlg::OnBnClickedClear)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_START, &CDIPIDlg::OnBnClickedStart)	
	ON_BN_CLICKED(IDC_STOP, &CDIPIDlg::OnBnClickedStop)
	ON_COMMAND(ID_MENU_RUN, &CDIPIDlg::OnMenuRun)
	ON_COMMAND(ID_MENU_STOP, &CDIPIDlg::OnMenuStop)
	ON_COMMAND(ID_MENU_PAUSE, &CDIPIDlg::OnMenuPause)
	ON_COMMAND(ID_SINGLELOGIN, &CDIPIDlg::OnSinglelogin)
	ON_COMMAND(ID_SINGLELOGOUT, &CDIPIDlg::OnSinglelogout)
	ON_NOTIFY(NM_CLICK, IDC_LISTCHAR, &CDIPIDlg::OnNMClickListchar)
	ON_NOTIFY(NM_RCLICK, IDC_LISTCHAR, &CDIPIDlg::OnNMRClickListchar)
	ON_BN_CLICKED(IDC_CHkTALK, &CDIPIDlg::OnBnClickedChktalk)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST2, &CDIPIDlg::OnLvnColumnclickList2)
	ON_BN_CLICKED(IDC_SAVE, &CDIPIDlg::OnBnClickedSave)
	ON_NOTIFY(NM_DBLCLK, IDC_LISTCHAR, &CDIPIDlg::OnNMDblclkListchar)
	ON_BN_CLICKED(IDC_CHKUPDATE, &CDIPIDlg::OnBnClickedChkupdate)
END_MESSAGE_MAP()

// CDIPIDlg 消息处理程序

BOOL CDIPIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	//默认界面实时更新
	CButton * pbox=(CButton *)GetDlgItem(IDC_CHKUPDATE);
	pbox->SetCheck(1);

	LONG styles; 
	styles=m_charinfo.GetExtendedStyle(); 
	//LVS_EX_FULLROWSELECT整行选中
	//LVS_EX_GRIDLINES加网格线
	//LVS_EX_CHECKBOXES前面加复选框
	m_charinfo.SetExtendedStyle(styles|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	LV_COLUMN lvColumn; 
	lvColumn.mask=LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM; 
	lvColumn.fmt=LVCFMT_LEFT; 
	lvColumn.cx=70; 
	lvColumn.iSubItem=0; // 第一列 
	lvColumn.pszText=_T("人物"); 
	m_charinfo.InsertColumn(0,&lvColumn); 
	lvColumn.cx=40;
	lvColumn.iSubItem=1; // 第二列 
	lvColumn.pszText=_T("Level"); 
	m_charinfo.InsertColumn(1,&lvColumn); 
	lvColumn.cx=80; 
	lvColumn.iSubItem=2; 
	lvColumn.pszText=_T("经验"); 
	m_charinfo.InsertColumn(2,&lvColumn);
	lvColumn.cx=80; 
	lvColumn.iSubItem=3; 
	lvColumn.pszText=_T("Next"); 
	m_charinfo.InsertColumn(3,&lvColumn); 
	lvColumn.cx=70; 
	lvColumn.iSubItem=4;
	lvColumn.pszText=_T("HP"); 
	m_charinfo.InsertColumn(4,&lvColumn);
	lvColumn.iSubItem=5; 
	lvColumn.pszText=_T("MP"); 
	m_charinfo.InsertColumn(5,&lvColumn);
	lvColumn.cx=40; 
	lvColumn.iSubItem=6;  
	lvColumn.pszText=_T("攻击"); 
	m_charinfo.InsertColumn(6,&lvColumn); 
	lvColumn.iSubItem=7; 
	lvColumn.pszText=_T("防御"); 
	m_charinfo.InsertColumn(7,&lvColumn); 
	lvColumn.iSubItem=8;
	lvColumn.pszText=_T("敏捷"); 
	m_charinfo.InsertColumn(8,&lvColumn);
	lvColumn.iSubItem=9;  
	lvColumn.pszText=_T("魅力"); 
	m_charinfo.InsertColumn(9,&lvColumn);
	lvColumn.iSubItem=10;  
	lvColumn.pszText=_T("转生"); 
	m_charinfo.InsertColumn(10,&lvColumn);
	lvColumn.cx=60; 
	lvColumn.iSubItem=11; 
	lvColumn.pszText=_T("属性"); 
	m_charinfo.InsertColumn(11,&lvColumn);
	lvColumn.cx=40; 
	lvColumn.iSubItem=12;
	lvColumn.pszText=_T("体力"); 
	m_charinfo.InsertColumn(12,&lvColumn);
	lvColumn.iSubItem=13;  
	lvColumn.pszText=_T("腕力"); 
	m_charinfo.InsertColumn(13,&lvColumn);
	lvColumn.iSubItem=14;
	lvColumn.pszText=_T("耐力"); 
	m_charinfo.InsertColumn(14,&lvColumn);
	lvColumn.iSubItem=15;  
	lvColumn.pszText=_T("速度"); 
	m_charinfo.InsertColumn(15,&lvColumn);
	lvColumn.cx=80; 
	lvColumn.iSubItem=16;  
	lvColumn.pszText=_T("DP"); 
	m_charinfo.InsertColumn(16,&lvColumn);
	lvColumn.cx=80; 
	lvColumn.iSubItem=17;  
	lvColumn.pszText=_T("现金"); 
	m_charinfo.InsertColumn(17,&lvColumn);
	lvColumn.cx=80; 
	lvColumn.iSubItem=18;  
	lvColumn.pszText=_T("声望"); 
	m_charinfo.InsertColumn(18,&lvColumn);
	lvColumn.cx=60; 
	lvColumn.iSubItem=19;  
	lvColumn.pszText=_T("升级点数"); 
	m_charinfo.InsertColumn(19,&lvColumn);
	lvColumn.cx=50; 
	lvColumn.iSubItem=20;  
	lvColumn.pszText=_T("地图"); 
	m_charinfo.InsertColumn(20,&lvColumn);
	lvColumn.cx=120; 
	lvColumn.iSubItem=21;  
	lvColumn.pszText=_T("地图名称"); 
	m_charinfo.InsertColumn(21,&lvColumn);
	lvColumn.cx=60; 
	lvColumn.iSubItem=22;  
	lvColumn.pszText=_T("坐标"); 
	m_charinfo.InsertColumn(22,&lvColumn);
	lvColumn.cx=40; 
	lvColumn.iSubItem=23;  
	lvColumn.pszText=_T("状态"); 
	m_charinfo.InsertColumn(23,&lvColumn);
	lvColumn.cx=150; 
	lvColumn.iSubItem=24;  
	lvColumn.pszText=_T("当前脚本"); 
	m_charinfo.InsertColumn(24,&lvColumn);
	lvColumn.cx=40; 
	lvColumn.iSubItem=25;  
	lvColumn.pszText=_T("回合"); 
	m_charinfo.InsertColumn(25,&lvColumn);
	//帐号列表的设置
	styles=m_list.GetExtendedStyle(); 	
	m_list.SetExtendedStyle(styles|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	lvColumn.mask=LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM; 
	lvColumn.fmt=LVCFMT_LEFT; 
	lvColumn.cx=70; 
	lvColumn.iSubItem=0; // 第一列 
	lvColumn.pszText=_T("人物"); 
	m_list.InsertColumn(0,&lvColumn); 
	lvColumn.cx=60;
	lvColumn.iSubItem=1; // 第二列 
	lvColumn.pszText=_T("密码"); 
	m_list.InsertColumn(1,&lvColumn); 
	lvColumn.cx=60;
	lvColumn.iSubItem=2; // 第二列 
	lvColumn.pszText=_T("安全码"); 
	m_list.InsertColumn(2,&lvColumn); 
	lvColumn.cx=100;
	lvColumn.iSubItem=3; // 第二列 
	lvColumn.pszText=_T("脚本"); 
	m_list.InsertColumn(3,&lvColumn); 

	//读取用户信息
	CString szAccount,szPwd,szSafeCode,szScript,szVal;
	int nRow,pos,i=0;
	try{
		CString szVal;
		CStdioFile f(_T("data.txt"),CFile::modeRead|CFile::typeText);
		CDIPIApp *pApp=(CDIPIApp *)AfxGetApp();
		while(f.ReadString(szVal) && i<pApp->MAXACCOUNT){
			i++;
			pos=szVal.Find(_T("\r"));
			if(pos>0)
				szVal=szVal.Left(pos);
			else{
				pos=szVal.Find(_T("\n"));
				if(pos>0)
					szVal=szVal.Left(pos);
			}
			//if(pos>0){
				pos=0;
				szAccount=szVal.Tokenize("|",pos);
				szPwd=szVal.Tokenize("|",pos);
				szSafeCode=szVal.Tokenize("|",pos);
				szScript=szVal.Tokenize("|",pos);
				nRow = m_list.InsertItem(m_list.GetItemCount(),szAccount);
				m_list.SetItemText(nRow, 1, szPwd);
				m_list.SetItemText(nRow, 2, szSafeCode);
				m_list.SetItemText(nRow, 3, szScript);	
			//}
		}
		f.Close();		
	}
	catch(...){
	}
	//帐号处理
	g_userNum=m_list.GetItemCount();
	if(g_userNum>0){
		g_userinfo= new USERINFO[g_userNum];
		for(int i=0;i<g_userNum;i++){
			szAccount=m_list.GetItemText(i,0);
			szPwd=m_list.GetItemText(i,1);
			szSafeCode=m_list.GetItemText(i,2);
			szScript=m_list.GetItemText(i,3);
			strcpy_s(g_userinfo[i].charname,szAccount);
			strcpy_s(g_userinfo[i].password,szPwd);
			strcpy_s(g_userinfo[i].safecode,szSafeCode);
			strcpy_s(g_userinfo[i].scriptName,szScript);			
			g_userinfo[i].index=i;
		}
		pDp=new CDpMain[g_userNum];
	}
	else
		g_userinfo=NULL;	
	gametime=0;
	Set_Callback(Callback_func, WPARAM(this));

	GetDlgItem(IDC_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_START)->EnableWindow(TRUE);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDIPIDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。
void CDIPIDlg::OnPaint()
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

#pragma endregion 

#pragma region 帐号列表处理

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CDIPIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
//向帐号列表上添加一项
void CDIPIDlg::OnBnClickedAppend()
{
	CString szAccount,szPwd,szSafeCode,szScript,szVal;
	GetDlgItem(IDC_ACCOUNT)->GetWindowText(szAccount);
	if(szAccount.IsEmpty()){
		AfxMessageBox(_T("请输入帐号！"));
		return;
	}
	GetDlgItem(IDC_PASSWORD)->GetWindowText(szPwd);
	if(szPwd.IsEmpty()){
		AfxMessageBox(_T("请输入密码！"));
		return;
	}
	GetDlgItem(IDC_SAFECODE)->GetWindowText(szSafeCode);
	if(szSafeCode.IsEmpty()){
		AfxMessageBox(_T("请输入安全码！"));
		return;
	}
	GetDlgItem(IDC_SCRIPTNAME)->GetWindowText(szScript);
	if(szSafeCode.IsEmpty()){
		AfxMessageBox(_T("请输入本号要运行的脚本名！"));
		return;
	}
	int num;
	CDIPIApp *pApp=(CDIPIApp *)AfxGetApp();
	if(m_list.GetItemCount()>=pApp->MAXACCOUNT){
		AfxMessageBox("外挂未注册，帐户数目受限制!注册请与作者QQ:1515149834联系！");
		return;
	}
	num=m_list.GetItemCount();
	for(int i=0;i<num;i++){
		szVal=m_list.GetItemText(i,0);
		if(szVal.CompareNoCase(szAccount)==0)
			return;
	}
	int nRow = m_list.InsertItem(num,szAccount);
	m_list.SetItemText(nRow, 1, szPwd);
	m_list.SetItemText(nRow, 2, szSafeCode);
	m_list.SetItemText(nRow, 3, szScript);

	if(g_userinfo)
		delete []g_userinfo;
	g_userNum=num;
	g_userinfo= new USERINFO[g_userNum];
	for(int i=0;i<g_userNum;i++){
		szAccount=m_list.GetItemText(i,0);
		szPwd=m_list.GetItemText(i,1);
		szSafeCode=m_list.GetItemText(i,2);
		szScript=m_list.GetItemText(i,3);
		strcpy_s(g_userinfo[i].charname,szAccount);
		strcpy_s(g_userinfo[i].password,szPwd);
		strcpy_s(g_userinfo[i].safecode,szSafeCode);
		strcpy_s(g_userinfo[i].scriptName,szScript);
		g_userinfo[i].index=i;
	}
	if(pDp)
		delete []pDp;
	pDp=new CDpMain[g_userNum];	
}

//删除帐号列表上的一项
void CDIPIDlg::OnBnClickedDelete()
{
	CString szAccount,szPwd,szSafeCode,szScript,szVal;
	int index;
	index=(int)m_list.GetFirstSelectedItemPosition()-1;
	if(index==-1){
		AfxMessageBox(_T("请先选择要删除的帐号！"));
		return;
	}
	m_list.DeleteItem(index);
	if(g_userinfo)
		delete []g_userinfo;
	g_userNum=m_list.GetItemCount();
	if(g_userNum>0){
		g_userinfo= new USERINFO[g_userNum];
		for(int i=0;i<g_userNum;i++){
			szAccount=m_list.GetItemText(i,0);
			szPwd=m_list.GetItemText(i,1);
			szSafeCode=m_list.GetItemText(i,2);
			szScript=m_list.GetItemText(i,3);
			strcpy_s(g_userinfo[i].charname,szAccount);
			strcpy_s(g_userinfo[i].password,szPwd);
			strcpy_s(g_userinfo[i].safecode,szSafeCode);
			strcpy_s(g_userinfo[i].scriptName,szScript);
			g_userinfo[i].index=i;
		}
		if(pDp)
			delete []pDp;
		pDp=new CDpMain[g_userNum];		
	}
	else{
		g_userinfo=NULL;
		if(pDp)
			delete []pDp;
		pDp=NULL;		
	}
}

//清空帐号列表
void CDIPIDlg::OnBnClickedClear()
{
	m_list.DeleteAllItems();
	GetDlgItem(IDC_ACCOUNT)->SetWindowText("");
	GetDlgItem(IDC_PASSWORD)->SetWindowText("");
	GetDlgItem(IDC_SAFECODE)->SetWindowText("");
	GetDlgItem(IDC_SCRIPTNAME)->SetWindowText("");
	if(g_userinfo)
		delete g_userinfo;
	g_userinfo=NULL;
	g_userNum=0;
	if(pDp)
		delete []pDp;
	pDp=NULL;	
}

//用户帐号列表框中被选项发生改变时
void CDIPIDlg::OnNMClickListchar(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	int index;
	CString szAccount,szPwd,szSafeCode,szScript,szVal;
	index=(int)m_list.GetFirstSelectedItemPosition()-1;
	szAccount=m_list.GetItemText(index,0);
	szPwd=m_list.GetItemText(index,1);
	szSafeCode=m_list.GetItemText(index,2);
	szScript=m_list.GetItemText(index,3);
	GetDlgItem(IDC_ACCOUNT)->SetWindowText(szAccount);
	GetDlgItem(IDC_PASSWORD)->SetWindowText(szPwd);
	GetDlgItem(IDC_SAFECODE)->SetWindowText(szSafeCode);
	GetDlgItem(IDC_SCRIPTNAME)->SetWindowText(szScript);

	*pResult = 0;
}
//在帐号列表上点击右键时弹出菜单
void CDIPIDlg::OnNMRClickListchar(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	CPoint mousePoint;
	GetCursorPos (&mousePoint);

	CMenu m_popMenu;
	m_popMenu.LoadMenu(IDR_MENU1);
	CMenu*   pSubMenu=m_popMenu.GetSubMenu(0);
	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON,mousePoint.x,mousePoint.y,this);
	*pResult = 0;
}

#pragma endregion 

void CDIPIDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	if(TimerOn){
		KillTimer(1);
		TimerOn=0;
	}
	//关闭所有线程，并等待所有线程退出
	if(pDp!=NULL){
		for(int i=0;i<g_userNum;i++){
			if(!pDp[i].bScriptExit){
				pDp[i].SetScriptExit(TRUE);
			}
			if(!pDp[i].bExit){
				pDp[i].bAutoEscape=TRUE;
				pDp[i].bReLogin=FALSE;
				pDp[i].SetExit(TRUE);				
			}
		}		
	}
	if(g_userinfo)
		delete []g_userinfo;
	g_userinfo=NULL;
		
	Sleep(1000);
	if(pDp){
		delete []pDp;
		pDp=NULL;
	}
}

//起一个线程
void RunThread(LPVOID p)
{
	THREADPARA *para=(THREADPARA *)p;
	pDp[para->index].Run(&g_userinfo[para->index]);
}

//运行脚本
void RunScriptThread(LPVOID p)
{
	THREADPARA *para=(THREADPARA *)p;
	pDp[para->index].RunScript();
}

void CDIPIDlg::OnBnClickedStart()
{
	UpdateData(TRUE);
	m_charinfo.DeleteAllItems();
	m_message.ResetContent();
	//-----------------------------------------------	
	if(!SelectLine()){
		MessageBox("无法获取服务端ip地址！","提示信息",MB_OK|MB_ICONINFORMATION);
		return;
	}	
	gametime=0;
	//帐号登入
	for(int i=0;i<g_userNum;i++){
		CharLogin(i);
		Sleep(100);		
	}
	GetDlgItem(IDC_STOP)->EnableWindow(TRUE);
	GetDlgItem(IDC_START)->EnableWindow(FALSE);
}

void CDIPIDlg::OnTimer(UINT_PTR nIDEvent)
{
	CDIPIApp *pApp=(CDIPIApp *)AfxGetApp();
	UpdateData(TRUE);
	gametime++;
	CTime time1(gametime);
	CString szVal,szStr;
	
	szVal=time1.Format(_T("%d日 %H:%M:%S"));
	m_time.SetWindowText(szVal);
	int i;
	for(i=0;i<g_userNum;i++){
		//当前线程未退出并且在线,则发送连接信息
		if(!pDp[i].bExit && pDp[i].IsOnLine){
			if((GetTickCount()-pDp[i].nStartTime)/1000>=30){
				if(pDp[i].SendOnlineInfo("hoge")!=SUCCESSFUL){
					CTime cTime = CTime::GetCurrentTime();
					szVal.Format("%s %s%s",cTime.Format(_T("%H:%M:%S")),pDp[i].user.charname,"与服务端连接已断！");
					m_message.AddString(szVal);					
				}				
			}
		}
		//如果当前帐号不在线
		if(!pDp[i].IsOnLine){
			//启用了断线重登,并且不在线上，并且没在登录过程中
			if(pDp[i].bReLogin && !pDp[i].IsOnLine && !pDp[i].IsLogin){
				pDp[i].IsLogin=TRUE;
				CharLogin(i);
			}						
		}//END:如果当前帐号不在线
		//自动吃鱼鳃草
		if(pDp[i].bAutoEatSYC && pDp[i].charotherinfo.state==0 && (GetTickCount()-pDp[i].nEatSYCTime)>=3600000){			
			pDp[i].SendEatSYC();
			//pDp[i].nEatSYCTime=GetTickCount();
		}
		//自动吃智慧果
		if(pDp[i].bAutoExpNut && pDp[i].charotherinfo.state==0 && (GetTickCount()-pDp[i].nExpNutTime)>=3600000){
			pDp[i].SendEatExpNut();
			//pDp[i].nExpNutTime=GetTickCount();
		}		
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CDIPIDlg::CallbackMsg(NOTIFYPARA *pNotifyPara)
{
	CString szVal,szTemp,szStr,szName,szError;
	int i,n,nRow,pos;
	CTime cTime = CTime::GetCurrentTime();
	if(m_message.GetCount()>500)
		m_message.ResetContent();
	switch(pNotifyPara->nNotityType){
	case NOTIFY_TIME:
		if(!TimerOn)
			TimerOn=SetTimer(1,1000,NULL);
		break;
	case NOTIFY_MSG:		
		szVal.Format("%s %s",cTime.Format(_T("%H:%M:%S")),(LPCTSTR)pNotifyPara->lpNotifyData);
		m_message.AddString(szVal);
		m_message.SendMessage(WM_VSCROLL,SB_BOTTOM,   0);
		break;
	case NOTIFY_LOGIN:
		szVal=(LPCTSTR)pNotifyPara->lpNotifyData;
		pos=0;
		//取帐号
		szName=szVal.Tokenize("|",pos);		
		n=m_charinfo.GetItemCount();
		//取人物名
		szTemp=szVal.Tokenize("|",pos);		
		//指定帐号是否在列表框中
		for(i=0;i<n;i++){
			szStr=m_charinfo.GetItemText(i,0);
			if(szStr==szTemp)
				break;
		}
		if(i>=n){
			nRow = m_charinfo.InsertItem(n,szTemp);
			szTemp=szVal.Tokenize("|",pos);			
			m_charinfo.SetItemText(nRow, 1, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 2, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 3, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 4, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 5, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 6, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 7, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 8, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 9, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 10, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 11, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 12, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 13, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 14, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 15, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 16, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 17, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 19, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 20, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			if(!szTemp.Trim().IsEmpty())
				m_charinfo.SetItemText(nRow, 21, szTemp);			
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 22, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 23, szTemp);
			szVal.Format("%s %s帐号已登入！",cTime.Format(_T("%H:%M:%S")),szName);
			m_message.AddString(szVal);
			m_message.SendMessage(WM_VSCROLL,SB_BOTTOM,0);
		}
		else{
			nRow =i;
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 1, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 2, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 3, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 4, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 5, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 6, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 7, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 8, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 9, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 10, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 11, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 12, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 13, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 14, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 15, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 16, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 17, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 19, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 20, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			if(!szTemp.Trim().IsEmpty())
				m_charinfo.SetItemText(nRow, 21, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 22, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 23, szTemp);
			szVal.Format("%s %s帐号重新登入！",cTime.Format(_T("%H:%M:%S")),szName);
			m_message.AddString(szVal);
			m_message.SendMessage(WM_VSCROLL,SB_BOTTOM,   0);
		}	
		
		break;
	case NOTIFY_LOGOUT:
		szVal=(LPCTSTR)pNotifyPara->lpNotifyData;
		pos=0;
		n=m_charinfo.GetItemCount();
		szTemp=szVal.Tokenize("|",pos);
		for(i=0;i<n;i++){
			szStr=m_charinfo.GetItemText(i,0);
			if(szStr==szTemp)
				break;
		}
		if(i<n){
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(i, 23, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			szVal.Format("%s %s已退出！",cTime.Format(_T("%H:%M:%S")),szTemp);
			m_message.AddString(szVal);
			m_message.SendMessage(WM_VSCROLL,SB_BOTTOM,0);
		}		
		break;
	case NOTIFY_STARTSCRIPT:
		szTemp=(LPCTSTR)pNotifyPara->lpNotifyData;
		szVal.Format("%s %s脚本线程开始运行！",cTime.Format(_T("%H:%M:%S")),szTemp);
		m_message.AddString(szVal);
		m_message.SendMessage(WM_VSCROLL,SB_BOTTOM,0);
		break;
	case NOTIFY_STOPSCRIPT:
		szTemp=(LPCTSTR)pNotifyPara->lpNotifyData;			
		pos=0;
		szStr=szTemp.Tokenize("|",pos);
		szError=szTemp.Tokenize("|",pos);
		n=atoi(szError);
		if(n==0)
			szVal.Format("%s %s脚本线程已退出！",cTime.Format(_T("%H:%M:%S")),szStr);
		else
			szVal.Format("%s %s脚本线程在%d行有错误！",cTime.Format(_T("%H:%M:%S")),szStr,n+1);
		m_message.AddString(szVal);
		m_message.SendMessage(WM_VSCROLL,SB_BOTTOM,0);
		break;
	case NOTIFY_CHARDETAIL:
		szVal=(LPCTSTR)pNotifyPara->lpNotifyData;
		pos=0;
		n=m_charinfo.GetItemCount();
		szTemp=szVal.Tokenize("|",pos);
		for(i=0;i<n;i++){
			szStr=m_charinfo.GetItemText(i,0);
			if(szStr==szTemp)
				break;
		}
		if(i<n){
			nRow =i;
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 1, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 2, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 3, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 4, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 5, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 6, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 7, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 8, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 9, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 10, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 11, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 12, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 13, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 14, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 15, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 16, szTemp);
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 17, szTemp);
		}
		break;
	case NOTIFY_MAP:
		szVal=(LPCTSTR)pNotifyPara->lpNotifyData;
		pos=0;
		n=m_charinfo.GetItemCount();
		szTemp=szVal.Tokenize("|",pos);
		for(i=0;i<n;i++){
			szStr=m_charinfo.GetItemText(i,0);
			if(szStr==szTemp)
				break;
		}
		if(i<n){
			nRow =i;
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 20, szTemp);//地图
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 21, szTemp);//地图名称
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 22, szTemp);//坐标
		}
		break;
	case NOTIFY_XY:
		szVal=(LPCTSTR)pNotifyPara->lpNotifyData;
		pos=0;
		n=m_charinfo.GetItemCount();
		szTemp=szVal.Tokenize("|",pos);
		for(i=0;i<n;i++){
			szStr=m_charinfo.GetItemText(i,0);
			if(szStr==szTemp)
				break;
		}
		if(i<n){
			nRow =i;			
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 22, szTemp);//坐标
		}
		break;
	case NOTIFY_UPLEVELPOINT:
		szVal=(LPCTSTR)pNotifyPara->lpNotifyData;
		pos=0;
		n=m_charinfo.GetItemCount();
		szTemp=szVal.Tokenize("|",pos);
		for(i=0;i<n;i++){
			szStr=m_charinfo.GetItemText(i,0);
			if(szStr==szTemp)
				break;
		}
		if(i<n){
			nRow =i;
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 19, szTemp);
		}
		break;
	case NOTIFY_ROUND:
		szVal=(LPCTSTR)pNotifyPara->lpNotifyData;
		pos=0;
		n=m_charinfo.GetItemCount();
		szTemp=szVal.Tokenize("|",pos);
		for(i=0;i<n;i++){
			szStr=m_charinfo.GetItemText(i,0);
			if(szStr==szTemp)
				break;
		}
		if(i<n){
			nRow =i;
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 25, szTemp);
		}
		break;
	case NOTIFY_FAME:
		szVal=(LPCTSTR)pNotifyPara->lpNotifyData;
		pos=0;
		n=m_charinfo.GetItemCount();
		szTemp=szVal.Tokenize("|",pos);
		for(i=0;i<n;i++){
			szStr=m_charinfo.GetItemText(i,0);
			if(szStr==szTemp)
				break;
		}
		if(i<n){
			nRow =i;
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(nRow, 18, szTemp);
		}
		break;
	case NOTIFY_SCRIPT:
		szVal=(LPCTSTR)pNotifyPara->lpNotifyData;
		pos=0;
		n=m_charinfo.GetItemCount();
		szTemp=szVal.Tokenize("|",pos);
		for(i=0;i<n;i++){
			szStr=m_charinfo.GetItemText(i,0);
			if(szStr==szTemp)
				break;
		}
		if(i<n){
			nRow =i;
			szTemp=szVal.Tokenize("^",pos);
			m_charinfo.SetItemText(nRow, 24, szTemp);
		}
		break;
	case NOTIFY_GAMESTATE:
		szVal=(LPCTSTR)pNotifyPara->lpNotifyData;
		pos=0;
		n=m_charinfo.GetItemCount();
		szTemp=szVal.Tokenize("|",pos);
		for(i=0;i<n;i++){
			szStr=m_charinfo.GetItemText(i,0);
			if(szStr==szTemp)
				break;
		}
		if(i<n){
			szTemp=szVal.Tokenize("|",pos);
			m_charinfo.SetItemText(i, 23, szTemp);			
		}		
		break;
	}
}

//所有帐号全部登出
void CDIPIDlg::OnBnClickedStop()
{
	CString szVal;
	if(TimerOn){
		KillTimer(1);
		TimerOn=0;
		gametime=0;
	}
	if(pDp!=NULL){
		for(int i=0;i<g_userNum;i++){
			if(!pDp[i].bScriptExit){
				pDp[i].SetScriptExit(TRUE);
			}
			if(!pDp[i].bExit){
				pDp[i].bAutoEscape=TRUE;
				pDp[i].bReLogin=FALSE;
				pDp[i].SetExit(TRUE);				
			}
		}		
	}
	
	GetDlgItem(IDC_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_START)->EnableWindow(TRUE);
}

//根据选择线路获取该线路的ip地址
BOOL CDIPIDlg::SelectLine()
{
	HOSTENT *lpHostEnt;
	struct in_addr inAddr;
	LPSTR lpaddr;
	char ip[30];	
	memset(ip,0,sizeof(ip));
	switch(m_line){
	case 0:
		strncpy_s(ip,"64dx1.230572.com",strlen("64dx1.230572.com"));
		lpHostEnt=gethostbyname(ip);
		if(!lpHostEnt)return FALSE;
		lpaddr = lpHostEnt->h_addr_list[0];			
		memmove (&inAddr,lpaddr, 4);
		sprintf_s(ip,"%d.%d.%d.%d",inAddr.S_un.S_addr&0xff,(inAddr.S_un.S_addr>>8)&0xff,(inAddr.S_un.S_addr>>16)&0xff,(inAddr.S_un.S_addr>>24)&0xff);
		strcpy_s(g_serverinfo.ip,ip);
		g_serverinfo.port=7001;
		break;
	}
	return TRUE;
}

//让某帐号运行脚本
void CDIPIDlg::OnMenuRun()
{
	DWORD thread;
	HANDLE hThreads;

	if((int)m_list.GetFirstSelectedItemPosition()-1<0){
		MessageBox("请先选中要操作的帐号！","提示信息",MB_OK|MB_ICONINFORMATION);
		return;
	}
	int pos=(int)m_list.GetFirstSelectedItemPosition()-1;
	g_para[pos].index=pos;
	//向线程传递参数不能用局部变量	
	hThreads=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)RunScriptThread,(void *)&g_para[pos],0,&thread);
	if(!hThreads){
		MessageBox("启动脚本线程失败！","错误信息",MB_OK|MB_ICONEXCLAMATION);
		return;
	}	
}

//让某帐号运行脚本退出
void CDIPIDlg::OnMenuStop()
{
	if((int)m_list.GetFirstSelectedItemPosition()-1<0){
		MessageBox("请先选中要操作的帐号！","提示信息",MB_OK|MB_ICONINFORMATION);
		return;
	}
	int pos=(int)m_list.GetFirstSelectedItemPosition()-1;

	if(!pDp[pos].bScriptExit){
		pDp[pos].SetScriptExit(TRUE);						
	}	
}

void CDIPIDlg::OnMenuPause()
{
	int a=lssproto_a62toi("3yo");
	a=a+1;
}
//帐号登入
BOOL CDIPIDlg::CharLogin(int index)
{
	CString szVal;
	DWORD thread;
	HANDLE hThreads;
	
	g_para[index].index=index;
	//向线程传递参数不能用局部变量	
	hThreads=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)RunThread,(void *)&g_para[index],0,&thread);
	if(!hThreads){
		MessageBox("启动游戏线程失败！","错误信息",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;
	}	
	if(m_autorun && pDp[index].bScriptExit){
		hThreads=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)RunScriptThread,(void *)&g_para[index],0,&thread);
		if(!hThreads){
			MessageBox("启动脚本线程失败！","错误信息",MB_OK|MB_ICONEXCLAMATION);
			return FALSE;
		}		
	}
	return TRUE;
}

//选中帐号登入
void CDIPIDlg::OnSinglelogin()
{
	UpdateData(TRUE);
	
	if(!SelectLine()){
		MessageBox("无法获取服务端ip地址！","提示信息",MB_OK|MB_ICONINFORMATION);
		return;
	}
	int pos=(int)m_list.GetFirstSelectedItemPosition()-1;
	if(pos<0){
		MessageBox("请先选中要操作的帐号！","提示信息",MB_OK|MB_ICONINFORMATION);
		return;
	}
	if(!TimerOn){
		gametime=0;
	}
	pDp[pos].SetScriptExit(TRUE);
	Sleep(400);
	if(CharLogin(pos)){	
		GetDlgItem(IDC_STOP)->EnableWindow(TRUE);
		GetDlgItem(IDC_START)->EnableWindow(FALSE);
	}
}

//选中帐号登出
void CDIPIDlg::OnSinglelogout()
{
	int pos=(int)m_list.GetFirstSelectedItemPosition()-1;
	if(pos<0){
		MessageBox("请先选中要操作的帐号！","提示信息",MB_OK|MB_ICONINFORMATION);
		return;
	}	
	if(!pDp[pos].bExit){
		pDp[pos].bReLogin=FALSE;
		pDp[pos].bAutoEscape=TRUE;
		pDp[pos].bReLogin=FALSE;
		pDp[pos].SetExit(TRUE);						
	}
}

//是否显示说话内容
void CDIPIDlg::OnBnClickedChktalk()
{
	CButton * pbox=(CButton *)GetDlgItem(IDC_CHkTALK);
	int val=pbox->GetCheck();
	for(int i=0;i<g_userNum;i++){
		pDp[i].IsDispTalk=(BOOL)val;
	}
}

void CDIPIDlg::OnLvnColumnclickList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if (pNMLV->iSubItem==m_nSordCol)
		m_bASC=!m_bASC;
	else{
		m_bASC=TRUE;
		m_nSordCol=pNMLV->iSubItem;
	}
	int i=m_charinfo.GetItemCount();
    while(i--)m_charinfo.SetItemData(i,i);

	m_charinfo.SortItems((PFNLVCOMPARE)CompareFunc,(DWORD)&m_charinfo);
	*pResult = 0;
}

int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2,LPARAM lParamSort)
{ 
	CListCtrl *pList=(CListCtrl*)lParamSort;
	int nItem1,nItem2;
	LVFINDINFO FindInfo;
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=lParam1;
	nItem1=pList->FindItem(&FindInfo,-1);
	FindInfo.lParam=lParam2;
	nItem2=pList->FindItem(&FindInfo,-1);

	if((nItem1==-1)||(nItem2==-1))
	{
	   return 0;
	}
	CString Str1,Str2,szNum1,szNum2;
	Str1=pList->GetItemText(nItem1,m_nSordCol);
	Str2=pList->GetItemText(nItem2,m_nSordCol);
	szNum1=GetDigitFormString(Str1);
	szNum2=GetDigitFormString(Str2);
	int iCompRes,n1,n2;
	if(IsNumber(szNum1) && IsNumber(szNum2)){
		n1=atoi(szNum1);
		n2=atoi(szNum2);
		if(n1>n2)
			iCompRes=1;
		else if(n1==n2)
			iCompRes=0;
		else
			iCompRes=-1;
		if(!m_bASC)
		   return iCompRes;
		else
		   return iCompRes * -1;
	}
	else{
		iCompRes=Str1.Compare(Str2);
		if(!m_bASC)
		   return iCompRes;
		else
		   return iCompRes * -1;
	}

} 
//保存帐号配置信息
void CDIPIDlg::OnBnClickedSave()
{
	CString szAccount,szPwd,szSafeCode,szScript,szVal;
	CStdioFile f(_T("data.txt"),CFile::modeCreate|CFile::modeWrite|CFile::typeText);
	for(int i=0;i<m_list.GetItemCount();i++){
		szAccount=m_list.GetItemText(i,0);
		szPwd=m_list.GetItemText(i,1);
		szSafeCode=m_list.GetItemText(i,2);
		szScript=m_list.GetItemText(i,3);
		f.WriteString(szAccount+"|"+szPwd+"|"+szSafeCode+"|"+szScript+_T("\r\n"));
	}
	f.Close();
}

void CDIPIDlg::OnNMDblclkListchar(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int index;
	CString szAccount,szPwd,szSafeCode,szScript,szVal;
	index=(int)m_list.GetFirstSelectedItemPosition()-1;
	if(index==-1){
		AfxMessageBox(_T("请先选择要编辑的帐号！"));
		return;
	}
	szAccount=m_list.GetItemText(index,0);
	szPwd=m_list.GetItemText(index,1);
	szSafeCode=m_list.GetItemText(index,2);
	szScript=m_list.GetItemText(index,3);
	AccountEdit dlg;
	dlg.szAccount=szAccount;
	dlg.szPwd=szPwd;
	dlg.szSafeCode=szSafeCode;
	dlg.szScript=szScript;
	if ( dlg.DoModal() == IDOK ){
		m_list.SetItemText(index, 0, dlg.szAccount);
		m_list.SetItemText(index, 1, dlg.szPwd);
		m_list.SetItemText(index, 2, dlg.szSafeCode);
		m_list.SetItemText(index, 3, dlg.szScript);
		strcpy_s(g_userinfo[index].charname,dlg.szAccount);
		strcpy_s(g_userinfo[index].password,dlg.szPwd);
		strcpy_s(g_userinfo[index].safecode,dlg.szSafeCode);
		strcpy_s(g_userinfo[index].scriptName,dlg.szScript);
	}
	*pResult = 0;
}

//获取注册号
void CAboutDlg::OnBnClickedGetregcode()
{
	char buf[1024]={0};
	if(GetNicInfo(buf)){
		GetDlgItem(IDC_REGEDITCODE)->SetWindowText(buf);
	}
	else{
		AfxMessageBox(_T("获取注册码失败！"));
	}
}

//是否显示实时更新信息
void CDIPIDlg::OnBnClickedChkupdate()
{
	CButton * pbox=(CButton *)GetDlgItem(IDC_CHKUPDATE);
	int val=pbox->GetCheck();
	for(int i=0;i<g_userNum;i++){
		pDp[i].IsDispInfoOnTime=(BOOL)val;
	}
}


#pragma region 辅助函数

int lssproto_a62toi(char *a)
{
	int ret = 0;
	int minus;
	if (a[0] == '-'){
		minus = -1;
		a++;
	}
	else {
		minus = 1;
	}

	while (*a != '\0')
	{
		ret *= 62;
		if ('0' <= (*a) && (*a) <= '9')
			ret += (*a) - '0';
		else
			if ('a' <= (*a) && (*a) <= 'z')
				ret += (*a) - 'a' + 10;
			else
				if ('A' <= (*a) && (*a) <= 'Z')
					ret += (*a) - 'A' + 36;
				else
					return 0;
		a++;
	}
	return ret * minus;
}

#pragma endregion 