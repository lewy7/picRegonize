@echo off
setlocal enabledelayedexpansion
set /a num=000
rm ./*.txt
rem 排序/o:？ -代表逆序
rem n按名称的字母顺序
rem e按扩展名的字母顺序
rem d按日期和时间，最早的优先 
rem s按大小，最小的优先
rem g在文件之前分组的目录
for /f "tokens=* delims=" %%i in ('dir /b /a-d /o:d *.png') do (
set /a num=num+1
echo "%%i"  !num!
tesseract  "%%i"  !num! 
sed "$d" !num!.txt  > a.txt
sed "/|/d"  a.txt > b.txt
sed "/^$/d"  b.txt > c.txt
cat  c.txt  >> all.txt
rem cat !num!.txt >> all.txt
)
rm ./*.png
pause
