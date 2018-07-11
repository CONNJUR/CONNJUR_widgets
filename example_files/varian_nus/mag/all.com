#!/bin/csh

clean.com

cd gcosy_std.fid
fid.com
proc.com
tmpClean.com
cd ../

cd gcosy_nus.fid
fid.com
proc.com 
tmpClean.com
cd ../

view3.com
viewS.com
