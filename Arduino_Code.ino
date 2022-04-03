
#include <Servo.h>
#include <LiquidCrystal.h>

const int rs = 12, en = 11, d4 = 2, d5 = 4, d6 = 8, d7 = 10;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int neutralPose[3] = {0, 90, 90}; 
int toolOnePose[3] = {0, 60, 140};
int toolTwoPose[3] = {15, 60, 140};
int toolThreePose[3] = {30, 60, 180};
int handoffPose[3] = {180, 100, 80};

Servo leftRightServo;  // create servo object to control a servo
Servo upDownServo;
Servo inOutServo;
Servo gripperServo;
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

void setup() {
  leftRightServo.attach(3);  // attaches the servo on pin 9 to the servo object
  upDownServo.attach(6);
  inOutServo.attach(5);
  gripperServo.attach(9);
  lcd.begin(16,12);

  Serial.begin(9600);
  
  setPose(neutralPose);
  delay(1000);
  //gripperServo.write(0);
  //delay(500);
  ungrip();
}

void loop() {
  while (true) {
    if (Serial.available() > 0) {
      String input = Serial.readString();
      input.trim();
      lcd.print(input);
      delay(1000);
      lcd.clear();
  
      if (input.equals("scalpel")) {
//        Serial.println("scalpel selected");
        setPose(neutralPose);
        delay(1000);
        setPose(toolOnePose);
        delay(1000);
        grip();
        delay(100);
        setPoseInv(neutralPose);
        delay(300);
        break;
      } else if (input.equals("knife")) {
//        Serial.println("knife selected");
        setPose(neutralPose);
        delay(1000);
        setPose(toolTwoPose);
        grip();
        delay(1000);
        setPoseInv(neutralPose);
        delay(300);
        break;
      } 
    }
  }

  
  setPose(handoffPose);
  delay(1000);
  int *currPose = handoffPose;
  while (true) {
    if (Serial.available() > 0) {
      String input = Serial.readString();
      input.trim();
      lcd.print(input);
      delay(1000);
      lcd.clear(); 
      
        if (input.equals("thank you")) {
//        Serial.println("you're welcome :)");
          ungrip();
          delay(1000);
          setPose(neutralPose);
          break;
        }
//        }else if(input.equals("right")) {
//          currPose[0] = currPose[0] -10;
//          setPose(currPose);
//          delay(1000);
//          break; 
//        }else if(input.equals("left")) {
//          currPose[0] = currPose[0] + 10;
//          setPose(currPose);
//          delay(1000);
//          break;} 
        else {
          continue;
        }
    }
  }
  lcd.clear();  
}

void setPose(int *poseValues) {
  leftRightServo.write(poseValues[0]); //moves arm rotated
  delay(750);
  upDownServo.write(poseValues[1]); //moves arm up and down
  delay(750);
  inOutServo.write(poseValues[2]); //moves arm in and out
  delay(750);
}
void setPoseInv(int *poseValues) {
  upDownServo.write(poseValues[1]); //moves arm up and down
  delay(750);
  leftRightServo.write(poseValues[0]); //moves arm rotated
  delay(750);
  inOutServo.write(poseValues[2]); //moves arm in and out
  delay(750);
}
void grip() {
  gripperServo.write(0);
  delay(1500);
}

void ungrip() {
  gripperServo.write(110);
  delay(1500);
  
}
