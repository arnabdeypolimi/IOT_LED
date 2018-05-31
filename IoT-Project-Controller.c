#include "contiki.h"

#include "net/rime.h"
#include "net/rime/mesh.h"

#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "lib/list.h"
#include "lib/memb.h"

#include <stdio.h>
#include <string.h>

#define ARRAY_SIZE( array ) sizeof( array ) / sizeof( char )
#define CHANNEL 132

/*---------------------------------------------------------------------------*/
PROCESS(controller_process, "Controller");
AUTOSTART_PROCESSES(&controller_process);
/*---------------------------------------------------------------------------*/

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

static void
recv(struct unicast_conn *c, const rimeaddr_t *from, uint8_t hops)
{
  printf("Data received from %d.%d: %.*s (%d)\n",
	 from->u8[0], from->u8[1],
	 packetbuf_datalen(), (char *)packetbuf_dataptr(), packetbuf_datalen());
}

static const struct unicast_callbacks unicast_callbacks = {recv, sent, timedout};
static struct unicast_conn uc; 

PROCESS_THREAD(controller_process, ev, data)
{
  PROCESS_EXITHANDLER(unicast_close(&uc);)
    
  PROCESS_BEGIN();

  unicast_open(&uc, CHANNEL, &unicast_callbacks);

  /* Activate the button sensor. We use the button to drive traffic -
     when the button is pressed, a packet is sent. */
  SENSORS_ACTIVATE(button_sensor);
  
  static int i;
  static char * message;
  static int counter = 0;
  

  while(1) {
    rimeaddr_t addr;
    int pattern[] = {2, 4, 6, 8, 10};
    /* Wait for button click before sending the first message. */
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);
    counter += 1;
    printf("Button clicked %d times.\n", counter);

    for(i = 0; i < 5; i++){	/* Send a message to node number 2 or 5 or 8. */
	message = pattern[i];
	packetbuf_copyfrom(message, strlen(message));
	printf("MESSAGE is: %d \n", pattern[i]);
	if(*message < 5){
		addr.u8[0] = 2;
        	addr.u8[1] = 0;
	}
	else if(*message < 8){
		addr.u8[0] = 5;
        	addr.u8[1] = 0;
	}
	else{
	 	addr.u8[0] = 8;
        	addr.u8[1] = 0;
	}
	unicast_send(&uc, &addr);
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

