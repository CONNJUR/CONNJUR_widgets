#!/bin/csh

set hi = 2.0

nmrDraw -broadcast -in test.ft2 \
        -nophase -hi $hi `getGeom -left` &

sleep 3

nmrDraw -broadcast -in lp.ft2 \
        -nophase -hi $hi `getGeom -mid` &
sleep 3

nmrDraw -broadcast -in ist.ft2  \
        -nophase -hi $hi `getGeom -right` &

sleep 3
