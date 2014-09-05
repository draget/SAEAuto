/* REV Autonomous SAE Low Level Code
 Low Level Controller Code
 Version: 1.0  Date: 18/10/2013
 Board: Arduino Uno R3
 Author: Jordan Kalinowski
 */

//Turn on autonomous system before motors so accelerate off value has a chance to charge up and therefore not trip the Kelly motor controller sensor failure error

/*Error codes present in this code
 ER0: Serial array overflow error. Command received too long, ignored
 ER1: Emergency brake condition currently present
 ER2: Watchdog timeout occurred. Emergency brake engaged
 ER3: Brake servo on for too long. Brake disengaged to prevent servo pole burnout
 ER4: The motor controller that controls the motor responsible for steering reports a fault. Emergency brake engaged
 ER5: No new commands received within 300ms. Emergency brake engaged
 ER6: Steering sensor out of bounds. Emergency brake engaged
 */

//Libraries, object variables
#include <Servo.h>             //Upload the servo library
#include <avr/wdt.h>           //Upload the watchdog library

//Time keeping vairables. millis() returns time since Arduino was started (ms)
unsigned long PreviousComTime=0;   //Time since last updated communications (ms). Used for debugging only
unsigned long PreviousBrakeTime=0; //Time since brake was engaged in emergency situation
unsigned long BrakeTime=0;         //Time since brake was applied (all conditions). Used to tell how long the brake has been on
unsigned long CommandTime=0;       //Time since last new command was received. Used to emergency brake if no new command received within 300ms
volatile unsigned long EmergencyTime=0;     //Time since emergency brake engaged
unsigned long PrevSteeringTime=0;  //Previous time steering PID was last updated, used for calculations in PID loop
unsigned long SteeringErrorCheck=0;//Used to time between checking if a steering motor controller fault is present
unsigned long BrakeKeepOff=0;      //Used to keep brake off after ER3

//Serial variables
char Start;                     //Use to indicate the start of a command and to receive bytes
int ReceivedNumber=0;           //Number indicating the current number of received bytes
const int ArrayLength = 5;      //Define length of serial communication array.
char ReceivedCommand[ArrayLength];       //Array to store incoming bytes (up to 5 byte numerical commands (ie B255\n is only 3 bytes))
int ReceivedValue=0;            //Final value received in one integer
char Type=0;                    //Storage for command type received: A, B or S

//Brake variables
Servo BrakeServo;               //Name the servo
const int BrakeOff=2010;        //Brake in off position
int BrakeAngle=BrakeOff;        //Current brake position (microseconds). Set to no brake on startup
const int BrakeSlight=2000;     //Brake in position that just applies the brake
const int BrakeFull=1965;       //Full brake value
boolean BrakeOn=false;          //If true, brake is on
boolean EmergBrakeSent=false;   //If true, ER1 sent to upper level

//Accelerator variables      
const int AccelerateOff=50;     //Minimum accelerator value
int Accelerate=AccelerateOff;   //Current accelerator value. Set to no accelerate on startup
const int AccelerateFull=200;   //Maximum accelerator value

//Steering variables
int SteeringPosition;                 //Steering value from steering sensor
int SteeringDirection=0;              //Indicates if the received steering command is zero, positive or negative
boolean SteeringControllerError=false;//If true, the motor controller responsible for steering has a fault
const int SteeringSensorMin=137;       //Steering sensor full left position
const int SteeringSensorZero=509;     //The value the steering sensor outputs when steering straight
const int SteeringSensorMax=881;      //Steering sensor full right position
int SteeringAngle=0;                  //Current steering value to achieve: -128 to 127, left is negative, zero is straight. Set to keep steering at current position at startup
float SteeringPWM;                    //Steering speed
int SteeringError=0;                  //Difference between desired and actual steering position
const int SteeringMaxPWM=245;         //Represents the maximum value that the PID loop is constrained to. Can be used to restrict top steering speed. Ensure this value is between 0 and 255 inclusive (limits on arguments of the analogWrite (PWM) command)
int PreviousSteeringError=0;          //Previous steering error, used for calculations in PID loop
const float Kp=5.0;                   //Proportional constant for steering PID loop
const float Ki=0.05;                  //Integral constant for steering PID loop
const float Kd=0.0;                   //Differential constant for steering PID loop
int SteeringControllerErrorPoll=0;    //Increments depending on if the motor controller that controls steering reports an error
int SteeringPosError=0;

/* Steering motor controller commands
 INA      INB      Motor
 1        1        Brake to Vcc
 1        0        Clockwise: Steer vehicle right
 0        1        Anticlockwise: Steer vehicle left
 0        0        Brake to Gnd
 Coast by setting ENA, ENB or PWM to zero. If the motor controller pulls ENA or ENB low, a fault is present
 */

//Set Pin Numbers:

//Digital Pins
//Pin 0 reserved for USB Rx
//Pin 1 reserved for USB Tx
const int InterruptPin = 2;                //Safety circuit interrupt pin. Used to trigger emergency braking condition
const int SteeringPin = 3;                 //Steering PWM Pin
const int ENB = 4;                         //Steering motor controller pin
//Pin 5 unused
//Pin 6 unused
const int INA = 7;                         //Steering motor controller pin
const int INB = 8;                         //Steering motor controller pin
//Pin 9 unused
const int BrakePin = 10;                   //Servo PWM Pin
const int AcceleratePin = 11;              //Accelerator PWM Pin
const int ENA = 12;                        //Steering motor controller pin

/*Analog Pins
 A0: Hall sensor for steering position
 A1: Spare digital/analog pin
 A2: Spare digital/analog pin
 A3: Spare digital/analog pin
 A4: Spare digital/analog pin
 A5: Spare digital/analog pin
 */

//Other variables
volatile boolean EmergencyBrake=false;       //If true, emergency brake triggered
volatile boolean EmergencyDog=false;         //If true, watchdog not reset in time
boolean FirstCom=true;                       //Used to indicate if communications with high level has started. True means waiting for first communication to be sent
boolean SteeringSensorError=false;           //If true, steering sensor input pin has received a value not physically possible. Emergency brake will be triggered
boolean SteeringLock=false;                  //If true, steering motor has already been temporarily locked. Used to increase steering accuracy
boolean SteeringTarget=false;                //If true, have reached target steering value

void setup()
{
  //Initialise pin I/O
  pinMode(BrakePin, OUTPUT);
  pinMode(AcceleratePin, OUTPUT);
  pinMode(SteeringPin, OUTPUT);
  pinMode(InterruptPin, INPUT);
  pinMode(ENA, INPUT);                          //Set to output and send low to coast steering motor. Input can be used for reading steering motor controller fault condition
  pinMode(ENB, INPUT);                          //Set to output and send low to coast steering motor. Input can be used for reading steering motor controller fault condition
  pinMode(INA, OUTPUT);
  pinMode(INB, OUTPUT);
  pinMode(A0, INPUT);

  //Change accelerator/motor controller timer: PWM mode and frequency for both pins
  TCCR2A= _BV (WGM20) | _BV (WGM21);            //Set WGM21, WGM20 to 1 for fast PWM mode for digital pins 3, 11
  TCCR2B= _BV (CS21);                           //Set CS21 to 1 for 16MHz/(256*N) period PWM, N=8 with CS21=1 giving PWM frequency of ~7.8KHz.
  analogWrite(AcceleratePin, Accelerate);       //Set accelerator to off (see initial value declared above)

  BrakeServo.attach(BrakePin);                  //Set servo PWM pin to Digital pin 10
  BrakeServo.writeMicroseconds(BrakeAngle);     //Set servo to no brake position (see initial value declared above)

  SteeringAngle=analogRead(A0);                 //Set steering to same position as when system was started
  if (SteeringAngle>SteeringSensorZero)
    {
      SteeringAngle = map(SteeringAngle,SteeringSensorZero,SteeringSensorMax,SteeringSensorZero,SteeringSensorMin);
    } else {
      SteeringAngle = map(SteeringAngle,SteeringSensorMin,SteeringSensorZero,SteeringSensorMax,SteeringSensorZero);
    }

  //Start watchdog timer
  MCUSR= _BV (WDRF) | _BV (BORF) | _BV (EXTRF) | _BV (PORF);    //Clear watchdog bits in case of reset and watchdog remains set. Prevents infinite loop situation
  WDTCSR=B00000000;                                             //Clear watchdog bits in case of reset and watchdog remains set. Prevents infinite loop situation
  noInterrupts();                                               //Turn off interrupts for time critical event
  WDTCSR= _BV (WDCE) | _BV (WDE);                               //Request access to change watchdog setup
  WDTCSR= _BV (WDCE)&0 | _BV (WDE)&0 | _BV (WDIE) | _BV(WDP3)&0 | _BV(WDP2) | _BV(WDP1)&0 | _BV(WDP0)&0;  //Set WDCE to zero to accept new values, set WDE and WDIE for interrupt mode. WDP sets time for watchdog to trip out. Currently 0.25 seconds
  interrupts();                                                 //Reenable interrupts

    //Begin serial communications
  Serial.begin(115200,SERIAL_8N1);                      //Start serial communications to higher level. Use 115200 baud rate with 'New line' in serial monitor. New line is end character of sent command
  Serial.print(F("Finished booting\n"));                //The use of F(string) stores the string in flash memory, saving SRAM for other variable use
  wdt_reset();                                          //Reset watchdog before exiting initial setup
  attachInterrupt(0, EmergencyBrakeFunction, LOW);      //Start interrupt
}

void serialEvent()                              //At end of each cycle of loop, check for and accept new serial data if present
{
  Start=Serial.read();
  if (Start=='B' || Start=='S' || Start=='A' || Start=='R')   //If brake, steer, accelerate or reset command sent
  {
    FirstCom=false;//First valid communication received ie no longer waiting for first communication
    Type=Start;
    if (Type=='S')        //If steer command sent
    {
      do                  //Wait for next part of command
      {
        Start=Serial.read();
      } 
      while (Start==-1 && EmergencyDog==false);
      if (Start=='-') //if -, make steering direction -, if 0 make steering direction 0, if a number set steering direction 1 and store the number
      {
        SteeringDirection=-1;
      }
      else if (Start>='0' && Start<='9')            //If a number received
      {
        SteeringDirection=1;
        ReceivedCommand[ReceivedNumber]=Start;  //Add the character to the end of the array
        ReceivedNumber++;
      }
      else if (Start=='0')
      {
        SteeringDirection=0;
      }
    }
    while (Start!='\n' && EmergencyDog==false)  //While end character not received, receive data. Watchdog will break out of loop if locked up
    {
      Start=Serial.read();
      if (Start>='0' && Start<='9')       //If a number received store it. All other characters are ignored
      {
        if (ReceivedNumber==ArrayLength)
        {
          Serial.print(F("ER0\n"));   //If array is not long enough for the command: Array overflow error. Command too long, ignored.
          ReceivedNumber=0;
          Type=0;
        }
        else
        {
          ReceivedCommand[ReceivedNumber]=Start;  //Add the character to the end of the array
          ReceivedNumber++;
        }
      }
      else if (Start=='\n')                       //If end of command received: Checks for newline character
      {
        while (ReceivedNumber<ArrayLength)        //Clear bytes in array if not part of newly received command
        {
          ReceivedCommand[ReceivedNumber]=0;
          ReceivedNumber++;
        }
        ReceivedValue=atoi(ReceivedCommand);   //Convert to single integer
        if (Type=='S')   //Set sign of received value
        {
          ReceivedValue=ReceivedValue*SteeringDirection;
        }
        ReceivedNumber=0;                      //Finished receiving command
        Serial.print(F("ACK"));                //Send acknowledgement
        Serial.print(Type);                    //Echo back received command
        Serial.print(ReceivedValue);           //Echo back received command
        Serial.print('\n');                    //Echo back received command
      }
    }
    if (EmergencyDog==true)                    //If emergency watchdog tripped while receiving data, set type so incomplete data is not used
    {
      Type=0;
    }
    wdt_reset();                               //Reset watchdog timer if while loop completed in time
  }
}

ISR(WDT_vect)             //Code to execute when watchdog timer not reset in time
{
  EmergencyDog=true;
}

void EmergencyBrakeFunction()
{
  Serial.print(F("Interrupt From Safety Supervisor\n"));
  EmergencyBrake=true;
  EmergencyTime=millis();
  
}

void MotorCoast()
{
  if (SteeringLock==false)
  {
    digitalWrite(INA, 1);   //Brake to Vcc briefly
    digitalWrite(INB, 1);   //Brake to Vcc briefly
    delay(20);
    SteeringLock=true;
  }
  SteeringPWM=0;//Coast motor
}

void loop()  //Reset Type in each receiving if statement to anything besides A, B, S so loop does not go through processing the data again. Used to get better performance in loop function and to ignore bad data
{
  if (Type!=0 || FirstCom==true)           //If new command received and communication with high level established
  {
    CommandTime=millis();                  //Store time that the command was received
  }
  
  if (Type=='R')
  {
    EmergencyBrake=false;
    SteeringSensorError=false;
    SteeringControllerError=false;
    SteeringPosError=0;
    EmergBrakeSent=false;
    
  }

  if (Type=='B' && EmergencyBrake==false && (millis()-BrakeKeepOff)>1000)  //Brake command received and emergency brake not received
  {
    Type=0;                                         //Reset Type
    BrakeAngle=ReceivedValue;
    BrakeAngle=constrain(BrakeAngle, 0, 255);
    if (BrakeAngle!=0)
    {
      if (BrakeOn==false)   //If brake was not previously on, set brake time to indicate when it was turned on
      {
        BrakeTime=millis();
      }
      BrakeOn=true;
      BrakeAngle=map(BrakeAngle, 1, 255, BrakeSlight, BrakeFull);
      Accelerate=AccelerateOff;                     //If braking, dont accelerate
      analogWrite(AcceleratePin, Accelerate);       //Set accelerator to off
    }
    else
    {
      BrakeOn=false;
      BrakeAngle=BrakeOff;
    }
    BrakeServo.writeMicroseconds(BrakeAngle);
  }

  if (Type=='A' && EmergencyBrake==false) //Accelerate command received. Don't accelerate in emergency brake condition
  {
    Type=0;                          //Reset Type
    Accelerate=ReceivedValue;
    Accelerate=constrain(Accelerate, 0, 255);
    if (Accelerate!=0)               //Turn brake off
    {
      BrakeOn=false;
      BrakeAngle=BrakeOff;
      BrakeServo.writeMicroseconds(BrakeAngle);
    }
    Accelerate=map(Accelerate, 0, 255, AccelerateOff, AccelerateFull);
    analogWrite(AcceleratePin, Accelerate);       //Send accelerate signal
  }

  if (Type=='S') //Steer command received. Keep steering in emergency situations
  {
    Type=0;      //Reset Type
    SteeringAngle=ReceivedValue;
    SteeringAngle=constrain(SteeringAngle, -128, 127);
    if (SteeringAngle<0)      //Told to turn car left
    {
      SteeringAngle=map(SteeringAngle, 0, -128, SteeringSensorZero, SteeringSensorMin);//Map desired position into comparable range (left) of steering sensor.
    }
    else if (SteeringAngle>0) //Told to turn car right
    {
      SteeringAngle=map(SteeringAngle, 0, 127, SteeringSensorZero, SteeringSensorMax); //Map desired position into comparable range (right) of steering sensor.
    }
    else if (SteeringAngle==0)//Told to steer straight
    {
      SteeringAngle=SteeringSensorZero;
    }
  }
  SteeringPosition=analogRead(A0);  //Read steering sensor. analogRead returns a value between 0 and 1023
  if (SteeringPosition>SteeringSensorZero)
    {
      SteeringPosition = map(SteeringPosition,SteeringSensorZero,SteeringSensorMax,SteeringSensorZero,SteeringSensorMin);
    } else {
      SteeringPosition = map(SteeringPosition,SteeringSensorMin,SteeringSensorZero,SteeringSensorMax,SteeringSensorZero);
    }
    
  SteeringError=SteeringAngle-SteeringPosition; //Calculate steering error 
  //Do PID control of steering motor:
  int AverageError=((SteeringError+PreviousSteeringError)/2);
  SteeringPWM=Kp*SteeringError + Ki*AverageError*(millis()-PrevSteeringTime) + Kd*((SteeringError-PreviousSteeringError)/(millis()-PrevSteeringTime));//PID
  PrevSteeringTime=millis();   //Update variables for next PID calculation
  PreviousSteeringError=SteeringError;
  SteeringPWM=constrain(SteeringPWM, -SteeringMaxPWM, SteeringMaxPWM);   //SteeringMaxPWM can be used to restrict top steering speed
  if (SteeringSensorError==false && SteeringControllerError==false)      //If steering operating normally, check direction and if at target
  {
    //Decide to control steering motor direction (ie turn further left, right, or stop):
    if (SteeringTarget==false)
    {
      if ((int) SteeringPWM<0)      //If steering too far right //left
      {
        //Turn steering motor left
        digitalWrite(INA, 0);
        digitalWrite(INB, 1);
        SteeringLock=false;
      }
      else if ((int) SteeringPWM>0) //If steering too far left //right
      {
        //Turn steering motor right
        digitalWrite(INA, 1);
        digitalWrite(INB, 0);
        SteeringLock=false;
      }
      //If SteeringPWM=0, the motor will stop regardless of INA, INB being set
    }
    if (abs(SteeringError)<=3) //If close to the steering target, stop adjusting. Used to prevent jitter at close values
    {
      SteeringTarget=true;
      MotorCoast();
    }
    if (SteeringTarget==true && abs(SteeringError)>10)  //Allow coasting slightly either side of target position
    {
      SteeringTarget=false;
    }
  }
  //Check within physical bounds
  if (SteeringPosition<=SteeringSensorMin && ((digitalRead(INA)==0 && digitalRead(INB)==1) || SteeringAngle<=SteeringSensorMin))    //If car steering equal to or past full left position and being told to steer further left
  {
    //Coast steering motor
    MotorCoast();
  }
  if (SteeringPosition>=SteeringSensorMax && ((digitalRead(INA)==1 && digitalRead(INB)==0) ||  SteeringAngle>=SteeringSensorMax))    //If car steering equal to or past full right position and being told to steer further right
  {
    //Coast steering motor
    MotorCoast();
  }

  if (SteeringPosition<20 || SteeringPosition>1010) //If steering sensor producing values outside of physically possible range
  {
    if (SteeringPosError<6)
    {
      SteeringPosError=SteeringPosError+1;
    }
  }
  else
  {
    SteeringPosError=0;
  }
  if (SteeringPosError==6 && SteeringSensorError==false)
  {
    Serial.print(F("ER6\n"));
    SteeringSensorError=true;
    EmergencyBrake=true;
    EmergencyTime=millis();
    //Coast steering motor
    MotorCoast();
  }

  //Ensure PWM signal sent not out of bounds of analogWrite (0-255)
  if (SteeringSensorError==false && SteeringControllerError==false && SteeringLock==false)
  {
    int SteeringSpeed= abs(SteeringPWM);
    // This is done because the chip creating the PWM for the motor (housed under brake cylinders) cannot handle a 6us pulse which corresponds roughly to 245 - 250
    // Note this 244 value is right on the limit so it may need adjusting to a lower value.
    if(SteeringSpeed>244) 
     {
        SteeringSpeed = 255;
     }
    analogWrite(SteeringPin, SteeringSpeed);  //Send PID output to motor]
  }
  else
  {
    SteeringPWM=0;
    analogWrite(SteeringPin, 0);     //Send zero output to motor
  }

  if (EmergencyDog==true)            //If watchdog timer not reset in time
  {
    Serial.print(F("ER2\n"));        //Send error code to high level system
    EmergencyBrake=true;             //Emergency brake engaged
    EmergencyTime=millis();
    EmergencyDog=false;              //Reset tripped watchdog status
  }

  if ((millis()-SteeringErrorCheck)>150 && SteeringControllerError==false)//Check for a steering motor controller error every 150ms
  {
    if (digitalRead(ENA)==0 || digitalRead(ENB)==0) //If a steering motor controller error
    {
      Serial.print(F("ENA:"));
      Serial.print(digitalRead(ENA));
      Serial.print(F("\nENB:"));
      Serial.print(digitalRead(ENB));
      Serial.print(F("\n"));
      Serial.print(F("INA:"));
      Serial.print(digitalRead(INA));
      Serial.print(F("\nINB:"));
      Serial.print(digitalRead(INB));
      Serial.print(F("\n"));
      if (SteeringControllerErrorPoll<=2)
      {
        SteeringControllerErrorPoll=SteeringControllerErrorPoll+1;
      }
      if (SteeringControllerErrorPoll==3) //If the fault is not a temporary false reading from operating the steering motor
      {
        SteeringControllerError=true;
        Serial.print(F("ER4\n"));          //Motor controller that controls steering has reported a fault. Error is sent once until fault is cleared
        EmergencyBrake=true;
        EmergencyTime=millis();
        //Coast steering motor
        MotorCoast();
      }
      //Attempt to clear error: Clears on rising edge
      digitalWrite(INA, 0);
      digitalWrite(INB, 0);
      digitalWrite(INA, 1);
      digitalWrite(INB, 1);
    }
    else //If no error
    {
      SteeringControllerErrorPoll=0;
    }
    SteeringErrorCheck=millis();
  }

  if ((millis()-CommandTime)>300)                     //If 300 milliseconds elapsed and a new command not received over serial
  {
    Serial.print(F("ER5\n"));                         //Send error code to higher level system, emergency brake engaged
    EmergencyBrake=true;
    EmergencyTime=millis();
  }

  if (BrakeOn==true && ((millis()-BrakeTime)>8000))   //If servo on for more than 8 seconds, turn it off. Prevents brake servo pole burnout
  {
    BrakeAngle=BrakeOff;
    BrakeServo.writeMicroseconds(BrakeAngle);	      //Move Servo to off
    BrakeOn=false;
    Serial.print(F("ER3\n"));                         //Send error code to high level system to indicate brake disengaged
    BrakeKeepOff=millis();
  }

  if (EmergencyBrake==true)
  {
    if (BrakeOn==false)                           //If brake was not previously on, set brake time to indicate when it was turned on
    {
      BrakeTime=millis();
    }
    BrakeOn=true;
    Accelerate=AccelerateOff;
    analogWrite(AcceleratePin, Accelerate);       //Set accelerator to off
    BrakeAngle=BrakeFull;
    BrakeServo.writeMicroseconds(BrakeFull);
    PreviousBrakeTime=millis();
    if (EmergBrakeSent==false)
    {
      Serial.print(F("ER1\n"));                   //Send message to high level that low level is in emergency brake condition
      EmergBrakeSent=true;
    }
  }

  if (millis()-PreviousComTime>500)                   //If 0.5 seconds have elapsed, report parameters. Used to prevent serial spam. Used for debug and data logging
  {
    Serial.print(F("Servo value: "));                 //Report actual command sent to servo
    Serial.print(BrakeAngle);
    Serial.print(F(".  Accelerate value: "));         //Report actual command sent to accelerator
    Serial.print(Accelerate);
    Serial.print(F(".  Servo on: "));                 //Report if servo is on
    Serial.print(BrakeOn);
    Serial.print(F("\nSteer target: "));              //Report actual command sent to steering
    Serial.print(SteeringAngle);
    Serial.print(F(".  Steer value: "));              //Report actual steering position
    Serial.print(SteeringPosition);
    Serial.print(F(".  Steering PWM: "));             //Report PWM sent to steering motor controller
    Serial.print((int) SteeringPWM);              
    Serial.print(F(".\nEmergency brake: "));          //Report emergency brake status
    Serial.print(EmergencyBrake);
    Serial.print(F("\n\n"));                          //Print a blank line between readouts
    PreviousComTime=millis();
  }
  wdt_reset();                                        //Reset watchdog timer if loop completed in time
}
