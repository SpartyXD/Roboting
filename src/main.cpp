#include <Arduino.h>
#include <objects.h>
#include <BluetoothSerial.h>
#include <Preferences.h>

//Globals
MotorShield motors;
BluetoothSerial server;
Preferences data;

//CONSTANTS
#define NOMBRE_BLUETOOTH "RobotingUC"
int LEFT_SPEED = 250;
int RIGHT_SPEED = 250;

//Bluetooth stop
#define CONECTION_DELAY 1000
unsigned long time_now=0, last_check=0;
bool running = false;


//==========================================
//Functions
void handleBluetoothStop();
void handleCommand(String command);
void handleConfig(String command);
void spin();


//==========================================
//MAIN PROGRAM

void setup(){
  Serial.begin(115200);
  data.begin("RobotingUC");
  server.begin(NOMBRE_BLUETOOTH);
  delay(500);

  motors.init(STNDBY_PIN, PWM_A, A1_PIN, A2_PIN, PWM_B, B1_PIN, B2_PIN);

  LEFT_SPEED = data.getInt("L", 250);
  RIGHT_SPEED = data.getInt("R", 250);

  Serial.println("Systems ready!\n");
}


void loop(){
  time_now = get_time();

  while(server.available()){
    last_check = time_now;
    handleCommand(server.readStringUntil('\n'));
  }

  //Check if bluetooth lost conection
  handleBluetoothStop();
}

//==========================================
//Implementations

void handleCommand(String command){
  if(command.length() > 1){
    handleConfig(command);
    return;
  }

  char letter = command[0];
  //STOP
  if(letter == 'S'){
      motors.stopMotors();
      running = false;
      Serial.println("STOP!\n");
      return;
  }

  running = true;
  switch (letter)
  {
  case 'F':
      motors.controlMotors(LEFT_SPEED, RIGHT_SPEED);
      Serial.println("Forward!\n");
      break;
    case 'B':
      motors.controlMotors(-LEFT_SPEED, -RIGHT_SPEED);
      Serial.println("Backwards!\n");
      break;
    case 'L':
      motors.controlMotors(-LEFT_SPEED, RIGHT_SPEED);
      Serial.println("Turning Left!\n");
      break;
    case 'R':
      motors.controlMotors(LEFT_SPEED, -RIGHT_SPEED);
      Serial.println("Turning right!\n");
      break;
    case 'Z':
      spin();
      break;
  //------------------------
  default:
      break;
  }
}


void handleBluetoothStop(){
  if(running && (time_now-last_check) > CONECTION_DELAY){
    motors.stopMotors();
    running = false;
  }
}


void spin(){
  motors.controlMotors(LEFT_SPEED, -RIGHT_SPEED);
  delay(1250);
  motors.stopMotors();
}


void handleConfig(String command){
  char letter = command[0];
  int split = command.indexOf(" ");

  if(letter == 'M'){
    LEFT_SPEED = command.substring(1, split).toInt();
    RIGHT_SPEED = command.substring(split+1).toInt();

    data.putInt("L", LEFT_SPEED);
    data.putInt("R", RIGHT_SPEED);

    Serial.println("Received motor speeds: " + String(LEFT_SPEED) + " " + String(RIGHT_SPEED));
  }
}