@echo off


REM ========================================================
REM configure ark api directory
REM ========================================================
set ARK_API_DIR=""

REM ========================================================
REM configure project name 
REM ========================================================
set ProjectName=CuddlePlugin

REM ========================================================
REM configure description
REM ========================================================
set Description=overwrite dino walking

REM ========================================================
REM configure Version
REM ========================================================
set Version=1.0

REM ========================================================
REM configure min api version
REM ========================================================
set MinApiVersion=3.4

REM ========================================================
REM configure ResourceId
REM ========================================================
set ResourceId=42


set MAIN_DIR=%cd%
mkdir workspace
cd workspace
set WORKSPACE_DIR=%cd%

if not exist %ARK_API_DIR% (

echo #
echo #     no ark api directory configured
echo # 



if not exist %cd%\ARK-Server-API ( 

echo #
echo #    start cloning ark api from git repository
echo #

  git clone https://github.com/SaltyBay/ARK-Server-API.git --branch feature/UObject-GetPrivateStaticClass

)
  set ARK_API_DIR=%cd%/ARK-Server-API

echo #
echo #    ark api directory set to %cd%\ARK-Server-API
echo #


)

cmake .. -G "Visual Studio 15 Win64" ^
     -DProjectName=%ProjectName% ^
	 -DPATH_ARK_API=%ARK_API_DIR% 
	 
if errorlevel 1 pause % exit /b
	 
echo #
echo #    solution has been created in
echo #    %WORKSPACE_DIR%
echo #	


echo #
echo #    building the project in directory
echo #    %WORKSPACE_DIR%
echo #
cmake --build . --target ALL_BUILD --config Release 

if errorlevel 1 pause % exit /b
echo # 
echo #    finished! solution has been created in
echo #    %WORKSPACE_DIR%\Release 
echo # 

if not exist %WORKSPACE_DIR%\Release\config.json ( 
copy %MAIN_DIR%\config\config.json %WORKSPACE_DIR%\Release 
)


REM ========================================================
REM PluginInfo description
REM ========================================================
if exist %WORKSPACE_DIR%\Release\PluginInfo.json ( 
del %WORKSPACE_DIR%\Release\PluginInfo.json
)

cd %WORKSPACE_DIR%\Release  

echo {                                 >> PluginInfo.json
echo "FullName":"%ProjectName%",       >> PluginInfo.json
echo "Description":"%Description%",    >> PluginInfo.json
echo "Version":%Version%,              >> PluginInfo.json
echo "MinApiVersion":%MinApiVersion%,  >> PluginInfo.json
echo "ResourceId":%ResourceId%         >> PluginInfo.json
echo }                                 >> PluginInfo.json

   
explorer  %WORKSPACE_DIR%\Release 
 

Pause 




