#!/bin/csh

set inName   = (`getArgDD $argv -in       data/spec%03d.fid   data/spec%03d.fid`)
set outName  = (`getArgDD $argv -out      spec_xz.ft2  spec.ft2`)
set auxName  = (`getArgDD $argv -aux      spec_xz.fid  spec_xz.fid`)
set specType = (`getArgDD $argv -type     HN             NMR`)
set yPlane   = (`getArgDD $argv -yPlane  _yPlane_  1`)
set aPlane   = (`getArgDD $argv -aPlane  _aPlane_  1`)
set planeHi  = (`getArgDD $argv -planeHi  10%         2.5@`)
set dimCount = (`getArgDD $argv -ndim     3              3` )

set graphFlag = (`getLogArgDD $argv -graph     -nograph     _graphFlag_ 1 0`)
set drawPlane = (`getLogArgDD $argv -drawPlane -nodrawPlane _drawPlane_ 1 0`)

if (`string csame $specType HN`) then
   set xSOL = (`getArgDD $argv -xSOL  SOL         SOL`)
else
   set xSOL = (`getArgDD $argv -xSOL  SOL         None`)
endif

if (`string csame $xSOL None`) then
   set xSOL = NULL
endif
 
set xAPOD = (`getArgDD $argv -xAPOD  SP          SP`)
set xQ1   = (`getArgDD $argv -xQ1    0.5           0.5`)
set xQ2   = (`getArgDD $argv -xQ2    0.95          0.95`)
set xQ3   = (`getArgDD $argv -xQ3    2             2`)
set xELB  = (`getArgDD $argv -xELB   0            0.0`)
set xGLB  = (`getArgDD $argv -xGLB   0            0.0`)
set xGOFF = (`getArgDD $argv -xGOFF  0           0.0`)
set xC1   = (`getArgDD $argv -xC1    0.5           1.0`)
set xP0   = (`getArgDD $argv -xP0    -201.1        0.0`)
set xP1   = (`getArgDD $argv -xP1    0.0           0.0`)

set xZFARG   = (`getArgDD $argv -xZFARG    zf=1,auto     zf=1,auto`)
set xEXTX1   = (`getArgDD $argv -xEXTX1    9.780ppm      0%`)
set xEXTXN   = (`getArgDD $argv -xEXTXN    5.981ppm      100%`)
set xBASEARG = (`getArgDD $argv -xBASEARG  POLY,auto   POLY,auto`)

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

set zAPOD = (`getArgDD $argv -zAPOD  SP          SP`)
set zQ1   = (`getArgDD $argv -zQ1    0.5           0.5`)
set zQ2   = (`getArgDD $argv -zQ2    0.95          0.95`)
set zQ3   = (`getArgDD $argv -zQ3    1             1`)
set zELB  = (`getArgDD $argv -zELB  _zELB_  0.0`)
set zGLB  = (`getArgDD $argv -zGLB  _zGLB_  0.0`)
set zGOFF = (`getArgDD $argv -zGOFF _zGOFF_ 0.0`)
set zC1   = (`getArgDD $argv -zC1    0.5           1.0`)
set zP0   = (`getArgDD $argv -zP0    0.0           0.0`)
set zP1   = (`getArgDD $argv -zP1    0.0           0.0`)

set zZFARG   = (`getArgDD $argv -zZFARG  zf=1,auto     zf=1,auto`)
set zFTARG   = (`getArgDD $argv -zFTARG  None          None`)
set zBASEARG = (`getArgDD $argv -zFTARG  NULL        NULL`)

if (`string csame $zC1 Auto`) then
   set v = (`printf "%f" $zP1`)
   set i = (`IMATH "abs($v) < 10.0"`)

   if ($i == 0) then
      set zC1 = 1.0
   else
      set zC1 = 0.5
   endif
endif

if (`string csame $zBASEARG Auto`) then
   set v = (`printf "%f" $zP1`)
   set i = (`IMATH "abs($v) > 10.0 && abs($v-180.0) > 10.0"`)

   if ($i == 0) then
      echo "Z-Axis Baseline Auto Mode: No Baseline Correction Used."
      set zBASEARG = NULL
   else
      echo "Z-Axis Baseline Auto Mode: Using POLY -auto Correction."
      set zBASEARG = POLY,auto,ord=0
   endif
endif

set yP0   = (`getArgDD $argv -yP0    45.0          0.0`)
set yP1   = (`getArgDD $argv -yP1    0.0           0.0`)

if (`string csame $yPlane Auto`) then
   set v0 = (`printf "%f" $yP0`)
   set v1 = (`printf "%f" $yP1`)
   set i  = (`IMATH "abs(abs($v0) - 90.0) < 5.0 && abs($v1) < 5.0"`)

   if ($i == 0) then
      set yPlane = 1
   else
      set yPlane = 2
   endif
endif

set aP0   = (`getArgDD $argv -aP0   _aP0_   0.0`)
set aP1   = (`getArgDD $argv -aP1   _aP1_   0.0`)

if (`string csame $aPlane Auto`) then
   set v0 = (`printf "%f" $aP0`)
   set v1 = (`printf "%f" $aP1`)
   set i  = (`IMATH "abs(abs($v0) - 90.0) < 5.0 && abs($v1) < 5.0"`)

   if ($i == 0) then
      set aPlane = 1
   else
      set aPlane = 2
   endif
endif

set thisName = (`nmrPrintf $inName 1 1`)

if (!(-e $thisName)) then
   echo "$0 Notice: Missing Input $thisName"
   exit 1
endif

if ($dimCount == 3) then
   ext_xz.com $inName $auxName $yPlane
else
   ext_xz.com $inName $auxName $yPlane $aPlane
endif

if (!(-e $auxName)) then
   echo "$0 Notice: Missing Input $auxName"
   exit 0
endif

nmrPipe -in $auxName \
| nmrPipe -fn `expandArg0 $xSOL` \
| nmrPipe -fn APOD -qName $xAPOD -q1 $xQ1 -q2 $xQ2 -q3 $xQ3 -elb $xELB -glb $xGLB -goff $xGOFF -c $xC1 \
| nmrPipe -fn ZF `expandArg $xZFARG` \
| nmrPipe -fn FT -verb \
| nmrPipe -fn PS -p0 $xP0 -p1 $xP1 -di \
| nmrPipe -fn EXT -x1 $xEXTX1 -xn $xEXTXN -sw \
| nmrPipe -fn TP \
| nmrPipe -fn APOD -qName $zAPOD -q1 $zQ1 -q2 $zQ2 -q3 $zQ3 -elb $zELB -glb $zGLB -goff $zGOFF -c $zC1 \
| nmrPipe -fn ZF `expandArg $zZFARG` \
| nmrPipe -fn FT `expandArg $zFTARG` -verb \
| nmrPipe -fn PS -p0 $zP0 -p1 $zP1 -di \
| nmrPipe -fn `expandArg0 $zBASEARG` \
| nmrPipe -fn TP \
| nmrPipe -fn `expandArg0 $xBASEARG` -verb \
  -out $outName -ov

if ($graphFlag != 0 && $drawPlane != 0) then
   set hiInfo = (`getSpecHi.tcl -in $outName $* -hi $planeHi`)
   nmrDraw -in $outName $hiInfo `getGeom -east -xAdj 100 -yAdj 200` &
   sleep 3
endif

