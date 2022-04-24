#include <Adafruit_GPS.h> //vlivliothiki GPS
#include <TimeLib.h> //vlivliothiki xronou
#include <SD.h> //vlivliothiki SD card reader
#include <SoftwareSerial.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <SoftwareSerial.h>
#include <LoRa.h>

#define SEALEVELPRESSURE_HPA (1013.25)

//LORA
SoftwareSerial lora(4,5);

//gia ton kyrio GPS
int RXPin = 2;
int TXPin = 3;
int GPSBaud = 9600;
SoftwareSerial gpsSerial(RXPin, TXPin);

File DEDOM1; //dedomena 1vousas :()

Adafruit_BME280 bme;

unsigned long delayTime=1000; //enas deuteroleptos

int counter=0;

void setup() 
{
  Serial.begin(9600); //arxikopoihsh

  lora.begin(115200);

   if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
   }
  Serial.println("LoRa has started");
  
  Serial.println(F("BME280 test")); //arxh BME280 dest

  bool status;
 
  if (!bme.begin()) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  Serial.println("BME has started");
  
  Serial.print("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("Initialization failed!");
    while (1);
  }
  Serial.println("Initialization done.");

  DEDOM1 = SD.open("dedom1.txt", FILE_WRITE); //dhmiourgia arxeiou 1vousas DI EX TEE
}

void loop()
{
  printValues(); //TYPWNOUME TIMOULES STHN OTHONOULA ;)
  time_t t = now(); //PAIRNEI THN TIMH TOU XRONOU APO TO 1970 MAGKA MOU KAI STHN DINEI NA THN FAS STHN METAVLHTH t

  DEDOM1.print("2310"); //gia arithmo omadas
  DEDOM1.print(counter); //arithmos packetou
  counter++;
  
  DEDOM1.print(t); //typwnei timh xronou sthn arxh tou dedom1.txt
  DEDOM1.print(": ");
  
  DEDOM1.print(bme.readTemperature());
  DEDOM1.print(" *C,"); //typwnei timh thermokrasias sto dedom1.txt
  
  DEDOM1.print(bme.readPressure() / 100.0F);
  DEDOM1.print(" hPa,"); //typwnei timh pieshs sto dedom1.txt
  
  DEDOM1.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  DEDOM1.print(" m,"); //typwnei timh ypsometrou sto dedom1.txt

  DEDOM1.print(bme.readHumidity());
  DEDOM1.print(" %\n"); //typwnei timh ygrasias sto dedom1.txt kai stelnei ton kersora sthn epomenh grammh

  LoRa.beginPacket();

  LoRa.print("2310"); //gia arithmo omadas
  LoRa.print(counter); //arithmos packetou
  
  LoRa.print(t); //typwnei timh xronou sthn arxh tou dedom1.txt
  LoRa.print(": ");
  
  LoRa.print(bme.readTemperature());
  //LoRa.print(" *C,"); //typwnei timh thermokrasias sto dedom1.txt
  
  LoRa.print(bme.readPressure() / 100.0F);
  //LoRa.print(" hPa,"); //typwnei timh pieshs sto dedom1.txt
  
  LoRa.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  //LoRa.print(" m,"); //typwnei timh ypsometrou sto dedom1.txt

  LoRa.print(bme.readHumidity());
  //LoRa.print(" %\n"); //typwnei timh ygrasias sto dedom1.txt kai stelnei ton kersora sthn epomenh grammh

  LoRa.endPacket();
  
  delay(delayTime); //kathysterhsh 1 se(x)ond
}
void printValues()
{ 
  time_t t_1 = now();
  
  Serial.print(t_1); //typwnei timh xronou sthn arxh tou dedom1.txt
  Serial.print(": ");
  
  Serial.print("Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");
 
  Serial.print("Pressure = ");
  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");
  
  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");
  
  Serial.print("Humidity = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");

  while (gpsSerial.available() > 0){
    Serial.write(gpsSerial.read());
  }
  Serial.println();
}
