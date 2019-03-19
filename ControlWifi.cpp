#include "ControlWifi.h"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

ControlWifi::ControlWifi() {  
  ntpUDP.begin(8888);
  timeClient.begin();
  timeClient.setTimeOffset(3600);
}

void ControlWifi::connectToWifi() {
   ESP8266WiFiMulti WiFiMulti;
   
  if (WiFiMulti.run() != WL_CONNECTED) {
    unsigned int retries = 15; // Add to settings
    unsigned int intento = 0;

    const char ssid[] = "XS";
    const char pass[] = "@Juanan07Sofia06Hugo11Jaime08@";
               
    while ((WiFiMulti.run() != WL_CONNECTED) && (intento < retries))
    { 
      WiFi.mode(WIFI_STA);
      WiFiMulti.addAP("XS", "@Juanan07Sofia06Hugo11Jaime08@");
      ESP.wdtFeed();      
      yield();
      ESP.wdtFeed();
      //ConfiguracionSistema::myDelay(500); add to settings
      yield();
      ESP.wdtFeed();
      Serial.print(".");
      intento++;
      delay(500);
    }

    //ConfiguracionSistema::myDelay(500); Add to settings
    yield();
    ESP.wdtFeed();
    yield();
    ESP.wdtFeed();
    yield();
    ESP.wdtFeed();

    //Serial.println("WIFI status:" + wifi->status());  
    
    if (WiFiMulti.run() == WL_CONNECTED) {      
      Serial.println("Connected to WIFI");
      Serial.println(WiFi.localIP());
    } 
  } 

  yield();
  ESP.wdtFeed();
}

boolean ControlWifi::isConnected() {
  ESP8266WiFiMulti WiFiMulti;
  boolean status = (WiFiMulti.run() == WL_CONNECTED);
  return status;
}

unsigned long ControlWifi::getTimeFromTimeServer()
{
  unsigned long mi;
  int cb = 0;
  IPAddress timeServerIP;
  
  const char ntpServerName[] = "hora.roa.es";
  
  WiFi.hostByName(ntpServerName, timeServerIP);
  yield();
  
  Serial.println("sending NTP packet...");
  byte packetBuffer[ NTP_PACKET_SIZE];
  WiFiUDP udp;
  udp.begin(localPort);
  yield();
  
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);

  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(timeServerIP, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
  yield();
  ESP.wdtFeed();
  
  mi = millis();

  // wait to see if a reply is available
  while (millis() - mi < 2000 && !cb)
  {
    cb = udp.parsePacket();
    yield();
    ESP.wdtFeed();
  }

  if (!cb)
  {
    //Serial.println("[ERROR]: No packet available.");
    yield();
    return -1;
  }

  //Serial.print("packet received, length=");
  //Serial.println(cb);
  // We've received a packet, read the data from it
  udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
  ESP.wdtFeed();
  
  //the timestamp starts at byte 40 of the received packet and is four bytes,
  // or two words, long. First, esxtract the two words:

  unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
  unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
  // combine the four bytes (two words) into a long integer
  // this is NTP time (seconds since Jan 1 1900):
  unsigned long secsSince1900 = highWord << 16 | lowWord;
  //Serial.print("Seconds since Jan 1 1900 = " );
  //Serial.println(secsSince1900);

  // now convert NTP time into everyday time:
  Serial.print("Unix UTC time = ");
  // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
  const unsigned long seventyYears = 2208988800UL;
  // subtract seventy years:
  unsigned long epoch = secsSince1900 - seventyYears;
  // print Unix time:
  //Serial.println(epoch);

  // adjust to user timezone
  epoch += timezone * 3600;  

  return epoch;
}
