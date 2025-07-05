@echo off
echo Este script requiere ImageMagick instalado para convertir SVG a PNG
echo Intentando convertir v5CodigoQR.svg a v5CodigoQR.png
magick convert v5CodigoQR.svg v5CodigoQR.png
if %ERRORLEVEL% EQU 0 (
  echo Conversion exitosa. PNG guardado en: v5CodigoQR.png
) else (
  echo Error: No se pudo convertir a PNG. ImageMagick podria no estar instalado.
)
pause
