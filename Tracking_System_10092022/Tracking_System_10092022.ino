#include <TinyGPS++.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

static const int RxGSM = 4, TxGSM = 3;
static const int RxGPS = 7, TxGPS = 6;

TinyGPSPlus gps;

int temp = 0, i;

SoftwareSerial SIM900A(RxGSM, TxGSM);
SoftwareSerial NEOGPS6(RxGPS, TxGPS);

String Val = "";
String Date, Latitude, Longitude, Time;

void setup()
{
  Serial.begin(9600);
  SIM900A.begin(9600);
  NEOGPS6.begin(9600);

  Serial.println("System initializing... Please wait.");
  delay(1000);
  Serial.println("AT+CNMI=2,2,0,0,0");
  delay(1000);
  Serial.println("GPS module initializing...");
  delay(1000);
  Serial.println("GSM module initializing...");
  delay(1000);
  Serial.println("Latitude: "+Latitude);
  delay(1000);
  Serial.println("Longitude: "+Longitude);  
  delay(1000);
  Serial.println("Date: "+Date);
  delay(1000);
  Serial.println("Time: "+Time);
  Serial.println();
  delay(1000);

  temp = 0;
}

void loop()
{
  TriggerSMS();

  while (temp)
  {
    while (NEOGPS6.available() > 0)
    {
      gps.encode(NEOGPS6.read());
      if (gps.location.isUpdated())
      {
        temp = 0;
        TrackingGPSLocation();
      }
      if (!temp)
      break;
    }
  }
}

void TriggerSMS()
{
  while(Serial.available() > 0)
  {
    if(Serial.find("trackdevice"))
    {
      temp = 1;
      break;
    }
    else
    {
      temp = 0;
    }
  }
}

void startGSM()
{
  Serial.println("Cheking module...");
  boolean at_flag = 1;
  while(at_flag)
  {
    Serial.println("AT");
    delay(100);
    while(Serial.available() > 0)
    {
      if(Serial.find("OK"))
      at_flag = 0;
    }
    
    delay(1000);
  }
  Serial.println("Module is connected!");
  delay(2000);
  Serial.println("Turning Echo off...");
  boolean echo_flag = 1;
  while(echo_flag)
  {
    Serial.println("ATEO");
    while(Serial.available() > 0)
    {
      if(Serial.find("OK"))
      echo_flag = 0;
    }

    delay(1000);
  }
  Serial.println("Echo off");
  delay(2000);
  Serial.println("Finding Network...");
  boolean net_flag = 1;
  while(net_flag)
  {
    Serial.println("AT+CPIN?");
    while(Serial.available() > 0)
    {
      if(Serial.find("+CPIN: READY"))
      net_flag = 0;
    }

    delay(1000);
  }
  Serial.println("Network found.");
  delay(1000);
}

void StartSMS()
{
  Serial.println("AT+CMGF=1");
  delay(1000);
  Serial.println("AT+CMGS=\"+63XXXXXXXXXX\"");    //change to recipient's phone number
  delay(1000);
}

void SendData(String message)
{
  Serial.println(message);
  delay(1000);
}

void SendSMS()
{
  Serial.write(26);
}

void TrackingGPSLocation()
{
  Date = gps.date.value();
  Latitude = gps.location.lat(), 6;
  Longitude = gps.location.lng(), 6;
  Time = gps.time.value();
  
  
  StartSMS();
  SendData("Device Current Location:");
  Serial.println(" ");
  SendData("Your device is currently at Latitude of " + Latitude + " and Longitude of " + Longitude + ".");
  Serial.println(" ");
  Serial.println("Click on the link to check the map.");
  Serial.println("www.google.com/maps/place/" + Latitude + "," + Longitude);
  Serial.println(" ");
  Serial.println("Date: " + Date);
  Serial.println("Time:" + Time);
  Serial.println("Nothing follows.");
  SendSMS();
  delay(2000);
}