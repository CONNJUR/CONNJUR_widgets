#!/bin/csh

specView.tcl $* -hi 2.0 -ref None \
 -in indanone_hsqc_std.fid/test.ft2  \
     indanone_hsqc_nus.fid/ist.ft2   \
     indanone_hsqc_nus.fid/smile.ft2 \
     indanone_hsqc_std.fid/trunc.ft2 \
     indanone_hsqc_nus.fid/test.ft2

