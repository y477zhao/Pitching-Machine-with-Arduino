#include "ArduinoMotorShieldR3.h"

#define TIMER_MAX 7812 //OCR1A = [16 MHz / (2 * N * fDesired)] - 1, N is prescalar (1024)
//I put in a timer interrupt if you want one. Use the equation above and set TIMER_MAX to get fDesired.
//That is, it will call ISR(TIMER1_COMPA_vect) every 1/fDesired seconds. The default value gives 10 Hz.

ArduinoMotorShieldR3 md;

void setup()
{
  md.init();
  md.setSpeed2(0, 0);
  md.clearBrake2();
  pinMode(ENCODER_1, INPUT); // set ENCODER_1 to input
  pinMode(ENCODER_2, INPUT); // set ENCODER_2 to input
  InitializeInterrupt();
  interrupts();
  Serial.begin(115200); //115200 baud, 8 data bits, 1 stop bit, no parity, XON/XOFF flow control
 while (!Serial) {
   ; // wait for serial port to connect. Needed for native USB port only
  }
 Serial.println("");
 Serial.println("UW ECE Ideas Clinic Pitching Machine");
}
float speed = 0.1;
unsigned int cur1;
unsigned int cur2;
unsigned int counter = 0;
unsigned int counter1 = 0;
unsigned int counter2 = 0;
float rpm1 = 0;
float rpm2 = 0; 

void loop()
{
  float tempSpeed = 0;
  float targetRPM;
  Serial.println("Enter an RPM value");
  
  while(Serial.peek() == -1){
    ;
  }
  
  targetRPM = Serial.parseFloat();
  speed = tempSpeed;
  Serial.println(speed);
  Serial.println("Activating now");
  
  while (Serial.available() <= 0) {
      md.setSpeed2(speed, speed);
      if (targetRPM > rpm2){
      
        speed += 0.00001;
      }else if (targetRPM < rpm2){
       
        speed -= 0.00001;
      }
      cur1 = md.getCurrent(MOTOR_1);
      cur2 = md.getCurrent(MOTOR_2);
      counter++;
      if (counter == 2000){
        Serial.print("Current 1: ");
        Serial.print(cur1);
        Serial.print(" | Current 2: ");
        Serial.print(cur2);
        Serial.print(" | RPM 1: ");
        Serial.print(rpm2);
        Serial.print(" | RPM 2: ");
        Serial.println(rpm2);
        counter = 0;
      }
  }
  md.setSpeed2(0, 0);
  
  return;
}

void InitializeInterrupt() //Don't mess with this function - it sets up the control registers for the IR sensor and timer interrupts
{
  cli();    // switch interrupts off while messing with their settings
  
  PCICR   = 0x02;   // Enable PCINT1 interrupt
  PCMSK1  = 0b00001100;
  
  PRR    &= ~0x04;   //Enable Timer/Counter1
  TCCR1A  = 0b00000000; //No output pins, WGM11 = 0, WGM10 = 0
  TCCR1B  = 0b00001101; //WGM12 = 1, WGM13 = 0, CS1 = 0b101 (clk/1024)
  OCR1A   = TIMER_MAX; //Set count
  TIMSK1 |= 0b00000010; //OCIE1A = 1 (interrupt enable for OCR1A)
  
  sei();    // turn interrupts back on
}

ISR(PCINT1_vect) //Encoder Interrupt Service Routine
{
//This will trigger each time either of the IR sensors experiences any change in state
//  Serial.print(digitalRead(ENCODER_1));
//  Serial.print(" | ");
//  Serial.println(digitalRead(ENCODER_2));
  counter1 += (digitalRead(ENCODER_1));
  counter2 += (digitalRead(ENCODER_2));
}

ISR(TIMER1_COMPA_vect) //Timer Interrupt Service Routine
{
//This will trigger at a frequency determined by TIMER_MAX
  rpm1 = counter1*60;
  rpm2 = counter2*60;
  counter1 = 0;
  counter2 = 0;
}


