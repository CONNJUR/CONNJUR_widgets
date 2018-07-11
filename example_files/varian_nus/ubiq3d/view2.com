#!/bin/csh

nmrDraw -in n15noe_std.fid/ft/test%03d.ft3 -hi 5.0e+4 \
        -broadcast `getGeom -west`  -nophase -group 1 &

sleep 3

nmrDraw -in n15noe_nus.fid/ist/test%03d.ft3 -hi 4.0e+4 \
        -broadcast `getGeom -east`  -nophase -group 1 

