# Microsoft Developer Studio Generated NMAKE File, Format Version 4.10
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=csend - Win32 Debug
!MESSAGE 構成が指定されていません。ﾃﾞﾌｫﾙﾄの csend - Win32 Debug を設定します。
!ENDIF 

!IF "$(CFG)" != "csend - Win32 Release" && "$(CFG)" != "csend - Win32 Debug" &&\
 "$(CFG)" != "csend - Win32 Release2"
!MESSAGE 指定された ﾋﾞﾙﾄﾞ ﾓｰﾄﾞ "$(CFG)" は正しくありません。
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛ 'CFG' を定義することによって
!MESSAGE NMAKE 実行時にﾋﾞﾙﾄﾞ ﾓｰﾄﾞを指定できます。例えば:
!MESSAGE 
!MESSAGE NMAKE /f "csend.mak" CFG="csend - Win32 Debug"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "csend - Win32 Release" ("Win32 (x86) Application" 用)
!MESSAGE "csend - Win32 Debug" ("Win32 (x86) Application" 用)
!MESSAGE "csend - Win32 Release2" ("Win32 (x86) Application" 用)
!MESSAGE 
!ERROR 無効な構成が指定されています。
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "csend - Win32 Debug"
MTL=mktyplib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "csend - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\csend.exe"

CLEAN : 
	-@erase "$(INTDIR)\csend.obj"
	-@erase "$(INTDIR)\csend.pch"
	-@erase "$(INTDIR)\csend.res"
	-@erase "$(INTDIR)\csendDlg.obj"
	-@erase "$(INTDIR)\InputBox.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(OUTDIR)\csend.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_MBCS" /Fp"$(INTDIR)/csend.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
RSC_PROJ=/l 0x411 /fo"$(INTDIR)/csend.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/csend.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/csend.pdb" /machine:I386 /out:"$(OUTDIR)/csend.exe" 
LINK32_OBJS= \
	"$(INTDIR)\csend.obj" \
	"$(INTDIR)\csend.res" \
	"$(INTDIR)\csendDlg.obj" \
	"$(INTDIR)\InputBox.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\csend.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "csend - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\csend.exe"

CLEAN : 
	-@erase "$(INTDIR)\csend.obj"
	-@erase "$(INTDIR)\csend.pch"
	-@erase "$(INTDIR)\csend.res"
	-@erase "$(INTDIR)\csendDlg.obj"
	-@erase "$(INTDIR)\InputBox.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\csend.exe"
	-@erase "$(OUTDIR)\csend.ilk"
	-@erase "$(OUTDIR)\csend.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_MBCS" /Fp"$(INTDIR)/csend.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
RSC_PROJ=/l 0x411 /fo"$(INTDIR)/csend.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/csend.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/csend.pdb" /debug /machine:I386 /out:"$(OUTDIR)/csend.exe" 
LINK32_OBJS= \
	"$(INTDIR)\csend.obj" \
	"$(INTDIR)\csend.res" \
	"$(INTDIR)\csendDlg.obj" \
	"$(INTDIR)\InputBox.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\csend.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "csend - Win32 Release2"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "csend___"
# PROP BASE Intermediate_Dir "csend___"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "csend___"
# PROP Intermediate_Dir "csend___"
# PROP Target_Dir ""
OUTDIR=.\csend___
INTDIR=.\csend___

ALL : "$(OUTDIR)\csend.exe"

CLEAN : 
	-@erase "$(INTDIR)\csend.obj"
	-@erase "$(INTDIR)\csend.pch"
	-@erase "$(INTDIR)\csend.res"
	-@erase "$(INTDIR)\csendDlg.obj"
	-@erase "$(INTDIR)\InputBox.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(OUTDIR)\csend.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/csend.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\csend___/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x411 /fo"$(INTDIR)/csend.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/csend.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/csend.pdb" /machine:I386 /out:"$(OUTDIR)/csend.exe" 
LINK32_OBJS= \
	"$(INTDIR)\csend.obj" \
	"$(INTDIR)\csend.res" \
	"$(INTDIR)\csendDlg.obj" \
	"$(INTDIR)\InputBox.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\csend.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "csend - Win32 Release"
# Name "csend - Win32 Debug"
# Name "csend - Win32 Release2"

!IF  "$(CFG)" == "csend - Win32 Release"

!ELSEIF  "$(CFG)" == "csend - Win32 Debug"

!ELSEIF  "$(CFG)" == "csend - Win32 Release2"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ReadMe.txt

!IF  "$(CFG)" == "csend - Win32 Release"

!ELSEIF  "$(CFG)" == "csend - Win32 Debug"

!ELSEIF  "$(CFG)" == "csend - Win32 Release2"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\csend.cpp

!IF  "$(CFG)" == "csend - Win32 Release"

DEP_CPP_CSEND=\
	".\csend.h"\
	".\csendDlg.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\csend.obj" : $(SOURCE) $(DEP_CPP_CSEND) "$(INTDIR)"\
 "$(INTDIR)\csend.pch"


!ELSEIF  "$(CFG)" == "csend - Win32 Debug"

DEP_CPP_CSEND=\
	".\csend.h"\
	".\csendDlg.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\csend.obj" : $(SOURCE) $(DEP_CPP_CSEND) "$(INTDIR)"\
 "$(INTDIR)\csend.pch"


!ELSEIF  "$(CFG)" == "csend - Win32 Release2"

DEP_CPP_CSEND=\
	".\csend.h"\
	".\csendDlg.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\csend.obj" : $(SOURCE) $(DEP_CPP_CSEND) "$(INTDIR)"\
 "$(INTDIR)\csend.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\csendDlg.cpp

!IF  "$(CFG)" == "csend - Win32 Release"

DEP_CPP_CSENDD=\
	".\csend.h"\
	".\csendDlg.h"\
	".\InputBox.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\csendDlg.obj" : $(SOURCE) $(DEP_CPP_CSENDD) "$(INTDIR)"\
 "$(INTDIR)\csend.pch"


!ELSEIF  "$(CFG)" == "csend - Win32 Debug"

DEP_CPP_CSENDD=\
	".\csend.h"\
	".\csendDlg.h"\
	".\InputBox.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\csendDlg.obj" : $(SOURCE) $(DEP_CPP_CSENDD) "$(INTDIR)"\
 "$(INTDIR)\csend.pch"


!ELSEIF  "$(CFG)" == "csend - Win32 Release2"

DEP_CPP_CSENDD=\
	".\csend.h"\
	".\csendDlg.h"\
	".\InputBox.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\csendDlg.obj" : $(SOURCE) $(DEP_CPP_CSENDD) "$(INTDIR)"\
 "$(INTDIR)\csend.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "csend - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS"\
 /Fp"$(INTDIR)/csend.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\csend.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "csend - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_MBCS" /Fp"$(INTDIR)/csend.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\csend.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "csend - Win32 Release2"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/csend.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\csend.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\csend.rc
DEP_RSC_CSEND_=\
	".\res\csend.ico"\
	".\res\csend.rc2"\
	

!IF  "$(CFG)" == "csend - Win32 Release"


"$(INTDIR)\csend.res" : $(SOURCE) $(DEP_RSC_CSEND_) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "csend - Win32 Debug"


"$(INTDIR)\csend.res" : $(SOURCE) $(DEP_RSC_CSEND_) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "csend - Win32 Release2"


"$(INTDIR)\csend.res" : $(SOURCE) $(DEP_RSC_CSEND_) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\InputBox.cpp

!IF  "$(CFG)" == "csend - Win32 Release"

DEP_CPP_INPUT=\
	".\csend.h"\
	".\InputBox.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\InputBox.obj" : $(SOURCE) $(DEP_CPP_INPUT) "$(INTDIR)"\
 "$(INTDIR)\csend.pch"


!ELSEIF  "$(CFG)" == "csend - Win32 Debug"

DEP_CPP_INPUT=\
	".\csend.h"\
	".\InputBox.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\InputBox.obj" : $(SOURCE) $(DEP_CPP_INPUT) "$(INTDIR)"\
 "$(INTDIR)\csend.pch"


!ELSEIF  "$(CFG)" == "csend - Win32 Release2"

DEP_CPP_INPUT=\
	".\csend.h"\
	".\InputBox.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\InputBox.obj" : $(SOURCE) $(DEP_CPP_INPUT) "$(INTDIR)"\
 "$(INTDIR)\csend.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
