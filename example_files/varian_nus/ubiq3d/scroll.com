#!/bin/csh

echo Using HN/N assignments from existing manually-created hn.ass.tab

set inList = (n15noe_std.fid/ft/test%03d.ft3)
set hiList = (7.8e+3)
set tList  = (US_FT)

if (-e n15noe_nus.fid/ist/test001.ft3) then
   set inList = ($inList n15noe_nus.fid/ist/test%03d.ft3)
   set hiList = ($hiList 4.0e+3)
   set tList  = ($tList NUS_25PCT_IST)
endif

scroll.tcl $* \
 -noyGrid -colorB dimGray -colorW black -fg black \
 -tab hn.ass.tab -pair -lab INDEX -seq seqInfo.tab -yw 1.8ppm \
 -hi $hiList -in $inList -title $tList

