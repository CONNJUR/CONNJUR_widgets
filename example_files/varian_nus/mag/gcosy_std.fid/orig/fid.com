#!/bin/csh

var2pipe -in ./fid \
 -noaswap  \
  -xN              1150  -yN               512  \
  -xT               575  -yT               512  \
  -xMODE        Complex  -yMODE           Real  \
  -xSW         3834.356  -ySW         3834.356  \
  -xOBS         399.758  -yOBS         399.758  \
  -xCAR           4.773  -yCAR           4.773  \
  -xLAB             H1x  -yLAB             H1y  \
  -ndim               2  -aq2D       Magnitude  \
  -out ./test.fid -verb -ov

