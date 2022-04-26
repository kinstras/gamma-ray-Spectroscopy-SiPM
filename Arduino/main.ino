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
#include <SD.h>
#define SEALEVELPRESSURE_HPA (1013.25)

#define SDPIN 10

const int ANALOG_INPUT_PIN = A0;       // Analog input pin for the counts
const int PIN_RESET = 8;          // Digital input pin for emptying the pulse height array to start a new measurement
const int SIGNAL_THRESHOLD = 50;        // Measure counts that are higher than background noise

//LORA
SoftwareSerial lora(4,5);

//gia ton kyrio GPS
int RXPin = 2;
int TXPin = 3;
int GPSBaud = 9600;
SoftwareSerial gpsSerial(RXPin, TXPin);

File DEDOM1; //dedomena 1vousas :()
File myFile; //dedomena 2vousas:()
Adafruit_BME280 bme;

int analogInputV, index, dValue;              
int pulse_rate[1024];         // array with the number of 128 different pulse heights
int size_of_myArray = 1024;

const int chipSelect = 10;
//initialize variables
char detector_name[40];

unsigned long delayTime=1000; //enas deuteroleptos

int counter=0;

unsigned long time_stamp                    = 0L;
unsigned long measurement_deadtime          = 0L;
unsigned long time_measurement              = 0L;      // Time stamp
unsigned long interrupt_timer               = 0L;      // Time stamp
int           start_time                    = 0L;      // Start time reference variable
long int      total_deadtime                = 0L;      // total time between signals

float temperatureC;
long int      count                         = 0L;         // A tally of the number of muon counts observed
float         last_adc_value                = 0;
char          filename[]                    = "File_000.txt";
int           Mode                          = 1;
// ===========================
// ========== SETUP ==========
// ===========================
void setup() 
{
  Serial.begin(9600); //arxikopoihsh

  lora.begin(115200);
  for (int i = 0; i < 1024; i++)
       {
        pulse_rate[i] = 0;           // set all heart rate hits to 0
       }

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

// ===================================
// ========== MAIN LOOP ==========
// ===================================
void loop()
{
  printValues(); //TYPWNOUME TIMOULES STHN OTHONOULA ;)
  time_t t = now(); //PAIRNEI THN TIMH TOU XRONOU APO TO 1970 MAGKA MOU KAI STHN DINEI NA THN FAS STHN METAVLHTH t

    
testLabFunc();
  delay(10);

  
  // Query whether the reset button has been pressed
  // ------------------------------------------
  
  if (digitalRead(pin_reset) == HIGH)
    {
     // Send spectrum to the computer
     // ===============================
      
     for (int i = 0; i < 1024; i++)
        {
         Serial.println(pulse_rate[i]);
        }
  
    
     // Delete Spectrum
     // ================
     
     for (int i = 0; i < 128; i++)
        {
         pulse_rate[i] = 0;           // set all pulse height hits to 0
        } 
    }
  
  
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
void saveSDCard(file)
{
  file.print("2310"); //gia arithmo omadas
  file.print(counter); //arithmos packetou
  counter++;
  
  file.print(t); //typwnei timh xronou sthn arxh tou dedom1.txt
  file.print(": ");
  
  file.print(bme.readTemperature());
  file.print(" *C,"); //typwnei timh thermokrasias sto dedom1.txt
  
  file.print(bme.readPressure() / 100.0F);
  file.print(" hPa,"); //typwnei timh pieshs sto dedom1.txt
  
  file.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  file.print(" m,"); //typwnei timh ypsometrou sto dedom1.txt

  file.print(bme.readHumidity());
  file.print(" %\n"); //typwnei timh ygrasias sto dedom1.txt kai stelnei ton kersora sthn epomenh grammh

}
void loraTransmitter()
{
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
}
void dummyInputFunc(){
  Serial.println("\n Give a analog voltage [mv]"); //Prompt User for Input
  while (Serial.available() == 0) {
       //Wait for User to Input Data
     }
  
//  if (Serial.available() > 0) {
//    int value = Serial.parseInt();
//    if (value != 0) {
//      Serial.println(value);
//      Serial.println("Enter a integer, but not zero, with a linefeed at the end");
//    }
//  }
  analogInputV = Serial.parseInt(); //Serial.parseInt returns zero when it can not find a number within a timeout!!
  Serial.print("analogInputV is:");   
  Serial.println(analogInputV);                    
  
  if(ANALOG_INPUT_PIN > SIGNAL_THRESHOLD and analogInputV <= 5000)     // Avoidance of displaying dominant peaks with low amplitude (θα τροποποιηθεί)
        {
         //int index = anlogRead(A0)
         index = map(analogInputV, 0, 5000, 0, 1023);
         Serial.print(" Analog voltage [mv] corresponds at ");
         Serial.println(index);
         Serial.print(" index");
         pulse_rate[index] +=  1;

         //iterate over array
         for(int i = 0; i < size_of_myArray; i++)
         {
           Serial.println(pulse_rate[i]);
         }
        }
}
void testLabFunc(){
  Serial.print("Starting to read analog Signal:");   
  
  dValue = analogRead(A0);  
  Serial.print("Digital value: ");
  Serial.print(dValue);

  analogInputV = dValue * 4.887;
  Serial.print(" , Analog Input voltage: ");
  Serial.print(analogInputV);
  Serial.println(" mV");
  pulse_rate[index] +=  1;
  //iterate over array
  for(int i = 0; i < size_of_myArray; i++)
  {
   Serial.println(pulse_rate[i]);
  }
}

void get_Mode(){ //fuction for automatic port finding on PC
    Serial.println("Gamma Ray Detector - LISER");
    Serial.println(detector_name);
    String message = "";
    message = Serial.readString();
    if(message == "write"){
      delay(1000);
      Mode = 1;
    }
    else if(message == "read"){
      delay(1000);
      Mode =  2;
    }
    else if(message == "remove"){
      delay(1000);
      Mode = 3;
    }
}
