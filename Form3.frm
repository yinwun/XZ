VERSION 5.00
Object = "{BDC217C8-ED16-11CD-956C-0000C04E4C0A}#1.1#0"; "TABCTL32.OCX"
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "comdlg32.ocx"
Begin VB.Form Form3 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Form3"
   ClientHeight    =   5250
   ClientLeft      =   45
   ClientTop       =   450
   ClientWidth     =   7365
   LinkTopic       =   "Form3"
   MaxButton       =   0   'False
   ScaleHeight     =   5250
   ScaleWidth      =   7365
   StartUpPosition =   3  '窗口缺省
   Begin VB.TextBox Text31 
      Height          =   270
      Left            =   1320
      TabIndex        =   72
      Top             =   4200
      Width           =   5895
   End
   Begin VB.CommandButton Command15 
      Caption         =   "保存路径"
      Height          =   375
      Left            =   0
      TabIndex        =   71
      Top             =   4200
      Width           =   1215
   End
   Begin VB.TextBox Text30 
      Height          =   270
      Left            =   1320
      TabIndex        =   69
      Top             =   3720
      Width           =   5895
   End
   Begin VB.CommandButton Command2 
      Caption         =   "添加资源"
      Height          =   375
      Left            =   0
      TabIndex        =   28
      Top             =   4680
      Width           =   1215
   End
   Begin MSComDlg.CommonDialog CommonDialog1 
      Left            =   6480
      Top             =   2280
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   393216
   End
   Begin TabDlg.SSTab SSTab1 
      Height          =   3615
      Left            =   120
      TabIndex        =   1
      Top             =   0
      Width           =   6015
      _ExtentX        =   10610
      _ExtentY        =   6376
      _Version        =   393216
      Style           =   1
      Tabs            =   4
      Tab             =   1
      TabsPerRow      =   4
      TabHeight       =   520
      TabCaption(0)   =   "adrn"
      TabPicture(0)   =   "Form3.frx":0000
      Tab(0).ControlEnabled=   0   'False
      Tab(0).Control(0)=   "Command14"
      Tab(0).Control(1)=   "Command4"
      Tab(0).Control(2)=   "Command1"
      Tab(0).Control(3)=   "Text13"
      Tab(0).Control(4)=   "Text12"
      Tab(0).Control(5)=   "Text11"
      Tab(0).Control(6)=   "Text10"
      Tab(0).Control(7)=   "Text9"
      Tab(0).Control(8)=   "Text8"
      Tab(0).Control(9)=   "Text7"
      Tab(0).Control(10)=   "Text6"
      Tab(0).Control(11)=   "Text5"
      Tab(0).Control(12)=   "Text4"
      Tab(0).Control(13)=   "Text3"
      Tab(0).Control(14)=   "Text2"
      Tab(0).Control(15)=   "Text1"
      Tab(0).Control(16)=   "Label26"
      Tab(0).Control(17)=   "Label12"
      Tab(0).Control(18)=   "Label11"
      Tab(0).Control(19)=   "Label10"
      Tab(0).Control(20)=   "Label9"
      Tab(0).Control(21)=   "Label8"
      Tab(0).Control(22)=   "Label7"
      Tab(0).Control(23)=   "Label6"
      Tab(0).Control(24)=   "Label5"
      Tab(0).Control(25)=   "Label4"
      Tab(0).Control(26)=   "Label3"
      Tab(0).Control(27)=   "Label2"
      Tab(0).Control(28)=   "Label1"
      Tab(0).ControlCount=   29
      TabCaption(1)   =   "real"
      TabPicture(1)   =   "Form3.frx":001C
      Tab(1).ControlEnabled=   -1  'True
      Tab(1).Control(0)=   "Label13"
      Tab(1).Control(0).Enabled=   0   'False
      Tab(1).Control(1)=   "Label14"
      Tab(1).Control(1).Enabled=   0   'False
      Tab(1).Control(2)=   "Label15"
      Tab(1).Control(2).Enabled=   0   'False
      Tab(1).Control(3)=   "Label16"
      Tab(1).Control(3).Enabled=   0   'False
      Tab(1).Control(4)=   "Label17"
      Tab(1).Control(4).Enabled=   0   'False
      Tab(1).Control(5)=   "Text14"
      Tab(1).Control(5).Enabled=   0   'False
      Tab(1).Control(6)=   "Text15"
      Tab(1).Control(6).Enabled=   0   'False
      Tab(1).Control(7)=   "Text16"
      Tab(1).Control(7).Enabled=   0   'False
      Tab(1).Control(8)=   "Text17"
      Tab(1).Control(8).Enabled=   0   'False
      Tab(1).Control(9)=   "Text18"
      Tab(1).Control(9).Enabled=   0   'False
      Tab(1).Control(10)=   "Command5"
      Tab(1).Control(10).Enabled=   0   'False
      Tab(1).Control(11)=   "Text19"
      Tab(1).Control(11).Enabled=   0   'False
      Tab(1).Control(12)=   "Command6"
      Tab(1).Control(12).Enabled=   0   'False
      Tab(1).Control(13)=   "Command13"
      Tab(1).Control(13).Enabled=   0   'False
      Tab(1).ControlCount=   14
      TabCaption(2)   =   "spr"
      TabPicture(2)   =   "Form3.frx":0038
      Tab(2).ControlEnabled=   0   'False
      Tab(2).Control(0)=   "Command12"
      Tab(2).Control(1)=   "Command8"
      Tab(2).Control(2)=   "Text25"
      Tab(2).Control(3)=   "Text24"
      Tab(2).Control(4)=   "Command7"
      Tab(2).Control(5)=   "Text23"
      Tab(2).Control(6)=   "Text22"
      Tab(2).Control(7)=   "Text21"
      Tab(2).Control(8)=   "Text20"
      Tab(2).Control(9)=   "Label22"
      Tab(2).Control(10)=   "Label21"
      Tab(2).Control(11)=   "Label20"
      Tab(2).Control(12)=   "Label19"
      Tab(2).Control(13)=   "Label18"
      Tab(2).ControlCount=   14
      TabCaption(3)   =   "spradrn"
      TabPicture(3)   =   "Form3.frx":0054
      Tab(3).ControlEnabled=   0   'False
      Tab(3).Control(0)=   "Command11"
      Tab(3).Control(1)=   "Command10"
      Tab(3).Control(2)=   "Text29"
      Tab(3).Control(3)=   "Command9"
      Tab(3).Control(4)=   "Text28"
      Tab(3).Control(5)=   "Text27"
      Tab(3).Control(6)=   "Text26"
      Tab(3).Control(7)=   "Label25"
      Tab(3).Control(8)=   "Label24"
      Tab(3).Control(9)=   "Label23"
      Tab(3).ControlCount=   10
      Begin VB.CommandButton Command14 
         Caption         =   "读当前数据"
         Height          =   375
         Left            =   -70200
         TabIndex        =   68
         Top             =   3120
         Width           =   1095
      End
      Begin VB.CommandButton Command13 
         Caption         =   "读当前数据"
         Height          =   375
         Left            =   4680
         TabIndex        =   67
         Top             =   2040
         Width           =   1215
      End
      Begin VB.CommandButton Command12 
         Caption         =   "读当前数据"
         Height          =   375
         Left            =   -70200
         TabIndex        =   66
         Top             =   2400
         Width           =   1095
      End
      Begin VB.CommandButton Command11 
         Caption         =   "读当前数据"
         Height          =   375
         Left            =   -70320
         TabIndex        =   65
         Top             =   1680
         Width           =   1215
      End
      Begin VB.CommandButton Command10 
         Caption         =   "添加数据"
         Height          =   375
         Left            =   -70320
         TabIndex        =   64
         Top             =   1200
         Width           =   1215
      End
      Begin VB.TextBox Text29 
         Height          =   270
         Left            =   -74040
         TabIndex        =   63
         Top             =   840
         Width           =   4935
      End
      Begin VB.CommandButton Command9 
         Caption         =   "路径"
         Height          =   255
         Left            =   -74880
         TabIndex        =   62
         Top             =   840
         Width           =   735
      End
      Begin VB.TextBox Text28 
         Height          =   270
         Left            =   -70320
         TabIndex        =   61
         Top             =   480
         Width           =   1215
      End
      Begin VB.TextBox Text27 
         Height          =   270
         Left            =   -72360
         TabIndex        =   59
         Top             =   480
         Width           =   1215
      End
      Begin VB.TextBox Text26 
         Height          =   270
         Left            =   -74280
         TabIndex        =   57
         Top             =   480
         Width           =   1095
      End
      Begin VB.CommandButton Command8 
         Caption         =   "添加数据"
         Height          =   375
         Left            =   -70200
         TabIndex        =   55
         Top             =   1920
         Width           =   1095
      End
      Begin VB.TextBox Text25 
         Height          =   270
         Left            =   -74280
         TabIndex        =   53
         Top             =   1200
         Width           =   5175
      End
      Begin VB.TextBox Text24 
         Height          =   270
         Left            =   -74040
         TabIndex        =   52
         Top             =   1560
         Width           =   4935
      End
      Begin VB.CommandButton Command7 
         Caption         =   "路径"
         Height          =   255
         Left            =   -74880
         TabIndex        =   51
         Top             =   1560
         Width           =   735
      End
      Begin VB.TextBox Text23 
         Height          =   270
         Left            =   -74280
         TabIndex        =   50
         Top             =   840
         Width           =   1215
      End
      Begin VB.TextBox Text22 
         Height          =   270
         Left            =   -70320
         TabIndex        =   48
         Top             =   480
         Width           =   1215
      End
      Begin VB.TextBox Text21 
         Height          =   270
         Left            =   -72240
         TabIndex        =   46
         Top             =   480
         Width           =   1215
      End
      Begin VB.TextBox Text20 
         Height          =   270
         Left            =   -74280
         TabIndex        =   44
         Top             =   480
         Width           =   1215
      End
      Begin VB.CommandButton Command6 
         Caption         =   "添加数据"
         Height          =   375
         Left            =   4680
         TabIndex        =   42
         Top             =   1560
         Width           =   1215
      End
      Begin VB.TextBox Text19 
         Height          =   270
         Left            =   1200
         TabIndex        =   41
         Top             =   1200
         Width           =   4695
      End
      Begin VB.CommandButton Command5 
         Caption         =   "路径"
         Height          =   255
         Left            =   120
         TabIndex        =   40
         Top             =   1200
         Width           =   855
      End
      Begin VB.TextBox Text18 
         Height          =   270
         Left            =   3360
         TabIndex        =   39
         Top             =   840
         Width           =   2535
      End
      Begin VB.TextBox Text17 
         Height          =   270
         Left            =   720
         TabIndex        =   37
         Top             =   840
         Width           =   1215
      End
      Begin VB.TextBox Text16 
         Height          =   270
         Left            =   4680
         TabIndex        =   35
         Top             =   480
         Width           =   1215
      End
      Begin VB.TextBox Text15 
         Height          =   270
         Left            =   2760
         TabIndex        =   33
         Top             =   480
         Width           =   1215
      End
      Begin VB.TextBox Text14 
         Height          =   270
         Left            =   720
         TabIndex        =   31
         Top             =   480
         Width           =   1215
      End
      Begin VB.CommandButton Command4 
         Caption         =   "添加数据"
         Height          =   375
         Left            =   -70200
         TabIndex        =   29
         Top             =   2640
         Width           =   1095
      End
      Begin VB.CommandButton Command1 
         Caption         =   "路径"
         Height          =   255
         Left            =   -74880
         TabIndex        =   27
         Top             =   2280
         Width           =   735
      End
      Begin VB.TextBox Text13 
         Height          =   270
         Left            =   -74040
         TabIndex        =   26
         Top             =   2280
         Width           =   4935
      End
      Begin VB.TextBox Text12 
         Height          =   270
         Left            =   -74040
         TabIndex        =   25
         Top             =   1920
         Width           =   4935
      End
      Begin VB.TextBox Text11 
         Height          =   270
         Left            =   -72000
         TabIndex        =   24
         Top             =   1560
         Width           =   1095
      End
      Begin VB.TextBox Text10 
         Height          =   270
         Left            =   -70200
         TabIndex        =   21
         Top             =   1200
         Width           =   1095
      End
      Begin VB.TextBox Text9 
         Height          =   270
         Left            =   -72000
         TabIndex        =   19
         Top             =   1200
         Width           =   1095
      End
      Begin VB.TextBox Text8 
         Height          =   270
         Left            =   -74040
         TabIndex        =   17
         Top             =   1560
         Width           =   1095
      End
      Begin VB.TextBox Text7 
         Height          =   270
         Left            =   -74040
         TabIndex        =   15
         Top             =   1200
         Width           =   1095
      End
      Begin VB.TextBox Text6 
         Height          =   270
         Left            =   -70200
         TabIndex        =   13
         Top             =   840
         Width           =   1095
      End
      Begin VB.TextBox Text5 
         Height          =   270
         Left            =   -72000
         TabIndex        =   11
         Top             =   840
         Width           =   1095
      End
      Begin VB.TextBox Text4 
         Height          =   270
         Left            =   -74040
         TabIndex        =   9
         Top             =   840
         Width           =   1095
      End
      Begin VB.TextBox Text3 
         Height          =   270
         Left            =   -70200
         TabIndex        =   7
         Top             =   480
         Width           =   1095
      End
      Begin VB.TextBox Text2 
         Height          =   270
         Left            =   -72000
         TabIndex        =   5
         Top             =   480
         Width           =   1095
      End
      Begin VB.TextBox Text1 
         Height          =   270
         Left            =   -74040
         TabIndex        =   3
         Top             =   480
         Width           =   1095
      End
      Begin VB.Label Label26 
         Caption         =   "Label26"
         Height          =   255
         Left            =   -74880
         TabIndex        =   70
         Top             =   3240
         Width           =   855
      End
      Begin VB.Label Label25 
         Caption         =   "动作数"
         Height          =   255
         Left            =   -71040
         TabIndex        =   60
         Top             =   480
         Width           =   615
      End
      Begin VB.Label Label24 
         Caption         =   "Spr地址"
         Height          =   255
         Left            =   -73080
         TabIndex        =   58
         Top             =   480
         Width           =   735
      End
      Begin VB.Label Label23 
         Caption         =   "动画ID"
         Height          =   255
         Left            =   -74880
         TabIndex        =   56
         Top             =   480
         Width           =   735
      End
      Begin VB.Label Label22 
         Caption         =   "帧图片"
         Height          =   255
         Left            =   -74880
         TabIndex        =   54
         Top             =   1200
         Width           =   615
      End
      Begin VB.Label Label21 
         Caption         =   "帧数"
         Height          =   255
         Left            =   -74880
         TabIndex        =   49
         Top             =   840
         Width           =   495
      End
      Begin VB.Label Label20 
         Caption         =   "时间"
         Height          =   255
         Left            =   -70920
         TabIndex        =   47
         Top             =   480
         Width           =   375
      End
      Begin VB.Label Label19 
         Caption         =   "动作号"
         Height          =   255
         Left            =   -72960
         TabIndex        =   45
         Top             =   480
         Width           =   615
      End
      Begin VB.Label Label18 
         Caption         =   "方向"
         Height          =   255
         Left            =   -74880
         TabIndex        =   43
         Top             =   480
         Width           =   495
      End
      Begin VB.Label Label17 
         Caption         =   "块长(16进制)"
         Height          =   255
         Left            =   2040
         TabIndex        =   38
         Top             =   840
         Width           =   1095
      End
      Begin VB.Label Label16 
         Caption         =   "高度"
         Height          =   255
         Left            =   120
         TabIndex        =   36
         Top             =   840
         Width           =   495
      End
      Begin VB.Label Label15 
         Caption         =   "宽度"
         Height          =   255
         Left            =   4080
         TabIndex        =   34
         Top             =   480
         Width           =   615
      End
      Begin VB.Label Label14 
         Caption         =   "未知"
         Height          =   255
         Left            =   2040
         TabIndex        =   32
         Top             =   480
         Width           =   615
      End
      Begin VB.Label Label13 
         Caption         =   "压缩"
         Height          =   255
         Left            =   120
         TabIndex        =   30
         Top             =   480
         Width           =   615
      End
      Begin VB.Label Label12 
         Caption         =   "地图号"
         Height          =   255
         Left            =   -72840
         TabIndex        =   23
         Top             =   1560
         Width           =   615
      End
      Begin VB.Label Label11 
         Caption         =   "未知"
         Height          =   255
         Left            =   -74880
         TabIndex        =   22
         Top             =   1920
         Width           =   615
      End
      Begin VB.Label Label10 
         Caption         =   "障碍"
         Height          =   255
         Left            =   -70800
         TabIndex        =   20
         Top             =   1200
         Width           =   615
      End
      Begin VB.Label Label9 
         Caption         =   "南占"
         Height          =   255
         Left            =   -72840
         TabIndex        =   18
         Top             =   1200
         Width           =   615
      End
      Begin VB.Label Label8 
         Caption         =   "东占"
         Height          =   255
         Left            =   -74880
         TabIndex        =   16
         Top             =   1560
         Width           =   615
      End
      Begin VB.Label Label7 
         Caption         =   "高度"
         Height          =   255
         Left            =   -74880
         TabIndex        =   14
         Top             =   1200
         Width           =   735
      End
      Begin VB.Label Label6 
         Caption         =   "宽度"
         Height          =   255
         Left            =   -70800
         TabIndex        =   12
         Top             =   840
         Width           =   375
      End
      Begin VB.Label Label5 
         Caption         =   "Y"
         Height          =   255
         Left            =   -72840
         TabIndex        =   10
         Top             =   840
         Width           =   135
      End
      Begin VB.Label Label4 
         Caption         =   "X"
         Height          =   255
         Left            =   -74880
         TabIndex        =   8
         Top             =   840
         Width           =   135
      End
      Begin VB.Label Label3 
         Caption         =   "块长度"
         Height          =   255
         Left            =   -70800
         TabIndex        =   6
         Top             =   480
         Width           =   615
      End
      Begin VB.Label Label2 
         Caption         =   "Real地址"
         Height          =   255
         Left            =   -72840
         TabIndex        =   4
         Top             =   480
         Width           =   735
      End
      Begin VB.Label Label1 
         AutoSize        =   -1  'True
         Caption         =   "图片ID"
         Height          =   180
         Left            =   -74880
         TabIndex        =   2
         Top             =   480
         Width           =   540
      End
   End
   Begin VB.CommandButton Command3 
      Caption         =   "图片路径"
      Height          =   375
      Left            =   0
      TabIndex        =   0
      Top             =   3720
      Width           =   1215
   End
   Begin VB.Label Label27 
      Caption         =   "0"
      Height          =   255
      Left            =   5880
      TabIndex        =   73
      Top             =   4800
      Width           =   1335
   End
End
Attribute VB_Name = "Form3"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Private Sub Command1_Click()
CommonDialog1.ShowOpen
Text13.Text = CommonDialog1.FileName
End Sub

Private Sub Command10_Click()
Dim Temp_Spradrn As Spradrn
Temp_Spradrn.ActionNum = Val(Text28.Text)
Temp_Spradrn.addr = Val(Text27.Text)
Temp_Spradrn.AnimationNum = Val(Text26.Text)
Temp_Spradrn.SpradrnNotKnow = 2
Open Text29.Text For Binary Access Write As #100
Put #100, LOF(100) + 1, Temp_Spradrn
Close #100
End Sub

Private Sub Command11_Click()
Text26.Text = Form1.Label9.Caption
Text27.Text = Form1.Label12.Caption
Text28.Text = Form1.Label14.Caption
End Sub

Private Sub Command12_Click()
Dim Temp_String As String
Text20.Text = Form1.Label19.Caption
Text21.Text = Form1.Label20.Caption
Text22.Text = Form1.Label22.Caption
Text23.Text = Form1.Label24.Caption
For A = 0 To Form1.List1.ListCount - 1
    Temp_String = Temp_String & Form1.List1.List(A) & ","
Next
Text25.Text = Temp_String
End Sub

Private Sub Command13_Click()
Text14.Text = Form1.Label50.Caption
Text15.Text = Form1.Label52.Caption
Text16.Text = Form1.Label56.Caption
Text17.Text = Form1.Label57.Caption
End Sub

Private Sub Command14_Click()
Text1.Text = Form1.Label27.Caption
Text2.Text = Form1.Label29.Caption
Text3.Text = 16
Text4.Text = Form1.Label33.Caption
Text5.Text = Form1.Label35.Caption
Text6.Text = Form1.Label37.Caption
Text7.Text = Form1.Label39.Caption
Text8.Text = Form1.Label41.Caption
Text9.Text = Form1.Label43.Caption
Text10.Text = Form1.Label45.Caption
Text11.Text = Form1.Label48.Caption
Text12.Text = Form1.Text5.Text
End Sub

Private Sub Command15_Click()
CommonDialog1.ShowSave
Text31.Text = CommonDialog1.FileName
End Sub

Private Sub Command2_Click()
    Dim Temp_MyBmp_Header As BITMAPFILEHEADER
    Dim Temp_MyBmp_Info As BITMAPINFO
    Dim buff() As Byte
    Dim buff1() As Byte
    Dim datalen As Long
    Open Text30.Text For Binary Access Read As #100
    Get #100, , Temp_MyBmp_Header
    Get #100, , Temp_MyBmp_Info
    ReDim buff(1 To Temp_MyBmp_Info.bmiHeader.biHeight * Temp_MyBmp_Info.bmiHeader.biWidth) As Byte
    Get #100, , buff
    Close #100
    
    Open Text31.Text For Binary Access Write As #101

    Dim addr1 As Long

    addr1 = BmptoReal(VarPtr(buff(1)), Temp_MyBmp_Info.bmiHeader.biWidth, Temp_MyBmp_Info.bmiHeader.biHeight, datalen, 1)
    
    Me.Caption = addr1
    
    ReDim buff1(1 To datalen) As Byte
    
    CopyMemory buff1(1), ByVal addr1, datalen
    
    Put #101, LOF(101) + 1, buff1
    Close #100
    Close #101
    Label27.Caption = Val(Label27.Caption) + 1
End Sub

Private Sub Command3_Click()
CommonDialog1.ShowOpen
Text30.Text = CommonDialog1.FileName
End Sub

Private Sub Command4_Click()
Dim Temp_Adrn As adrn
Temp_Adrn.addr = Val(Text2.Text)
Temp_Adrn.Num = Val(Text1.Text)
Temp_Adrn.datalen = Val(Text3.Text)
Temp_Adrn.X = Val(Text4.Text)
Temp_Adrn.Y = Val(Text5.Text)
Temp_Adrn.width = Val(Text6.Text)
Temp_Adrn.height = Val(Text7.Text)
Temp_Adrn.EastCover = Val(Text8.Text)
Temp_Adrn.SouthCover = Val(Text9.Text)
Temp_Adrn.ObstacleFlags = Val(Text10.Text)
Temp_Adrn.MapNum = Val(Text11.Text)
For A = 1 To 45
    Temp_Adrn.AdrnNotKnow(A) = Val(StrQ(Text12.Text, ",", A))
Next
Open Text13.Text For Binary Access Write As #100
Put #100, LOF(100) + 1, Temp_Adrn
Close #100
MsgBox "完成"
Label26.Caption = Temp_Adrn.Num
End Sub

Private Sub Command5_Click()
CommonDialog1.ShowOpen
Text19.Text = CommonDialog1.FileName
End Sub

Private Sub Command6_Click()
Dim Temp_Real As Real
Temp_Real.Compress = Val(Text14.Text)
Temp_Real.datalen = Val("&H" & Text18.Text)
Temp_Real.height = Val(Text17.Text)
Temp_Real.width = Val(Text16.Text)
Temp_Real.hwnd(1) = &H52
Temp_Real.hwnd(2) = &H44
Temp_Real.RealNotKnow = Val(Text15.Text)
Open Text19.Text For Binary Access Write As #5
Put #5, LOF(5) + 1, Temp_Real
Close #5

End Sub

Private Sub Command7_Click()
CommonDialog1.ShowOpen
Text24.Text = CommonDialog1.FileName
End Sub

Private Sub Command8_Click()
Dim Temp_Spr As Spr
Dim Temp_Sequence As Sequence
Temp_Spr.Number = Val(Text23.Text)
Temp_Spr.Direction = Val(Text20.Text)
Temp_Spr.ActionFlags = Val(Text21.Text)
Temp_Spr.Time = Val(Text22.Text)
Open Text24.Text For Binary Access Write As #100
Put #100, LOF(100) + 1, Temp_Spr

For A = 1 To Temp_Spr.Number
    Temp_Sequence.PictureNum = Val(StrQ(Text25.Text, ",", A))
    Put #100, , Temp_Sequence
Next

Close #100
MsgBox "完成"
End Sub

Private Sub Command9_Click()
CommonDialog1.ShowOpen
Text29.Text = CommonDialog1.FileName
End Sub

