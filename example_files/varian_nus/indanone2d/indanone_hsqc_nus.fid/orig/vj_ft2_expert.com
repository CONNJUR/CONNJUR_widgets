#!/bin/csh

nmrPipe -in spec.fid \
| nmrPipe -fn NULL \
| nmrPipe -fn APOD -qName SP -q1 0.5 -q2 0.95 -q3 2 -elb 0.0 -glb 0.0 -goff 0.0 -c 0.5 \
| nmrPipe -fn ZF -zf 1 -auto \
| nmrPipe -fn FT -verb \
| nmrPipe -fn PS -p0 25.42 -p1 0.00 -di \
| nmrPipe -fn EXT -x1 0% -xn 100% -sw \
| nmrPipe -fn TP \
| nmrPipe -fn APOD -qName SP -q1 0.5 -q2 0.95 -q3 1 -elb 0.0 -glb 0.0 -goff 0.0 -c 0.5 \
| nmrPipe -fn ZF -zf 1 -auto \
| nmrPipe -fn FT  -verb \
| nmrPipe -fn PS -p0 0.00 -p1 0.00 -di \
| nmrPipe -fn NULL \
| nmrPipe -fn TP \
| nmrPipe -fn NULL -verb \
  -out spec.ft2 -ov

# proj2D.tcl -in spec.ft2 -axis -sum
#
# set hiInfo = (`getSpecHi.tcl -in spec.ft2  -hi 2.5%`)
# nmrDraw -in spec.ft2 $hiInfo `getGeom -west -xAdj 100 -yAdj 200` &
# sleep 3
