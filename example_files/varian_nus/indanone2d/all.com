#!/bin/csh

clean.com

cd indanone_hsqc_std.fid
fid.com
proc.com
tmpClean.com
cd ../

cd indanone_hsqc_nus.fid
fid.com
proc.com
tmpClean.com
cd ../

viewS.com
