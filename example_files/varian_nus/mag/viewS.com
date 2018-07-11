#!/bin/csh


specView.tcl $* -hi 5.0e+4 -ref None \
 -in gcosy_std.fid/test.ft2  \
     gcosy_std.fid/trunc.ft2 \
     gcosy_nus.fid/test.ft2  \
     gcosy_nus.fid/ist.ft2

