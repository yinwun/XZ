VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "comdlg32.ocx"
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Begin VB.Form Form2 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "��ͼ�༭��"
   ClientHeight    =   10095
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   15255
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   ScaleHeight     =   10095
   ScaleWidth      =   15255
   StartUpPosition =   3  '����ȱʡ
   Begin VB.PictureBox Picture2 
      Height          =   9615
      Left            =   4080
      ScaleHeight     =   9555
      ScaleWidth      =   11115
      TabIndex        =   20
      Top             =   0
      Width           =   11175
   End
   Begin MSComctlLib.StatusBar StatusBar1 
      Align           =   2  'Align Bottom
      Height          =   375
      Left            =   0
      TabIndex        =   16
      Top             =   9720
      Width           =   15255
      _ExtentX        =   26908
      _ExtentY        =   661
      _Version        =   393216
      BeginProperty Panels {8E3867A5-8586-11D1-B16A-00C0F0283628} 
         NumPanels       =   1
         BeginProperty Panel1 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
         EndProperty
      EndProperty
      OLEDropMode     =   1
   End
   Begin VB.Frame Frame2 
      Caption         =   "��ͼ��������̨"
      Height          =   7215
      Left            =   0
      TabIndex        =   9
      Top             =   2280
      Width           =   3975
      Begin VB.TextBox Text9 
         Height          =   270
         Left            =   2760
         TabIndex        =   29
         Top             =   2040
         Width           =   1095
      End
      Begin VB.CommandButton Command7 
         Caption         =   "��"
         Height          =   255
         Left            =   3000
         TabIndex        =   28
         Top             =   2880
         Width           =   855
      End
      Begin VB.TextBox Text8 
         Height          =   270
         Left            =   120
         TabIndex        =   27
         Top             =   2880
         Width           =   2775
      End
      Begin VB.TextBox Text7 
         Height          =   270
         Left            =   2760
         TabIndex        =   26
         Top             =   1680
         Width           =   1095
      End
      Begin VB.CommandButton Command6 
         Caption         =   "�ر�"
         Enabled         =   0   'False
         Height          =   255
         Left            =   2760
         TabIndex        =   25
         Top             =   2520
         Width           =   1095
      End
      Begin VB.TextBox Text6 
         Height          =   270
         Left            =   3120
         TabIndex        =   22
         Text            =   "1"
         Top             =   1320
         Width           =   735
      End
      Begin VB.TextBox Text5 
         Height          =   270
         Left            =   3120
         TabIndex        =   21
         Text            =   "1"
         Top             =   960
         Width           =   735
      End
      Begin VB.PictureBox Picture1 
         BackColor       =   &H80000008&
         Height          =   2535
         Left            =   120
         ScaleHeight     =   2475
         ScaleWidth      =   2475
         TabIndex        =   19
         Top             =   240
         Width           =   2535
      End
      Begin VB.CommandButton Command5 
         Caption         =   "��"
         Enabled         =   0   'False
         Height          =   255
         Left            =   2760
         TabIndex        =   14
         Top             =   240
         Width           =   1095
      End
      Begin VB.CommandButton Command4 
         Caption         =   "���ͻ���ͼƬ"
         Height          =   255
         Left            =   2040
         TabIndex        =   13
         Top             =   3240
         Width           =   1815
      End
      Begin VB.ListBox List2 
         Height          =   3480
         Left            =   2040
         TabIndex        =   12
         Top             =   3600
         Width           =   1815
      End
      Begin VB.ListBox List1 
         Height          =   3840
         Left            =   120
         TabIndex        =   11
         Top             =   3240
         Width           =   1815
      End
      Begin VB.CommandButton Command2 
         Caption         =   "����Χ"
         Enabled         =   0   'False
         Height          =   255
         Left            =   2760
         TabIndex        =   10
         Top             =   600
         Width           =   1095
      End
      Begin VB.Label Label7 
         AutoSize        =   -1  'True
         Caption         =   "��:"
         Height          =   180
         Left            =   2760
         TabIndex        =   24
         Top             =   1320
         Width           =   270
      End
      Begin VB.Label Label6 
         AutoSize        =   -1  'True
         Caption         =   "��:"
         Height          =   180
         Left            =   2760
         TabIndex        =   23
         Top             =   960
         Width           =   270
      End
   End
   Begin VB.Frame Frame1 
      Caption         =   "��ͼת��"
      Height          =   2175
      Left            =   0
      TabIndex        =   0
      Top             =   0
      Width           =   3975
      Begin VB.TextBox Text10 
         Height          =   375
         Left            =   480
         MultiLine       =   -1  'True
         TabIndex        =   30
         Top             =   1680
         Width           =   975
      End
      Begin VB.TextBox Text4 
         Height          =   270
         Left            =   120
         TabIndex        =   17
         Top             =   1320
         Width           =   2415
      End
      Begin VB.TextBox Text1 
         Height          =   270
         Left            =   120
         TabIndex        =   5
         Top             =   240
         Width           =   2775
      End
      Begin VB.TextBox Text2 
         Height          =   270
         Left            =   120
         TabIndex        =   4
         Top             =   960
         Width           =   2415
      End
      Begin VB.CommandButton Command3 
         Caption         =   "ת��"
         Height          =   255
         Left            =   2760
         Style           =   1  'Graphical
         TabIndex        =   3
         Top             =   1800
         Width           =   1095
      End
      Begin VB.TextBox Text3 
         Height          =   270
         Left            =   120
         TabIndex        =   2
         Top             =   600
         Width           =   2775
      End
      Begin VB.CheckBox Check1 
         Caption         =   "����ת��"
         Height          =   300
         Left            =   1560
         TabIndex        =   1
         Top             =   1800
         Width           =   1095
      End
      Begin VB.Label Label5 
         Caption         =   "����ͼ���"
         Height          =   255
         Left            =   2520
         TabIndex        =   18
         Top             =   1320
         Width           =   1215
      End
      Begin VB.Label Label4 
         Caption         =   "��Ŀ���ļ�"
         Height          =   255
         Left            =   2880
         TabIndex        =   15
         Top             =   240
         Width           =   975
      End
      Begin VB.Label Label1 
         Caption         =   "�ȴ�!"
         Height          =   255
         Left            =   120
         TabIndex        =   8
         Top             =   1800
         Width           =   2535
      End
      Begin VB.Label Label2 
         Caption         =   "������ļ�·��"
         Height          =   255
         Left            =   2520
         TabIndex        =   7
         Top             =   960
         Width           =   1335
      End
      Begin VB.Label Label3 
         Caption         =   "����ͼ����"
         Height          =   255
         Left            =   2880
         TabIndex        =   6
         Top             =   600
         Width           =   975
      End
   End
   Begin MSComDlg.CommonDialog CommonDialog1 
      Left            =   4080
      Top             =   0
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   393216
   End
End
Attribute VB_Name = "Form2"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Private Sub Command1_Click()
End Sub

Private Sub Command2_Click()
Dim X1 As Integer
Dim Y1 As Integer
Dim X2 As Integer
Dim Y2 As Integer
X1 = StrQ(Text5.Text, ",", 1)
Y1 = StrQ(Text5.Text, ",", 2)
X2 = StrQ(Text6.Text, ",", 1)
Y2 = StrQ(Text6.Text, ",", 2)
Text10.Text = ""
Dim B(1 To 3) As Byte
Dim A As Long

Dim Map_Tle As Integer
Dim Map_Obj As Integer

For c = Y1 To Y2
    For d = X1 To X2
        A = (c) * (Map_x) + d
        A = A * 2
        A = A + 45
        Get #10, A, B
        B(3) = B(1)
        B(1) = B(2)
        B(2) = B(3)
        CopyMemory A, B(1), 2
        Map_Tle = A
        A = ((Map_x * Map_y) + ((c * Map_x) + d)) * 2
        A = A + 45
        Get #10, A, B
        B(3) = B(1)
        B(1) = B(2)
        B(2) = B(3)
        CopyMemory A, B(1), 2
        Map_Obj = A
        Text10.Text = Text10.Text & "X:" & d & ",Y:" & c & ",Tle:" & Map_Tle & ",Obj:" & Map_Obj & Chr(13) & Chr(10)
    Next
Next
End Sub

Private Sub Command3_Click()
Label1.Caption = "��ȡ�ļ���........"
DoEvents
Dim buff() As Byte
Dim Dat1() As Byte
Dim X_Size As Integer
Dim Y_Size As Integer
Dim X_Byte(2) As Byte
Dim Y_Byte(2) As Byte
Dim L As Long
Dim L1 As Long
Dim Temp As Long
'��ȡ�ļ�
Open Text1.Text For Binary Access Read As #1
ReDim buff(3)
'��ȡ���ݳ���
Get #1, , buff
CopyMemory X_Byte(0), buff(0), 2
X_Byte(2) = X_Byte(1)
X_Byte(1) = X_Byte(0)
X_Byte(0) = X_Byte(2)
CopyMemory X_Size, buff(0), 2
Get #1, , buff
CopyMemory Y_Byte(0), buff(0), 2
Y_Byte(2) = Y_Byte(1)
Y_Byte(1) = Y_Byte(0)
Y_Byte(0) = Y_Byte(2)
CopyMemory Y_Size, buff(0), 2
'�ض�������
L = X_Size
L1 = Y_Size
L = L * L1
L = L * 4
ReDim Dat1(32 + 6 + 4 + L + 1)
'��ʼת��MAP�ļ�
'MAP�ļ�ͷ
Dat1(0) = &H4C
Dat1(1) = &H53
Dat1(2) = &H32
Dat1(3) = &H4D
Dat1(4) = &H41
Dat1(5) = &H50
'��ͼ���
Temp = Val(Text4.Text)
CopyMemory Dat1(6), Temp, 2
Dat1(8) = Dat1(6)
Dat1(6) = Dat1(7)
Dat1(7) = Dat1(8)
'��ͼ����
buff() = StrConv(Text3.Text & "|0", vbFromUnicode)
CopyMemory Dat1(8), buff(0), UBound(buff()) + 1
'��ͼͼ�����ݳ���
CopyMemory Dat1(40), X_Byte(0), 2
CopyMemory Dat1(42), Y_Byte(0), 2
'��ͼͼ������
ReDim buff(1)
For A = 1 To L Step 2
Label1.Caption = "ת����........"
DoEvents
Get #1, , buff
Dat1(43 + A) = buff(1)
If buff(0) = 63 And Check1.Value = 1 Then
buff(0) = 0
End If
Dat1(44 + A) = buff(0)
Next
'ѭ�����Data1������������MAP�ļ���Ϣ
Close #1
'����MPA�ļ�
Dim MyFs As New FileSystemObject, MyTxtS As TextStream
Set MyTxtS = MyFs.OpenTextFile(Text2.Text, ForReading, True)
Open Text2.Text For Binary Access Write As #2
Put #2, , Dat1
Close #2
Label1.Caption = "���......"
End Sub

Private Sub Command5_Click()
Dim B(1 To 3) As Byte
Dim A As Long
A = (Val(Text6.Text) * (Map_x)) + Text5.Text
A = A * 2
A = A + 45
Get #10, A, B
B(3) = B(1)
B(1) = B(2)
B(2) = B(3)
CopyMemory A, B(1), 2
Text7.Text = A
A = ((Map_x * Map_y) + ((Val(Text6.Text) * (Map_x)) + Text5.Text)) * 2
A = A + 45
Get #10, A, B
B(3) = B(1)
B(1) = B(2)
B(2) = B(3)
CopyMemory A, B(1), 2
Text9.Text = A
End Sub

Private Sub Command6_Click()
Close #10
Command6.Enabled = False
Command5.Enabled = False
Command2.Enabled = False
Command7.Enabled = True
End Sub

Private Sub Command7_Click()
CommonDialog1.ShowOpen
Open CommonDialog1.FileName For Binary Access Read As #10
Dim B(1 To 3) As Byte
Get #10, 41, B
B(3) = B(1)
B(1) = B(2)
B(2) = B(3)
CopyMemory Map_x, B(1), 2
Get #10, 43, B
B(3) = B(1)
B(1) = B(2)
B(2) = B(3)
CopyMemory Map_y, B(1), 2
Command6.Enabled = True
Command5.Enabled = True
Command2.Enabled = True
Command7.Enabled = False
End Sub

Private Sub Label2_Click()
CommonDialog1.FileName = Text1.Text
CommonDialog1.ShowSave
Text2.Text = CommonDialog1.FileName
End Sub

Private Sub Label4_Click()
CommonDialog1.FileName = Text1.Text
CommonDialog1.ShowOpen
Text1.Text = CommonDialog1.FileName
End Sub

