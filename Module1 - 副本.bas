Attribute VB_Name = "Module1"
Public Const MAX_PATH = 260
Public Const INVALID_HANDLE_VALUE = -1

Public Const TRANSPARENT = 1


Public Const Decrypt_CO = &HF0

Public Const DIB_PAL_COLORS = 1 '  color table in palette indices
Public Const DIB_RGB_COLORS = 0 '  color table in RGBs
Public Const SRCCOPY = &HCC0020 ' (DWORD) dest = source
Public Const SRCPAINT = &HEE0086        ' (DWORD) dest = source OR dest
Public Const SRCINVERT = &H660046       ' (DWORD) dest = source XOR dest

Public Const WM_PAINT = &HF

Public Type SECURITY_ATTRIBUTES
        nLength As Long
        lpSecurityDescriptor As Long
        bInheritHandle As Long
End Type

Public Type BITMAPFILEHEADER
    bfType(1 To 2) As Byte
    bfSize As Long
    bfReserved1 As Integer
    bfReserved2 As Integer
    bfOffBits As Long
End Type

Public Type BITMAPINFOHEADER '40 bytes
        biSize As Long
        biWidth As Long
        biHeight As Long
        biPlanes As Integer
        biBitCount As Integer
        biCompression As Long
        biSizeImage As Long
        biXPelsPerMeter As Long
        biYPelsPerMeter As Long
        biClrUsed As Long
        biClrImportant As Long
End Type

Public Type RGBQUAD
        rgbBlue As Byte
        rgbGreen As Byte
        rgbRed As Byte
        rgbReserved As Byte
End Type

Public Type BITMAPINFO
        bmiHeader As BITMAPINFOHEADER
        bmiColors(1 To 256) As RGBQUAD
End Type

Public Type BrowseInfo
    hWndOwner As Long
    pIDLRoot As Long
    pszDisplayName As Long
    lpszTitle As Long
    ulFlags As Long
    lpfnCallback As Long
    lParam As Long
    iImage As Long
End Type

Public Type FILETIME
    dwLowDateTime As Long
    dwHighDateTime As Long
End Type

Public Type WIN32_FIND_DATA
    dwFileAttributes As Long
    ftCreationTime As FILETIME
    ftLastAccessTime As FILETIME
    ftLastWriteTime As FILETIME
    nFileSizeHigh As Long
    nFileSizeLow As Long
    dwReserved0 As Long
    dwReserved1 As Long
    cFileName As String * MAX_PATH
    cAlternate As String * 14
End Type

'自定义结构
Public Type adrn                    '长80字节
    Num As Long                     '序号; 图片的编号
    addr As Long                    '地址; 指明图片在数据文件中的起始位置
    datalen As Long                 '块长度; 图片数据块的大小
    X As Long                       '偏移量X; 显示图片时，横坐标偏移X
    Y As Long                       '偏移量Y; 显示图片时，纵坐标偏移Y
    width As Long                   '图片宽度; ...
    height As Long                  '图片高度; ...
    EastCover As Byte               '占地面积-东; 占地面积是物件所占的大小，1就表示占1格
    SouthCover As Byte              '占地面积-南; 同上
    ObstacleFlags As Byte           '标志; 用于地图，0表示障碍物，1表示可以走上去
    AdrnNotKnow(1 To 45) As Byte    '未知; 在StoneAge中本字段长度为45字节
    MapNum As Long                  '地图编号; 低16位表示在地图文件里的编号，高16位可能表示版本，非地图单位的此项均为0
End Type

'自定义结构
Public Type Real
    hwnd(1 To 2) As Byte            '魔数; 固定为'RD'
    Compress As Byte                '版本; 偶数表示未压缩，按位图存放；奇数则表示压缩过
    RealNotKnow As Byte             '未知; ...
    width As Long                   '宽度; ...
    height As Long                  '高度; ...
    datalen As Long                 '块长度; 数据块的长度，包括数据头本身的长度(16BYTE)
End Type

'自定义结构
Public Type Spradrn
    AnimationNum As Long            '序号; 动画序号
    addr As Long                    '地址; 指明在动画信息文件中的地址
    ActionNum As Integer            '动作数目; 表示该角色有多少个完整的动作(包括各个方向)
    SpradrnNotKnow As Integer       '未知;
End Type

'自定义结构
Public Type Spr
    Direction As Integer            '方向号; 0-7分别表示8个方向
    ActionFlags As Integer          '动作号; 表示该动作的含义，比如坐下或者走路
    Time As Long                    '时间; 该动作完成一遍所需时间，单位为毫秒
    Number As Long                  '帧数; 该动画有多少帧，决定后面数据的大小
End Type

'自定义结构
Public Type Sequence
    PictureNum As Long              'Adrn中的图片编号
    SequenceNotKnow(1 To 6) As Byte '未知
End Type

'自定义结构
Public Type JzAdrnVer
    ConjectureID1 As Long         '虚拟图片ID1
    AdrnVer As Long               'Adrn文件版本
    Debarkation As Long           '登陆器版本
    Check As Long                 '校验码
    Further_Check1 As Long        '2级校验码
    AdrnMax As Long               'Adrn最大图片编号
    MapMax As Long                '地图最大编号
    SpradrnMax As Long            '动画最大编号
    Jz1(1 To 8) As Byte           '文件标记
    Reserve1(1 To 40) As Byte     '预留字节
    ConjectureID2 As Long         '虚拟图片ID2
    Reserve2(1 To 76) As Byte     '预留字节
End Type

Public Type IndexRepetition
    ID As Long
    P_Type As Long
End Type

Public Type AdrnIndexRepetition
    ID As Long
    P_Type As Long
    Index As Byte
End Type

Public Type AdrnLong
    Repetition As Byte
    Point As Long
End Type

'浏览文件夹
Public Declare Function SHBrowseForFolder Lib "shell32" (lpbi As BrowseInfo) As Long
Public Declare Function SHGetPathFromIDList Lib "shell32" (ByVal pidList As Long, ByVal lpBuffer As String) As Long

'文件查找
Public Declare Function FindFirstFile Lib "kernel32" Alias "FindFirstFileA" (ByVal lpFileName As String, lpFindFileData As WIN32_FIND_DATA) As Long
'Public Declare Function FindNextFile Lib "kernel32" Alias "FindNextFileA" (ByVal hFindFile As Long, lpFindFileData As WIN32_FIND_DATA) As Long
Public Declare Function FindClose Lib "kernel32" (ByVal hFindFile As Long) As Long
Public Declare Function CreateDirectory& Lib "kernel32" Alias "CreateDirectoryA" (ByVal lpNewDirectory As String, lpSecurityAttributes As SECURITY_ATTRIBUTES)
'图片操作
'Public Declare Function SetDIBitsToDevice Lib "gdi32" (ByVal hdc As Long, ByVal x As Long, ByVal y As Long, ByVal dx As Long, ByVal dy As Long, ByVal SrcX As Long, ByVal SrcY As Long, ByVal Scan As Long, ByVal NumScans As Long, bits As Any, BitsInfo As BITMAPINFO, ByVal wUsage As Long) As Long
Public Declare Function StretchDIBits Lib "gdi32" (ByVal hdc As Long, ByVal X As Long, ByVal Y As Long, ByVal dx As Long, ByVal dy As Long, ByVal SrcX As Long, ByVal SrcY As Long, ByVal wSrcWidth As Long, ByVal wSrcHeight As Long, lpBits As Any, lpBitsInfo As BITMAPINFO, ByVal wUsage As Long, ByVal dwRop As Long) As Long
 
Public Declare Function SetBkMode Lib "gdi32" (ByVal hdc As Long, ByVal nBkMode As Long) As Long

'内存操作
Public Declare Sub CopyMemory Lib "kernel32" Alias "RtlMoveMemory" (Destination As Any, Source As Any, ByVal Length As Long)

'系统消息操作
Public Declare Function SendMessage& Lib "user32" Alias "SendMessageA" (ByVal hwnd As Long, ByVal wMsg As Long, ByVal wParam As Long, lParam As Any)
Public Declare Function SendMessageBynum& Lib "user32" Alias "SendMessageA" (ByVal hwnd As Long, ByVal wMsg As Long, ByVal wParam As Long, ByVal lParam As Long)
Public Declare Function SendMessageByString& Lib "user32" Alias "SendMessageA" (ByVal hwnd As Long, ByVal wMsg As Long, ByVal wParam As Long, ByVal lParam As String)


Public Declare Function BmptoReal Lib "unpack.dll" Alias "encoder" (ByVal buf As Long, ByVal width As Long, ByVal height As Long, ByRef datalen As Long, ByVal cmpFlag As Long) As Long

'全局变量
Public AdrnIndex() As adrn, SpradrnIndex() As Spradrn                '图片&动画索引数组
Public MySequence() As Sequence
Public AdrnIndex_Long As Long
Public MapIndex() As Long
Public Map_x As Integer
Public Map_y As Integer
Public MyPals_Num As String                                          '0-15
Public MyBmp_Header As BITMAPFILEHEADER
Public MyBmp_Info As BITMAPINFO
Public BmpData_Byte() As Byte
Public CdPath As String

'搜索文件
Public Function ManhuntFile(FileName As String) As String

Dim MylpFindFileData As WIN32_FIND_DATA
Dim ManhuntH As Long
Dim AdrnPath As String

'开始搜索
ManhuntH = FindFirstFile(FileName, MylpFindFileData)

If ManhuntH = 0 Then
    ManhuntFile = "0"
    Exit Function
ElseIf ManhuntH = INVALID_HANDLE_VALUE Then
    ManhuntFile = "0"
    Exit Function
End If

'获得目标文件路径
ManhuntFile = fDelInvaildChr(MylpFindFileData.cFileName)

'关闭搜索
FindClose ManhuntH
End Function

'文件搜索字符处理
Public Function fDelInvaildChr(str As String) As String

  On Error Resume Next
  For i = Len(str) To 1 Step -1
    If Asc(Mid(str, i, 1)) <> 0 And Asc(Mid(str, i, 1)) <> 32 Then
      fDelInvaildChr = Left(str, i)
      Exit For
    End If
  Next
End Function

'用照参数str2去分割参数str1，并且输出第index个
'举例 strq("123|123|3334","|",3) = "3334"
'index = 0 时输出总共有多少个被分割的字符串
'index = -1 时输出str2在str1出现的所有位置
Function StrQ(ByVal str1 As String, ByVal Str2 As String, ByVal Index As Long) As String
Dim Ms As Long, i As Long, A As Long, B As String
    i = 1
Do
    A = A + 1
    Ms = InStr(i, str1, Str2)
    B = B & Ms & ","
If Index = A Then
If Ms = 0 Then StrQ = Mid(str1, i) Else StrQ = Mid(str1, i, (Ms - i))
Exit Function
End If
    i = Ms + Len(Str2)
Loop Until Ms = 0
If Index = 0 Then StrQ = str(A)
If Index = -1 Then StrQ = Mid(B, 1, Len(B) - 1)
End Function

Sub Adrn_BMP_Decrypt(datalen As Long, FileNum As Long, addr As Long, W As Long, H As Long, Com As Byte)
    Dim RelData_Byte() As Byte
    Dim RelData_Byte_Index As Long, BmpData_Byte_Index As Long
    Dim RealCode As Byte
    Dim Code_Len As Long
    
    Dim Decrypt_Temp_Long As Long
    
    Dim HeaderSize As Long
    Dim ImageSize  As Long
    MyBmp_Info.bmiHeader.biSizeImage = 0
    
    
    ImageSize = W * H
    HeaderSize = Len(MyBmp_Header) + Len(MyBmp_Info)
    
    RelData_Byte_Index = 1
    BmpData_Byte_Index = 1
    
    ReDim RelData_Byte(0 To datalen) As Byte
    ReDim BmpData_Byte(0 To ImageSize) As Byte
    
    Get #FileNum, addr + 1, RelData_Byte
    If (Com And 1) = 1 Then
        RealToBmp RelData_Byte(), BmpData_Byte(), datalen
    Else
        CopyMemory BmpData_Byte(0), RelData_Byte(0), datalen
    End If
    '重组BMP文件结构
    
    MyBmp_Header.bfType(1) = &H42
    MyBmp_Header.bfType(2) = &H4D
    MyBmp_Header.bfSize = HeaderSize + ImageSize
    MyBmp_Header.bfOffBits = HeaderSize
    
    MyBmp_Info.bmiHeader.biClrImportant = &H100
    MyBmp_Info.bmiHeader.biClrUsed = &H100
    MyBmp_Info.bmiHeader.biWidth = W
    MyBmp_Info.bmiHeader.biHeight = H
    MyBmp_Info.bmiHeader.biBitCount = 8
    MyBmp_Info.bmiHeader.biPlanes = 1
    MyBmp_Info.bmiHeader.biCompression = 0
    MyBmp_Info.bmiHeader.biSize = 40
    
    Dim PalsFileName As String
    PalsFileName = "pal\Palet_" & MyPals_Num & ".sap"
    PalsFileName = ManhuntFile(Form1.Text1.Text & "\" & PalsFileName)
    
    If PalsFileName = "0" Then
        MsgBox "找不到指定调色板文件"
        Exit Sub
    End If
    Open Form1.Text1.Text & "\pal\" & PalsFileName For Binary Access Read As #100
    
    Dim palbyte(1 To 708) As Byte
    Dim t, yy As Long
    With MyBmp_Info
        .bmiColors(2).rgbRed = 80
    
        .bmiColors(3).rgbGreen = 80
    
        .bmiColors(4).rgbRed = 80
        .bmiColors(4).rgbGreen = 80
    
        .bmiColors(5).rgbBlue = 80
    
        .bmiColors(6).rgbBlue = 80
        .bmiColors(6).rgbRed = 80
    
        .bmiColors(7).rgbBlue = 80
        .bmiColors(7).rgbGreen = 80
    
        .bmiColors(8).rgbBlue = &HC0
        .bmiColors(8).rgbGreen = &HC0
        .bmiColors(8).rgbRed = &HC0
    
        .bmiColors(9).rgbBlue = &HC0
        .bmiColors(9).rgbRed = &HC0
        .bmiColors(9).rgbGreen = &HDC
    
        .bmiColors(10).rgbBlue = &HF0
        .bmiColors(10).rgbGreen = &HCA
        .bmiColors(10).rgbRed = &HA6
    
        .bmiColors(11).rgbRed = &HDE
    
        .bmiColors(12).rgbGreen = &H5F
        .bmiColors(12).rgbRed = &HFF
    
        .bmiColors(13).rgbBlue = &HA0
        .bmiColors(13).rgbGreen = &HFF
        .bmiColors(13).rgbRed = &HFF
    
        .bmiColors(14).rgbBlue = &HD2
        .bmiColors(14).rgbGreen = &H5F
    
        .bmiColors(15).rgbBlue = &HFF
        .bmiColors(15).rgbGreen = &H50
        .bmiColors(15).rgbRed = &H50
    
        .bmiColors(16).rgbRed = &H28
        .bmiColors(16).rgbGreen = &HE1
        .bmiColors(16).rgbBlue = &H28
        
        .bmiColors(241).rgbBlue = &H96
        .bmiColors(241).rgbGreen = &HC3
        .bmiColors(241).rgbRed = &HF5
        
        .bmiColors(242).rgbBlue = &H5F
        .bmiColors(242).rgbGreen = &HA0
        .bmiColors(242).rgbRed = &H1E
        
        .bmiColors(243).rgbBlue = &H46
        .bmiColors(243).rgbGreen = &H7D
        .bmiColors(243).rgbRed = &HC3
        
        .bmiColors(244).rgbRed = &H1E
        .bmiColors(244).rgbGreen = &H55
        .bmiColors(244).rgbBlue = &H9B
        
        .bmiColors(245).rgbRed = &H37
        .bmiColors(245).rgbGreen = &H41
        .bmiColors(245).rgbBlue = &H46
        
        .bmiColors(246).rgbBlue = &H1E
        .bmiColors(246).rgbGreen = &H23
        .bmiColors(246).rgbRed = &H28
        
        .bmiColors(247).rgbBlue = &HF0
        .bmiColors(247).rgbGreen = &HFB
        .bmiColors(247).rgbRed = &HFF
        
        .bmiColors(248).rgbBlue = &HA5
        .bmiColors(248).rgbGreen = &H6E
        .bmiColors(248).rgbRed = &H3A
        
        .bmiColors(249).rgbRed = &H80
        .bmiColors(249).rgbGreen = &H80
        .bmiColors(249).rgbBlue = &H80
        
        .bmiColors(250).rgbRed = &HFF
        
        .bmiColors(251).rgbGreen = &HFF
        
        .bmiColors(252).rgbRed = &HFF
        .bmiColors(252).rgbGreen = &HFF
        
        .bmiColors(253).rgbBlue = &HFF
        
        .bmiColors(254).rgbBlue = &HFF
        .bmiColors(254).rgbGreen = &H80
        .bmiColors(254).rgbRed = &HFF
        
        .bmiColors(255).rgbBlue = &HFF
        .bmiColors(255).rgbGreen = &HFF
        
        .bmiColors(256).rgbBlue = &HFF
        .bmiColors(256).rgbGreen = &HFF
        .bmiColors(256).rgbRed = &HFF
        
    End With
    Get #100, , palbyte
    For t = 1 To 708 Step 3
        yy = yy + 1
        MyBmp_Info.bmiColors(yy + 16).rgbBlue = palbyte(t)
        MyBmp_Info.bmiColors(yy + 16).rgbGreen = palbyte(t + 1)
        MyBmp_Info.bmiColors(yy + 16).rgbRed = palbyte(t + 2)
    Next
    Close #100
    With MyBmp_Info
        .bmiColors(2).rgbRed = 80
    
        .bmiColors(3).rgbGreen = 80
    
        .bmiColors(4).rgbRed = 80
        .bmiColors(4).rgbGreen = 80
    
        .bmiColors(5).rgbBlue = 80
    
        .bmiColors(6).rgbBlue = 80
        .bmiColors(6).rgbRed = 80
    
        .bmiColors(7).rgbBlue = 80
        .bmiColors(7).rgbGreen = 80
    
        .bmiColors(8).rgbBlue = &HC0
        .bmiColors(8).rgbGreen = &HC0
        .bmiColors(8).rgbRed = &HC0
    
        .bmiColors(9).rgbBlue = &HC0
        .bmiColors(9).rgbRed = &HC0
        .bmiColors(9).rgbGreen = &HDC
    
        .bmiColors(10).rgbBlue = &HF0
        .bmiColors(10).rgbGreen = &HCA
        .bmiColors(10).rgbRed = &HA6
    
        .bmiColors(11).rgbRed = &HDE
    
        .bmiColors(12).rgbGreen = &H5F
        .bmiColors(12).rgbRed = &HFF
    
        .bmiColors(13).rgbBlue = &HA0
        .bmiColors(13).rgbGreen = &HFF
        .bmiColors(13).rgbRed = &HFF
    
        .bmiColors(14).rgbBlue = &HD2
        .bmiColors(14).rgbGreen = &H5F
    
        .bmiColors(15).rgbBlue = &HFF
        .bmiColors(15).rgbGreen = &H50
        .bmiColors(15).rgbRed = &H50
    
        .bmiColors(16).rgbRed = &H28
        .bmiColors(16).rgbGreen = &HE1
        .bmiColors(16).rgbBlue = &H28
        
        .bmiColors(241).rgbBlue = &H96
        .bmiColors(241).rgbGreen = &HC3
        .bmiColors(241).rgbRed = &HF5
        
        .bmiColors(242).rgbBlue = &H5F
        .bmiColors(242).rgbGreen = &HA0
        .bmiColors(242).rgbRed = &H1E
        
        .bmiColors(243).rgbBlue = &H46
        .bmiColors(243).rgbGreen = &H7D
        .bmiColors(243).rgbRed = &HC3
        
        .bmiColors(244).rgbRed = &H1E
        .bmiColors(244).rgbGreen = &H55
        .bmiColors(244).rgbBlue = &H9B
        
        .bmiColors(245).rgbRed = &H37
        .bmiColors(245).rgbGreen = &H41
        .bmiColors(245).rgbBlue = &H46
        
        .bmiColors(246).rgbBlue = &H1E
        .bmiColors(246).rgbGreen = &H23
        .bmiColors(246).rgbRed = &H28
        
        .bmiColors(247).rgbBlue = &HF0
        .bmiColors(247).rgbGreen = &HFB
        .bmiColors(247).rgbRed = &HFF
        
        .bmiColors(248).rgbBlue = &HA5
        .bmiColors(248).rgbGreen = &H6E
        .bmiColors(248).rgbRed = &H3A
        
        .bmiColors(249).rgbRed = &H80
        .bmiColors(249).rgbGreen = &H80
        .bmiColors(249).rgbBlue = &H80
        
        .bmiColors(250).rgbRed = &HFF
        
        .bmiColors(251).rgbGreen = &HFF
        
        .bmiColors(252).rgbRed = &HFF
        .bmiColors(252).rgbGreen = &HFF
        
        .bmiColors(253).rgbBlue = &HFF
        
        .bmiColors(254).rgbBlue = &HFF
        .bmiColors(254).rgbGreen = &H80
        .bmiColors(254).rgbRed = &HFF
        
        .bmiColors(255).rgbBlue = &HFF
        .bmiColors(255).rgbGreen = &HFF
        
        .bmiColors(256).rgbBlue = &HFF
        .bmiColors(256).rgbGreen = &HFF
        .bmiColors(256).rgbRed = &HFF
        
    End With
    MyBmp_Info.bmiHeader.biSizeImage = ImageSize
    Exit Sub
Error:
    MsgBox Err.Description
End Sub

Sub SaveBmpFile(Path As String)
    If CdPath <> "" Then
        Path = CdPath & Path
    Else
        Path = App.Path & "\pic" & Path
    End If

    If MyBmp_Info.bmiHeader.biSizeImage = 0 Then Exit Sub
    Dim MyFs As New FileSystemObject, MyTxtS As TextStream
    Dim buff200() As Byte

    Open Path For Binary Access Write As #200
    Put #200, , MyBmp_Header
    Put #200, , MyBmp_Info
    ReDim buff2000(1 To MyBmp_Info.bmiHeader.biSizeImage) As Byte
    CopyMemory buff2000(1), BmpData_Byte(0), MyBmp_Info.bmiHeader.biSizeImage
    Put #200, , buff2000
    Close #200
End Sub

Sub CdFile(Path As String)
Dim attribut As SECURITY_ATTRIBUTES
attribut.nLength = Len(attribut)
attribut.lpSecurityDescriptor = &O0
attribut.bInheritHandle = False
   Dim counter As Long
   Dim tnew As String
   Dim onew As String
   Dim i As Long
   Dim lnew As Long
   Dim tx As String
   Dim mi As String
   Dim minstra As Long
   Dim A As String
   A = Path
   counter = 0
   If Right(A, 1) <> "\" Then
   A = A & "\"
   End If
   lnew = Len(A)
    For i = 1 To lnew
     tx = Left(A, i)
     If Len(tx) > 3 Then
      mi = Right(tx, 1)
      If mi = "\" Then
      Call CreateDirectory(tx, attribut)
      End If
     End If
    Next i
End Sub

Function RealToBmp(ByRef Target() As Byte, ByRef Source() As Byte, RDDataLen As Long)
    On Error Resume Next
    Dim DecryptFillPieceLen As Long
    
    Dim SourceCurrent As Long
    Dim TargetCurrent As Long
    
    Dim RealToBmp_For1 As Long
    
    Dim TempD As Byte
    
    Do
        TempD = Target(TargetCurrent)
        TargetCurrent = TargetCurrent + 1
        If TargetCurrent > RDDataLen Then
            Exit Do
        End If
        If (TempD And &H80) = 0 Then
            If (TempD And &H10) <> 0 Then
                '7x、5x、3x、1x aa bb
                '填充xaa个bb颜色点
                DecryptFillPieceLen = ((TempD And &HF) * 256) + Target(TargetCurrent)
                TargetCurrent = TargetCurrent + 1
            Else
                '6x、4x、2x、0x aa
                '填充x个aa颜色点
                DecryptFillPieceLen = (TempD And &HF)
            End If
            If DecryptFillPieceLen <= &HFFFFF And DecryptFillPieceLen > 0 Then
            For RealToBmp_For1 = 1 To DecryptFillPieceLen
                Source(SourceCurrent) = Target(TargetCurrent)
                TargetCurrent = TargetCurrent + 1
                SourceCurrent = SourceCurrent + 1
            Next
            End If
        Else
            Dim TempC As Byte
    
            If (TempD And &H40) = 0 Then
                TempC = Target(TargetCurrent)
                TargetCurrent = TargetCurrent + 1
            Else
                TempC = 0
            End If
            
            If (TempD And &H20) <> 0 Then
                DecryptFillPieceLen = ((TempD And &HF) * 256) + Target(TargetCurrent)
                TargetCurrent = TargetCurrent + 1
                DecryptFillPieceLen = (DecryptFillPieceLen * 256) + Target(TargetCurrent)
                TargetCurrent = TargetCurrent + 1
            Else
                If (TempD And &H10) <> 0 Then
                    DecryptFillPieceLen = ((TempD And &HF) * 256) + Target(TargetCurrent)
                    TargetCurrent = TargetCurrent + 1
                Else
                    DecryptFillPieceLen = (TempD And &HF)
                End If
            End If
                
                For RealToBmp_For1 = 0 To DecryptFillPieceLen
                    Source(SourceCurrent + RealToBmp_For1) = TempC
                Next
                SourceCurrent = SourceCurrent + DecryptFillPieceLen
        End If
    Loop
End Function
