#!/bin/csh

if (!(-e fid)) then
   exit 0
endif

var2pipe -in fid -noaswap -ndim 2 -nusDim 2 -verb \
 -title c2hsqcse -aq2D States -aqORD 1 \
 -xN              1924 -yN                64 \
 -xT               962 -yT               125 \
 -xMODE        Complex -yMODE      Rance-Kay \
 -xSW        6410.2564 -ySW       20105.5542 \
 -xOBS        399.7563 -yOBS        100.5181 \
 -xCAR          4.7729 -yCAR         96.2018 \
 -xLAB              H1 -yLAB             C13 \
 -out spec_nus.fid -ov

nusExpand.tcl  -in spec_nus.fid -out spec.fid      -sample sampling.sch -sampleCount Auto
nusExpand.tcl  -in spec_nus.fid -out spec_mask.fid -sample sampling.sch -sampleCount Auto -mask
