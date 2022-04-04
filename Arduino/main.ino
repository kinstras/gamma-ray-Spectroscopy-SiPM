
#include <U8glib.h>

U8GLIB_ST7920_128X64 u8g(13, 11, 12, U8G_PIN_NONE);

// Settings table for increasing the clock frequency
// ======================================================

//  ADPS2     ADPS1    ADPS0     Division factor
//    0         0        0              2
//    0         0        1              2
//    0         1        0              4
//    0         1        1              8
//    1         0        0             16
//    1         0        1             32
//    1         1        0             64
//    1         1        1            128   (Standard)

#define FASTADC 1

// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif


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
float voltage;              // current measured pulse voltage
int index = 0;               // index the measured pulse height in the interval [0.127]
float factor = 1;            // Enlargement or reduction factor
int pulse_rate[1024];         // array with the number of 128 different pulse heights
int reset_time;              // Duration in ms of U-Max memory emptying
int j;                       // Count variable as a measure of the speed of the pulse query
 
int time_old, time_new;      // Variables for determining the loop duration
int delay;            // Delay time in µs



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
       
    factor = 1;         // Start magnification for the graphic display
    
    reset_time = 5;     // Duration in ms of U-Max memory emptying
    
    
    
    #if FASTADC
    
    // set prescale to 32
    
    sbi(ADCSRA,ADPS2) ;      // ADPS2 set to 1
    cbi(ADCSRA,ADPS1) ;      // ADPS1 set to 0
    sbi(ADCSRA,ADPS0) ;      // ADPS0 set to 1
    
    // cbi = 0; sbi = 1
    
    #endif
    
    
    // Drawing the axes
    // ===================
    
    u8g.firstPage();
      
    do
       {
        u8g.drawLine(0, 63, 127, 63);
        u8g.drawLine(0, 0, 0, 63);          
       }
    while( u8g.nextPage() );
           
   }
   
   
   

// ===================================
// ========== MAIN LOOP ==========
// ===================================

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
     
     U_pulse = analogRead(analogInPin);             // Reading the pulse height 0-5V
     
     
     
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
     
         pulse_rate[index] = pulse_rate[index] + 1;    // current pulse height receives a hit
         serial output of the current pulse height 0-5V
         //voltage = U_pulse * 5.0 / 1023;
         //Serial.print("Pulse rate = ");                 // 
         //Serial.println(voltage,2);      
        }


     // ==============================================
     // ============ graphical output ==============
     // ==============================================
     
           
     if (int(factor * pulse_rate[index]) > 62)    // Too many hits for sufficient y-representation ---> compression by a factor of 2
        {         
         u8g.firstPage();
                  
         do
            {
             u8g.drawLine(0, 63, 127, 63);
             u8g.drawLine(0, 0, 0, 63);
        
             for (int i = 0; i < 128; i++)
                {
                 u8g.drawLine(0+i, 63, 0+i, 63 - int(factor * pulse_rate[i]));     // draw all hits
                }        
            }
         while( u8g.nextPage() );
         
         factor = factor / 2;       
        }
      
     
         
     // Query whether the reset button has been pressed
     // ------------------------------------------
     
     if (digitalRead(pin_reset) == HIGH)
        {
         // Send spectrum to the computer
         // ===============================
          
         for (int i = 0; i < 128; i++)
            {
             Serial.println(pulse_rate[i]);
            }

        
         // Delete Spectrum
         // ================
         
         for (int i = 0; i < 128; i++)
            {
             pulse_rate[i] = 0;           // set all pulse height hits to 0
            }
       
         factor = 1;         // Start magnification for graphical representation
        
         
         u8g.firstPage();
      
         do
            {
             u8g.drawLine(0, 63, 127, 63);
             u8g.drawLine(0, 0, 0, 63);          
            }
         while( u8g.nextPage() );
         
        }
    
       
    //delay(2);   // optional time delay
  
   }
