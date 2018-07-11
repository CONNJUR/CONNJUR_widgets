#!/bin/csh

ext_xz.com data/spec%03d.fid spec_xz.fid 1

nmrPipe -in spec_xz.fid \
| nmrPipe -fn SOL \
| nmrPipe -fn APOD -qName SP -q1 0.5 -q2 0.95 -q3 2 -elb 0 -glb 0 -goff 0 -c 0.5 \
| nmrPipe -fn ZF -zf 1 -auto \
| nmrPipe -fn FT -verb \
| nmrPipe -fn PS -p0 -201.10 -p1 0.00 -di \
| nmrPipe -fn EXT -x1 9.780ppm -xn 5.981ppm -sw \
| nmrPipe -fn TP \
| nmrPipe -fn APOD -qName SP -q1 0.5 -q2 0.95 -q3 1 -elb 0.0 -glb 0.0 -goff 0.0 -c 0.5 \
| nmrPipe -fn ZF -zf 1 -auto \
| nmrPipe -fn FT  -verb \
| nmrPipe -fn PS -p0 0.00 -p1 0.00 -di \
| nmrPipe -fn NULL \
| nmrPipe -fn TP \
| nmrPipe -fn POLY -auto -verb \
  -out spec_xz.ft2 -ov

# set hiInfo = (`getSpecHi.tcl -in spec_xz.ft2  -hi 10%`)
# nmrDraw -in spec_xz.ft2 $hiInfo `getGeom -east -xAdj 100 -yAdj 200` &
# sleep 3
