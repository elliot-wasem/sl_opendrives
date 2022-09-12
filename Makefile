#==========================================
#    Makefile: makefile for sl 5.1
#	Copyright 1993, 1998, 2014
#                 Toyoda Masashi
#		  (mtoyoda@acm.org)
#	Last Modified: 2014/03/31
#==========================================

CC=gcc
CFLAGS=-O -Wall
INSTALLDIR=$(DESTDIR)/usr/bin
MANDIR=$(DESTDIR)/usr/share/man/man1

all: sl

sl: sl.c sl.h
	$(CC) $(CFLAGS) -o sl sl.c -lncurses

install: sl
	cp sl $(INSTALLDIR)
	mkdir -p $(MANDIR)
	gzip -c sl.man > $(MANDIR)/sl.1.gz

uninstall: sl
	rm $(INSTALLDIR)/sl
	rm $(MANDIR)/sl.1.gz

deb: sl
	debuild -us -uc -b

clean:
	rm -f sl

distclean: clean
