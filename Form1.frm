VERSION 5.00
Object = "{86CF1D34-0C5F-11D2-A9FC-0000F8754DA1}#2.0#0"; "mscomct2.ocx"
Begin VB.Form Form1 
   Appearance      =   0  'Flat
   BorderStyle     =   1  'Fixed Single
   Caption         =   "ʯ��ͼ����ع��� v1.0"
   ClientHeight    =   7680
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   9750
   ForeColor       =   &H00000000&
   MaxButton       =   0   'False
   ScaleHeight     =   512
   ScaleMode       =   3  'Pixel
   ScaleWidth      =   650
   StartUpPosition =   1  'CenterOwner
   Begin VB.Frame Frame1 
      Caption         =   "ͼ����ȡ:"
      Height          =   7455
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   9495
      Begin VB.Frame Frame8 
         Caption         =   "ͼ����ʾ"
         Height          =   3735
         Left            =   4320
         TabIndex        =   45
         Top             =   240
         Width           =   5055
         Begin VB.PictureBox Picture1 
            Appearance      =   0  'Flat
            AutoRedraw      =   -1  'True
            BackColor       =   &H00000000&
            BorderStyle     =   0  'None
            ClipControls    =   0   'False
            ForeColor       =   &H00000000&
            Height          =   3375
            Left            =   120
            ScaleHeight     =   225
            ScaleMode       =   3  'Pixel
            ScaleWidth      =   321
            TabIndex        =   46
            Top             =   240
            Width           =   4815
         End
      End
      Begin VB.Frame Frame2 
         Caption         =   "������Ϣ"
         Height          =   3735
         Left            =   120
         TabIndex        =   19
         Top             =   240
         Width           =   4095
         Begin VB.ListBox List2 
            Height          =   2985
            Left            =   2760
            TabIndex        =   20
            Top             =   480
            Width           =   1215
         End
         Begin VB.CheckBox Check3 
            Caption         =   "����ͼƬ����"
            Height          =   255
            Left            =   1320
            TabIndex        =   47
            Top             =   3000
            Width           =   1455
         End
         Begin VB.CheckBox Check5 
            Caption         =   "�Զ��涯��"
            Height          =   180
            Left            =   120
            TabIndex        =   98
            Top             =   3360
            Width           =   1215
         End
         Begin VB.TextBox Text1 
            Height          =   270
            Left            =   120
            Locked          =   -1  'True
            TabIndex        =   32
            Text            =   "e:\Stoneage80\data"
            Top             =   240
            Width           =   2535
         End
         Begin VB.CommandButton Command1 
            Caption         =   "ʯ��·��"
            Height          =   300
            Left            =   120
            TabIndex        =   31
            Top             =   600
            Width           =   1335
         End
         Begin VB.CommandButton Command2 
            Caption         =   "���ļ�"
            Height          =   300
            Left            =   1440
            TabIndex        =   30
            Top             =   600
            Width           =   1215
         End
         Begin VB.TextBox Text2 
            Height          =   270
            Left            =   1200
            TabIndex        =   29
            Text            =   "420000"
            Top             =   1200
            Width           =   1455
         End
         Begin VB.TextBox Text3 
            Height          =   270
            Left            =   840
            TabIndex        =   28
            Top             =   2280
            Width           =   735
         End
         Begin VB.CommandButton Command6 
            Caption         =   "��ʾ"
            Enabled         =   0   'False
            Height          =   300
            Left            =   120
            TabIndex        =   27
            Top             =   2280
            Width           =   615
         End
         Begin VB.CommandButton Command7 
            Caption         =   "��ʾ"
            Enabled         =   0   'False
            Height          =   300
            Left            =   120
            TabIndex        =   26
            Top             =   2640
            Width           =   615
         End
         Begin VB.TextBox Text6 
            Height          =   270
            Left            =   840
            TabIndex        =   25
            Top             =   2640
            Width           =   735
         End
         Begin VB.CheckBox Check1 
            Caption         =   "�Զ���ͼƬ"
            Height          =   255
            Left            =   120
            TabIndex        =   23
            Top             =   3000
            Width           =   1215
         End
         Begin MSComCtl2.UpDown UpDown3 
            Height          =   255
            Left            =   3720
            TabIndex        =   22
            Top             =   240
            Width           =   255
            _ExtentX        =   450
            _ExtentY        =   450
            _Version        =   393216
            Enabled         =   -1  'True
         End
         Begin MSComCtl2.UpDown UpDown1 
            Height          =   255
            Left            =   1680
            TabIndex        =   24
            Top             =   2280
            Width           =   240
            _ExtentX        =   450
            _ExtentY        =   450
            _Version        =   393216
            Enabled         =   -1  'True
         End
         Begin MSComCtl2.UpDown UpDown2 
            Height          =   255
            Left            =   1680
            TabIndex        =   33
            Top             =   2640
            Width           =   240
            _ExtentX        =   450
            _ExtentY        =   450
            _Version        =   393216
            Enabled         =   -1  'True
         End
         Begin VB.CheckBox Check4 
            Caption         =   "���涯������"
            Height          =   180
            Left            =   1320
            TabIndex        =   97
            Top             =   3360
            Width           =   1455
         End
         Begin VB.Label Label3 
            AutoSize        =   -1  'True
            Caption         =   "�ȴ�!"
            Height          =   180
            Left            =   600
            TabIndex        =   44
            Top             =   960
            Width           =   450
         End
         Begin VB.Label Label5 
            AutoSize        =   -1  'True
            Caption         =   "Adrn   �����:"
            Height          =   180
            Left            =   120
            TabIndex        =   43
            Top             =   1560
            Width           =   1440
         End
         Begin VB.Label Label6 
            AutoSize        =   -1  'True
            Caption         =   "0"
            Height          =   180
            Left            =   1560
            TabIndex        =   42
            Top             =   1560
            Width           =   90
         End
         Begin VB.Label Label61 
            AutoSize        =   -1  'True
            Caption         =   "�ŵ�ͼ"
            Height          =   180
            Left            =   2040
            TabIndex        =   41
            Top             =   2715
            Width           =   540
         End
         Begin VB.Label Label7 
            AutoSize        =   -1  'True
            Caption         =   "Spradrn�����:"
            Height          =   180
            Left            =   120
            TabIndex        =   40
            Top             =   1800
            Width           =   1440
         End
         Begin VB.Label Label62 
            AutoSize        =   -1  'True
            Caption         =   "��ͼ   �����:"
            Height          =   180
            Left            =   120
            TabIndex        =   39
            Top             =   2040
            Width           =   1440
         End
         Begin VB.Label Label63 
            AutoSize        =   -1  'True
            Caption         =   "0"
            Height          =   180
            Left            =   1560
            TabIndex        =   38
            Top             =   2040
            Width           =   90
         End
         Begin VB.Label Label8 
            AutoSize        =   -1  'True
            Caption         =   "0"
            Height          =   180
            Left            =   1560
            TabIndex        =   37
            Top             =   1800
            Width           =   90
         End
         Begin VB.Label Label73 
            AutoSize        =   -1  'True
            Caption         =   "״̬:"
            Height          =   180
            Left            =   120
            TabIndex        =   36
            Top             =   960
            Width           =   450
         End
         Begin VB.Label Label10 
            AutoSize        =   -1  'True
            Caption         =   "��ͼƬ"
            Height          =   180
            Left            =   2040
            TabIndex        =   35
            Top             =   2355
            Width           =   540
         End
         Begin VB.Label Label4 
            AutoSize        =   -1  'True
            Caption         =   "�ڴ�Ԥ����:"
            Height          =   180
            Left            =   120
            TabIndex        =   34
            Top             =   1275
            Width           =   990
         End
         Begin VB.Label Label1 
            AutoSize        =   -1  'True
            Caption         =   "�������:"
            Height          =   180
            Left            =   2880
            TabIndex        =   21
            Top             =   240
            Width           =   810
         End
      End
      Begin VB.Frame Frame4 
         Caption         =   "ͼƬ�ṹ��Ϣ:"
         Height          =   3255
         Left            =   120
         TabIndex        =   2
         Top             =   4080
         Width           =   4575
         Begin VB.Frame Frame10 
            Caption         =   "Adrn�ļ��ṹ��Ϣ"
            Height          =   2895
            Left            =   120
            TabIndex        =   59
            Top             =   240
            Width           =   4335
            Begin VB.Frame Frame6 
               Caption         =   "Real�ļ��ṹ��Ϣ:"
               Height          =   1695
               Left            =   2160
               TabIndex        =   84
               Top             =   240
               Width           =   1935
               Begin VB.Label Label60 
                  AutoSize        =   -1  'True
                  Caption         =   "ħ������:"
                  Height          =   180
                  Left            =   120
                  TabIndex        =   96
                  Top             =   240
                  Width           =   810
               End
               Begin VB.Label Label59 
                  AutoSize        =   -1  'True
                  Caption         =   "RD"
                  Height          =   180
                  Left            =   960
                  TabIndex        =   95
                  Top             =   240
                  Width           =   180
               End
               Begin VB.Label Label49 
                  AutoSize        =   -1  'True
                  Caption         =   "ѹ����־:"
                  Height          =   180
                  Left            =   120
                  TabIndex        =   94
                  Top             =   480
                  Width           =   810
               End
               Begin VB.Label Label50 
                  AutoSize        =   -1  'True
                  Caption         =   "0"
                  Height          =   180
                  Left            =   960
                  TabIndex        =   93
                  Top             =   480
                  Width           =   90
               End
               Begin VB.Label Label51 
                  AutoSize        =   -1  'True
                  Caption         =   "δ����֪:"
                  Height          =   180
                  Left            =   120
                  TabIndex        =   92
                  Top             =   720
                  Width           =   810
               End
               Begin VB.Label Label52 
                  AutoSize        =   -1  'True
                  Caption         =   "0"
                  Height          =   180
                  Left            =   960
                  TabIndex        =   91
                  Top             =   720
                  Width           =   90
               End
               Begin VB.Label Label53 
                  AutoSize        =   -1  'True
                  Caption         =   "������:"
                  Height          =   180
                  Left            =   120
                  TabIndex        =   90
                  Top             =   960
                  Width           =   810
               End
               Begin VB.Label Label54 
                  AutoSize        =   -1  'True
                  Caption         =   "�ߡ�����:"
                  Height          =   180
                  Left            =   120
                  TabIndex        =   89
                  Top             =   1200
                  Width           =   810
               End
               Begin VB.Label Label55 
                  AutoSize        =   -1  'True
                  Caption         =   "�� �� ��:"
                  Height          =   180
                  Left            =   120
                  TabIndex        =   88
                  Top             =   1440
                  Width           =   810
               End
               Begin VB.Label Label56 
                  AutoSize        =   -1  'True
                  Caption         =   "0"
                  Height          =   180
                  Left            =   960
                  TabIndex        =   87
                  Top             =   960
                  Width           =   90
               End
               Begin VB.Label Label57 
                  AutoSize        =   -1  'True
                  Caption         =   "0"
                  Height          =   180
                  Left            =   960
                  TabIndex        =   86
                  Top             =   1200
                  Width           =   90
               End
               Begin VB.Label Label58 
                  AutoSize        =   -1  'True
                  Caption         =   "0"
                  Height          =   180
                  Left            =   960
                  TabIndex        =   85
                  Top             =   1440
                  Width           =   90
               End
            End
            Begin VB.TextBox Text5 
               Height          =   270
               Left            =   960
               Locked          =   -1  'True
               TabIndex        =   60
               Top             =   2520
               Width           =   3255
            End
            Begin VB.Label Label48 
               AutoSize        =   -1  'True
               Caption         =   "0"
               Height          =   180
               Left            =   3120
               TabIndex        =   83
               Top             =   2040
               Width           =   90
            End
            Begin VB.Label Label47 
               AutoSize        =   -1  'True
               Caption         =   "��ͼ���:"
               Height          =   180
               Left            =   2280
               TabIndex        =   82
               Top             =   2040
               Width           =   810
            End
            Begin VB.Label Label46 
               AutoSize        =   -1  'True
               Caption         =   "δ֪����:"
               Height          =   180
               Left            =   120
               TabIndex        =   81
               Top             =   2565
               Width           =   810
            End
            Begin VB.Label Label45 
               AutoSize        =   -1  'True
               Caption         =   "0"
               Height          =   180
               Left            =   3120
               TabIndex        =   80
               Top             =   2280
               Width           =   90
            End
            Begin VB.Label Label44 
               AutoSize        =   -1  'True
               Caption         =   "�ϰ���־:"
               Height          =   180
               Left            =   2280
               TabIndex        =   79
               Top             =   2280
               Width           =   810
            End
            Begin VB.Label Label43 
               AutoSize        =   -1  'True
               Caption         =   "0"
               Height          =   180
               Left            =   960
               TabIndex        =   78
               Top             =   2280
               Width           =   90
            End
            Begin VB.Label Label42 
               AutoSize        =   -1  'True
               Caption         =   "ռ��(��):"
               Height          =   180
               Left            =   120
               TabIndex        =   77
               Top             =   2280
               Width           =   810
            End
            Begin VB.Label Label41 
               AutoSize        =   -1  'True
               Caption         =   "0"
               Height          =   180
               Left            =   960
               TabIndex        =   76
               Top             =   2040
               Width           =   90
            End
            Begin VB.Label Label40 
               AutoSize        =   -1  'True
               Caption         =   "ռ��(��):"
               Height          =   180
               Left            =   120
               TabIndex        =   75
               Top             =   2040
               Width           =   810
            End
            Begin VB.Label Label39 
               AutoSize        =   -1  'True
               Caption         =   "0"
               Height          =   180
               Left            =   960
               TabIndex        =   74
               Top             =   1800
               Width           =   90
            End
            Begin VB.Label Label38 
               AutoSize        =   -1  'True
               Caption         =   "ͼƬ�߶�:"
               Height          =   180
               Left            =   120
               TabIndex        =   73
               Top             =   1800
               Width           =   810
            End
            Begin VB.Label Label37 
               AutoSize        =   -1  'True
               Caption         =   "0"
               Height          =   180
               Left            =   960
               TabIndex        =   72
               Top             =   1560
               Width           =   90
            End
            Begin VB.Label Label36 
               AutoSize        =   -1  'True
               Caption         =   "ͼƬ���:"
               Height          =   180
               Left            =   120
               TabIndex        =   71
               Top             =   1560
               Width           =   810
            End
            Begin VB.Label Label35 
               AutoSize        =   -1  'True
               Caption         =   "0"
               Height          =   180
               Left            =   960
               TabIndex        =   70
               Top             =   1320
               Width           =   90
            End
            Begin VB.Label Label34 
               AutoSize        =   -1  'True
               Caption         =   "ƫ����-Y:"
               Height          =   180
               Left            =   120
               TabIndex        =   69
               Top             =   1320
               Width           =   810
            End
            Begin VB.Label Label33 
               AutoSize        =   -1  'True
               Caption         =   "0"
               Height          =   180
               Left            =   960
               TabIndex        =   68
               Top             =   1080
               Width           =   90
            End
            Begin VB.Label Label32 
               AutoSize        =   -1  'True
               Caption         =   "ƫ����-X:"
               Height          =   180
               Left            =   120
               TabIndex        =   67
               Top             =   1080
               Width           =   810
            End
            Begin VB.Label Label31 
               AutoSize        =   -1  'True
               Caption         =   "0"
               Height          =   180
               Left            =   960
               TabIndex        =   66
               Top             =   840
               Width           =   90
            End
            Begin VB.Label Label30 
               AutoSize        =   -1  'True
               Caption         =   "�� �� ��:"
               Height          =   180
               Left            =   120
               TabIndex        =   65
               Top             =   840
               Width           =   810
            End
            Begin VB.Label Label29 
               AutoSize        =   -1  'True
               Caption         =   "0"
               Height          =   180
               Left            =   960
               TabIndex        =   64
               Top             =   600
               Width           =   90
            End
            Begin VB.Label Label28 
               AutoSize        =   -1  'True
               Caption         =   "Real��ַ:"
               Height          =   180
               Left            =   120
               TabIndex        =   63
               Top             =   600
               Width           =   810
            End
            Begin VB.Label Label27 
               AutoSize        =   -1  'True
               Caption         =   "0"
               Height          =   180
               Left            =   960
               TabIndex        =   62
               Top             =   360
               Width           =   90
            End
            Begin VB.Label Label26 
               AutoSize        =   -1  'True
               Caption         =   "ͼƬ���:"
               Height          =   180
               Left            =   120
               TabIndex        =   61
               Top             =   360
               Width           =   810
            End
         End
      End
      Begin VB.Frame Frame3 
         Caption         =   "�����ṹ��Ϣ:"
         Height          =   3255
         Left            =   4800
         TabIndex        =   1
         Top             =   4080
         Width           =   4575
         Begin VB.Frame Frame9 
            Caption         =   "SprAdrn�ļ��ṹ��Ϣ"
            Height          =   1215
            Left            =   120
            TabIndex        =   48
            Top             =   240
            Width           =   2415
            Begin VB.TextBox Text23 
               Height          =   270
               Left            =   1560
               TabIndex        =   50
               Text            =   "0"
               Top             =   680
               Width           =   375
            End
            Begin MSComCtl2.UpDown UpDown5 
               Height          =   255
               Left            =   2040
               TabIndex        =   49
               Top             =   675
               Width           =   240
               _ExtentX        =   450
               _ExtentY        =   450
               _Version        =   393216
               Enabled         =   -1  'True
            End
            Begin VB.Label Label2 
               AutoSize        =   -1  'True
               Caption         =   "�������:"
               Height          =   180
               Left            =   120
               TabIndex        =   58
               Top             =   240
               Width           =   810
            End
            Begin VB.Label Label9 
               AutoSize        =   -1  'True
               Caption         =   "0"
               Height          =   180
               Left            =   960
               TabIndex        =   57
               Top             =   240
               Width           =   90
            End
            Begin VB.Label Label11 
               AutoSize        =   -1  'True
               Caption         =   "Spr ��ַ:"
               Height          =   180
               Left            =   120
               TabIndex        =   56
               Top             =   480
               Width           =   810
            End
            Begin VB.Label Label12 
               AutoSize        =   -1  'True
               Caption         =   "0"
               Height          =   180
               Left            =   960
               TabIndex        =   55
               Top             =   480
               Width           =   90
            End
            Begin VB.Label Label13 
               AutoSize        =   -1  'True
               Caption         =   "������Ŀ:"
               Height          =   180
               Left            =   120
               TabIndex        =   54
               Top             =   720
               Width           =   810
            End
            Begin VB.Label Label14 
               AutoSize        =   -1  'True
               Caption         =   "0"
               Height          =   180
               Left            =   960
               TabIndex        =   53
               Top             =   720
               Width           =   90
            End
            Begin VB.Label Label15 
               AutoSize        =   -1  'True
               Caption         =   "δ����֪:"
               Height          =   180
               Left            =   120
               TabIndex        =   52
               Top             =   960
               Width           =   810
            End
            Begin VB.Label Label16 
               AutoSize        =   -1  'True
               Caption         =   "0"
               Height          =   180
               Left            =   960
               TabIndex        =   51
               Top             =   960
               Width           =   90
            End
         End
         Begin VB.CommandButton Command27 
            Caption         =   "�������ͼ��"
            Height          =   300
            Left            =   3000
            TabIndex        =   18
            Top             =   2880
            Width           =   1455
         End
         Begin VB.CommandButton Command4 
            Caption         =   "ֹͣ"
            Height          =   300
            Left            =   2400
            TabIndex        =   15
            Top             =   2880
            Width           =   615
         End
         Begin VB.CommandButton Command5 
            Caption         =   "��ͣ"
            Height          =   300
            Left            =   1800
            TabIndex        =   16
            Top             =   2880
            Width           =   615
         End
         Begin VB.Timer Timer1 
            Enabled         =   0   'False
            Interval        =   100
            Left            =   2400
            Top             =   2400
         End
         Begin VB.CommandButton Command3 
            Caption         =   "����"
            Height          =   300
            Left            =   1200
            TabIndex        =   14
            Top             =   2880
            Width           =   615
         End
         Begin VB.CheckBox Check2 
            Caption         =   "ѭ������"
            Height          =   255
            Left            =   120
            TabIndex        =   17
            Top             =   2880
            Width           =   1095
         End
         Begin VB.Frame Frame7 
            Caption         =   "֡����"
            Height          =   2535
            Left            =   2640
            TabIndex        =   12
            Top             =   240
            Width           =   1815
            Begin VB.TextBox Text4 
               Height          =   270
               Left            =   120
               TabIndex        =   100
               Top             =   2160
               Width           =   1575
            End
            Begin VB.ListBox List1 
               Height          =   1230
               Left            =   120
               TabIndex        =   13
               Top             =   480
               Width           =   1575
            End
            Begin VB.Label Label64 
               AutoSize        =   -1  'True
               Caption         =   "δ֪����:"
               Height          =   180
               Left            =   120
               TabIndex        =   101
               Top             =   1920
               Width           =   810
            End
            Begin VB.Label Label25 
               AutoSize        =   -1  'True
               Caption         =   "ͼƬID:"
               Height          =   180
               Left            =   120
               TabIndex        =   99
               Top             =   240
               Width           =   630
            End
         End
         Begin VB.Frame Frame5 
            Caption         =   "Spr�ļ��ṹ��Ϣ:"
            Height          =   1215
            Left            =   120
            TabIndex        =   3
            Top             =   1560
            Width           =   2415
            Begin VB.Label Label24 
               AutoSize        =   -1  'True
               Caption         =   "0"
               Height          =   180
               Left            =   960
               TabIndex        =   11
               Top             =   960
               Width           =   90
            End
            Begin VB.Label Label23 
               AutoSize        =   -1  'True
               Caption         =   "֡������:"
               Height          =   180
               Left            =   120
               TabIndex        =   10
               Top             =   960
               Width           =   810
            End
            Begin VB.Label Label22 
               AutoSize        =   -1  'True
               Caption         =   "0"
               Height          =   180
               Left            =   960
               TabIndex        =   9
               Top             =   720
               Width           =   90
            End
            Begin VB.Label Label21 
               AutoSize        =   -1  'True
               Caption         =   "���ʱ��:"
               Height          =   180
               Left            =   120
               TabIndex        =   8
               Top             =   720
               Width           =   810
            End
            Begin VB.Label Label20 
               AutoSize        =   -1  'True
               Caption         =   "0"
               Height          =   180
               Left            =   960
               TabIndex        =   7
               Top             =   480
               Width           =   90
            End
            Begin VB.Label Label19 
               AutoSize        =   -1  'True
               Caption         =   "0"
               Height          =   180
               Left            =   960
               TabIndex        =   6
               Top             =   240
               Width           =   90
            End
            Begin VB.Label Label18 
               AutoSize        =   -1  'True
               Caption         =   "�� �� ��:"
               Height          =   180
               Left            =   120
               TabIndex        =   5
               Top             =   480
               Width           =   810
            End
            Begin VB.Label Label17 
               AutoSize        =   -1  'True
               Caption         =   "�� �� ��:"
               Height          =   180
               Left            =   120
               TabIndex        =   4
               Top             =   240
               Width           =   810
            End
         End
      End
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False

Private Sub Command1_Click()
    Dim lpIDList     As Long
    Dim sBuffer     As String
    Dim tBrowseInfo     As BrowseInfo
    lpIDList = SHBrowseForFolder(tBrowseInfo)
    If (lpIDList) Then
        sBuffer = Space(MAX_PATH)
        SHGetPathFromIDList lpIDList, sBuffer
        sBuffer = Left(sBuffer, InStr(sBuffer, vbNullChar) - 1)
        Text1.Text = sBuffer
    End If
End Sub

Private Sub Command10_Click()

End Sub

Private Sub Command12_Click()

End Sub

Private Sub Command2_Click()
    Command2.Enabled = False
    Dim FileName As String
    Dim TempStr As String
    
    Dim Buffer() As Byte
    Dim NewAddr As Long
    
    Dim TempAdrn As Long
    
    Dim AdrnIndexNum As Long
    Dim SpradrnIndexNum As Long
    
    Dim AdrnNumMax As Long
    Dim SpradrnNumMax As Long
    Dim MapIndexMax As Long
    
    ReDim AdrnIndex(0) As adrn
    ReDim AdrnIndex(Val(Text2.Text)) As adrn
    ReDim SpradrnIndex(0) As Spradrn
    ReDim MapIndex(0) As Long
    '=======================��Adrn����ͷ�ļ�=========================================
    FileName = Text1.Text & "\adrn_*.bin"
    TempStr = ManhuntFile(FileName)
    
    If TempStr = "0" Then
        TempStr = "adrn.bin"
    End If

    '���ļ�
    Open Text1.Text & "\" & TempStr For Binary Access Read As #1

    AdrnIndexNum = 0
'    List3.Clear
    ReDim Buffer(1 To 80) As Byte
    '���ļ�
    Label3.Caption = "��adrn"
    DoEvents
    Do While Not EOF(1)
        Get #1, , Buffer
        If Not EOF(1) Then
            'ȡ��ͼƬ���
            CopyMemory AdrnIndexNum, Buffer(1), 4
            '����ռ�
            If AdrnIndexNum > UBound(AdrnIndex) Then
                ReDim Preserve AdrnIndex(AdrnIndexNum) As adrn
            End If
            '����Adrn���ݽṹ
            CopyMemory AdrnIndex(AdrnIndexNum), Buffer(1), 80

'            If AdrnIndex(AdrnIndexNum).height = 480 And AdrnIndex(AdrnIndexNum).width = 640 Then
'                List3.AddItem AdrnIndex(AdrnIndexNum).Num
'            End If
            
            If MapIndexMax < AdrnIndex(AdrnIndexNum).MapNum Then MapIndexMax = AdrnIndex(AdrnIndexNum).MapNum
        
            If AdrnIndex(AdrnIndexNum).MapNum > UBound(MapIndex) Then ReDim Preserve MapIndex(AdrnIndex(AdrnIndexNum).MapNum) As Long
            MapIndex(AdrnIndex(AdrnIndexNum).MapNum) = AdrnIndexNum
        
            If AdrnNumMax < AdrnIndexNum Then AdrnNumMax = AdrnIndexNum
            Label3.Caption = "��adrn"
        End If
    Loop
    Close #1
    DoEvents
    Label6.Caption = str(AdrnNumMax)

    '===============================================================================
    
    '��Spradrn����ͷ�ļ�
    FileName = Text1.Text & "\spradrn_*.bin"
    TempStr = ManhuntFile(FileName)
    
    If TempStr = "0" Then
        TempStr = "spradrn.bin"
    End If
    
    Open Text1.Text & "\" & TempStr For Binary Access Read As #2
    SpradrnIndexNum = 0
    ReDim Buffer(1 To 12) As Byte
    '���ļ�
    Label3.Caption = "��Spradrn"
    DoEvents
    Do While Not EOF(2)
        Get #2, , Buffer
        If Not EOF(2) Then
            '�����ڴ�
            ReDim Preserve SpradrnIndex(SpradrnIndexNum) As Spradrn
        
            CopyMemory SpradrnIndex(SpradrnIndexNum), Buffer(1), 12
        
            If SpradrnNumMax < SpradrnIndex(SpradrnIndexNum).AnimationNum Then SpradrnNumMax = SpradrnIndex(SpradrnIndexNum).AnimationNum
            Label3.Caption = "��Spradrn"
            SpradrnIndexNum = SpradrnIndexNum + 1
            DoEvents
        End If
    Loop
    Close #2
    Label8.Caption = str(SpradrnNumMax)
    Label63.Caption = str(MapIndexMax)
    DoEvents
    List2.Clear
    
    For A = 0 To UBound(SpradrnIndex)
        List2.AddItem SpradrnIndex(A).AnimationNum
        DoEvents
    Next
    
    Label3.Caption = "�������"
    Command2.Enabled = True
    Command6.Enabled = True
    Command7.Enabled = True
End Sub


Private Sub Command24_Click()

End Sub

Private Sub Command27_Click()
    Text23.Text = 0
    Dim CheckVv As Long
    Dim A As Long
    CheckVv = Check1.Value
    Check1.Value = 1
    Dim FilePath As String
    FilePath = App.Path & "\pic\" & Trim(Label9.Caption) & "\"
    CdFile FilePath

    DoEvents
    For m = 0 To Val(Label14.Caption) - 1
        CdPath = FilePath & "����" & Trim(Label19.Caption)
        CdFile CdPath
        For n = 0 To List1.ListCount - 1
            CdPath = FilePath & "����" & Trim(Label19.Caption) & "\����" & Trim(Label20.Caption)
            CdFile CdPath
            List1.ListIndex = n
            DoEvents
            List1_DblClick
        Next
        Text23.Text = Trim(str(Val(Text23.Text) + 1))
        For A = 0 To List2.ListCount
            If List2.List(A) = Trim(Label9.Caption) Then
                ShowSpr A
                Exit For
            End If
        Next
        DoEvents
    Next
    Check1.Value = CheckVv

End Sub

Private Sub Command3_Click()
    If List1.ListIndex = List1.ListCount - 1 Then List1.ListIndex = 0
    Timer1.Enabled = True
End Sub

Private Sub Command4_Click()
    Timer1.Enabled = False
    List1.ListIndex = 0
    ShowAdrn Val(List1.List(List1.ListIndex))
End Sub

Private Sub Command5_Click()
    Timer1.Enabled = False
End Sub

Private Sub Command6_Click()
    ShowAdrn Val(Text3.Text)
    If Check3.Value <> 0 Then
        SaveData Val(Text3.Text)
    End If
End Sub

Private Sub Command7_Click()
    If Val(Text6.Text) > UBound(MapIndex) Then Exit Sub
    ShowAdrn MapIndex(Val(Text6.Text))
    If Check3.Value <> 0 Then
        SaveData MapIndex(Val(Text6.Text))
    End If
End Sub



Private Sub Form_Load()
    SetBkMode Picture1.hdc, TRANSPARENT
    
    'Ĭ�ϵ�ɫ��
    MyPals_Num = "1"
End Sub

Private Sub Label76_Click()
End Sub

Private Sub List1_DblClick()
    ShowAdrn Val(List1.List(List1.ListIndex))
    If Check3.Value <> 0 Then
        SaveData Val(List1.List(List1.ListIndex))
    End If
End Sub

Private Sub List2_DblClick()
    ShowSpr List2.ListIndex
    List1.ListIndex = 0
    List1_DblClick
    If Check4.Value <> 0 Then
        SaveSprData List2.ListIndex
    End If
End Sub

Private Sub ShowAdrn(Index As Long)
    MyBmp_Info.bmiHeader.biSizeImage = 0
    Picture1.Cls

    If UBound(AdrnIndex) < Index Then Exit Sub
    'ͼƬ���
    Label27.Caption = str(AdrnIndex(Index).Num)
    'Real�ļ��еĵ�ַ
    Label29.Caption = str(AdrnIndex(Index).addr)
    '�鳤��
    Label31.Caption = str(AdrnIndex(Index).datalen)
    'ƫ����X
    Label33.Caption = str(AdrnIndex(Index).X)
    'ƫ����Y
    Label35.Caption = str(AdrnIndex(Index).Y)
    'ͼƬ���
    Label37.Caption = str(AdrnIndex(Index).width)
    'ͼƬ�߶�
    Label39.Caption = str(AdrnIndex(Index).height)
    'ռ�����-��
    Label41.Caption = str(AdrnIndex(Index).EastCover)
    'ռ�����-��
    Label43.Caption = str(AdrnIndex(Index).SouthCover)
    '�ϰ���־
    Label45.Caption = str(AdrnIndex(Index).ObstacleFlags)
    '��ͼ���

    Label48.Caption = str(AdrnIndex(Index).MapNum)


    Text5.Text = ""
    For A = 1 To 45
    Text5.Text = Text5.Text & str(AdrnIndex(Index).AdrnNotKnow(A)) & ","
    Next
    ShowReal AdrnIndex(Index).addr, AdrnIndex(Index).datalen
    If MyBmp_Info.bmiHeader.biSizeImage <> 0 Then
        Dim ErrLong As Long
        ErrLong = StretchDIBits(Picture1.hdc, ((Picture1.width / 15) - MyBmp_Info.bmiHeader.biWidth) / 2 _
        , ((Picture1.height / 15) - MyBmp_Info.bmiHeader.biHeight) / 2, MyBmp_Info.bmiHeader.biWidth, _
        MyBmp_Info.bmiHeader.biHeight, 0, 0, MyBmp_Info.bmiHeader.biWidth, MyBmp_Info.bmiHeader.biHeight, _
        BmpData_Byte(0), MyBmp_Info, DIB_RGB_COLORSn, SRCCOPY)
        SendMessageBynum& Picture1.hwnd, WM_PAINT, 0, 0
    End If
    If Check1.Value = 1 Then SaveBmpFile "\" & AdrnIndex(Index).Num & ".bmp"
End Sub

Private Sub ShowReal(addr As Long, datalen As Long)
    MyBmp_Info.bmiHeader.biSizeImage = 0
    Dim FileName As String
    Dim TempStr As String
    Dim MyReal As Real
    Dim buff() As Byte
    FileName = Text1.Text & "\real_*.bin"
    TempStr = ManhuntFile(FileName)
    
    If TempStr = "0" Then
        TempStr = "real.bin"
    End If
    Close #3
    Open Text1.Text & "\" & TempStr For Binary Access Read As #3
    If datalen = 0 Then
        Close #3
        Exit Sub
    End If
    'If DataLen < 17 Then
    '    Close #3
    '    Exit Sub
    'End If
    ReDim buff(1 To datalen) As Byte
    Get #3, addr + 1, MyReal
    'Get #3, , Buff
    '�������ͷ
    If MyReal.hwnd(1) = 82 And MyReal.hwnd(2) = 68 Then
        '������ȷ
        
        Label59.Caption = Chr(MyReal.hwnd(1)) + Chr(MyReal.hwnd(2))
        Label50.Caption = MyReal.Compress
        Label52.Caption = MyReal.RealNotKnow
        Label56.Caption = MyReal.width
        Label57.Caption = MyReal.height
        Adrn_BMP_Decrypt MyReal.datalen - 16, 3, addr + 16, MyReal.width, MyReal.height, MyReal.Compress
        
        Label58.Caption = MyReal.datalen
    Else
        MsgBox "���ݴ���"
    End If
    Close #3
End Sub

Sub ShowSpr(Index As Long)
    Dim FileName As String
    Dim TempStr As String
    Dim Current_ActionNum As Long
    Current_ActionNum = Val(Text23.Text)
    
    Label9.Caption = str(SpradrnIndex(Index).AnimationNum)
    Label12.Caption = str(SpradrnIndex(Index).addr)
    Label14.Caption = str(SpradrnIndex(Index).ActionNum)
    Label16.Caption = str(SpradrnIndex(Index).SpradrnNotKnow)
    
    FileName = Text1.Text & "\spr_*.bin"
    TempStr = ManhuntFile(FileName)
    
    If TempStr = "0" Then
        TempStr = "spr.bin"
    End If
    
    Dim MySpr As Spr
    Dim addr As Long
    addr = SpradrnIndex(Index).addr + 1
    '���ļ�
    Open Text1.Text & "\" & TempStr For Binary Access Read As #4
    '��ָ������
    For A = 0 To SpradrnIndex(Index).ActionNum - 1
        Get #4, addr, MySpr
        addr = addr + Len(MySpr) + (MySpr.Number * 10)
        If Current_ActionNum = A Then Exit For
    Next
    
    Label19.Caption = str(MySpr.Direction)
    Label20.Caption = str(MySpr.ActionFlags)
    Label22.Caption = str(MySpr.Time)
    Label24.Caption = str(MySpr.Number)
    List1.Clear
    If MySpr.Number > 0 Then
        ReDim buff(1 To 10) As Byte
        ReDim MySequence(1 To MySpr.Number) As Sequence
    
        For A = 1 To MySpr.Number
            Get #4, , buff
            CopyMemory MySequence(A), buff(1), 10
            List1.AddItem MySequence(A).PictureNum
            Text4.Text = MySequence(A).SequenceNotKnow(1) & MySequence(A).SequenceNotKnow(2) & _
            MySequence(A).SequenceNotKnow(3) & MySequence(A).SequenceNotKnow(4) _
            & MySequence(A).SequenceNotKnow(5) & MySequence(A).SequenceNotKnow(6)
        Next
    End If
    Close #4
End Sub

Private Sub List3_Click()

End Sub

Private Sub Timer1_Timer()
    If List1.ListIndex + 1 = List1.ListCount Then
        If Check2.Value = 0 Then
            Timer1.Enabled = False
            Exit Sub
        Else
            List1.ListIndex = 0
        End If
    Else
        List1.ListIndex = List1.ListIndex + 1
    End If
    ShowAdrn Val(List1.List(List1.ListIndex))
End Sub

Private Sub UpDown1_DownClick()
    If Command6.Enabled = False Then Exit Sub
    If Val(Text3.Text) - 1 < 0 Then Exit Sub
    Text3.Text = Trim(str(Val(Text3.Text) - 1))
    Command6_Click
End Sub

Private Sub UpDown1_UpClick()
    If Command6.Enabled = False Then Exit Sub
    If Val(Text3.Text) + 1 > Val(Label6.Caption) Then Exit Sub
    Text3.Text = Trim(str(Val(Text3.Text) + 1))
    Command6_Click
End Sub

Private Sub UpDown2_DownClick()
    If Command7.Enabled = False Then Exit Sub
    If Val(Text6.Text) - 1 < 0 Then Exit Sub
    Dim Temp1 As Long
    Temp1 = Val(Text6.Text) - 1
    For A = Temp1 To 0 Step -1
        If MapIndex(A) > 0 Then
            Text6.Text = Trim(str(A))
            Exit For
        End If
    Next
    Command7_Click
End Sub

Private Sub UpDown2_UpClick()
    If Command7.Enabled = False Then Exit Sub
    If Val(Text6.Text) + 1 > Val(Label63.Caption) Then Exit Sub
    Dim Temp1 As Long
    Temp1 = Val(Text6.Text) + 1
    For A = Temp1 To UBound(MapIndex) Step 1
        If MapIndex(A) > 0 Then
            Text6.Text = Trim(str(A))
            Exit For
        End If
    Next
    Text6.Text = Trim(str(A))
    Command7_Click
End Sub


Private Sub UpDown3_DownClick()
    On Error GoTo Error
    List2.ListIndex = List2.ListIndex + 1
    List2_DblClick
Error:
End Sub

Private Sub UpDown3_UpClick()
    On Error GoTo Error
    List2.ListIndex = List2.ListIndex - 1
    List2_DblClick
Error:
End Sub

Private Sub UpDown5_DownClick()
    Dim A As Long
    If Command2.Enabled = False Then Exit Sub
    If Val(Text23.Text) - 1 < 0 Then Exit Sub
    Text23.Text = Trim(str(Val(Text23.Text) - 1))
    For A = 0 To List2.ListCount
        If List2.List(A) = Trim(Label9.Caption) Then
            ShowSpr A
            Exit For
        End If
    Next
    List1.ListIndex = 0
    ShowAdrn Val(List1.List(List1.ListIndex))
End Sub

Private Sub UpDown5_UpClick()
    Dim A As Long
    If Command2.Enabled = False Then Exit Sub
    If Val(Text23.Text) + 1 > Val(Label14.Caption) - 1 Then Exit Sub
    Text23.Text = Trim(str(Val(Text23.Text) + 1))
    For A = 0 To List2.ListCount
        If List2.List(A) = Trim(Label9.Caption) Then
            ShowSpr A
            Exit For
        End If
    Next
    List1.ListIndex = 0
    ShowAdrn Val(List1.List(List1.ListIndex))
End Sub

Function Perfor_DiagramShow_Function1(A As Long) As Long
Text3.Text = str(A)
Command6_Click
Perfor_DiagramShow_Function1 = MsgBox("��ȷ����������", vbOKCancel)
End Function

Private Sub SaveData(Index As Long)
    Dim AdrnFileNum, RealFileNum As Integer                   '�����ļ���
    
    Dim AdrnData As adrn                                      '����һ��Adrn���ݿ�

    '��ȡ��Adrn����
    AdrnFileNum = FreeFile                                    '����Adrn�ļ���
    Open App.Path & "\data\adrn.bin" For Binary As AdrnFileNum
    RealFileNum = FreeFile                                    '����Real�ļ���
    Open App.Path & "\data\real.bin" For Binary As RealFileNum
    AdrnData = AdrnIndex(Index)
    AdrnData.addr = LOF(RealFileNum)
    If LOF(AdrnFileNum) = 0 Then
        Put AdrnFileNum, , AdrnData
    Else
        Put AdrnFileNum, LOF(AdrnFileNum) + 1, AdrnData
    End If
    Close AdrnFileNum
    
    '��ȡ��Real����
    Dim FileName As String
    Dim TempStr As String
    Dim MyReal As Real
    Dim buff() As Byte
    FileName = Text1.Text & "\real_*.bin"
    TempStr = ManhuntFile(FileName)
    
    If TempStr = "0" Then
        TempStr = "real.bin"
    End If
    
    FileName = FreeFile
    Open Text1.Text & "\" & TempStr For Binary Access Read As FileName
    
    ReDim buff(1 To AdrnIndex(Index).datalen) As Byte
    
    Get FileName, AdrnIndex(Index).addr + 1, buff
    
    CopyMemory MyReal, buff(1), 16
    
    Close FileName
    
    If LOF(RealFileNum) = 0 Then
        Put RealFileNum, , buff
    Else
        Put RealFileNum, LOF(RealFileNum) + 1, buff
    End If
    Close RealFileNum
End Sub

Private Sub SaveSprData(Index As Long)
    Dim SprFileNum, SprAdrnFileNum As Integer                    '�����ļ���
    Dim SprAdrnData As Spradrn                                   '����һ��Spr���ݿ�
    
    SprAdrnData = SpradrnIndex(Index)
    

    '��ȡ��Spr����
    SprFileNum = FreeFile                                        '����Spr�ļ���
    Open App.Path & "\data\spr.bin" For Binary As SprFileNum
    '��ȡ��SprAdrn����
    SprAdrnFileNum = FreeFile                                    '����SprAdrn�ļ���
    Open App.Path & "\data\spradrn.bin" For Binary As SprAdrnFileNum
    
    SprAdrnData.addr = LOF(SprFileNum)
    
    If LOF(SprAdrnFileNum) = 0 Then
        Put SprAdrnFileNum, , SprAdrnData
    Else
        Put SprAdrnFileNum, LOF(SprAdrnFileNum) + 1, SprAdrnData
    End If
    Close SprAdrnFileNum
    
    '��ȡ��Spr����
    Dim FileName As String
    Dim TempStr As String
    Dim Current_ActionNum As Long
    Current_ActionNum = Val(Text23.Text)
    
    FileName = Text1.Text & "\spr_*.bin"
    TempStr = ManhuntFile(FileName)
    
    If TempStr = "0" Then
        TempStr = "spr.bin"
    End If
    
    Dim MySpr As Spr
    Dim addr As Long
    addr = SpradrnIndex(Index).addr + 1
    
    FileName = FreeFile
    '���ļ�
    Open Text1.Text & "\" & TempStr For Binary Access Read As FileName
    
    Dim i As Long
    Dim j As Long
    Dim ActionNum As Long
    Dim PictureNo() As Long
    Dim PictureNum As Long
    ReDim Preserve PictureNo(0 To PictureNum)
    For ActionNum = 0 To SprAdrnData.ActionNum - 1
        '��ָ������
        For i = 0 To SpradrnIndex(Index).ActionNum - 1
            Get FileName, addr, MySpr
            addr = addr + Len(MySpr) + (MySpr.Number * 10)
            If Current_ActionNum = i Then Exit For
        Next
        
        If LOF(SprFileNum) = 0 Then
            Put SprFileNum, , MySpr
        Else
            Put SprFileNum, LOF(SprFileNum) + 1, MySpr
        End If
        
        If MySpr.Number > 0 Then
            ReDim buff(1 To 10) As Byte
            ReDim MySequence(1 To MySpr.Number) As Sequence

            For i = 1 To MySpr.Number
                Get FileName, , buff
                CopyMemory MySequence(i), buff(1), 10
                
                For j = 0 To PictureNum
                    If PictureNo(j) = MySequence(i).PictureNum Then
                        Exit For
                    End If
                    If j = PictureNum Then
                        PictureNo(j) = MySequence(i).PictureNum
                        PictureNum = PictureNum + 1
                        ReDim Preserve PictureNo(0 To PictureNum)
                        SaveData MySequence(i).PictureNum
                    End If
                Next j
                
                If LOF(SprFileNum) = 0 Then
                    Put SprFileNum, , MySequence(i)
                Else
                    Put SprFileNum, LOF(SprFileNum) + 1, MySequence(i)
                End If
            Next i
        End If
    Next
    
    Close FileName
    Close SprFileNum
    
End Sub
