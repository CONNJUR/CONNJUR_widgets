#!/bin/csh

basicFT2Mag.com -in test.fid -out test.ft2 -yFTARG neg

nmrPipe -in test.fid \
| nmrPipe -fn TP \
| nmrPipe -fn EXT -xn 50% -sw \
| nmrPipe -fn TP \
   -out trunc.fid -ov

basicFT2Mag.com -in trunc.fid -out trunc.ft2 -yFTARG neg

sethdr test.ft2  -title STD_FULL_FT
sethdr trunc.ft2 -title STD_50pct_FT

