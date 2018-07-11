#!/bin/csh

basicFT2Mag.com -in test.fid -out test.ft2 -yFTARG neg

set istMaxRes = (`specStat.com -in test.ft2 -stat istMaxRes -brief`)

ist2DMag.com -istMaxRes $istMaxRes \
 -in test.fid -mask mask.fid -out ist.ft2 -yFTARG neg

sethdr test.ft2 -title NUS_50pct_FT
sethdr ist.ft2  -title NUS_50pct_IST

