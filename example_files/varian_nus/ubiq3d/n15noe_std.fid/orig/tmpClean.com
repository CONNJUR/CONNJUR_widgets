#!/bin/csh

foreach d (data fid mask istmult istspec nusmask)
   if (-d $d) then
      /bin/rm -rf $d
   endif
end

touch      tmp.dat tmp.fid tmp.ftx ser_full fid_full
/bin/rm -f *.dat   *.fid   *.ft*   ser_full fid_full


