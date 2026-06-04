#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <AccelStepper.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Ultrasonic
int trig = 5;
int echo = 18;

// Buttons
const int button1 = 17;
const int button2 = 4;
const int button3 = 15;
const int button4 = 16;
const int confirmBtn = 27;

// Stepper setup
#define STEPS_PER_REV 4096

AccelStepper stepperDolo(AccelStepper::HALF4WIRE, 23, 19, 13, 12);
AccelStepper stepperBrufen(AccelStepper::HALF4WIRE, 32, 33, 25, 26);

int count1 = 0;
int count2 = 0;

int lastState1 = HIGH;
int lastState2 = HIGH;
int lastState3 = HIGH;
int lastState4 = HIGH;
int lastConfirm = HIGH;

bool waitingShown = false;
bool enterShown = false;

long duration;
int distance;

int stepsPer45 = STEPS_PER_REV / 8;

void setup()
{
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);
  pinMode(button4, INPUT_PULLUP);
  pinMode(confirmBtn, INPUT_PULLUP);

  // ✅ SPEED INCREASED ONLY HERE
  stepperDolo.setMaxSpeed(1200);
  stepperDolo.setAcceleration(700);

  stepperBrufen.setMaxSpeed(1200);
  stepperBrufen.setAcceleration(700);

  lcd.init();
  lcd.backlight();
  lcd.clear();
}

void loop()
{
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  duration = pulseIn(echo, HIGH);
  distance = duration * 0.034 / 2;

  int state1 = digitalRead(button1);
  int state2 = digitalRead(button2);
  int state3 = digitalRead(button3);
  int state4 = digitalRead(button4);
  int confirmState = digitalRead(confirmBtn);

  if (distance < 10)
  {
    waitingShown = false;

    if (!enterShown)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("PLEASE ENTER");
      lcd.setCursor(0, 1);
      lcd.print("THE TABLET");
      enterShown = true;
    }

    if (state1 == LOW && lastState1 == HIGH)
    {
      if (count1 < 7) count1++;
      lcd.clear();
      lcd.print("DOLO: "); lcd.print(count1);
    }

    if (state2 == LOW && lastState2 == HIGH)
    {
      if (count2 < 7) count2++;
      lcd.clear();
      lcd.print("BRUFEN: "); lcd.print(count2);
    }

    if (state3 == LOW && lastState3 == HIGH)
    {
      if (count1 > 0) count1--;
      lcd.clear();
      lcd.print("DOLO: "); lcd.print(count1);
    }

    if (state4 == LOW && lastState4 == HIGH)
    {
      if (count2 > 0) count2--;
      lcd.clear();
      lcd.print("BRUFEN: "); lcd.print(count2);
    }

    if (confirmState == LOW && lastConfirm == HIGH)
    {
      lcd.clear();
      lcd.print("DISPENSING...");

      if (count1 > 0)
      {
        int steps = count1 * stepsPer45;
        stepperDolo.move(steps);
        while (stepperDolo.distanceToGo() != 0)
        {
          stepperDolo.run();
        }
      }

      if (count2 > 0)
      {
        int steps = count2 * stepsPer45;
        stepperBrufen.move(steps);
        while (stepperBrufen.distanceToGo() != 0)
        {
          stepperBrufen.run();
        }
      }

      lcd.clear();
      lcd.print("DONE");

      delay(2000);

      count1 = 0;
      count2 = 0;
    }
  }
  else
  {
    enterShown = false;

    if (!waitingShown)
    {
      lcd.clear();
      lcd.print("WAITING...");
      waitingShown = true;
    }
  }

  lastState1 = state1;
  lastState2 = state2;
  lastState3 = state3;
  lastState4 = state4;
  lastConfirm = confirmState;

  delay(200);
}