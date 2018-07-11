#!/bin/csh

set inName    = (`getArgDD $argv -in      data/spec%03d.fid   data/spec%03d.fid`)
set outName   = (`getArgDD $argv -out     ft/spec%03d.ft3  ft/spec%03d.ft3`)
set auxName   = (`getArgDD $argv -aux     ft/spec%03d.ft2  ft/spec%03d.ft2`)
set specType  = (`getArgDD $argv -type    HN             NMR`)
set cubeHi    = (`getArgDD $argv -cubeHi  10%          0.15@`)

set graphFlag = (`getLogArgDD $argv -graph     -nograph     _graphFlag_ 1 0`)
set drawCube  = (`getLogArgDD $argv -drawCube  -nodrawCube  _drawCube_  1 0`)

set nID       = (`getArgD $argv -nID 1`)

set progressFlag = 1
set sendName     = nmrProgress.tcl

if (`flagLoc $argv -progress`) then
   set progressFlag = 1
   set sendName     = nmrProgress.tcl
endif

if (`flagLoc $argv -noprogress`) then
   set progressFlag = 0
   set sendName     = None
endif

if ($nID == 1 && $progressFlag != 0) then
   echo "Stopping any existing progress bar displays ..."
   nmrProgress.com -stop -noprogress
   echo "Starting new progress bar display ..."
   nmrProgress.tcl &
   sleep 3
   echo "Done starting new progress bar display."
endif

if (`string csame $specType HN`) then
   set xSOL = (`getArgDD $argv -xSOL  SOL         SOL`)
else
   set xSOL = (`getArgDD $argv -xSOL  SOL         None`)
endif

if (`string csame $xSOL None`) then
   set xSOL = NULL
endif
 
set xAPOD = (`getArgDD $argv -xAPOD  SP            SP`)
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

set yAPOD = (`getArgDD $argv -yAPOD  SP            SP`)
set yQ1   = (`getArgDD $argv -yQ1    0.5           0.5`)
set yQ2   = (`getArgDD $argv -yQ2    0.95          0.95`)
set yQ3   = (`getArgDD $argv -yQ3    1             1`)
set yELB  = (`getArgDD $argv -yELB   0            0.0`)
set yGLB  = (`getArgDD $argv -yGLB   0            0.0`)
set yGOFF = (`getArgDD $argv -yGOFF  0           0.0`)
set yC1   = (`getArgDD $argv -yC1    0.5           1.0`)
set yP0   = (`getArgDD $argv -yP0    45.0          0.0`)
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

set zAPOD = (`getArgDD $argv -zAPOD  SP            SP`)
set zQ1   = (`getArgDD $argv -zQ1    0.5           0.5`)
set zQ2   = (`getArgDD $argv -zQ2    0.95          0.95`)
set zQ3   = (`getArgDD $argv -zQ3    1             1`)
set zELB  = (`getArgDD $argv -zELB  _zELB_  0.0`)
set zGLB  = (`getArgDD $argv -zGLB  _zGLB_  0.0`)
set zGOFF = (`getArgDD $argv -zGOFF _zGOFF_ 0.0`)
set zC1   = (`getArgDD $argv -zC1    0.5           1.0`)
set zP0   = (`getArgDD $argv -zP0    0.0           0.0`)
set zP1   = (`getArgDD $argv -zP1    0.0           0.0`)

set zZFARG   = (`getArgDD $argv -zZFARG    zf=1,auto     zf=1,auto`)
set zFTARG   = (`getArgDD $argv -zFTARG    None          None`)
set zBASEARG = (`getArgDD $argv -zBASEARG  NULL        NULL`)

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

set thisName = (`nmrPrintf $inName 1 1`)

if (!(-e $thisName)) then
   echo "$0 Notice: Missing Input $thisName"
   exit 1
endif

xyz2pipe -in $inName -x -verb \
| nmrPipe -fn `expandArg0 $xSOL` \
| nmrPipe -fn APOD -qName $xAPOD -q1 $xQ1 -q2 $xQ2 -q3 $xQ3 -elb $xELB -glb $xGLB -goff $xGOFF -c $xC1 \
| nmrPipe -fn ZF `expandArg $xZFARG` \
| nmrPipe -fn FT \
| nmrPipe -fn PS -p0 $xP0 -p1 $xP1 -di \
| nmrPipe -fn EXT -x1 $xEXTX1 -xn $xEXTXN -sw \
| nmrSend -fn SEND -send $sendName -title FT \
| nmrPipe -fn TP \
| nmrPipe -fn APOD -qName $yAPOD -q1 $yQ1 -q2 $yQ2 -q3 $yQ3 -elb $yELB -glb $yGLB -goff $yGOFF -c $yC1 \
| nmrPipe -fn ZF `expandArg $yZFARG` \
| nmrPipe -fn FT `expandArg $yFTARG` \
| nmrPipe -fn PS -p0 $yP0 -p1 $yP1 -di \
| nmrSend -fn SEND -send $sendName -title FT \
| nmrPipe -fn `expandArg0 $yBASEARG` \
| nmrPipe -fn TP \
| nmrPipe -fn `expandArg0 $xBASEARG` \
| nmrSend -fn SEND -send $sendName -title "Baseline Correct" \
| pipe2xyz -out $auxName -x -ov

xyz2pipe -in $auxName -z -verb \
| nmrPipe -fn APOD -qName $zAPOD -q1 $zQ1 -q2 $zQ2 -q3 $zQ3 -elb $zELB -glb $zGLB -goff $zGOFF -c $zC1 \
| nmrPipe -fn ZF `expandArg $zZFARG` \
| nmrPipe -fn FT `expandArg $zFTARG` \
| nmrPipe -fn PS -p0 $zP0 -p1 $zP1 -di \
| nmrPipe -fn `expandArg0 $zBASEARG` \
| nmrSend -fn SEND -send $sendName -title FT \
| pipe2xyz -out $outName -z

proj3D.tcl -in $outName -axis -sum

if ($graphFlag != 0 && $drawCube != 0) then
   nmrCube.tcl -in $outName -xOff 300 -yOff 200 -hi $cubeHi &
   sleep 10 
endif 
