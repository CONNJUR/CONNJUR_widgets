#!/bin/csh

xyz2pipe -in data/spec%03d.fid -x -verb \
| nmrPipe -fn SOL \
| nmrPipe -fn APOD -qName SP -q1 0.5 -q2 0.95 -q3 2 -elb 0 -glb 0 -goff 0 -c 0.5 \
| nmrPipe -fn ZF -zf 1 -auto \
| nmrPipe -fn FT \
| nmrPipe -fn PS -p0 -201.10 -p1 0.00 -di \
| nmrPipe -fn EXT -x1 9.780ppm -xn 5.981ppm -sw \
| nmrPipe -fn TP \
| nmrPipe -fn APOD -qName SP -q1 0.5 -q2 0.95 -q3 1 -elb 0 -glb 0 -goff 0 -c 0.5 \
| nmrPipe -fn ZF -zf 1 -auto \
| nmrPipe -fn FT  \
| nmrPipe -fn PS -p0 45.00 -p1 0.00 -di \
| nmrPipe -fn NULL \
| nmrPipe -fn TP \
| nmrPipe -fn POLY -auto \
| pipe2xyz -out ft/spec%03d.ft2 -x -ov

xyz2pipe -in ft/spec%03d.ft2 -z -verb \
| nmrPipe -fn APOD -qName SP -q1 0.5 -q2 0.95 -q3 1 -elb 0.0 -glb 0.0 -goff 0.0 -c 0.5 \
| nmrPipe -fn ZF -zf 1 -auto \
| nmrPipe -fn FT  \
| nmrPipe -fn PS -p0 0.00 -p1 0.00 -di \
| nmrPipe -fn NULL \
| pipe2xyz -out ft/spec%03d.ft3 -z

# proj3D.tcl -in ft/spec%03d.ft3 -axis -sum
# nmrCube.tcl -in ft/spec%03d.ft3 -xOff 300 -yOff 200 -hi 10% &
# sleep 5
