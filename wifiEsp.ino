
#include "WiFiEsp.h"
#include "secrets.h"
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros
#include <Streaming.h>

char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiEspClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;
const char myReadAPIKey = SECRET_READ_APIKEY;
// 初始化我們的值
float Temperature = 0;
float Humidity = 0;
String Remarks = "";

String myStatus = "";

void setup() {
  //等待初始化序列埠
  Serial.begin(9600);  // Initialize serial
  while(!Serial){
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }
  
  // initialize serial for ESP module  
  setEspBaudRate(9600);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }

  Serial.print("Searching for ESP8266..."); 
  // initialize ESP module
  WiFi.init(&Serial1);

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }
  Serial.println("found it!");
   
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {
  
  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }

  // set the fields with the values
  ThingSpeak.setField(1, Temperature);
  ThingSpeak.setField(2, Humidity);
  ThingSpeak.setField(3, Remarks);
 
  
  // set the status
  ThingSpeak.setStatus(myStatus);
  
  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

   // 200 = ok , 404 = Not Found (HTTP 狀態碼)https://developer.mozilla.org/zh-TW/docs/Web/HTTP/Status
  if(x == 200){                
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  
  // change the values
  Temperature++;
  if(Temperature > 99){
    Temperature = 0;
  }
  Humidity = random(0,100);

  float T = ThingSpeak.readFloatField(myChannelNumber, 1, myReadAPIKey );
  float H = ThingSpeak.readFloatField(myChannelNumber, 2, myReadAPIKey );
  String R = ThingSpeak.readStringField(myChannelNumber, 3, myReadAPIKey );
  
  int statusCode = ThingSpeak.getLastReadStatus();
  if(statusCode == 200){
    Serial << "[READ] T= " << T << " H= " << H << " R= " << R ;
  }
  delay(20000); // Wait 20 seconds to update the channel again
  
}


// This function attempts to set the ESP8266 baudrate. Boards with additional hardware serial ports
// can use 115200, otherwise software serial is limited to 19200.

void setEspBaudRate(unsigned long baudrate){
  long rates[6] = {115200,74880,57600,38400,19200,9600};

  Serial.print("Setting ESP8266 baudrate to ");
  Serial.print(baudrate);
  Serial.println("...");
    
  Serial1.begin(baudrate);
}
