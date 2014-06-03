#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };
// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

// Initialize the Ethernet server library
// with the IP address and port you want to use
int serverPort = 8888;
EthernetServer server(serverPort);

long timeold = 0;
long timenew = 0;
long timetemp = 0;
long period = 0;
boolean newDataAvailable = 0;
long windspeed = 0;

void windpulscallback() {
  timeold = timenew;
  timetemp = millis();
  
  if((timetemp - timeold) <= 1)
  {
      //this has been a bounce, since even at huricane
      //diff between two rising edges is longer than 1ms (~6ms)
  } else {
      timenew = timetemp;
      period = timenew - timeold;
      //set Flag that we have new data
      newDataAvailable = 1;
  }   
  
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  attachInterrupt(1, windpulscallback, RISING);
  
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    for(;;)
      ;
  }
  
  Serial.print("My IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print(".");
  }
  Serial.println();
}

void loop() {
  // put your main code here, to run repeatedly: 
  if(newDataAvailable == 1)
  {
    //calc Windspeed in m/s
    //HINT: "period" is current period length from sensor signal
    //HINT: "1000/period" is how many pulsys per second do occur
    //HINT: Formula for Windesnsor WS for windspeed in m/s is
    //      (pulses_per_second+2)/3
    windspeed = ((1000/period)+2)/3;
    
    //reset new data flag
    newDataAvailable = 0;
  } else {
    //no new data since last loop, must be calm (windstill)
    windspeed = 0;
  }
  
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    String clientMsg ="";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        //Serial.print(c);
        clientMsg+=c;//store the recieved chracters in a string
        //if the character is an "end of line" the whole message is recieved
        if (c == '\n') {
          Serial.println("Message from Client:"+clientMsg);//print it to the serial
          client.println("You said:"+clientMsg+windspeed);//modify the string and send it back
          clientMsg="";
        }
      }
    }
    // give the Client time to receive the data
    delay(1);
    // close the connection:
    client.stop();
  }
  
  
  Serial.println(windspeed);
  
  
  
  
  
  delay(500);
}
