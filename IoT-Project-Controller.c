#include "contiki.h"

#include "net/rime.h"
#include "net/rime/mesh.h"

#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "lib/list.h"
#include "lib/memb.h"

#include "node-id.h" // Include this library in order to be able to set node's ID.

#include <stdio.h>
#include <string.h>


#define CHANNEL 132
#define MESSAGE "LED ON"
#define MAX_NEIGHBORS 3

static struct mesh_conn mesh;

/* This structure holds information about neighbors. */
struct neighbor {
  /* The ->next pointer is needed since we are placing these on a
     Contiki list. */
  struct neighbor *next;

  /* The ->addr field holds the Rime address of the neighbor. */
  rimeaddr_t addr;
};

MEMB(neighbors_memb, struct neighbor, MAX_NEIGHBORS);
LIST(neighbors_list);


/*---------------------------------------------------------------------------*/
PROCESS(controller_process, "Controller");
AUTOSTART_PROCESSES(&controller_process);
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
}

const static struct mesh_callbacks callbacks = {recv, sent, timedout};

PROCESS_THREAD(controller_process, ev, data)
{
  unsigned short id = 1; // This is the ID which will be set in Controller

  PROCESS_EXITHANDLER(mesh_close(&mesh);)
  PROCESS_BEGIN();
  
  node_id_burn(id); // Call this function to burn the defined id

  mesh_open(&mesh, CHANNEL, &callbacks); // Call this function to establish a new mesh connection

  /* Activate the button sensor. We use the button to drive traffic -
     when the button is pressed, a packet is sent. */
  SENSORS_ACTIVATE(button_sensor);

  while(1) {
    rimeaddr_t addr;

    /* Wait for button click before sending the first message. */
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

    printf("Button clicked\n");

    /* Send a message to node number 2 or 5 or 8. */
    
    packetbuf_copyfrom(MESSAGE, strlen(MESSAGE));
    addr.u8[0] = 2;
    addr.u8[1] = 0;
    mesh_send(&mesh, &addr);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

