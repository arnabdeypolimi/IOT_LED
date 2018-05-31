#include "contiki.h"

#include "net/rime.h"
#include "net/rime/mesh.h"

#include "dev/leds.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHANNEL 132

/*---------------------------------------------------------------------------*/
PROCESS(node_led_process, "LED Node");
AUTOSTART_PROCESSES(&node_led_process);
/*---------------------------------------------------------------------------*/

char * message_receive;

static void
sent(struct unicast_conn *c)
{
  printf("packet sent\n");
}

static void
timedout(struct unicast_conn *c)
{
  printf("packet timedout\n");
}

static struct unicast_conn uc; 

static void
recv(struct unicast_conn *c, const rimeaddr_t *from, uint8_t hops)
{ int MSG;

  printf("Data received from %d.%d: %.*s (%d)\n",
	 from->u8[0], from->u8[1],
	 packetbuf_datalen(), (char *)packetbuf_dataptr(), packetbuf_datalen());
  /*storing the message in the message_receive*/
  message_receive = (char *)packetbuf_dataptr();
  MSG = atoi(message_receive);
  rimeaddr_t addr;

  if(MSG == rimeaddr_node_addr.u8[0]){
	leds_on(LEDS_RED);
	printf("LED Node #%d ON!\n", MSG);
  }
  else if(MSG > rimeaddr_node_addr.u8[0]){
	packetbuf_copyfrom(message_receive, strlen(message_receive));
	addr.u8[0] = rimeaddr_node_addr.u8[0] + 1;
    	addr.u8[1] = 0;
   	if(addr.u8[0] != from->u8[0]){
	   unicast_send(&uc, &addr);
   	}
  }
  else{
      	packetbuf_copyfrom(message_receive, strlen(message_receive));
	addr.u8[0] = rimeaddr_node_addr.u8[0] - 1;
	addr.u8[1] = 0;
	if(addr.u8[0] != from->u8[0]){
	   unicast_send(&uc, &addr);
	}
  }
}

static const struct unicast_callbacks unicast_callbacks = {recv, sent, timedout};

PROCESS_THREAD(node_led_process, ev, data)
{
  PROCESS_EXITHANDLER(unicast_close(&uc);)
  PROCESS_BEGIN();

  unicast_open(&uc, CHANNEL, &unicast_callbacks); /* Call this function to establish a 
						 new unicastconnection*/
  
  leds_init();

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
