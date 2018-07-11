#!/bin/csh

set inName    = (`getArgDD $argv -in    spec.fid     spec.fid`)
set outName   = (`getArgDD $argv -out   spec.ft2    spec.ft2`)
set specType  = (`getArgDD $argv -type  HC             NMR`)
set hi        = (`getArgDD $argv -hi    2.5%             2.5@`)

set graphFlag = (`getLogArgDD $argv -graph     -nograph     _graphFlag_ 1 0`)
set drawPlane = (`getLogArgDD $argv -drawPlane -nodrawPlane _drawPlane_ 1 0`)

if (!(-e $inName)) then
   echo "$0 Notice: Missing Input $inName"
   exit 1
endif

if (`string csame $specType HN`) then
   set xSOL = (`getArgDD $argv -xSOL  None        SOL`)
else
   set xSOL = (`getArgDD $argv -xSOL  None        None`)
endif

if (`string csame $xSOL None`) then
   set xSOL = NULL
endif
 
set xAPOD = (`getArgDD $argv -xAPOD  SP            SP`)
set xQ1   = (`getArgDD $argv -xQ1    0.5           0.5`)
set xQ2   = (`getArgDD $argv -xQ2    0.95          0.95`)
set xQ3   = (`getArgDD $argv -xQ3    2             2`)
set xELB  = (`getArgDD $argv -xELB   0.0          0.0`)
set xGLB  = (`getArgDD $argv -xGLB   0.0          0.0`)
set xGOFF = (`getArgDD $argv -xGOFF  0.0         0.0`)
set xC1   = (`getArgDD $argv -xC1    0.5           1.0`)
set xP0   = (`getArgDD $argv -xP0    25.4208572191    0.0`)
set xP1   = (`getArgDD $argv -xP1    0.0           0.0`)

set xZFARG   = (`getArgDD $argv -xZFARG    zf=1,auto     zf=1,auto`)
set xEXTX1   = (`getArgDD $argv -xEXTX1    0%            0%`)
set xEXTXN   = (`getArgDD $argv -xEXTXN    100%          100%`)
set xBASEARG = (`getArgDD $argv -xBASEARG  NULL        POLY,auto`)

if (`string csame $xBASEARG Auto`) then
   set xBASEARG = POLY,auto
endif

if (`string csame $xC1 Auto`) then
   set v = (`printf "%f" $xP1`)
   set i = (`IMATH "abs($v) < 10.0"`)

   if ($i == 0) then
      set xC1 = 1.0
   else
      set xC1 = 0.5
   endif
endif

set yAPOD = (`getArgDD $argv -yAPOD  SP            SP`)
set yQ1   = (`getArgDD $argv -yQ1    0.5           0.5`)
set yQ2   = (`getArgDD $argv -yQ2    0.95          0.95`)
set yQ3   = (`getArgDD $argv -yQ3    1             1`)
set yELB  = (`getArgDD $argv -yELB   0.0          0.0`)
set yGLB  = (`getArgDD $argv -yGLB   0.0          0.0`)
set yGOFF = (`getArgDD $argv -yGOFF  0.0         0.0`)
set yC1   = (`getArgDD $argv -yC1    0.5           1.0`)
set yP0   = (`getArgDD $argv -yP0    0.0           0.0`)
set yP1   = (`getArgDD $argv -yP1    0.0           0.0`)

set yZFARG   = (`getArgDD $argv -yZFARG    zf=1,auto     zf=1,auto`)
set yFTARG   = (`getArgDD $argv -yFTARG    None          None`)
set yBASEARG = (`getArgDD $argv -yBASEARG  NULL        NULL`)

if (`string csame $yC1 Auto`) then
   set v = (`printf "%f" $yP1`)
   set i = (`IMATH "abs($v) < 10.0"`)

   if ($i == 0) then
      set yC1 = 1.0
   else
      set yC1 = 0.5
   endif
endif

if (`string csame $yBASEARG Auto`) then
   set v = (`printf "%f" $yP1`)
   set i = (`IMATH "abs($v) > 10.0 && abs($v-180.0) > 10.0"`)

   if ($i == 0) then
      echo "Y-Axis Baseline Auto Mode: No Baseline Correction Used."
      set yBASEARG = NULL
   else
      echo "Y-Axis Baseline Auto Mode: Using POLY -auto Correction."
      set yBASEARG = POLY,auto,ord=0
   endif
endif

nmrPipe -in $inName \
| nmrPipe -fn `expandArg0 $xSOL` \
| nmrPipe -fn APOD -qName $xAPOD -q1 $xQ1 -q2 $xQ2 -q3 $xQ3 -elb $xELB -glb $xGLB -goff $xGOFF -c $xC1 \
| nmrPipe -fn ZF `expandArg $xZFARG` \
| nmrPipe -fn FT -verb \
| nmrPipe -fn PS -p0 $xP0 -p1 $xP1 -di \
| nmrPipe -fn EXT -x1 $xEXTX1 -xn $xEXTXN -sw \
| nmrPipe -fn TP \
| nmrPipe -fn APOD -qName $yAPOD -q1 $yQ1 -q2 $yQ2 -q3 $yQ3 -elb $yELB -glb $yGLB -goff $yGOFF -c $yC1 \
| nmrPipe -fn ZF `expandArg $yZFARG` \
| nmrPipe -fn FT `expandArg $yFTARG` -verb \
| nmrPipe -fn PS -p0 $yP0 -p1 $yP1 -di \
| nmrPipe -fn `expandArg0 $yBASEARG` \
| nmrPipe -fn TP \
| nmrPipe -fn `expandArg0 $xBASEARG` -verb \
  -out $outName -ov

proj2D.tcl -in $outName -axis -sum

if ($graphFlag != 0 && $drawPlane != 0) then
   set hiInfo = (`getSpecHi.tcl -in $outName $* -hi $hi`)
   nmrDraw -in $outName $hiInfo `getGeom -west -xAdj 100 -yAdj 200` &
   sleep 3
endif
