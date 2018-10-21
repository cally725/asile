LDLIBS=-lm -lusb-1.0 /usr/local/lib/libwiringPi.so
CFLAGS=-Os -Wall
DOCCO=docco

asile: asile.c usbscale.c scales.h
	$(CC) $(CFLAGS) asile.c usbscale.c $(LDLIBS) -o $@

lsusb: lsusb.c scales.h

docs: usbscale.c asile.c
	$(DOCCO) usbscale.c asile.c 

clean:
	rm -f lsscale
	rm -f usbscale
