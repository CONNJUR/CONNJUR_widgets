#!/bin/csh

clean.com
fid.com
proc.com

specView.tcl -hi 3.0 -in ist.ft2 smile.ft2 test.ft2
