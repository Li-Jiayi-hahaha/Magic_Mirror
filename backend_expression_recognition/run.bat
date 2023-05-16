SETLOCAL
SET PATH=%PATH%;..\..\samples\external\opencv\bin;..\..\bin;
SET NVAR_MODEL_DIR=..\..\bin\models
ExpressionApp.exe --cam_id=0 --cam_res=1280x720
