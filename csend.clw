; CLW ファイルは MFC ClassWizard の情報を含んでいます。

[General Info]
Version=1
LastClass=CCsendDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "csend.h"

ClassCount=4
Class1=CCsendApp
Class2=CCsendDlg
Class3=CAboutDlg

ResourceCount=5
Resource1=IDD_CSEND_DIALOG
Resource2=IDR_MAINFRAME
Resource3=IDR_MENU1
Class4=CInputBox
Resource4=IDD_DIALOG1
Resource5=IDD_ABOUTBOX

[CLS:CCsendApp]
Type=0
HeaderFile=csend.h
ImplementationFile=csend.cpp
Filter=N
LastObject=CCsendApp

[CLS:CCsendDlg]
Type=0
HeaderFile=csendDlg.h
ImplementationFile=csendDlg.cpp
Filter=D
LastObject=ID_HELP
BaseClass=CDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=csendDlg.h
ImplementationFile=csendDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=6
Control1=IDOK,button,1342373889
Control2=IDC_STATIC,static,1342177283
Control3=IDC_STATIC,static,1342308481
Control4=IDC_STATIC,static,1342308353
Control5=IDC_STATIC,static,1342308353
Control6=IDC_STATIC,static,1342308353

[DLG:IDD_CSEND_DIALOG]
Type=1
Class=CCsendDlg
ControlCount=3
Control1=IDC_CLIST,listbox,1344341379
Control2=IDOK,button,1073741824
Control3=IDCANCEL,button,1073741824

[DLG:IDD_DIALOG1]
Type=1
Class=CInputBox
ControlCount=3
Control1=IDC_EDIT1,edit,1350631552
Control2=IDOK,button,1342242817
Control3=IDCANCEL,button,1342242816

[CLS:CInputBox]
Type=0
HeaderFile=InputBox.h
ImplementationFile=InputBox.cpp
BaseClass=CDialog
Filter=D
LastObject=CInputBox
VirtualFilter=dWC

[MNU:IDR_MENU1]
Type=1
Class=CCsendDlg
Command1=ID_ADDSTRING
Command2=ID_CHANGE
Command3=ID_DELETESTRING
Command4=ID_HELP
Command5=ID_EXIT
Command6=ID_ABOUT
CommandCount=6

