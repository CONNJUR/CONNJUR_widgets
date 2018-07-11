#!/bin/csh

set inName   = (`getArgDD $argv -in       ft/spec%03d.ft3   ft/spec%03d.ft3`)
set specType = (`getArgDD $argv -type     HN             NMR`)
set projHi   = (`getArgDD $argv -projHi   Full         Full`)
set stripHi  = (`getArgDD $argv -stripHi  3.0%        3.00@`)

set graphFlag  = (`getLogArgDD $argv -graph      -nograph      _graphFlag_ 1 0`)
set drawStrips = (`getLogArgDD $argv -drawStrips -nodrawStrips _drawStrips_ 1 0`)

if ($graphFlag == 0 || $drawStrips == 0) then
   exit 0
endif

set thisName = (`nmrPrintf $inName 1 1`)

if (!(-e $thisName)) then
   echo "$0 Notice: Missing Input $thisName"
   exit 1
endif

set dimCount = (`getParm -in $thisName -fmt %.0f -parm FDDIMCOUNT -dim NULL_DIM`)

if ($dimCount == 4) then
   echo "4D Data $inName"
   echo "No Strip Display will be drawn."
   exit 0   
endif

set hiInfo = (`getSpecHi.tcl -in $inName -hi $stripHi`)

if (`string csame $specType HC`) then
   proj3D.tcl -in $inName -abs
   peakHN.tcl -in $inName -out proj.tab -proj -hi $projHi -type HC

   vj_scroll.tcl -tab proj.tab -pair -lab INDEX -xVar X_PPM -yVar Y_PPM \
                 -noyGrid -colorB dimGray -colorW black -fg black -shade \
                 -in $inName $hiInfo &
else
   proj3D.tcl -in $inName -abs
   peakHN.tcl -in $inName -out proj.tab -proj -hi $projHi 

   vj_scroll.tcl -tab proj.tab -pair -lab INDEX \
                 -noyGrid -colorB dimGray -colorW black -fg black -shade \
                 -in $inName $hiInfo & 
endif

