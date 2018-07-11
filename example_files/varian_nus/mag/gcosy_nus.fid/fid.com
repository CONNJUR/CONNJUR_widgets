#!/bin/csh

var2pipe -in ./fid \
 -ndim 2 -nusDim 2 -noaswap  \
  -xN              1150  -yN               256  \
  -xT               575  -yT               511  \
  -xMODE        Complex  -yMODE           Real  \
  -xSW         3834.356  -ySW         3834.356  \
  -xOBS         399.758  -yOBS         399.758  \
  -xCAR           4.773  -yCAR           4.773  \
  -xLAB             H1x  -yLAB             H1y  \
  -aq2D       Magnitude  \
  -out ./nus.fid -verb -ov

nusExpand.tcl  -in nus.fid -out test.fid -sample sampling.sch -sampleCount Auto
nusExpand.tcl  -in nus.fid -out mask.fid -sample sampling.sch -sampleCount Auto -mask

