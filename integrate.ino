#include <Servo.h>

#define BUMPER_DDR  DDRA
#define BUMPER_PIN  PINA

void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if(pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } else if(pin == 3 || pin == 11) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x07; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}

#define FWD 0b00000001
#define STOP 0b00000000
#define BWD 0b00000010

//Pins for the two LEDs and the black button
//const int LED_A = 36;
//const int LED_B = 37;

//Pins for servo and motor control
const int INA = 8;
const int INB = 9;
const int MOTOR_PWM_PIN = 6;
int MOTOR_SPEED = 15; //be careful with this!

const int WAIT_TIME = 10000;
const int Spe[11] = {8,10,12,14,16,18,22,25,28,30,32};   ///declare the speed array
//below is the servo initialization.
const int STEERING_SERVO_PIN = 11;

//Pins for the LEDs
const int LED_A = 36;
const int LED_B = 37;

//const int WAIT_TIME = 50;

//Physical limits for servo angle (deg)
const int STEERING_LIMIT_LOW = 70;
const int STEERING_LIMIT_HIGH = 110;
uint8_t bumper_status = 0b11111111;
int pos;
uint8_t scan_bumper[8] = {0b10000000,0b01000000,0b00100000,0b00010000,0b00001000,0b00000100,0b00000010,0b00000001};
int scan_array[8];
int First_Zero, Last_Zero;
int Differ;
int Division = 0; 
int Last_Division = 0;
int Integration = 0;
Servo myservo;

int convertChar2Array(uint8_t a , uint8_t b ){
  boolean c = a&b;
  
  return c;
}

byte receiveMsg(){       ////Here is used for receiving Msg 
  byte Msg = 10; 
  Msg = Serial.read();
  return Msg;
  
 }
 
void transmitMsg(byte Num){   /// Here is used for sending Msg
  
 // byte a = byte(Num);
  Serial.write(Num);
  
  }

void setup() {
  // put your setup code here, to run once:

   //Initialize serial commiunication over USB
  Serial.begin(9600);
   BUMPER_DDR = 0b00000000;// drive for the bumper, all the infra red switch on.
  //set pin modes for LEDs and button
  pinMode(LED_A, OUTPUT);
  pinMode(LED_B, OUTPUT);

  //Define PWM pin
  setPwmFrequency(MOTOR_PWM_PIN, 8);


  // intitial the servo parameter.
   Serial.begin(9600);

  //Set pin modes for LEDs and define servo pin
    pinMode(LED_A, OUTPUT);
    pinMode(LED_B, OUTPUT);
    myservo.attach(STEERING_SERVO_PIN);

   pos = 90;

}
int count = 0; 
void loop() {
  /* Here is used for the serial read to get the speed                   */
   count = count + 1;
   if(count > 300){
     count = 0;
     if (Serial.available()>0)
      {
        //int index;
        byte Temp_S;
        Temp_S = receiveMsg();
        int index = Temp_S -'0';
        //0index = index - '0';
        //index = Temp_S.toInt();
        MOTOR_SPEED = Spe[index];
        //Serial.print(MOTOR_SPEED,DEC);
        transmitMsg(byte(MOTOR_SPEED));
        delay(10);
        //Serial.print(index,DEC);
        transmitMsg(byte(index));
        if ( MOTOR_SPEED> 30){
            MOTOR_SPEED = 30;
          }
        
        
        } 
   }
     
  // servo control, 90 is the middle.
  digitalWrite(LED_A, HIGH);
  digitalWrite(LED_B, LOW);
  myservo.write(pos);
   
  // put your main code here, to run repeatedly:
  //Serial.println("Forward");
  digitalWrite(LED_A, HIGH);
  digitalWrite(LED_B, LOW);
  PORTK = FWD;
  analogWrite(MOTOR_PWM_PIN, MOTOR_SPEED);
  //delay(WAIT_TIME);

  //Here is only the forward 

  // scan the bumper and get 0 and 1 from white and black. when it is white the reflection is 0;
  // write down the servo extremely left and right value. 
  // pos = 70 is the extremely right.
  // pos = 110 is the extremely left.
   bumper_status = BUMPER_PIN;
   delay(10);
  // int a = int bumper_status[];
  //Serial.print(a);
  for(int i=0; i<8; i++){
    scan_array[i] = convertChar2Array(bumper_status,scan_bumper[i] );
  }
  for(int i=0; i<8; i++){
   if (scan_array[i] == 0)
   {First_Zero = i;
   break;}
  }

  for(int i=7; i>=0; i--){
   if (scan_array[i] == 0)
   {Last_Zero = i;
   break;}
  }
  Division = (7- Last_Zero)-(First_Zero - 0);
  Integration = Integration + Division;
  if(Integration > 20)
     Integration = 20;
  if (Integration < -20)
     Integration = -20;
  pos = pos + 20*((7- Last_Zero)-(First_Zero - 0)) + 1*Integration  + 20*(Division - Last_Division);
  
  if(pos >=110)
    pos = 110;
  if(pos<= 70)
    pos = 70;  
  Last_Division = Division; 
 // delay(10000);
}
