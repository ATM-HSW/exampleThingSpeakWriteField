/*
  WriteSingleField
  
  Description: Writes a value to a channel on ThingSpeak every 20 seconds.
  
  Hardware:  tested with Mbed platform NUCLEO_F767ZI

  !!! IMPORTANT - Modify the secrets.h file for this project with your ThingSpeak channel details. !!!

  Note:
  - Requires the Ethernet library

  ThingSpeak ( https://www.thingspeak.com ) is an analytic IoT platform service that allows you to aggregate, visualize, and
  analyze live data streams in the cloud. Visit https://www.thingspeak.com to sign up for a free account and create a channel.

  Documentation for the ThingSpeak Communication Library for Arduino is in the README.md folder where the library was installed.
  See https://www.mathworks.com/help/thingspeak/index.html for the full ThingSpeak documentation.

  For licensing information, see the accompanying license file.

  Copyright 2018, The MathWorks, Inc.
*/

#include "mbed.h"
#include "EthernetInterface.h"


#include "secrets.h"
//#define THINGSPEAK_DBG_MSG
//#define THINGSPEAK_DBG_HTTP
#include "ThingSpeak.h"

Serial pc(USBTX, USBRX);
NetworkInterface *net;
ThingSpeak thingSpeak;
TCPSocket socket;

// Counting channel details
unsigned long counterChannelNumber = SECRET_CH_ID_COUNTER;
const char * myCounterReadAPIKey = SECRET_READ_APIKEY_COUNTER;
unsigned int counterFieldNumber = 1;
nsapi_size_or_error_t result;

int setup() {
#ifdef MBED_MAJOR_VERSION
  pc.printf("Mbed OS version: %d.%d.%d\n\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);
#endif

  net = new EthernetInterface();
  if (!net) {
      pc.printf("Error! No network inteface found.\n");
      return -1;
  }

  result = net->connect();
  if (result != 0) {
      pc.printf("Error! net->connect() returned: %d\n", result);
      return result;
  }

  // Show the network address
  const char *ip = net->get_ip_address();
  const char *netmask = net->get_netmask();
  const char *gateway = net->get_gateway();
  pc.printf("IP address: %s\n", ip ? ip : "None");
  pc.printf("Netmask: %s\n", netmask ? netmask : "None");
  pc.printf("Gateway: %s\n", gateway ? gateway : "None");

  nsapi_error_t open_result = socket.open(net);
  if (open_result != 0) {
      pc.printf("Error! socket.open(net) returned: %d\n", open_result);
      return open_result;
  }

  // thingSpeak.setSerial(&pc);
  thingSpeak.begin(net);  // Initialize ThingSpeak

  return 0;
}

#define SECRET_CH_ID 719728
#define SECRET_WRITE_APIKEY "U48IWT4797KKLW29"

void loop() {
  static uint8_t number=0;

  // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
  // pieces of information in a channel.  Here, we write to field 1.
  int x = thingSpeak.writeField(SECRET_CH_ID, 1, number, SECRET_WRITE_APIKEY);
  if(x == 200){
    pc.printf("Channel update successful.\n");
  }
  else{
    pc.printf("Problem updating channel. HTTP error code %d\n",x);
  }

  // change the value
  number++;
  if(number > 99){
    number = 0;
  }

  thread_sleep_for(20000); // Wait 20 seconds to update the channel again
}

int main() {
  if(setup() == 0) 
    while(true) loop();
  else
    while(true) thread_sleep_for(1000);
}
