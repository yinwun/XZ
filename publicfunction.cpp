#include "stdafx.h"
#include "publicfunction.h"
#include <math.h>
#include "malloc.h"
#include "iphlpapi.h"
#pragma   comment(lib, "Iphlpapi.lib ")

int GetScriptLinsNum(char *path)
{
	if(FileExist(path)){
		int i=0;
		CString szVal;
		CStdioFile f(path,CFile::modeRead|CFile::typeText|CFile::shareDenyNone);
		while(f.ReadString(szVal)){
			i++;
		}
		f.Close();
		return i;
	}
	else
		return -1;
}

//���ܺ���
CString Encrypt(CString s)
{
	CString v,p;
	const int CREATE_TYPE = 8;
	TCHAR bTemp;
	int n;
	for(int i=0;i<s.GetLength();i++){
		bTemp = (TCHAR)s.GetAt(i);
		n=bTemp % CREATE_TYPE;
		switch(n)
		{
			case 1:
				// 00000001
				bTemp =(TCHAR)( bTemp ^ 0x01 );
				break;
			case 2:
				// 00000010
				bTemp =(TCHAR)( bTemp ^ 0x02 );
				break;
			case 3:
				// 00000100
				bTemp =(TCHAR)( bTemp ^ 0x04 );
				break;
			case 4:
				// 00001000
				bTemp =(TCHAR)( bTemp ^ 0x08 );
				break;
			case 5:
				// 00010000
				bTemp =(TCHAR)( bTemp ^ 0x10 );
				break;
			case 6:
				// 00100000
				bTemp =(TCHAR)( bTemp ^ 0x20 );
				break;
			case 7:
				// 01000000
				bTemp =(TCHAR)( bTemp ^ 0x40 );
				break;
			default:
				// 10000000
				bTemp =(TCHAR)( bTemp ^ 0x80 );
				break;
		}
		p.Format(_T("%d"),n);
		v+=p+CString(bTemp);
	}
	return v;
}

//���ܺ���
CString Decrypt(CString s)
{
	CString v;
	const int CREATE_TYPE = 8;
	TCHAR bn,bTemp;
	if(s.GetLength()%2!=0)
		s.Left(s.GetLength()-1);
	for(int i=0;i<s.GetLength();i++){
		bn = (TCHAR)s.GetAt(i);
		int t=_ttoi(CString(bn));
		i++;
		bTemp=(TCHAR)s.GetAt(i);
		switch(t)
		{
			case 1:
				// 00000001
				bTemp =(TCHAR)( bTemp ^ 0x01 );
				break;
			case 2:
				// 00000010
				bTemp =(TCHAR)( bTemp ^ 0x02 );
				break;
			case 3:
				// 00000100
				bTemp =(TCHAR)( bTemp ^ 0x04 );
				break;
			case 4:
				// 00001000
				bTemp =(TCHAR)( bTemp ^ 0x08 );
				break;
			case 5:
				// 00010000
				bTemp =(TCHAR)( bTemp ^ 0x10 );
				break;
			case 6:
				// 00100000
				bTemp =(TCHAR)( bTemp ^ 0x20 );
				break;
			case 7:
				// 01000000
				bTemp =(TCHAR)( bTemp ^ 0x40 );
				break;
			default:
				// 10000000
				bTemp =(TCHAR)( bTemp ^ 0x80 );
				break;
		}
		v+=CString(bTemp);
	}
	return v;
}

//���Ŀ¼�Ƿ����
BOOL DirExist(LPCSTR lpszDirName)
{
	DWORD attr; 
    attr = GetFileAttributes(lpszDirName); 
    if( (attr != (DWORD)(-1)) && ( attr & FILE_ATTRIBUTE_DIRECTORY))
		return TRUE;
	else
		return FALSE;
}

//�ļ��Ƿ����
BOOL FileExist(LPCSTR lpszFileName)
{
	DWORD attr; 
    attr = GetFileAttributes(lpszFileName); 
    if( (attr != (DWORD)(-1)) && !( attr & FILE_ATTRIBUTE_DIRECTORY))
		return TRUE;
	else
		return FALSE;
}

/********************************************************************************
* Function Type	:	public
* Description	:	��ȡ��ǰʱ����ַ���,�磺2003-10-01 12:00:00
*********************************************************************************/
CString GetCurTimeString ()
{
	time_t tNow=time(NULL);
	CTime cTime( tNow );
	return cTime.Format (_T("%Y-%m-%d %H:%M:%S"));
}

/********************************************************************************
* Function Type	:	Global
* Parameter		:	lpszFormat	-	Ҫ��¼����־����
* Return Value	:	�޷���ֵ
* Description	:	��¼��־
*********************************************************************************/
CCriticalSection f_CSFor_DbgLog;
void DbgLog ( LPCSTR lpszFormat, ... )
{
	// ��ʽ��
	f_CSFor_DbgLog.Lock ();
	char szLogBuf[1024*4] = {0};
	char *p = szLogBuf;
	*p = '[';
	p ++;
	CString csDate=GetCurTimeString();
	int nLen = csDate.GetLength();
	strcpy_s(p,nLen+1,(LPCSTR)csDate);
	p += nLen;
	*p = ']';
	p ++;
	*p = ' ';
	p ++;
	
	va_list  va;
	va_start (va, lpszFormat);
	nLen=sizeof(szLogBuf)/2-(int)(p-szLogBuf);
	vsprintf_s( p,nLen,lpszFormat, va);
	va_end(va);
	//char buf[1024*4]={0};
	//WideCharToMultiByte(CP_ACP,0,szLogBuf,-1,buf,sizeof(buf),NULL,NULL);
	WriteDataToFile ( "log.txt", szLogBuf, strlen(szLogBuf), "ab+");
	f_CSFor_DbgLog.Unlock ();
}

/********************************************************************************
* Function Type	:	Global
* Parameter		:	filename		-	�ļ���
*					data			-	Ҫ���������
*					mode			-	�ļ��򿪵�ģʽ
*					size			-	���ݴ�С
*					nStartPos		-	�ļ���ʼλ��
* Return Value	:	>=0				-	д���ļ��Ĵ�С
*					-1				-	д����ʧ��
* Description	:	�������ݵ��ļ�
*********************************************************************************/
int WriteDataToFile(LPCSTR filename,char* data,long size,LPCSTR mode, int nStartPos/*=-1*/ )
{
	//ASSERT ( filename && strlen(filename) > 0 );
	FILE *fp;
	long retval;
	errno_t err;
	err=fopen_s(&fp,filename,mode);
	if ( err==0)
	{
		if ( nStartPos >= 0 )
		{
			if ( fseek ( fp, nStartPos, SEEK_SET ) != 0 )
				return -1;
		}
		retval = (long)fwrite(data,sizeof(UCHAR),size,fp);
		fclose(fp);
		if(retval != size)
		{
			return -1;
		}
		else 	return retval;
	}
	else
	{
		return -1;
	}
}

int ConnectServer(SOCKET &rsocket,char *ip,int port)
{
	int nRet = -1;
	BOOL bConnected = FALSE;

	rsocket= socket(AF_INET, SOCK_STREAM, 0);
	//�׽��ִ���ʧ�ܣ�
    if(rsocket==INVALID_SOCKET)		
		return -1;
	sockaddr_in servAddr;
	servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(port);	
    servAddr.sin_addr.s_addr = inet_addr(ip);

	//���ý��ճ�ʱʱ��Ϊ35��
	int nTimeout = 35000;
	if(setsockopt(rsocket,SOL_SOCKET,SO_RCVTIMEO,(char*)&nTimeout,sizeof(nTimeout)) == SOCKET_ERROR)
		return -2;
	//�ѵ�ǰ�׽�����Ϊ������ģʽ
	unsigned long nFlag = 1;
	nRet = ioctlsocket(rsocket,FIONBIO,(unsigned long*)&nFlag);
	if(nRet == SOCKET_ERROR)//�ѵ�ǰ�׽�����Ϊ������ģʽʧ��!
		return -3;
	//������ģʽ��ִ��I/O����ʱ��Winsock�������������ز���������Ȩ������ģʽʹ�������Ƚϸ��ӣ�
	//��Ϊ������û��������ɾͽ��з��أ��᲻�ϵط���WSAEWOULDBLOCK����
	if(connect(rsocket,(struct sockaddr *)&servAddr,sizeof(servAddr))==SOCKET_ERROR){	
		struct timeval timeout = {0};
		timeout.tv_sec = 10;	//���ӳ�ʱʱ��Ϊ10��,��ֵ��С����ɶ��߳�ͬʱ���ӷ����ʱ���޷��������Ӷ�ͨ��ʧ��
		timeout.tv_usec = 0;	//�����������ԭ�����̵߳�ִ��˳���ǲ���Ԥ֪��

		char *recvBuf;
		recv(rsocket, recvBuf, 50, 0);//��������


		fd_set fdWrite;
		FD_ZERO(&fdWrite);
		FD_SET(rsocket,&fdWrite);

		int nError = -1;
		int nLen = sizeof(int);	
		nRet = select(rsocket,0,&fdWrite,0,&timeout);
		if (nRet > 0)
		{
			getsockopt(rsocket,SOL_SOCKET,SO_ERROR,(char*)&nError,&nLen);
			if (nError != 0)
				bConnected = FALSE;
			else
				bConnected = TRUE;
		}
		else
			bConnected = FALSE; 
	}
	//�����û�����ģʽ
	nFlag = 0;
	ioctlsocket(rsocket,FIONBIO,(unsigned long*)&nFlag);
	//������ʧ���򷵻�
	if(bConnected==FALSE)//���������������ʧ�ܣ�
		return -4;	
	return 1;
}

//���ش�nstart��ʼ��token��source�г���λ��֮����ַ���
void Tokenize(char *source,char *dest,char *token,int &nstart)
{
	char *p,*p1=NULL;
	//Ҫ�����ַ����ĳ���
	int len=strlen(token);
	//���ַ�������
	int totallen=strlen(source);
	//�����ʼ����λ�ô��ڵ���ԭʼ�ַ������ȣ��򷵻�
	if(nstart>=totallen){
		dest[0]=0;
		return;
	}
	//���忪ʼ���ҵ�λ��
	p=source+nstart;
	//�ҳ�token���ַ�����λ��
	p1=strstr(p,token);
	//����ҵ�
	if(p1){
		//��һ����ʼλ��
		nstart=nstart+(p1-p)+len;
		//�����ӷ�����dest��
		strncpy(dest,p,p1-p);
		dest[p1-p]=0;
		return;
	}
	//�������һ����
	strncpy(dest,p,strlen(p));
	dest[strlen(p)]=0;
	nstart+=strlen(p);
}

//�ҳ�substr��src�е�λ�ã�lenΪsrc���ܳ���
int mystrstr(char *src,char *substr,int len)
{
	int i=0;
	char *p;
	p=src;
	while(i++,*p!=*substr && i<=len){
		p++;	
	}
	if(i<=len)
		return i;
	else
		return 0;
}

//ʮ�������ַ���ת��Ϊʮ������
unsigned int HexStrToDec(char *data)
{
	unsigned int result =0;
	unsigned maxPos = strlen(data);
	unsigned tv;
	for(unsigned int index=0;index < maxPos; index++)
	{
		tv = data[index];
		if(isalpha(tv))
		{
			if(islower(tv))tv=toupper(tv);
			tv -= char('A');
			tv+= 10;			
		}
		else if(isdigit(tv))
		{  
			tv -= char('0');			
		}
		else 
			return unsigned (-1); // invalid string
		result +=(unsigned int) tv * pow((double)16, (int)(maxPos-index-1)); // result = sum * step + tv;
	}
	return result;

}

//�Ѱٷֱ��ַ���ת��ΪС��
double PerStrToDbl(CString &data)
{
	CString szStr;
	if(data.Right(1).Compare("%")==0)
		szStr=data.Left(data.GetLength()-1);
	return atof(szStr);
}

//�Ƿ�����ֵ
BOOL IsNumber(CString text)
{
	CString left,right;
	int pos=text.Find(_T("."));
	if(pos>0){
		left=text.Left(pos);
		right=text.Right(text.GetLength()-pos-1);
		for(int i=0;i<left.GetLength();i++){
			if(i==0){
				if(left.GetAt(i)=='+' || left.GetAt(i)=='-' || IsDigit(left.GetAt(i)))
					continue;
			}else{
				if(!IsDigit(left.GetAt(i)))
					return FALSE;
			}
		}
		for(int i=0;i<right.GetLength();i++){
			if(!IsDigit(right.GetAt(i)))
				return FALSE;
		}
	}
	else{
		for(int i=0;i<text.GetLength();i++){
			if(i==0){
				if(text.GetAt(i)=='+' || text.GetAt(i)=='-' || IsDigit(text.GetAt(i)))
					continue;
			}else{
				if(!IsDigit(text.GetAt(i)))
					return FALSE;
			}
		}
	}
	return TRUE;
}

BOOL IsDigit(char c)
{
	if(c>='0' && c<='9')
		return TRUE;
	else
		return FALSE;
}

int Myatoi(CString text)
{
	if(text.GetAt(0)=='+')
		return atoi(text.Right(text.GetLength()-1));
	else if(text.GetAt(0)=='-')
		return -1 * atoi(text.Right(text.GetLength()-1));
	else
		return atoi(text);
}
//��  �ܣ���lpszSour�в����ַ���lpszFind��lpszFind�п��԰���ͨ���ַ���?��
//��  ����nStartΪ��lpszSour�е���ʼ����λ��
//����ֵ���ɹ�����ƥ��λ�ã����򷵻�-1
//ע  �⣺Called by ��bool MatchingString()��
int FindingString(const char* lpszSour, const char* lpszFind, int nStart /* = 0 */)
{
//	ASSERT(lpszSour && lpszFind && nStart >= 0);
	if(lpszSour == NULL || lpszFind == NULL || nStart < 0)
		return -1;

	int m = strlen(lpszSour);
	int n = strlen(lpszFind);

	if( nStart+n > m )
		return -1;

	if(n == 0)
		return nStart;

//KMP�㷨
	int* next = new int[n];
	//�õ������ַ�����next����
	{	n--;

		int j, k;
		j = 0;
		k = -1;
		next[0] = -1;

		while(j < n)
		{	if(k == -1 || lpszFind[k] == '?' || lpszFind[j] == lpszFind[k])
			{	j++;
				k++;
				next[j] = k;
			}
			else
				k = next[k];
		}

		n++;
	}

	int i = nStart, j = 0;
	while(i < m && j < n)
	{
		if(j == -1 || lpszFind[j] == '?' || lpszSour[i] == lpszFind[j])
		{	i++;
			j++;
		}
		else
			j = next[j];
	}

	delete []next;

	if(j >= n)
		return i-n;
	else
		return -1;
}

//��	  �ܣ���ͨ������ַ���ƥ��
//��	  ����lpszSour��һ����ͨ�ַ�����
//			  lpszMatch��һ���԰���ͨ������ַ�����
//			  bMatchCaseΪ0�������ִ�Сд���������ִ�Сд��
//��  ��  ֵ��ƥ�䣬����1�����򷵻�0��
//ͨ������壺
//		��*��	���������ַ������������ַ�����
//		��?��	��������һ���ַ�������Ϊ�գ�
//ʱ	  �䣺	2001.11.02	13:00
bool MatchingString(const char* lpszSour, const char* lpszMatch, bool bMatchCase /*  = true */)
{
//	ASSERT(AfxIsValidString(lpszSour) && AfxIsValidString(lpszMatch));
	if(lpszSour == NULL || lpszMatch == NULL)
		return false;

	if(lpszMatch[0] == 0)//Is a empty string
	{
		if(lpszSour[0] == 0)
			return true;
		else
			return false;
	}

	int i = 0, j = 0;

	//���ɱȽ�����ʱԴ�ַ���'szSource'
	char* szSource =
		new char[ (j = strlen(lpszSour)+1) ];

	if( bMatchCase )
	{	//memcpy(szSource, lpszSour, j);
		while( *(szSource+i) = *(lpszSour+i++) );
	}
	else
	{	//Lowercase 'lpszSour' to 'szSource'
		i = 0;
		while(lpszSour[i])
		{	if(lpszSour[i] >= 'A' && lpszSour[i] <= 'Z')
				szSource[i] = lpszSour[i] - 'A' + 'a';
			else
				szSource[i] = lpszSour[i];

			i++;
		}
		szSource[i] = 0;
	}

	//���ɱȽ�����ʱƥ���ַ���'szMatcher'
	char* szMatcher = new char[strlen(lpszMatch)+1];

	//��lpszMatch���������ġ�*������һ����*�����Ƶ�szMatcher��
	i = j = 0;
	while(lpszMatch[i])
	{
		szMatcher[j++] = (!bMatchCase) ?
								( (lpszMatch[i] >= 'A' && lpszMatch[i] <= 'Z') ?//Lowercase lpszMatch[i] to szMatcher[j]
										lpszMatch[i] - 'A' + 'a' :
										lpszMatch[i]
								) :
								lpszMatch[i];		 //Copy lpszMatch[i] to szMatcher[j]
		//Merge '*'
		if(lpszMatch[i] == '*')
			while(lpszMatch[++i] == '*');
		else
			i++;
	}
	szMatcher[j] = 0;

	//��ʼ����ƥ����

	int nMatchOffset, nSourOffset;

	bool bIsMatched = true;
	nMatchOffset = nSourOffset = 0;
	while(szMatcher[nMatchOffset])
	{
		if(szMatcher[nMatchOffset] == '*')
		{
			if(szMatcher[nMatchOffset+1] == 0)
			{	//szMatcher[nMatchOffset]�����һ���ַ�

				bIsMatched = true;
				break;
			}
			else
			{	//szMatcher[nMatchOffset+1]ֻ����'?'����ͨ�ַ�

				int nSubOffset = nMatchOffset+1;

				while(szMatcher[nSubOffset])
				{	if(szMatcher[nSubOffset] == '*')
						break;
					nSubOffset++;
				}

				if( strlen(szSource+nSourOffset) <
						size_t(nSubOffset-nMatchOffset-1) )
				{	//Դ�ַ���ʣ�µĳ���С��ƥ�䴮ʣ��Ҫ�󳤶�
					bIsMatched = false; //�ж���ƥ��
					break;			//�˳�
				}

				if(!szMatcher[nSubOffset])//nSubOffset is point to ender of 'szMatcher'
				{	//���ʣ�²����ַ��Ƿ�һһƥ��

					nSubOffset--;
					int nTempSourOffset = strlen(szSource)-1;
					//�Ӻ���ǰ����ƥ��
					while(szMatcher[nSubOffset] != '*')
					{
						if(szMatcher[nSubOffset] == '?')
							;
						else
						{	if(szMatcher[nSubOffset] != szSource[nTempSourOffset])
							{	bIsMatched = false;
								break;
							}
						}
						nSubOffset--;
						nTempSourOffset--;
					}
					break;
				}
				else//szMatcher[nSubOffset] == '*'
				{	nSubOffset -= nMatchOffset;

					char* szTempFinder = new char[nSubOffset];
					nSubOffset--;
					memcpy(szTempFinder, szMatcher+nMatchOffset+1, nSubOffset);
					szTempFinder[nSubOffset] = 0;

					int nPos = ::FindingString(szSource+nSourOffset, szTempFinder, 0);
					delete []szTempFinder;

					if(nPos != -1)//��'szSource+nSourOffset'���ҵ�szTempFinder
					{	nMatchOffset += nSubOffset;
						nSourOffset += (nPos+nSubOffset-1);
					}
					else
					{	bIsMatched = false;
						break;
					}
				}
			}
		}		//end of "if(szMatcher[nMatchOffset] == '*')"
		else if(szMatcher[nMatchOffset] == '?')
		{
			if(!szSource[nSourOffset])
			{	bIsMatched = false;
				break;
			}
			if(!szMatcher[nMatchOffset+1] && szSource[nSourOffset+1])
			{	//���szMatcher[nMatchOffset]�����һ���ַ���
				//��szSource[nSourOffset]�������һ���ַ�
				bIsMatched = false;
				break;
			}
			nMatchOffset++;
			nSourOffset++;
		}
		else//szMatcher[nMatchOffset]Ϊ�����ַ�
		{
			if(szSource[nSourOffset] != szMatcher[nMatchOffset])
			{	bIsMatched = false;
				break;
			}
			if(!szMatcher[nMatchOffset+1] && szSource[nSourOffset+1])
			{	bIsMatched = false;
				break;
			}
			nMatchOffset++;
			nSourOffset++;
		}
	}

	delete []szSource;
	delete []szMatcher;
	return bIsMatched;
}

//��  �ܣ�����ƥ�䣬��ͬƥ���ַ���֮���á�,������
//			�磺��*.h,*.cpp��������ƥ�䡰*.h���͡�*.cpp��
//��  ����nMatchLogic = 0, ��ͬƥ�����else���룻bMatchCase, �Ƿ��С����
//����ֵ�����bRetReversed = 0, ƥ�䷵��true������ƥ�䷵��true
//ʱ  �䣺2001.11.02  17:00
bool MultiMatching(const char* lpszSour, const char* lpszMatch, int nMatchLogic /* = 0 */, bool bRetReversed /* = 0 */, bool bMatchCase /* = true */)
{
//	ASSERT(AfxIsValidString(lpszSour) && AfxIsValidString(lpszMatch));
	if(lpszSour == NULL || lpszMatch == NULL)
		return false;

	char* szSubMatch = new char[strlen(lpszMatch)+1];
	bool bIsMatch;

	if(nMatchLogic == 0)//���
	{	bIsMatch = 0;
		int i = 0;
		int j = 0;
		while(1)
		{	if(lpszMatch[i] != 0 && lpszMatch[i] != ',')
				szSubMatch[j++] = lpszMatch[i];
			else
			{	szSubMatch[j] = 0;
				if(j != 0)
				{
					bIsMatch = MatchingString(lpszSour, szSubMatch, bMatchCase);
					if(bIsMatch)
						break;
				}
				j = 0;
			}

			if(lpszMatch[i] == 0)
				break;
			i++;
		}
	}
	else//����
	{	bIsMatch = 1;
		int i = 0;
		int j = 0;
		while(1)
		{	if(lpszMatch[i] != 0 && lpszMatch[i] != ',')
				szSubMatch[j++] = lpszMatch[i];
			else
			{	szSubMatch[j] = 0;

				bIsMatch = MatchingString(lpszSour, szSubMatch, bMatchCase);
				if(!bIsMatch)
					break;

				j = 0;
			}

			if(lpszMatch[i] == 0)
				break;
			i++;
		}
	}

	delete []szSubMatch;

	if(bRetReversed)
		return !bIsMatch;
	else
		return bIsMatch;
}
//��ȫ������ת������ֵ
int myatoi(CString src)
{
	CString table[]={_T("��"),_T("��"),_T("��"),_T("��"),_T("��"),_T("��"),_T("��"),_T("��"),_T("��"),_T("��")};
	int i,j,val=0;
	CString szStr;
	for(i=0;i<(int)strlen(src);i++){
		szStr=src.Left(2);
		src=src.Right(strlen(src)-2);
		for(j=0;j<sizeof(table);j++){			
			if(szStr.Compare(table[j])==0){
				val=val*10+j;
				break;
			}
		}
	}
	return val;
}

CString GetDigitFormString(CString str)
{
	int i,n;	
	n=str.GetLength();
	i=0;
	while(i<n){
		if(IsDigit(str.GetAt(i)))
			break;
		i++;
	}
	return str.Right(n-i);
}
