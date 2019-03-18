#include "GestionWifi.h"
#include "ConfiguracionSistema.h"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

GestionWifi::GestionWifi(GestionMqtt *mqtt) {
  gMqtt = mqtt;  
  wifi = new ESP8266WiFiClass();  
  wifi->persistent(false);
  ntpUDP.begin(8888);
  timeClient.begin();
  timeClient.setTimeOffset(3600);
}

void GestionWifi::connectToWifi() {
  if (wifi->status() != WL_CONNECTED) {
    unsigned int retries = ConfiguracionSistema::TRIES_RECONNET;
    unsigned int intento = 0;

    const char ssid[] = "X";
    const char pass[] = "@Juanan07Sofia06Hugo11Jaime08@";

    yield();
    ESP.wdtFeed();
    wifi->disconnect();
    wifi->mode(WIFI_OFF);
    delay(20);    
    ESP.wdtFeed();
    yield();
        
    while ((wifi->status() != WL_CONNECTED) && (intento < retries))
    { 
      wifi->mode(WIFI_STA);
      ESP.wdtFeed();
      wifi->begin(ssid, pass);
      yield();
      ESP.wdtFeed();
      ConfiguracionSistema::myDelay(500);
      yield();
      ESP.wdtFeed();
      Serial.print(".");
      intento++;
    }

    ConfiguracionSistema::myDelay(500);
    yield();
    ESP.wdtFeed();
    gMqtt->sendConexionWifi();
    yield();
    ESP.wdtFeed();
    sendEmail("Conectado Wifi ", "Conectado");
    yield();
    ESP.wdtFeed();

    Serial.println("WIFI status:" + wifi->status());  
    
    if (wifi->status() == WL_CONNECTED) {
      
      gMqtt->sendConexionWifi();
      Serial.println("Connected to WIFI");
    } 
      /**else if (wifi->status() == WL_DISCONNECTED) {
        ESP.reset();
      }**/
  } 

  yield();
  ESP.wdtFeed();
}

boolean GestionWifi::isConnected() {
  return (wifi->status() == WL_CONNECTED);
}

unsigned long GestionWifi::getTimeFromTimeServerOLD()
{
  Serial.println("Trying to get new time");
  //NTPClient timeClient(ntpUDP, "hora.roa.es", timezone*3600);
  
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }

  Serial.println("New time!!");
  return timeClient.getEpochTime();
}

unsigned long GestionWifi::getTimeFromTimeServer()
{
  unsigned long mi;
  int cb = 0;
  IPAddress timeServerIP;
  
  const char ntpServerName[] = "hora.roa.es";
  
  wifi->hostByName(ntpServerName, timeServerIP);
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

byte GestionWifi::sendEmail(String strEmailSubject, String strEmailContent) {
  byte thisByte = 0;
  byte respCode;

  char server[] = "mail.domosystems.com";
  
  if (client.connect(server, 25) == 1) {
    Serial.println(F("connected"));
  } else {
    Serial.println(F("connection failed"));
    return 0;
  }
  if (!this->eRcv()) return 0;

  Serial.println(F("Sending EHLO"));
  client.println("EHLO www.domosystems.com");
  if (!this->eRcv()) return 0;
  Serial.println(F("Sending auth login"));
  client.println("auth login");
  if (!this->eRcv()) return 0;
  Serial.println(F("Sending User"));
  // Change to your base64, ASCII encoded user
  client.println("anVhbmFuQGRvbW9zeXN0ZW1zLmNvbQ=="); //<---------User
  if (!this->eRcv()) return 0;
  Serial.println(F("Sending Password"));
  // change to your base64, ASCII encoded password
  client.println("bWFmaWFzMDI=");//<---------Passw
  if (!this->eRcv()) return 0;
  Serial.println(F("Sending From"));
  // change to your email address (sender)
  client.println(F("MAIL From: <acuario@domosystems.com>"));
  if (!this->eRcv()) return 0;
  // change to recipient address
  Serial.println(F("Sending To"));
  client.println(F("RCPT To: <juanan@abmail.es>"));
  if (!this->eRcv()) return 0;
  Serial.println(F("Sending DATA"));
  client.println(F("DATA"));
  if (!this->eRcv()) return 0;
  Serial.println(F("Sending email"));
  // change to recipient address
  client.println(F("To:  juanan@abmail.es"));
  // change to your address
  client.println(F("From: acuario@domosystems.com"));
  client.println("Subject: " + strEmailSubject + "\r\n");   
  client.println(strEmailContent);

  client.println(F("."));
  if (!this->eRcv()) return 0;
  Serial.println(F("Sending QUIT"));
  client.println(F("QUIT"));
  if (!this->eRcv()) return 0;
  client.stop();
  Serial.println(F("disconnected"));
  return 1;
}

byte GestionWifi::eRcv()
{
  byte respCode;
  byte thisByte;
  int loopCount = 0;

  while (!client.available()) {
    delay(1);
    loopCount++;
    // if nothing received for 10 seconds, timeout
    if (loopCount > 10000) {
      client.stop();
      Serial.println(F("\r\nTimeout"));
      return 0;
    }
  }

  respCode = client.peek();
  while (client.available())
  {
    thisByte = client.read();
    Serial.write(thisByte);
  }

  if (respCode >= '4')
  {
    //  efail();
    return 0;
  }
  return 1;  
}
