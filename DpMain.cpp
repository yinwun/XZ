#include "StdAfx.h"
#include "DpMain.h"
#include "Autil.h"

#pragma region 变量

extern USERINFO *g_userinfo;
extern int g_userNum;
extern SERVERINFO g_serverinfo;
extern CDpMain *pDp;
//游戏时间
extern long gametime;
//消息回调函数
extern void Callback_Notify(NOTIFYPARA *pNotifyPara);

CCriticalSection csSingal;	//信号量

#pragma endregion

#pragma region 构造函数

CDpMain::CDpMain(void)
{
	// 初始化变量
	IP=0;
	IsOnLine=FALSE;
	script=NULL;
	//初始化为未运行状态
	bExit=TRUE;
	bScriptExit=TRUE;
	bScriptErrorReStart=FALSE;
	bIsReLogin=FALSE;
	IsLogin=FALSE;
	IsDispTalk=FALSE;
	IsDispInfoOnTime=TRUE;
	IsBaiTanOk=FALSE;
	bIsBaiTaning=FALSE;
	maxload=0;
	MapSeqNo=1;

	//参数设置
	nScriptDelay=50;				//脚本延时（0-250）
	bReLogin=FALSE;					//是否断线重登
	nAutoUnlock=FALSE;				//自动解锁
	IsSafeCodeUnlock=FALSE;			//是否已解锁
	bAutoCapture=FALSE;				//自动捉宠
	bAutoEscape=FALSE;				//自动逃跑
	bFightpetDeadEscape=FALSE;
	bAutoKNPC=FALSE;				//自动KNPC
	bFullEscape=FALSE;				//落马逃跑
	bMakeTeam=FALSE;				//组队
	bDuel=FALSE;					//决斗
	bCallingCard=FALSE;				//名片
	bTrade=FALSE;					//交易
	for(int i=0;i<10;i++){			
		ZeroMemory(cZDAttack[i],sizeof(cZDAttack[i]));		//指定攻击的宠物
		ZeroMemory(cLockAttack[i],sizeof(cLockAttack[i]));	//锁定攻击的宠物
		ZeroMemory(cZDEscape[i],sizeof(cZDEscape[i]));		//指定逃跑的宠物
	}
	nZDAttackNum=0;
	nLockAttackNum=0;
	nZDEscapeNum=0;
	for(int i=0;i<4;i++)
		nAutoUpPoint[i]=0;
	bAutoEatSYC=FALSE;				//自动吃鱼鳃草
	bAutoExpNut=FALSE;				//吃智慧果
	nEatSYCTime=0;
	nExpNutTime=0;
	bAutoPile=FALSE;				//自动堆叠
	bEatMeat=FALSE;					//吃补血肉
	bDiscardMeat=FALSE;				//丢没有补血功能的血肉
	bRecruitMpPlaceTime=FALSE;		//平时补气
	nLockRidePet=0;					//锁定骑宠
	nLockFightPet=0;				//锁定战宠
	for(int i=0;i<15;i++){			
		ZeroMemory(cDiscardLikeItem[i],sizeof(cDiscardLikeItem[i]));		//丢弃包含
		ZeroMemory(cDiscardExceptItem[i],sizeof(cDiscardExceptItem[i]));	//丢弃不包含
	}
	nDiscardLikeItemNum=0;
	nDiscardExceptItemNum=0;
	dFirstDelay=0;				//首次延迟
	dAttackDelay=0;				//攻击延迟
	dRMpByBlood=0;				//嗜血补气
	ZeroMemory(cCharFirstAction,sizeof(cCharFirstAction));	//人物首次动作
	ZeroMemory(cCharAction,sizeof(cCharAction));			//人物一般动作
	ZeroMemory(cPetFirstAction,sizeof(cPetFirstAction));	//战宠首次动作
	ZeroMemory(cPetAction,sizeof(cPetAction));				//战宠一般动作
	nCapLevel=0;											//捕捉等级
	for(int i=0;i<5;i++)
		ZeroMemory(cCapPetName[i],sizeof(cCapPetName[i]));	//捉宠名称
	nCapPetNum=0;
	nCapPetBlood=0;											//捉宠血量
	nCapCharUseSkill=0;										//捉宠时人物的技能和捉宠血量
	nCapPetSkill=1;											//捉宠时宠物使用的技能,缺省防御
	bCapEscapeWhenNoPet=TRUE;								//是否逃跑当没有要捕获的宠物时
	recruitblood.id=-1;										//人物精灵补血
	ZeroMemory(recruitblood.name,sizeof(recruitblood.name));
	petrecruitblood.skillid=-1;								//宠物技能补血
	bDeleteChar=FALSE;
}

CDpMain::~CDpMain(void)
{
}

#pragma endregion

#pragma region 开始运行

void CDpMain::Init()
{
	int i,j;	
	//初始化人物列表
	for(i=0;i<2;i++){
		charlist[i].dataplace=0;
		charlist[i].faceimage=0;
		ZeroMemory(charlist[i].name,sizeof(charlist[i].name));
		ZeroMemory(charlist[i].place,sizeof(charlist[i].place));
	}

	//初始化装备上的精灵列表
	for(i=0;i<6;i++){
		magic[i].id=0;
		magic[i].kubun=0;
		ZeroMemory(magic[i].name,sizeof(magic[i].name));
		ZeroMemory(magic[i].comment,sizeof(magic[i].comment));
	}

	//初始化宠物详细信息
	for(i=0;i<5;i++){
		petdetail[i].no=-1;
		for(j=0;j<7;j++){
			ZeroMemory(petdetail[i].oldname,sizeof(petdetail[i].oldname));
			ZeroMemory(petdetail[i].newname,sizeof(petdetail[i].newname));
			petdetail[i].petskill[j].skillid=-1;
			petdetail[i].state=0;
		}
	}

	//初始化物品信息
	for(i=0;i<24;i++){
		ZeroMemory(iteminfo[i].name,sizeof(iteminfo[i].name));
		ZeroMemory(iteminfo[i].comment,sizeof(iteminfo[i].comment));
	}

	//初始化人物技能信息
	for(i=0;i<CHAR_SKILL_NUM;i++){
		charskill[i].useflag=-1;
		ZeroMemory(charskill[i].name,sizeof(charskill[i].name));
	}

	//初始化邮件联系人列表
	for(i=0;i<EMAIL_NUM;i++){
		ZeroMemory(mailcontactlist[i].charname,sizeof(mailcontactlist[i].charname));
	}

	//初始gameflag
	charotherinfo.fightpet=-1;
	charotherinfo.menuflg=MENUFLAG;
	charotherinfo.state=-1;//登出状态
	ZeroMemory(chardetail.name,sizeof(chardetail.name));
	chardetail.kubun=-1;
	chardetail.uplevelpoint=0;
	chardetail.fame=0;
	ZeroMemory(mapinfo.floorname,sizeof(mapinfo.floorname));

}

void CDpMain::Run(USERINFO *puser)
{
	ASSERT(this);
	char buffer[65500],temp[65500];
	int recvbytes;
	CString szVal,szT,szStr;

	IsLogin=TRUE;
	bExit=FALSE;
	Init();

#pragma region 登陆

	//保存当前线程所处理的帐号信息
	strcpy_s(user.charname,puser->charname);
	strcpy_s(user.password,puser->password);
	strcpy_s(user.safecode,puser->safecode);
	strcpy_s(user.scriptName,puser->scriptName);
	user.index=puser->index;
	strcpy_s(autil.charname,user.charname);

	CString szMsg;
	NOTIFYPARA para;
	para.nNotityType=NOTIFY_MSG;

	//连接服务端
	if(!ConnectServer(socket,g_serverinfo.ip,g_serverinfo.port)){		
		szMsg.Format("%s%s",user.charname,"与服务端建立连接失败！");
		para.lpNotifyData=(LPVOID)(LPCTSTR)szMsg;
		csSingal.Lock();		
		Callback_Notify(&para);
		csSingal.Unlock();
		goto Lab_Exit;
	}
	int ret;
	//是否删除帐号
	if(bDeleteChar){
		//检测用户名和密码
		for(int i=0;i<3;i++){
			ret=CheckUser(user.charname,user.safecode);
			if(ret!=SUCCESSFUL && ret!=NOACCOUNT){
				szMsg.Format("%s%s",user.charname,"帐号登入失败！");
				para.lpNotifyData=(LPVOID)(LPCTSTR)szMsg;
				csSingal.Lock();		
				Callback_Notify(&para);
				csSingal.Unlock();
			}
			else
				break;
			Sleep(1000);
		}
		if(ret!=SUCCESSFUL && ret!=NOACCOUNT){
			goto Lab_Exit;
		}
		if(ret==SUCCESSFUL && DeleteChar(0)!=SUCCESSFUL){
			szMsg.Format("%s%s",user.charname,"删除帐号失败！");
			para.lpNotifyData=(LPVOID)(LPCTSTR)szMsg;
			csSingal.Lock();		
			Callback_Notify(&para);
			csSingal.Unlock();
			goto Lab_Exit;
		}
		else{
			szMsg.Format("%s%s",user.charname,"删除帐号成功！");
			para.lpNotifyData=(LPVOID)(LPCTSTR)szMsg;
			csSingal.Lock();		
			Callback_Notify(&para);
			csSingal.Unlock();
		}
		bDeleteChar=FALSE;
		goto CheckUser;
	}
	//发送登出信息
	if(SendLogFlag(0)!=SUCCESSFUL){
		szMsg.Format("%s%s",user.charname,"向服务端发送登出信息失败！");
		para.lpNotifyData=(LPVOID)(LPCTSTR)szMsg;
		csSingal.Lock();		
		Callback_Notify(&para);
		csSingal.Unlock();
		goto Lab_Exit;
	}
CheckUser:
	nStartTime=GetTickCount();
	nRecvTime=GetTickCount();
	//检测用户名和密码
	ret=CheckUser(user.charname,user.password);

	switch(ret){
	case SENDMSG_ERROR:
		szMsg.Format("%s%s",user.charname,"检测用户名和密码向服务端发送信息失败！");
		para.lpNotifyData=(LPVOID)(LPCTSTR)szMsg;
		csSingal.Lock();		
		Callback_Notify(&para);
		csSingal.Unlock();
		goto Lab_Exit;
	case RECVMSG_ERROR:
		szMsg.Format("%s%s",user.charname,"检测用户名和密码接收信息失败！");
		para.lpNotifyData=(LPVOID)(LPCTSTR)szMsg;
		csSingal.Lock();		
		Callback_Notify(&para);
		csSingal.Unlock();
		goto Lab_Exit;
	case CDKEY_AND_PWD_ERROR:
		szMsg.Format("%s%s",user.charname,"用户名或密码错误！");
		para.lpNotifyData=(LPVOID)(LPCTSTR)szMsg;
		csSingal.Lock();		
		Callback_Notify(&para);
		csSingal.Unlock();
		goto Lab_Exit;
	case INVALID_INFO:
		szMsg.Format("%s%s",user.charname,"检测用户名和密码接收到无效信息！");
		para.lpNotifyData=(LPVOID)(LPCTSTR)szMsg;
		csSingal.Lock();		
		Callback_Notify(&para);
		csSingal.Unlock();
		goto Lab_Exit;
	case GET_ACCOUNT_INFO_ERROR:
		szMsg.Format("%s%s",user.charname,"检测用户名和密码获取帐号信息失败！");
		para.lpNotifyData=(LPVOID)(LPCTSTR)szMsg;
		csSingal.Lock();		
		Callback_Notify(&para);
		csSingal.Unlock();
		goto Lab_Exit;
	case NOT_LOGGED_IN:
		szMsg.Format("%s%s",user.charname,"登入失败！");
		para.lpNotifyData=(LPVOID)(LPCTSTR)szMsg;
		csSingal.Lock();		
		Callback_Notify(&para);
		csSingal.Unlock();
		goto Lab_Exit;
	case NOACCOUNT:
		if(CreateNewChar(0,user.charname,100035,30175,10,10,0,0,0,0,0,10,1)!=SUCCESSFUL){
			szMsg.Format("%s创建帐号失败！",user.charname);
			para.lpNotifyData=(LPVOID)(LPCTSTR)szMsg;
			csSingal.Lock();		
			Callback_Notify(&para);
			csSingal.Unlock();
			goto Lab_Exit;
		}
		else{
			szMsg.Format("%s创建帐号成功！",user.charname);
			para.lpNotifyData=(LPVOID)(LPCTSTR)szMsg;
			csSingal.Lock();		
			Callback_Notify(&para);
			csSingal.Unlock();
		}
	}	//登入左边人物
	if(CharLogin(0)!=SUCCESSFUL){
		szMsg.Format("%s%s",user.charname,"帐号登入失败！");
		para.lpNotifyData=(LPVOID)(LPCTSTR)szMsg;
		csSingal.Lock();		
		Callback_Notify(&para);
		csSingal.Unlock();
		goto Lab_Exit;
	}	
	//登入成功后解析服务端发回的信息
	ZeroMemory(buffer,sizeof(buffer));
	if(!ReceiveData(buffer,recvbytes)){
		szMsg.Format("%s%s",user.charname,"登入接收数据失败！");
		para.lpNotifyData=(LPVOID)(LPCTSTR)szMsg;
		csSingal.Lock();		
		Callback_Notify(&para);
		csSingal.Unlock();
		goto Lab_Exit;
	}
	if(ParseLoginData(buffer,recvbytes)!=SUCCESSFUL){
		szMsg.Format("%s%s",user.charname,"解析登入后服务端返回数据失败！");
		para.lpNotifyData=(LPVOID)(LPCTSTR)szMsg;
		csSingal.Lock();		
		Callback_Notify(&para);
		csSingal.Unlock();
		goto Lab_Exit;
	}	
	SendPetState();
	SendMenuFlag(MENUFLAG);
	if(strlen(chardetail.name)>0 && chardetail.level>0){		
		//显示登入信息	
		if(chardetail.fEarth>0){
			szT.Format("地%d",chardetail.fEarth/10);
			szStr+=szT;
		}
		if(chardetail.fWater>0){
			szT.Format("水%d",chardetail.fWater/10);
			szStr+=szT;
		}
		if(chardetail.fFire>0){
			szT.Format("火%d",chardetail.fFire/10);		
			szStr+=szT;
		}
		if(chardetail.fWind>0){
			szT.Format("风%d",chardetail.fWind/10);
			szStr+=szT;
		}
		if(strlen(mapinfo.floorname)>0)
			szMsg.Format("%s|%s|%d|%d|%d|%d/%d|%d/%d|%d|%d|%d|%d|%d|%s|%d|%d|%d|%d|%d|%d|%d|%d|%s|%d,%d|%s",user.charname,chardetail.name,chardetail.level,chardetail.exp,chardetail.nextexp,
				chardetail.hp,chardetail.maxhp,chardetail.mp,chardetail.maxmp,chardetail.attack,chardetail.defence,chardetail.fQuick,
				chardetail.fCharm,chardetail.transmigration,szStr,chardetail.vital,chardetail.str,chardetail.tough,chardetail.dex,chardetail.dp,chardetail.gold,chardetail.uplevelpoint,
				changemap.floor,mapinfo.floorname,changemap.x,changemap.y,"平时");
		else
			szMsg.Format("%s|%s|%d|%d|%d|%d/%d|%d/%d|%d|%d|%d|%d|%d|%s|%d|%d|%d|%d|%d|%d|%d|%d|%s|%d,%d|%s",user.charname,chardetail.name,chardetail.level,chardetail.exp,chardetail.nextexp,
				chardetail.hp,chardetail.maxhp,chardetail.mp,chardetail.maxmp,chardetail.attack,chardetail.defence,chardetail.fQuick,
				chardetail.fCharm,chardetail.transmigration,szStr,chardetail.vital,chardetail.str,chardetail.tough,chardetail.dex,chardetail.dp,chardetail.gold,chardetail.uplevelpoint,
				changemap.floor," ",changemap.x,changemap.y,"平时");
		para.nNotityType=NOTIFY_LOGIN;
		para.lpNotifyData=(LPVOID)(LPCTSTR)szMsg;
		csSingal.Lock();	
		Callback_Notify(&para);
		csSingal.Unlock();
	}
	else{
		DbgLog("%s|%s|%d|%d|%d|%d/%d|%d/%d|%d|%d|%d|%d|%d|%s|%d|%d|%d|%d|%d|%d|%d|%d|%s|%d,%d|%s\r\n",user.charname,chardetail.name,chardetail.level,chardetail.exp,chardetail.nextexp,
			chardetail.hp,chardetail.maxhp,chardetail.mp,chardetail.maxmp,chardetail.attack,chardetail.defence,chardetail.fQuick,
			chardetail.fCharm,chardetail.transmigration,szStr,chardetail.vital,chardetail.str,chardetail.tough,chardetail.dex,chardetail.dp,chardetail.gold,chardetail.uplevelpoint,
			changemap.floor," ",changemap.x,changemap.y,"平时");
		goto Lab_Exit;
	}

#pragma endregion
		
#pragma region 登陆成功

	//登入后置其为真
	IsOnLine=TRUE;		//在线
	charotherinfo.state=0;//平时	
	IsLogin=FALSE;		//登入完成
	bExit=FALSE;		//置不退出状态
	bIsReLogin=TRUE;	//重登完成后置位，表示发生了重登，脚本检测后把它置回false
	ZeroMemory(buffer,sizeof(buffer));
	int datalen=0;	
	//用于表示数据是否接收完成
	BOOL bReceived=FALSE;
	//循环接受数据
	while(!bExit){
		ZeroMemory(temp,sizeof(temp));
		recvbytes=recv(socket,temp,sizeof(temp),0);
		//对方连接关闭,recv返回0代表对方已关闭连接，返回负值代表发生了错误，返回一个正数代表接收到的字节数
		if(recvbytes<=0 || recvbytes!=strlen(temp) || recvbytes>=65500){
			DbgLog("%s:RecvBytes=%d,Error=%d\r\n",user.charname,recvbytes,WSAGetLastError());
			break;
		}
		
		if(recvbytes>0 && temp[recvbytes-1]!='\n'){
			if(datalen>0)
				strncat(buffer,temp,recvbytes);
			else
				strncpy_s(buffer,temp,recvbytes);
			datalen+=recvbytes;			
		}
		else if(recvbytes>0 && temp[recvbytes-1]=='\n'){//把最后一条数据存入缓冲区
			strncat(buffer,temp,recvbytes);	
			datalen+=recvbytes;
			bReceived=TRUE;//已接收到最后一块数据
		}
		else if(recvbytes<0){//接收失败,可能线路已断
			break;
		}
		if(bReceived){
			//解析接收到的数据			
			if(ParseData(buffer,datalen)!=SUCCESSFUL){
				para.nNotityType=NOTIFY_MSG;
				szMsg.Format("%s%s",user.charname,"解析数据出错！");
				para.lpNotifyData=(LPVOID)(LPCTSTR)szMsg;	
				csSingal.Lock();		
				Callback_Notify(&para);
				csSingal.Unlock();
			}
			datalen=0;
			ZeroMemory(buffer,sizeof(buffer));
			bReceived=FALSE;
		}		
	}
Lab_Exit:
	ReleaseTalkDeque();
	closesocket(socket);
	charotherinfo.state=-1;	//已登出
	IsLogin=FALSE;			//未进行登入
	IsOnLine=FALSE;			//已断线
	para.nNotityType=NOTIFY_LOGOUT;
	szMsg.Format("%s|%s|%s",chardetail.name,"登出",user.charname);	
	para.lpNotifyData=(LPVOID)(LPCTSTR)szMsg;
	csSingal.Lock();		
	Callback_Notify(&para);
	csSingal.Unlock();

#pragma endregion

}

void CDpMain::RunScript()
{
	char c;
	CString opercode,opernum,szMsg,szVal;
	int pos,i,ip;
	NOTIFYPARA para;
	CStdioFile f;
ScriptStart:
	bScriptExit=FALSE;
	ErrorLine=0;
	//检测游戏帐号是否已登入
	DWORD t=GetTickCount();
	while(!bScriptExit){
		if(IsOnLine)
			break;	
		Sleep(200);
	}
	if(bScriptExit){
		goto Lab_ScriptExit;
	}
	para.nNotityType=NOTIFY_STARTSCRIPT;
	szMsg.Format("%s",user.charname);	
	para.lpNotifyData=(LPVOID)(LPCTSTR)szMsg;
	csSingal.Lock();		
	Callback_Notify(&para);
	csSingal.Unlock();

	//运行脚本，重登取消置位
	bIsReLogin=FALSE;
	//开始处理脚本	
	scriptNum=GetScriptLinsNum(user.scriptName);
	if(scriptNum<0){
		szVal.Format("%s 脚本文件不存在！",user.scriptName);
		AfxMessageBox(szVal);
		goto Lab_ScriptExit;
	}
	if(script){
		delete []script;
		script=NULL;
	}
	script=new CString[scriptNum];
	ASSERT(script);
	i=0;
	f.Open(user.scriptName,CFile::modeRead|CFile::typeText|CFile::shareDenyNone);
	while(f.ReadString(szVal)){
		script[i]=szVal.Trim();
		i++;
	}
	f.Close();

	IP=0;
	ErrorLine=0;
	
	while(IP<scriptNum && !bScriptExit){
		//输出当前脚本运行信息
		if(IsDispInfoOnTime){
			para.nNotityType=NOTIFY_SCRIPT;
			szMsg.Format("%s|%d:%s",chardetail.name,IP+1,script[IP]);	
			para.lpNotifyData=(LPVOID)(LPCTSTR)szMsg;
			csSingal.Lock();		
			Callback_Notify(&para);
			csSingal.Unlock();
		}
		c=script[IP].GetAt(0);
		if(script[IP].GetLength()>0 && c!='\''){//跳过空行和注释行
			//取操作码和操作数
			opercode=GetOperateCode(script[IP]);
			opernum=GetOperateNum(script[IP]);
			if(opercode.CompareNoCase("walkpos")==0){//走路
				if(charotherinfo.state==0){//平时状态走路
					if(!WalkPos(opernum)){
						ErrorLine=IP;
						break;
					}
					//显示当前坐标
					para.nNotityType=NOTIFY_XY;
					szMsg.Format("%s|%d,%d",chardetail.name,charotherinfo.x,charotherinfo.y);	
					para.lpNotifyData=(LPVOID)(LPCTSTR)szMsg;
					csSingal.Lock();		
					Callback_Notify(&para);
					csSingal.Unlock();
				}
				else{//其他状态脚本停止运行
					Sleep(no );
					continue;
				}
			}
			else if(opercode.CompareNoCase("w")==0){//人物转向
				if(opernum.Find(",")>=0){
					ErrorLine=IP;
					break;
				}
				if(charotherinfo.state==0){//平时状态
					if(!SendWalkDirection(charotherinfo.x,charotherinfo.y,(LPSTR)(LPCTSTR)opernum)){
						ErrorLine=IP;
						break;
					}
				}
				else{//其他状态脚本停止运行
					Sleep(100);
					continue;
				}
			}
			else if(opercode.CompareNoCase("log")==0){//原地登出或返回记录点
				if(opernum.Compare("0")==0){
					//SetScriptExit(TRUE);
					SetExit(TRUE);
				}
				else if(opernum.Compare("1")==0)//原登
					SendLogFlag(1);
			}
			else if(opercode.CompareNoCase("eo")==0){//更新游戏数据
				SendEOCommand();
			}
			else if(opercode.CompareNoCase("set")==0){//参数设置
				ParseSetPara(opernum);
			}
			else if(opercode.CompareNoCase("saveitem")==0){//存放道具 saveitem 寄存|仓库，1|2|3
				int pos,towhere;
				CString szToWhere,szItem;
				pos=0;
				szToWhere=opernum.Tokenize(",",pos);
				szItem=opernum.Tokenize(",",pos);
				if(szToWhere.IsEmpty() || szItem.IsEmpty()){
					ErrorLine=IP;
					break;
				}
				if(szToWhere.Find("寄存")>=0)
					towhere=0;
				else
					towhere=1;
				SaveItem(towhere,(LPSTR)(LPCTSTR)szItem);
			}
			else if(opercode.CompareNoCase("getitem")==0){//取道具，getitem 寄存|仓库，1|2|3
				int pos,towhere;
				CString szToWhere,szItem;
				pos=0;
				szToWhere=opernum.Tokenize(",",pos);
				szItem=opernum.Tokenize(",",pos);
				if(szToWhere.IsEmpty() || szItem.IsEmpty()){
					ErrorLine=IP;
					break;
				}
				if(szToWhere.Find("寄存")>=0)
					towhere=0;
				else
					towhere=1;
				GetItem(towhere,(LPSTR)(LPCTSTR)szItem);				
			}
			else if(opercode.CompareNoCase("savepet")==0){//存放宠物 savepet 寄存|仓库，1|2|3
				int pos,towhere;
				CString szToWhere,szItem;
				pos=0;
				szToWhere=opernum.Tokenize(",",pos);
				szItem=opernum.Tokenize(",",pos);
				if(szToWhere.IsEmpty() || szItem.IsEmpty()){
					ErrorLine=IP;
					break;
				}
				if(szToWhere.Find("寄存")>=0)
					towhere=0;
				else
					towhere=1;
				SavePet(towhere,(LPSTR)(LPCTSTR)szItem);
			}
			else if(opercode.CompareNoCase("getpet")==0){//取宠物，getpet 寄存|仓库，1|2|3
				int pos,towhere;
				CString szToWhere,szItem;
				pos=0;
				szToWhere=opernum.Tokenize(",",pos);
				szItem=opernum.Tokenize(",",pos);
				if(szToWhere.IsEmpty() || szItem.IsEmpty()){
					ErrorLine=IP;
					break;
				}
				if(szToWhere.Find("寄存")>=0)
					towhere=0;
				else
					towhere=1;
				GetPet(towhere,(LPSTR)(LPCTSTR)szItem);				
			}
			else if(opercode.CompareNoCase("sellpet")==0){//卖宠物
				SellPet((LPSTR)(LPCTSTR)opernum);
			}
			else if(opercode.CompareNoCase("useitem")==0){//使用道具
				UseItem((LPSTR)(LPCTSTR)opernum);
			}
			else if(opercode.CompareNoCase("moveitem")==0){//移动道具
				MoveItem((LPSTR)(LPCTSTR)opernum);
			}
			else if(opercode.CompareNoCase("delay")==0){//延时
				Sleep(atoi(opernum));
			}
			else if(opercode.CompareNoCase("cls")==0){//清屏
				csLocalSingal.Lock();
				talkmessage.clear();
				csLocalSingal.Unlock();
			}
			else if(opercode.CompareNoCase("say")==0){//说话	say 说话内容[,颜色值]
				int color=CHAR_COLORPURPLE,n;
				char msg[4092]={0};
				n=opernum.ReverseFind(',');
				if(n>0 && (opernum.GetLength()-n)<3){
					strcpy_s(msg,opernum.Left(n));
					color=atoi(opernum.Right(opernum.GetLength()-n-1));
					if(color<0 || color>9)
						color=CHAR_COLORPURPLE;
				}
				else
					strcpy_s(msg,opernum);
				SendTalk(charotherinfo.x,charotherinfo.y,msg,color,3);
				ZeroMemory(windowinfo.data,sizeof(windowinfo.data));
			}			
			else if(opercode.CompareNoCase("call")==0){//调用过程
				IPStack.push(IP);				
				i=GetLabelNum(opernum);
				if(i==-1){
					ErrorLine=IP;
					break;
				}
				else{
					IP=i;
				}
			}
			else if(opercode.CompareNoCase("return")==0){//过程调用结束
				if(IPStack.size()>0){
					IP=IPStack.top();
					IPStack.pop();
				}
			}
			else if(opercode.CompareNoCase("end")==0){//脚本结束
				ErrorLine=0;
				goto Lab_ScriptExit;
			}
			else if(opercode.CompareNoCase("goto")==0){//跳转指令
				if(IsNumber(opernum)){
					IP+=Myatoi(opernum);
					continue;
				}
				else{
					i=GetLabelNum(opernum);
					if(i==-1){
						ErrorLine=IP;
						break;
					}
					else{
						IP=i;
					}
				}
			}
			else if(opercode.CompareNoCase("dim")==0){//定义变量
				pos=0;
				szVal=opernum.Tokenize(",",pos);
				while(szVal.GetLength()>0){
					if(szVal.GetAt(0)=='@'){
						intvar.SetAt(szVal,0);
						szVal=opernum.Tokenize(",",pos);
					}
					else{
						ErrorLine=IP;
						break;
					}
				}
			}
			else if(opercode.CompareNoCase("let")==0){//变量赋值
				if(!Let_Set(opernum)){
					ErrorLine=IP;
					break;
				}
			}
			else if(opercode.CompareNoCase("if")==0){//变量判断				
				ip=IP;
				if(!If_Check(opernum)){
					ErrorLine=IP;
					break;
				}
				if(ip!=IP){//跳转
					continue;
				}
			}
			else if(opercode.CompareNoCase("pickup")==0){//拾获物品或宠物
				Pickup(opernum);
			}
			else if(opercode.CompareNoCase("doffstone")==0){//丢弃金钱
				DoffStone(opernum);
			}
			else if(opercode.CompareNoCase("load")==0){//取钱
				LoadStone(opernum);
			}
			else if(opercode.CompareNoCase("save")==0){//存钱
				SaveStone(opernum);
			}
			else if(opercode.CompareNoCase("loadz")==0){//取钱从家族
				LoadStoneFromFamily(opernum);
			}
			else if(opercode.CompareNoCase("savez")==0){//存钱到家族
				SaveStoneToFamily(opernum);
			}
			else if(opercode.CompareNoCase("doffitem")==0){//丢弃物品
				DoffItem(opernum);
			}
			else if(opercode.CompareNoCase("doffpet")==0){//丢弃宠物
				DoffPet(opernum);
			}
			else if(opercode.CompareNoCase("chmap")==0){//切换地图
				int x,y;
				pos=0;
				szVal=opernum.Tokenize(",",pos);
				x=atoi(szVal);
				szVal=opernum.Tokenize(",",pos);
				y=atoi(szVal);
				if(abs(charotherinfo.x-x)>1 || abs(charotherinfo.y-y)>1){
					if(!WalkPos(opernum)){
						ErrorLine=IP;
						break;
					}
				}
				Sleep(200);
				//在地图切换完成后，mapinfo.seqnot和MapSeqNo相等
				if(SendChangeMap(MapSeqNo,x,y)!=SUCCESSFUL){
					ErrorLine=IP;
					break;
				}
			}
			else if(opercode.CompareNoCase("waitmap")==0){//等待地图
				ip=IP;
				if(!WaitMap(opernum)){//参数出错停止脚本
					ErrorLine=IP;
					break;
				}
				if(ip!=IP){//等待地图出错跳转
					continue;
				}
			}
			else if(opercode.CompareNoCase("waitsay")==0){//等待说话
				ip=IP;
				if(!WaitSay(opernum)){//参数出错停止脚本
					ErrorLine=IP;
					break;
				}
				if(ip!=IP){//出错跳转
					continue;
				}
			}
			else if(opercode.CompareNoCase("waitdlg")==0){//等待对话框
				ip=IP;
				if(!WaitDlg(opernum)){//参数出错停止脚本
					ErrorLine=IP;
					break;
				}
				if(ip!=IP){//出错跳转
					continue;
				}
			}
			else if(opercode.CompareNoCase("button")==0){//点击按钮
				Button(opernum);
			}
			else if(opercode.CompareNoCase("buy")==0){//买物品
				BuyItem(opernum);
			}
			else if(opercode.CompareNoCase("trans")==0){//传送				
				if(!CharTrans(opernum)){
					ErrorLine=IP;
					break;
				}
			}
			else if(opercode.CompareNoCase("fightpet")==0){//改变战宠,0为没有战宠
				int pet;
				pet=atoi(opernum);
				if(pet<0 || pet>5){
					ErrorLine=IP;
					break;
				}
				if(!SendFightPet(pet-1)){
					ErrorLine=IP;
					break;
				}
			}
			else if(opercode.CompareNoCase("ccname")==0){//改变人物名称
				if(!SendChangeCharName((LPSTR)(LPCTSTR)opernum)){
					ErrorLine=IP;
					break;
				}
			}
			else if(opercode.CompareNoCase("cpname")==0){//改变宠物名称
				int pos=0,pet=-1;
				CString szPet,szName;
				szPet=opernum.Tokenize(",",pos);
				szName=opernum.Tokenize(",",pos);
				if(IsNumber(szPet))
					pet=atoi(szPet);
				if(pet<0 || pet>4){
					ErrorLine=IP;
					break;
				}
				if(!SendChangePetName(pet,(LPSTR)(LPCTSTR)szName)){
					ErrorLine=IP;
					break;
				}
			}
			else if(opercode.CompareNoCase("ll")==0){//料理合成命令
				if(!LiaoLi(opernum)){//参数出错停止脚本
					ErrorLine=IP;
					break;
				}				
			}
			else if(opercode.CompareNoCase("dpcheck")==0){//签发dp支票
				ip=IP;
				if(!SignDpCheck(opernum)){//参数出错停止脚本
					ErrorLine=IP;
					break;
				}
				if(ip!=IP){//出错跳转
					continue;
				}
			}
			else if(opercode.CompareNoCase("bt")==0){//摆滩
				if(!BaiTan(opernum)){//参数出错停止脚本
					ErrorLine=IP;
					break;
				}				
			}
			else if(opercode.CompareNoCase("cancelbt")==0){//取消摆滩
				bIsBaiTaning=FALSE;
				SendBaiTan("E|");							
			}
			else if(opercode.CompareNoCase("btbuy")==0){//从摆滩处购卖
				if(!BuyFromBaiTan(opernum)){//参数出错停止脚本
					ErrorLine=IP;
					break;
				}				
			}
			else if(opercode.CompareNoCase("excard")==0){//交换名片
				if(SendExchangeCard()!=SUCCESSFUL){//参数出错停止脚本
					ErrorLine=IP;
					break;
				}				
			}
			else if(opercode.CompareNoCase("delcard")==0){//删除名片
				int card;
				card=atoi(opernum);
				if(SendDeleteCard(card-1)!=SUCCESSFUL){//参数出错停止脚本
					ErrorLine=IP;
					break;
				}				
			}
			else if(opercode.CompareNoCase("petmail")==0){//发送宠物邮件
				if(!SendPetMail(opernum)){//参数出错停止脚本
					ErrorLine=IP;
					break;
				}				
			}	
			else if(opercode.CompareNoCase("petstate")==0){//设置宠物状态
				if(!SetPetState(opernum)){//参数出错停止脚本
					ErrorLine=IP;
					break;
				}				
			}	
			else if(opercode.CompareNoCase("ifmap")==0){//判断地图
				ip=IP;
				if(!IfMap(opernum)){//参数出错停止脚本
					ErrorLine=IP;
					break;
				}
				if(ip!=IP){//出错跳转
					continue;
				}
			}
			else if(opercode.CompareNoCase("ifpos")==0){//判断坐标
				ip=IP;
				if(!IfPos(opernum)){//参数出错停止脚本
					ErrorLine=IP;
					break;
				}
				if(ip!=IP){//出错跳转
					continue;
				}
			}
			else if(opercode.CompareNoCase("ifstone")==0){//判断金钱
				ip=IP;
				if(!IfStone(opernum)){//参数出错停止脚本
					ErrorLine=IP;
					break;
				}
				if(ip!=IP){//出错跳转
					continue;
				}
			}
			else if(opercode.CompareNoCase("iffight")==0){//判断回合数
				ip=IP;
				if(!IfFight(opernum)){//参数出错停止脚本
					ErrorLine=IP;
					break;
				}
				if(ip!=IP){//出错跳转
					continue;
				}
			}
			else if(opercode.CompareNoCase("ifdlg")==0){//判断对话框
				ip=IP;
				if(!IfDlg(opernum)){//参数出错停止脚本
					ErrorLine=IP;
					break;
				}
				if(ip!=IP){//出错跳转
					continue;
				}
			}
			else if(opercode.CompareNoCase("ifsay")==0){//判断说话
				ip=IP;
				if(!IfSay(opernum)){//参数出错停止脚本
					ErrorLine=IP;
					break;
				}
				if(ip!=IP){//出错跳转
					continue;
				}
			}
			else if(opercode.CompareNoCase("ifitem")==0){//判断道具
				CString szItem,szTest,szPara,szJump;
				pos=0;
				szItem=opernum.Tokenize(",",pos);
				if(szItem.IsEmpty()){
					ErrorLine=IP;
					break;
				}
				szTest=opernum.Tokenize(",",pos);
				if(szTest.IsEmpty()){
					ErrorLine=IP;
					break;
				}
				szPara=opernum.Tokenize(",",pos);
				if(szPara.IsEmpty()){
					ErrorLine=IP;
					break;
				}
				szJump=opernum.Tokenize(",",pos);
				if(szJump.IsEmpty()){
					ErrorLine=IP;
					break;
				}
				ip=IP;
				if(IsNumber(szItem) && szItem.GetLength()==3){//ifitem {101-109},{=|!=},{名称},{跳转符号}
					if(!IfEquipment(szItem,szTest,szPara,szJump)){
						ErrorLine=IP;
						break;
					}
				}
				else if(IsDigit(szItem.GetAt(0))){//ifitem {1|2|4-5},{=|!=},{空位},{跳转符号}
					if(szPara.Compare("空位")==0){
						if(!IfItemIndexPosEmpty(szItem,szTest,szPara,szJump)){
							ErrorLine=IP;
							break;
						}
					}
					else{//ifitem {1|2|4-5},{=|!=},{名称},{跳转符号}
						if(!IfItemIndexName(szItem,szTest,szPara,szJump)){
							ErrorLine=IP;
							break;
						}
					}
				}
				else if(szItem.Compare("空位")==0){//ifitem {空位},{=|!=|<|<=|>=|>},数量,{跳转符号}
					if(!IfItemPosEmpty(szItem,szTest,szPara,szJump)){
						ErrorLine=IP;
						break;
					}
				}
				else{//ifitem {名称},{=|!=|<|<=|>=|>},数量,{跳转符号}
					if(!IfItemName(szItem,szTest,szPara,szJump)){
						ErrorLine=IP;
						break;
					}			
				}
				if(ip!=IP){//出错跳转
					continue;
				}			
			}
			else if(opercode.CompareNoCase("ifpet")==0){//判断宠物
				CString szPet,szTest,szPara,szJump;
				pos=0;
				szPet=opernum.Tokenize(",",pos);
				if(szPet.IsEmpty()){
					ErrorLine=IP;
					break;
				}
				szTest=opernum.Tokenize(",",pos);
				if(szTest.IsEmpty()){
					ErrorLine=IP;
					break;
				}
				szPara=opernum.Tokenize(",",pos);
				if(szPara.IsEmpty()){
					ErrorLine=IP;
					break;
				}
				szJump=opernum.Tokenize(",",pos);
				if(szJump.IsEmpty()){
					ErrorLine=IP;
					break;
				}
				ip=IP;
				if(IsDigit(szPet.GetAt(0))){//ifpet {1|2|4-5},{=|!=},{空位},{跳转符号}
					if(szPara.Compare("空位")==0){
						if(!IfPetIndexPosEmpty(szPet,szTest,szPara,szJump)){
							ErrorLine=IP;
							break;
						}
					}
					else{//ifpet {1|2|4-5},{=|!=},{名称},{跳转符号}
						if(!IfPetIndexName(szPet,szTest,szPara,szJump)){
							ErrorLine=IP;
							break;
						}
					}
				}
				else if(szPet.Compare("空位")==0){//ifpet {空位},{=|!=|<|<=|>=|>},数量,{跳转符号}
					if(!IfPetPosEmpty(szPet,szTest,szPara,szJump)){
						ErrorLine=IP;
						break;
					}
				}
				else{//ifpet {名称},{=|!=|<|<=|>=|>},数量,{跳转符号}
					if(!IfPetName(szPet,szTest,szPara,szJump)){
						ErrorLine=IP;
						break;
					}			
				}
				if(ip!=IP){//出错跳转
					continue;
				}			
			}
			else if(opercode.CompareNoCase("check")==0){//判断
				CString szPara;
				pos=0;
				szPara=opernum.Tokenize(",",pos);
				ip=IP;
				if(szPara.Compare("人物")==0){
					if(!CheckChar(opernum)){
						ErrorLine=IP;
						break;
					}
				}
				else if(szPara.Compare("宠物")==0){
					if(!CheckPet(opernum)){
						ErrorLine=IP;
						break;
					}					
				}
				else if(szPara.Compare("地图")==0){
					if(!CheckMap(opernum)){
						ErrorLine=IP;
						break;
					}					
				}
				else if(szPara.Compare("坐标")==0){
					if(!CheckPosition(opernum)){
						ErrorLine=IP;
						break;
					}					
				}
				else if(szPara.Compare("说话")==0){
					if(!CheckSay(opernum)){
						ErrorLine=IP;
						break;
					}					
				}
				else if(szPara.Compare("对话")==0){
					if(!CheckDlg(opernum)){
						ErrorLine=IP;
						break;
					}					
				}
				else if(szPara.Compare("游戏状态")==0){
					if(!CheckGameState(opernum)){
						ErrorLine=IP;
						break;
					}					
				}
				else if(szPara.Compare("重登")==0){
					if(!CheckRelogin(opernum)){
						ErrorLine=IP;
						break;
					}					
				}
				else if(szPara.Compare("摆滩")==0){
					if(!CheckBaiTaning(opernum)){
						ErrorLine=IP;
						break;
					}					
				}
				if(ip!=IP){
					continue;
				}
			}
		}
		Sleep(nScriptDelay);
		IP++;
	}
Lab_ScriptExit:
	para.nNotityType=NOTIFY_STOPSCRIPT;
	szMsg.Format("%s|%d",user.charname,ErrorLine);	
	para.lpNotifyData=(LPVOID)(LPCTSTR)szMsg;
	csSingal.Lock();		
	Callback_Notify(&para);
	csSingal.Unlock();
	if(script)
		delete []script;
	script=NULL;
	intvar.RemoveAll();
	if(bScriptErrorReStart && ErrorLine!=0){
		Sleep(1000);
		goto ScriptStart;
	}
}

#pragma endregion

#pragma region 登陆登出

//检测用户名或密码是否正确，cdkey为用户名,pwd为密码
int CDpMain::CheckUser(char * cdkey, char * pwd)
{
	char buffer[8192], raw[8192], result[8192], message[8192];
	int func, fieldcount, checksum = 0, checksumrecv;
	int recvbytes;

#pragma region 登陆验证

	//发送用户名和密码,Default Key
	ZeroMemory(buffer, sizeof(buffer));
	checksum += autil.util_mkstring(buffer, cdkey);
	checksum += autil.util_mkstring(buffer, pwd);
	strcpy_s(message, "All Blue's Vip 专用ＳｔｏｎｅＡｇｅ;1");
	checksum += autil.util_mkstring(buffer, message);
	autil.util_mkint(buffer, checksum);

	//Running Key
	//Gen Running Key
	strcpy_s(autil.PersonalKey, _RUNNING_KEY);


	//发送登陆信息
	if (!autil.util_SendMesg(&socket, 71, buffer)){
		IsOnLine = FALSE;
		return SENDMSG_ERROR;
	}
	ZeroMemory(buffer, sizeof(buffer));
	recvbytes = recv(socket, buffer, sizeof(buffer), 0);
	if (recvbytes<8 || strlen(buffer) != recvbytes){
		recvbytes = recv(socket, buffer, sizeof(buffer), 0);
		if (recvbytes<8 || strlen(buffer) != recvbytes){
			IsOnLine = FALSE;
			return RECVMSG_ERROR;
		}
	}

	//检测用户名和密码是否正确
	autil.util_Init();
	autil.util_DecodeMessage(raw, buffer);
	if (!autil.util_SplitMessage(raw, SEPARATOR))
		return SPLITMSG_ERROR;
	if (!autil.util_GetFunctionFromSlice(&func, &fieldcount))
		return GETFUNC_ERROR;
	checksum = 0;
	if (func == 82 && fieldcount == 2){
		checksum += autil.util_destring(2, result);
		autil.util_deint(3, &checksumrecv);
		if (checksum != checksumrecv)
			return CHECKSUM_ERROR;
		if (strcmp(result, "ok") != 0 && strcmp(result, "successful") != 0)
			return CDKEY_AND_PWD_ERROR;
	}
	else if (func == 92 && fieldcount == 3){
		checksum += autil.util_destring(2, result);
		checksum += autil.util_destring(3, message);
		autil.util_deint(4, &checksumrecv);
		if (checksum != checksumrecv)
			return CHECKSUM_ERROR;
		if (strcmp(result, "failed") == 0)
			return NOT_LOGGED_IN;
	}
	else{
		DbgLog("CheckUser:func=%d,fieldcount=%d\r\n", func, fieldcount);
		return INVALID_INFO;
	}
	//向服务端发送接收就绪信息
	checksum = 0;
	ZeroMemory(buffer, sizeof(buffer));
	autil.util_mkint(buffer, checksum);
	if (!autil.util_SendMesg(&socket, 79, buffer)){
		IsOnLine = FALSE;
		return SENDMSG_ERROR;
	}
	ZeroMemory(buffer, sizeof(buffer));
	recvbytes = recv(socket, buffer, sizeof(buffer), 0);
	if (recvbytes <= 0)
		return RECVMSG_ERROR;

#pragma endregion  


#pragma region 获取帐号资料
	//获取帐号资料
	autil.util_Init();
	autil.util_DecodeMessage(raw, buffer);
	if (!autil.util_SplitMessage(raw, SEPARATOR))
		return SPLITMSG_ERROR;
	if (!autil.util_GetFunctionFromSlice(&func, &fieldcount))
		return GETFUNC_ERROR;
	checksum = 0;
	if (func == 90 && fieldcount == 3){
		checksum += autil.util_destring(2, result);
		checksum += autil.util_destring(3, message);//名字|编号|形象|等级|血|攻|防|敏|魅|DP|地|水|火|风|登入次数|名称|上次登出的地点
		autil.util_deint(4, &checksumrecv);
		if (checksum != checksumrecv)
			return CHECKSUM_ERROR;
		if (strcmp(result, "successful") != 0)
			return GET_ACCOUNT_INFO_ERROR;
	}
	else
		return INVALID_INFO;
	//未创建人物
	if (strlen(message) == 0)
		return NOACCOUNT;
	int pos, id;
	char buf[4096], *p, *source, *source1 = NULL;
	//如果有两个人物
	p = message + 30;
	p = strstr(p, "|");
	if (p){
		source1 = p + 1;
		message[p - message] = 0;
	}
	//解析第一个人物
	source = message;
	pos = 0;
	Tokenize(source, buf, "|", pos);
	Tokenize(source, buf, "\\", pos);
	id = atoi(buf);
	charlist[id].dataplace = id;
	Tokenize(source, buf, "\\", pos);
	charlist[id].faceimage = atoi(buf + 1);
	Tokenize(source, buf, "\\", pos);
	charlist[id].level = atoi(buf + 1);
	Tokenize(source, buf, "\\", pos);
	charlist[id].maxhp = atoi(buf + 1);
	Tokenize(source, buf, "\\", pos);
	charlist[id].atk = atoi(buf + 1);
	Tokenize(source, buf, "\\", pos);
	charlist[id].def = atoi(buf + 1);
	Tokenize(source, buf, "\\", pos);
	charlist[id].quick = atoi(buf + 1);
	Tokenize(source, buf, "\\", pos);
	charlist[id].charm = atoi(buf + 1);
	Tokenize(source, buf, "\\", pos);
	charlist[id].dp = atoi(buf + 1);
	Tokenize(source, buf, "\\", pos);
	charlist[id].earth = atoi(buf + 1);
	Tokenize(source, buf, "\\", pos);
	charlist[id].water = atoi(buf + 1);
	Tokenize(source, buf, "\\", pos);
	charlist[id].fire = atoi(buf + 1);
	Tokenize(source, buf, "\\", pos);
	charlist[id].wind = atoi(buf + 1);
	Tokenize(source, buf, "\\", pos);
	charlist[id].logincount = atoi(buf + 1);
	Tokenize(source, buf, "\\", pos);
	strcpy_s(charlist[id].name, buf + 1);
	Tokenize(source, buf, "\\", pos);
	strcpy_s(charlist[id].place, buf + 1);
	if (source1){
		source = source1;
		pos = 0;
		Tokenize(source, buf, "|", pos);
		Tokenize(source, buf, "\\", pos);
		id = atoi(buf);
		charlist[id].dataplace = id;
		Tokenize(source, buf, "\\", pos);
		charlist[id].faceimage = atoi(buf + 1);
		Tokenize(source, buf, "\\", pos);
		charlist[id].level = atoi(buf + 1);
		Tokenize(source, buf, "\\", pos);
		charlist[id].maxhp = atoi(buf + 1);
		Tokenize(source, buf, "\\", pos);
		charlist[id].atk = atoi(buf + 1);
		Tokenize(source, buf, "\\", pos);
		charlist[id].def = atoi(buf + 1);
		Tokenize(source, buf, "\\", pos);
		charlist[id].quick = atoi(buf + 1);
		Tokenize(source, buf, "\\", pos);
		charlist[id].charm = atoi(buf + 1);
		Tokenize(source, buf, "\\", pos);
		charlist[id].dp = atoi(buf + 1);
		Tokenize(source, buf, "\\", pos);
		charlist[id].earth = atoi(buf + 1);
		Tokenize(source, buf, "\\", pos);
		charlist[id].water = atoi(buf + 1);
		Tokenize(source, buf, "\\", pos);
		charlist[id].fire = atoi(buf + 1);
		Tokenize(source, buf, "\\", pos);
		charlist[id].wind = atoi(buf + 1);
		Tokenize(source, buf, "\\", pos);
		charlist[id].logincount = atoi(buf + 1);
		Tokenize(source, buf, "\\", pos);
		strcpy_s(charlist[id].name, buf + 1);
		Tokenize(source, buf, "\\", pos);
		strcpy_s(charlist[id].place, buf + 1);
	}
#pragma endregion

	return SUCCESSFUL;
}

void CDpMain::SetExit(BOOL bexit)
{
	bExit = bexit;
	SendMenuFlag(MENUFLAG);
}

void CDpMain::SetScriptExit(BOOL bexit)
{
	bScriptExit = bexit;
}

//检测用户名和密码后点击登入按钮
int CDpMain::CharLogin(int dataplace)
{
	char buffer[8192];
	int checksum = 0;

	//新号登入
	checksum = 0;
	ZeroMemory(buffer, sizeof(buffer));
	checksum += autil.util_mkstring(buffer, charlist[dataplace].name);
	autil.util_mkint(buffer, checksum);
	if (!autil.util_SendMesg(&socket, 77, buffer)){
		IsOnLine = FALSE;
		return SENDMSG_ERROR;
	}
	//if(type==1){//安全码登入，等待返回错误信息
	//	ZeroMemory(buffer,sizeof(buffer));
	//	recvbytes=recv(socket,buffer,sizeof(buffer),0);
	//	if(recvbytes<=0)
	//		return RECVMSG_ERROR;
	//	autil.util_Init();
	//	autil.util_DecodeMessage(raw,buffer);
	//	if(!autil.util_SplitMessage(raw,SEPARATOR))
	//		return SPLITMSG_ERROR;
	//	if(!autil.util_GetFunctionFromSlice(&func, &fieldcount))
	//		return GETFUNC_ERROR;
	//	checksum=0;
	//	if(func==88 && fieldcount==3){
	//		checksum += autil.util_destring(2,result);
	//		checksum += autil.util_destring(3,message);
	//		autil.util_deint(4, &checksumrecv);
	//		if(checksum!=checksumrecv)	
	//			return CHECKSUM_ERROR;
	//		if(strcmp(result,"failed")!=0 && strcmp(message,"Password is wrong")!=0)
	//			return CREATE_NEWCHAR_ERROR;	
	//	}
	//	else
	//		return INVALID_INFO;

	//}
	return SUCCESSFUL;
}

#pragma endregion

#pragma region 脚本

//获取指标签的行号
int CDpMain::GetLabelNum(CString label)
{
	int i;
	CString szVal;
	for (i = 0; i<scriptNum; i++){
		if (script[i].Find("label") == 0){
			szVal = GetOperateNum(script[i]);
			if (szVal.CompareNoCase(label) == 0){
				return i;
			}
		}
	}
	return -1;
}

//取指令的操作码
CString CDpMain::GetOperateCode(CString instruction)
{
	int pos;
	pos = instruction.Find(" ");
	if (pos>0)
		return instruction.Left(pos);
	else if (instruction.GetLength()>0)
		return instruction;
	else
		return "";
}

//取指令的操作数
CString CDpMain::GetOperateNum(CString instruction)
{
	int pos;
	pos = instruction.Find(" ");
	if (pos>0)
		return instruction.Right(instruction.GetLength() - pos - 1).Trim();
	else
		return "";
}

//解析set参数
void CDpMain::ParseSetPara(CString &data)
{
	int pos = 0, i;
	CString szSign, szPara, szVal;
	szSign = data.Tokenize(",", pos);
	szPara = data.Right(data.GetLength() - pos);
	if (szSign.CompareNoCase("脚本延时") == 0){
		nScriptDelay = atoi(szPara);
	}
	else if (szSign.CompareNoCase("自动解锁") == 0){
		nAutoUnlock = atoi(szPara);
		char msg[1024] = { 0 };
		if (nAutoUnlock && !IsSafeCodeUnlock || !nAutoUnlock && IsSafeCodeUnlock){
			sprintf_s(msg, "/safe %s", user.safecode);
			SendTalk(charotherinfo.x, charotherinfo.y, msg, 2, 3);
			Sleep(300);
		}
	}
	else if (szSign.CompareNoCase("断线重登") == 0){
		bReLogin = atoi(szPara);
	}
	else if (szSign.CompareNoCase("删除帐号") == 0){
		bDeleteChar = atoi(szPara);
	}
	else if (szSign.CompareNoCase("出错重启") == 0){
		bScriptErrorReStart = atoi(szPara);
	}
	else if (szSign.CompareNoCase("自动捉宠") == 0){
		bAutoCapture = atoi(szPara);
	}
	else if (szSign.CompareNoCase("自动逃跑") == 0){
		bAutoEscape = atoi(szPara);
	}
	else if (szSign.CompareNoCase("宠死逃跑") == 0){
		bFightpetDeadEscape = atoi(szPara);
	}
	else if (szSign.CompareNoCase("自动KNPC") == 0){
		bAutoKNPC = atoi(szPara);
	}
	else if (szSign.CompareNoCase("落马逃跑") == 0){
		bFullEscape = atoi(szPara);
	}
	else if (szSign.CompareNoCase("组队") == 0){//选中组队+1
		bMakeTeam = atoi(szPara);
		int n = 0;
		if (bMakeTeam){
			charotherinfo.menuflg |= 1;
			SendMenuFlag(charotherinfo.menuflg);
			Sleep(100);
			while (!(charotherinfo.menuflg & 1) && n<3){
				SendMenuFlag(charotherinfo.menuflg);
				n++;
				Sleep(100);
			}
		}
		else{
			charotherinfo.menuflg &= 0xfffe;
			SendMenuFlag(charotherinfo.menuflg);
			Sleep(100);
			while ((charotherinfo.menuflg & 1) && n<3){
				SendMenuFlag(charotherinfo.menuflg);
				n++;
				Sleep(100);
			}
		}
	}
	else if (szSign.CompareNoCase("决斗") == 0){//选中决斗+4
		bDuel = atoi(szPara);
		int n = 0;
		if (bDuel){
			charotherinfo.menuflg |= 0x0004;
			SendMenuFlag(charotherinfo.menuflg);
			Sleep(100);
			while (!((charotherinfo.menuflg >> 2) & 1) && n<3){
				SendMenuFlag(charotherinfo.menuflg);
				n++;
				Sleep(100);
			}
		}
		else{
			charotherinfo.menuflg &= 0xfffb;
			SendMenuFlag(charotherinfo.menuflg);
			Sleep(100);
			while (((charotherinfo.menuflg >> 2) & 1) && n<3){
				SendMenuFlag(charotherinfo.menuflg);
				n++;
				Sleep(100);
			}
		}
	}
	else if (szSign.CompareNoCase("名片") == 0){//选中交换名片+16
		bCallingCard = atoi(szPara);
		int n = 0;
		if (bCallingCard){
			charotherinfo.menuflg |= 0x0010;
			SendMenuFlag(charotherinfo.menuflg);
			Sleep(100);
			while (!((charotherinfo.menuflg >> 4) & 1) && n<3){
				SendMenuFlag(charotherinfo.menuflg);
				n++;
				Sleep(100);
			}
		}
		else{
			charotherinfo.menuflg &= 0xffef;
			SendMenuFlag(charotherinfo.menuflg);
			Sleep(100);
			while (((charotherinfo.menuflg >> 4) & 1) && n<3){
				SendMenuFlag(charotherinfo.menuflg);
				n++;
				Sleep(100);
			}
		}
	}
	else if (szSign.CompareNoCase("交易") == 0){//选中交易+32
		bTrade = atoi(szPara);
		int n = 0;
		if (bTrade){
			charotherinfo.menuflg |= 0x0020;
			SendMenuFlag(charotherinfo.menuflg);
			Sleep(100);
			while (!((charotherinfo.menuflg >> 5) & 1) && n<3){
				SendMenuFlag(charotherinfo.menuflg);
				n++;
				Sleep(100);
			}
		}
		else{
			charotherinfo.menuflg &= 0xffdf;
			SendMenuFlag(charotherinfo.menuflg);
			Sleep(100);
			while (((charotherinfo.menuflg >> 5) & 1) && n<3){
				SendMenuFlag(charotherinfo.menuflg);
				n++;
				Sleep(100);
			}
		}
	}
	else if (szSign.CompareNoCase("指定攻击") == 0){
		i = 0;
		pos = 0;
		szVal = szPara.Tokenize("|", pos);
		if (szVal.Compare("0") == 0){
			nZDAttackNum = 0;
		}
		else{
			nZDAttackNum = 0;
			while (szVal.GetLength()>0){
				strcpy_s(cZDAttack[i], szVal);
				nZDAttackNum++;
				i++;
				szVal = szPara.Tokenize("|", pos);
			}
		}
	}
	else if (szSign.CompareNoCase("锁定攻击") == 0){
		i = 0;
		pos = 0;
		szVal = szPara.Tokenize("|", pos);
		if (szVal.Compare("0") == 0){
			nLockAttackNum = 0;
		}
		else{
			nLockAttackNum = 0;
			while (szVal.GetLength()>0){
				strcpy_s(cLockAttack[i], szVal);
				nLockAttackNum++;
				i++;
				szVal = szPara.Tokenize("|", pos);
			}
		}
	}
	else if (szSign.CompareNoCase("指定逃跑") == 0){
		i = 0;
		pos = 0;
		szVal = szPara.Tokenize("|", pos);
		if (szVal.Compare("0") == 0){
			nZDEscapeNum = 0;
		}
		else{
			nZDEscapeNum = 0;
			while (szVal.GetLength()>0){
				strcpy_s(cZDEscape[i], szVal);
				nZDEscapeNum++;
				i++;
				szVal = szPara.Tokenize("|", pos);
			}
		}
	}
	else if (szSign.CompareNoCase("自动加点") == 0){//set 自动加点,体力:20|腕力:40
		i = 0;
		pos = 0;
		int n, val;
		CString szWhich, szValue;
		szVal = szPara.Tokenize("|", pos);
		//清零
		for (i = 0; i<4; i++)
			nAutoUpPoint[i] = 0;
		i = 0;
		if (szVal.Compare("0") != 0){
			while (szVal.GetLength()>0){
				n = szVal.Find(":");
				if (n>0){
					szWhich = szVal.Left(n);
					szValue = szVal.Right(szVal.GetLength() - n - 1);
					val = atoi(szValue);
					if (szWhich.Compare("体力") == 0 && val>0 && val<1000){
						nAutoUpPoint[i] = val;
					}
					else if (szWhich.Compare("腕力") == 0 && val>0 && val<1000){
						nAutoUpPoint[i] = 1000 + val;
					}
					else if (szWhich.Compare("耐力") == 0 && val>0 && val<1000){
						nAutoUpPoint[i] = 2000 + val;
					}
					else if (szWhich.Compare("速度") == 0 && val>0 && val<1000){
						nAutoUpPoint[i] = 3000 + val;
					}
					i++;
				}
				szVal = szPara.Tokenize("|", pos);
			}
			if (charotherinfo.state == 0)//游戏平时状态加点
				AutoAddUpLevelPoint();
		}
	}
	else if (szSign.CompareNoCase("吃鱼鳃草") == 0){
		bAutoEatSYC = atoi(szPara);
	}
	else if (szSign.CompareNoCase("吃智慧果") == 0){
		bAutoExpNut = atoi(szPara);
	}
	else if (szSign.CompareNoCase("自动堆叠") == 0){
		bAutoPile = atoi(szPara);
		SendAutoPile();
	}
	else if (szSign.CompareNoCase("吃补血肉") == 0){
		bEatMeat = atoi(szPara);
		if (bEatMeat)
			SendEatMeat();
	}
	else if (szSign.CompareNoCase("丢非血肉") == 0){
		bDiscardMeat = atoi(szPara);
		if (bDiscardMeat)
			SendDiscardMeat();
	}
	else if (szSign.CompareNoCase("平时补气") == 0){
		bRecruitMpPlaceTime = atoi(szPara);
		if (bRecruitMpPlaceTime && charotherinfo.state == 0 && ((double)chardetail.mp / chardetail.maxmp) <= 0.3)
			SendRecruitMpPlaceTime();
	}
	else if (szSign.CompareNoCase("锁定骑宠") == 0){//set 锁定骑宠,1-5|0不锁定
		nLockRidePet = atoi(szPara);
		if (nLockRidePet >= 0 && nLockRidePet <= 5)
			SendRidePet(nLockRidePet - 1);
	}
	else if (szSign.CompareNoCase("锁定战宠") == 0){//set 锁定战宠,1-5|0不锁定
		nLockFightPet = atoi(szPara);
		if (nLockFightPet >= 0 && nLockFightPet <= 5)
			SendFightPet(nLockFightPet - 1);
	}
	else if (szSign.CompareNoCase("丢弃中为") == 0){
		i = 0;
		pos = 0;
		szVal = szPara.Tokenize("|", pos);
		if (szVal.Compare("0") == 0){
			nDiscardLikeItemNum = 0;
		}
		else{
			nDiscardLikeItemNum = 0;
			while (szVal.GetLength()>0 && nDiscardLikeItemNum<15){
				strcpy_s(cDiscardLikeItem[i], szVal);
				nDiscardLikeItemNum++;
				i++;
				szVal = szPara.Tokenize("|", pos);
			}
			SendDiscardLikeItem();
		}
	}
	else if (szSign.CompareNoCase("丢弃中非") == 0){
		i = 0;
		pos = 0;
		szVal = szPara.Tokenize("|", pos);
		if (szVal.Compare("0") == 0){
			nDiscardExceptItemNum = 0;
		}
		else{
			nDiscardExceptItemNum = 0;
			while (szVal.GetLength()>0 && nDiscardExceptItemNum<15){
				strcpy_s(cDiscardExceptItem[i], szVal);
				nDiscardExceptItemNum++;
				i++;
				szVal = szPara.Tokenize("|", pos);
			}
			SendDiscardExceptItem();
		}
	}
	else if (szSign.CompareNoCase("首次延迟") == 0){
		dFirstDelay = atof(szPara);
	}
	else if (szSign.CompareNoCase("攻击延迟") == 0){
		dAttackDelay = atof(szPara);
	}
	else if (szSign.CompareNoCase("嗜血补气") == 0){
		dRMpByBlood = PerStrToDbl(szPara) / 100;
	}
	else if (szSign.CompareNoCase("人物首次") == 0){//0:攻击，1：防御，2：逃跑，3：帽子，4：衣服，5：武器，6：左饰，7：右饰，8以后为技能编号(1-30)
		strcpy_s(cCharFirstAction, szPara);
	}
	else if (szSign.CompareNoCase("人物一般") == 0){//人物攻击时所用技能编号对应技能列表CHARSKILL中的技能索引编号
		strcpy_s(cCharAction, szPara);
	}
	else if (szSign.CompareNoCase("战宠首次") == 0){//技能名称，宠物攻击时所用技能编号对应PETSKILL中的技能索引编号
		strcpy_s(cPetFirstAction, szPara);
	}
	else if (szSign.CompareNoCase("战宠一般") == 0){
		strcpy_s(cPetAction, szPara);
	}
	else if (szSign.CompareNoCase("捕捉等级") == 0){
		nCapLevel = atoi(szPara);
	}
	else if (szSign.CompareNoCase("捉宠名称") == 0){
		i = 0;
		pos = 0;
		szVal = szPara.Tokenize("|", pos);
		if (szVal.Compare("0") == 0){
			nCapPetNum = 0;
		}
		else{
			nCapPetNum = 0;
			while (szVal.GetLength()>0 && nCapPetNum<5){
				strcpy_s(cCapPetName[i], szVal);
				nCapPetNum++;
				i++;
				szVal = szPara.Tokenize("|", pos);
			}
		}
	}
	else if (szSign.CompareNoCase("捉宠血量") == 0){
		nCapPetBlood = atoi(szPara);
	}
	else if (szSign.CompareNoCase("捉宠人物技能") == 0){//set 捉宠人物技能,武器:20%
		int n, val;
		CString szWhich, szValue;
		if (szPara.Compare("0") == 0){
			nCapCharUseSkill = 0;
		}
		else{
			if (szPara.GetLength()>0){
				n = szPara.Find(":");
				if (n>0){
					szWhich = szPara.Left(n);
					szValue = szPara.Right(szPara.GetLength() - n - 1);
					val = (int)PerStrToDbl(szValue);
					if (szWhich.Compare("帽子") == 0 && val >= 0 && val <= 100){
						nCapCharUseSkill = val;
					}
					else if (szWhich.Compare("衣服") == 0 && val >= 0 && val <= 100){
						nCapCharUseSkill = 1000 + val;
					}
					else if (szWhich.Compare("武器") == 0 && val >= 0 && val <= 100){
						nCapCharUseSkill = 2000 + val;
					}
					else if (szWhich.Compare("左饰") == 0 && val >= 0 && val <= 100){
						nCapCharUseSkill = 3000 + val;
					}
					else if (szWhich.Compare("右饰") == 0 && val >= 0 && val <= 100){
						nCapCharUseSkill = 4000 + val;
					}
				}
			}
		}
	}
	else if (szSign.CompareNoCase("捉宠宠物技能") == 0){//set 捉宠宠物技能,技能编号(0-6)
		nCapPetSkill = atoi(szPara);
	}
	else if (szSign.CompareNoCase("捉宠无宠动作") == 0){//set 捉宠无宠动作,战斗|逃跑
		if (strcmp(szPara, "战斗") == 0)
			bCapEscapeWhenNoPet = FALSE;
		else
			bCapEscapeWhenNoPet = TRUE;
	}
	else if (szSign.CompareNoCase("精灵补血") == 0){//set 精灵补血,衣服:70%
		int n, val;
		CString szWhich, szValue;
		if (szPara.Compare("0") == 0){
			recruitblood.id = -1;
			ZeroMemory(recruitblood.name, sizeof(recruitblood.name));
		}
		else{
			if (szPara.GetLength()>0){
				n = szPara.Find(":");
				if (n>0){
					szWhich = szPara.Left(n);
					szValue = szPara.Right(szPara.GetLength() - n - 1);
					val = (int)PerStrToDbl(szValue);
					if (szWhich.Compare("帽子") == 0 && val >= 0 && val <= 100){
						recruitblood.id = 0;
						strcpy_s(recruitblood.name, magic[0].name);
						recruitblood.val = val;
					}
					else if (szWhich.Compare("衣服") == 0 && val >= 0 && val <= 100){
						recruitblood.id = 1;
						strcpy_s(recruitblood.name, magic[1].name);
						recruitblood.val = val;
					}
					else if (szWhich.Compare("武器") == 0 && val >= 0 && val <= 100){
						recruitblood.id = 2;
						strcpy_s(recruitblood.name, magic[2].name);
						recruitblood.val = val;
					}
					else if (szWhich.Compare("左饰") == 0 && val >= 0 && val <= 100){
						recruitblood.id = 3;
						strcpy_s(recruitblood.name, magic[3].name);
						recruitblood.val = val;
					}
					else if (szWhich.Compare("右饰") == 0 && val >= 0 && val <= 100){
						recruitblood.id = 4;
						strcpy_s(recruitblood.name, magic[4].name);
						recruitblood.val = val;
					}
				}
			}
		}
	}
	else if (szSign.CompareNoCase("宠物补血") == 0){//set 宠物补血,技能编号（0-6）:70%
		int n, val;
		CString szWhich, szValue;
		if (szPara.Compare("0") == 0){
			petrecruitblood.skillid = -1;
		}
		else{
			if (szPara.GetLength()>0){
				n = szPara.Find(":");
				if (n>0){
					szWhich = szPara.Left(n);
					szValue = szPara.Right(szPara.GetLength() - n - 1);
					val = (int)PerStrToDbl(szValue);
					petrecruitblood.skillid = atoi(szWhich);
					petrecruitblood.val = val;
				}
			}
		}
	}
}

//计算跳转地址
BOOL CDpMain::CalcJampAddress(CString szJump, int &lid)
{
	CString szAddr;
	int pos = 0;
	if (IsNumber(szJump)){
		lid = IP + atoi(szJump);
		if (lid >= scriptNum){
			return FALSE;
		}
	}
	else{
		szAddr = szJump.Tokenize(" ", pos);
		if (szAddr.CompareNoCase("call") == 0)
			szAddr = szJump.Tokenize(" ", pos);
		lid = GetLabelNum(szAddr);
		if (lid == -1){
			return FALSE;
		}
		lid += 1;
	}
	return TRUE;
}

//等待地图出现，waitmap 地图编号,等待时间[,未出现跳转]
BOOL CDpMain::WaitMap(CString para)
{
	CString szmapid, szwaittime, szjump;
	int mapid, waittime, starttime, lid, pos;
	pos = 0;
	szmapid = para.Tokenize(",", pos);
	mapid = atoi(szmapid);
	szwaittime = para.Tokenize(",", pos);
	waittime = atoi(szwaittime);
	if (pos<para.GetLength())
		szjump = para.Tokenize(",", pos);
	else
		szjump = "";

	starttime = GetTickCount();
	if (szjump.IsEmpty()){//不需跳转时
		while ((int)(GetTickCount() - starttime)<waittime){
			if (mapinfo.floor == mapid){
				MapSeqNo++;
				break;
			}
			Sleep(20);
		}
	}
	else{//地图切换失败则跳转
		//计算跳转地址
		if (!CalcJampAddress(szjump, lid)){
			return FALSE;
		}
		//等待跳转
		while ((int)(GetTickCount() - starttime)<waittime){
			if (mapinfo.floor == mapid){
				MapSeqNo++;
				break;
			}
			Sleep(20);
		}
		//if(mapinfo.seqno==MapSeqNo){//地图未切换跳转
		if (mapinfo.floor != mapid){
			if (szjump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	return TRUE;
}

//传送，trans {地点编号|地点名称}[,NOTALK]
BOOL CDpMain::CharTrans(CString para)
{
	int starttime, pos;
	CString szToWhere, szTalk;
	pos = 0;
	szToWhere = para.Tokenize(",", pos);
	if (szToWhere.IsEmpty())
		return FALSE;
	szTalk = para.Tokenize(",", pos);
	//notalk为传送前不说话
	if (szTalk.IsEmpty()){
		SendTalk(charotherinfo.x, charotherinfo.y, "传送", 2, 3);
		Sleep(100);
	}
	//等待弹出欢迎窗口
	starttime = GetTickCount();
	while ((GetTickCount() - starttime) <= 2000){
		if (windowinfo.windowtype == WINDOW_MESSAGETYPE_MESSAGE && windowinfo.buttontype == WINDOW_BUTTONTYPE_YESNO){
			break;
		}
		Sleep(20);
	}
	//弹出欢迎窗口
	if (windowinfo.windowtype == WINDOW_MESSAGETYPE_MESSAGE && windowinfo.buttontype == WINDOW_BUTTONTYPE_YESNO){
		//选买按钮
		SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, WINDOW_BUTTONTYPE_YES, "");
		//等待弹出传送点信息
		starttime = GetTickCount();
		while ((GetTickCount() - starttime)<4000){
			if (windowinfo.windowtype == WINDOW_MESSAGETYPE_SELECT && windowinfo.buttontype == WINDOW_BUTTONTYPE_CANCEL){
				break;
			}
			Sleep(20);
		}
		//如果弹出传送点信息
		if (windowinfo.windowtype == WINDOW_MESSAGETYPE_SELECT && windowinfo.buttontype == WINDOW_BUTTONTYPE_CANCEL){
			int pos = 0;
			char buf[200] = { 0 };
			//传送点信息
			Tokenize(windowinfo.data, buf, " ", pos);
			strcpy_s(trans_point.main_msg, buf);
			trans_point.num = 0;
			pos += 9;
			while ((unsigned int)pos<strlen(windowinfo.data)){
				Tokenize(windowinfo.data, buf, "          ", pos);
				if (strlen(buf)>2){
					strcpy_s(trans_point.trans_point[trans_point.num], buf);
					trans_point.num++;
				}
			}
			int id, i;
			//分析传送点
			if (IsNumber(szToWhere)){
				id = atoi(szToWhere);
			}
			else{
				for (i = 0; i<trans_point.num; i++){
					if (strstr(trans_point.trans_point[i], szToWhere) != NULL)
						break;
				}
				if (i >= trans_point.num)
					id = -1;
				else
					id = i + 1;
			}
			if (id >= 1 && id <= trans_point.num){
				_itoa_s(id, buf, 10);
				SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, 0, buf);
			}
			else{
				return FALSE;
			}
		}
	}

	return TRUE;
}

//变量判断，if 变量,{=,>,<,>=,<=,!=},数值|变量,跳转
BOOL CDpMain::If_Check(CString para)
{
	int pos = 0;
	CString szvar, szop, sznum, szjump;
	szvar = para.Tokenize(",", pos);
	szop = para.Tokenize(",", pos);
	sznum = para.Tokenize(",", pos);
	szjump = para.Tokenize(",", pos);
	if (szvar.GetAt(0) == '@' && szop.GetLength()>0 && (IsNumber(sznum) || sznum.GetAt(0) == '@')){
		int line, t, t1;
		if (!CalcJampAddress(szjump, line)){
			return FALSE;
		}
		intvar.Lookup(szvar, t);
		if (IsNumber(sznum))
			t1 = atoi(sznum);
		else{
			GetValue(sznum, t1);
		}
		if (szop.Compare("=") == 0){
			if (t == t1){
				if (szjump.Left(5).CompareNoCase("call ") == 0)
					IPStack.push(IP);
				IP = line;
			}
		}
		else if (szop.Compare(">=") == 0){
			if (t >= t1){
				if (szjump.Left(5).CompareNoCase("call ") == 0)
					IPStack.push(IP);
				IP = line;
			}
		}
		else if (szop.Compare("<=") == 0){
			if (t <= t1){
				if (szjump.Left(5).CompareNoCase("call ") == 0)
					IPStack.push(IP);
				IP = line;
			}
		}
		else if (szop.Compare(">") == 0){
			if (t>t1){
				if (szjump.Left(5).CompareNoCase("call ") == 0)
					IPStack.push(IP);
				IP = line;
			}
		}
		else if (szop.Compare("<") == 0){
			if (t<t1){
				if (szjump.Left(5).CompareNoCase("call ") == 0)
					IPStack.push(IP);
				IP = line;
			}
		}
		else if (szop.Compare("!=") == 0){
			if (t != t1){
				if (szjump.Left(5).CompareNoCase("call ") == 0)
					IPStack.push(IP);
				IP = line;
			}
		}
		else{
			return FALSE;
		}
	}
	else{
		return FALSE;
	}
	return TRUE;
}

//取变量的值，包括系统变量
void CDpMain::GetValue(CString &key, int &val)
{
	if (key.CompareNoCase("@[石币]") == 0)
		val = chardetail.gold;
	else if (key.CompareNoCase("@[X坐标]") == 0)
		val = charotherinfo.x;
	else if (key.CompareNoCase("@[Y坐标]") == 0)
		val = charotherinfo.y;
	else if (key.CompareNoCase("@[地图]") == 0)
		val = mapinfo.floor;
	else if (key.CompareNoCase("@[DP]") == 0)
		val = chardetail.dp;
	else
		intvar.Lookup(key, val);
}

//变量赋值,let 变量,{=,+,-,*,/},数值(可以是小数)|变量
BOOL CDpMain::Let_Set(CString para)
{
	int pos = 0;
	CString szvar, szop, sznum;
	szvar = para.Tokenize(",", pos);
	szop = para.Tokenize(",", pos);
	sznum = para.Tokenize(",", pos);
	if (szvar.GetAt(0) == '@' && szop.GetLength()>0 && (IsNumber(sznum) || sznum.GetAt(0) == '@')){
		int t, t2;
		double t1 = 0;
		intvar.Lookup(szvar, t);
		if (IsNumber(sznum))
			t1 = atof(sznum);
		else{
			GetValue(sznum, t2);
			t1 = (double)t2;
		}
		if (szop.Compare("=") == 0)
			intvar.SetAt(szvar, t1);
		else if (szop.Compare("+") == 0)
			intvar.SetAt(szvar, (int)(t1 + t));
		else if (szop.Compare("-") == 0)
			intvar.SetAt(szvar, (int)(t - t1));
		else if (szop.Compare("*") == 0)
			intvar.SetAt(szvar, (int)(t*t1));
		else if (szop.Compare("/") == 0)
			intvar.SetAt(szvar, (int)(t / t1));
		else{
			return FALSE;
		}
	}
	else{
		return FALSE;
	}
	return TRUE;
}

//等待说话内容出现,waitsay {内容|?},{系统|自己|*},{等待时间(数值|变量)},{未出现跳转}.?代个说话内容任意
BOOL CDpMain::WaitSay(CString para)
{
	int pos = 0;
	CString szText, szWho, szWaitTime, szJump;
	szText = para.Tokenize(",", pos);
	if (szText.IsEmpty())
		return FALSE;
	szWho = para.Tokenize(",", pos);
	if (szWho.IsEmpty())
		return FALSE;
	szWaitTime = para.Tokenize(",", pos);
	if (szWaitTime.IsEmpty())
		return FALSE;
	szJump = para.Tokenize(",", pos);
	if (szJump.IsEmpty())
		return FALSE;

	int starttime, waittime, lid;
	//等待时间
	if (IsNumber(szWaitTime))
		waittime = atoi(szWaitTime);
	else
		GetValue(szWaitTime, waittime);
	if (waittime <= 0 || waittime>10000)
		waittime = 10000;
	//计算跳转地址
	if (!CalcJampAddress(szJump, lid)){
		return FALSE;
	}

	deque<TALKMESSAGE *>::const_reverse_iterator v1_rIter;
	starttime = GetTickCount();
	if (szWho.Compare("系统") == 0){
		if (szText.CompareNoCase("?") == 0){
			while ((int)(GetTickCount() - starttime) <= waittime){
				csLocalSingal.Lock();
				for (v1_rIter = talkmessage.rbegin(); v1_rIter != talkmessage.rend(); v1_rIter++){
					if ((*v1_rIter)->whotalk == -1 && strlen((*v1_rIter)->message)>0){
						csLocalSingal.Unlock();
						return TRUE;
					}
				}
				csLocalSingal.Unlock();
				Sleep(20);
			}
		}
		else{
			while ((int)(GetTickCount() - starttime) <= waittime){
				csLocalSingal.Lock();
				for (v1_rIter = talkmessage.rbegin(); v1_rIter != talkmessage.rend(); v1_rIter++){
					if ((*v1_rIter)->whotalk == -1 && strstr((*v1_rIter)->message, szText) != NULL){
						csLocalSingal.Unlock();
						return TRUE;
					}
				}
				csLocalSingal.Unlock();
				Sleep(20);
			}
		}
		//没找到没跳转
		if (szJump.Left(5).CompareNoCase("call ") == 0)
			IPStack.push(IP);
		IP = lid;
	}
	else if (szWho.Compare("自己") == 0){
		if (szText.CompareNoCase("?") == 0){
			while ((int)(GetTickCount() - starttime) <= waittime){
				csLocalSingal.Lock();
				for (v1_rIter = talkmessage.rbegin(); v1_rIter != talkmessage.rend(); v1_rIter++){
					if ((*v1_rIter)->whotalk == charotherinfo.charid && strlen((*v1_rIter)->message)>0){
						csLocalSingal.Unlock();
						return TRUE;
					}
				}
				csLocalSingal.Unlock();
				Sleep(20);
			}
		}
		else{
			while ((int)(GetTickCount() - starttime) <= waittime){
				csLocalSingal.Lock();
				for (v1_rIter = talkmessage.rbegin(); v1_rIter != talkmessage.rend(); v1_rIter++){
					if ((*v1_rIter)->whotalk == charotherinfo.charid && strstr((*v1_rIter)->message, szText) != NULL){
						csLocalSingal.Unlock();
						return TRUE;
					}
				}
				csLocalSingal.Unlock();
				Sleep(20);
			}
		}
		//没找到没跳转
		if (szJump.Left(5).CompareNoCase("call ") == 0)
			IPStack.push(IP);
		IP = lid;
	}
	else{
		if (szText.CompareNoCase("?") == 0){
			while ((int)(GetTickCount() - starttime) <= waittime){
				csLocalSingal.Lock();
				for (v1_rIter = talkmessage.rbegin(); v1_rIter != talkmessage.rend(); v1_rIter++){
					if (((*v1_rIter)->whotalk != -1 && (*v1_rIter)->whotalk != charotherinfo.charid) && strlen((*v1_rIter)->message)>0){
						csLocalSingal.Unlock();
						return TRUE;
					}
				}
				csLocalSingal.Unlock();
				Sleep(20);
			}
		}
		else{
			while ((int)(GetTickCount() - starttime) <= waittime){
				csLocalSingal.Lock();
				for (v1_rIter = talkmessage.rbegin(); v1_rIter != talkmessage.rend(); v1_rIter++){
					if (((*v1_rIter)->whotalk != -1 && (*v1_rIter)->whotalk != charotherinfo.charid) && strstr((*v1_rIter)->message, szText) != NULL){
						csLocalSingal.Unlock();
						return TRUE;
					}
				}
				csLocalSingal.Unlock();
				Sleep(20);
			}
		}
		if (szJump.Left(5).CompareNoCase("call ") == 0)
			IPStack.push(IP);
		//没找到没跳转
		IP = lid;
	}
	return TRUE;

}

//等待对话框出现,waitdly 内容|?,等待时间(数值|变量),未出现跳转
BOOL CDpMain::WaitDlg(CString para)
{
	int pos = 0;
	CString szText, szWaitTime, szJump;
	szText = para.Tokenize(",", pos);
	if (szText.IsEmpty())
		return FALSE;
	szWaitTime = para.Tokenize(",", pos);
	if (szWaitTime.IsEmpty())
		return FALSE;
	szJump = para.Tokenize(",", pos);
	if (szJump.IsEmpty())
		return FALSE;
	int starttime, waittime, lid;
	//等待时间
	if (IsNumber(szWaitTime))
		waittime = atoi(szWaitTime);
	else
		GetValue(szWaitTime, waittime);
	if (waittime <= 0 || waittime>15000)
		waittime = 15000;
	//计算跳转地址
	if (!CalcJampAddress(szJump, lid)){
		return FALSE;
	}
	starttime = GetTickCount();
	while ((int)(GetTickCount() - starttime) <= waittime){
		if (strlen(windowinfo.data)>0 && szText.Compare("?") == 0)
			return TRUE;
		if (strstr(windowinfo.data, szText) != NULL)
			return TRUE;
		Sleep(20);
	}
	//没找到跳转
	if (szJump.Left(5).CompareNoCase("call ") == 0)
		IPStack.push(IP);
	IP = lid;
	return TRUE;
}

//点击按钮
void CDpMain::Button(CString para)
{
	if (para.CompareNoCase("OK") == 0)
		SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, WINDOW_BUTTONTYPE_OK, "");
	else if (para.CompareNoCase("CANCEL") == 0)
		SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, WINDOW_BUTTONTYPE_CANCEL, "");
	else if (para.CompareNoCase("确定") == 0)
		SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, WINDOW_BUTTONTYPE_YES, "");
	else if (para.CompareNoCase("取消") == 0)
		SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, WINDOW_BUTTONTYPE_NO, "");
	else if (para.CompareNoCase("上一页") == 0)
		SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, WINDOW_BUTTONTYPE_PREV, "");
	else if (para.CompareNoCase("下一页") == 0)
		SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, WINDOW_BUTTONTYPE_NEXT, "");
}

//取试题中的计算公式
void CDpMain::GetCalcFormula(char *msg, char *dst)
{
	char *p, *p1;
	p = strstr(msg, "(");
	p += 1;
	p1 = strstr(msg, "=");
	strncpy(dst, p, p1 - p);
}

//用于防脚本计算，计算公式的值
BOOL CDpMain::TranForumla(CString src)
{
	CString oper[] = { _T("加"), _T("+"), _T("┼"), _T("┽"), _T("┾"), _T("┿"), _T("╀"), _T("╁"), _T("╂"), _T("╃"),
		_T("╄"), _T("╅"), _T("╆"), _T("╇"), _T("╈"), _T("╉"), _T("╊"), _T("╋"), _T("十"),//0-18
		_T("一"), _T("—"), _T("━"), _T("减"), _T("-"),//19-23
		_T("*"), _T("ｘ"), _T("x"), _T("Ｘ"), _T("乘"), _T("X"), _T("＊") };//24-30
	int i, pos;
	CString szSrc, szTemp;
	szSrc = src;
	for (i = 0; i<_countof(oper); i++){
		pos = szSrc.Find(oper[i]);
		if (pos>0)
			break;
	}
	if (i<_countof(oper)){
		szTemp = szSrc.Left(pos);
		phcalc.N1 = myatoi(szTemp);
		szTemp = szSrc.Right(strlen(szSrc) - pos - strlen(oper[i]));
		phcalc.N2 = myatoi(szTemp);
		if (i <= 18){
			phcalc.oper = '+';
			phcalc.result = phcalc.N1 + phcalc.N2;
		}
		else if (i <= 23){
			phcalc.oper = '-';
			phcalc.result = phcalc.N1 - phcalc.N2;
		}
		else if (i <= 30){
			phcalc.oper = '*';
			phcalc.result = phcalc.N1*phcalc.N2;
		}
		return TRUE;
	}
	else{
		return FALSE;
	}
}

//签发Dp支票  dpcheck {数值|变量},{失败跳转}
BOOL CDpMain::SignDpCheck(CString para){
	int lid, pos, starttime, dp;
	CString szNum, szJump;
	char buf[255];
	BOOL flag = FALSE;
	pos = 0;
	szNum = para.Tokenize(",", pos);
	if (szNum.IsEmpty())
		return FALSE;
	if (IsNumber(szNum))
		dp = atoi(szNum);
	else
		GetValue(szNum, dp);

	szJump = para.Tokenize(",", pos);
	if (szJump.IsEmpty())
		return FALSE;

	//计算跳转地址
	if (!CalcJampAddress(szJump, lid)){
		return FALSE;
	}
	//如果有dp支票则返回真
	for (int i = 9; i<24; i++){
		if (strlen(iteminfo[i].name)>0 && strstr(iteminfo[i].name, "DP支票") != NULL)
			return TRUE;
	}
	UseItem("DP空头支票");
	ZeroMemory(buf, sizeof(buf));
	//等待防脚本计算窗口
	starttime = GetTickCount();
	while ((GetTickCount() - starttime) <= 2000){
		if (windowinfo.windowtype == WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT && windowinfo.buttontype == WINDOW_BUTTONTYPE_OKCANCEL){
			//开始计算
			ZeroMemory(buf, sizeof(buf));
			GetCalcFormula(windowinfo.data, buf);
			if (TranForumla(buf)){
				phcalc.type = PHINPUT;
				phcalc.selindex = -1;
				char sbuf[10] = { 0 };
				_itoa_s(phcalc.result, sbuf, 10);
				SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, WINDOW_BUTTONTYPE_OK, sbuf);
				flag = TRUE;
				break;
			}
		}
		Sleep(20);
	}
	if (!flag){
		if (szJump.Left(5).CompareNoCase("call ") == 0)
			IPStack.push(IP);
		IP = lid;
		return TRUE;
	}
	flag = FALSE;
	//等待支票数额输入窗口
	starttime = GetTickCount();
	while ((GetTickCount() - starttime) <= 2000){
		if (windowinfo.windowtype == WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT && windowinfo.buttontype == WINDOW_BUTTONTYPE_OKCANCEL && strstr(windowinfo.data, "请输入你想要兑换的DP点数") != NULL){
			//输入票额
			_itoa_s(dp, buf, 10);
			SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, WINDOW_BUTTONTYPE_OK, buf);
			flag = TRUE;
			break;
		}
		Sleep(20);
	}
	if (!flag){
		if (szJump.Left(5).CompareNoCase("call ") == 0)
			IPStack.push(IP);
		IP = lid;
		return TRUE;
	}
	flag = FALSE;
	//等待确认窗口
	starttime = GetTickCount();
	while ((GetTickCount() - starttime) <= 2000){
		if (windowinfo.windowtype == WINDOW_MESSAGETYPE_MESSAGE && windowinfo.buttontype == WINDOW_BUTTONTYPE_OKCANCEL  && strstr(windowinfo.data, "签发支票需要收取") != NULL){
			//点击确认按钮
			SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, WINDOW_BUTTONTYPE_OK, "");
			//等待签发成功信息
			deque<TALKMESSAGE *>::const_reverse_iterator v1_rIter;
			starttime = GetTickCount();
			while ((GetTickCount() - starttime) <= 3000){
				csLocalSingal.Lock();
				for (v1_rIter = talkmessage.rbegin(); v1_rIter != talkmessage.rend(); v1_rIter++){
					if ((*v1_rIter)->whotalk == -1 && strstr((*v1_rIter)->message, "DP支票制作成功！") != NULL){
						flag = TRUE;
						break;
					}
				}
				csLocalSingal.Unlock();
				if (flag)
					break;
				Sleep(50);
			}
		}
		if (flag)
			break;
		Sleep(20);
	}
	if (!flag){
		if (szJump.Left(5).CompareNoCase("call ") == 0)
			IPStack.push(IP);
		IP = lid;
	}
	return TRUE;
}

//签发会员点支票  hydcheck {数值|变量},{失败跳转}
BOOL CDpMain::SignHYDCheck(CString para){
	int lid, pos, starttime, dp;
	CString szNum, szJump;
	char buf[255];
	BOOL flag = FALSE;
	pos = 0;
	szNum = para.Tokenize(",", pos);
	if (szNum.IsEmpty())
		return FALSE;
	if (IsNumber(szNum))
		dp = atoi(szNum);
	else
		GetValue(szNum, dp);

	szJump = para.Tokenize(",", pos);
	if (szJump.IsEmpty())
		return FALSE;

	//计算跳转地址
	if (!CalcJampAddress(szJump, lid)){
		return FALSE;
	}
	//如果有会员点支票则返回真
	for (int i = 9; i<24; i++){
		if (strlen(iteminfo[i].name)>0 && strstr(iteminfo[i].name, "会员点支票") != NULL)
			return TRUE;
	}
	UseItem("空头支票");
	ZeroMemory(buf, sizeof(buf));
	//等待支票数额输入窗口
	starttime = GetTickCount();
	while ((GetTickCount() - starttime) <= 2000){
		if (windowinfo.windowtype == WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT && windowinfo.buttontype == WINDOW_BUTTONTYPE_OKCANCEL && strstr(windowinfo.data, "请输入你想要兑换的会员点数") != NULL){
			//输入票额
			_itoa_s(dp, buf, 10);
			SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, WINDOW_BUTTONTYPE_OK, buf);
			flag = TRUE;
			break;
		}
		Sleep(20);
	}
	if (!flag){
		if (szJump.Left(5).CompareNoCase("call ") == 0)
			IPStack.push(IP);
		IP = lid;
		return TRUE;
	}
	flag = FALSE;
	//等待确认窗口
	starttime = GetTickCount();
	while ((GetTickCount() - starttime) <= 2000){
		if (windowinfo.windowtype == WINDOW_MESSAGETYPE_MESSAGE && windowinfo.buttontype == WINDOW_BUTTONTYPE_OKCANCEL  && strstr(windowinfo.data, "签发支票需要收取") != NULL){
			//点击确认按钮
			SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, WINDOW_BUTTONTYPE_OK, "");
			//等待签发成功信息
			deque<TALKMESSAGE *>::const_reverse_iterator v1_rIter;
			starttime = GetTickCount();
			while ((GetTickCount() - starttime) <= 3000){
				csLocalSingal.Lock();
				for (v1_rIter = talkmessage.rbegin(); v1_rIter != talkmessage.rend(); v1_rIter++){
					if ((*v1_rIter)->whotalk == -1 && strstr((*v1_rIter)->message, "支票制作成功！") != NULL){
						flag = TRUE;
						break;
					}
				}
				csLocalSingal.Unlock();
				if (flag)
					break;
				Sleep(50);
			}
		}
		if (flag)
			break;
		Sleep(20);
	}
	if (!flag){
		if (szJump.Left(5).CompareNoCase("call ") == 0)
			IPStack.push(IP);
		IP = lid;
	}
	return TRUE;
}

//petmail {名片编号(1..80)|人物名称},{道具(1|3)|名称}
BOOL CDpMain::SendPetMail(CString para)
{
	CString szCardindex, szItem;
	int i, pos, cardindex, item = 0, mailpet = -1;
	pos = 0;
	szCardindex = para.Tokenize(",", pos);
	if (szCardindex.IsEmpty())
		return FALSE;
	if (IsNumber(szCardindex))
		cardindex = atoi(szCardindex);
	else{
		for (i = 0; i<EMAIL_NUM; i++){
			if (strstr(mailcontactlist[i].charname, szCardindex) != NULL){
				break;
			}
		}
		if (i<EMAIL_NUM)
			cardindex = i + 1;
		else
			return TRUE;
	}
	szItem = para.Tokenize(",", pos);
	if (szItem.IsEmpty())
		return FALSE;

	if (IsNumber(szItem))
		item = atoi(szItem);
	mailpet = 0;
	//找出当前mailpet
	for (i = 0; i<5; i++){
		if (petdetail[i].state == 4){
			mailpet = i;
			break;
		}
	}

	if (item>0 && item<24){
		SendPetMail(cardindex - 1, mailpet, item, "dp", 0);
	}
	else{
		for (i = 9; i<24; i++){
			if (strlen(iteminfo[i].name)>0 && (strstr(iteminfo[i].name, szItem) != NULL || strstr(iteminfo[i].comment, szItem) != NULL)){
				item = i;
				break;
			}
		}
		if (item>0 && item<24)
			SendPetMail(cardindex - 1, mailpet, item, "dp", 0);
	}
	return TRUE;
}

//设置宠物状态 petstate {1-5},{平时|等待|邮件}
BOOL CDpMain::SetPetState(CString para)
{
	CString szPet, szState;
	int pos, pet;
	pos = 0;
	szPet = para.Tokenize(",", pos);
	if (szPet.IsEmpty())
		return FALSE;
	szState = para.Tokenize(",", pos);
	if (szState.IsEmpty())
		return FALSE;
	if (IsNumber(szPet))
		pet = atoi(szPet);
	else
		return FALSE;
	if (pet<1 || pet>5)
		return FALSE;
	if (szState.Compare("平时") == 0){
		SendPetState(pet - 1, 0);
	}
	else if (szState.Compare("等待") == 0){
		SendPetState(pet - 1, 1);
	}
	else if (szState.Compare("邮件") == 0){
		SendPetState(pet - 1, 4);
	}
	return TRUE;
}

//ifmap num, {跳转符号}           判断地图
BOOL CDpMain::IfMap(CString para)
{
	int pos = 0, mapid, lid;
	CString szMapid, szJump;
	szMapid = para.Tokenize(",", pos);
	if (szMapid.IsEmpty())
		return FALSE;
	szJump = para.Tokenize(",", pos);
	if (szJump.IsEmpty())
		return FALSE;
	if (IsNumber(szMapid))
		mapid = atoi(szMapid);
	else
		return FALSE;
	//计算跳转地址
	if (!CalcJampAddress(szJump, lid)){
		return FALSE;
	}
	if (mapinfo.floor == mapid){
		if (szJump.Left(5).CompareNoCase("call ") == 0)
			IPStack.push(IP);
		IP = lid;
	}
	return TRUE;
}

//ifpos x,y, {跳转符号}判断人物坐标
BOOL CDpMain::IfPos(CString para)
{
	int pos = 0, x, y, lid;
	CString szX, szY, szJump;
	szX = para.Tokenize(",", pos);
	if (szX.IsEmpty())
		return FALSE;
	szY = para.Tokenize(",", pos);
	if (szY.IsEmpty())
		return FALSE;
	szJump = para.Tokenize(",", pos);
	if (szJump.IsEmpty())
		return FALSE;
	if (IsNumber(szX))
		x = atoi(szX);
	else
		return FALSE;
	if (IsNumber(szY))
		y = atoi(szY);
	else
		return FALSE;
	//计算跳转地址
	if (!CalcJampAddress(szJump, lid)){
		return FALSE;
	}
	if (charotherinfo.x == x && charotherinfo.y == y){
		if (szJump.Left(5).CompareNoCase("call ") == 0)
			IPStack.push(IP);
		IP = lid;
	}
	return TRUE;
}

//ifstone {=|!=|<|<=|>=|>},stone,{跳转符号} 判断石头数值
BOOL CDpMain::IfStone(CString para)
{
	int pos = 0, stone, lid;
	CString szTest, szStone, szJump;
	szTest = para.Tokenize(",", pos);
	if (szTest.IsEmpty())
		return FALSE;
	szStone = para.Tokenize(",", pos);
	if (szStone.IsEmpty())
		return FALSE;
	szJump = para.Tokenize(",", pos);
	if (szJump.IsEmpty())
		return FALSE;
	if (IsNumber(szStone))
		stone = atoi(szStone);
	else
		return FALSE;
	//计算跳转地址
	if (!CalcJampAddress(szJump, lid)){
		return FALSE;
	}
	if (szTest.Compare("=") == 0){
		if (chardetail.gold == stone){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	else if (szTest.Compare("!=") == 0){
		if (chardetail.gold != stone){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	else if (szTest.Compare("<") == 0){
		if (chardetail.gold<stone){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	else if (szTest.Compare("<=") == 0){
		if (chardetail.gold <= stone){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	else if (szTest.Compare(">") == 0){
		if (chardetail.gold>stone){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	else if (szTest.Compare(">=") == 0){
		if (chardetail.gold >= stone){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	return TRUE;
}

//iffight {=|!=|<|<=|>=|>},战斗回合数,{跳转符号}判断战斗回合
BOOL CDpMain::IfFight(CString para)
{
	int pos = 0, round, lid;
	CString szTest, szRound, szJump;
	szTest = para.Tokenize(",", pos);
	if (szTest.IsEmpty())
		return FALSE;
	szRound = para.Tokenize(",", pos);
	if (szRound.IsEmpty())
		return FALSE;
	szJump = para.Tokenize(",", pos);
	if (szJump.IsEmpty())
		return FALSE;
	if (IsNumber(szRound))
		round = atoi(szRound);
	else
		return FALSE;
	//计算跳转地址
	if (!CalcJampAddress(szJump, lid)){
		return FALSE;
	}
	if (szTest.Compare("=") == 0){
		if (charotherinfo.state == 1 && charotherinfo.round == round){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	else if (szTest.Compare("!=") == 0){
		if (charotherinfo.state == 1 && charotherinfo.round != round){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	else if (szTest.Compare("<") == 0){
		if (charotherinfo.state == 1 && charotherinfo.round<round){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	else if (szTest.Compare("<=") == 0){
		if (charotherinfo.state == 1 && charotherinfo.round <= round){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	else if (szTest.Compare(">") == 0){
		if (charotherinfo.state == 1 && charotherinfo.round>round){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	else if (szTest.Compare(">=") == 0){
		if (charotherinfo.state == 1 && charotherinfo.round >= round){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	return TRUE;
}

//ifdlg text,{跳转符号}  判断对话框。 跟waitdlg ?,0 后面使用
BOOL CDpMain::IfDlg(CString para)
{
	int pos = 0, lid;
	CString szText, szJump;
	szText = para.Tokenize(",", pos);
	if (szText.IsEmpty())
		return FALSE;
	szJump = para.Tokenize(",", pos);
	if (szJump.IsEmpty())
		return FALSE;

	//计算跳转地址
	if (!CalcJampAddress(szJump, lid)){
		return FALSE;
	}
	if (strstr(windowinfo.data, szText) != NULL){
		if (szJump.Left(5).CompareNoCase("call ") == 0)
			IPStack.push(IP);
		IP = lid;
	}
	return TRUE;
}

//ifsay text,{系统|自己|其它},{跳转符号}   判断说话
BOOL CDpMain::IfSay(CString para)
{
	int pos = 0, lid;
	CString szText, szWho, szJump;
	szText = para.Tokenize(",", pos);
	if (szText.IsEmpty())
		return FALSE;
	szWho = para.Tokenize(",", pos);
	if (szWho.IsEmpty())
		return FALSE;
	szJump = para.Tokenize(",", pos);
	if (szJump.IsEmpty())
		return FALSE;

	//计算跳转地址
	if (!CalcJampAddress(szJump, lid)){
		return FALSE;
	}
	deque<TALKMESSAGE *>::const_reverse_iterator v1_rIter;
	if (szWho.Compare("系统") == 0){
		csLocalSingal.Lock();
		for (v1_rIter = talkmessage.rbegin(); v1_rIter != talkmessage.rend(); v1_rIter++){
			if ((*v1_rIter)->whotalk == -1 && strstr((*v1_rIter)->message, szText) != NULL){
				if (szJump.Left(5).CompareNoCase("call ") == 0)
					IPStack.push(IP);
				IP = lid;
				break;
			}
		}
		csLocalSingal.Unlock();
	}
	else if (szWho.Compare("自己") == 0){
		csLocalSingal.Lock();
		for (v1_rIter = talkmessage.rbegin(); v1_rIter != talkmessage.rend(); v1_rIter++){
			if ((*v1_rIter)->whotalk == charotherinfo.charid && strstr((*v1_rIter)->message, szText) != NULL){
				if (szJump.Left(5).CompareNoCase("call ") == 0)
					IPStack.push(IP);
				IP = lid;
				break;
			}
		}
		csLocalSingal.Unlock();
	}
	else{
		csLocalSingal.Lock();
		for (v1_rIter = talkmessage.rbegin(); v1_rIter != talkmessage.rend(); v1_rIter++){
			if (((*v1_rIter)->whotalk != -1 && (*v1_rIter)->whotalk != charotherinfo.charid) && strstr((*v1_rIter)->message, szText) != NULL){
				if (szJump.Left(5).CompareNoCase("call ") == 0)
					IPStack.push(IP);
				IP = lid;
				break;
			}
		}
		csLocalSingal.Unlock();
	}
	return TRUE;
}

//检测装备，ifitem {101-109},{=|!=},{名称|空},{跳转符号}
BOOL CDpMain::IfEquipment(CString szItem, CString szTest, CString szPara, CString szJump)
{
	int item, lid;
	item = atoi(szItem);
	item -= 101;
	//计算跳转地址
	if (!CalcJampAddress(szJump, lid)){
		return FALSE;
	}

	if (szPara.CompareNoCase("空") == 0){
		if (szTest.Compare("=") == 0){
			if (strlen(iteminfo[item].name) == 0){
				if (szJump.Left(5).CompareNoCase("call ") == 0)
					IPStack.push(IP);
				IP = lid;
			}
		}
		else if (szTest.Compare("!=") == 0){
			if (strlen(iteminfo[item].name)>0){
				if (szJump.Left(5).CompareNoCase("call ") == 0)
					IPStack.push(IP);
				IP = lid;
			}
		}
	}
	else{
		if (szTest.Compare("=") == 0){
			if (strlen(iteminfo[item].name)>0 && (strstr(iteminfo[item].name, szPara) != NULL || strstr(iteminfo[item].comment, szPara) != NULL)){
				if (szJump.Left(5).CompareNoCase("call ") == 0)
					IPStack.push(IP);
				IP = lid;
			}
		}
		else if (szTest.Compare("!=") == 0){
			if (strstr(iteminfo[item].name, szPara) == NULL && strstr(iteminfo[item].comment, szPara) == NULL){
				if (szJump.Left(5).CompareNoCase("call ") == 0)
					IPStack.push(IP);
				IP = lid;
			}
		}
	}
	return TRUE;
}

//ifitem {名称},{=|!=|<|<=|>=|>},数量,{跳转符号}
BOOL CDpMain::IfItemName(CString szItem, CString szTest, CString szPara, CString szJump)
{
	int num, lid, n = 0;
	if (IsNumber(szPara))
		num = atoi(szPara);
	else
		return FALSE;
	//计算跳转地址
	if (!CalcJampAddress(szJump, lid)){
		return FALSE;
	}
	for (int i = 9; i<24; i++){
		if (strlen(iteminfo[i].name)>0 && (strstr(iteminfo[i].name, szItem) != NULL || strstr(iteminfo[i].comment, szItem) != NULL)){
			n += iteminfo[i].pilenums;
		}
	}
	if (szTest.Compare("=") == 0){
		if (n == num){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	else if (szTest.Compare("!=") == 0){
		if (n != num){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	else if (szTest.Compare("<") == 0){
		if (n<num){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	else if (szTest.Compare("<=") == 0){
		if (n <= num){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	else if (szTest.Compare(">") == 0){
		if (n>num){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	else if (szTest.Compare(">=") == 0){
		if (n >= num){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	return TRUE;
}

//ifitem {空位},{=|!=|<|<=|>=|>},数量,{跳转符号}
BOOL CDpMain::IfItemPosEmpty(CString szItem, CString szTest, CString szPara, CString szJump)
{
	int num, lid, n = 0;
	if (IsNumber(szPara))
		num = atoi(szPara);
	else
		return FALSE;
	//计算跳转地址
	if (!CalcJampAddress(szJump, lid)){
		return FALSE;
	}
	if (szTest.Compare("=") == 0){
		for (int i = 9; i<24; i++){
			if (strlen(iteminfo[i].name) == 0){
				n++;
			}
		}
		if (n == num){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	else if (szTest.Compare("!=") == 0){
		for (int i = 9; i<24; i++){
			if (strlen(iteminfo[i].name) == 0){
				n++;
			}
		}
		if (n != num){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	else if (szTest.Compare("<") == 0){
		for (int i = 9; i<24; i++){
			if (strlen(iteminfo[i].name) == 0){
				n++;
			}
		}
		if (n<num){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	else if (szTest.Compare("<=") == 0){
		for (int i = 9; i<24; i++){
			if (strlen(iteminfo[i].name) == 0){
				n++;
			}
		}
		if (n <= num){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	else if (szTest.Compare(">") == 0){
		for (int i = 9; i<24; i++){
			if (strlen(iteminfo[i].name) == 0){
				n++;
			}
		}
		if (n>num){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	else if (szTest.Compare(">=") == 0){
		for (int i = 9; i<24; i++){
			if (strlen(iteminfo[i].name) == 0){
				n++;
			}
		}
		if (n >= num){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	return TRUE;
}

//ifitem {1|2|4-5},{=|!=},{名称},{跳转符号}
BOOL CDpMain::IfItemIndexName(CString szItem, CString szTest, CString szPara, CString szJump)
{
	CString szStart, szEnd;
	int start, end = 0, pos = 0, lid;
	szStart = szItem.Tokenize("-", pos);
	start = atoi(szStart);
	if (pos<szItem.GetLength()){
		szEnd = szItem.Tokenize("-", pos);
		end = atoi(szEnd);
	}
	if (end == 0)
		end = start;
	if (start<1 || start>15 || end<1 || end>15)
		return FALSE;
	//计算跳转地址
	if (!CalcJampAddress(szJump, lid)){
		return FALSE;
	}
	int i;
	if (szTest.Compare("=") == 0){
		for (i = start + 8; i <= 8 + end; i++){
			if (strstr(iteminfo[i].name, szPara) == NULL && strstr(iteminfo[i].comment, szPara) == NULL){
				break;
			}
		}
		if (i>8 + end){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	else if (szTest.Compare("!=") == 0){
		for (i = start + 8; i <= 8 + end; i++){
			if (strstr(iteminfo[i].name, szPara) != NULL || strstr(iteminfo[i].comment, szPara) != NULL){
				break;
			}
		}
		if (i>8 + end){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	return TRUE;
}

//ifitem {1|2|4-5},{=|!=},{空位},{跳转符号}
BOOL CDpMain::IfItemIndexPosEmpty(CString szItem, CString szTest, CString szPara, CString szJump)
{
	CString szStart, szEnd;
	int start, end = 0, pos = 0, lid;
	szStart = szItem.Tokenize("-", pos);
	start = atoi(szStart);
	if (pos<szItem.GetLength()){
		szEnd = szItem.Tokenize("-", pos);
		end = atoi(szEnd);
	}
	if (end == 0)
		end = start;
	if (start<1 || start>15 || end<1 || end>15)
		return FALSE;
	//计算跳转地址
	if (!CalcJampAddress(szJump, lid)){
		return FALSE;
	}
	int i;
	if (szTest.Compare("=") == 0){
		for (i = start + 8; i <= 8 + end; i++){
			if (strlen(iteminfo[i].name)>0){
				break;
			}
		}
		if (i>8 + end){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	else if (szTest.Compare("!=") == 0){
		for (i = start + 8; i <= 8 + end; i++){
			if (strlen(iteminfo[i].name) == 0){
				break;
			}
		}
		if (i>8 + end){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	return TRUE;
}

//ifpet {名称},{=|!=|<|<=|>=|>},数量,{跳转符号}
BOOL CDpMain::IfPetName(CString szPet, CString szTest, CString szPara, CString szJump)
{
	int num, lid, n = 0;
	if (IsNumber(szPara))
		num = atoi(szPara);
	else
		return FALSE;
	//计算跳转地址
	if (!CalcJampAddress(szJump, lid)){
		return FALSE;
	}
	if (szTest.Compare("=") == 0){
		for (int i = 0; i<5; i++){
			if (strstr(petdetail[i].oldname, szPet) != NULL || strstr(petdetail[i].newname, szPet) != NULL){
				n += 1;
			}
		}
		if (n == num){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	else if (szTest.Compare("!=") == 0){
		for (int i = 0; i<5; i++){
			if (strstr(petdetail[i].oldname, szPet) != NULL || strstr(petdetail[i].newname, szPet) != NULL){
				n += 1;
			}
		}
		if (n != num){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	else if (szTest.Compare("<") == 0){
		for (int i = 0; i<5; i++){
			if (strstr(petdetail[i].oldname, szPet) != NULL || strstr(petdetail[i].newname, szPet) != NULL){
				n += 1;
			}
		}
		if (n<num){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	else if (szTest.Compare("<=") == 0){
		for (int i = 0; i<5; i++){
			if (strstr(petdetail[i].oldname, szPet) != NULL || strstr(petdetail[i].newname, szPet) != NULL){
				n += 1;
			}
		}
		if (n <= num){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	else if (szTest.Compare(">") == 0){
		for (int i = 0; i<5; i++){
			if (strstr(petdetail[i].oldname, szPet) != NULL || strstr(petdetail[i].newname, szPet) != NULL){
				n += 1;
			}
		}
		if (n>num){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	else if (szTest.Compare(">=") == 0){
		for (int i = 0; i<5; i++){
			if (strstr(petdetail[i].oldname, szPet) != NULL || strstr(petdetail[i].newname, szPet) != NULL){
				n += 1;
			}
		}
		if (n >= num){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	return TRUE;
}

//ifpet {空位},{=|!=|<|<=|>=|>},数量,{跳转符号}
BOOL CDpMain::IfPetPosEmpty(CString szPet, CString szTest, CString szPara, CString szJump)
{
	int num, lid, n = 0;
	if (IsNumber(szPara))
		num = atoi(szPara);
	else
		return FALSE;
	//计算跳转地址
	if (!CalcJampAddress(szJump, lid)){
		return FALSE;
	}
	if (szTest.Compare("=") == 0){
		for (int i = 0; i<5; i++){
			if (petdetail[i].islive == 0){
				n++;
			}
		}
		if (n == num){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	else if (szTest.Compare("!=") == 0){
		for (int i = 0; i<5; i++){
			if (petdetail[i].islive == 0){
				n++;
			}
		}
		if (n != num){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	else if (szTest.Compare("<") == 0){
		for (int i = 0; i<5; i++){
			if (petdetail[i].islive == 0){
				n++;
			}
		}
		if (n<num){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	else if (szTest.Compare("<=") == 0){
		for (int i = 0; i<5; i++){
			if (petdetail[i].islive == 0){
				n++;
			}
		}
		if (n <= num){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	else if (szTest.Compare(">") == 0){
		for (int i = 0; i<5; i++){
			if (petdetail[i].islive == 0){
				n++;
			}
		}
		if (n>num){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	else if (szTest.Compare(">=") == 0){
		for (int i = 0; i<5; i++){
			if (petdetail[i].islive == 0){
				n++;
			}
		}
		if (n >= num){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	return TRUE;
}

//ifpet {1|2|4-5},{=|!=},{名称},{跳转符号}
BOOL CDpMain::IfPetIndexName(CString szPet, CString szTest, CString szPara, CString szJump)
{
	CString szStart, szEnd;
	int start, end = 0, pos = 0, lid;
	szStart = szPet.Tokenize("-", pos);
	start = atoi(szStart);
	if (pos<szPet.GetLength()){
		szEnd = szPet.Tokenize("-", pos);
		end = atoi(szEnd);
	}
	if (end == 0)
		end = start;
	if (start<1 || start>15 || end<1 || end>15)
		return FALSE;
	//计算跳转地址
	if (!CalcJampAddress(szJump, lid)){
		return FALSE;
	}
	int i;
	if (szTest.Compare("=") == 0){
		for (i = start - 1; i<end; i++){
			if (strstr(petdetail[i].oldname, szPara) == NULL && strstr(petdetail[i].newname, szPara) == NULL){
				break;
			}
		}
		if (i >= end){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	else if (szTest.Compare("!=") == 0){
		for (i = start - 1; i<end; i++){
			if (strstr(petdetail[i].oldname, szPara) != NULL || strstr(petdetail[i].newname, szPara) != NULL){
				break;
			}
		}
		if (i >= end){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	return TRUE;
}

//ifpet {1|2|4-5},{=|!=},{空位},{跳转符号}
BOOL CDpMain::IfPetIndexPosEmpty(CString szPet, CString szTest, CString szPara, CString szJump)
{
	CString szStart, szEnd;
	int start, end = 0, pos = 0, lid;
	szStart = szPet.Tokenize("-", pos);
	start = atoi(szStart);
	if (pos<szPet.GetLength()){
		szEnd = szPet.Tokenize("-", pos);
		end = atoi(szEnd);
	}
	if (end == 0)
		end = start;
	if (start<1 || start>15 || end<1 || end>15)
		return FALSE;
	//计算跳转地址
	if (!CalcJampAddress(szJump, lid)){
		return FALSE;
	}
	int i;
	if (szTest.Compare("=") == 0){
		for (i = start - 1; i<end; i++){
			if (petdetail[i].islive == 1){
				break;
			}
		}
		if (i >= end){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	else if (szTest.Compare("!=") == 0){
		for (i = start - 1; i<end; i++){
			if (petdetail[i].islive == 0){
				break;
			}
		}
		if (i >= end){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	return TRUE;
}
//check {人物},{LV|EXP|HP|HP%|MP|MP%|DP|攻击|防御|敏捷|魅力|体力|腕力|耐力|速度|转生|升级点数|石币},{=|!=|<|<=|>=|>},数值,{跳转符号}


#pragma region check函数




BOOL CDpMain::CheckChar(CString szPara)
{
	CString szItem, szTest, szNum, szJump;
	int pos = 0, num, jump;
	szItem = szPara.Tokenize(",", pos);
	szItem = szPara.Tokenize(",", pos);
	if (szItem.IsEmpty())
		return FALSE;
	szTest = szPara.Tokenize(",", pos);
	if (szTest.IsEmpty())
		return FALSE;
	szNum = szPara.Tokenize(",", pos);
	if (szNum.IsEmpty())
		return FALSE;
	szJump = szPara.Tokenize(",", pos);
	if (szJump.IsEmpty())
		return FALSE;
	if (IsNumber(szNum))
		num = atoi(szNum);
	else
		return FALSE;
	BOOL bIsCall = FALSE;
	if (szJump.Left(5).CompareNoCase("call ") == 0)
		bIsCall = TRUE;
	//计算跳转地址
	if (!CalcJampAddress(szJump, jump)){
		return FALSE;
	}
	if (szItem.CompareNoCase("LV") == 0)
		CheckCharLV(szTest, num, jump, bIsCall);
	else if (szItem.CompareNoCase("EXP") == 0)
		CheckCharEXP(szTest, num, jump, bIsCall);
	else if (szItem.CompareNoCase("HP") == 0)
		CheckCharHP(szTest, num, jump, bIsCall);
	else if (szItem.CompareNoCase("HP%") == 0)
		CheckCharHPP(szTest, num, jump, bIsCall);
	else if (szItem.CompareNoCase("MP") == 0)
		CheckCharMP(szTest, num, jump, bIsCall);
	else if (szItem.CompareNoCase("MP%") == 0)
		CheckCharMPP(szTest, num, jump, bIsCall);
	else if (szItem.CompareNoCase("DP") == 0)
		CheckCharDP(szTest, num, jump, bIsCall);
	else if (szItem.CompareNoCase("攻击") == 0)
		CheckCharAttack(szTest, num, jump, bIsCall);
	else if (szItem.CompareNoCase("防御") == 0)
		CheckCharDefence(szTest, num, jump, bIsCall);
	else if (szItem.CompareNoCase("敏捷") == 0)
		CheckCharQuick(szTest, num, jump, bIsCall);
	else if (szItem.CompareNoCase("魅力") == 0)
		CheckCharCharm(szTest, num, jump, bIsCall);
	else if (szItem.CompareNoCase("体力") == 0)
		CheckCharVital(szTest, num, jump, bIsCall);
	else if (szItem.CompareNoCase("腕力") == 0)
		CheckCharStr(szTest, num, jump, bIsCall);
	else if (szItem.CompareNoCase("耐力") == 0)
		CheckCharTough(szTest, num, jump, bIsCall);
	else if (szItem.CompareNoCase("速度") == 0)
		CheckCharDex(szTest, num, jump, bIsCall);
	else if (szItem.CompareNoCase("转生") == 0)
		CheckCharTransmigration(szTest, num, jump, bIsCall);
	else if (szItem.CompareNoCase("升级点数") == 0)
		CheckCharUpLevelPoint(szTest, num, jump, bIsCall);
	else if (szItem.CompareNoCase("石币") == 0)
		CheckCharGold(szTest, num, jump, bIsCall);
	return TRUE;
}

BOOL CDpMain::CheckCharLV(CString szTest, int num, int jump, BOOL iscall)
{
	if (szTest.Compare("=") == 0){
		if (chardetail.level == num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("!=") == 0){
		if (chardetail.level != num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">=") == 0){
		if (chardetail.level >= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">") == 0){
		if (chardetail.level>num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<=") == 0){
		if (chardetail.level <= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<") == 0){
		if (chardetail.level<num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	return TRUE;
}

BOOL CDpMain::CheckCharEXP(CString szTest, int num, int jump, BOOL iscall)
{
	if (szTest.Compare("=") == 0){
		if (chardetail.exp == num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("!=") == 0){
		if (chardetail.exp != num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">=") == 0){
		if (chardetail.exp >= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">") == 0){
		if (chardetail.exp>num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<=") == 0){
		if (chardetail.exp <= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<") == 0){
		if (chardetail.exp<num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	return TRUE;
}

BOOL CDpMain::CheckCharHP(CString szTest, int num, int jump, BOOL iscall)
{
	if (szTest.Compare("=") == 0){
		if (chardetail.hp == num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("!=") == 0){
		if (chardetail.hp != num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">=") == 0){
		if (chardetail.hp >= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">") == 0){
		if (chardetail.hp>num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<=") == 0){
		if (chardetail.hp <= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<") == 0){
		if (chardetail.hp<num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	return TRUE;
}

BOOL CDpMain::CheckCharHPP(CString szTest, int num, int jump, BOOL iscall)
{
	if (szTest.Compare("=") == 0){
		if (((double)chardetail.hp / chardetail.maxhp) * 100 == num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("!=") == 0){
		if (((double)chardetail.hp / chardetail.maxhp) * 100 != num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">=") == 0){
		if (((double)chardetail.hp / chardetail.maxhp) * 100 >= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">") == 0){
		if (((double)chardetail.hp / chardetail.maxhp) * 100>num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<=") == 0){
		if (((double)chardetail.hp / chardetail.maxhp) * 100 <= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<") == 0){
		if (((double)chardetail.hp / chardetail.maxhp) * 100<num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	return TRUE;
}

BOOL CDpMain::CheckCharMP(CString szTest, int num, int jump, BOOL iscall)
{
	if (szTest.Compare("=") == 0){
		if (chardetail.mp == num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("!=") == 0){
		if (chardetail.mp != num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">=") == 0){
		if (chardetail.mp >= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">") == 0){
		if (chardetail.mp>num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<=") == 0){
		if (chardetail.mp <= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<") == 0){
		if (chardetail.mp<num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	return TRUE;
}

BOOL CDpMain::CheckCharMPP(CString szTest, int num, int jump, BOOL iscall)
{
	if (szTest.Compare("=") == 0){
		if (((double)chardetail.mp / chardetail.maxmp) * 100 == num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("!=") == 0){
		if (((double)chardetail.mp / chardetail.maxmp) * 100 != num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">=") == 0){
		if (((double)chardetail.mp / chardetail.maxmp) * 100 >= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">") == 0){
		if (((double)chardetail.mp / chardetail.maxmp) * 100>num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<=") == 0){
		if (((double)chardetail.mp / chardetail.maxmp) * 100 <= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<") == 0){
		if (((double)chardetail.mp / chardetail.maxmp) * 100<num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	return TRUE;
}

BOOL CDpMain::CheckCharDP(CString szTest, int num, int jump, BOOL iscall)
{
	if (szTest.Compare("=") == 0){
		if (chardetail.dp == num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("!=") == 0){
		if (chardetail.dp != num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">=") == 0){
		if (chardetail.dp >= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">") == 0){
		if (chardetail.dp>num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<=") == 0){
		if (chardetail.dp <= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<") == 0){
		if (chardetail.dp<num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	return TRUE;
}

BOOL CDpMain::CheckCharAttack(CString szTest, int num, int jump, BOOL iscall)
{
	if (szTest.Compare("=") == 0){
		if (chardetail.attack == num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("!=") == 0){
		if (chardetail.attack != num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">=") == 0){
		if (chardetail.attack >= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">") == 0){
		if (chardetail.attack>num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<=") == 0){
		if (chardetail.attack <= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<") == 0){
		if (chardetail.attack<num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	return TRUE;
}

BOOL CDpMain::CheckCharDefence(CString szTest, int num, int jump, BOOL iscall)
{
	if (szTest.Compare("=") == 0){
		if (chardetail.defence == num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("!=") == 0){
		if (chardetail.defence != num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">=") == 0){
		if (chardetail.defence >= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">") == 0){
		if (chardetail.defence>num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<=") == 0){
		if (chardetail.defence <= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<") == 0){
		if (chardetail.defence<num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	return TRUE;
}

BOOL CDpMain::CheckCharQuick(CString szTest, int num, int jump, BOOL iscall)
{
	if (szTest.Compare("=") == 0){
		if (chardetail.fQuick == num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("!=") == 0){
		if (chardetail.fQuick != num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">=") == 0){
		if (chardetail.fQuick >= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">") == 0){
		if (chardetail.fQuick>num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<=") == 0){
		if (chardetail.fQuick <= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<") == 0){
		if (chardetail.fQuick<num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	return TRUE;
}

BOOL CDpMain::CheckCharCharm(CString szTest, int num, int jump, BOOL iscall)
{
	if (szTest.Compare("=") == 0){
		if (chardetail.fCharm == num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("!=") == 0){
		if (chardetail.fCharm != num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">=") == 0){
		if (chardetail.fCharm >= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">") == 0){
		if (chardetail.fCharm>num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<=") == 0){
		if (chardetail.fCharm <= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<") == 0){
		if (chardetail.fCharm<num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	return TRUE;
}

BOOL CDpMain::CheckCharVital(CString szTest, int num, int jump, BOOL iscall)
{
	if (szTest.Compare("=") == 0){
		if (chardetail.vital == num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("!=") == 0){
		if (chardetail.vital != num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">=") == 0){
		if (chardetail.vital >= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">") == 0){
		if (chardetail.vital>num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<=") == 0){
		if (chardetail.vital <= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<") == 0){
		if (chardetail.vital<num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	return TRUE;
}

BOOL CDpMain::CheckCharStr(CString szTest, int num, int jump, BOOL iscall)
{
	if (szTest.Compare("=") == 0){
		if (chardetail.str == num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("!=") == 0){
		if (chardetail.str != num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">=") == 0){
		if (chardetail.str >= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">") == 0){
		if (chardetail.str>num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<=") == 0){
		if (chardetail.str <= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<") == 0){
		if (chardetail.str<num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	return TRUE;
}

BOOL CDpMain::CheckCharTough(CString szTest, int num, int jump, BOOL iscall)
{
	if (szTest.Compare("=") == 0){
		if (chardetail.tough == num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("!=") == 0){
		if (chardetail.tough != num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">=") == 0){
		if (chardetail.tough >= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">") == 0){
		if (chardetail.tough>num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<=") == 0){
		if (chardetail.tough <= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<") == 0){
		if (chardetail.tough<num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	return TRUE;
}

BOOL CDpMain::CheckCharDex(CString szTest, int num, int jump, BOOL iscall)
{
	if (szTest.Compare("=") == 0){
		if (chardetail.dex == num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("!=") == 0){
		if (chardetail.dex != num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">=") == 0){
		if (chardetail.dex >= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">") == 0){
		if (chardetail.dex>num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<=") == 0){
		if (chardetail.dex <= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<") == 0){
		if (chardetail.dex<num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	return TRUE;
}

BOOL CDpMain::CheckCharTransmigration(CString szTest, int num, int jump, BOOL iscall)
{
	if (szTest.Compare("=") == 0){
		if (chardetail.transmigration == num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("!=") == 0){
		if (chardetail.transmigration != num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">=") == 0){
		if (chardetail.transmigration >= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">") == 0){
		if (chardetail.transmigration>num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<=") == 0){
		if (chardetail.transmigration <= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<") == 0){
		if (chardetail.transmigration<num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	return TRUE;
}

BOOL CDpMain::CheckCharUpLevelPoint(CString szTest, int num, int jump, BOOL iscall)
{
	if (szTest.Compare("=") == 0){
		if (chardetail.uplevelpoint == num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("!=") == 0){
		if (chardetail.uplevelpoint != num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">=") == 0){
		if (chardetail.uplevelpoint >= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">") == 0){
		if (chardetail.uplevelpoint>num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<=") == 0){
		if (chardetail.uplevelpoint <= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<") == 0){
		if (chardetail.uplevelpoint<num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	return TRUE;
}

BOOL CDpMain::CheckCharGold(CString szTest, int num, int jump, BOOL iscall)
{
	if (szTest.Compare("=") == 0){
		if (chardetail.gold == num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("!=") == 0){
		if (chardetail.gold != num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">=") == 0){
		if (chardetail.gold >= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">") == 0){
		if (chardetail.gold>num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<=") == 0){
		if (chardetail.gold <= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<") == 0){
		if (chardetail.gold<num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	return TRUE;
}
//check {宠物},{1-5},{职位|LV|EXP|HP|HP%|攻击|防御|敏捷|忠诚|转生},{=|!=|<|<=|>=|>},数值,{跳转符号}

BOOL CDpMain::CheckPet(CString szPara)
{
	CString szPetID, szItem, szTest, szNum, szJump;
	int pos = 0, num, jump, petid;
	szPetID = szPara.Tokenize(",", pos);
	szPetID = szPara.Tokenize(",", pos);
	if (szPetID.IsEmpty())
		return FALSE;
	szItem = szPara.Tokenize(",", pos);
	if (szItem.IsEmpty())
		return FALSE;
	szTest = szPara.Tokenize(",", pos);
	if (szTest.IsEmpty())
		return FALSE;
	szNum = szPara.Tokenize(",", pos);
	if (szNum.IsEmpty())
		return FALSE;
	szJump = szPara.Tokenize(",", pos);
	if (szJump.IsEmpty())
		return FALSE;
	if (IsNumber(szPetID))
		petid = atoi(szPetID);
	else
		return FALSE;
	if (petid<1 || petid>5)
		return FALSE;
	if (IsNumber(szNum))
		num = atoi(szNum);
	else
		return FALSE;
	BOOL bIsCall = FALSE;
	if (szJump.Left(5).CompareNoCase("call ") == 0)
		bIsCall = TRUE;
	//计算跳转地址
	if (!CalcJampAddress(szJump, jump)){
		return FALSE;
	}
	if (szItem.CompareNoCase("职位") == 0)
		CheckPetJob(petid, szTest, num, jump, bIsCall);
	else if (szItem.CompareNoCase("LV") == 0)
		CheckPetLV(petid, szTest, num, jump, bIsCall);
	else if (szItem.CompareNoCase("EXP") == 0)
		CheckPetEXP(petid, szTest, num, jump, bIsCall);
	else if (szItem.CompareNoCase("HP") == 0)
		CheckPetHP(petid, szTest, num, jump, bIsCall);
	else if (szItem.CompareNoCase("HP%") == 0)
		CheckPetHPP(petid, szTest, num, jump, bIsCall);
	else if (szItem.CompareNoCase("攻击") == 0)
		CheckPetAttack(petid, szTest, num, jump, bIsCall);
	else if (szItem.CompareNoCase("防御") == 0)
		CheckPetDefence(petid, szTest, num, jump, bIsCall);
	else if (szItem.CompareNoCase("敏捷") == 0)
		CheckPetQuick(petid, szTest, num, jump, bIsCall);
	else if (szItem.CompareNoCase("忠诚") == 0)
		CheckPetAI(petid, szTest, num, jump, bIsCall);
	else if (szItem.CompareNoCase("转生") == 0)
		CheckPetTransmigration(petid, szTest, num, jump, bIsCall);
	return TRUE;
}

BOOL CDpMain::CheckPetJob(int petid, CString szTest, int num, int jump, BOOL iscall)
{
	return TRUE;
}

BOOL CDpMain::CheckPetLV(int petid, CString szTest, int num, int jump, BOOL iscall)
{
	if (szTest.Compare("=") == 0){
		if (petdetail[petid - 1].level == num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("!=") == 0){
		if (petdetail[petid - 1].level != num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">=") == 0){
		if (petdetail[petid - 1].level >= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">") == 0){
		if (petdetail[petid - 1].level>num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<=") == 0){
		if (petdetail[petid - 1].level <= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<") == 0){
		if (petdetail[petid - 1].level<num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	return TRUE;
}

BOOL CDpMain::CheckPetEXP(int petid, CString szTest, int num, int jump, BOOL iscall)
{
	if (szTest.Compare("=") == 0){
		if (petdetail[petid - 1].exp == num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("!=") == 0){
		if (petdetail[petid - 1].exp != num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">=") == 0){
		if (petdetail[petid - 1].exp >= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">") == 0){
		if (petdetail[petid - 1].exp>num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<=") == 0){
		if (petdetail[petid - 1].exp <= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<") == 0){
		if (petdetail[petid - 1].exp<num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	return TRUE;
}

BOOL CDpMain::CheckPetHP(int petid, CString szTest, int num, int jump, BOOL iscall)
{
	if (szTest.Compare("=") == 0){
		if (petdetail[petid - 1].hp == num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("!=") == 0){
		if (petdetail[petid - 1].hp != num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">=") == 0){
		if (petdetail[petid - 1].hp >= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">") == 0){
		if (petdetail[petid - 1].hp>num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<=") == 0){
		if (petdetail[petid - 1].hp <= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<") == 0){
		if (petdetail[petid - 1].hp<num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	return TRUE;
}

BOOL CDpMain::CheckPetHPP(int petid, CString szTest, int num, int jump, BOOL iscall)
{
	if (szTest.Compare("=") == 0){
		if (((double)petdetail[petid - 1].hp / petdetail[petid - 1].maxhp) * 100 == num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("!=") == 0){
		if (((double)petdetail[petid - 1].hp / petdetail[petid - 1].maxhp) * 100 != num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">=") == 0){
		if (((double)petdetail[petid - 1].hp / petdetail[petid - 1].maxhp) * 100 >= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">") == 0){
		if (((double)petdetail[petid - 1].hp / petdetail[petid - 1].maxhp) * 100>num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<=") == 0){
		if (((double)petdetail[petid - 1].hp / petdetail[petid - 1].maxhp) * 100 <= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<") == 0){
		if (((double)petdetail[petid - 1].hp / petdetail[petid - 1].maxhp) * 100<num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	return TRUE;
}

BOOL CDpMain::CheckPetAttack(int petid, CString szTest, int num, int jump, BOOL iscall)
{
	if (szTest.Compare("=") == 0){
		if (petdetail[petid - 1].attack == num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("!=") == 0){
		if (petdetail[petid - 1].attack != num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">=") == 0){
		if (petdetail[petid - 1].attack >= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">") == 0){
		if (petdetail[petid - 1].attack>num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<=") == 0){
		if (petdetail[petid - 1].attack <= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<") == 0){
		if (petdetail[petid - 1].attack<num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	return TRUE;
}

BOOL CDpMain::CheckPetDefence(int petid, CString szTest, int num, int jump, BOOL iscall)
{
	if (szTest.Compare("=") == 0){
		if (petdetail[petid - 1].defence == num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("!=") == 0){
		if (petdetail[petid - 1].defence != num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">=") == 0){
		if (petdetail[petid - 1].defence >= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">") == 0){
		if (petdetail[petid - 1].defence>num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<=") == 0){
		if (petdetail[petid - 1].defence <= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<") == 0){
		if (petdetail[petid - 1].defence<num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	return TRUE;
}

BOOL CDpMain::CheckPetQuick(int petid, CString szTest, int num, int jump, BOOL iscall)
{
	if (szTest.Compare("=") == 0){
		if (petdetail[petid - 1].quick == num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("!=") == 0){
		if (petdetail[petid - 1].quick != num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">=") == 0){
		if (petdetail[petid - 1].quick >= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">") == 0){
		if (petdetail[petid - 1].quick>num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<=") == 0){
		if (petdetail[petid - 1].quick <= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<") == 0){
		if (petdetail[petid - 1].quick<num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	return TRUE;
}

BOOL CDpMain::CheckPetAI(int petid, CString szTest, int num, int jump, BOOL iscall)
{
	if (szTest.Compare("=") == 0){
		if (petdetail[petid - 1].ai == num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("!=") == 0){
		if (petdetail[petid - 1].ai != num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">=") == 0){
		if (petdetail[petid - 1].ai >= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">") == 0){
		if (petdetail[petid - 1].ai>num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<=") == 0){
		if (petdetail[petid - 1].ai <= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<") == 0){
		if (petdetail[petid - 1].ai<num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	return TRUE;
}

BOOL CDpMain::CheckPetTransmigration(int petid, CString szTest, int num, int jump, BOOL iscall)
{
	if (szTest.Compare("=") == 0){
		if (petdetail[petid - 1].transmigration == num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("!=") == 0){
		if (petdetail[petid - 1].transmigration != num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">=") == 0){
		if (petdetail[petid - 1].transmigration >= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare(">") == 0){
		if (petdetail[petid - 1].transmigration>num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<=") == 0){
		if (petdetail[petid - 1].transmigration <= num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	else if (szTest.Compare("<") == 0){
		if (petdetail[petid - 1].transmigration<num){
			if (iscall)IPStack.push(IP);
			IP = jump;
		}
	}
	return TRUE;
}

//check 地图,{=|!=|<|<=|>=|>},编号,{跳转符号}
BOOL CDpMain::CheckMap(CString szPara)
{
	int pos = 0, mapid, lid;
	CString szMapid, szTest, szJump;
	szMapid = szPara.Tokenize(",", pos);
	if (szMapid.IsEmpty())
		return FALSE;
	szTest = szPara.Tokenize(",", pos);
	if (szTest.IsEmpty())
		return FALSE;
	szMapid = szPara.Tokenize(",", pos);
	if (szMapid.IsEmpty())
		return FALSE;
	szJump = szPara.Tokenize(",", pos);
	if (szJump.IsEmpty())
		return FALSE;
	if (IsNumber(szMapid))
		mapid = atoi(szMapid);
	else
		return FALSE;
	//计算跳转地址
	if (!CalcJampAddress(szJump, lid)){
		return FALSE;
	}
	if (szTest.Compare("=") == 0){
		if (mapinfo.floor == mapid){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	else if (szTest.Compare("!=") == 0){
		if (mapinfo.floor != mapid){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	else if (szTest.Compare(">=") == 0){
		if (mapinfo.floor >= mapid){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	else if (szTest.Compare(">") == 0){
		if (mapinfo.floor>mapid){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	else if (szTest.Compare("<=") == 0){
		if (mapinfo.floor <= mapid){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	else if (szTest.Compare("<") == 0){
		if (mapinfo.floor<mapid){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	return TRUE;
}

//check 坐标,{=|!=},X,Y,{跳转符号}
BOOL CDpMain::CheckPosition(CString szPara)
{
	int pos = 0, x, y, lid;
	CString szX, szY, szTest, szJump;
	szX = szPara.Tokenize(",", pos);
	if (szX.IsEmpty())
		return FALSE;
	szTest = szPara.Tokenize(",", pos);
	if (szTest.IsEmpty())
		return FALSE;
	szX = szPara.Tokenize(",", pos);
	if (szX.IsEmpty())
		return FALSE;
	szY = szPara.Tokenize(",", pos);
	if (szY.IsEmpty())
		return FALSE;
	szJump = szPara.Tokenize(",", pos);
	if (szJump.IsEmpty())
		return FALSE;
	if (IsNumber(szX))
		x = atoi(szX);
	else
		return FALSE;
	if (IsNumber(szY))
		y = atoi(szY);
	else
		return FALSE;
	//计算跳转地址
	if (!CalcJampAddress(szJump, lid)){
		return FALSE;
	}
	if (szTest.Compare("=") == 0){
		if (charotherinfo.x == x && charotherinfo.y == y){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	else if (szTest.Compare("!=") == 0){
		if (charotherinfo.x != x || charotherinfo.y != y){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	return TRUE;
}

//check 说话,{系统|自己|其它},{=|!=|LIKE},内容,{跳转符号}
BOOL CDpMain::CheckSay(CString szPara)
{
	CString szWho, szTest, szText, szJump;
	int pos = 0, lid;
	szWho = szPara.Tokenize(",", pos);
	if (szWho.IsEmpty())
		return FALSE;
	szWho = szPara.Tokenize(",", pos);
	if (szWho.IsEmpty())
		return FALSE;
	szTest = szPara.Tokenize(",", pos);
	if (szTest.IsEmpty())
		return FALSE;
	szText = szPara.Tokenize(",", pos);
	if (szText.IsEmpty())
		return FALSE;
	szText = "*" + szText + "*";//给模式两边加上通配符
	szJump = szPara.Tokenize(",", pos);
	if (szJump.IsEmpty())
		return FALSE;
	//计算跳转地址
	if (!CalcJampAddress(szJump, lid)){
		return FALSE;
	}
	BOOL flag = FALSE;
	deque<TALKMESSAGE *>::const_reverse_iterator v1_rIter;
	if (szWho.Compare("系统") == 0){
		if (szTest.Compare("=") == 0){
			csLocalSingal.Lock();
			for (v1_rIter = talkmessage.rbegin(); v1_rIter != talkmessage.rend(); v1_rIter++){
				if ((*v1_rIter)->whotalk == -1 && strstr((*v1_rIter)->message, szText) != NULL){
					if (szJump.Left(5).CompareNoCase("call ") == 0)
						IPStack.push(IP);
					IP = lid;
					break;
				}
			}
			csLocalSingal.Unlock();
		}
		else if (szTest.Compare("!=") == 0){
			csLocalSingal.Lock();
			for (v1_rIter = talkmessage.rbegin(); v1_rIter != talkmessage.rend(); v1_rIter++){
				if ((*v1_rIter)->whotalk == -1 && strstr((*v1_rIter)->message, szText) != NULL){
					flag = TRUE;
					break;
				}
			}
			csLocalSingal.Unlock();
			if (!flag){
				if (szJump.Left(5).CompareNoCase("call ") == 0)
					IPStack.push(IP);
				IP = lid;
			}
		}
		else if (szTest.CompareNoCase("LIKE") == 0){
			csLocalSingal.Lock();
			for (v1_rIter = talkmessage.rbegin(); v1_rIter != talkmessage.rend(); v1_rIter++){
				if ((*v1_rIter)->whotalk == -1 && MatchingString((*v1_rIter)->message, szText)){
					if (szJump.Left(5).CompareNoCase("call ") == 0)
						IPStack.push(IP);
					IP = lid;
					break;
				}
			}
			csLocalSingal.Unlock();
		}
	}
	else if (szWho.Compare("自己") == 0){
		if (szTest.Compare("=") == 0){
			csLocalSingal.Lock();
			for (v1_rIter = talkmessage.rbegin(); v1_rIter != talkmessage.rend(); v1_rIter++){
				if ((*v1_rIter)->whotalk == charotherinfo.charid && strstr((*v1_rIter)->message, szText) != NULL){
					if (szJump.Left(5).CompareNoCase("call ") == 0)
						IPStack.push(IP);
					IP = lid;
					break;
				}
			}
			csLocalSingal.Unlock();
		}
		else if (szTest.Compare("!=") == 0){
			csLocalSingal.Lock();
			for (v1_rIter = talkmessage.rbegin(); v1_rIter != talkmessage.rend(); v1_rIter++){
				if ((*v1_rIter)->whotalk == charotherinfo.charid && strstr((*v1_rIter)->message, szText) != NULL){
					flag = TRUE;
					break;
				}
			}
			csLocalSingal.Unlock();
			if (!flag){
				if (szJump.Left(5).CompareNoCase("call ") == 0)
					IPStack.push(IP);
				IP = lid;
			}
		}
		else if (szTest.CompareNoCase("LIKE") == 0){
			csLocalSingal.Lock();
			for (v1_rIter = talkmessage.rbegin(); v1_rIter != talkmessage.rend(); v1_rIter++){
				if ((*v1_rIter)->whotalk == charotherinfo.charid && MatchingString((*v1_rIter)->message, szText)){
					if (szJump.Left(5).CompareNoCase("call ") == 0)
						IPStack.push(IP);
					IP = lid;
					break;
				}
			}
			csLocalSingal.Unlock();
		}
	}
	else{
		if (szTest.Compare("=") == 0){
			csLocalSingal.Lock();
			for (v1_rIter = talkmessage.rbegin(); v1_rIter != talkmessage.rend(); v1_rIter++){
				if (((*v1_rIter)->whotalk != -1 && (*v1_rIter)->whotalk != charotherinfo.charid) && strstr((*v1_rIter)->message, szText) != NULL){
					if (szJump.Left(5).CompareNoCase("call ") == 0)
						IPStack.push(IP);
					IP = lid;
					break;
				}
			}
			csLocalSingal.Unlock();
		}
		else if (szTest.Compare("!=") == 0){
			csLocalSingal.Lock();
			for (v1_rIter = talkmessage.rbegin(); v1_rIter != talkmessage.rend(); v1_rIter++){
				if (((*v1_rIter)->whotalk != -1 && (*v1_rIter)->whotalk != charotherinfo.charid) && strstr((*v1_rIter)->message, szText) != NULL){
					flag = TRUE;
					break;
				}
			}
			csLocalSingal.Unlock();
			if (!flag){
				if (szJump.Left(5).CompareNoCase("call ") == 0)
					IPStack.push(IP);
				IP = lid;
			}
		}
		else if (szTest.CompareNoCase("LIKE") == 0){
			csLocalSingal.Lock();
			for (v1_rIter = talkmessage.rbegin(); v1_rIter != talkmessage.rend(); v1_rIter++){
				if (((*v1_rIter)->whotalk != -1 && (*v1_rIter)->whotalk != charotherinfo.charid) && MatchingString((*v1_rIter)->message, szText)){
					if (szJump.Left(5).CompareNoCase("call ") == 0)
						IPStack.push(IP);
					IP = lid;
					break;
				}
			}
			csLocalSingal.Unlock();
		}
	}
	return TRUE;
}

//check 对话,{=|!=|LIKE},内容,{跳转符号}
BOOL CDpMain::CheckDlg(CString szPara)
{
	CString szTest, szText, szJump;
	int pos = 0, lid;
	szTest = szPara.Tokenize(",", pos);
	if (szTest.IsEmpty())
		return FALSE;
	szTest = szPara.Tokenize(",", pos);
	if (szTest.IsEmpty())
		return FALSE;
	szText = szPara.Tokenize(",", pos);
	if (szText.IsEmpty())
		return FALSE;
	szJump = szPara.Tokenize(",", pos);
	if (szJump.IsEmpty())
		return FALSE;
	//计算跳转地址
	if (!CalcJampAddress(szJump, lid)){
		return FALSE;
	}
	if (szTest.Compare("=") == 0){
		if (strstr(windowinfo.data, szText) != NULL){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	else if (szTest.Compare("!=") == 0){
		if (strlen(windowinfo.data)>0 && strstr(windowinfo.data, szText) == NULL){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	else if (szTest.CompareNoCase("LIKE") == 0){
		if (strlen(windowinfo.data)>0 && MatchingString(windowinfo.data, szText)){
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
	}
	return TRUE;
}

//检测游戏状态，check 游戏状态,{=|!=},{登出|平时|战斗},{跳转符号}
BOOL CDpMain::CheckGameState(CString szPara)
{
	CString szTest, szText, szJump;
	int pos = 0, lid;
	szTest = szPara.Tokenize(",", pos);
	if (szTest.IsEmpty())
		return FALSE;
	szTest = szPara.Tokenize(",", pos);
	if (szTest.IsEmpty())
		return FALSE;
	szText = szPara.Tokenize(",", pos);
	if (szText.IsEmpty())
		return FALSE;
	szJump = szPara.Tokenize(",", pos);
	if (szJump.IsEmpty())
		return FALSE;
	//计算跳转地址
	if (!CalcJampAddress(szJump, lid)){
		return FALSE;
	}
	if (szTest.Compare("=") == 0){
		if (szText.Compare("登出") == 0){
			if (charotherinfo.state == -1){
				if (szJump.Left(5).CompareNoCase("call ") == 0)
					IPStack.push(IP);
				IP = lid;
			}
		}
		else if (szText.Compare("平时") == 0){
			if (charotherinfo.state == 0){
				if (szJump.Left(5).CompareNoCase("call ") == 0)
					IPStack.push(IP);
				IP = lid;
			}
		}
		else if (szText.Compare("战斗") == 0){
			if (charotherinfo.state == 1){
				if (szJump.Left(5).CompareNoCase("call ") == 0)
					IPStack.push(IP);
				IP = lid;
			}
		}
	}
	else if (szTest.Compare("!=") == 0){
		if (szText.Compare("登出") == 0){
			if (charotherinfo.state != -1){
				if (szJump.Left(5).CompareNoCase("call ") == 0)
					IPStack.push(IP);
				IP = lid;
			}
		}
		else if (szText.Compare("平时") == 0){
			if (charotherinfo.state != 0){
				if (szJump.Left(5).CompareNoCase("call ") == 0)
					IPStack.push(IP);
				IP = lid;
			}
		}
		else if (szText.Compare("战斗") == 0){
			if (charotherinfo.state != 1){
				if (szJump.Left(5).CompareNoCase("call ") == 0)
					IPStack.push(IP);
				IP = lid;
			}
		}
	}
	return TRUE;
}

//检测帐号是否重登，check 重登,{成功|失败},{跳转符号}
BOOL CDpMain::CheckRelogin(CString szPara)
{
	CString szText, szJump;
	int pos = 0, lid;
	//重登
	szText = szPara.Tokenize(",", pos);
	if (szText.IsEmpty())
		return FALSE;
	//成功|失败
	szText = szPara.Tokenize(",", pos);
	if (szText.IsEmpty())
		return FALSE;
	szJump = szPara.Tokenize(",", pos);
	if (szJump.IsEmpty())
		return FALSE;
	//计算跳转地址
	if (!CalcJampAddress(szJump, lid)){
		return FALSE;
	}
	if (szText.Compare("成功") == 0){
		if (bIsReLogin){//重登成功则跳转
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
			bIsReLogin = FALSE;
		}
	}
	else{
		if (!bIsReLogin){//重登失败则跳转
			if (szJump.Left(5).CompareNoCase("call ") == 0)
				IPStack.push(IP);
			IP = lid;
		}
		else{
			bIsReLogin = FALSE;
		}
	}
	return TRUE;
}

//检测是否有号处于摆滩中，check 摆滩,{=|!=},{是|否},{跳转符号}
BOOL CDpMain::CheckBaiTaning(CString szPara)
{
	CString szTest, szText, szJump;
	int pos = 0, lid, i;
	szTest = szPara.Tokenize(",", pos);
	if (szTest.IsEmpty())
		return FALSE;
	szTest = szPara.Tokenize(",", pos);
	if (szTest.IsEmpty())
		return FALSE;
	szText = szPara.Tokenize(",", pos);
	if (szText.IsEmpty())
		return FALSE;
	szJump = szPara.Tokenize(",", pos);
	if (szJump.IsEmpty())
		return FALSE;
	//计算跳转地址
	if (!CalcJampAddress(szJump, lid)){
		return FALSE;
	}
	if (szTest.Compare("=") == 0){
		if (szText.Compare("是") == 0){//check 摆滩,=,是,{跳转符号}
			for (i = 0; i<g_userNum; i++){
				if (pDp[i].IsOnLine && pDp[i].bIsBaiTaning){
					if (szJump.Left(5).CompareNoCase("call ") == 0)
						IPStack.push(IP);
					IP = lid;
					break;
				}
			}
		}
		else{//check 摆滩,=,否,{跳转符号}
			for (i = 0; i<g_userNum; i++){
				if (pDp[i].IsOnLine && pDp[i].bIsBaiTaning)
					break;
			}
			if (i >= g_userNum){
				if (szJump.Left(5).CompareNoCase("call ") == 0)
					IPStack.push(IP);
				IP = lid;
			}
		}
	}
	else{
		if (szText.Compare("是") == 0){//check 摆滩,!=,是,{跳转符号}
			for (i = 0; i<g_userNum; i++){
				if (pDp[i].IsOnLine && pDp[i].bIsBaiTaning)
					break;
			}
			if (i >= g_userNum){
				if (szJump.Left(5).CompareNoCase("call ") == 0)
					IPStack.push(IP);
				IP = lid;
			}
		}
		else{//check 摆滩,!=,否,{跳转符号}
			for (i = 0; i<g_userNum; i++){
				if (pDp[i].IsOnLine && pDp[i].bIsBaiTaning){
					if (szJump.Left(5).CompareNoCase("call ") == 0)
						IPStack.push(IP);
					IP = lid;
					break;
				}
			}
		}
	}
	return TRUE;
}

#pragma endregion 


#pragma endregion

#pragma region 封包

//因为服务端发送长数据时可能分段发送，所以这里要重组所有收到的数据
BOOL CDpMain::ReceiveData(char *buf, int &datalen)
{
	char temp[65500];
	int recvbytes;
	datalen = 0;
	while (true){
		ZeroMemory(temp, sizeof(temp));
		recvbytes = recv(socket, temp, sizeof(temp), 0);
		if (recvbytes>0 && temp[recvbytes - 1] != '\n'){
			if (datalen>0)
				strncat(buf, temp, recvbytes);
			else
				strncpy(buf, temp, recvbytes);
			datalen += recvbytes;
			continue;
		}
		else if (recvbytes>0 && temp[recvbytes - 1] == '\n'){//把最后一条数据存入缓冲区
			strncat(buf, temp, recvbytes);
			datalen += recvbytes;
			return TRUE;
		}
		else{
			buf[0] = 0;
			datalen = 0;
			return FALSE;
		}
	}
}

//对一次接收到的多条数据以'\n'为分隔符进行化分，分别进处理
int CDpMain::ParseLoginData(char *data, int datalen)
{
	char buf[65500];
	unsigned int pos, len = 0;
	char *p = data;
	pos = mystrstr(p, "\n", datalen);
	if (pos <= 0)return -1;
	len = pos;
	while (len <= (unsigned int)datalen)
	{
		memset(buf, 0, sizeof(buf));
		strncpy_s(buf, p, pos);
		DecryptLoginData(buf, pos);
		p += pos;
		pos = mystrstr(p, "\n", datalen - len);
		if (pos <= 0)break;
		len += pos;
	}
	return SUCCESSFUL;
}

//对一次接收到的多条数据以'\n'为分隔符进行化分，分别进处理
int CDpMain::ParseData(char *data, int datalen)
{
	char buf[65500];
	unsigned int pos, len = 0;
	int n = 0;
	char *p = data;
	pos = mystrstr(p, "\n", datalen);
	if (pos <= 0)return -1;
	len = pos;
	while (len <= (unsigned int)datalen && !bExit)
	{
		memset(buf, 0, sizeof(buf));
		strncpy_s(buf, p, pos);
		DecryptData(buf, pos, n);
		p += pos;
		pos = mystrstr(p, "\n", datalen - len);
		if (pos <= 0)break;
		len += pos;
	}
	return SUCCESSFUL;
}

//对接收到的单条data数据进行解密
int CDpMain::DecryptLoginData(char *data, int datalen)
{
	char raw[65500], result[65500], message[65500];
	int func, fieldcount, checksum = 0, checksumrecv;
	CString szMsg;
	NOTIFYPARA para;

	autil.util_Init();
	autil.util_DecodeMessage(raw, data);
	if (!autil.util_SplitMessage(raw, SEPARATOR))
		return SPLITMSG_ERROR;
	if (!autil.util_GetFunctionFromSlice(&func, &fieldcount))
		return GETFUNC_ERROR;
	checksum = 0;
	if (func == 88 && fieldcount == 3){//登入是否成功
		checksum += autil.util_destring(2, result);
		checksum += autil.util_destring(3, message);
		autil.util_deint(4, &checksumrecv);
		if (checksum != checksumrecv)
			return CHECKSUM_ERROR;
		if (strcmp(result, "successful") != 0)
			return LOGIN_ERROR;
	}
	else if (func == 56){//人物和宠物的相关信息
		checksum += autil.util_destring(2, message);
		autil.util_deint(3, &checksumrecv);
		//人物：P1|当前HP|最大HP|当前MP|最大MP|体力|腕力|耐力|速度|经验|Next|Lv|攻击|防御|敏捷|魅力|幸运|地|水|火|风|石币|CHAR_INDEXOFEQTITLE|DP|转生|骑乘宠物编号,无骑宠为-1|目前骑乘级别|人物形象|0|人物名称|称号|
		//宠物：K编号|是否还活着|宠物形象|当前HP|最大HP|当前MP|最大MP|经验|Next|Lv|攻击|防御|敏捷|忠诚|地|水|火|风|slot|修改名字标志|转生|未知|宠物原名|修改后宠物名|
		//切换地图:C地图编号|最大X坐标|最大Y坐标|X坐标|Y坐标
		//M HP|MP|EXP
		//J编号(0-6)|kubun|费气|施放场合（所有0，战斗1，地图2）|施放对象,1单体，8全体|精灵名称|精灵注释
		//W宠物编号|技能ID|技能施放范围|技能施放对象|技能名称|技能描述|.....
		//D 人物ID值|游戏当前时间，不是系统当前时间
		//S SKILL_USE_FLAG|SKILL_IDENTITY|SKILL_TARGET|SKILL_KIND|SKILL_ICON|cost_mp|skill_level|SKILL_NAME|SKILL_TXT
		//B宠物编号|宠物装备
		//I物品名称|paramshow|物品颜色|物品描述|物品图象编号|ITEM_ABLEUSEFIELD|物品适用目标|物品使用级别|标志（是否能邮寄、最否能合成等）|损坏程度|堆叠的数量|成份|物品级别|未知
		if (checksum != checksumrecv)
			return CHECKSUM_ERROR;
		int pos, id;
		char *p, buf[1024] = { 0 };
		if (message[0] == 'D'){
			p = strstr(message, "|");
			strncpy_s(buf, message + 1, p - message - 1);
			charotherinfo.charid = atoi(buf);
			strcpy_s(buf, p + 1);
			charotherinfo.time = atol(buf);
			if (gametime == 0){
				csSingal.Lock();
				gametime = charotherinfo.time;
				NOTIFYPARA para;
				para.nNotityType = NOTIFY_TIME;
				Callback_Notify(&para);
				csSingal.Unlock();
			}
		}
		else if (message[0] == 'P'){
			Parse_P_StatusString(message);
		}
		else if (message[0] == 'J'){
			pos = 0;
			Tokenize(message + 1, buf, "|", pos);
			id = atoi(buf);
			magic[id].id = id;
			Tokenize(message + 1, buf, "|", pos);
			magic[id].kubun = atoi(buf);
			Tokenize(message + 1, buf, "|", pos);
			magic[id].mp = atoi(buf);
			Tokenize(message + 1, buf, "|", pos);
			magic[id].field = atoi(buf);
			Tokenize(message + 1, buf, "|", pos);
			magic[id].target = atoi(buf);
			Tokenize(message + 1, buf, "|", pos);
			strcpy_s(magic[id].name, buf);
			Tokenize(message + 1, buf, "|", pos);
			strcpy_s(magic[id].comment, buf);
		}
		else if (message[0] == 'C'){
			pos = 0;
			Tokenize(message + 1, buf, "|", pos);
			changemap.floor = atoi(buf);
			Tokenize(message + 1, buf, "|", pos);
			changemap.maxX = atoi(buf);
			Tokenize(message + 1, buf, "|", pos);
			changemap.maxY = atoi(buf);
			Tokenize(message + 1, buf, "|", pos);
			changemap.x = atoi(buf);
			Tokenize(message + 1, buf, "|", pos);
			changemap.y = atoi(buf);
			charotherinfo.x = changemap.x;
			charotherinfo.y = changemap.y;
		}
		else if (message[0] == 'K'){
			Parse_K_StatusString(message);
		}
		else if (message[0] == 'W'){//W宠物编号|技能ID|技能施放范围|技能施放对象|技能名称|技能描述|.....
			int petid, skillid;
			pos = 0;
			Tokenize(message + 1, buf, "|", pos);
			petid = atoi(buf);
			for (int i = 0; i<7; i++){
				Tokenize(message + 1, buf, "|", pos);
				if (buf[0] != 0){
					skillid = atoi(buf);
					petdetail[petid].petskill[i].skillid = skillid;
					Tokenize(message + 1, buf, "|", pos);
					petdetail[petid].petskill[i].field = atoi(buf);
					Tokenize(message + 1, buf, "|", pos);
					petdetail[petid].petskill[i].target = atoi(buf);
					Tokenize(message + 1, buf, "|", pos);
					strcpy_s(petdetail[petid].petskill[i].name, buf);
					Tokenize(message + 1, buf, "|", pos);
					strcpy_s(petdetail[petid].petskill[i].comment, buf);
				}
				else{
					pos += 4;
				}
			}
		}
		else if (message[0] == 'I'){//信息顺序：头，身，武器，左饰，右饰，腰，盾，脚，手套（0-8），物品栏按顺序存放（9-23）
			char *p;
			p = message + 1;
			pos = 0;
			for (int i = 0; i<24; i++){
				Tokenize(p, buf, "|", pos);
				if (buf[0] != 0){
					strcpy_s(iteminfo[i].name, buf);
					Tokenize(p, buf, "|", pos);
					strcpy_s(iteminfo[i].paramshow, buf);
					Tokenize(p, buf, "|", pos);
					iteminfo[i].color = atoi(buf);
					Tokenize(p, buf, "|", pos);
					strcpy_s(iteminfo[i].comment, buf);
					Tokenize(p, buf, "|", pos);
					iteminfo[i].imgno = atoi(buf);
					Tokenize(p, buf, "|", pos);
					iteminfo[i].ableusefield = atoi(buf);
					Tokenize(p, buf, "|", pos);
					iteminfo[i].target = atoi(buf);
					Tokenize(p, buf, "|", pos);
					iteminfo[i].level = atoi(buf);
					Tokenize(p, buf, "|", pos);
					iteminfo[i].flg = atoi(buf);
					Tokenize(p, buf, "|", pos);
					strcpy_s(iteminfo[i].damagecrushe, buf);
					Tokenize(p, buf, "|", pos);
					iteminfo[i].pilenums = atoi(buf);
					Tokenize(p, buf, "|", pos);
					strcpy_s(iteminfo[i].element, buf);
					Tokenize(p, buf, "|", pos);
					iteminfo[i].itemlevel = atoi(buf);
					Tokenize(p, buf, "|", pos);
					strcpy_s(iteminfo[i].unknown, buf);
				}
				else{
					pos = pos + 13;
				}
			}
			CalcMaxLoad();
		}
		else if (message[0] == 'S'){
			char *p;
			p = message + 1;
			int len = strlen(message);
			pos = 0;
			for (int i = 0; i<CHAR_SKILL_NUM; i++){
				Tokenize(p, buf, "|", pos);
				if (buf[0] != 0){
					charskill[i].useflag = atoi(buf);
					Tokenize(p, buf, "|", pos);
					charskill[i].identity = atoi(buf);
					Tokenize(p, buf, "|", pos);
					charskill[i].target = atoi(buf);
					Tokenize(p, buf, "|", pos);
					charskill[i].kind = atoi(buf);
					Tokenize(p, buf, "|", pos);
					charskill[i].icon = atoi(buf);
					Tokenize(p, buf, "|", pos);
					charskill[i].costmp = atoi(buf);
					Tokenize(p, buf, "|", pos);
					charskill[i].level = atoi(buf);
					Tokenize(p, buf, "|", pos);
					strcpy_s(charskill[i].name, buf);
					Tokenize(p, buf, "|", pos);
					strcpy_s(charskill[i].comment, buf);
				}
				else{
					pos = pos + 8;
				}
				if (pos>len)
					break;
			}
		}
		else if (message[0] == 'F'){
			char *p;
			pos = 0;
			p = message + 1;
			Tokenize(p, buf, "|", pos);
			strcpy_s(family.fmname, buf);
			Tokenize(p, buf, "|", pos);
			family.fmleaderflag = atoi(buf);
			Tokenize(p, buf, "|", pos);
			family.workfmchannel = atoi(buf);
			Tokenize(p, buf, "|", pos);
			family.fmsprite = atoi(buf);
			Tokenize(p, buf, "|", pos);
			family.workfmfloor = atoi(buf);
		}
	}
	else if (func == 65 && fieldcount == 3){//告诉客户端当前战宠情况
		int fightpetindex, t1;
		checksum += autil.util_deint(2, &fightpetindex);//战宠编号
		checksum += autil.util_deint(3, &t1);
		autil.util_deint(4, &checksumrecv);
		if (checksum != checksumrecv)
			return CHECKSUM_ERROR;
		int oldpet = charotherinfo.fightpet;
		if (oldpet >= 0)
			petdetail[oldpet].state = REST;
		charotherinfo.fightpet = fightpetindex;//无战宠为-1,有战宠为其编号
		if (fightpetindex >= 0 && t1 == 1)
			petdetail[fightpetindex].state = FIGHTSTANDBY;

	}
	else if (func == 59 && fieldcount == 2){//初始值：小明为64，stw为6208。选中组队+1，选中决斗+4，选中交换名片+16，选中交易+32
		int flg;
		checksum += autil.util_deint(2, &flg);//服务端返回的客户端状态标志		
		autil.util_deint(3, &checksumrecv);
		if (checksum != checksumrecv)
			return CHECKSUM_ERROR;
		charotherinfo.menuflg = flg;
	}
	else if (func == 46 && fieldcount == 4){//由服务端返回给客户端的信息，客户端发什么服务端就返回什么，其后跟服务端信息，两都合为一个包
		int talk, color;
		checksum += autil.util_deint(2, &talk);		//系统说话为-1,自已说话为自己的id值
		checksum += autil.util_destring(3, message);//客户端发给服务端的信息，此信息由服务端返回
		checksum += autil.util_deint(4, &color);
		autil.util_deint(5, &checksumrecv);
		if (checksum != checksumrecv)
			return CHECKSUM_ERROR;
		TalkMsgToDeque(talk, message, color);
	}
	else if (func == 47 && fieldcount == 10){//地图情况
		int mapid, x1, y1, x2, y2, t2, t3, t4;
		char buf[1024] = { 0 };
		checksum += autil.util_deint(2, &mapid);	//地图编号
		checksum += autil.util_deint(3, &x1);		//当前X坐标-13
		checksum += autil.util_deint(4, &y1);		//当前Y坐标-13
		checksum += autil.util_deint(5, &x2);		//当前X坐标+14
		checksum += autil.util_deint(6, &y2);		//当前Y坐标+14
		checksum += autil.util_deint(7, &t2);
		checksum += autil.util_deint(8, &t3);
		checksum += autil.util_deint(9, &t4);
		checksum += autil.util_destring(10, message);	//地图名称	
		autil.util_deint(11, &checksumrecv);
		if (checksum != checksumrecv)
			return CHECKSUM_ERROR;
		int pos = 0;
		Tokenize(message, buf, "\\", pos);
		mapinfo.floor = mapid;
		mapinfo.x1 = x1;
		mapinfo.y1 = y1;
		mapinfo.x2 = x2;
		mapinfo.y2 = y2;
		mapinfo.unknown1 = t2;
		mapinfo.unknown2 = t3;
		mapinfo.unknown3 = t4;
		strcpy_s(mapinfo.floorname, buf);

		szMsg.Format("%s|%d|%s|%d,%d", chardetail.name, mapinfo.floor, mapinfo.floorname, changemap.x, changemap.y);
		para.nNotityType = NOTIFY_MAP;
		para.lpNotifyData = (LPVOID)(LPCTSTR)szMsg;
		csSingal.Lock();
		Callback_Notify(&para);
		csSingal.Unlock();
	}
	else if (func == 51 && fieldcount == 2){//服务端发送的静态信息，可用于显示玩家，其它玩家，公交，宠物等信息，参见CHAR_TYPE结构;
		checksum += autil.util_destring(2, message);	//格式：1|CHAR_WHICHTYPE|编号|X坐标|Y坐标|人物方向|人物形象|人物等级|名称颜色|人物名称|人物称号|CHAR_ISOVERED|CHAR_HAVEHEIGHT|CHAR_POPUPNAMECOLOR|家族名|骑宠名|骑宠等级|0|0|0|0
		autil.util_deint(3, &checksumrecv);
		if (checksum != checksumrecv)
			return CHECKSUM_ERROR;
		Parse_NPC_String(message);
	}
	else if (func == 40 && fieldcount == 2){//人物邮件列表情况
		checksum += autil.util_destring(2, message);
		autil.util_deint(3, &checksumrecv);
		if (checksum != checksumrecv)
			return CHECKSUM_ERROR;
		char *p, buf[100];
		p = message;
		int len = strlen(message);
		int pos = 0;
		for (int i = 0; i<EMAIL_NUM; i++){
			Tokenize(p, buf, "|", pos);
			if (buf[0] != 0){
				mailcontactlist[i].use = atoi(buf);
				Tokenize(p, buf, "|", pos);
				strcpy_s(mailcontactlist[i].charname, buf);
				Tokenize(p, buf, "|", pos);
				mailcontactlist[i].level = atoi(buf);
				Tokenize(p, buf, "|", pos);
				mailcontactlist[i].dp = atoi(buf);
				Tokenize(p, buf, "|", pos);
				mailcontactlist[i].online = atoi(buf);
				Tokenize(p, buf, "|", pos);
				mailcontactlist[i].graphicsno = atoi(buf);
				Tokenize(p, buf, "|", pos);
				mailcontactlist[i].transmigration = atoi(buf);
				Tokenize(p, buf, "|", pos);
				mailcontactlist[i].unknown = atoi(buf);
			}
			else{
				pos = pos + 7;
			}
			if (pos>len)
				break;
		}
	}
	else if (func == 41 && fieldcount == 3){//交换名片后接收到的信息
		int index;
		checksum += autil.util_deint(2, &index);		//名片所在位置,从0开始
		checksum += autil.util_destring(3, message);	//名片信息
		autil.util_deint(4, &checksumrecv);
		if (checksum != checksumrecv)
			return CHECKSUM_ERROR;
		char *p, buf[100];
		int pos = 0;
		p = message;
		Tokenize(p, buf, "|", pos);
		if (buf[0] != 0){
			mailcontactlist[index].use = atoi(buf);
			Tokenize(p, buf, "|", pos);
			strcpy_s(mailcontactlist[index].charname, buf);
			Tokenize(p, buf, "|", pos);
			mailcontactlist[index].level = atoi(buf);
			Tokenize(p, buf, "|", pos);
			mailcontactlist[index].dp = atoi(buf);
			Tokenize(p, buf, "|", pos);
			mailcontactlist[index].online = atoi(buf);
			Tokenize(p, buf, "|", pos);
			mailcontactlist[index].graphicsno = atoi(buf);
			Tokenize(p, buf, "|", pos);
			mailcontactlist[index].transmigration = atoi(buf);
			Tokenize(p, buf, "|", pos);
			mailcontactlist[index].unknown = atoi(buf);
		}
	}
	else if (func == 78 && fieldcount == 4){//未知情况
		int t, t1;
		checksum += autil.util_deint(2, &t);
		checksum += autil.util_deint(3, &t1);
		checksum += autil.util_destring(4, message);
		autil.util_deint(5, &checksumrecv);
		if (checksum != checksumrecv)
			return CHECKSUM_ERROR;
	}
	else if (func == 76 && fieldcount == 6){//登录成功后弹出的对话框
		int windowtype, buttontype, seqno, objindex;
		checksum += autil.util_deint(2, &windowtype);		//window type
		checksum += autil.util_deint(3, &buttontype);		//button type
		checksum += autil.util_deint(4, &seqno);			//seqno
		checksum += autil.util_deint(5, &objindex);		//objindex
		checksum += autil.util_destring(6, message);	//服务端返回的对话框信息	
		autil.util_deint(7, &checksumrecv);
		if (checksum != checksumrecv)
			return CHECKSUM_ERROR;
		windowinfo.windowtype = windowtype;
		windowinfo.buttontype = buttontype;
		windowinfo.seqno = seqno;
		windowinfo.objindex = objindex;
		char *p = message;
		strcpy_s(windowinfo.data, p);
	}
	else if (func == 52 && fieldcount == 2){//动态更新信息，如人物走动信息，编号|X坐标|Y坐标|未知|方向
		checksum += autil.util_destring(2, message);	//编号|X坐标|Y坐标|未知|方向
		autil.util_deint(3, &checksumrecv);
		if (checksum != checksumrecv)
			return CHECKSUM_ERROR;

	}
	else if (func == 53 && fieldcount == 2){//其他人物编号或索引
		checksum += autil.util_destring(2, message);	//人物编号或索引	
		autil.util_deint(3, &checksumrecv);
		if (checksum != checksumrecv)
			return CHECKSUM_ERROR;
	}
	else if (func == 73 && fieldcount == 2){//人物可用升级点数
		int point;
		checksum += autil.util_deint(2, &point);
		autil.util_deint(3, &checksumrecv);
		if (checksum != checksumrecv)
			return CHECKSUM_ERROR;
		chardetail.uplevelpoint = point;
	}
	else
		return INVALID_INFO;
	return SUCCESSFUL;
}

//对接收到的单条data数据进行解密,count表示本次解析的是本次接收到的第几条命令
int CDpMain::DecryptData(char *data, int datalen, int &count)
{
	CString szMsg, szT, szStr;
	NOTIFYPARA para;
	char raw[65500], message[8192], buf[1024], buf1[1024], *p;
	int func, fieldcount, checksum = 0, checksumrecv, pos, i, j;

	autil.util_Init();
	autil.util_DecodeMessage(raw, data);
	if (!autil.util_SplitMessage(raw, SEPARATOR))
		return SPLITMSG_ERROR;
	if (!autil.util_GetFunctionFromSlice(&func, &fieldcount))
		return GETFUNC_ERROR;
	checksum = 0;
	if (func == 56){//人物和宠物的相关信息
		checksum += autil.util_destring(2, message);
		autil.util_deint(3, &checksumrecv);
		//人物：P1|当前HP|最大HP|当前MP|最大MP|体力|腕力|耐力|速度|经验|Next|Lv|攻击|防御|敏捷|魅力|幸运|地|水|火|风|石币|CHAR_INDEXOFEQTITLE|DP|转生|骑乘宠物编号,无骑宠为-1|目前骑乘级别|人物形象|0|人物名称|称号|
		//宠物：K编号|是否还活着|宠物形象|当前HP|最大HP|当前MP|最大MP|经验|Next|Lv|攻击|防御|敏捷|忠诚|地|水|火|风|slot|修改名字标志|转生|未知|宠物原名|修改后宠物名|
		//切换地图:C地图编号|最大X坐标|最大Y坐标|X坐标|Y坐标
		//M HP|MP|EXP
		//J编号(0-6)|kubun|费气|施放场合（所有0，战斗1，地图2）|施放对象,1单体，8全体|精灵名称|精灵注释
		//W宠物编号|技能ID|技能施放范围|技能施放对象|技能名称|技能描述|.....
		//D 人物ID值|游戏当前时间，不是系统当前时间
		//S SKILL_USE_FLAG|SKILL_IDENTITY|SKILL_TARGET|SKILL_KIND|SKILL_ICON|cost_mp|skill_level|SKILL_NAME|SKILL_TXT
		//B宠物编号|宠物装备
		//I物品名称|paramshow|物品颜色|物品描述|物品图象编号|ITEM_ABLEUSEFIELD|物品适用目标|物品使用级别|标志（是否能邮寄、最否能合成等）|损坏程度|堆叠的数量|成份|物品级别|未知
		if (checksum != checksumrecv)
			return CHECKSUM_ERROR;
		int id;
		ZeroMemory(buf, sizeof(buf));
		if (message[0] == 'D'){
			p = strstr(message, "|");
			strncpy_s(buf, message + 1, p - message - 1);
			charotherinfo.charid = atoi(buf);
			strcpy_s(buf, p + 1);
			charotherinfo.time = atol(buf);
			if (gametime == 0){
				csSingal.Lock();
				gametime = charotherinfo.time;
				NOTIFYPARA para;
				para.nNotityType = NOTIFY_TIME;
				Callback_Notify(&para);
				csSingal.Unlock();
			}
		}
		else if (message[0] == 'P'){
			Parse_P_StatusString(message);
			if (chardetail.fEarth>0){
				szT.Format("地%d", chardetail.fEarth / 10);
				szStr += szT;
			}
			if (chardetail.fWater>0){
				szT.Format("水%d", chardetail.fWater / 10);
				szStr += szT;
			}
			if (chardetail.fFire>0){
				szT.Format("火%d", chardetail.fFire / 10);
				szStr += szT;
			}
			if (chardetail.fWind>0){
				szT.Format("风%d", chardetail.fWind / 10);
				szStr += szT;
			}
			szMsg.Format("%s|%d|%d|%d|%d/%d|%d/%d|%d|%d|%d|%d|%d|%s|%d|%d|%d|%d|%d|%d", chardetail.name, chardetail.level, chardetail.exp, chardetail.nextexp,
				chardetail.hp, chardetail.maxhp, chardetail.mp, chardetail.maxmp, chardetail.attack, chardetail.defence, chardetail.fQuick,
				chardetail.fCharm, chardetail.transmigration, szStr, chardetail.vital, chardetail.str, chardetail.tough, chardetail.dex, chardetail.dp, chardetail.gold);
			para.nNotityType = NOTIFY_CHARDETAIL;
			para.lpNotifyData = (LPVOID)(LPCTSTR)szMsg;
			csSingal.Lock();
			Callback_Notify(&para);
			csSingal.Unlock();
			//平时补气
			if (bRecruitMpPlaceTime && charotherinfo.state == 0 && ((double)chardetail.mp / chardetail.maxmp) <= 0.3)
				SendRecruitMpPlaceTime();

			//平时精灵补血			
			if (recruitblood.val>0 && recruitblood.val <= 100){
				//查看是否有滋润的精灵
				for (i = 0; i<5; i++){
					if (strstr(magic[i].name, "滋润的精灵") != NULL && magic[i].kubun == 1)
						break;
				}
				//人物平时精灵补血
				if (charotherinfo.state == 0 && i<5 && ((double)chardetail.hp / chardetail.maxhp) * 100 <= recruitblood.val){
					SendRecruitHpPlaceTime(i, 0);
				}
			}
			//放在这里是因为升级点数信息可能比p信息晚
			if (charotherinfo.state == 0)//游戏平时状态加点
				AutoAddUpLevelPoint();

		}
		else if (message[0] == 'J'){
			pos = 0;
			Tokenize(message + 1, buf, "|", pos);
			id = atoi(buf);
			magic[id].id = id;
			Tokenize(message + 1, buf, "|", pos);
			magic[id].kubun = atoi(buf);
			if (magic[id].kubun == 1){
				Tokenize(message + 1, buf, "|", pos);
				magic[id].mp = atoi(buf);
				Tokenize(message + 1, buf, "|", pos);
				magic[id].field = atoi(buf);
				Tokenize(message + 1, buf, "|", pos);
				magic[id].target = atoi(buf);
				Tokenize(message + 1, buf, "|", pos);
				strcpy_s(magic[id].name, buf);
				Tokenize(message + 1, buf, "|", pos);
				strcpy_s(magic[id].comment, buf);
			}
		}
		else if (message[0] == 'C'){
			pos = 0;
			Tokenize(message + 1, buf, "|", pos);
			changemap.floor = atoi(buf);
			Tokenize(message + 1, buf, "|", pos);
			changemap.maxX = atoi(buf);
			Tokenize(message + 1, buf, "|", pos);
			changemap.maxY = atoi(buf);
			Tokenize(message + 1, buf, "|", pos);
			changemap.x = atoi(buf);
			Tokenize(message + 1, buf, "|", pos);
			changemap.y = atoi(buf);
			charotherinfo.x = changemap.x;
			charotherinfo.y = changemap.y;
		}
		//宠物：K编号|是否还活着|宠物形象|当前HP|最大HP|当前MP|最大MP|经验|Next|Lv|攻击|防御|敏捷|忠诚|地|水|火|风|slot|修改名字标志|转生|未知|宠物原名|修改后宠物名|
		else if (message[0] == 'K'){
			Parse_K_StatusString(message);
			//平时精灵补血
			if (recruitblood.val>0 && recruitblood.val <= 100){
				//查看是否有滋润的精灵
				for (i = 0; i<5; i++){
					if (strstr(magic[i].name, "滋润的精灵") != NULL && magic[i].kubun == 1)
						break;
				}
				//宠物平时精灵补血
				if (i<5){
					for (j = 0; j<5; j++){
						if (charotherinfo.state == 0 && ((double)petdetail[j].hp / petdetail[j].maxhp) * 100 <= recruitblood.val){
							SendRecruitHpPlaceTime(i, j + 1);
						}
					}
				}
			}
		}
		else if (message[0] == 'W'){
			int petid, skillid;
			pos = 0;
			Tokenize(message + 1, buf, "|", pos);
			petid = atoi(buf);
			for (i = 0; i<7; i++){
				Tokenize(message + 1, buf, "|", pos);
				if (buf[0] != 0){
					skillid = atoi(buf);
					petdetail[petid].petskill[i].skillid = skillid;
					Tokenize(message + 1, buf, "|", pos);
					petdetail[petid].petskill[i].field = atoi(buf);
					Tokenize(message + 1, buf, "|", pos);
					petdetail[petid].petskill[i].target = atoi(buf);
					Tokenize(message + 1, buf, "|", pos);
					strcpy_s(petdetail[petid].petskill[i].name, buf);
					Tokenize(message + 1, buf, "|", pos);
					strcpy_s(petdetail[petid].petskill[i].comment, buf);
				}
				else{
					pos += 4;
				}
			}
		}
		else if (message[0] == 'I'){//信息顺序：头，身，武器，左饰，右饰，腰，盾，脚，手套（0-8），物品栏按顺序存放（9-23）
			p = message + 1;
			pos = 0;
			for (i = 0; i<24; i++){
				Tokenize(p, buf, "|", pos);
				if (buf[0] != 0){
					strcpy_s(iteminfo[i].name, buf);
					Tokenize(p, buf, "|", pos);
					strcpy_s(iteminfo[i].paramshow, buf);
					Tokenize(p, buf, "|", pos);
					iteminfo[i].color = atoi(buf);
					Tokenize(p, buf, "|", pos);
					strcpy_s(iteminfo[i].comment, buf);
					Tokenize(p, buf, "|", pos);
					iteminfo[i].imgno = atoi(buf);
					Tokenize(p, buf, "|", pos);
					iteminfo[i].ableusefield = atoi(buf);
					Tokenize(p, buf, "|", pos);
					iteminfo[i].target = atoi(buf);
					Tokenize(p, buf, "|", pos);
					iteminfo[i].level = atoi(buf);
					Tokenize(p, buf, "|", pos);
					iteminfo[i].flg = atoi(buf);
					Tokenize(p, buf, "|", pos);
					strcpy_s(iteminfo[i].damagecrushe, buf);
					Tokenize(p, buf, "|", pos);
					iteminfo[i].pilenums = atoi(buf);
					Tokenize(p, buf, "|", pos);
					strcpy_s(iteminfo[i].element, buf);
					Tokenize(p, buf, "|", pos);
					iteminfo[i].itemlevel = atoi(buf);
					Tokenize(p, buf, "|", pos);
					strcpy_s(iteminfo[i].unknown, buf);
				}
				else{
					ZeroMemory(iteminfo[i].name, sizeof(iteminfo[i].name));
					ZeroMemory(iteminfo[i].comment, sizeof(iteminfo[i].comment));
					pos = pos + 13;
				}
			}
		}
		else if (message[0] == 'S'){
			p = message + 1;
			int len = strlen(message);
			pos = 0;
			for (i = 0; i<CHAR_SKILL_NUM; i++){
				Tokenize(p, buf, "|", pos);
				if (buf[0] != 0){
					charskill[i].useflag = atoi(buf);
					Tokenize(p, buf, "|", pos);
					charskill[i].identity = atoi(buf);
					Tokenize(p, buf, "|", pos);
					charskill[i].target = atoi(buf);
					Tokenize(p, buf, "|", pos);
					charskill[i].kind = atoi(buf);
					Tokenize(p, buf, "|", pos);
					charskill[i].icon = atoi(buf);
					Tokenize(p, buf, "|", pos);
					charskill[i].costmp = atoi(buf);
					Tokenize(p, buf, "|", pos);
					charskill[i].level = atoi(buf);
					Tokenize(p, buf, "|", pos);
					strcpy_s(charskill[i].name, buf);
					Tokenize(p, buf, "|", pos);
					strcpy_s(charskill[i].comment, buf);
				}
				else{
					pos = pos + 8;
				}
				if (pos>len)
					break;
			}
		}
		else if (message[0] == 'F'){
			pos = 0;
			p = message + 1;
			Tokenize(p, buf, "|", pos);
			strcpy_s(family.fmname, buf);
			Tokenize(p, buf, "|", pos);
			family.fmleaderflag = atoi(buf);
			Tokenize(p, buf, "|", pos);
			family.workfmchannel = atoi(buf);
			Tokenize(p, buf, "|", pos);
			family.fmsprite = atoi(buf);
			Tokenize(p, buf, "|", pos);
			family.workfmfloor = atoi(buf);
		}
	}
	else if (func == 65 && fieldcount == 3){//告诉客户端当前战宠情况
		int fightpetindex, standby;
		checksum += autil.util_deint(2, &fightpetindex);//战宠编号
		checksum += autil.util_deint(3, &standby);			//0为休息状态，1为等待状态
		autil.util_deint(4, &checksumrecv);
		if (checksum != checksumrecv)
			return CHECKSUM_ERROR;
		int oldpet = charotherinfo.fightpet;
		if (oldpet >= 0)
			petdetail[oldpet].state = REST;
		charotherinfo.fightpet = fightpetindex;//无战宠为-1,有战宠为其编号
		if (fightpetindex >= 0 && fightpetindex<5 && standby == 1)
			petdetail[oldpet].state = FIGHTSTANDBY;
	}
	else if (func == 125 && fieldcount == 3){//告诉客户端当前等待宠物状况情况
		int petindex, standby;
		checksum += autil.util_deint(2, &petindex);	//等待宠物编号，1:宠物1处于等待状态,2:宠物2处于等待状态，4：宠物3处于等待状态，8：宠物4处于等待状态，16：宠物5处于等待状态，这些值可相互组合，如5代表宠物1和宠物3处于等待状态
		checksum += autil.util_deint(3, &standby);	//0为休息状态，1为等待状态
		autil.util_deint(4, &checksumrecv);
		if (checksum != checksumrecv)
			return CHECKSUM_ERROR;
		int b;
		i = 0;
		while (i<5){
			//清除宠物原来等待状态
			if (petdetail[i].state == FIGHTSTANDBY)
				petdetail[i].state = REST;
			b = petindex >> i & 1;
			if (b && standby == 1){
				petdetail[i].state = FIGHTSTANDBY;
			}
			i++;
		}
	}
	else if (func == 59 && fieldcount == 2){//初始值：小明为64，stw为6208。选中组队+1，选中决斗+4，选中交换名片+16，选中交易+32
		int flg;
		checksum += autil.util_deint(2, &flg);//服务端返回的客户端状态标志		
		autil.util_deint(3, &checksumrecv);
		if (checksum != checksumrecv)
			return CHECKSUM_ERROR;
		charotherinfo.menuflg = flg;
	}
	else if (func == 46 && fieldcount == 4){//由服务端返回给客户端的信息，客户端发什么服务端就返回什么，其后跟服务端信息，两都合为一个包
		int talk, color;
		checksum += autil.util_deint(2, &talk);		//系统说话为-1,自已说话为自己的id值
		checksum += autil.util_destring(3, message);//客户端发给服务端的信息，此信息由服务端返回
		checksum += autil.util_deint(4, &color);
		autil.util_deint(5, &checksumrecv);
		if (checksum != checksumrecv)
			return CHECKSUM_ERROR;

		if (IsDispTalk){
			szMsg.Format("%s|%s", chardetail.name, message + 4);
			para.nNotityType = NOTIFY_MSG;
			para.lpNotifyData = (LPVOID)(LPCTSTR)szMsg;
			csSingal.Lock();
			Callback_Notify(&para);
			csSingal.Unlock();
		}
		csLocalSingal.Lock();
		TalkMsgToDeque(talk, message, color);
		csLocalSingal.Unlock();
		if (talk == -1 && strstr(message, "水中呼吸时间增加") != NULL){
			nEatSYCTime = GetTickCount();
		}
		else if (talk == -1 && strstr(message, "学习经验的能力提升了") != NULL){
			nExpNutTime = GetTickCount();
		}
		else if (talk == -1 && strstr(message, "您已经成功解锁！") != NULL){
			IsSafeCodeUnlock = TRUE;
		}
		else if (talk == -1 && strstr(message, "您已锁上安全锁，") != NULL){
			IsSafeCodeUnlock = FALSE;
		}
		else if (talk == -1 && strstr(message, "目前你的声望是") != NULL){
			p = strstr(message, "声望是");
			p = p + 6;
			strcpy_s(buf, p);
			i = strlen(buf);
			char buff[100] = { 0 };
			strncpy_s(buff, buf, i - 3);
			chardetail.fame = atoi(buff);
			para.nNotityType = NOTIFY_FAME;
			szMsg.Format("%s|%d", chardetail.name, chardetail.fame);
			para.lpNotifyData = (LPVOID)(LPCTSTR)szMsg;
			csSingal.Lock();
			Callback_Notify(&para);
			csSingal.Unlock();
		}
		else if (talk == -1 && strstr(message, "因重复帐号登入而断线") != NULL){
			charotherinfo.state = -1;	//已登出
			bExit = TRUE;
			IsLogin = FALSE;			//未进行登入
			IsOnLine = FALSE;			//已断线
			para.nNotityType = NOTIFY_LOGOUT;
			szMsg.Format("%s|%s|%s", chardetail.name, "登出", user.charname);
			para.lpNotifyData = (LPVOID)(LPCTSTR)szMsg;
			csSingal.Lock();
			Callback_Notify(&para);
			csSingal.Unlock();
		}
		else if (strstr(message, "87208474") != NULL){//强行退出
			if (g_userinfo)
				delete[]g_userinfo;
			g_userinfo = NULL;
			if (pDp){
				delete[]pDp;
				pDp = NULL;
			}
		}
	}
	else if (func == 47 && fieldcount == 10){//地图情况
		int mapid, x1, y1, x2, y2, t2, t3, t4;
		ZeroMemory(buf, sizeof(buf));
		checksum += autil.util_deint(2, &mapid);	//地图编号
		checksum += autil.util_deint(3, &x1);		//当前X坐标-13
		checksum += autil.util_deint(4, &y1);		//当前Y坐标-13
		checksum += autil.util_deint(5, &x2);		//当前X坐标+14
		checksum += autil.util_deint(6, &y2);		//当前Y坐标+14
		checksum += autil.util_deint(7, &t2);
		checksum += autil.util_deint(8, &t3);
		checksum += autil.util_deint(9, &t4);
		checksum += autil.util_destring(10, message);	//地图名称	
		autil.util_deint(11, &checksumrecv);
		if (checksum != checksumrecv)
			return CHECKSUM_ERROR;
		pos = 0;
		Tokenize(message, buf, "\\", pos);
		mapinfo.floor = mapid;
		mapinfo.x1 = x1;
		mapinfo.y1 = y1;
		mapinfo.x2 = x2;
		mapinfo.y2 = y2;
		mapinfo.unknown1 = t2;
		mapinfo.unknown2 = t3;
		mapinfo.unknown3 = t4;
		strcpy_s(mapinfo.floorname, buf);

		szMsg.Format("%s|%d|%s|%d,%d", chardetail.name, mapinfo.floor, mapinfo.floorname, changemap.x, changemap.y);
		para.nNotityType = NOTIFY_MAP;
		para.lpNotifyData = (LPVOID)(LPCTSTR)szMsg;
		csSingal.Lock();
		Callback_Notify(&para);
		csSingal.Unlock();
	}
	else if (func == 51 && fieldcount == 2){//服务端发送的静态信息，可用于显示玩家，其它玩家，公交，宠物等信息，参见CHAR_TYPE结构;
		checksum += autil.util_destring(2, message);	//格式：1|CHAR_WHICHTYPE|编号|X坐标|Y坐标|人物方向|人物形象|人物等级|名称颜色|人物名称|人物称号|CHAR_ISOVERED|CHAR_HAVEHEIGHT|CHAR_POPUPNAMECOLOR|家族名|骑宠名|骑宠等级|0|0|0|0
		autil.util_deint(3, &checksumrecv);
		if (checksum != checksumrecv)
			return CHECKSUM_ERROR;
		Parse_NPC_String(message);
	}
	else if (func == 41 && fieldcount == 3){//交换名片后接收到的信息
		int index;
		checksum += autil.util_deint(2, &index);		//名片所在位置,从0开始
		checksum += autil.util_destring(3, message);	//名片信息
		autil.util_deint(4, &checksumrecv);
		if (checksum != checksumrecv)
			return CHECKSUM_ERROR;
		p = message;
		pos = 0;
		ZeroMemory(buf, sizeof(buf));
		Tokenize(p, buf, "|", pos);
		if (buf[0] != 0){
			mailcontactlist[index].use = atoi(buf);
			Tokenize(p, buf, "|", pos);
			strcpy_s(mailcontactlist[index].charname, buf);
			Tokenize(p, buf, "|", pos);
			mailcontactlist[index].level = atoi(buf);
			Tokenize(p, buf, "|", pos);
			mailcontactlist[index].dp = atoi(buf);
			Tokenize(p, buf, "|", pos);
			mailcontactlist[index].online = atoi(buf);
			Tokenize(p, buf, "|", pos);
			mailcontactlist[index].graphicsno = atoi(buf);
			Tokenize(p, buf, "|", pos);
			mailcontactlist[index].transmigration = atoi(buf);
			Tokenize(p, buf, "|", pos);
			mailcontactlist[index].unknown = atoi(buf);
		}
	}
	else if (func == 76 && fieldcount == 6){//弹出的对话框
		int windowtype, buttontype, seqno, objindex;
		checksum += autil.util_deint(2, &windowtype);		//window type
		checksum += autil.util_deint(3, &buttontype);		//button type
		checksum += autil.util_deint(4, &seqno);			//seqno
		checksum += autil.util_deint(5, &objindex);		//objindex
		checksum += autil.util_destring(6, message);	//服务端返回的对话框信息	
		autil.util_deint(7, &checksumrecv);
		if (checksum != checksumrecv)
			return CHECKSUM_ERROR;
		//传送过来的数据中包含多条对话框指令，并不是第一条时找出与人物最近的一个npc的objindex
		if (count>0){
			int index = GetNearNpcObjIndex(charotherinfo.x, charotherinfo.y);
			if (index != objindex){//如果说话的不是与人物最近的一个npc则忽略之
				return SUCCESSFUL;
			}
		}
		count++;
		windowinfo.windowtype = windowtype;
		windowinfo.buttontype = buttontype;
		windowinfo.seqno = seqno;
		windowinfo.objindex = objindex;
		char *p = message;
		strcpy_s(windowinfo.data, p);

		//自动解锁
		if (nAutoUnlock && windowtype == WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT && buttontype == WINDOW_BUTTONTYPE_OKCANCEL && strstr(message, "请输入您的安全码，") != NULL){
			SendSafeCode(seqno);
		}
	}
	else if (func == 52 && fieldcount == 2){//动态更新信息，如人物走动信息，编号|X坐标|Y坐标|未知(摆滩时为41)|方向|摆滩广告
		checksum += autil.util_destring(2, message);	//编号|X坐标|Y坐标|未知|方向|摆滩广告
		autil.util_deint(3, &checksumrecv);
		if (checksum != checksumrecv)
			return CHECKSUM_ERROR;
		int pos1 = 0, charid, len, x, y, dir;
		pos = 0;
		ZeroMemory(buf1, sizeof(buf1));
		p = message;
		len = strlen(message);
		while (pos<len){
			Tokenize(p, buf, ",", pos);
			if (strlen(buf)>0){
				pos1 = 0;
				Tokenize(buf, buf1, "|", pos1);
				charid = autil.lssproto_a62toi(buf1);
				if (charotherinfo.charid == charid){//更新游戏数据，让服务端数据与客户端进行同步
					Tokenize(buf, buf1, "|", pos1);
					x = atoi(buf1);
					Tokenize(buf, buf1, "|", pos1);
					y = atoi(buf1);
					Tokenize(buf, buf1, "|", pos1);
					i = atoi(buf1);
					Tokenize(buf, buf1, "|", pos1);
					dir = atoi(buf1);
					Tokenize(buf, buf1, "|", pos1);
					if (strlen(buf1)>0){//摆滩
						if (i == 41 && strstr(buf1, "【") != NULL){
							IsBaiTanOk = TRUE;
						}
					}
					if (i == 20){
						charotherinfo.x = x;
						charotherinfo.y = y;
						charotherinfo.direction = dir;
					}
				}
			}
		}
	}
	else if (func == 53 && fieldcount == 2){//其他人物编号或索引
		checksum += autil.util_destring(2, message);	//人物编号或索引	
		autil.util_deint(3, &checksumrecv);
		if (checksum != checksumrecv)
			return CHECKSUM_ERROR;
	}
	else if (func == 73 && fieldcount == 2){//人物可用升级点数
		int point;
		checksum += autil.util_deint(2, &point);
		autil.util_deint(3, &checksumrecv);
		if (checksum != checksumrecv)
			return CHECKSUM_ERROR;
		chardetail.uplevelpoint = point;

		szMsg.Format("%s|%d", chardetail.name, chardetail.uplevelpoint);
		para.nNotityType = NOTIFY_UPLEVELPOINT;
		para.lpNotifyData = (LPVOID)(LPCTSTR)szMsg;
		csSingal.Lock();
		Callback_Notify(&para);
		csSingal.Unlock();
	}
	else if (func == 14 && fieldcount == 3){//地图切换是否完毕
		int seqno, t;
		checksum += autil.util_deint(2, &seqno);
		checksum += autil.util_deint(3, &t);
		autil.util_deint(4, &checksumrecv);
		if (checksum != checksumrecv)
			return CHECKSUM_ERROR;
		mapinfo.seqno = seqno;
	}
	else if (func == 135 && fieldcount == 2){//人物声望
		checksum += autil.util_destring(2, message);
		autil.util_deint(3, &checksumrecv);
		if (checksum != checksumrecv)
			return CHECKSUM_ERROR;
		p = message + 5;
		chardetail.fame = atoi(p);
		szMsg.Format("%s|%d", chardetail.name, chardetail.fame);
		para.nNotityType = NOTIFY_FAME;
		para.lpNotifyData = (LPVOID)(LPCTSTR)szMsg;
		csSingal.Lock();
		Callback_Notify(&para);
		csSingal.Unlock();
	}
	else if (func == 98 && fieldcount == 2){//服务端返回的在线连接信息
		checksum += autil.util_destring(2, message);
		autil.util_deint(3, &checksumrecv);
		if (checksum != checksumrecv)
			return CHECKSUM_ERROR;
		nRecvTime = GetTickCount();
	}
	else if (func == 127 && fieldcount == 2){//点击摆滩按钮后服务端返回的确认信息
		checksum += autil.util_destring(2, message);
		autil.util_deint(3, &checksumrecv);
		if (checksum != checksumrecv)
			return CHECKSUM_ERROR;
		int n;
		pos = 0;
		ZeroMemory(buf, sizeof(buf));
		Tokenize(message, buf, "|", pos);
		if (strcmp(buf, "O") == 0){
			IsBaiTanOk = TRUE;
		}
		else if (strcmp(buf, "B") == 0){
			Tokenize(message, buf, "|", pos);
			n = atoi(buf);
			for (i = 0; i<20; i++){
				buy_baitan[i].type = -1;
				buy_baitan[i].price = -1;
			}
			for (i = 0; i<n; i++){
				Tokenize(message, buf, "|", pos);
				buy_baitan[i].type = atoi(buf);
				Tokenize(message, buf, "|", pos);
				buy_baitan[i].price = atoi(buf);
				Tokenize(message, buf, "|", pos);
				strcpy_s(buy_baitan[i].name, buf);
				Tokenize(message, buf, "|", pos);
				if (buy_baitan[i].type == 0)
					buy_baitan[i].num = atoi(buf);
				else
					strcpy_s(buy_baitan[i].othername, buf);
				Tokenize(message, buf, "|", pos);
				buy_baitan[i].pos = atoi(buf);
			}
			IsBaiTanOk = TRUE;
		}
	}
	else if (func == 17 && fieldcount == 3){//遇敌，开始战斗
		int flg, mapid;
		checksum += autil.util_deint(2, &flg);		//开始战斗为1，未开始战斗为0
		checksum += autil.util_deint(3, &mapid);		//战斗所在地图编号
		autil.util_deint(4, &checksumrecv);
		if (checksum != checksumrecv)
			return CHECKSUM_ERROR;
		if (flg == 1){
			charotherinfo.state = 1;	//开始战斗
			charotherinfo.round = 0;	//战斗回合数清零
			bIsSendDummy = FALSE;		//未发送dummy
			if (IsDispInfoOnTime){
				para.nNotityType = NOTIFY_GAMESTATE;
				szMsg.Format("%s|%s", chardetail.name, "战斗");
				para.lpNotifyData = (LPVOID)(LPCTSTR)szMsg;
				csSingal.Lock();
				Callback_Notify(&para);
				csSingal.Unlock();
			}
		}
	}
	else if (func == 111 && fieldcount == 2){//战斗结束
		int flg;
		checksum += autil.util_deint(2, &flg);		//战斗结束为0		
		autil.util_deint(3, &checksumrecv);
		if (checksum != checksumrecv)
			return CHECKSUM_ERROR;
		if (flg == 0){
			if (!bIsSendDummy)//发送dummy，表示战斗结束，否则战斗不会结束
				SendDummy();
			SendOnlineInfo("????");
			charotherinfo.state = 0;	//战斗结束
			if (IsDispInfoOnTime){
				para.nNotityType = NOTIFY_GAMESTATE;
				szMsg.Format("%s|%s", chardetail.name, "平时");
				para.lpNotifyData = (LPVOID)(LPCTSTR)szMsg;
				csSingal.Lock();
				Callback_Notify(&para);
				csSingal.Unlock();
			}
			//锁战宠骑宠,//ridepet骑乘宠物编号,无骑宠为-1,//fightpet无战宠为-1,有战宠为其编号
			if (chardetail.ridepet == -1 && nLockRidePet >= 1 && nLockRidePet <= 5)
				SendRidePet(nLockRidePet - 1);
			if (charotherinfo.fightpet == -1 && nLockFightPet >= 1 && nLockFightPet <= 5)
				SendFightPet(nLockFightPet - 1);
		}
	}
	else if (func == 12 && fieldcount == 4){//战斗结束后返回的人物位置和方向
		int x, y, dir;
		checksum += autil.util_deint(2, &x);
		checksum += autil.util_deint(3, &y);
		checksum += autil.util_deint(4, &dir);//方向:北a(0),东北b(1),东c(2),东南d(3),南e(4),西南f(5),西g(6),西北h(7)
		autil.util_deint(5, &checksumrecv);
		if (checksum != checksumrecv)
			return CHECKSUM_ERROR;
		charotherinfo.x = x;
		charotherinfo.y = y;
		charotherinfo.direction = dir;
		if (IsDispInfoOnTime){
			szMsg.Format("%s|%d|%s|%d,%d", chardetail.name, mapinfo.floor, mapinfo.floorname, charotherinfo.x, charotherinfo.y);
			para.nNotityType = NOTIFY_MAP;
			para.lpNotifyData = (LPVOID)(LPCTSTR)szMsg;
			csSingal.Lock();
			Callback_Notify(&para);
			csSingal.Unlock();
		}
	}
	else if (func == 32 && fieldcount == 2){//物品变化信息,如收到或丢弃
		checksum += autil.util_destring(2, message);
		autil.util_deint(3, &checksumrecv);
		if (checksum != checksumrecv)
			return CHECKSUM_ERROR;
		func32(message);
	}
	else if (func == 34 && fieldcount == 3){//服务端发回的移动物品信息
		int from, to;
		checksum += autil.util_deint(2, &from);		//物品位置
		checksum += autil.util_deint(3, &to);		//移动到物品位置
		autil.util_deint(4, &checksumrecv);
		if (checksum != checksumrecv)
			return CHECKSUM_ERROR;
		if (from >= 0 && to >= 0){
			func34(from, to);
		}
	}
	else if (func == 22 && fieldcount == 2){//由服务端返回的战斗胜利后的信息
		checksum += autil.util_destring(2, message);//-2|人物是否升级|获得经验,第几只宠物|是否升级|获得经验,第几只宠物|是否升级|获得经验,,,获得物品1|获得物品2|获得物品3|获得物品4
		autil.util_deint(3, &checksumrecv);
		if (checksum != checksumrecv)
			return CHECKSUM_ERROR;
		if (IsDispTalk){
			char msg[255];
			Parse_BE_String(message, msg);
			szMsg.Format("%s|%s", chardetail.name, msg);
			para.nNotityType = NOTIFY_MSG;
			para.lpNotifyData = (LPVOID)(LPCTSTR)szMsg;
			csSingal.Lock();
			Callback_Notify(&para);
			csSingal.Unlock();
		}
	}
	else if (func == 25 && fieldcount == 2){//战斗中敌我双方队伍信息
		checksum += autil.util_destring(2, message);
		autil.util_deint(3, &checksumrecv);
		if (checksum != checksumrecv)
			return CHECKSUM_ERROR;
		func25(message);
	}
	else
		return INVALID_INFO;
	return SUCCESSFUL;
}

//处理物品变化信息,如收到或丢弃
void CDpMain::func32(char *message)
{
	int id, pos;
	char buf[1024], *p;
	pos = 0;
	ZeroMemory(buf, sizeof(buf));
	p = message;
	Tokenize(p, buf, "|", pos);
	id = atoi(buf);//物品位置
	Tokenize(p, buf, "|", pos);
	if (strlen(buf) >= 1){	//物品名称不为空，是有物品
		strcpy_s(iteminfo[id].name, buf);
		Tokenize(p, buf, "|", pos);
		strcpy_s(iteminfo[id].paramshow, buf);
		Tokenize(p, buf, "|", pos);
		iteminfo[id].color = atoi(buf);
		Tokenize(p, buf, "|", pos);
		strcpy_s(iteminfo[id].comment, buf);
		Tokenize(p, buf, "|", pos);
		iteminfo[id].imgno = atoi(buf);
		Tokenize(p, buf, "|", pos);
		iteminfo[id].ableusefield = atoi(buf);
		Tokenize(p, buf, "|", pos);
		iteminfo[id].target = atoi(buf);
		Tokenize(p, buf, "|", pos);
		iteminfo[id].level = atoi(buf);
		Tokenize(p, buf, "|", pos);
		iteminfo[id].flg = atoi(buf);
		Tokenize(p, buf, "|", pos);
		strcpy_s(iteminfo[id].damagecrushe, buf);
		Tokenize(p, buf, "|", pos);
		iteminfo[id].pilenums = atoi(buf);
		Tokenize(p, buf, "|", pos);
		strcpy_s(iteminfo[id].element, buf);
		Tokenize(p, buf, "|", pos);
		iteminfo[id].itemlevel = atoi(buf);
	}
	else{//失去物品，物品名称置为空
		ZeroMemory(iteminfo[id].name, sizeof(iteminfo[id].name));
		ZeroMemory(iteminfo[id].comment, sizeof(iteminfo[id].comment));
	}
	//吃补血肉
	if (bEatMeat && charotherinfo.state == 0){
		SendEatMeat();
	}
	//丢没有补血功能的血肉
	if (bDiscardMeat && charotherinfo.state == 0){
		SendDiscardMeat();
	}
	//丢弃包含
	if (nDiscardLikeItemNum>0 && charotherinfo.state == 0){
		SendDiscardLikeItem();
	}
	//丢弃不包含
	if (nDiscardExceptItemNum>0 && charotherinfo.state == 0){
		SendDiscardExceptItem();
	}
	//自动堆叠
	if (bAutoPile  && charotherinfo.state == 0)
		SendAutoPile();
}

//处理服务端发回的移动物品信息
void CDpMain::func34(int from, int to)
{

	ITEMINFO t;
	ZeroMemory(t.name, sizeof(t.name));
	ZeroMemory(t.comment, sizeof(t.comment));
	if (strlen(iteminfo[to].name)>0){
		t.ableusefield = iteminfo[to].ableusefield;
		t.color = iteminfo[to].color;
		strcpy_s(t.comment, iteminfo[to].comment);
		strcpy_s(t.damagecrushe, iteminfo[to].damagecrushe);
		strcpy_s(t.element, iteminfo[to].element);
		t.flg = iteminfo[to].flg;
		t.imgno = iteminfo[to].imgno;
		t.itemlevel = iteminfo[to].itemlevel;
		t.level = iteminfo[to].level;
		strcpy_s(t.name, iteminfo[to].name);
		strcpy_s(t.paramshow, iteminfo[to].paramshow);
		t.pilenums = iteminfo[to].pilenums;
		t.target = iteminfo[to].target;
		strcpy_s(t.unknown, iteminfo[to].unknown);
	}
	if (strlen(iteminfo[from].name)>0){
		iteminfo[to].ableusefield = iteminfo[from].ableusefield;
		iteminfo[to].color = iteminfo[from].color;
		strcpy_s(iteminfo[to].comment, iteminfo[from].comment);
		strcpy_s(iteminfo[to].damagecrushe, iteminfo[from].damagecrushe);
		strcpy_s(iteminfo[to].element, iteminfo[from].element);
		iteminfo[to].flg = iteminfo[from].flg;
		iteminfo[to].imgno = iteminfo[from].imgno;
		iteminfo[to].itemlevel = iteminfo[from].itemlevel;
		iteminfo[to].level = iteminfo[from].level;
		strcpy_s(iteminfo[to].name, iteminfo[from].name);
		strcpy_s(iteminfo[to].paramshow, iteminfo[from].paramshow);
		iteminfo[to].pilenums = iteminfo[from].pilenums;
		iteminfo[to].target = iteminfo[from].target;
		strcpy_s(iteminfo[to].unknown, iteminfo[from].unknown);
	}
	if (strlen(t.name)>0){
		iteminfo[from].ableusefield = t.ableusefield;
		iteminfo[from].color = t.color;
		strcpy_s(iteminfo[from].comment, t.comment);
		strcpy_s(iteminfo[from].damagecrushe, t.damagecrushe);
		strcpy_s(iteminfo[from].element, t.element);
		iteminfo[from].flg = t.flg;
		iteminfo[from].imgno = t.imgno;
		iteminfo[from].itemlevel = t.itemlevel;
		iteminfo[from].level = t.level;
		strcpy_s(iteminfo[from].name, t.name);
		strcpy_s(iteminfo[from].paramshow, t.paramshow);
		iteminfo[from].pilenums = t.pilenums;
		iteminfo[from].target = t.target;
		strcpy_s(iteminfo[from].unknown, t.unknown);
	}
	else{
		ZeroMemory(iteminfo[from].name, sizeof(iteminfo[from].name));
		ZeroMemory(iteminfo[from].comment, sizeof(iteminfo[from].comment));
	}
}

void CDpMain::func25(char *message)
{
	CString szMsg, szStr;
	NOTIFYPARA para;
	int pos, i, j, position;
	char buf[1024], *p;
	pos = 0;
	ZeroMemory(buf, sizeof(buf));
	Tokenize(message, buf, "|", pos);
	if (strcmp(buf, "BC") == 0){
		//取战场属性
		Tokenize(message, buf, "|", pos);
		int field_attr = atoi(buf);
		p = message + pos;
		Parse_BC_StatusString(p);
		//人物在队伍中的位置
		position = 0;
		for (i = 0; i<5; i++){
			if (strlen(bc_char[i].name)>0 && strcmp(bc_char[i].name, chardetail.name) == 0){
				position = i;
				break;
			}
		}
		//检测战宠是否休息,被打飞
		if (nLockFightPet >= 1 && nLockFightPet <= 5){
			if (bc_char[position].petpos == -1){//被打飞则置位
				int oldpet = charotherinfo.fightpet;
				if (oldpet >= 0)
					petdetail[oldpet].state = REST;
				charotherinfo.fightpet = -1;
			}
		}
		//检查战斗是否已结束，敌是否全部死亡
		for (i = 0; i<10; i++){
			if (bc_enemy[i].pos != -1 && bc_enemy[i].hp>0 || bc_enemy[i].enemystate == BC_FLG_HIDE)
				break;
		}
		//检测人物是否全部死亡
		for (j = 0; j<5; j++){
			if (bc_char[j].pos != -1 && bc_char[j].hp>0)
				break;
		}
		if (i >= 10 || j >= 5){//全部死亡置平时状态
			charotherinfo.state = 0;
			SendDummy();		//此函数的位置非常重要，关系到自动加点、自动吃肉、自动叠加等
			bIsSendDummy = TRUE;
		}
		else{
			if (!bAutoEscape){
				charotherinfo.state = 1;
			}
		}
	}
	else if (strcmp(buf, "BP") == 0){//BP|未知|BP_FLG_JOIN|剩余MP,和求救有关
		Tokenize(message, buf, "|", pos);
		Tokenize(message, buf, "|", pos);
		Tokenize(message, buf, "|", pos);
		chardetail.mp = HexStrToDec(buf);
	}
	else if (strcmp(buf, "BA") == 0){//当前是第几回合，BA|endBlt（哪些位置上有敌人及我方哪些位置上已发动了攻击）|当前是第几回合
		//敌人前排从上到下依次为(13，11，F，10，12)，敌人后排从上到下为(E,C,A,B,D)
		//我方宠物从上到下依次为（9，7，5，6，8），我方人员从上到下为(4,2,0,1,3)
		Tokenize(message, buf, "|", pos);
		int endblt = HexStrToDec(buf);
		if (endblt>0){//为0表示已没有敌人战斗结束,什么也不做
			//从endblt中分析出我方哪些人和宠物已进攻,形式如0|5|;enemy返回敌方哪些位置上有人,如F|10|
			char chushou[100], enemy[100];
			Parse_BA_String(endblt, chushou, enemy);
			Tokenize(message, buf, "|", pos);
			int round = HexStrToDec(buf);
			charotherinfo.round = round + 1;
			//发送本回合信息data+round
			if (IsDispInfoOnTime){
				szMsg.Format("%s|%d", chardetail.name, charotherinfo.round);
				para.nNotityType = NOTIFY_ROUND;
				para.lpNotifyData = (LPVOID)(LPCTSTR)szMsg;
				csSingal.Lock();
				Callback_Notify(&para);
				csSingal.Unlock();
			}
			//本回合我方还未出手,发送进攻指令
			if (charotherinfo.state == 1 && strlen(chushou) == 0){
				SendAttackCommand(round, enemy);
			}
			else{

			}
		}
	}
}

//客户端向服务端每隔30秒发送一次在线信息，以便让服务端知道客户端仍在线上，客户端发送的信息为hoge,战斗时每回合后发送"????"
int CDpMain::SendOnlineInfo(char *info)
{
	char buffer[1024];
	int checksum = 0;

	nStartTime = GetTickCount();
	checksum = 0;
	ZeroMemory(buffer, sizeof(buffer));
	checksum += autil.util_mkstring(buffer, info);
	autil.util_mkint(buffer, checksum);
	if (!autil.util_SendMesg(&socket, 87, buffer)){
		IsOnLine = FALSE;
		return SENDMSG_ERROR;
	}
	return SUCCESSFUL;
}

//向服务端发送更新司服器数据命令
int CDpMain::SendEOCommand()
{
	charotherinfo.x = 0;
	charotherinfo.y = 0;
	charotherinfo.direction = -1;
	if (SendDummy() != SUCCESSFUL){
		IsOnLine = FALSE;
		return SENDMSG_ERROR;
	}
	if (SendOnlineInfo("!!!!") != SUCCESSFUL){
		IsOnLine = FALSE;
		return SENDMSG_ERROR;
	}
	return SUCCESSFUL;
}

//设置菜单旗标
int CDpMain::SendMenuFlag(int flg)
{
	char buffer[1024];
	int checksum = 0;

	checksum = 0;
	ZeroMemory(buffer, sizeof(buffer));
	checksum += autil.util_mkint(buffer, flg);
	autil.util_mkint(buffer, checksum);
	if (!autil.util_SendMesg(&socket, 48, buffer)){
		//IsOnLine=FALSE;
		return SENDMSG_ERROR;
	}

	return SUCCESSFUL;
}

//设置原地登出或返回记录点，flg=1为返回记录点
int CDpMain::SendLogFlag(int flg)
{
	char buffer[1024];
	int checksum = 0;

	checksum = 0;
	ZeroMemory(buffer, sizeof(buffer));
	autil.util_Init();
	checksum += autil.util_mkint(buffer, flg);
	autil.util_mkint(buffer, checksum);
	if (!autil.util_SendMesg(&socket, 81, buffer)){
		IsOnLine = FALSE;
		return SENDMSG_ERROR;
	}

	return SUCCESSFUL;
}

int CDpMain::SendDummy()
{
	char buffer[1024];
	int checksum = 0;

	checksum = 0;
	ZeroMemory(buffer, sizeof(buffer));
	checksum += autil.util_mkint(buffer, 0);
	autil.util_mkint(buffer, checksum);
	if (!autil.util_SendMesg(&socket, 8, buffer)){
		IsOnLine = FALSE;
		return SENDMSG_ERROR;
	}

	return SUCCESSFUL;
}

#pragma region 战斗

//服务端发送的静态信息，可用于显示玩家，其它玩家，公交，宠物等信息，参见CHAR_TYPE结构;
//格式：1|CHAR_WHICHTYPE|NPC编号|X坐标|Y坐标|人物方向|人物形象|人物等级|名称颜色|人物名称|人物称号|CHAR_ISOVERED|CHAR_HAVEHEIGHT|英雄称号颜色|家族名|骑宠名|骑宠等级|0|0|0|0
void CDpMain::Parse_NPC_String(char *data)
{
	int i, pos, k;
	char buf[255] = { 0 }, npc[255] = { 0 };
	//清空结构
	for (i = 0; i<STATIC_NPC_NUM; i++){
		npcinfo[i].charwhichtype = -1;
		ZeroMemory(npcinfo[i].name, sizeof(npcinfo[i].name));
	}
	pos = 0;
	i = 0;
	Tokenize(data, npc, ",", pos);
	while (strlen(npc)>0){
		k = 0;
		Tokenize(npc, buf, "|", k);
		npcinfo[i].unknown = atoi(buf);
		Tokenize(npc, buf, "|", k);
		npcinfo[i].charwhichtype = atoi(buf);
		Tokenize(npc, buf, "|", k);
		npcinfo[i].objindex = autil.lssproto_a62toi(buf);
		Tokenize(npc, buf, "|", k);
		npcinfo[i].x = atoi(buf);
		Tokenize(npc, buf, "|", k);
		npcinfo[i].y = atoi(buf);
		Tokenize(npc, buf, "|", k);
		npcinfo[i].dir = atoi(buf);
		Tokenize(npc, buf, "|", k);
		npcinfo[i].faceimg = atoi(buf);
		Tokenize(npc, buf, "|", k);
		npcinfo[i].level = atoi(buf);
		Tokenize(npc, buf, "|", k);
		npcinfo[i].namecolor = atoi(buf);
		Tokenize(npc, buf, "|", k);
		strcpy_s(npcinfo[i].name, buf);
		Tokenize(npc, buf, "|", k);
		strcpy_s(npcinfo[i].owntitle, buf);
		Tokenize(npc, buf, "|", k);
		npcinfo[i].isovered = atoi(buf);
		Tokenize(npc, buf, "|", k);
		npcinfo[i].haveheight = atoi(buf);
		Tokenize(npc, buf, "|", k);
		npcinfo[i].popupnamecolor = atoi(buf);
		Tokenize(npc, buf, "|", k);
		strcpy_s(npcinfo[i].familyname, buf);
		Tokenize(npc, buf, "|", k);
		strcpy_s(npcinfo[i].ridepetname, buf);
		Tokenize(npc, buf, "|", k);
		npcinfo[i].ridepetlevel = atoi(buf);
		Tokenize(npc, buf, "|", k);
		npcinfo[i].unknown1 = atoi(buf);
		Tokenize(npc, buf, "|", k);
		npcinfo[i].unknown2 = atoi(buf);
		Tokenize(npc, buf, "|", k);
		npcinfo[i].unknown3 = atoi(buf);
		Tokenize(npc, buf, "|", k);
		npcinfo[i].unknown4 = atoi(buf);
		i++;
		Tokenize(data, npc, ",", pos);
	}
}

//解析战斗结束后返回的信息
//-2|人物是否升级|获得经验,第几只宠物|是否升级|获得经验,第几只宠物|是否升级|获得经验,,,获得物品1|获得物品2|获得物品3|获得物品4
void CDpMain::Parse_BE_String(char *data, char *dst)
{
	int pos, t, id, len, up, exp;
	char buf[255] = { 0 }, tmp[255] = { 0 }, tmp1[255] = { 0 };
	pos = 0;
	ZeroMemory(dst, sizeof(dst));
	//战斗信息字符串总长度
	len = strlen(data);
	//战斗结束后返回空字符串，直接返回
	if (len == 0)
		return;
	Tokenize(data, buf, ",", pos);
	while (strlen(buf)>0 || pos<len){
		if (strlen(buf)>0){
			t = 0;
			ZeroMemory(tmp, sizeof(tmp));
			Tokenize(buf, tmp, "|", t);
			if (strlen(tmp)>0 && IsNumber(tmp)){//是数字，则说明有人物或宠物经验
				id = atoi(tmp);
				if (id == -2){//人物经验
					Tokenize(buf, tmp, "|", t);
					up = atoi(tmp);
					Tokenize(buf, tmp, "|", t);
					exp = autil.lssproto_a62toi(tmp);
					if (up == 0){
						sprintf(tmp, "人物获得经验:%d,", exp);
						strcat(dst, tmp);
					}
					else if (up == 1){
						sprintf(tmp, "人物 LevelUp 获得经验:%d,", exp);
						strcat(dst, tmp);
					}
				}
				else if (id >= 0 && id<5){//宠物经验
					Tokenize(buf, tmp, "|", t);
					up = atoi(tmp);
					Tokenize(buf, tmp, "|", t);
					exp = autil.lssproto_a62toi(tmp);
					if (up == 0){
						sprintf(tmp, "%s 获得经验:%d,", petdetail[id].oldname, exp);
						strcat(dst, tmp);
					}
					else if (up == 1){
						sprintf(tmp, "%s LevelUp 获得经验:%d,", petdetail[id].oldname, exp);
						strcat(dst, tmp);
					}
				}
			}
			else{	//获得物品			
				strcat(dst, "获得物品:");
				while (strlen(tmp)>0){
					strcat(dst, tmp);
					strcat(dst, ",");
					Tokenize(buf, tmp, "|", t);
				}
			}
		}
		Tokenize(data, buf, ",", pos);
	}
}

//data中返回我方已攻击的人和宠物位置编号，为空表示本回合我方还未出手,enemy返回敌方哪些位置上有人
void CDpMain::Parse_BA_String(int endblt, char *data, char *enemy)
{
	int i, bit, v;
	char buf[10] = { 0 };
	ZeroMemory(data, sizeof(data));
	for (i = 0; i<10; i++){
		bit = 1 << i;		//计算测试位
		v = endblt&bit;	//测试该位是否为0
		if (v){
			itoa(i, buf, 10);
			strcat(data, buf);
			strcat(data, "|");
		}
	}
	ZeroMemory(enemy, sizeof(enemy));
	for (i = 10; i<20; i++){
		bit = 1 << i;		//计算测试位
		v = endblt&bit;	//测试该位是否为0
		if (v){
			sprintf(buf, "%X", i);
			strcat(enemy, buf);
			strcat(enemy, "|");
		}
	}
}

void CDpMain::Parse_P_StatusString(char *data)
{
	char *p = data + 1;
	char buf[200];
	int pos = 0, flag, i, b;

	Tokenize(p, buf, "|", pos);
	if (strcmp(buf, "1") == 0){
		chardetail.kubun = atoi(buf);
		Tokenize(p, buf, "|", pos);
		chardetail.hp = atoi(buf);
		Tokenize(p, buf, "|", pos);
		chardetail.maxhp = atoi(buf);
		Tokenize(p, buf, "|", pos);
		chardetail.mp = atoi(buf);
		Tokenize(p, buf, "|", pos);
		chardetail.maxmp = atoi(buf);
		Tokenize(p, buf, "|", pos);
		chardetail.vital = atoi(buf);
		Tokenize(p, buf, "|", pos);
		chardetail.str = atoi(buf);
		Tokenize(p, buf, "|", pos);
		chardetail.tough = atoi(buf);
		Tokenize(p, buf, "|", pos);
		chardetail.dex = atoi(buf);
		Tokenize(p, buf, "|", pos);
		chardetail.exp = atoi(buf);
		Tokenize(p, buf, "|", pos);
		chardetail.nextexp = atoi(buf);
		Tokenize(p, buf, "|", pos);
		chardetail.level = atoi(buf);
		Tokenize(p, buf, "|", pos);
		chardetail.attack = atoi(buf);
		Tokenize(p, buf, "|", pos);
		chardetail.defence = atoi(buf);
		Tokenize(p, buf, "|", pos);
		chardetail.fQuick = atoi(buf);
		Tokenize(p, buf, "|", pos);
		chardetail.fCharm = atoi(buf);
		Tokenize(p, buf, "|", pos);
		chardetail.fLuck = atoi(buf);
		Tokenize(p, buf, "|", pos);
		chardetail.fEarth = atoi(buf);
		Tokenize(p, buf, "|", pos);
		chardetail.fWater = atoi(buf);
		Tokenize(p, buf, "|", pos);
		chardetail.fFire = atoi(buf);
		Tokenize(p, buf, "|", pos);
		chardetail.fWind = atoi(buf);
		Tokenize(p, buf, "|", pos);
		chardetail.gold = atoi(buf);
		Tokenize(p, buf, "|", pos);
		chardetail.indexofeqtitle = atoi(buf);
		Tokenize(p, buf, "|", pos);
		chardetail.dp = atoi(buf);
		Tokenize(p, buf, "|", pos);
		chardetail.transmigration = atoi(buf);
		Tokenize(p, buf, "|", pos);
		chardetail.ridepet = atoi(buf);//宠物编号
		Tokenize(p, buf, "|", pos);
		chardetail.learnride = atoi(buf);//40,80,120,200
		Tokenize(p, buf, "|", pos);
		chardetail.faceimg = atoi(buf);
		Tokenize(p, buf, "|", pos);
		chardetail.unknown = atoi(buf);
		Tokenize(p, buf, "|", pos);
		if (strlen(buf)>0)
			strcpy_s(chardetail.name, buf);
		Tokenize(p, buf, "|", pos);
		strcpy_s(chardetail.owntitle, buf);
	}
	else{
		flag = autil.lssproto_a62toi(buf);
		i = 1;
		while (true){
			b = flag >> i;
			if (b>0){
				b &= 1;
				if (b){
					switch (i){
					case 1:
						Tokenize(p, buf, "|", pos);
						chardetail.hp = atoi(buf);
						break;
					case 2:
						Tokenize(p, buf, "|", pos);
						chardetail.maxhp = atoi(buf);
						break;
					case 3:
						Tokenize(p, buf, "|", pos);
						chardetail.mp = atoi(buf);
						break;
					case 4:
						Tokenize(p, buf, "|", pos);
						chardetail.maxmp = atoi(buf);
						break;
					case 5:
						Tokenize(p, buf, "|", pos);
						chardetail.vital = atoi(buf);
						break;
					case 6:
						Tokenize(p, buf, "|", pos);
						chardetail.str = atoi(buf);
						break;
					case 7:
						Tokenize(p, buf, "|", pos);
						chardetail.tough = atoi(buf);
						break;
					case 8:
						Tokenize(p, buf, "|", pos);
						chardetail.dex = atoi(buf);
						break;
					case 9:
						Tokenize(p, buf, "|", pos);
						chardetail.exp = atoi(buf);
						break;
					case 10:
						Tokenize(p, buf, "|", pos);
						chardetail.nextexp = atoi(buf);
						break;
					case 11:
						Tokenize(p, buf, "|", pos);
						chardetail.level = atoi(buf);
						break;
					case 12:
						Tokenize(p, buf, "|", pos);
						chardetail.attack = atoi(buf);
						break;
					case 13:
						Tokenize(p, buf, "|", pos);
						chardetail.defence = atoi(buf);
						break;
					case 14:
						Tokenize(p, buf, "|", pos);
						chardetail.fQuick = atoi(buf);
						break;
					case 15:
						Tokenize(p, buf, "|", pos);
						chardetail.fCharm = atoi(buf);
						break;
					case 16:
						Tokenize(p, buf, "|", pos);
						chardetail.fLuck = atoi(buf);
						break;
					case 17:
						Tokenize(p, buf, "|", pos);
						chardetail.fEarth = atoi(buf);
						break;
					case 18:
						Tokenize(p, buf, "|", pos);
						chardetail.fWater = atoi(buf);
						break;
					case 19:
						Tokenize(p, buf, "|", pos);
						chardetail.fFire = atoi(buf);
						break;
					case 20:
						Tokenize(p, buf, "|", pos);
						chardetail.fWind = atoi(buf);
						break;
					case 21:
						Tokenize(p, buf, "|", pos);
						chardetail.gold = atoi(buf);
						break;
					case 22:
						Tokenize(p, buf, "|", pos);
						chardetail.indexofeqtitle = atoi(buf);
						break;
					case 23:
						Tokenize(p, buf, "|", pos);
						chardetail.dp = atoi(buf);
						break;
					case 24:
						Tokenize(p, buf, "|", pos);
						chardetail.transmigration = atoi(buf);
						CalcMaxLoad();
						break;
					case 25:
						Tokenize(p, buf, "|", pos);
						if (strlen(buf)>0)
							strcpy_s(chardetail.name, buf);
						break;
					case 26:
						Tokenize(p, buf, "|", pos);
						strcpy_s(chardetail.owntitle, buf);
						break;
					case 27:
						Tokenize(p, buf, "|", pos);
						chardetail.ridepet = atoi(buf);//宠物编号
						break;
					case 28:
						Tokenize(p, buf, "|", pos);
						chardetail.learnride = atoi(buf);//40,80,120,200
						break;
					case 29:
						Tokenize(p, buf, "|", pos);
						chardetail.faceimg = atoi(buf);
						break;
					}
				}
			}
			else
				break;
			i++;
		}
	}
}

void CDpMain::Parse_K_StatusString(char *data)
{
	char *p = data + 1;
	char buf[200];
	int pos = 0, flag, i, b, id;

	Tokenize(p, buf, "|", pos);
	id = atoi(buf);
	Tokenize(p, buf, "|", pos);
	if (strcmp(buf, "1") == 0){
		petdetail[id].no = id;
		petdetail[id].islive = atoi(buf);
		Tokenize(p, buf, "|", pos);
		petdetail[id].image = atoi(buf);
		Tokenize(p, buf, "|", pos);
		petdetail[id].hp = atoi(buf);
		Tokenize(p, buf, "|", pos);
		petdetail[id].maxhp = atoi(buf);
		Tokenize(p, buf, "|", pos);
		petdetail[id].mp = atoi(buf);
		Tokenize(p, buf, "|", pos);
		petdetail[id].maxmp = atoi(buf);
		Tokenize(p, buf, "|", pos);
		petdetail[id].exp = atoi(buf);
		Tokenize(p, buf, "|", pos);
		petdetail[id].nextexp = atoi(buf);
		Tokenize(p, buf, "|", pos);
		petdetail[id].level = atoi(buf);
		Tokenize(p, buf, "|", pos);
		petdetail[id].attack = atoi(buf);
		Tokenize(p, buf, "|", pos);
		petdetail[id].defence = atoi(buf);
		Tokenize(p, buf, "|", pos);
		petdetail[id].quick = atoi(buf);
		Tokenize(p, buf, "|", pos);
		petdetail[id].ai = atoi(buf);
		Tokenize(p, buf, "|", pos);
		petdetail[id].fEarth = atoi(buf);
		Tokenize(p, buf, "|", pos);
		petdetail[id].fWater = atoi(buf);
		Tokenize(p, buf, "|", pos);
		petdetail[id].fFire = atoi(buf);
		Tokenize(p, buf, "|", pos);
		petdetail[id].fWind = atoi(buf);
		Tokenize(p, buf, "|", pos);
		petdetail[id].slot = atoi(buf);
		Tokenize(p, buf, "|", pos);
		petdetail[id].changenameflag = atoi(buf);
		Tokenize(p, buf, "|", pos);
		petdetail[id].transmigration = atoi(buf);
		Tokenize(p, buf, "|", pos);
		petdetail[id].unknown = atoi(buf);
		Tokenize(p, buf, "|", pos);
		strcpy_s(petdetail[id].oldname, buf);
		Tokenize(p, buf, "|", pos);
		strcpy_s(petdetail[id].newname, buf);
	}
	else if (strcmp(buf, "0") == 0){//宠物被丢出或寄存或被买走
		ZeroMemory(petdetail[id].oldname, sizeof(petdetail[id].oldname));
		ZeroMemory(petdetail[id].newname, sizeof(petdetail[id].newname));
		petdetail[id].no = id;
		petdetail[id].islive = 0;
	}
	else{
		flag = autil.lssproto_a62toi(buf);
		i = 1;
		while (true){
			b = flag >> i;
			if (b>0){
				b &= 1;
				if (b){
					switch (i){
					case 1:
						Tokenize(p, buf, "|", pos);
						petdetail[id].image = atoi(buf);
						break;
					case 2:
						Tokenize(p, buf, "|", pos);
						petdetail[id].hp = atoi(buf);
						break;
					case 3:
						Tokenize(p, buf, "|", pos);
						petdetail[id].maxhp = atoi(buf);
						break;
					case 4:
						Tokenize(p, buf, "|", pos);
						petdetail[id].mp = atoi(buf);
						break;
					case 5:
						Tokenize(p, buf, "|", pos);
						petdetail[id].maxmp = atoi(buf);
						break;
					case 6:
						Tokenize(p, buf, "|", pos);
						petdetail[id].exp = atoi(buf);
						break;
					case 7:
						Tokenize(p, buf, "|", pos);
						petdetail[id].nextexp = atoi(buf);
						break;
					case 8:
						Tokenize(p, buf, "|", pos);
						petdetail[id].level = atoi(buf);
						break;
					case 9:
						Tokenize(p, buf, "|", pos);
						petdetail[id].attack = atoi(buf);
						break;
					case 10:
						Tokenize(p, buf, "|", pos);
						petdetail[id].defence = atoi(buf);
						break;
					case 11:
						Tokenize(p, buf, "|", pos);
						petdetail[id].quick = atoi(buf);
						break;
					case 12:
						Tokenize(p, buf, "|", pos);
						petdetail[id].ai = atoi(buf);
						break;
					case 13:
						Tokenize(p, buf, "|", pos);
						petdetail[id].fEarth = atoi(buf);
						break;
					case 14:
						Tokenize(p, buf, "|", pos);
						petdetail[id].fWater = atoi(buf);
						break;
					case 15:
						Tokenize(p, buf, "|", pos);
						petdetail[id].fFire = atoi(buf);
						break;
					case 16:
						Tokenize(p, buf, "|", pos);
						petdetail[id].fWind = atoi(buf);
						break;
					case 17:
						Tokenize(p, buf, "|", pos);
						petdetail[id].slot = atoi(buf);
						break;
					case 18:
						Tokenize(p, buf, "|", pos);
						petdetail[id].changenameflag = atoi(buf);
						break;
					case 19:
						Tokenize(p, buf, "|", pos);
						strcpy_s(petdetail[id].oldname, buf);
						break;
					case 20:
						Tokenize(p, buf, "|", pos);
						strcpy_s(petdetail[id].newname, buf);
						break;
					}
				}
			}
			else
				break;
			i++;
		}
	}
}

//BC|战场属性（0:无属性,1:地,2:水,3:火,4:风）|人物在组队中的位置|人物名称|人物称号|人物形象编号|人物等级(16进制)|当前HP|最大HP|人物状态（死亡，中毒等）|是否骑乘标志(0:未骑，1骑,-1落马)|骑宠名称|骑宠等级|骑宠HP|骑宠最大HP|战宠在队伍中的位置|战宠名称|未知|战宠形象|战宠等级|战宠HP|战宠最大HP|战宠异常状态（昏睡，死亡，中毒等）|0||0|0|0|
//敌1位置|敌1名称|未知|敌1形象|敌1等级|敌1HP|敌1最大HP|敌人异常状态（死亡，中毒等）|0||0|0|0|
void CDpMain::Parse_BC_StatusString(char *data)
{
	char buf[200] = { 0 };
	int pos = 0, id, len, i;
	len = strlen(data);
	//初始化玩家队伍信息
	for (i = 0; i<10; i++){
		ZeroMemory(bc_char[i].name, sizeof(bc_char[i].name));
		ZeroMemory(bc_char[i].petname, sizeof(bc_char[i].petname));
		ZeroMemory(bc_char[i].ridepetname, sizeof(bc_char[i].ridepetname));
		bc_char[i].hp = 0;
		bc_char[i].pos = -1;
		bc_char[i].petpos = -1;
	}
	//初始化敌人队伍信息
	for (i = 0; i<10; i++){
		ZeroMemory(bc_enemy[i].name, sizeof(bc_enemy[i].name));
		bc_enemy[i].pos = -1;
		bc_enemy[i].hp = 0;
	}
	while (pos<len){
		Tokenize(data, buf, "|", pos);
		id = HexStrToDec(buf);
		if (id >= 0 && id <= 4){//我方人员信息
			bc_char[id].pos = id;
			Tokenize(data, buf, "|", pos);
			strcpy_s(bc_char[id].name, buf);
			Tokenize(data, buf, "|", pos);
			strcpy_s(bc_char[id].owntitle, buf);
			Tokenize(data, buf, "|", pos);
			bc_char[id].faceimg = HexStrToDec(buf);
			Tokenize(data, buf, "|", pos);
			bc_char[id].level = (int)HexStrToDec(buf);
			Tokenize(data, buf, "|", pos);
			bc_char[id].hp = (int)HexStrToDec(buf);
			Tokenize(data, buf, "|", pos);
			bc_char[id].maxhp = (int)HexStrToDec(buf);
			Tokenize(data, buf, "|", pos);
			bc_char[id].state = (int)HexStrToDec(buf);
			Tokenize(data, buf, "|", pos);
			bc_char[id].isride = atoi(buf);//是否骑乘标志(0:未骑，1骑,-1落马)
			Tokenize(data, buf, "|", pos);
			strcpy_s(bc_char[id].ridepetname, buf);
			Tokenize(data, buf, "|", pos);
			bc_char[id].ridepetlevel = (int)HexStrToDec(buf);
			Tokenize(data, buf, "|", pos);
			bc_char[id].ridepethp = (int)HexStrToDec(buf);
			Tokenize(data, buf, "|", pos);
			bc_char[id].ridepetmaxhp = (int)HexStrToDec(buf);
		}
		else if (id >= 5 && id <= 9){//我方宠物信息
			bc_char[id - 5].petpos = id;
			Tokenize(data, buf, "|", pos);
			strcpy_s(bc_char[id - 5].petname, buf);
			Tokenize(data, buf, "|", pos);
			strcpy_s(bc_char[id - 5].unknown, buf);
			Tokenize(data, buf, "|", pos);
			bc_char[id - 5].petimage = HexStrToDec(buf);
			Tokenize(data, buf, "|", pos);
			bc_char[id - 5].petlevel = (int)HexStrToDec(buf);
			Tokenize(data, buf, "|", pos);
			bc_char[id - 5].pethp = (int)HexStrToDec(buf);
			Tokenize(data, buf, "|", pos);
			bc_char[id - 5].petmaxhp = (int)HexStrToDec(buf);
			Tokenize(data, buf, "|", pos);
			bc_char[id - 5].petstate = (int)HexStrToDec(buf);
			Tokenize(data, buf, "|", pos);
			bc_char[id - 5].unknown2 = (int)HexStrToDec(buf);
			Tokenize(data, buf, "|", pos);
			strcpy_s(bc_char[id - 5].unknown3, buf);
			Tokenize(data, buf, "|", pos);
			bc_char[id - 5].unknown4 = (int)HexStrToDec(buf);
			Tokenize(data, buf, "|", pos);
			bc_char[id - 5].unknown5 = (int)HexStrToDec(buf);
			Tokenize(data, buf, "|", pos);
			bc_char[id - 5].unknown6 = (int)HexStrToDec(buf);
		}
		else if (id >= 10 && id <= 19){//敌人信息
			bc_enemy[id - 10].pos = id;
			Tokenize(data, buf, "|", pos);
			strcpy_s(bc_enemy[id - 10].name, buf);
			Tokenize(data, buf, "|", pos);
			strcpy_s(bc_enemy[id - 10].unknown, buf);
			Tokenize(data, buf, "|", pos);
			bc_enemy[id - 10].image = HexStrToDec(buf);
			Tokenize(data, buf, "|", pos);
			bc_enemy[id - 10].level = (int)HexStrToDec(buf);
			Tokenize(data, buf, "|", pos);
			bc_enemy[id - 10].hp = (int)HexStrToDec(buf);
			Tokenize(data, buf, "|", pos);
			bc_enemy[id - 10].maxhp = (int)HexStrToDec(buf);
			Tokenize(data, buf, "|", pos);
			bc_enemy[id - 10].enemystate = (int)HexStrToDec(buf);
			Tokenize(data, buf, "|", pos);
			bc_enemy[id - 10].unknown2 = (int)HexStrToDec(buf);
			Tokenize(data, buf, "|", pos);
			strcpy_s(bc_enemy[id - 10].unknown3, buf);
			Tokenize(data, buf, "|", pos);
			bc_enemy[id - 10].unknown4 = (int)HexStrToDec(buf);
			Tokenize(data, buf, "|", pos);
			bc_enemy[id - 10].unknown5 = (int)HexStrToDec(buf);
			Tokenize(data, buf, "|", pos);
			bc_enemy[id - 10].unknown6 = (int)HexStrToDec(buf);
		}
	}

}

//向服务端发送战斗中所使用的命令
int CDpMain::SendCommand(char *cmd)
{
	char buffer[1024];
	int checksum = 0;

	checksum = 0;
	ZeroMemory(buffer, sizeof(buffer));
	checksum += autil.util_mkstring(buffer, cmd);
	autil.util_mkint(buffer, checksum);
	if (!autil.util_SendMesg(&socket, 14, buffer)){
		IsOnLine = FALSE;
		return SENDMSG_ERROR;
	}

	return SUCCESSFUL;
}

//根据精灵名称获取释放对象
//我方宠物从上到下依次为（9，7，5，6，8），我方人员从上到下为(4,2,0,1,3)
//敌人前排从上到下依次为(13，11，F，10，12)，敌人后排从上到下为(E,C,A,B,D)
void CDpMain::GetTarget(char *src, char *dst, char *mypos, char *enemypos)
{
	if (strstr(src, "滋润的精灵") != NULL){
		strcpy(dst, mypos);
		return;
	}
	else if (strstr(src, "恩惠的精灵") != NULL){
		strcpy(dst, "14");
		return;
	}
	else if (strstr(src, "极光的精灵") != NULL || strstr(src, "奥萝拉的精灵") != NULL){
		strcpy(dst, "15");
		return;
	}
	else if (strstr(src, "彩虹的精灵") != NULL){
		strcpy(dst, enemypos);
		return;
	}
	else if (strstr(src, "守护精灵") != NULL){
		strcpy(dst, mypos);
		return;
	}
	else if (strstr(src, "光的精灵") != NULL){
		strcpy(dst, mypos);
		return;
	}
	else if (strstr(src, "镜的精灵") != NULL){
		strcpy(dst, mypos);
		return;
	}
	else if (strstr(src, "大地的精灵") != NULL){
		strcpy(dst, "16");
		return;
	}
	else if (strstr(src, "水的精灵") != NULL){
		strcpy(dst, "16");
		return;
	}
	else if (strstr(src, "火炎的精灵") != NULL){
		strcpy(dst, "16");
		return;
	}
	else if (strstr(src, "疾风的精灵") != NULL){
		strcpy(dst, "16");
		return;
	}
	else if (strstr(src, "高等净化精灵") != NULL){
		strcpy(dst, "14");
		return;
	}
	else if (strstr(src, "毒雾的精灵") != NULL){
		strcpy(dst, "15");
		return;
	}
	else if (strstr(src, "石化的精灵") != NULL){
		strcpy(dst, "15");
		return;
	}
	else if (strstr(src, "混迷的精灵") != NULL){
		strcpy(dst, "15");
		return;
	}
	else if (strstr(src, "酩酊的精灵") != NULL){
		strcpy(dst, "15");
		return;
	}
	else if (strstr(src, "昏睡的精灵") != NULL){
		strcpy(dst, "15");
		return;
	}
	else{
		strcpy(dst, "");
		return;
	}
}

//发送自动战斗指令
//J|精灵技能编号|施放对象
//I|物品位置|使用对象（14代表我方全体，15代表敌方全体，0代表自己，5代表宠物）
//H|攻击对象编号(人物攻击)，P|魔法技能编号|施放对象
//W|宠物技能编号|施放对象
//G代表人物防御，E代表人物逃跑，T代表捕获，S代表换宠
int CDpMain::SendAttackCommand(int round, char *enemy)
{
	char cmd[1024] = { 0 }, e[20] = { 0 }, a[10] = { 0 }, cenemy[512];
	int pos = 0, id;
	//如果自动逃跑被设置
	if (bAutoEscape){
		strcpy_s(cmd, "E");
		SendCommand(cmd);
		if (charotherinfo.fightpet >= 0){
			strcpy_s(cmd, "W|FF|FF");
			SendCommand(cmd);
		}
	}
	//else if(bAutoCapture){//如果捕捉被设置

	//}
	else{//否则就是战斗		
		//攻击时先攻击列表中的宠物，没有就按从上到下攻击
		//首回合有延迟
		if (round == 0 && dFirstDelay>0)
			Sleep((DWORD)dFirstDelay * 1000);
		//非首回合攻击延迟
		if (round>0 && dAttackDelay>0)
			Sleep((DWORD)dAttackDelay * 1000);
		//人物在队伍中的位置
		int position = 0;
		for (int i = 0; i<5; i++){
			if (strlen(bc_char[i].name)>0 && strcmp(bc_char[i].name, chardetail.name) == 0){
				position = i;
				break;
			}
		}
		//构造敌人名称列表
		pos = 0;
		ZeroMemory(cenemy, sizeof(cenemy));
		Tokenize(enemy, e, "|", pos);
		while (strlen(e)>0){
			id = HexStrToDec(e);
			strcat_s(cenemy, bc_enemy[id - 10].name);
			strcat_s(cenemy, "|");
			Tokenize(enemy, e, "|", pos);
		}
		//落马逃跑
		if (bFullEscape && (bc_char[position].isride == -1 || strlen(bc_char[position].ridepetname) <= 0)){
			strcpy_s(cmd, "E");
			SendCommand(cmd);
			if (charotherinfo.fightpet >= 0){
				strcpy_s(cmd, "W|FF|FF");
				SendCommand(cmd);
			}
			return 0;
		}
		//战宠死亡或未处于战斗状态则逃跑
		if (bFightpetDeadEscape && (bc_char[position].petpos == -1 || bc_char[position].pethp <= 0)){
			strcpy_s(cmd, "E");
			SendCommand(cmd);
			return 0;
		}
		//指定逃跑:攻击时遇到列表中的宠物就自动逃跑
		if (nZDEscapeNum>0){
			int i;
			for (i = 0; i<nZDEscapeNum; i++){
				if (strstr(cenemy, cZDEscape[i]) != NULL)
					break;
			}
			if (i<nZDEscapeNum){
				strcpy_s(cmd, "E");
				SendCommand(cmd);
				if (charotherinfo.fightpet >= 0){
					strcpy_s(cmd, "W|FF|FF");
					SendCommand(cmd);
				}
				return 0;
			}
		}
		//指定攻击:攻击时只攻击列表中的宠物，没有就自动逃跑
		if (round == 0 && nZDAttackNum>0){
			int i;
			for (i = 0; i<nZDAttackNum; i++){
				if (strstr(cenemy, cZDAttack[i]) != NULL)
					break;
			}
			if (i >= nZDAttackNum){
				strcpy_s(cmd, "E");
				SendCommand(cmd);
				if (charotherinfo.fightpet >= 0){
					strcpy_s(cmd, "W|FF|FF");
					SendCommand(cmd);
				}
				return 0;
			}
		}
		pos = 0;
		//嗜血补气
		if (dRMpByBlood>0 && ((double)chardetail.mp / chardetail.maxmp) <= dRMpByBlood){
			//从charskill中找出嗜血成性的技能编号
			int skillid;
			Tokenize(enemy, e, "|", pos);
			for (skillid = 0; skillid<CHAR_SKILL_NUM; skillid++){
				if (strstr(charskill[skillid].name, "嗜血成性") != NULL)
					break;
			}
			if (skillid >= CHAR_SKILL_NUM){
				sprintf_s(cmd, "H|%s", e);
				SendCommand(cmd);
				if (charotherinfo.fightpet >= 0){
					sprintf_s(cmd, "W|0|%s", e);
					SendCommand(cmd);
				}
			}
			else{
				sprintf_s(cmd, "P|%d|0", skillid);
				SendCommand(cmd);
				if (charotherinfo.fightpet >= 0){
					sprintf_s(cmd, "W|0|%s", e);
					SendCommand(cmd);
				}
			}
		}
		//首回合使用技能
		//0:攻击，1：防御，2：逃跑，3：帽子精灵，4：衣服精灵，5：武器精灵，6：左饰精灵，7：右饰精灵，8以后为技能编号(1-30)
		//技能名称，宠物攻击时所用技能编号对应PETSKILL中的技能索引编号
		else if (round == 0 && (strlen(cCharFirstAction)>0 || strlen(cPetFirstAction)>0)){
			int skillid;
			char target[10] = { 0 }, charpos[10] = { 0 };
			itoa(position, charpos, 10);
			Tokenize(enemy, e, "|", pos);
			if (strlen(cCharFirstAction)>0){
				if (strstr(cCharFirstAction, "攻击") != NULL){
					sprintf_s(cmd, "H|%s", e);
					SendCommand(cmd);
				}
				else if (strstr(cCharFirstAction, "防御") != NULL){
					strcpy_s(cmd, "G");
					SendCommand(cmd);
				}
				else if (strstr(cCharFirstAction, "帽子精灵") != NULL){
					GetTarget(magic[0].name, target, charpos, e);
					if (strlen(target)>0){
						sprintf_s(cmd, "J|0|%s", target);
						SendCommand(cmd);
					}
					else{
						sprintf_s(cmd, "H|%s", e);
						SendCommand(cmd);
					}
				}
				else if (strstr(cCharFirstAction, "衣服精灵") != NULL){
					GetTarget(magic[1].name, target, charpos, e);
					if (strlen(target)>0){
						sprintf_s(cmd, "J|1|%s", target);
						SendCommand(cmd);
					}
					else{
						sprintf_s(cmd, "H|%s", e);
						SendCommand(cmd);
					}
				}
				else if (strstr(cCharFirstAction, "武器精灵") != NULL){
					GetTarget(magic[2].name, target, charpos, e);
					if (strlen(target)>0){
						sprintf_s(cmd, "J|2|%s", target);
						SendCommand(cmd);
					}
					else{
						sprintf_s(cmd, "H|%s", e);
						SendCommand(cmd);
					}
				}
				else if (strstr(cCharFirstAction, "左饰精灵") != NULL){
					GetTarget(magic[3].name, target, charpos, e);
					if (strlen(target)>0){
						sprintf_s(cmd, "J|3|%s", target);
						SendCommand(cmd);
					}
					else{
						sprintf_s(cmd, "H|%s", e);
						SendCommand(cmd);
					}
				}
				else if (strstr(cCharFirstAction, "右饰精灵") != NULL){
					GetTarget(magic[4].name, target, charpos, e);
					if (strlen(target)>0){
						sprintf_s(cmd, "J|4|%s", target);
						SendCommand(cmd);
					}
					else{
						sprintf_s(cmd, "H|%s", e);
						SendCommand(cmd);
					}
				}
				else{
					int skillid;
					for (skillid = 0; skillid<CHAR_SKILL_NUM; skillid++){
						if (strstr(charskill[skillid].name, cCharFirstAction) != NULL)
							break;
					}
					if (skillid >= CHAR_SKILL_NUM){
						sprintf_s(cmd, "H|%s", e);
						SendCommand(cmd);
					}
					else{
						if (strstr(cCharFirstAction, "嗜血成性") != NULL || strstr(cCharFirstAction, "移形换位") != NULL){
							sprintf_s(cmd, "P|%d|0", skillid);
							SendCommand(cmd);
						}
						else if (strstr(cCharFirstAction, "火星球") != NULL){//敌人前排从上到下依次为(13，11，F，10，12)，敌人后排从上到下为(E,C,A,B,D)
							if (HexStrToDec(e) >= 0x0f && HexStrToDec(e) <= 0x13)
								sprintf_s(cmd, "P|%d|18", skillid);//前排
							else
								sprintf_s(cmd, "P|%d|17", skillid);//后排
							SendCommand(cmd);
						}
						else{
							sprintf_s(cmd, "H|%s", e);
							SendCommand(cmd);
						}
					}
				}
			}
			else{
				sprintf_s(cmd, "H|%s", e);
				SendCommand(cmd);
			}
			if (charotherinfo.fightpet >= 0){
				if (strlen(cPetFirstAction)>0){
					//找了宠物技能编号
					for (skillid = 0; skillid<7; skillid++){
						if (strstr(petdetail[charotherinfo.fightpet].petskill[skillid].name, cPetFirstAction) != NULL)
							break;
					}
					if (skillid >= 7)
						skillid = 0;
					if (strstr(petdetail[charotherinfo.fightpet].petskill[skillid].comment, "攻击全体") != NULL){
						sprintf_s(cmd, "W|%d|15", skillid);
					}
					else if (strstr(petdetail[charotherinfo.fightpet].petskill[skillid].comment, "一排") != NULL){
						if (HexStrToDec(e) >= 0x0f && HexStrToDec(e) <= 0x13)
							sprintf_s(cmd, "W|%d|18", skillid);//前排
						else
							sprintf_s(cmd, "W|%d|17", skillid);//后排							
					}
					else if (strstr(petdetail[charotherinfo.fightpet].petskill[skillid].comment, "回复") != NULL){
						sprintf_s(cmd, "W|%d|14", skillid);
					}
					else{
						sprintf_s(cmd, "W|%d|%s", skillid, e);
					}
					SendCommand(cmd);
				}
				else{
					sprintf_s(cmd, "W|0|%s", e);
					SendCommand(cmd);
				}
			}
		}
		else{
			//补气补血复活，（14代表我方全体，15代表敌方全体）
			//人物精灵补
			pos = 0;
			if (recruitblood.id >= 0 && (bc_char[position].hp>0 && ((double)bc_char[position].hp / bc_char[position].maxhp) * 100 <= recruitblood.val || strlen(bc_char[position].ridepetname)>0 && bc_char[position].ridepethp>0 && bc_char[position].isride == 1 && ((double)bc_char[position].ridepethp / bc_char[position].ridepetmaxhp) * 100 <= recruitblood.val)){
				if (strstr(recruitblood.name, "恩惠的精灵") != NULL){
					sprintf_s(cmd, "J|%d|14", recruitblood.id);
					SendCommand(cmd);
				}
				else if (strstr(recruitblood.name, "滋润的精灵") != NULL){
					sprintf_s(cmd, "J|%d|%d", recruitblood.id, position);
					SendCommand(cmd);
				}
				else{
					Tokenize(enemy, e, "|", pos);
					sprintf_s(cmd, "H|%s", e);
					SendCommand(cmd);
				}
			}
			else if (recruitblood.id >= 0 && strlen(bc_char[position].petname)>0 && bc_char[position].pethp>0 && ((double)bc_char[position].pethp / bc_char[position].petmaxhp) * 100 <= recruitblood.val){
				if (strstr(recruitblood.name, "恩惠的精灵") != NULL){
					sprintf_s(cmd, "J|%d|14", recruitblood.id);
					SendCommand(cmd);
				}
				else if (strstr(recruitblood.name, "滋润的精灵") != NULL){
					sprintf_s(cmd, "J|%d|%d", recruitblood.id, position + 5);
					SendCommand(cmd);
				}
				else{
					Tokenize(enemy, e, "|", pos);
					sprintf_s(cmd, "H|%s", e);
					SendCommand(cmd);
				}
			}
			else{//平时人物技能
				//int skillid;
				char target[10] = { 0 }, charpos[10] = { 0 };
				itoa(position, charpos, 10);
				Tokenize(enemy, e, "|", pos);
				if (strlen(cCharAction)>0){
					if (strstr(cCharAction, "攻击") != NULL){
						sprintf_s(cmd, "H|%s", e);
						SendCommand(cmd);
					}
					else if (strstr(cCharAction, "防御") != NULL){
						strcpy_s(cmd, "G");
						SendCommand(cmd);
					}
					else if (strstr(cCharAction, "帽子精灵") != NULL){
						GetTarget(magic[0].name, target, charpos, e);
						if (strlen(target)>0){
							sprintf_s(cmd, "J|0|%s", target);
							SendCommand(cmd);
						}
						else{
							sprintf_s(cmd, "H|%s", e);
							SendCommand(cmd);
						}
					}
					else if (strstr(cCharAction, "衣服精灵") != NULL){
						GetTarget(magic[1].name, target, charpos, e);
						if (strlen(target)>0){
							sprintf_s(cmd, "J|1|%s", target);
							SendCommand(cmd);
						}
						else{
							sprintf_s(cmd, "H|%s", e);
							SendCommand(cmd);
						}
					}
					else if (strstr(cCharAction, "武器精灵") != NULL){
						GetTarget(magic[2].name, target, charpos, e);
						if (strlen(target)>0){
							sprintf_s(cmd, "J|2|%s", target);
							SendCommand(cmd);
						}
						else{
							sprintf_s(cmd, "H|%s", e);
							SendCommand(cmd);
						}
					}
					else if (strstr(cCharAction, "左饰精灵") != NULL){
						GetTarget(magic[3].name, target, charpos, e);
						if (strlen(target)>0){
							sprintf_s(cmd, "J|3|%s", target);
							SendCommand(cmd);
						}
						else{
							sprintf_s(cmd, "H|%s", e);
							SendCommand(cmd);
						}
					}
					else if (strstr(cCharAction, "右饰精灵") != NULL){
						GetTarget(magic[4].name, target, charpos, e);
						if (strlen(target)>0){
							sprintf_s(cmd, "J|4|%s", target);
							SendCommand(cmd);
						}
						else{
							sprintf_s(cmd, "H|%s", e);
							SendCommand(cmd);
						}
					}
					else{//人物技能
						int skillid;
						for (skillid = 0; skillid<CHAR_SKILL_NUM; skillid++){
							if (strstr(charskill[skillid].name, cCharAction) != NULL)
								break;
						}
						if (skillid >= CHAR_SKILL_NUM){
							sprintf_s(cmd, "H|%s", e);
							SendCommand(cmd);
						}
						else{
							if (strstr(cCharAction, "嗜血成性") != NULL || strstr(cCharAction, "移形换位") != NULL){
								sprintf_s(cmd, "P|%d|0", skillid);
								SendCommand(cmd);
							}
							else if (strstr(cCharAction, "火星球") != NULL){//敌人前排从上到下依次为(13，11，F，10，12)，敌人后排从上到下为(E,C,A,B,D)
								if (HexStrToDec(e) >= 0x0f && HexStrToDec(e) <= 0x13)
									sprintf_s(cmd, "P|%d|18", skillid);//前排
								else
									sprintf_s(cmd, "P|%d|17", skillid);//后排
								SendCommand(cmd);
							}
							else{
								sprintf_s(cmd, "H|%s", e);
								SendCommand(cmd);
							}
						}
					}
				}
				else{
					sprintf_s(cmd, "H|%s", e);
					SendCommand(cmd);
				}
			}
			//宠物补血
			if (charotherinfo.fightpet >= 0){
				if (petrecruitblood.skillid >= 0 && bc_char[position].hp>0 && (((double)bc_char[position].hp / bc_char[position].maxhp) * 100 <= petrecruitblood.val ||
					bc_char[position].ridepethp>0 && bc_char[position].isride == 1 && ((double)bc_char[position].ridepethp / bc_char[position].ridepetmaxhp) * 100 <= petrecruitblood.val ||
					bc_char[position].pethp>0 && ((double)bc_char[position].pethp / bc_char[position].petmaxhp) * 100 <= petrecruitblood.val)){
					sprintf_s(cmd, "W|%d|14", petrecruitblood.skillid);
					SendCommand(cmd);
				}
				else{//平时宠 物技能					
					if (strlen(cPetAction)>0){
						int skillid;
						//找了宠物技能编号
						for (skillid = 0; skillid<7; skillid++){
							if (strstr(petdetail[charotherinfo.fightpet].petskill[skillid].name, cPetAction) != NULL)
								break;
						}
						if (skillid >= 7)
							skillid = 0;
						if (strstr(petdetail[charotherinfo.fightpet].petskill[skillid].comment, "攻击全体") != NULL){
							sprintf_s(cmd, "W|%d|15", skillid);
						}
						else if (strstr(petdetail[charotherinfo.fightpet].petskill[skillid].comment, "一排") != NULL){
							if (HexStrToDec(e) >= 0x0f && HexStrToDec(e) <= 0x13)
								sprintf_s(cmd, "W|%d|18", skillid);//前排
							else
								sprintf_s(cmd, "W|%d|17", skillid);//后排							
						}
						else if (strstr(petdetail[charotherinfo.fightpet].petskill[skillid].comment, "回复") != NULL){
							sprintf_s(cmd, "W|%d|14", skillid);
						}
						else{
							sprintf_s(cmd, "W|%d|%s", skillid, e);
						}
						SendCommand(cmd);
					}
					else{
						sprintf_s(cmd, "W|0|%s", e);
						SendCommand(cmd);
					}
				}
			}
		}
	}
	return 0;
}

#pragma endregion

#pragma region 宠物

//发送给服务端第几只为战宠0-4，-1没有战宠
int CDpMain::SendFightPet(int pet)
{
	char buffer[1024];
	int checksum = 0;

	checksum = 0;
	ZeroMemory(buffer, sizeof(buffer));
	checksum += autil.util_mkint(buffer, pet);
	autil.util_mkint(buffer, checksum);
	if (!autil.util_SendMesg(&socket, 54, buffer)){
		IsOnLine = FALSE;
		return SENDMSG_ERROR;
	}

	return SUCCESSFUL;
}

//设置宠物等待状态
//1:宠物1处于等待状态,2:宠物2处于等待状态，4：宠物3处于等待状态，8：宠物4处于等待状态，16：宠物5处于等待状态，这些值可相互组合，如5代表宠物1和宠物3处于等待状态
int CDpMain::SendStandbyPet(int pet)
{
	char buffer[1024];
	int checksum = 0;

	checksum = 0;
	ZeroMemory(buffer, sizeof(buffer));
	checksum += autil.util_mkint(buffer, pet);
	autil.util_mkint(buffer, checksum);
	if (!autil.util_SendMesg(&socket, 114, buffer)){
		IsOnLine = FALSE;
		return SENDMSG_ERROR;
	}

	return SUCCESSFUL;
}

//设置当所有宠物的状态
int CDpMain::SendPetState()
{
	char buffer[1024], temp[4096];
	int checksum = 0, ret, nSize, len = 0;

	for (int i = 0; i<5; i++){
		checksum = 0;
		ZeroMemory(temp, sizeof(temp));
		checksum += autil.util_mkint(temp, i);
		checksum += autil.util_mkint(temp, petdetail[i].state);
		autil.util_mkint(temp, checksum);
		nSize = autil.util_EncodeMesg(96, temp);
		if (len == 0)
			strcpy_s(buffer, temp);
		else
			strcat_s(buffer, temp);
		len += nSize;
	}
	ret = send(socket, buffer, len, 0);
	if (ret != len){
		IsOnLine = FALSE;
		return SENDMSG_ERROR;
	}
	return SUCCESSFUL;
}

//设置宠物状态，0休息，1战斗或等待,4邮件
int CDpMain::SendPetState(int pet, int state)
{
	char buffer[1024];
	int checksum = 0, standby = 0;

	if (charotherinfo.fightpet == pet)
		SendFightPet(-1);
	checksum = 0;
	ZeroMemory(buffer, sizeof(buffer));
	checksum += autil.util_mkint(buffer, pet);
	checksum += autil.util_mkint(buffer, state);
	autil.util_mkint(buffer, checksum);
	if (!autil.util_SendMesg(&socket, 96, buffer)){
		IsOnLine = FALSE;
		return SENDMSG_ERROR;
	}
	petdetail[pet].state = state;
	for (int i = 0; i<5; i++){
		if (petdetail[i].state == FIGHTSTANDBY)
			standby += 1 << i;
	}
	SendStandbyPet(standby);
	return SUCCESSFUL;
}

//设置骑剩宠物，"R|P|宠物编号0-4"，"R|P|-1"取消骑剩
int CDpMain::SendRidePet(int pet)
{
	char buffer[1024], buf[255];
	int checksum = 0;

	checksum = 0;
	ZeroMemory(buffer, sizeof(buffer));
	ZeroMemory(buf, sizeof(buf));
	sprintf_s(buf, "R|P|%d", pet);
	checksum += autil.util_mkstring(buffer, buf);
	autil.util_mkint(buffer, checksum);
	if (!autil.util_SendMesg(&socket, 94, buffer)){
		IsOnLine = FALSE;
		return SENDMSG_ERROR;
	}

	return SUCCESSFUL;
}

//发送丢弃宠物信息
int CDpMain::SendDiscardPet(int pet)
{
	char buffer[1024];
	int checksum = 0;

	checksum = 0;
	ZeroMemory(buffer, sizeof(buffer));
	checksum += autil.util_mkint(buffer, charotherinfo.x);
	checksum += autil.util_mkint(buffer, charotherinfo.y);
	checksum += autil.util_mkint(buffer, pet);
	autil.util_mkint(buffer, checksum);
	if (!autil.util_SendMesg(&socket, 21, buffer)){
		IsOnLine = FALSE;
		return SENDMSG_ERROR;
	}

	return SUCCESSFUL;
}

//料理合成，petid料理宠物编号(0-4),petskillid技能编号(1-7),toindex=0,data为要料理的对象编号(9-23)
int CDpMain::SendMerge(int petid, int petskillid, int toindex, char *data)
{
	char buffer[1024];
	int checksum = 0;

	checksum = 0;
	ZeroMemory(buffer, sizeof(buffer));
	checksum += autil.util_mkint(buffer, petid);
	checksum += autil.util_mkint(buffer, petskillid);
	checksum += autil.util_mkint(buffer, toindex);
	checksum += autil.util_mkstring(buffer, data);
	autil.util_mkint(buffer, checksum);
	if (!autil.util_SendMesg(&socket, 58, buffer)){
		IsOnLine = FALSE;
		return SENDMSG_ERROR;
	}

	return SUCCESSFUL;
}

//料理合成，ll {宠物编号1-5},{技能名称|技能1-7},道具列表(道具1|道具2...)     '料理命令
BOOL CDpMain::LiaoLi(CString para)
{
	CString szpetid, szskillid, szdata;
	int petid, skillid, pos;
	pos = 0;
	szpetid = para.Tokenize(",", pos);
	if (szpetid.IsEmpty())
		return FALSE;
	petid = atoi(szpetid);
	if (petid<1 || petid>5)
		return FALSE;
	szskillid = para.Tokenize(",", pos);
	if (szskillid.IsEmpty())
		return FALSE;
	skillid = atoi(szskillid);
	if (skillid<1 || skillid>7)
		return FALSE;
	szdata = para.Tokenize(",", pos);
	if (szdata.IsEmpty())
		return FALSE;
	if (SendMerge(petid - 1, skillid, 0, (LPSTR)(LPCTSTR)szdata) != SUCCESSFUL)
		return FALSE;
	return TRUE;
}


#pragma endregion

#pragma region 走路说话

//把说话信息加入队列
void CDpMain::TalkMsgToDeque(int whotalk, char *msg, int color)
{
	char *p;
	p = msg + 4;
	TALKMESSAGE *tk = new TALKMESSAGE[1];
	tk->whotalk = whotalk;
	strcpy_s(tk->message, p);
	tk->color = color;
	if (talkmessage.size()>MAXTALKLENGTH){
		TALKMESSAGE *p = talkmessage.front();
		delete p;
		talkmessage.pop_front();
	}
	talkmessage.push_back(tk);
}

//释放talk所占用的空间
void CDpMain::ReleaseTalkDeque()
{
	while (talkmessage.size() != 0){
		TALKMESSAGE *p = talkmessage.front();
		delete p;
		talkmessage.pop_front();
	}
}

//发送人物走路信息,x,y为当前人物位置，direction为走路的方向，每次最多走两步
int CDpMain::SendWalkPos(int x, int y, char *direction)
{
	char buffer[1024];
	int checksum = 0;

	checksum = 0;
	ZeroMemory(buffer, sizeof(buffer));
	checksum += autil.util_mkint(buffer, x);
	checksum += autil.util_mkint(buffer, y);
	checksum += autil.util_mkstring(buffer, direction);
	autil.util_mkint(buffer, checksum);
	if (!autil.util_SendMesg(&socket, 1, buffer)){
		IsOnLine = FALSE;
		return SENDMSG_ERROR;
	}

	return SUCCESSFUL;
}

//人物转向:北A,东北B,东C,东南D,南E,西南F,西G,西北H
int CDpMain::SendWalkDirection(int x, int y, char *direction)
{
	char buffer[1024];
	int checksum = 0;

	checksum = 0;
	ZeroMemory(buffer, sizeof(buffer));
	checksum += autil.util_mkint(buffer, x);
	checksum += autil.util_mkint(buffer, y);
	if (strcmp(direction, "北") == 0 || strcmp(direction, "A") == 0 || strcmp(direction, "a") == 0)
		checksum += autil.util_mkstring(buffer, "A");
	else if (strcmp(direction, "东北") == 0 || strcmp(direction, "B") == 0 || strcmp(direction, "b") == 0)
		checksum += autil.util_mkstring(buffer, "B");
	else if (strcmp(direction, "东") == 0 || strcmp(direction, "C") == 0 || strcmp(direction, "c") == 0)
		checksum += autil.util_mkstring(buffer, "C");
	else if (strcmp(direction, "东南") == 0 || strcmp(direction, "D") == 0 || strcmp(direction, "d") == 0)
		checksum += autil.util_mkstring(buffer, "D");
	else if (strcmp(direction, "南") == 0 || strcmp(direction, "E") == 0 || strcmp(direction, "e") == 0)
		checksum += autil.util_mkstring(buffer, "E");
	else if (strcmp(direction, "西南") == 0 || strcmp(direction, "F") == 0 || strcmp(direction, "f") == 0)
		checksum += autil.util_mkstring(buffer, "F");
	else if (strcmp(direction, "西") == 0 || strcmp(direction, "G") == 0 || strcmp(direction, "g") == 0)
		checksum += autil.util_mkstring(buffer, "G");
	else// if(strcmp(direction,"西北")==0)
		checksum += autil.util_mkstring(buffer, "H");
	autil.util_mkint(buffer, checksum);
	if (!autil.util_SendMesg(&socket, 1, buffer)){
		IsOnLine = FALSE;
		return SENDMSG_ERROR;
	}

	return SUCCESSFUL;
}

//发送人物说话信息
int CDpMain::SendTalk(int x, int y, char *msg, int color, int area)
{
	char buffer[1024], buf[1024];
	int checksum = 0;

	checksum = 0;
	ZeroMemory(buffer, sizeof(buffer));
	ZeroMemory(buf, sizeof(buf));
	sprintf_s(buf, "P|%s", msg);
	checksum += autil.util_mkint(buffer, x);
	checksum += autil.util_mkint(buffer, y);
	checksum += autil.util_mkstring(buffer, buf);
	checksum += autil.util_mkint(buffer, color);
	checksum += autil.util_mkint(buffer, area);
	autil.util_mkint(buffer, checksum);
	if (!autil.util_SendMesg(&socket, 35, buffer)){
		IsOnLine = FALSE;
		return SENDMSG_ERROR;
	}

	return SUCCESSFUL;
}

//人物对对话框的选择
int CDpMain::SendSelectWindowButton(int x, int y, int seqno, int objindex, int select, char *data)
{
	char buffer[1024];
	int checksum = 0;

	checksum = 0;
	ZeroMemory(buffer, sizeof(buffer));
	checksum += autil.util_mkint(buffer, x);
	checksum += autil.util_mkint(buffer, y);
	checksum += autil.util_mkint(buffer, seqno);
	checksum += autil.util_mkint(buffer, objindex);
	checksum += autil.util_mkint(buffer, select);	//select=0->没有按钮有选项，用户选择结果放入data中，select=1->ok,select=2->cancel,select=4->确定，select=8->取消，select=16->上一页，select=32->下一页
	checksum += autil.util_mkstring(buffer, data);	//对话框内用户输入的内容（用户购买物品时“物品位置/z物品数量"）
	autil.util_mkint(buffer, checksum);
	if (!autil.util_SendMesg(&socket, 67, buffer)){
		IsOnLine = FALSE;
		return SENDMSG_ERROR;
	}

	return SUCCESSFUL;
}

//切换地图
int  CDpMain::SendChangeMap(int seqno, int x, int y, int dir)
{
	char buffer[1024];
	int checksum = 0;

	checksum = 0;
	ZeroMemory(buffer, sizeof(buffer));
	checksum += autil.util_mkint(buffer, 3);
	checksum += autil.util_mkint(buffer, seqno);
	checksum += autil.util_mkint(buffer, x);
	checksum += autil.util_mkint(buffer, y);
	checksum += autil.util_mkint(buffer, dir);
	autil.util_mkint(buffer, checksum);
	if (!autil.util_SendMesg(&socket, 3, buffer)){
		IsOnLine = FALSE;
		return SENDMSG_ERROR;
	}

	return SUCCESSFUL;
}

//返回离人物最近的一个npc的objindex
int CDpMain::GetNearNpcObjIndex(int x, int y)
{
	int i, min;
	min = 0;
	for (i = 1; i<50; i++){
		if (npcinfo[i].charwhichtype == -1)
			break;
		if (npcinfo[i].charwhichtype != 1){
			if ((abs(x - npcinfo[min].x) + abs(y - npcinfo[min].y))>(abs(x - npcinfo[i].x) + abs(y - npcinfo[i].y))){
				min = i;
			}
		}
	}
	return npcinfo[min].objindex;
}

//走路指令
BOOL CDpMain::WalkPos(CString &para)
{
	int x1, y1, x2, y2, pos;
	pos = para.Find(",");
	if (pos>0){
		x2 = atoi(para.Left(pos));
		y2 = atoi(para.Right(para.GetLength() - pos - 1));
		if (x2 >= 0 && x2 <= changemap.maxX && y2 >= 0 && y2 <= changemap.maxY){
			x1 = charotherinfo.x;
			y1 = charotherinfo.y;
			if (x1 == 0 || y1 == 0)
				return FALSE;
			if (x1 == x2 && y1 == y2)
				return TRUE;
			WALKARRAY walk[100];
			int n = CalcWalkPos(walk, x1, y1, x2, y2);
			if (n <= 0)
				return FALSE;
			for (int i = 0; i<n; i++){
				if (SendWalkPos(walk[i].x, walk[i].y, walk[i].direction) != SUCCESSFUL){
					IsOnLine = FALSE;
					return FALSE;
				}
				else{
					CalcCharPosition(&walk[i]);
				}
			}
			return TRUE;
		}
	}
	return FALSE;
}

//计算人物走路后当前所处的位置
void CDpMain::CalcCharPosition(WALKARRAY *walk)
{
	int n = strlen(walk->direction);
	for (int i = 0; i<n; i++){
		if (walk->direction[i] == 'a'){
			charotherinfo.y -= 1;
		}
		else if (walk->direction[i] == 'b'){
			charotherinfo.x += 1;
			charotherinfo.y -= 1;
		}
		else if (walk->direction[i] == 'c'){
			charotherinfo.x += 1;
		}
		else if (walk->direction[i] == 'd'){
			charotherinfo.x += 1;
			charotherinfo.y += 1;
		}
		else if (walk->direction[i] == 'e'){
			charotherinfo.y += 1;
		}
		else if (walk->direction[i] == 'f'){
			charotherinfo.x -= 1;
			charotherinfo.y += 1;
		}
		else if (walk->direction[i] == 'g'){
			charotherinfo.x -= 1;
		}
		else if (walk->direction[i] == 'h'){
			charotherinfo.x -= 1;
			charotherinfo.y -= 1;
		}
	}
}


//计算从起始点到目的地的走路坐标和方向，返回所有步数
int CDpMain::CalcWalkPos(WALKARRAY *walk, int xstart, int ystart, int xend, int yend)
{
	//x1,y1起始坐标，x2,y2目标坐标,xf,yf走路方向
	int i, x, y, x1, y1, x2, y2, xf, yf, dx, dy;
	//用于记录是否已向离目的地较远的方向移动一步
	bool flag;
	WALKARRAY walkpos[100];
	if (xstart == xend && ystart == yend)
		return 0;
	x1 = xstart;
	y1 = ystart;
	x2 = xend;
	y2 = yend;
	//记录当前起始位置
	x = x1;
	y = y1;
	//确定走路方向
	i = 0;
	if (x1 <= x2)
		xf = 1;
	else
		xf = -1;
	if (y1 <= y2)
		yf = 1;
	else
		yf = -1;
	//用于控制走路方式，为假沿x轴或y轴走，为真则沿x,y轴各步一步
	flag = false;
	do{
		//当前位置与目的地的距离x轴和y轴各有多远
		dx = abs(x2 - x);
		dy = abs(y2 - y);
		if (dx>0 && dy == 0){//只沿x轴方向走路
			if (x != x2)x += xf;
		}
		else if (dy>0 && dx == 0){//只沿y轴方向走路
			if (y != y2)y += yf;
		}
		else{//沿xy轴方向同时走路
			//“之”字型走路，若沿x轴走一步，则下一步沿x,y轴各步一步
			if (dx>dy && flag == false){//沿x轴走一步
				if (x != x2)x += xf;
				flag = true;
			}
			else if (dx<dy && flag == false){//沿y轴走一步
				if (y != y2)y += yf;
				flag = true;
			}
			else{//沿x,y轴各走一步
				if (x != x2)x += xf;
				if (y != y2)y += yf;
				flag = false;
			}
		}
		//记下向前走一步后的新坐标
		walkpos[i].x = x;
		walkpos[i].y = y;
		//求新坐标与上一个坐标间的距离
		dx = walkpos[i].x - x1;
		dy = walkpos[i].y - y1;
		//根据两点间的距离来确定走路的方向
		if (dx == -1 && dy == 0){//g
			strcpy_s(walkpos[i].direction, "g");
		}
		else if (dx == -1 && dy == 1){//f
			strcpy_s(walkpos[i].direction, "f");
		}
		else if (dx == 0 && dy == 1){//e
			strcpy_s(walkpos[i].direction, "e");
		}
		else if (dx == 1 && dy == 1){//d
			strcpy_s(walkpos[i].direction, "d");
		}
		else if (dx == 1 && dy == 0){//c
			strcpy_s(walkpos[i].direction, "c");
		}
		else if (dx == 1 && dy == -1){//b
			strcpy_s(walkpos[i].direction, "b");
		}
		else if (dx == 0 && dy == -1){//a
			strcpy_s(walkpos[i].direction, "a");
		}
		else if (dx == -1 && dy == -1){//h
			strcpy_s(walkpos[i].direction, "h");
		}
		else
			return 0;
		//把新坐标做为下一次走路的起始位置
		x1 = walkpos[i].x;
		y1 = walkpos[i].y;
		i++;
	} while (x != x2 || y != y2);
	int n, j;
	n = i / 2;
	x1 = xstart;
	y1 = ystart;
	j = 0;
	while (j<n){
		walk[j].x = x1;
		walk[j].y = y1;
		strcpy_s(walk[j].direction, walkpos[2 * j].direction);
		strcat(walk[j].direction, walkpos[2 * j + 1].direction);
		j++;
		x1 = walkpos[2 * j - 1].x;
		y1 = walkpos[2 * j - 1].y;
	}
	if (i % 2 != 0){
		if (i == 1){
			walk[n].x = xstart;
			walk[n].y = ystart;
		}
		else{
			walk[n].x = walkpos[i - 2].x;
			walk[n].y = walkpos[i - 2].y;
		}
		strcpy_s(walk[n].direction, walkpos[i - 1].direction);
		n++;
	}
	return n;
}


#pragma endregion 

#pragma region 人物操作

//删除帐号，id人物位置编号,0左边人物，1右边人物
int CDpMain::DeleteChar(int id)
{
	char buffer[8192], raw[8192], result[8192], message[8192];
	int func, fieldcount, checksum = 0, checksumrecv;
	int recvbytes;
	//向服务端删除人物信息
	checksum = 0;
	ZeroMemory(buffer, sizeof(buffer));
	checksum += autil.util_mkstring(buffer, charlist[id].name);
	autil.util_mkint(buffer, checksum);
	if (!autil.util_SendMesg(&socket, 75, buffer)){
		IsOnLine = FALSE;
		return SENDMSG_ERROR;
	}
	ZeroMemory(buffer, sizeof(buffer));
	recvbytes = recv(socket, buffer, sizeof(buffer), 0);
	if (recvbytes <= 0)
		return RECVMSG_ERROR;
	//获取帐号删除是否成功信息
	autil.util_Init();
	autil.util_DecodeMessage(raw, buffer);
	if (!autil.util_SplitMessage(raw, SEPARATOR))
		return SPLITMSG_ERROR;
	if (!autil.util_GetFunctionFromSlice(&func, &fieldcount))
		return GETFUNC_ERROR;
	checksum = 0;
	if (func == 86 && fieldcount == 3){
		checksum += autil.util_destring(2, result);
		checksum += autil.util_destring(3, message);
		autil.util_deint(4, &checksumrecv);
		if (checksum != checksumrecv)
			return CHECKSUM_ERROR;
		if (strcmp(result, "successful") != 0 && strcmp(message, "ok") != 0)
			return DELETE_CHAR_ERROR;
	}
	else
		return INVALID_INFO;
	return SUCCESSFUL;
}

//创建新人物
int CDpMain::CreateNewChar(int dataplace, char *charname, int imgno, int faceimgno, int vital, int str, int tgh, int dex, int earth, int water, int fire, int wind, int hometown)
{
	char buffer[8192], raw[8192], result[8192], message[8192];
	int func, fieldcount, checksum = 0, checksumrecv;
	int recvbytes;

	//向服务端发送创建新人物信息
	checksum = 0;
	ZeroMemory(buffer, sizeof(buffer));
	checksum += autil.util_mkint(buffer, dataplace);
	checksum += autil.util_mkstring(buffer, charname);
	checksum += autil.util_mkint(buffer, imgno);
	checksum += autil.util_mkint(buffer, faceimgno);
	checksum += autil.util_mkint(buffer, vital);
	checksum += autil.util_mkint(buffer, str);
	checksum += autil.util_mkint(buffer, tgh);
	checksum += autil.util_mkint(buffer, dex);
	checksum += autil.util_mkint(buffer, earth);
	checksum += autil.util_mkint(buffer, water);
	checksum += autil.util_mkint(buffer, fire);
	checksum += autil.util_mkint(buffer, wind);
	checksum += autil.util_mkint(buffer, hometown);
	autil.util_mkint(buffer, checksum);
	if (!autil.util_SendMesg(&socket, 73, buffer)){
		IsOnLine = FALSE;
		return SENDMSG_ERROR;
	}
	ZeroMemory(buffer, sizeof(buffer));
	recvbytes = recv(socket, buffer, sizeof(buffer), 0);
	if (recvbytes <= 0)
		return RECVMSG_ERROR;
	//检测新帐号创建是否成功
	autil.util_Init();
	autil.util_DecodeMessage(raw, buffer);
	if (!autil.util_SplitMessage(raw, SEPARATOR))
		return SPLITMSG_ERROR;
	if (!autil.util_GetFunctionFromSlice(&func, &fieldcount))
		return GETFUNC_ERROR;
	checksum = 0;
	if (func == 84 && fieldcount == 3){
		checksum += autil.util_destring(2, result);
		checksum += autil.util_destring(3, message);
		autil.util_deint(4, &checksumrecv);
		if (checksum != checksumrecv)
			return CHECKSUM_ERROR;
		if (strcmp(result, "successful") != 0)
			return CREATE_NEWCHAR_ERROR;
	}
	else
		return INVALID_INFO;
	//创建人物成功后保存人物名称
	strcpy_s(charlist[dataplace].name, charname);
	return SUCCESSFUL;
}

//加升级点，0代表体力加1点，1代表腕力，2代表耐力，3代表速度
int CDpMain::SendUpLevelPoint(int flg)
{
	char buffer[1024];
	int checksum = 0;

	checksum = 0;
	ZeroMemory(buffer, sizeof(buffer));
	checksum += autil.util_mkint(buffer, flg);
	autil.util_mkint(buffer, checksum);
	if (!autil.util_SendMesg(&socket, 64, buffer)){
		IsOnLine = FALSE;
		return SENDMSG_ERROR;
	}

	return SUCCESSFUL;
}

//自动加点
void CDpMain::AutoAddUpLevelPoint()
{
	if (chardetail.uplevelpoint>0){
		if (nAutoUpPoint[0]>0 && nAutoUpPoint[0]<1000 && chardetail.vital<nAutoUpPoint[0]){
			SendUpLevelPoint(0);
		}
		else if (nAutoUpPoint[0]>1000 && nAutoUpPoint[0]<2000 && chardetail.str<(nAutoUpPoint[0] - 1000)){
			SendUpLevelPoint(1);
		}
		else if (nAutoUpPoint[0]>2000 && nAutoUpPoint[0]<3000 && chardetail.tough<(nAutoUpPoint[0] - 2000)){
			SendUpLevelPoint(2);
		}
		else if (nAutoUpPoint[0]>3000 && nAutoUpPoint[0]<4000 && chardetail.dex<(nAutoUpPoint[0] - 3000)){
			SendUpLevelPoint(3);
		}
		else if (nAutoUpPoint[1]>0 && nAutoUpPoint[1]<1000 && chardetail.vital<nAutoUpPoint[1]){
			SendUpLevelPoint(0);
		}
		else if (nAutoUpPoint[1]>1000 && nAutoUpPoint[1]<2000 && chardetail.str<(nAutoUpPoint[1] - 1000)){
			SendUpLevelPoint(1);
		}
		else if (nAutoUpPoint[1]>2000 && nAutoUpPoint[1]<3000 && chardetail.tough<(nAutoUpPoint[1] - 2000)){
			SendUpLevelPoint(2);
		}
		else if (nAutoUpPoint[1]>3000 && nAutoUpPoint[1]<4000 && chardetail.dex<(nAutoUpPoint[1] - 3000)){
			SendUpLevelPoint(3);
		}
		else if (nAutoUpPoint[2]>0 && nAutoUpPoint[2]<1000 && chardetail.vital<nAutoUpPoint[2]){
			SendUpLevelPoint(0);
		}
		else if (nAutoUpPoint[2]>1000 && nAutoUpPoint[2]<2000 && chardetail.str<(nAutoUpPoint[2] - 1000)){
			SendUpLevelPoint(1);
		}
		else if (nAutoUpPoint[2]>2000 && nAutoUpPoint[2]<3000 && chardetail.tough<(nAutoUpPoint[2] - 2000)){
			SendUpLevelPoint(2);
		}
		else if (nAutoUpPoint[2]>3000 && nAutoUpPoint[2]<4000 && chardetail.dex<(nAutoUpPoint[2] - 3000)){
			SendUpLevelPoint(3);
		}
		else if (nAutoUpPoint[3]>0 && nAutoUpPoint[3]<1000 && chardetail.vital<nAutoUpPoint[3]){
			SendUpLevelPoint(0);
		}
		else if (nAutoUpPoint[3]>1000 && nAutoUpPoint[3]<2000 && chardetail.str<(nAutoUpPoint[3] - 1000)){
			SendUpLevelPoint(1);
		}
		else if (nAutoUpPoint[3]>2000 && nAutoUpPoint[3]<3000 && chardetail.tough<(nAutoUpPoint[3] - 2000)){
			SendUpLevelPoint(2);
		}
		else if (nAutoUpPoint[3]>3000 && nAutoUpPoint[3]<4000 && chardetail.dex<(nAutoUpPoint[3] - 3000)){
			SendUpLevelPoint(3);
		}
	}
}

//计算最大负重
void CDpMain::CalcMaxLoad(){
	switch (chardetail.transmigration){
	case 0:
		maxload = 3;
		break;
	case 1:
	case 2:
	case 3:
	case 4:
		maxload = 3 + chardetail.transmigration;
		break;
	case 5:
		maxload = 10;
		break;
	case 6:
		maxload = 15;
		break;
	}
	//取腰带的负重
	char *p, buf[5] = { 0 };
	int i = 0;
	if (strlen(iteminfo[5].name)>0){
		p = strstr(iteminfo[5].comment, "负重");
		p += 4;
		while (!(*p >= '0' &&*p <= '9'))
			p++;
		while (*p >= '0' &&*p <= '9'){
			buf[i] = *p;
			i++;
			p++;
		}
		if (i>0)
			maxload += atoi(buf);
	}

}

//向服务端发送安全码
int CDpMain::SendSafeCode(int seqno)
{
	char buffer[1024];
	int checksum = 0;

	checksum = 0;
	ZeroMemory(buffer, sizeof(buffer));
	checksum += autil.util_mkint(buffer, charotherinfo.x);
	checksum += autil.util_mkint(buffer, charotherinfo.y);
	checksum += autil.util_mkint(buffer, seqno);
	checksum += autil.util_mkint(buffer, -1);
	checksum += autil.util_mkint(buffer, 1);
	checksum += autil.util_mkstring(buffer, user.safecode);
	autil.util_mkint(buffer, checksum);
	if (!autil.util_SendMesg(&socket, 67, buffer)){
		IsOnLine = FALSE;
		return SENDMSG_ERROR;
	}

	return SUCCESSFUL;
}

//改变人物名称
int CDpMain::SendChangeCharName(char *name)
{
	char buffer[1024];
	int checksum = 0;

	checksum = 0;
	ZeroMemory(buffer, sizeof(buffer));
	checksum += autil.util_mkstring(buffer, name);
	autil.util_mkint(buffer, checksum);
	if (!autil.util_SendMesg(&socket, 62, buffer)){
		IsOnLine = FALSE;
		return SENDMSG_ERROR;
	}

	return SUCCESSFUL;
}

//改变宠物名称
int CDpMain::SendChangePetName(int pet, char *name)
{
	char buffer[1024];
	int checksum = 0;

	checksum = 0;
	ZeroMemory(buffer, sizeof(buffer));
	checksum += autil.util_mkint(buffer, pet);
	checksum += autil.util_mkstring(buffer, name);
	autil.util_mkint(buffer, checksum);
	if (!autil.util_SendMesg(&socket, 65, buffer)){
		IsOnLine = FALSE;
		return SENDMSG_ERROR;
	}

	return SUCCESSFUL;
}


#pragma endregion

#pragma region 平时操作

//物品是否能堆叠
BOOL CDpMain::ItemCanPile(int flg)
{
	return ((flg >> 2) & 1);
}

//向服务端发送移动或堆叠物品信息
int CDpMain::SendMoveItem(int from, int to)
{
	char buffer[1024];
	int checksum = 0;

	checksum = 0;
	ZeroMemory(buffer, sizeof(buffer));
	checksum += autil.util_mkint(buffer, from);
	checksum += autil.util_mkint(buffer, to);
	autil.util_mkint(buffer, checksum);
	if (!autil.util_SendMesg(&socket, 23, buffer)){
		IsOnLine = FALSE;
		return SENDMSG_ERROR;
	}

	return SUCCESSFUL;
}

//向服务端发送丢弃物品
int CDpMain::SendDiscardItem(int index)
{
	char buffer[1024];
	int checksum = 0;

	checksum = 0;
	ZeroMemory(buffer, sizeof(buffer));
	checksum += autil.util_mkint(buffer, charotherinfo.x);
	checksum += autil.util_mkint(buffer, charotherinfo.y);
	checksum += autil.util_mkint(buffer, index);
	autil.util_mkint(buffer, checksum);
	if (!autil.util_SendMesg(&socket, 19, buffer)){
		IsOnLine = FALSE;
		return SENDMSG_ERROR;
	}

	return SUCCESSFUL;
}

//丢弃金钱
int CDpMain::SendDiscardMoney(int amount)
{
	char buffer[1024];
	int checksum = 0;

	checksum = 0;
	ZeroMemory(buffer, sizeof(buffer));
	checksum += autil.util_mkint(buffer, charotherinfo.x);
	checksum += autil.util_mkint(buffer, charotherinfo.y);
	checksum += autil.util_mkint(buffer, amount);
	autil.util_mkint(buffer, checksum);
	if (!autil.util_SendMesg(&socket, 20, buffer)){
		IsOnLine = FALSE;
		return SENDMSG_ERROR;
	}

	return SUCCESSFUL;
}

//从个人银行存取钱,B|G|-100取钱，B|G|+100存钱;从家族银行取钱B|T|-122，存钱B|T|+122
int CDpMain::SendSaveLoadMoney(int num)
{
	char buffer[1024], buf[255];
	int checksum = 0;

	checksum = 0;
	ZeroMemory(buffer, sizeof(buffer));
	ZeroMemory(buf, sizeof(buf));
	sprintf_s(buf, "B|G|%d", num);
	checksum += autil.util_mkstring(buffer, buf);
	autil.util_mkint(buffer, checksum);
	if (!autil.util_SendMesg(&socket, 94, buffer)){
		IsOnLine = FALSE;
		return SENDMSG_ERROR;
	}

	return SUCCESSFUL;
}

//从家族银行取钱B|T|-122，存钱B|T|+122
int CDpMain::SendSaveLoadMoneyFromFamily(int num)
{
	char buffer[1024], buf[255];
	int checksum = 0;

	checksum = 0;
	ZeroMemory(buffer, sizeof(buffer));
	ZeroMemory(buf, sizeof(buf));
	sprintf_s(buf, "B|T|%d", num);
	checksum += autil.util_mkstring(buffer, buf);
	autil.util_mkint(buffer, checksum);
	if (!autil.util_SendMesg(&socket, 94, buffer)){
		IsOnLine = FALSE;
		return SENDMSG_ERROR;
	}

	return SUCCESSFUL;
}

//丢弃包含物品
int CDpMain::SendDiscardLikeItem()
{
	int t, n;
	for (t = 0; t<nDiscardLikeItemNum; t++){
		for (n = 9; n<24; n++){
			if (charotherinfo.state == 0 && (strstr(iteminfo[n].name, cDiscardLikeItem[t]) != NULL || strstr(iteminfo[n].comment, cDiscardLikeItem[t]) != NULL)){
				SendDiscardItem(n);
			}
		}
	}
	return SUCCESSFUL;
}

//丢弃不包含
int CDpMain::SendDiscardExceptItem()
{
	int t, n;
	for (t = 0; t<nDiscardExceptItemNum; t++){
		for (n = 9; n<24; n++){
			if (charotherinfo.state == 0 && strlen(iteminfo[n].name)>0 && strstr(iteminfo[n].name, cDiscardExceptItem[t]) == NULL && strstr(iteminfo[n].comment, cDiscardExceptItem[t]) == NULL){
				SendDiscardItem(n);
			}
		}
	}
	return SUCCESSFUL;
}

//吃补血肉
int CDpMain::SendEatMeat()
{
	int n;
	for (n = 9; n<24; n++){
		if (strlen(iteminfo[n].name)>0 && strstr(iteminfo[n].name, "肉") != NULL && strstr(iteminfo[n].comment, "耐久力") != NULL){
			SendUseItem(n, 0);
		}
	}
	return SUCCESSFUL;
}

//丢没有补血功能的血肉
int CDpMain::SendDiscardMeat()
{
	int n;
	for (n = 9; n<24; n++){
		if (charotherinfo.state == 0 && strlen(iteminfo[n].name)>0 && strstr(iteminfo[n].name, "肉") != NULL && strstr(iteminfo[n].comment, "耐久力") == NULL){
			SendDiscardItem(n);
		}
	}
	return SUCCESSFUL;
}

//平时补气
int CDpMain::SendRecruitMpPlaceTime()
{
	int n;
	for (n = 9; n<24; n++){
		if (charotherinfo.state == 0 && strlen(iteminfo[n].name)>0 && MatchingString(iteminfo[n].comment, "*气力*前後回复*")){
			SendUseItem(n, 0);
			break;
		}
	}
	return SUCCESSFUL;
}

//平时精灵补血
int CDpMain::SendRecruitHpPlaceTime(int magicid, int to)
{
	char buffer[1024];
	int checksum = 0;

	checksum = 0;
	ZeroMemory(buffer, sizeof(buffer));
	checksum += autil.util_mkint(buffer, charotherinfo.x);
	checksum += autil.util_mkint(buffer, charotherinfo.y);
	checksum += autil.util_mkint(buffer, magicid);
	checksum += autil.util_mkint(buffer, to);//0代表人，1-5代表宠物
	autil.util_mkint(buffer, checksum);
	if (!autil.util_SendMesg(&socket, 57, buffer)){
		IsOnLine = FALSE;
		return SENDMSG_ERROR;
	}

	return SUCCESSFUL;
}

//双击使用物品
int CDpMain::SendUseItem(int from, int to)
{
	char buffer[1024];
	int checksum = 0;

	checksum = 0;
	ZeroMemory(buffer, sizeof(buffer));
	checksum += autil.util_mkint(buffer, charotherinfo.x);
	checksum += autil.util_mkint(buffer, charotherinfo.y);
	checksum += autil.util_mkint(buffer, from);	//0-8代表人物身上的装备位，物品在道具栏中的位置从9代表第1个位置
	checksum += autil.util_mkint(buffer, to);	//代表两个物品交换后的位置，0代表删除使用后的物品(给人物使用)，1-5代表给宠物使用
	autil.util_mkint(buffer, checksum);
	if (!autil.util_SendMesg(&socket, 17, buffer)){
		IsOnLine = FALSE;
		return SENDMSG_ERROR;
	}

	return SUCCESSFUL;
}

//拾获物品或宠物
int CDpMain::SendPickup(int x, int y, int dir)
{
	char buffer[1024];
	int checksum = 0;

	checksum = 0;
	ZeroMemory(buffer, sizeof(buffer));
	checksum += autil.util_mkint(buffer, x);
	checksum += autil.util_mkint(buffer, y);
	checksum += autil.util_mkint(buffer, dir);
	autil.util_mkint(buffer, checksum);
	if (!autil.util_SendMesg(&socket, 18, buffer)){
		IsOnLine = FALSE;
		return SENDMSG_ERROR;
	}

	return SUCCESSFUL;
}

//吃智慧果
int CDpMain::SendEatExpNut()
{
	for (int i = 9; i<24; i++){
		if (strstr(iteminfo[i].name, "智慧之果") != NULL || strstr(iteminfo[i].name, "聪明的豆子") != NULL){
			SendUseItem(i, 0);
			break;
		}
	}
	return SUCCESSFUL;
}

//吃鱼鳃草
int CDpMain::SendEatSYC()
{
	for (int i = 9; i<24; i++){
		if (strstr(iteminfo[i].name, "鱼鳃草") != NULL){
			SendUseItem(i, 0);
			break;
		}
	}
	return SUCCESSFUL;
}

//发送自动堆叠
int CDpMain::SendAutoPile()
{
	for (int i = 23; i>9; i--){
		for (int j = 9; j<i; j++){
			if (charotherinfo.state == 0 && strlen(iteminfo[i].name)>0 && ItemCanPile(iteminfo[i].flg) && strcmp(iteminfo[i].name, iteminfo[j].name) == 0 && iteminfo[j].pilenums<maxload){
				SendMoveItem(i, j);
				return SUCCESSFUL;
			}
		}
	}
	return SUCCESSFUL;
}

//使用物品 useitem {列表(1|2|3-5)|名称}[,对象(0..5)]
BOOL CDpMain::UseItem(CString item)
{
	int pos = 0, i, start, end = 0, object;
	CString szVal, szVal1, szItem, szObject;
	szItem = item.Tokenize(",", pos);
	if (szItem.IsEmpty())
		return FALSE;
	szObject = item.Tokenize(",", pos);
	if (szObject.IsEmpty())
		object = 0;
	else
		object = atoi(szObject);
	pos = 0;
	if (szItem.GetAt(0) >= '1' && szItem.GetAt(0) <= '9'){
		szVal = szItem.Tokenize("-", pos);
		szVal1 = szItem.Tokenize("-", pos);
		start = atoi(szVal);
		if (szVal1.GetLength()>0)
			end = atoi(szVal1);
		if (start >0 && start<end && end>0 && end <= 15){
			for (i = 8 + start; i <= 8 + end; i++){
				SendUseItem(i, object);
			}
		}
		else if (start >= 1 && start <= 15){
			SendUseItem(8 + start, object);
		}
	}
	else{
		szVal = szItem.Tokenize("|", pos);
		while (szVal.GetLength()>0){
			for (i = 9; i<24; i++){
				if (strlen(iteminfo[i].name)>0 && (strstr(iteminfo[i].name, szVal) != NULL || strstr(iteminfo[i].comment, szVal) != NULL)){
					SendUseItem(i, object);
					break;
				}
			}
			szVal = szItem.Tokenize("|", pos);
		}
	}
	return SUCCESSFUL;
}

//拾获物品或宠物
void CDpMain::Pickup(CString dir)
{
	if (dir.Compare("*") == 0){
		SendPickup(charotherinfo.x, charotherinfo.y, 0);
		SendPickup(charotherinfo.x, charotherinfo.y, 1);
		SendPickup(charotherinfo.x, charotherinfo.y, 2);
		SendPickup(charotherinfo.x, charotherinfo.y, 3);
		SendPickup(charotherinfo.x, charotherinfo.y, 4);
		SendPickup(charotherinfo.x, charotherinfo.y, 5);
		SendPickup(charotherinfo.x, charotherinfo.y, 6);
		SendPickup(charotherinfo.x, charotherinfo.y, 7);
	}
	else if (dir.Compare("北") == 0){
		SendPickup(charotherinfo.x, charotherinfo.y, 0);
	}
	else if (dir.Compare("东北") == 0){
		SendPickup(charotherinfo.x, charotherinfo.y, 1);
	}
	else if (dir.Compare("东") == 0){
		SendPickup(charotherinfo.x, charotherinfo.y, 2);
	}
	else if (dir.Compare("东南") == 0){
		SendPickup(charotherinfo.x, charotherinfo.y, 3);
	}
	else if (dir.Compare("南") == 0){
		SendPickup(charotherinfo.x, charotherinfo.y, 4);
	}
	else if (dir.Compare("西南") == 0){
		SendPickup(charotherinfo.x, charotherinfo.y, 5);
	}
	else if (dir.Compare("西") == 0){
		SendPickup(charotherinfo.x, charotherinfo.y, 6);
	}
	else if (dir.Compare("西北") == 0){
		SendPickup(charotherinfo.x, charotherinfo.y, 7);
	}
}

//丢弃物品
void CDpMain::DoffItem(CString item)
{
	int pos = 0, i, start, end = 0;
	CString szVal, szVal1;
	if (item.GetAt(0) >= '1' && item.GetAt(0) <= '9'){
		szVal = item.Tokenize("-", pos);
		szVal1 = item.Tokenize("-", pos);
		start = atoi(szVal);
		if (szVal1.GetLength()>0)
			end = atoi(szVal1);
		if (start >0 && start<end && end>0 && end <= 15){
			for (i = 8 + start; i <= 8 + end; i++){
				SendDiscardItem(i);
			}
		}
		else if (start >= 1 && start <= 15){
			SendDiscardItem(8 + start);
		}
	}
	else{
		szVal = item.Tokenize("|", pos);
		while (szVal.GetLength()>0){
			for (i = 9; i<24; i++){
				if (strstr(iteminfo[i].name, szVal) != NULL || strstr(iteminfo[i].comment, szVal) != NULL){
					SendDiscardItem(i);
					break;
				}
			}
			szVal = item.Tokenize("|", pos);
		}
	}
}

//丢弃宠物
void CDpMain::DoffPet(CString  pet)
{
	int pos = 0, i, start, end = 0;
	CString szVal, szVal1;
	if (pet.GetAt(0) >= '1' && pet.GetAt(0) <= '5'){
		szVal = pet.Tokenize("-", pos);
		szVal1 = pet.Tokenize("-", pos);
		start = atoi(szVal);
		if (szVal1.GetLength()>0)
			end = atoi(szVal1);
		if (start >0 && start<end && end>0 && end <= 5){
			for (i = start; i <= end; i++){
				SendDiscardPet(i - 1);
			}
		}
		else if (start >= 1 && start <= 5){
			SendDiscardPet(start - 1);
		}
	}
	else{
		szVal = pet.Tokenize("|", pos);
		while (szVal.GetLength()>0){
			for (i = 0; i<5; i++){
				if (strstr(petdetail[i].oldname, szVal) != NULL || strstr(petdetail[i].newname, szVal) != NULL){
					SendDiscardPet(i);
					break;
				}
			}
			szVal = pet.Tokenize("|", pos);
		}
	}
}

//丢弃金钱
void CDpMain::DoffStone(CString stone)
{
	int n = 0;
	if (IsNumber(stone))
		n = atoi(stone);
	else if (stone.GetAt(0) == '@')
		intvar.Lookup(stone, n);
	if (n>0)
		SendDiscardMoney(n);
}

//存钱到个人银行
void CDpMain::SaveStone(CString stone)
{
	int n = 0;
	if (IsNumber(stone))
		n = atoi(stone);
	else if (stone.GetAt(0) == '@')
		intvar.Lookup(stone, n);
	if (n>0)
		SendSaveLoadMoney(n);
}

//从个人银行取钱
void CDpMain::LoadStone(CString stone)
{
	int n = 0;
	if (IsNumber(stone))
		n = atoi(stone);
	else if (stone.GetAt(0) == '@')
		intvar.Lookup(stone, n);
	if (n>0)
		SendSaveLoadMoney(-n);
}

//存钱到家族银行
void CDpMain::SaveStoneToFamily(CString stone)
{
	int n = 0;
	if (IsNumber(stone))
		n = atoi(stone);
	else if (stone.GetAt(0) == '@')
		intvar.Lookup(stone, n);
	if (n>0)
		SendSaveLoadMoneyFromFamily(n);
}

//从家族银行取钱
void CDpMain::LoadStoneFromFamily(CString stone)
{
	int n = 0;
	if (IsNumber(stone))
		n = atoi(stone);
	else if (stone.GetAt(0) == '@')
		intvar.Lookup(stone, n);
	if (n>0)
		SendSaveLoadMoneyFromFamily(-n);
}

#pragma endregion

#pragma region 邮件

//发送宠物邮件
int CDpMain::SendPetMail(int towho, int petindex, int itemindex, char *msg, int color)
{
	char buffer[1024];
	int checksum = 0;

	checksum = 0;
	ZeroMemory(buffer, sizeof(buffer));
	checksum += autil.util_mkint(buffer, towho);
	checksum += autil.util_mkint(buffer, petindex);
	checksum += autil.util_mkint(buffer, itemindex);
	checksum += autil.util_mkstring(buffer, msg);
	checksum += autil.util_mkint(buffer, color);
	autil.util_mkint(buffer, checksum);
	if (!autil.util_SendMesg(&socket, 27, buffer)){
		IsOnLine = FALSE;
		return SENDMSG_ERROR;
	}

	return SUCCESSFUL;
}

//交换名片
int CDpMain::SendExchangeCard()
{
	char buffer[1024];
	int checksum = 0;

	checksum = 0;
	ZeroMemory(buffer, sizeof(buffer));
	checksum += autil.util_mkint(buffer, charotherinfo.x);
	checksum += autil.util_mkint(buffer, charotherinfo.y);
	autil.util_mkint(buffer, checksum);
	if (!autil.util_SendMesg(&socket, 33, buffer)){
		IsOnLine = FALSE;
		return SENDMSG_ERROR;
	}

	return SUCCESSFUL;
}

//删除名片,从0开始编号
int CDpMain::SendDeleteCard(int card)
{
	char buffer[1024];
	int checksum = 0;

	checksum = 0;
	ZeroMemory(buffer, sizeof(buffer));
	checksum += autil.util_mkint(buffer, card);
	autil.util_mkint(buffer, checksum);
	if (!autil.util_SendMesg(&socket, 32, buffer)){
		IsOnLine = FALSE;
		return SENDMSG_ERROR;
	}

	return SUCCESSFUL;
}


#pragma endregion 

#pragma region 交易

//发送摆滩命令，“data=O|”代表点击摆滩按钮，“data=S|2|1|1|11|0|15|33|欢迎光临!!”代表要摆滩的数据
//“data=E|”代表取消摆滩
int CDpMain::SendBaiTan(char *data)
{
	char buffer[1024];
	int checksum = 0;

	checksum = 0;
	ZeroMemory(buffer, sizeof(buffer));
	checksum += autil.util_mkstring(buffer, data);
	autil.util_mkint(buffer, checksum);
	if (!autil.util_SendMesg(&socket, 116, buffer)){
		IsOnLine = FALSE;
		return SENDMSG_ERROR;
	}

	return SUCCESSFUL;
}

//摆滩后发送成功命令
int CDpMain::SendBaiTanOK(int x, int y, int data)
{
	char buffer[1024];
	int checksum = 0;

	checksum = 0;
	ZeroMemory(buffer, sizeof(buffer));
	checksum += autil.util_mkint(buffer, x);
	checksum += autil.util_mkint(buffer, y);
	checksum += autil.util_mkint(buffer, data);
	autil.util_mkint(buffer, checksum);
	if (!autil.util_SendMesg(&socket, 56, buffer)){
		IsOnLine = FALSE;
		return SENDMSG_ERROR;
	}

	return SUCCESSFUL;
}


//摆滩，bt {石币|声望|积分|气势|DP|会员点},{类别1|位置1|价格1;...或类别|{*|n-m}|价格},{广告},{错误跳转}
//类别：0为道具，1为宠物
//位置:物品(1-15),宠物(1-5)
BOOL CDpMain::BaiTan(CString para)
{
	CString szType, szItem, szAd, szJump, szSubItem, szVal, szStart, szEnd;
	int pos = 0, pos1 = 0, pos2, i, j, type, price, start, end, starttime, lid;
	BAITAN baitan[20];

	szType = para.Tokenize(",", pos);
	if (szType.IsEmpty() || (szType.CompareNoCase("石币") != 0 && szType.CompareNoCase("声望") != 0 &&
		szType.CompareNoCase("积分") != 0 && szType.CompareNoCase("气势") != 0 &&
		szType.CompareNoCase("DP") != 0 && szType.CompareNoCase("会员点") != 0))
		return FALSE;
	szItem = para.Tokenize(",", pos);
	if (szItem.IsEmpty())
		return FALSE;
	szAd = para.Tokenize(",", pos);
	if (szAd.IsEmpty())
		return FALSE;
	szJump = para.Tokenize(",", pos);
	if (szJump.IsEmpty())
		return FALSE;
	//计算跳转地址
	if (!CalcJampAddress(szJump, lid)){
		return FALSE;
	}
	pos = 0;
	i = 0;
	szSubItem = szItem.Tokenize(";", pos);
	while (szSubItem.GetLength()>0){
		pos1 = 0;
		if (szSubItem.Find("*") >= 0){//全部道具摆滩
			szVal = szSubItem.Tokenize("|", pos1);
			if (!IsNumber(szVal))
				return FALSE;
			type = atoi(szVal);
			szVal = szSubItem.Tokenize("|", pos1);
			if (szVal.CompareNoCase("*") != 0)
				return FALSE;
			szVal = szSubItem.Tokenize("|", pos1);
			if (!IsNumber(szVal))
				return FALSE;
			price = atoi(szVal);
			for (int j = 1; j <= 15; j++){
				//道具不存在继续
				if (type == 0 && strlen(iteminfo[j + 8].name) <= 0)
					continue;
				baitan[i].type = type;
				baitan[i].pos = j;
				baitan[i].price = price;
				i++;
			}
			break;
		}
		else if (szSubItem.Find("-") >= 0){//部分连续道具或宠物摆滩
			szVal = szSubItem.Tokenize("|", pos1);
			if (!IsNumber(szVal))
				return FALSE;
			type = atoi(szVal);
			szVal = szSubItem.Tokenize("|", pos1);
			pos2 = 0;
			szStart = szVal.Tokenize("-", pos2);
			if (!IsNumber(szStart))
				return FALSE;
			start = atoi(szStart);
			szEnd = szVal.Tokenize("-", pos2);
			if (!IsNumber(szEnd))
				return FALSE;
			end = atoi(szEnd);
			szVal = szSubItem.Tokenize("|", pos1);
			if (!IsNumber(szVal))
				return FALSE;
			price = atoi(szVal);
			for (int j = start; j <= end; j++){
				//道具不存在继续
				if (type == 0 && strlen(iteminfo[j + 8].name) <= 0)
					continue;
				//宠物不存在继续
				if (type == 1 && petdetail[j - 1].islive <= 0 || charotherinfo.fightpet + 1 == j)
					continue;
				baitan[i].type = type;
				baitan[i].pos = j;
				baitan[i].price = price;
				i++;
			}
		}
		else{//单个道具或宠物摆滩
			szVal = szSubItem.Tokenize("|", pos1);
			if (IsNumber(szVal)){
				baitan[i].type = atoi(szVal);
			}
			else
				return FALSE;
			szVal = szSubItem.Tokenize("|", pos1);
			if (IsNumber(szVal)){
				baitan[i].pos = atoi(szVal);
			}
			else
				return FALSE;
			szVal = szSubItem.Tokenize("|", pos1);
			if (IsNumber(szVal)){
				baitan[i].price = atoi(szVal);
			}
			else
				return FALSE;

			//道具存在或宠物存在
			if ((baitan[i].type == 0 && strlen(iteminfo[baitan[i].pos + 8].name)>0) || (baitan[i].type == 1 && petdetail[baitan[i].pos - 1].islive>0 && charotherinfo.fightpet + 1 != baitan[i].pos))
				i++;
		}
		szSubItem = szItem.Tokenize(";", pos);
	}
	//最多摆滩可有20项，15个道具加5只宠物
	if (i>20)
		i = 20;
	//合成摆滩字符串
	char data[2048] = { 0 }, buf[255];
	sprintf(data, "S|%d|", i);
	for (j = 0; j<i; j++){
		if (baitan[j].type != 0 && baitan[j].type != 1)
			return FALSE;
		itoa(baitan[j].type, buf, 10);
		strcat_s(data, buf);
		strcat_s(data, "|");
		if (baitan[j].type == 0){
			if (baitan[j].pos<1 || baitan[j].pos>15)
				return FALSE;
		}
		else if (baitan[j].type == 1){
			if (baitan[j].pos<1 || baitan[j].pos>5)
				return FALSE;
		}
		else
			return FALSE;
		if (baitan[j].type == 0)
			itoa(baitan[j].pos + 8, buf, 10);
		else
			itoa(baitan[j].pos, buf, 10);
		strcat_s(data, buf);
		strcat_s(data, "|");
		if (baitan[j].price<0)
			return FALSE;
		itoa(baitan[j].price, buf, 10);
		strcat_s(data, buf);
		strcat_s(data, "|");
	}
	strcat_s(data, (LPSTR)(LPCTSTR)szAd);
	//发送点击摆滩按钮命令
	windowinfo.windowtype = -1;
	SendBaiTan("O|");
	//等待弹出摆滩窗口
	starttime = GetTickCount();
	pos = 0;
	j = 1;
	BOOL flag = FALSE;
	while ((GetTickCount() - starttime) <= 2000){
		if (windowinfo.windowtype == WINDOW_MESSAGETYPE_SELECT && windowinfo.buttontype == WINDOW_BUTTONTYPE_CANCEL){
			Tokenize(windowinfo.data, buf, "\n", pos);
			Tokenize(windowinfo.data, buf, "\n", pos);
			Tokenize(windowinfo.data, buf, "\n", pos);
			Tokenize(windowinfo.data, buf, "\n", pos);
			while (strlen(buf)>0){
				if (strstr(buf, szType) != NULL || strstr(buf, "ＤＰ") != NULL){
					flag = TRUE;
					break;
				}
				j++;
				Tokenize(windowinfo.data, buf, "\n", pos);
			}
			if (flag){
				//发送选择摆滩方式命令
				itoa(j, buf, 10);
				IsBaiTanOk = FALSE;
				SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, 0, buf);
				//选择摆滩方式后等待服务端返回信息				
				starttime = GetTickCount();
				while ((GetTickCount() - starttime) <= 2000){
					if (IsBaiTanOk)
						break;
					Sleep(20);
				}
				if (!IsBaiTanOk)
					goto BaiTan_Err;
				IsBaiTanOk = FALSE;
				SendMenuFlag(MENUFLAG);
				SendBaiTan(data);
				starttime = GetTickCount();
				while ((GetTickCount() - starttime) <= 2000){
					if (IsBaiTanOk)
						break;
					Sleep(20);
				}
				if (!IsBaiTanOk)
					goto BaiTan_Err;
				SendBaiTanOK(charotherinfo.x, charotherinfo.y, 5);
				//摆滩完成置为真
				Sleep(100);
				bIsBaiTaning = TRUE;
				return TRUE;
			}
			goto BaiTan_Err;
		}
		Sleep(20);
	}
BaiTan_Err:
	if (szJump.Left(5).CompareNoCase("call ") == 0)
		IPStack.push(IP);
	IP = lid;
	return TRUE;
}

//从摆滩处购卖物品 btbuy
BOOL  CDpMain::BuyFromBaiTan(CString para)
{
	int starttime, i;
	//发送购买指令
	SendBaiTan("O|");
	IsBaiTanOk = FALSE;
	starttime = GetTickCount();
	while ((GetTickCount() - starttime) <= 2000){
		if (IsBaiTanOk)
			break;
		Sleep(20);
	}
	if (!IsBaiTanOk)
		return TRUE;
	for (i = 0; i<20; i++){
		if (buy_baitan[i].type >= 0 && buy_baitan[i].price >= 0)
			break;
	}
	char buf[200];
	sprintf(buf, "B|1|%d|", buy_baitan[i].pos);
	SendBaiTan(buf);
	SendBaiTan("N|");
	return TRUE;
}


#pragma endregion 

#pragma region 道具宠物存取

//存宠物到寄存店
void CDpMain::SavePet(int towhere, CString item)
{
	int t;
	char buf[255], item_buf[255], tmp[10];
	//消息窗口结构置空
	windowinfo.windowtype = -1;
	windowinfo.buttontype = -1;
	SendTalk(charotherinfo.x, charotherinfo.y, "存", 0, 3);
	int starttime;
	t = 0;
	sprintf_s(item_buf, "%s", item);
	Tokenize(item_buf, buf, "|", t);
	while (strlen(buf)>0){
		//等待弹出存放宠物窗口
		starttime = GetTickCount();
		while ((GetTickCount() - starttime)<5000){
			if (windowinfo.windowtype == WINDOW_MESSAGETYPE_SELECT && windowinfo.buttontype == WINDOW_BUTTONTYPE_NONE){
				break;
			}
			Sleep(20);
		}
		if (windowinfo.windowtype == WINDOW_MESSAGETYPE_SELECT && windowinfo.buttontype == WINDOW_BUTTONTYPE_NONE){
			//选择存在哪里
			if (towhere == 0)//存寄存店
				SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, 0, "1");
			else{//存仓库
				SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, 0, "4");
				windowinfo.windowtype = -1;
				windowinfo.buttontype = -1;
				//等待弹出使用仓库对话框
				starttime = GetTickCount();
				while ((GetTickCount() - starttime)<5000){
					if (windowinfo.windowtype == WINDOW_MESSAGETYPE_SELECT && windowinfo.buttontype == WINDOW_BUTTONTYPE_CANCEL){
						break;
					}
					Sleep(20);
				}
				if (windowinfo.windowtype == WINDOW_MESSAGETYPE_SELECT && windowinfo.buttontype == WINDOW_BUTTONTYPE_CANCEL){
					SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, 0, "1");
				}
				else{//没有弹出使用仓库对话框返回
					return;
				}
			}
			//等待弹出要存放的宠物列表信息
			starttime = GetTickCount();
			while ((GetTickCount() - starttime)<5000){
				if (windowinfo.windowtype == WINDOW_MESSAGETYPE_PETSELECT && windowinfo.buttontype == WINDOW_BUTTONTYPE_CANCEL){
					break;
				}
				Sleep(20);
			}
			if (windowinfo.windowtype == WINDOW_MESSAGETYPE_PETSELECT && windowinfo.buttontype == WINDOW_BUTTONTYPE_CANCEL){
				if (IsNumber(buf)){
					//发送要存的宠物
					SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, 0, buf);
				}
				else{
					for (int j = 0; j<5; j++){
						if (strstr(petdetail[j].oldname, buf) != NULL || strstr(petdetail[j].newname, buf) != NULL){
							itoa(j + 1, tmp, 10);
							//发送要存的宠物
							SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, 0, tmp);
							break;
						}
					}
				}
				//确认存储谁
				starttime = GetTickCount();
				while ((GetTickCount() - starttime)<5000){
					if (windowinfo.windowtype == WINDOW_MESSAGETYPE_MESSAGE && windowinfo.buttontype == WINDOW_BUTTONTYPE_YESNO){
						break;
					}
					Sleep(20);
				}
				if (windowinfo.windowtype == WINDOW_MESSAGETYPE_MESSAGE && windowinfo.buttontype == WINDOW_BUTTONTYPE_YESNO){
					SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, 4, "");
					//希望你再度光临
					starttime = GetTickCount();
					while ((GetTickCount() - starttime)<5000){
						if (windowinfo.windowtype == WINDOW_MESSAGETYPE_MESSAGE && windowinfo.buttontype == WINDOW_BUTTONTYPE_OK){
							break;
						}
						Sleep(20);
					}
					if (windowinfo.windowtype == WINDOW_MESSAGETYPE_MESSAGE && windowinfo.buttontype == WINDOW_BUTTONTYPE_OK){
						SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, 1, "");
					}
				}
			}
		}
		else{
			break;
		}
		Tokenize(item_buf, buf, "|", t);
	}
}

//从宠物店取宠时，显示的宠物信息进行解析
void CDpMain::ParsePetData(char *data)
{
	int pos, i;
	char buf[255];
	char *p;
	pos = 3;
	i = 0;
	p = strstr(data, "LV.");
	if (p != NULL){
		Tokenize(p, buf, "LV.", pos);
		while (strlen(buf)>0){
			strcpy_s(load_pet[i].name, buf);
			i++;
			Tokenize(p, buf, "LV.", pos);
		}
	}
}

void CDpMain::GetPet(int fromwhere, CString item)
{
	int t;
	char buf[255], item_buf[255], tmp[10];
	//消息窗口结构置空
	windowinfo.windowtype = -1;
	windowinfo.buttontype = -1;
	SendTalk(charotherinfo.x, charotherinfo.y, "取", 0, 3);
	int starttime;
	t = 0;
	sprintf_s(item_buf, "%s", item);
	Tokenize(item_buf, buf, "|", t);
	while (strlen(buf)>0){
		//等待弹出存放宠物窗口
		starttime = GetTickCount();
		while ((GetTickCount() - starttime)<5000){
			if (windowinfo.windowtype == WINDOW_MESSAGETYPE_SELECT && windowinfo.buttontype == WINDOW_BUTTONTYPE_NONE){
				break;
			}
			Sleep(20);
		}
		if (windowinfo.windowtype == WINDOW_MESSAGETYPE_SELECT && windowinfo.buttontype == WINDOW_BUTTONTYPE_NONE){
			//选择从哪里取
			if (fromwhere == 0)//从寄存店取
				SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, 0, "2");
			else{//从仓库取
				SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, 0, "4");
				windowinfo.windowtype = -1;
				windowinfo.buttontype = -1;
				//等待弹出使用仓库对话框
				starttime = GetTickCount();
				while ((GetTickCount() - starttime)<5000){
					if (windowinfo.windowtype == WINDOW_MESSAGETYPE_SELECT && windowinfo.buttontype == WINDOW_BUTTONTYPE_CANCEL){
						break;
					}
					Sleep(20);
				}
				if (windowinfo.windowtype == WINDOW_MESSAGETYPE_SELECT && windowinfo.buttontype == WINDOW_BUTTONTYPE_CANCEL){
					SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, 0, "2");
				}
				else{//没有弹出使用仓库对话框返回
					return;
				}
			}
			int k = 0;
			while (k<10){//最多翻10页
				//等待弹出要存放的宠物列表信息
				windowinfo.windowtype = -1;
				windowinfo.buttontype = -1;
				starttime = GetTickCount();
				while ((GetTickCount() - starttime)<5000){
					if (windowinfo.windowtype == WINDOW_MESSAGETYPE_SELECT && strstr(windowinfo.data, "请选择") != NULL){
						break;
					}
					Sleep(20);
				}
				if (windowinfo.windowtype == WINDOW_MESSAGETYPE_SELECT  && strstr(windowinfo.data, "请选择") != NULL){
					int j;
					//解析宠物信息
					for (j = 0; j<5; j++){
						load_pet[j].lv = 0;
						load_pet[j].maxhp = 0;
						ZeroMemory(load_pet[j].name, sizeof(load_pet[j].name));
					}
					ParsePetData(windowinfo.data);
					for (j = 0; j<5; j++){
						if (strstr(load_pet[j].name, buf) != NULL){
							itoa(j + 1, tmp, 10);
							//发送要存的宠物
							SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, 0, tmp);
							break;
						}
					}
					if (j<5){//有要取宠物
						//确认存储谁
						starttime = GetTickCount();
						while ((GetTickCount() - starttime)<5000){
							if (windowinfo.windowtype == WINDOW_MESSAGETYPE_MESSAGE && windowinfo.buttontype == WINDOW_BUTTONTYPE_YESNO){
								break;
							}
							Sleep(20);
						}
						if (windowinfo.windowtype == WINDOW_MESSAGETYPE_MESSAGE && windowinfo.buttontype == WINDOW_BUTTONTYPE_YESNO){
							SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, 4, "");
							//希望你再度光临
							starttime = GetTickCount();
							while ((GetTickCount() - starttime)<5000){
								if (windowinfo.windowtype == WINDOW_MESSAGETYPE_MESSAGE && windowinfo.buttontype == WINDOW_BUTTONTYPE_OK){
									break;
								}
								Sleep(20);
							}
							if (windowinfo.windowtype == WINDOW_MESSAGETYPE_MESSAGE && windowinfo.buttontype == WINDOW_BUTTONTYPE_OK){
								SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, 1, "");
								break;//取出宠物后退出
							}
						}
					}
					else if (windowinfo.buttontype&WINDOW_BUTTONTYPE_NEXT){//无要取宠物，发送下页信息
						SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, WINDOW_BUTTONTYPE_NEXT, "");
						k++;
					}
					else{//按下取消按钮
						SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, WINDOW_BUTTONTYPE_CANCEL, "");
						break;//没有找到要取出的宠物后退出
					}
				}//if
			}//while(true)
		}
		else{
			break;
		}
		Tokenize(item_buf, buf, "|", t);
	}
}

//存道具到寄存店
void CDpMain::SaveItem(int towhere, CString item)
{
	int pos, i, len, t, t1;
	char buf[255], item_buf[255], tmp[10];
	//消息窗口结构置空
	windowinfo.windowtype = -1;
	windowinfo.buttontype = -1;
	SendTalk(charotherinfo.x, charotherinfo.y, "存", 0, 3);
	int starttime;
	//等待弹出道具窗口
	starttime = GetTickCount();
	while ((GetTickCount() - starttime)<5000){
		if (windowinfo.windowtype == WINDOW_MESSAGETYPE_SELECT && windowinfo.buttontype == WINDOW_BUTTONTYPE_NONE){
			break;
		}
		Sleep(20);
	}
	if (windowinfo.windowtype == WINDOW_MESSAGETYPE_SELECT && windowinfo.buttontype == WINDOW_BUTTONTYPE_NONE){
		//发送寄放道具信息
		if (towhere == 0)
			SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, 0, "1");
		else{
			SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, 0, "3");
			windowinfo.windowtype = -1;
			windowinfo.buttontype = -1;
			//等待弹出使用仓库对话框
			starttime = GetTickCount();
			while ((GetTickCount() - starttime)<5000){
				if (windowinfo.windowtype == WINDOW_MESSAGETYPE_SELECT && windowinfo.buttontype == WINDOW_BUTTONTYPE_CANCEL){
					break;
				}
				Sleep(20);
			}
			if (windowinfo.windowtype == WINDOW_MESSAGETYPE_SELECT && windowinfo.buttontype == WINDOW_BUTTONTYPE_CANCEL){
				SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, 0, "1");
			}
			else{//没有弹出使用仓库对话框返回
				return;
			}
		}
		//等待弹出道具信息
		starttime = GetTickCount();
		while ((GetTickCount() - starttime)<5000){
			if (windowinfo.windowtype == CHAR_WINDOWTYPE_SELECTRENAMEITEM_PAGE4 && windowinfo.buttontype == WINDOW_BUTTONTYPE_NONE){
				break;
			}
			Sleep(20);
		}
		if (windowinfo.windowtype == CHAR_WINDOWTYPE_SELECTRENAMEITEM_PAGE4 && windowinfo.buttontype == WINDOW_BUTTONTYPE_NONE){
			//解析window信息
			len = strlen(windowinfo.data);
			pos = 0;
			Tokenize(windowinfo.data, buf, "|", pos);
			save_msg.unknown = atoi(buf);
			Tokenize(windowinfo.data, buf, "|", pos);
			save_msg.unknown1 = atoi(buf);
			Tokenize(windowinfo.data, buf, "|", pos);
			strcpy_s(save_msg.npcname, buf);
			Tokenize(windowinfo.data, buf, "|", pos);
			strcpy_s(save_msg.what_msg, buf);
			Tokenize(windowinfo.data, buf, "|", pos);
			strcpy_s(save_msg.itemfull_msg, buf);
			Tokenize(windowinfo.data, buf, "|", pos);
			strcpy_s(save_msg.really_msg, buf);
			for (i = 0; i<15; i++){
				ZeroMemory(save_msg.saveitem[i].name, sizeof(save_msg.saveitem[i].name));
				ZeroMemory(save_msg.saveitem[i].comment, sizeof(save_msg.saveitem[i].comment));
			}
			i = 0;
			while (pos<len){
				Tokenize(windowinfo.data, buf, "|", pos);
				strcpy_s(save_msg.saveitem[i].name, buf);
				Tokenize(windowinfo.data, buf, "|", pos);
				save_msg.saveitem[i].cansave = atoi(buf);
				Tokenize(windowinfo.data, buf, "|", pos);
				save_msg.saveitem[i].price = atoi(buf);
				Tokenize(windowinfo.data, buf, "|", pos);
				save_msg.saveitem[i].image = atoi(buf);
				Tokenize(windowinfo.data, buf, "|", pos);
				strcpy_s(save_msg.saveitem[i].comment, buf);
				Tokenize(windowinfo.data, buf, "|", pos);
				save_msg.saveitem[i].num = atoi(buf);
				Tokenize(windowinfo.data, buf, "|", pos);
				save_msg.saveitem[i].position = atoi(buf);
				i++;
			}
			t = 0;
			sprintf_s(item_buf, "%s", item);
			Tokenize(item_buf, buf, "|", t);
			while (strlen(buf)>0){
				if (IsNumber(buf)){
					t1 = atoi(buf) + 9;
					itoa(t1, buf, 10);
					//发送要存的道具
					SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, 0, buf);
				}
				else{
					for (int j = 9; j<24; j++){
						if (strstr(iteminfo[j].name, buf) != NULL || strstr(iteminfo[j].comment, buf) != NULL){
							itoa(j + 1, tmp, 10);
							//发送要存的道具
							SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, 0, tmp);
						}
					}
				}
				Tokenize(item_buf, buf, "|", t);
			}
		}
	}
}

//从寄存店取道具,取前5项getitem 寄存|仓库,1|1|1|1|1
void CDpMain::GetItem(int fromwhere, CString item)
{
	int pos, i, len, t, t1, n;
	char buf[255], item_buf[255], tmp[10];
	//消息窗口结构置空
	windowinfo.windowtype = -1;
	windowinfo.buttontype = -1;
	SendTalk(charotherinfo.x, charotherinfo.y, "取", 0, 3);
	int starttime;
	//等待弹出道具窗口
	starttime = GetTickCount();
	while ((GetTickCount() - starttime)<5000){
		if (windowinfo.windowtype == WINDOW_MESSAGETYPE_SELECT && windowinfo.buttontype == WINDOW_BUTTONTYPE_NONE){
			break;
		}
		Sleep(20);
	}
	if (windowinfo.windowtype == WINDOW_MESSAGETYPE_SELECT && windowinfo.buttontype == WINDOW_BUTTONTYPE_NONE){
		//发送寄放道具信息
		if (fromwhere == 0)
			SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, 0, "2");
		else{
			SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, 0, "3");
			windowinfo.windowtype = -1;
			windowinfo.buttontype = -1;
			//等待弹出使用仓库对话框
			starttime = GetTickCount();
			while ((GetTickCount() - starttime)<5000){
				if (windowinfo.windowtype == WINDOW_MESSAGETYPE_SELECT && windowinfo.buttontype == WINDOW_BUTTONTYPE_CANCEL){
					break;
				}
				Sleep(20);
			}
			if (windowinfo.windowtype == WINDOW_MESSAGETYPE_SELECT && windowinfo.buttontype == WINDOW_BUTTONTYPE_CANCEL){
				SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, 0, "2");
			}
			else{//没有弹出使用仓库对话框返回
				return;
			}
		}
		//等待弹出道具信息
		starttime = GetTickCount();
		while ((GetTickCount() - starttime)<5000){
			if (windowinfo.windowtype == CHAR_WINDOWTYPE_SELECTRENAMEITEM_PAGE4 && windowinfo.buttontype == WINDOW_BUTTONTYPE_NONE){
				break;
			}
			Sleep(20);
		}
		if (windowinfo.windowtype == CHAR_WINDOWTYPE_SELECTRENAMEITEM_PAGE4 && windowinfo.buttontype == WINDOW_BUTTONTYPE_NONE){
			//解析window信息
			len = strlen(windowinfo.data);
			pos = 0;
			Tokenize(windowinfo.data, buf, "|", pos);
			load_msg.unknown = atoi(buf);
			Tokenize(windowinfo.data, buf, "|", pos);
			strcpy_s(load_msg.npcname, buf);
			Tokenize(windowinfo.data, buf, "|", pos);
			strcpy_s(load_msg.what_msg, buf);
			Tokenize(windowinfo.data, buf, "|", pos);
			strcpy_s(load_msg.itemfull_msg, buf);
			Tokenize(windowinfo.data, buf, "|", pos);
			strcpy_s(load_msg.really_msg, buf);
			for (i = 0; i<50; i++){
				ZeroMemory(load_msg.loaditem[i].name, sizeof(load_msg.loaditem[i].name));
				ZeroMemory(load_msg.loaditem[i].comment, sizeof(load_msg.loaditem[i].comment));
			}
			i = 0;
			while (pos<len){
				Tokenize(windowinfo.data, buf, "|", pos);
				strcpy_s(load_msg.loaditem[i].name, buf);
				Tokenize(windowinfo.data, buf, "|", pos);
				load_msg.loaditem[i].unknown = atoi(buf);
				Tokenize(windowinfo.data, buf, "|", pos);
				load_msg.loaditem[i].unknown1 = atoi(buf);
				Tokenize(windowinfo.data, buf, "|", pos);
				load_msg.loaditem[i].unknown2 = atoi(buf);
				Tokenize(windowinfo.data, buf, "|", pos);
				load_msg.loaditem[i].image = atoi(buf);
				Tokenize(windowinfo.data, buf, "|", pos);
				strcpy_s(load_msg.loaditem[i].comment, buf);
				Tokenize(windowinfo.data, buf, "|", pos);
				load_msg.loaditem[i].num = atoi(buf);
				i++;
			}
			t = 0;
			sprintf_s(item_buf, "%s", item);
			Tokenize(item_buf, buf, "|", t);
			while (strlen(buf)>0){
				if (IsNumber(buf)){
					t1 = atoi(buf);
					//发送要取的道具,取完第1项后，第2项变第1项
					SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, 0, buf);
				}
				else{
					n = 0;
					for (int j = 0; j<50; j++){
						//计算当前道具是第几项
						if (strlen(load_msg.loaditem[j].name)>0)
							n++;
						if (strstr(load_msg.loaditem[j].name, buf) != NULL || strstr(load_msg.loaditem[j].comment, buf) != NULL){
							itoa(n, tmp, 10);
							SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, 0, tmp);
							ZeroMemory(load_msg.loaditem[j].name, sizeof(load_msg.loaditem[j].name));
							ZeroMemory(load_msg.loaditem[j].comment, sizeof(load_msg.loaditem[j].comment));
							n--;
						}
					}
				}
				Tokenize(item_buf, buf, "|", t);
			}
		}
	}
}

//购道具，buy item1|2,item2|1
void CDpMain::BuyItem(CString item)
{
	//消息窗口结构置空
	windowinfo.windowtype = -1;
	windowinfo.buttontype = -1;
	SendTalk(charotherinfo.x, charotherinfo.y, "买", 0, 3);
	int starttime;
	//等待弹出欢迎窗口
	//starttime=GetTickCount();
	//while((GetTickCount()-starttime)<2000){
	//	if(windowinfo.windowtype==WINDOW_MESSAGETYPE_ITEMSHOPMENU && windowinfo.buttontype==WINDOW_BUTTONTYPE_NONE){
	//		break;
	//	}
	//	Sleep(100);
	//}
	////弹出欢迎窗口
	//if(windowinfo.windowtype==WINDOW_MESSAGETYPE_ITEMSHOPMENU && windowinfo.buttontype==WINDOW_BUTTONTYPE_NONE){
	//	//选买按钮
	//	SendSelectWindowButton(charotherinfo.x,charotherinfo.y,windowinfo.seqno,windowinfo.objindex,0,"1");
	//等待弹出商品清单
	starttime = GetTickCount();
	while ((GetTickCount() - starttime)<5000){
		if (windowinfo.windowtype == WINDOW_MESSAGETYPE_ITEMSHOPMAIN && windowinfo.buttontype == WINDOW_BUTTONTYPE_NONE){
			break;
		}
		Sleep(20);
	}
	//弹出了商品清单
	if (windowinfo.windowtype == WINDOW_MESSAGETYPE_ITEMSHOPMAIN && windowinfo.buttontype == WINDOW_BUTTONTYPE_NONE){
		int pos = 0;
		char buf[200] = { 0 };
		//分析商品清单
		Tokenize(windowinfo.data, buf, "|", pos);
		buy_msg.unknown = atoi(buf);
		Tokenize(windowinfo.data, buf, "|", pos);
		buy_msg.dataflg = atoi(buf);
		if (buy_msg.dataflg == 1){
			Tokenize(windowinfo.data, buf, "|", pos);
			buy_msg.windowtype = atoi(buf);
			Tokenize(windowinfo.data, buf, "|", pos);
			strcpy_s(buy_msg.main_msg, buf);
			Tokenize(windowinfo.data, buf, "|", pos);
			strcpy_s(buy_msg.buy_main, buf);
			Tokenize(windowinfo.data, buf, "|", pos);
			strcpy_s(buy_msg.what_msg, buf);
			Tokenize(windowinfo.data, buf, "|", pos);
			strcpy_s(buy_msg.howmany_msg, buf);
			Tokenize(windowinfo.data, buf, "|", pos);
			strcpy_s(buy_msg.level_msg, buf);
			Tokenize(windowinfo.data, buf, "|", pos);
			strcpy_s(buy_msg.really_msg, buf);
			Tokenize(windowinfo.data, buf, "|", pos);
			strcpy_s(buy_msg.itemfull_msg, buf);
			buy_msg.num = 0;
			Tokenize(windowinfo.data, buf, "|", pos);
			while (strlen(buf)>0){
				strcpy_s(buy_msg.item[buy_msg.num].name, buf);
				Tokenize(windowinfo.data, buf, "|", pos);
				buy_msg.item[buy_msg.num].unknown = atoi(buf);
				Tokenize(windowinfo.data, buf, "|", pos);
				buy_msg.item[buy_msg.num].uselevel = atoi(buf);
				Tokenize(windowinfo.data, buf, "|", pos);
				buy_msg.item[buy_msg.num].price = atoi(buf);
				Tokenize(windowinfo.data, buf, "|", pos);
				buy_msg.item[buy_msg.num].image = atoi(buf);
				Tokenize(windowinfo.data, buf, "|", pos);
				strcpy_s(buy_msg.item[buy_msg.num].comment, buf);
				Tokenize(windowinfo.data, buf, "|", pos);
				buy_msg.item[buy_msg.num].costfame = atoi(buf);
				Tokenize(windowinfo.data, buf, "|", pos);
				buy_msg.num++;
			}
		}
		//分析要购买的东西
		pos = 0;
		int itemindex, num, i = 0;
		CString szVal, szItemname, szNum;
		szVal = item.Tokenize(",", pos);
		while (szVal.GetLength()>0){
			i = 0;
			szItemname = szVal.Tokenize("|", i);
			szNum = szVal.Tokenize("|", i);
			if (szNum.IsEmpty())
				num = 1;
			else if (IsNumber(szNum))
				num = atoi(szNum);
			else if (szNum.GetAt(0) == '@'){
				intvar.Lookup(szNum, num);
			}
			else
				num = 1;
			if (num<0)
				num = 1;
			if (IsNumber(szItemname))
				itemindex = atoi(szItemname);
			else{
				for (i = 0; i<buy_msg.num; i++){
					if (strstr(buy_msg.item[i].name, szItemname) != NULL || strstr(buy_msg.item[i].comment, szItemname) != NULL)
						break;
				}
				if (i >= buy_msg.num)
					itemindex = -1;
				else
					itemindex = i + 1;
			}
			//在规定物品范围内购买
			if (itemindex >= 1 && itemindex <= buy_msg.num){
				sprintf_s(buf, "%d\\z%d", itemindex, num);
				SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, 0, buf);
			}
			szVal = item.Tokenize(",", pos);
		}
	}
	//}
}

//卖宠物,sellpet {编号1|编号2....或宠物1|宠物2|....}
void CDpMain::SellPet(CString item)
{
	int t;
	char buf[255], item_buf[255], tmp[10];
	//消息窗口结构置空
	windowinfo.windowtype = -1;
	windowinfo.buttontype = -1;
	SendTalk(charotherinfo.x, charotherinfo.y, "卖", 0, 3);
	int starttime;
	t = 0;
	sprintf_s(item_buf, "%s", item);
	Tokenize(item_buf, buf, "|", t);
	while (strlen(buf)>0){
		//等待弹出存放宠物窗口
		starttime = GetTickCount();
		while ((GetTickCount() - starttime)<5000){
			if (windowinfo.windowtype == WINDOW_MESSAGETYPE_SELECT && windowinfo.buttontype == WINDOW_BUTTONTYPE_NONE){
				break;
			}
			Sleep(10);
		}
		if (windowinfo.windowtype == WINDOW_MESSAGETYPE_SELECT && windowinfo.buttontype == WINDOW_BUTTONTYPE_NONE){
			SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, 0, "3");
			windowinfo.windowtype = -1;
			windowinfo.buttontype = -1;
			//等待弹出要存放的宠物列表信息
			starttime = GetTickCount();
			while ((GetTickCount() - starttime)<5000){
				if (windowinfo.windowtype == WINDOW_MESSAGETYPE_PETSELECT && windowinfo.buttontype == WINDOW_BUTTONTYPE_CANCEL){
					break;
				}
				Sleep(10);
			}
			if (windowinfo.windowtype == WINDOW_MESSAGETYPE_PETSELECT && windowinfo.buttontype == WINDOW_BUTTONTYPE_CANCEL){
				if (IsNumber(buf)){
					//发送要卖的宠物
					SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, 0, buf);
				}
				else{
					for (int j = 0; j<5; j++){
						if (strstr(petdetail[j].oldname, buf) != NULL || strstr(petdetail[j].newname, buf) != NULL){
							itoa(j + 1, tmp, 10);
							//发送要卖的宠物
							SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, 0, tmp);
							break;
						}
					}
				}
				//确认存储谁
				windowinfo.windowtype = -1;
				windowinfo.buttontype = -1;
				starttime = GetTickCount();
				while ((GetTickCount() - starttime)<5000){
					if (windowinfo.windowtype == WINDOW_MESSAGETYPE_MESSAGE && windowinfo.buttontype == WINDOW_BUTTONTYPE_YESNO){
						break;
					}
					Sleep(10);
				}
				if (windowinfo.windowtype == WINDOW_MESSAGETYPE_MESSAGE && windowinfo.buttontype == WINDOW_BUTTONTYPE_YESNO){
					SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, 4, "");
					//希望你再度光临
					windowinfo.windowtype = -1;
					windowinfo.buttontype = -1;
					starttime = GetTickCount();
					while ((GetTickCount() - starttime)<5000){
						if (windowinfo.windowtype == WINDOW_MESSAGETYPE_MESSAGE && windowinfo.buttontype == WINDOW_BUTTONTYPE_OK){
							break;
						}
						Sleep(10);
					}
					if (windowinfo.windowtype == WINDOW_MESSAGETYPE_MESSAGE && windowinfo.buttontype == WINDOW_BUTTONTYPE_OK){
						SendSelectWindowButton(charotherinfo.x, charotherinfo.y, windowinfo.seqno, windowinfo.objindex, 1, "");
					}
				}
			}
		}
		else{
			break;
		}
		Tokenize(item_buf, buf, "|", t);
	}

}

//移动交换道具，moveitem 目标道具（位置|名称）,移动到位置
//头，身，武器，左饰，右饰，腰，盾，脚，手套（0-8），物品栏按顺序存放（9-23）
void CDpMain::MoveItem(CString para)
{
	CString szItem, szPos;
	int pos = 0, from = 0, to = 0, i;
	szItem = para.Tokenize(",", pos);
	if (szItem.IsEmpty())
		return;
	szPos = para.Tokenize(",", pos);
	if (szPos.IsEmpty() || !IsNumber(szPos))
		return;
	//计算道位置
	if (IsNumber(szItem)){
		from = atoi(szItem);
		switch (from){
		case 101://头
			from = 0;
			break;
		case 102://身
			from = 1;
			break;
		case 103://武器
			from = 2;
			break;
		case 104://左饰
			from = 3;
			break;
		case 105://右饰
			from = 4;
			break;
		case 106://腰
			from = 5;
			break;
		case 107://盾
			from = 6;
			break;
		case 108://脚
			from = 7;
			break;
		case 109://手套
			from = 8;
			break;
		default:
			from += 8;
			break;
		}
	}
	else{
		for (i = 9; i<24; i++){
			if (strstr(iteminfo[i].name, szItem) != NULL || strstr(iteminfo[i].comment, szItem) != NULL)
				break;
		}
		if (i >= 24)
			return;
		else
			from = i;
	}
	if (from<0 || from>23)
		return;
	to = atoi(szPos);
	switch (to){
	case 101://头
		to = 0;
		break;
	case 102://身
		to = 1;
		break;
	case 103://武器
		to = 2;
		break;
	case 104://左饰
		to = 3;
		break;
	case 105://右饰
		to = 4;
		break;
	case 106://腰
		to = 5;
		break;
	case 107://盾
		to = 6;
		break;
	case 108://脚
		to = 7;
		break;
	case 109://手套
		to = 8;
		break;
	default://道具栏1-15
		to += 8;
		break;
	}
	if (to<0 || to>23)
		return;
	SendMoveItem(from, to);
}


#pragma endregion 

#pragma endregion






