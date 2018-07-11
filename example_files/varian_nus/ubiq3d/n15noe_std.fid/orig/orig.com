#!/bin/csh

var2pipe -in ./fid \
 -noaswap -aqORD 1 \
  -xN              2404  -yN               140  -zN                52  \
  -xT              1202  -yT                70  -zT                26  \
  -xMODE        Complex  -yMODE        Complex  -zMODE      Rance-Kay  \
  -xSW        12019.231  -ySW         7000.000  -zSW         1320.000  \
  -xOBS         599.422  -yOBS         599.422  -zOBS          60.746  \
  -xCAR           4.770  -yCAR           4.770  -zCAR         118.178  \
  -xLAB              HN  -yLAB              1H  -zLAB             N15  \
  -ndim               3  -aq2D          States                         \
  -out ./data/test%03d.fid -verb -ov

