#!/bin/csh

set hi = 2.0

nmrDraw -broadcast -in indanone_hsqc_std.fid/test.ft2 \
        -nophase -hi $hi `getGeom -left` &

sleep 3

nmrDraw -broadcast -in indanone_hsqc_std.fid/trunc.ft2 \
        -nophase -hi $hi `getGeom -mid` &
sleep 3

nmrDraw -broadcast -in indanone_hsqc_nus.fid/ist.ft2  \
        -nophase -hi $hi `getGeom -right` &

sleep 3
