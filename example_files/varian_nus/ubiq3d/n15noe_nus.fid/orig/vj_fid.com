#!/bin/csh

if (!(-e fid)) then
   exit 0
endif

var2pipe -in fid -noaswap -ndim 2 -nusDim 3 -aqORD 0 -verb \
 -title N15-NOE -aq2D Complex \
 -xN              2404 -yN              1820 -zN                 1 \
 -xT              1202 -yT                70 -zT                26 \
 -xMODE        Complex -yMODE        Complex -zMODE           Real \
 -xSW       12019.2308 -ySW        7000.0000 -zSW        1320.0000 \
 -xOBS        599.4225 -yOBS        599.4225 -zOBS         60.7459 \
 -xCAR          4.6300 -yCAR          4.6300 -zCAR        118.5700 \
 -xLAB              HN -yLAB              H1 -zLAB             N15 \
| nmrPipe -fn MAC -macro $NMRTXT/var_ranceN.M -noRd -noWr -var nShuf 1 nusDim 3 \
 -out spec_nus.fid -ov

nusExpand.tcl  -in spec_nus.fid -out data/spec%03d.fid    -sample sampling.sch -sampleCount Auto -aqORD 1 -sign
nusExpand.tcl  -in spec_nus.fid -out nusmask/spec%03d.fid -sample sampling.sch -sampleCount Auto -aqORD 1 -mask
