#pragma once
//服务端ip地址和端口号
typedef struct{
	char ip[30];
	int port;
}SERVERINFO;

//登录帐号信息
typedef struct{
	char charname[30];
	char password[30];
	char safecode[30];
	int index;			//第几个帐号
	char scriptName[100];//脚本文件名
}USERINFO;

//用于向线程传递整型参数
typedef struct{
	int index;
}THREADPARA;

//回调消息类型
enum{
	NOTIFY_TIME,
	NOTIFY_MSG,
	NOTIFY_LOGIN,
	NOTIFY_LOGOUT,
	NOTIFY_CHARDETAIL,
	NOTIFY_MAP,
	NOTIFY_XY,
	NOTIFY_UPLEVELPOINT,
	NOTIFY_SCRIPT,
	NOTIFY_STARTSCRIPT,
	NOTIFY_STOPSCRIPT,
	NOTIFY_GAMESTATE,
	NOTIFY_ROUND,
	NOTIFY_FAME
};

//回调消息结构
typedef struct{
	UINT nNotityType;		//信息类型，见上面的枚举定义
	LPVOID lpNotifyData;	//信息内容
}NOTIFYPARA;

typedef enum{
	PHINPUT,
	PHSELECT
}CALCTYPE;

typedef struct{
	int N1;		//第一个操作数
	int N2;		//第二个操作数
	char oper;	//运算符
	int result; //运算结果
	CALCTYPE type;//输入类型
	int selindex;		//选择结果时选项索引
}PHCALC;

typedef void (*CALLBACK_Func) (WPARAM wParam,NOTIFYPARA *msg);
int GetScriptLinsNum(char *path);
CString Encrypt(CString s);
CString Decrypt(CString s);
BOOL DirExist(LPCTSTR lpszDirName);
BOOL FileExist(LPCTSTR lpszFileName);
void DbgLog( LPCTSTR lpszFormat, ... );
int WriteDataToFile(LPCSTR filename,char * data,long size,LPCSTR mode, int nStartPos=-1 );
CString GetCurTimeString ();
int ConnectServer(SOCKET &rsocket,char *ip,int port);
void Tokenize(char *source,char *dest,char *token,int &nstart);
int mystrstr(char *src,char *substr,int len);
unsigned int HexStrToDec(char *data);
double PerStrToDbl(CString &data);
BOOL IsDigit(char c);
BOOL IsNumber(CString text);
int Myatoi(CString text);
//查找字符串
int  FindingString(const char* lpszSour, const char* lpszFind, int nStart = 0);
//带通配符的字符串匹配
bool MatchingString(const char* lpszSour, const char* lpszMatch, bool bMatchCase = true);
//多重匹配
bool MultiMatching(const char* lpszSour, const char* lpszMatch, int nMatchLogic = 0, bool bRetReversed = 0, bool bMatchCase = true);
int myatoi(CString src);
CString GetDigitFormString(CString str);
