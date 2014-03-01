//Sending ir codes over ethernet
//This sketch accept IR from an IR sensor connected to Pin 6 and sends over IP
// parts of the code for storing ir in to buffer is from  Ken Shirriff's ir recorder http://arcfn.com
//
//                       THIS ONE PICKS IR FROM A REMOTE
//
#include <IRremote.h>
#include <SPI.h>
#include <Ethernet.h>
 

int RECV_PIN = 6;
IRrecv irrecv(RECV_PIN);
decode_results results;
 
// Network settings 
byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; //MAC
byte gateway[] = { 192,168,0,1 };// Gateway
byte ip[] = { 192,168,0,123 };  //ip if dhcp fails 
 
// IP of the Target arduino with ir emitter
IPAddress server(192,168,0,102);  
EthernetClient client;

unsigned int rawCodes[RAWBUF]; // The durations if raw
int codeLen; // The length of the code
//Get the infrared codes from remote

void storeCode(decode_results *results) {
  int count = results->rawlen;
  Serial.println("Received code");
  codeLen = results->rawlen - 1;
    for (int i = 1; i <= codeLen; i++) {
      if (i % 2) {
        // Mark
        rawCodes[i - 1] = results->rawbuf[i]*USECPERTICK - MARK_EXCESS;
        Serial.print(" m");
      } 
      else {
        // Space
        rawCodes[i - 1] = results->rawbuf[i]*USECPERTICK + MARK_EXCESS;
        Serial.print(" s");
      }
      Serial.print(rawCodes[i - 1], DEC);
    }
    Serial.println("");
}

 
void setup()
{
  // For debug
  Serial.begin(9600);
  // Start ethernet
  Ethernet.begin(mac, ip, gateway);
  Serial.println("Initialised");
}

//sending the raw IR code over the ether net

void sendIR()
{
   
    // Send over HTTP
    if (client.connect(server, 80))
    {
      client.print("GET /");
      for (int i = 0; i < codeLen; i++) {
      client.print(rawCodes[i]);
      }
      client.println(" HTTP/1.1");
      client.println("Host: 192.168.0.101");
      client.println();
      client.println();
      client.stop();
    } 
  
}
 

void loop()
{
  if (irrecv.decode(&results))
  {
    storeCode(&results);
    sendIR();
    irrecv.resume(); // Receive the next value
  }
}
