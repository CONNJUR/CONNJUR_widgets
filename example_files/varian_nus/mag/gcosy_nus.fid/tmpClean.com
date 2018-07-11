#!/bin/csh

foreach d (data fid mask istmult istspec nusmask)
   if (-d $d) then
      /bin/rm -rf $d
   endif
end

touch      tmp.dat tmp.fid ser_full fid_full pk.tcl
/bin/rm -f *.dat   *.fid   ser_full fid_full pk.tcl


