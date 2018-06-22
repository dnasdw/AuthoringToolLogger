PUSH "%~dp0"
COPY AuthoringTool.exe AuthoringToolOrig.exe
COPY AuthoringToolLogger.exe AuthoringTool.exe
DEL AuthoringToolRename.bat
POPD
