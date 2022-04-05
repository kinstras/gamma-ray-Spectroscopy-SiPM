/*
  Gamma Ray Detector with SiPM Arduino Code
  This code is used to record data to the built in microSD card reader/writer.
*/

#include <SD.h>
#define SDPIN 10
File myFile;
const int ANALOG_INPUT_PIN = A0;       // Analog input pin for the counts
const int PIN_RESET = 8;          // Digital input pin for emptying the pulse height array to start a new measurement
const int SIGNAL_THRESHOLD = 50;        // Measure counts that are higher than background noise

int analogInputV, index, dValue;              
int pulse_rate[1024];         // array with the number of 128 different pulse heights
int size_of_myArray = 1024;

const int chipSelect = 10;
//initialize variables
char detector_name[40];

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
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.setTimeout(3000);
  pinMode(PIN_RESET, INPUT);           // reset-pin declared as input for display

  for (int i = 0; i < 1024; i++)
       {
        pulse_rate[i] = 0;           // set all heart rate hits to 0
       }
  // wait for Serial Monitor to connect. Needed for native USB port boards only:
  if (!SD.begin(SDPIN)) {
    Serial.println(F("SD initialization failed!"));
    Serial.println(F("Is there an SD card inserted?"));
    return;
  }
  
    if (SD.exists("first_draft.txt")) {
      Serial.println("first_draft.txt exists.");
    } else {
      Serial.println("first_draft.txt doesn't exist.");
    }
    // open the file. note that only one file can be open at a time,

  // so you have to close this one before opening another.

  myFile = SD.open("first_draft.txt", FILE_WRITE);
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to first_draft.txt...");
    myFile.println("testing 1, 2, 3.");
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }


}
// ===================================
// ========== MAIN LOOP ==========
// ===================================

void loop() {
  //dummyInputFunc();
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




