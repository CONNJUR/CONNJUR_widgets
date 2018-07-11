#!/bin/csh

var2pipe -in ./fid \
 -noaswap  \
  -xN              1924  -yN               256  \
  -xT               962  -yT               128  \
  -xMODE        Complex  -yMODE      Rance-Kay  \
  -xSW         6410.256  -ySW        20105.554  \
  -xOBS         399.758  -yOBS         100.528  \
  -xCAR           4.773  -yCAR          91.429  \
  -xLAB              H1  -yLAB             C13  \
  -ndim               2  -aq2D          States  \
  -out ./test.fid -verb -ov

