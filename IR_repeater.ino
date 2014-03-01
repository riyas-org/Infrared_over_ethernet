//A somple IR over ethernet solution by blog.riyas.org
//it sends the raw buffer over ip
// Credits to IRlibrary from hobbycomponents

//An IR LED must be connected to the output PWM pin 3.

//
//                  THIS IS THE SERVER WHICH SEND THE IR OUT
//

#include <SPI.h>
#include <Ethernet.h>

#include <IRremote.h>

IRsend irsend; //make the ir sender
int STATUS_PIN = 4;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; //physical mac address
byte ip[] = { 192, 168, 0, 123 }; // ip in lan
byte gateway[] = { 192, 168, 0, 1 }; // internet access via router
byte subnet[] = { 255, 255, 255, 0 }; //subnet mask
EthernetServer server(80); //server port

String readString; 

String serialDataIn;
int counter=0;
unsigned int rawCodes[100];

//////////////////////

void setup(){ 
  pinMode(STATUS_PIN, OUTPUT); 
  //start Ethernet
  Ethernet.begin(mac, ip, gateway, gateway, subnet);
  server.begin();
  //enable serial data print 
  Serial.begin(9600); 
  Serial.println("Testing IR over ethernet"); 
}

void loop(){
  // Create a client connection
  EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        //read char by char HTTP request
        if (readString.length() < 200) {
          readString +=c;         
        }        
         
        //if HTTP request has ended
        if (c == '\n') {
         
        serialDataIn = String("");
        for (int i = 1; i <= readString.length(); i++) 
          {
            char d =readString.charAt(i);
            if(d >= '0' & d <= '9')
            serialDataIn += d;
            if (d == '&'){  // Handle delimiter             
            rawCodes[counter]=serialDataIn.toInt();            
            serialDataIn = String("");
            counter = counter + 1; }            
          }          

          ///////////////
          Serial.println(readString);          
          Serial.println(counter); //debuging the length
          //print the raw buff to serial
          for (int i = 0; i < counter; i++) {
          Serial.print(rawCodes[i]);
          Serial.print('#');}
          //Now send the IR
          digitalWrite(STATUS_PIN, HIGH);
          irsend.sendRaw(rawCodes, counter, 38);
          digitalWrite(STATUS_PIN, LOW);
          Serial.println("Sent IR signals from LED at Pin 3 (PWM)");
          
          client.println("HTTP/1.1 200 OK"); //send new page
          client.println("Content-Type: text/html");
          client.println();

          client.println("<HTML>");
          client.println("<HEAD>");
          client.println("<TITLE>IR over ethernet debug page</TITLE>");
          client.println("</HEAD>");
          client.println("<BODY>");

          client.println("<H1>Debug (blog.riyas.org)</H1>");
          client.println("Check on serial monitor");
          client.println("<a href=\"/1000&750&1900&800&950&800&1000&800&900&850&900&1750&1800&900&850&1800&900&850&1800&1750&900&\">MUTE</a>");
          client.println("</BODY>");
          client.println("</HTML>");
 
          delay(1);
          //stopping client
          client.stop();

          ///////////////////// control arduino pin for reference
          //if(readString.indexOf("on") >0)//
          //{
          //  digitalWrite(5, HIGH);    // set pin 5 high
          //  Serial.println("Led On");
          //}
          readString="";
          serialDataIn = String("");
          counter=0;
        }
      }
    }
  }
} 
