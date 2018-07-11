#!/bin/csh

set inName    = (`getArgDD $argv -in    data/spec%03d.fid    data/spec%03d.fid`)
set outName   = (`getArgDD $argv -out   ist/spec%03d.ft3   ist/spec%03d.ft3`)
set specType  = (`getArgDD $argv -type  HN              NMR`)
set istHi     = (`getArgDD $argv -hi   _istHi_    10%`)

set maskName  = (`getArgDD $argv -nusMask _nusMaskName_ nusmask/spec%03d.fid`)
set profName  = (`getArgDD $argv -nusProf _nusProfName_ profYZ.dat`)

set istPrefix = (`getArgDD $argv -istPrefix _istPrefix_  ist`)
set tMult     = (`getArgDD $argv -istTMult  _istTMult_   0.70`)
set cMult     = (`getArgDD $argv -istCMult  _istCMult_   0.30`)
set istIter   = (`getArgDD $argv -istIter   _istIter_    1024`)
set istMaxRes = (`getArgDD $argv -istMaxRes  Auto         1.0`)

set istWinFlag  = (`getLogArgDD $argv -istWindow -noistWindow _istWinFlag_  1 0`)
set istSaveFlag = (`getLogArgDD $argv -noistSave -istSave     _istSaveFlag_ 0 1`)

set graphFlag  = (`getLogArgDD $argv -graph     -nograph     _graphFlag_  1 0`)
set drawPlane  = (`getLogArgDD $argv -drawPlane -nodrawPlane _drawPlane_  1 0`)

set thisName = (`nmrPrintf $inName 1 1`)

if (!(-e $thisName)) then
   echo "$0 Notice: Missing Input $thisName"
   exit 1
endif

set thisPID      = $$
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

if ($progressFlag != 0) then
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
set xBASEARG = (`getArgDD $argv -xBASEARG  POLY,auto   NULL`)

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

if ($istWinFlag != 0) then
   set yAPOD = (`getArgDD $argv -yAPOD  SP            SP`)
   set yQ1   = (`getArgDD $argv -yQ1    0.5           0.5`)
   set yQ2   = (`getArgDD $argv -yQ2    0.95          0.5`)
   set yQ3   = (`getArgDD $argv -yQ3    1             1`)
   set yELB  = (`getArgDD $argv -yELB   0            0.0`)
   set yGLB  = (`getArgDD $argv -yGLB   0            0.0`)
   set yGOFF = (`getArgDD $argv -yGOFF  0           0.0`)
else
   set yAPOD = SP
   set yQ1   = 0.5
   set yQ2   = 0.5
   set yQ3   = 1
   set yELB  = 0.0
   set yGLB  = 0.0
   set yGOFF = 0.0
endif

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

if ($istWinFlag != 0) then
   set zAPOD = (`getArgDD $argv -zAPOD  SP            SP`)
   set zQ1   = (`getArgDD $argv -zQ1    0.5           0.5`)
   set zQ2   = (`getArgDD $argv -zQ2    0.95          0.95`)
   set zQ3   = (`getArgDD $argv -zQ3    1             1`)
   set zELB  = (`getArgDD $argv -zELB  _zELB_  0.0`)
   set zGLB  = (`getArgDD $argv -zGLB  _zGLB_  0.0`)
   set zGOFF = (`getArgDD $argv -zGOFF _zGOFF_ 0.0`)
else
   set zAPOD = SP
   set zQ1   = 0.5
   set zQ2   = 0.5
   set zQ3   = 1
   set zELB  = 0.0
   set zGLB  = 0.0 
   set zOFF  = 0.0 
endif

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

set multDir  = ${istPrefix}mult
set specDir  = ${istPrefix}spec

set ft1Name   = ${istPrefix}.ft1
set istName   = ${istPrefix}.ft3
set istMask   = ${istPrefix}_mask.ft1
set sumName   = ${istPrefix}_sum.ft3
set resName   = ${istPrefix}_res.ft3
set invName   = ${istPrefix}_inv.ft1
set tmp1Name  = ${istPrefix}_tmp.ft1
set tmp2Name  = ${istPrefix}_tmp.ft3
set tmp3Name  = ${istPrefix}_tmp.ft3

if (-d $multDir) then
   /bin/rm -rf $multDir
endif

if (-d $specDir) then
   /bin/rm -rf $specDir 
endif

foreach f ($tmp1Name $tmp2Name $ft1Name $sumName $invName $istName)
   if (-e $f) then
      /bin/rm -f $f
   endif 
end

if ($istIter < 1) then
   exit 0
endif

mkdir $multDir
mkdir $specDir

echo ""
echo "Preparing Spectrum $tmp3Name ..."

xyz2pipe -in $inName \
| nmrPipe -fn `expandArg0 $xSOL` \
| nmrPipe -fn APOD -qName $xAPOD -q1 $xQ1 -q2 $xQ2 -q3 $xQ3 -elb $xELB -glb $xGLB -goff $xGOFF -c $xC1 \
| nmrPipe -fn ZF `expandArg $xZFARG` \
| nmrPipe -fn FT \
| nmrPipe -fn PS -p0 $xP0 -p1 $xP1 -di \
| nmrPipe -fn EXT -x1 $xEXTX1 -xn $xEXTXN -sw \
| nmrPipe -fn TP \
| nmrPipe -fn APOD -qName $yAPOD -q1 $yQ1 -q2 $yQ2 -q3 $yQ3 -elb $yELB -glb $yGLB -goff $yGOFF -c $yC1 \
| nmrPipe -fn ZF `expandArg $yZFARG` \
| nmrPipe -fn FT `expandArg $yFTARG` \
| nmrPipe -fn PS -p0 $yP0 -p1 $yP1 -di \
| nmrPipe -fn `expandArg0 $yBASEARG` \
| nmrPipe -fn TP \
| nmrPipe -fn `expandArg0 $xBASEARG` \
| nmrPipe -fn ZTP \
| nmrPipe -fn APOD -qName $zAPOD -q1 $zQ1 -q2 $zQ2 -q3 $zQ3 -elb $zELB -glb $zGLB -goff $zGOFF -c $zC1 \
| nmrPipe -fn ZF `expandArg $zZFARG` \
| nmrPipe -fn FT `expandArg $zFTARG` \
| nmrPipe -fn PS -p0 $zP0 -p1 $zP1 -di \
| nmrPipe -fn `expandArg0 $zBASEARG` \
| nmrPipe -fn TP \
   -out $tmp3Name -ov

if (`string csame $istMaxRes Auto`) then
   echo "Estimating IST Convergence Value ..."
   set istMaxRes = (`specStat.com -in $tmp3Name -brief -stat istMaxRes`)
endif

echo "Preparing Interferogram $ft1Name ..."

nmrPipe -in $tmp3Name \
| nmrPipe -fn HT -auto -nozf \
| nmrPipe -fn PS -inv -hdr \
| nmrPipe -fn FT -inv \
| nmrPipe -fn EXT -time -sw -ad \
| nmrPipe -fn TP -hyper \
| nmrPipe -fn HT -auto -nozf \
| nmrPipe -fn PS -inv -hdr \
| nmrPipe -fn FT -inv \
| nmrPipe -fn EXT -time -sw \
| nmrPipe -fn TP -hyper \
  -out $ft1Name -ov

echo "Preparing Interferogram Mask $maskName ..."

xyz2pipe -in $maskName \
| nmrPipe -fn ZF `expandArg $xZFARG` \
| nmrPipe -fn MAC -macro $NMRTXT/setFirst.M \
| nmrPipe -fn FT -null -di \
| nmrPipe -fn EXT -x1 $xEXTX1 -xn $xEXTXN -sw \
| nmrPipe -fn ZTP \
| nmrPipe -fn TP -hyper \
  -out $istMask -ov

set thisInName = $ft1Name

set i  = 1
set ii = 1

echo ""

if ($progressFlag != 0) then
   set thisMsg = "nmrProgress -loc 0 -n $istIter -pct 0.0 -fromID 1 -pid $thisPID -title IST Convergence (steps get smaller) -axis None"
   nmrSend -norecv -send $sendName -msg "$thisMsg"
endif

nmrPrintf "IST Convergence Threshold: %.2f%%\n\n" $istMaxRes

set convFlag = 0

set yZFInfo = (`expandArg $yZFARG`)
set zZFInfo = (`expandArg $zZFARG`)

while( $i <= $istIter )
   if ($istSaveFlag) then
      set ii = $i
   else
      set ii = 1
   endif

   addNMR -in1 $thisInName -in2 $istMask -out $tmp1Name -mul

   nmrPipe -in $tmp1Name \
   | nmrPipe -fn ZF $yZFInfo \
   | nmrPipe -fn FT  \
   | nmrPipe -fn PS -hdr -di \
   | nmrPipe -fn TP \
   | nmrPipe -fn ZF $zZFInfo \
   | nmrPipe -fn FT \
   | nmrPipe -fn PS -hdr -di \
   | nmrPipe -fn TP \
      -out $tmp2Name -ov

   set thresh = (`scale2D -brief $tmp2Name`)

   if ($i == 1) then
      set firstThresh = $thresh
   endif

   set thisThresh = (`MATH "100.0*div($thresh,$firstThresh)"`)

   nmrPrintf " %4d. Threshold: %7.2f%%\n" $i `MATH "100.0*div($thresh,$firstThresh)"`

   set thresh   = (`MATH "$thresh*$tMult"`)
   set multName = (`nmrPrintf $multDir/spec%03d.ft2 $ii`)
   set specName = (`nmrPrintf $specDir/spec%03d.ft2 $ii`)

   nmrPipe -in $tmp2Name -out $multName -ov -fn MULT -c $cMult -thresh  $thresh
   nmrPipe -in $tmp2Name -out $specName -ov -fn MULT -c $cMult -thresh -$thresh

   if ($i == 1) then
      nmrPipe -in $specName -out $sumName -fn SET -c 0.0 -ov
   endif

   addNMR -in1 $sumName -in2 $specName -out $tmp2Name -add
   /bin/mv $tmp2Name $sumName

   nmrPipe -in $multName \
   | nmrPipe -fn HT -auto -nozf \
   | nmrPipe -fn PS -inv -hdr \
   | nmrPipe -fn FT -inv \
   | nmrPipe -fn EXT -time -sw -ad \
   | nmrPipe -fn TP -hyper \
   | nmrPipe -fn HT -auto -nozf \
   | nmrPipe -fn PS -inv -hdr \
   | nmrPipe -fn FT -inv \
   | nmrPipe -fn EXT -time -sw \
   | nmrPipe -fn TP -hyper \
     -out $invName -ov

   if ($progressFlag != 0) then
      set progPct = (`MATH "100.0 - 100.0*div($thisThresh-$istMaxRes, 100.0-$istMaxRes)"`)
      set thisMsg = "nmrProgress -loc $i -pct `nmrPrintf %.3f $progPct` -n $istIter -fromID 1 -pid $thisPID -title IST Convergence (steps get smaller) -axis None"
      nmrSend -norecv -send $sendName -msg "$thisMsg"
   endif

   set thisInName = $invName 
   @ i++

  if (`IMATH "$thisThresh < $istMaxRes"`) then
      set convFlag = 1
      break
   endif
end

if ($progressFlag != 0) then
   if ($convFlag == 0) then
      set statusStr = "Iteration Count Exceeded."
   else
      set statusStr = "None"
   endif

   set thisMsg = "nmrProgress -loc $istIter -n $istIter -pct 100.0 -fromID 1 -pid $thisPID -title IST Convergence -axis None -status $statusStr"

   nmrSend -norecv -send $sendName -msg "$thisMsg"
endif

echo ""

nmrPipe -in $multName -out $resName -ov -fn POLY -auto -ord 0
addNMR  -in1 $sumName -in2 $resName -out $istName -add

nmrPipe -in $istName -fn TP | nmrPipe -fn ZTP | pipe2xyz -out $outName -ov

if ($graphFlag != 0) then
   vj_scroll_user.com -in $outName -hi $istHi &
   sleep 10
endif

