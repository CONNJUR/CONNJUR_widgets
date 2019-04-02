# CONNJUR_widgets

## CONNJUR Widgets

The purpose of this repository is to create/document widgets for metadata management issues associated with the CONNJUR and NMRbox projects.  

The first widgets released here are a series of graphical widgets based on the GTK+ library used in the LINUX operating systems used by NMRbox.  This code base is written primarily in C and includes both graphical widgets as well as base code for parsing the various NMR file formats, CONNJUR_ML, and RDF.   

Better documentation can be found on the [wiki](https://github.com/CONNJUR/CONNJUR_widgets/wiki).

## Projects and Status

| Project  | State | Description |
| ------------- | ------------- | ------------- |
| cjrShowProcpar  | Stable and Complete | Shows procpar file as GTK Treestore.  Only improvements could be glade |
| cjrShowSparky  | Stable and Incomplete | Parses sparky header.  Still need common GUI for sparky, pipe, view, etc. |
| cjrShowNMRPipe  | Stable and Incomplete | Parses nmrPipe header.  Above, plus need to Git Commit :) |
| cjrShowNMRView  | Stable and Incomplete | Parses nmrView header.  Above, plus need to add support for Big Endian |
| cjrShowVarian  | Functional | Parses Varian .fid folder.  Works but needs code cleanup.  Seqfil's also need to be XML |
| xmlParser | Work in Progress | |
| cjrShow | Stable | First pass work with GTK.  Abandoned |
| cjrShowFull | Stable | First pass work with GTK.  Abandoned |
