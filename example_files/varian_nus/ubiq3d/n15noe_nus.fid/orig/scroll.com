#!/bin/csh

proj3D.tcl -in ist/test%03d.ft3
peakHN.tcl -in ist/test%03d.ft3 -out hn.proj.tab -proj -hi Full

scroll.tcl $* \
 -noyGrid -colorB dimGray -colorW black -fg black \
 -in ist/test%03d.ft3 -tab hn.proj.tab -yw 1.8ppm -pair -hi 3%  
