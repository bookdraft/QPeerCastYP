dmd -v1 pcraw_proxy http tools pcraw/all pcraw/arcfour pcraw/exception pcraw/key pcraw/pcp pcraw/pcptools pcraw/pcraw pcraw/pcrp pcraw/socket pcraw/common pcraw/pcrawtools ws2_32.lib -ofpcraw_proxy.exe -release -O
del *.obj *.map
