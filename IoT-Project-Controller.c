#include "contiki.h"

#include "net/rime.h"
#include "net/rime/mesh.h"

#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "lib/list.h"
#include "lib/memb.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

static const struct unicast_callbacks callbacks = {recv, sent, timedout};
static struct unicast_conn uc; 

PROCESS_THREAD(controller_process, ev, data)
{
  PROCESS_EXITHANDLER(unicast_close(&uc));
  static struct etimer et;  
  PROCESS_BEGIN();

  unicast_open(&uc, CHANNEL, &callbacks);

  /* Activate the button sensor. We use the button to drive traffic -
     when the button is pressed, a packet is sent. */
  SENSORS_ACTIVATE(button_sensor);
  
  static int i = 0;
  static int counter = 0;
  static char *message;
  static char *pattern[] = {"4", "10", "7", "3", "6", "9", "2", "5", "8"};
  static char *pattern1[] = {"4", "10", "6", "2", "8"};
  static char *pattern2[] = {"2", "6", "8", "4", "10"};

  while(1) {
    rimeaddr_t addr;
    /* Wait for button click before sending the first message. */
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);
    counter += 1;
    printf("Button clicked %d times.\n", counter);

    if(counter%2 == 0){
	printf("Switching OFF the LEDS...\n");
	for(i = 0; i < 9; i++){	/* Switch off the leds */
		message = pattern[i];
		packetbuf_copyfrom(message, strlen(message));
		printf("MESSAGE is: %s \n", message);
		if(atoi(message) < 5){
				addr.u8[0] = 2;
				addr.u8[1] = 0;
		}
		else if(atoi(message) < 8 && atoi(message) >= 5){
				addr.u8[0] = 5;
				addr.u8[1] = 0;
		}
		else if(atoi(message) < 11 && atoi(message) >= 8){
			 	addr.u8[0] = 8;
				addr.u8[1] = 0;
		}
		unicast_send(&uc, &addr);
		etimer_set(&et, CLOCK_SECOND *10);
    		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
		etimer_reset(&et);
	}
    }
    
    int st=counter;
	if(st==1){ 
		for(i = 0; i < 5; i++){	/* Send a message to node number 2 or 5 or 8. */
			message = pattern1[i];
			packetbuf_copyfrom(message, strlen(message));
			printf("MESSAGE is: %s \n", message);
			if(atoi(message) < 5){
				addr.u8[0] = 2;
				addr.u8[1] = 0;
			}
			else if(atoi(message) < 8 && atoi(message) >= 5){
				addr.u8[0] = 5;
				addr.u8[1] = 0;
			}
			else if(atoi(message) < 11 && atoi(message) >= 8){
			 	addr.u8[0] = 8;
				addr.u8[1] = 0;
			}
			unicast_send(&uc, &addr);
			etimer_set(&et, CLOCK_SECOND *10);
    			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
			etimer_reset(&et);
    		}
		//break;

    }
    
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
