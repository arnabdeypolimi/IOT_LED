CONTIKI = ../..

all: example-abc example-mesh example-collect example-trickle example-polite \
     example-rudolph0 example-rudolph1 example-rudolph2 example-rucb \
     example-runicast example-unicast example-neighbors

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"
include $(CONTIKI)/Makefile.include
