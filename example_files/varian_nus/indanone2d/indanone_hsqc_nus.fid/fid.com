#!/bin/csh

nusExpand.tcl -mode varian -sampleCount 32 -off 0 \
 -in ./fid -out ./fid_full -sample ./sampling.sch

var2pipe -in ./fid_full \
 -noaswap  \
  -xN              1924  -yN               250  \
  -xT               962  -yT               125  \
  -xMODE        Complex  -yMODE      Rance-Kay  \
  -xSW         6410.256  -ySW        20105.554  \
  -xOBS         399.758  -yOBS         100.528  \
  -xCAR           4.773  -yCAR          91.429  \
  -xLAB              H1  -yLAB             C13  \
  -ndim               2  -aq2D          States  \
  -out ./test.fid -verb -ov

nusExpand.tcl -mask -noexpand -mode pipe -sampleCount 32 -off 0 \
 -in ./test.fid -out ./mask.fid -sample ./sampling.sch

