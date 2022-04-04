
// Variable Declaration
// ====================



const int analogInPin = A0;       // Analog input pin for the heart rate
const int pin_delay = A1;  // Analog input pin for the variable delay adjustable via a potentiometer
const int pin_drain = 3;      // Digital output pin for draining the storage capacitor
const int pin_hit = 4;        // Digital input pin for hits coming from the monoflop
const int pin_switch = 5;         // Digital output pin for switches
const int pin_test = 7;           // Digital output pin to check in which phase of the discharge curve of C the voltage measurement takes place
const int pin_reset = 8;          // Digital input pin for emptying the pulse height array to start a new measurement
const int threshsold = 60;

int U_pulse = 0;              // current measured heart rate
int index = 0;               // index the measured pulse height in the interval [0.127]
int pulse_rate[512];         // array with the number of 128 different pulse heights
int reset_time;              // Duration in ms of U-Max memory emptying
int j;                       // Count variable as a measure of the speed of the pulse query
 
int time_old, time_new;      // Variables for determining the loop duration
float threshold;
//int delay;            // Delay time in µs



// ===========================
// ========== SETUP ==========
// ===========================

void setup(void)
   {
    Serial.begin(9600);
    
    pinMode(pin_drain, OUTPUT);      // U memory drain pin declared as output
    pinMode(pin_switch, OUTPUT);         // switch-pin declared as output
    pinMode(pin_test, OUTPUT);           // test-pin declared as output
        
    pinMode(pin_hit, INPUT);         // hit-pin declared as input
    pinMode(pin_reset, INPUT);           // reset-pin declared as input for display
    
    
    digitalWrite(pin_drain, HIGH);   // Empty U_max memory
    digitalWrite(pin_switch, LOW);       // Switch for U-supply line open
    digitalWrite(pin_test, LOW);         // Set test pin to 0
    
    for (int i = 0; i < 1024; i++)
       {
        pulse_rate[i] = 0;           // set all heart rate hits to 0
       }
       
    //factor = 1;         // Start magnification for the graphic display
    
    reset_time = 5;     // Duration in ms of U-Max memory emptying  

// ===================================
// ========== MAIN LOOP ==========
// ===================================
   }
void loop(void)
   {
     /*
     delay = analogRead(pin_delay);
     
     delay = delay * 10;   // Delay interval 0 to 1023 * 10 µs
     
     Serial.print("delay im µs = ");
     Serial.println(delay);
     */
            
     digitalWrite(pin_drain, HIGH);    // U_max memory cleared
     delay(reset_time);                    // reset_time ms Waiting time for sufficient deletion of the U_max memory
     digitalWrite(pin_drain, LOW);     // U_max memory ready
     digitalWrite(pin_switch, HIGH);       // switch for U-supply line closed; Measurement starts... actually unnecessary, since the capacitor is quenched anyway and the first pulse is awaited
         
     
     // Wait until a pulse extended by the monoflop is registered via DIGITAL-IN!
     // ==============================================================================
     
     while(digitalRead(pin_hit) == LOW)
        {
        }
     
     
     
     //delayMicroseconds(delay);    // variable delay via the pot, so that the supply circuit switch does not already close although the voltage pulse has not yet reached its maximum!
     delayMicroseconds(5);                 // fixed delay, so that the supply circuit switch does not close too early and the voltage pulse can also grow to its maximum!
     
     digitalWrite(pin_switch, LOW);        // Pulse has been registered --> switch for U-supply line open; Measurement stopped
          
     
     // Pulse height is read in
     // ========================     

      

     //Serial.println("How Many Times Do You Want the Red LEDs to blink?"); //Prompt User for Input
     //while (Serial.available() == 0) {
      // Wait for User to Input Data
     //}
     //U_pulse = Serial.parseInt(); //Read the data the user has input
     U_pulse = analogRead(analogInPin);             // Reading the pulse height 0-5V
     Serial.print("Detector read:");
     Serial.println(U_pulse);
     Serial.println(" Volt");
         
     
     // digital output via the test pin immediately after reading the voltage to determine "where" in the discharge curve the voltage is measured. Should be done exactly in the maximum!
     // -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
     /*
     digitalWrite(pin_test, HIGH);      // Set test pin to 1
     delay(5);                          // Delay
     digitalWrite(pin_test, LOW);       // Set test pin to 0
     */
     
       
     
     // graphical and memory processing of the registered pulse height
     // ---------------------------------------------------------------
     
     if(U_pulse > threshold)     // Avoidance of displaying dominant peaks with low amplitude (θα τροποποιηθεί)
        {
         //index = map(U_pulse, 0, 1023, 0, 127);         // index of the currently measured pulse height in the interval [0.127]
     
         pulse_rate[index] = pulse_rate[index] + 1;    // current pulse height receives a hit serial output of the current pulse height 0-5V
         voltage = U_pulse * 5.0 / 1023;
         Serial.print("Pulse rate = ");                 // 
         Serial.println(voltage,2);      
        }
   }


     
