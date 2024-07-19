@ECHO OFF
:Mapname
REM Change dir first
cd .\compile

SET /P mapname=Map to compile: 
IF "%mapname%"=="" GOTO Error

REM Check if the BSP file exists
IF NOT exist %mapname%.map GOTO Error_NoMAP

ECHO Compiling %mapname%...
hlrad -bumpmaps -low -chart -blur 1.0 -smooth2 60 -notexscale -minlight 1 -estimate %mapname%.map
GOTO CopyPrompt

:CopyPrompt
SET /P CopyInput=Copy BSP to maps?(Y/N):
IF "%CopyInput%"=="Y" ( GOTO CopyBSP ) ELSE ( IF "%CopyInput%"=="N" ( GOTO Exit ) ELSE ( GOTO CopyPrompt_Error ) )

:CopyBSP
ECHO Copying "compile/%mapname%.bsp" to "maps/%mapname%.bsp"
copy %mapname%.bsp ..\..\maps\ /y

IF exist ./%mapname%.ald GOTO Copy_ALD

GOTO Exit

:Copy_ALD
ECHO Copying "compile/%mapname%.ald" to "maps/%mapname%.ald"
copy %mapname%.ald ..\..\maps\ /y
GOTO Exit

:CopyPrompt_Error
ECHO Only "Y/N" accepted
GOTO CopyPrompt

:Error
ECHO Error: No map specified!
GOTO Mapname

:Error_NoMAP
ECHO Error: File %mapname%.map is missing
GOTO Exit

:Error_Leak
ECHO Error: Leak detected. Ceasing compile.
GOTO Exit

:Exit
set /p DUMMY=Hit ENTER to exit...