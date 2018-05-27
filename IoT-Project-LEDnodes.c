#include "contiki.h"

#include "net/rime.h"
#include "net/rime/mesh.h"

#include "dev/leds.h"

#include "node-id.h" // Include this library in order to be able to set node's ID.

#include <stdio.h>
#include <string.h>

#define CHANNEL 132
char *message_receive;
messge_receive="1";

static struct mesh_conn mesh;
/*---------------------------------------------------------------------------*/
PROCESS(node_led_process, "LED Node");
AUTOSTART_PROCESSES(&node_led_process);
/*---------------------------------------------------------------------------*/

static void
sent(struct mesh_conn *c)
{
  printf("packet sent\n");
}

static void
timedout(struct mesh_conn *c)
{
  printf("packet timedout\n");
}

static void
recv(struct mesh_conn *c, const rimeaddr_t *from, uint8_t hops)
{
  printf("Data received from %d.%d: %.*s (%d)\n",
	 from->u8[0], from->u8[1],
	 packetbuf_datalen(), (char *)packetbuf_dataptr(), packetbuf_datalen());
	 /*storing the message in the message_receive*/
  message_receive=(char *)packetbuf_dataptr();
}

const static struct mesh_callbacks callbacks = {recv, sent, timedout};

PROCESS_THREAD(node_led_process, ev, data)
{
  //unsigned short id = 1; // This is the ID which will be set in Controller

  PROCESS_EXITHANDLER(mesh_close(&mesh);)
  PROCESS_BEGIN();
  
  //node_id_burn(id); // Call this function to burn the defined id

  mesh_open(&mesh, CHANNEL, &callbacks); // Call this function to establish a new mesh connection

  while(1) {
    rimeaddr_t addr;

    /* Wait for button click before sending the first message. */
    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_MSG);

    printf("Message received\n");

    /* Send a message to node number 2 or 5 or 8. */
    int MSG = atoi(&message_receive);
    if(MSG==rimeaddr_node_addr.u8[0]){
	leds_blink();
    }
    if(MSG>rimeaddr_node_addr){
	packetbuf_copyfrom(message_receive, strlen(message_receive));
	addr.u8[0] =rime_node_addr.u8[0]+1 ;
    	addr.u8[1] = 0;
    	mesh_send(&mesh, &addr)
    }
    /*else{
    packetbuf_copyfrom(message_receive, strlen(message_receive));
    addr.u8[0] = 2;
    addr.u8[1] = 0;
    mesh_send(&mesh, &addr);
	}*/
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
