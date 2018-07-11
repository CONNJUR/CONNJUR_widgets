#!/bin/csh

clean.com

cd n15noe_std.fid
clean.com
fid.com
proc.com
xyz.lp.com
xyz.ist.com
tmpClean.com
cd ../

cd n15noe_nus.fid
clean.com
fid.com
proc.com
tmpClean.com
cd ../

scroll.com
view2.com
