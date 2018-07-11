#!/bin/csh

specView.tcl $* -hi 2.0 -ref None \
 -in test.ft2  \
     lp.ft2    \
     ist.ft2   \
     smile.ft2 \
     trunc.ft2 \

