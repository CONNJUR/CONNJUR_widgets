#!/bin/csh

proj3D.tcl -in ft/test%03d.ft3
peakHN.tcl -in ft/test%03d.ft3 -out hn.proj.tab -proj -hi 2% 

set inList = (ft/test%03d.ft3)
set hiList = (2%)
set tList  = (FT)
set pList  = (multi)
set nList  = (multi)

if (-e lp/test001.ft3) then
   set inList = ($inList lp/test%03d.ft3)
   set hiList = ($hiList 2%)
   set tList  = ($tList  LP)
   set pList  = ($pList green)
   set nList  = ($nList yellow)
endif

if (-e ist/test001.ft3) then
   set inList = ($inList ist/test%03d.ft3)
   set hiList = ($hiList 2%)
   set tList  = ($tList  IST)
   set pList  = ($pList  pink)
   set nList  = ($nList  white)
endif

scroll.tcl $* -tab hn.proj.tab -yw 1.8ppm -pair \
 -noyGrid -colorB dimGray -colorW black -fg black \
 -in $inList -hi $hiList -title $tList  -colorP $pList -colorN $nList

