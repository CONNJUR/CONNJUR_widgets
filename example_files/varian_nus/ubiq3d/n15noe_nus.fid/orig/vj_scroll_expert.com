#!/bin/csh

set hiInfo = (`getSpecHi.tcl -in ft/spec%03d.ft3 -hi 3.0%`)

proj3D.tcl -in ft/spec%03d.ft3 -abs
peakHN.tcl -in ft/spec%03d.ft3 -out proj.tab -proj -hi Full

vj_scroll.tcl -tab proj.tab -pair -lab INDEX \
              -noyGrid -colorB dimGray -colorW black -fg black -shade \
              -in ft/spec%03d.ft3 $hiInfo &
