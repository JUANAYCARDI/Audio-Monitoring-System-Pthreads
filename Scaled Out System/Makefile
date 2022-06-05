#
# Makefile for make
#
# Compiler: gcc
#

PROGS   = AMS
CFLAGST = -D_REENTRANT -Wall
LIBS    =	pthread

AMS : phtrdsMsgLyr.o AudioMonitoringSystem.o
	$(CC) $(CFLAGST) -o AMS AudioMonitoringSystem.o phtrdsMsgLyr.o -l $(LIBS)

phtrdsMsgLyr.o : phtrdsMsgLyr.c phtrdsMsgLyr.h pMLusrConf.h
	$(CC) $(CFLAGST) -g -c phtrdsMsgLyr.c

AudioMonitoringSystem.o : AudioMonitoringSystem.c pMLusrConf.h
	$(CC) $(CFLAGST) -g -c AudioMonitoringSystem.c

clean:
	rm -f $(PROGS) *~ *.o
