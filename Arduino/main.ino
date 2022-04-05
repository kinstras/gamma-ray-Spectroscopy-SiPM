const int analogInPin = A0;       // Analog input pin for the counts
const int pin_reset = 8;          // Digital input pin for emptying the pulse height array to start a new measurement
const int threshsold = 60;        // Measure counts that are higher than background noise


int analogInputV, index, dValue;              
int pulse_rate[1024];         // array with the number of 128 different pulse heights
int size_of_myArray = 1024;
float threshold = 0;

// ===========================
// ========== SETUP ==========
// ===========================
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(pin_reset, INPUT);           // reset-pin declared as input for display

  for (int i = 0; i < 1024; i++)
       {
        pulse_rate[i] = 0;           // set all heart rate hits to 0
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
  
  if(analogInputV > threshold and analogInputV <= 5000)     // Avoidance of displaying dominant peaks with low amplitude (θα τροποποιηθεί)
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
  




