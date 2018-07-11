#!/bin/csh

set hi = 5.0e+4

nmrDraw -broadcast -in gcosy_std.fid/test.ft2 \
        -nophase -hi $hi `getGeom -left` &

sleep 3

nmrDraw -broadcast -in gcosy_std.fid/trunc.ft2 \
        -nophase -hi $hi `getGeom -mid` &
sleep 3

nmrDraw -broadcast -in gcosy_nus.fid/ist.ft2  \
        -nophase -hi $hi `getGeom -right` &

sleep 3
