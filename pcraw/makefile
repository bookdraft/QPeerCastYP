SRV_SRC = pcraw_srv.d http.d tools.d
PROXY_SRC = pcraw_proxy.d http.d tools.d 
PCRAW_SRC = pcraw/all.d pcraw/arcfour.d pcraw/common.d pcraw/exception.d \
            pcraw/key.d pcraw/pcp.d pcraw/pcptools.d pcraw/pcraw.d \
            pcraw/pcrawtools.d pcraw/pcrp.d pcraw/socket.d

SRV_OBJ = $(SRV_SRC:.d=.o)
PROXY_OBJ = $(PROXY_SRC:.d=.o)
PCRAW_OBJ = $(PCRAW_SRC:.d=.o)

DD = gdc

all: pcraw_srv pcraw_proxy

pcraw_srv: $(PCRAW_OBJ) $(SRV_OBJ)
	$(DD) -o $@ $(PCRAW_OBJ) $(SRV_OBJ)

pcraw_proxy: $(PCRAW_OBJ) $(PROXY_OBJ)
	$(DD) -o $@ $(PCRAW_OBJ) $(PROXY_OBJ)

%.o: %.d
	$(DD) -c $< -o $@

clean:
	rm -f $(PCRAW_OBJ) $(SRV_OBJ) $(PROXY_OBJ)
