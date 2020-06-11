#include <LiquidCrystal.h>
#define temperature A0
#define buzzer 6
#define B1 7
#define B2 8
#define B3 9
#define B4 10
//B1 is sliding switch if high will be 24 hrs else 12
//variables...............
// hrs for alarm
unsigned int aHrs = -1;
//min for alarm;
unsigned int aMins = -1;
// set the current time
int ss = 0;
int mm = 0;
int hh = 0; 
//time format if true 12H
bool timeSet = false;
 bool timeFormat = true;
 bool alarm = false;
 bool type = false;
 bool snoozer = false;
unsigned int snoozeTimer = 0;
unsigned int counterB1 = 0;
unsigned int counterB2 = 0;
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

unsigned int  start = millis();
//........................................
void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  Serial.begin(9600);
  // Print a message to the LCD.
  lcd.print("HH:MM");
  lcd.setCursor(9,0);
  lcd.print("|");
  lcd.setCursor(9,1);
  lcd.print("|");
  pinMode(temperature,INPUT);
  pinMode(buzzer,OUTPUT);
  pinMode(B1,INPUT_PULLUP);
  pinMode(B2,INPUT_PULLUP);
  pinMode(B3,INPUT_PULLUP);
  pinMode(B4,INPUT_PULLUP);
  ss = 0;
  mm = 0;
  hh = 0;
}
void displayAll() {
  displayTime(hh,mm,timeFormat);
  displayTemp(type);
  displayAlarm(aHrs,aMins,alarm);
}

void loop() {

  //call Display function
  countTime();
  start = millis();
  displayAll();
  if(digitalRead(B1) == LOW) {
    counterB1++;  //3sec
    return;
  }
  // for handling B1
  if(counterB1 >= 30) {
    setTime();
    counterB1 = 0;
  } else if (counterB1 > 0){
    timeFormat = !timeFormat;
    counterB1 = 0;
  }
   if(digitalRead(B2) == LOW) {
    counterB2++;  //3sec
    return;
  }
  //for handling B2
  if(counterB2 >= 30) {
    setAlarm();
    counterB2 = 0;
  }
   else if(counterB2 > 0 ) {
    //initially set alarm
    if(aHrs == -1) {
      setAlarm();
    }
     //mark it active or inactive
    alarm = !alarm;
     counterB2 = 0;
  }
  if(digitalRead(B3) == LOW) {
    // change bluetooth
    type = !type;
  }
  
  if(alarm && snoozeTimer < 1) {
    ringAlarm();
    snoozer = false;
    snoozeTimer = 0;
  }
  
  if(digitalRead(B4) == LOW) {
      snoozeTimer = 5;
      snoozer = true;
      snooze();
  }
}

void displayTime(unsigned int hours, unsigned int minutes, bool check) {
  //as tinkercad doesn't have RTC
  //so we assume initial time to be the mills(Start) time
  String hrs,mins;
  //checking minutes doesn't exceed 60
  minutes = minutes % 60;
  // for 12 hours (default)
    //checking if hours does'nt exceed 12 or 24
  if(check) {
   if(hours < 12) {
    lcd.setCursor(6,1);
    lcd.print("AM");
   } else {
     lcd.setCursor(6,1);
     lcd.print("PM");
   }
    hours = hours % 12;
    lcd.setCursor(5,0);
    lcd.print(":12H");
  } else {
      hours = hours % 24;
    lcd.setCursor(5,0);
    lcd.print(":24H");
    lcd.setCursor(6,1);
    lcd.print("  ");
  }
  //setting for hours
  if(hours < 9) {
    hrs = "0" + String(hours);
  } else {
    hrs = String(hours);
  }
  
  //setting for minutes
    if(minutes < 9) {
    mins = "0" + String(minutes);
  } else {
    mins = String(minutes);
  }
  String dispTime = hrs + ":" + mins;
  //set cursor to col 0, line 0
  lcd.setCursor(0,1);
  lcd.print(dispTime);
}

void displayTemp(bool check) {
  int reading = analogRead(temperature);  
 
 // converting that reading to voltage, for 3.3v arduino use 3.3
 float voltage = reading * 5.0;
 voltage /= 1024.0; 
 
 // now print out the temperature
 float temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
                                               //to degrees ((voltage - 500mV) times 100) 
   // now convert to Fahrenheit
 float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
    lcd.setCursor(10, 1);
  if(check) {
    //display clesius
    lcd.print(String(temperatureC)+ "C");
  }
  else {
    lcd.print(String(temperatureF)+ "F");
  }
}

void displayAlarm(unsigned int hours, unsigned int minutes, bool check) {
  lcd.setCursor(10, 0);
  if(hours == -1) {
    lcd.print("UNSET");
    return ;
  }
  String hrs,mins;
  if(hours <= 9) {
    hrs = "0" + String(hours);
  } else {
    hrs = String(hours);
  }
  
  //setting for minutes
    if(minutes <= 9) {
    mins = "0" + String(minutes);
  } else {
    mins = String(minutes);
  }
  String active;
  //set active = S if set else set it N
  if(check) {
   active = "S"; 
  } else {
    active = "N";
  }
  lcd.print(hrs + ":" + mins + active);
}


void ringAlarm() {
  if(aHrs == hh && aMins == mm)
      digitalWrite(buzzer,HIGH);
}

void snooze() {
  digitalWrite(buzzer,LOW);
}

void countTime() {
  // count seconds
  unsigned int currentMillis = millis();
    ss += currentMillis - start;
  if (ss < 3000) {
    return;
  }
  ss = 0;
  // count minutes
  mm++;
  if(snoozer)
    snoozeTimer--;
  if (mm < 60) {
    return;
  }
  mm = 0;
  // count hours
  hh++;
  if (hh < 24) {
    return;
  }
  hh = 0;
}

void setTime() {
//first set time
 int i = 3;
 int counter = 0;
  int mins = 0;
  int hrs = 0;
  lcd.setCursor(0,1);
  lcd.print("       ");
  while(counter < 2 ) {
      lcd.setCursor(i,1);
      lcd.blink();
    if(mins > 59)
      mins = 0;
     if(hrs > 12)
        hrs = 0;
    if(digitalRead(B3) == LOW && i == 3) {
        mins++;
        lcd.print(mins);

    }
    if(digitalRead(B3) == LOW && i == 0) {
      hrs++;
      lcd.print(hrs);
    }
    if(digitalRead(B1) == LOW) {
      counter++;
      i = 0;
    }
  }
  hh = hrs;
  mm = mins;
  lcd.noBlink();
}

void setAlarm() {
  int i = 13;
  int counter = 0;
  int mins = 0;
  int hrs = 0;
  lcd.setCursor(10,0);
  lcd.print("     ");
  while(counter < 2 ) {
      lcd.setCursor(i,0);
      lcd.blink();
    Serial.println(mins);
    if(mins > 59) {
      mins = 0;
    }
    if(hrs > 12) {
        hrs = 0;
    }
    if(digitalRead(B3) == LOW && i == 13) {
        mins++;
      lcd.print(mins);
    }
    if(digitalRead(B3) == LOW && i == 10) {
      hrs++;
      lcd.print(hrs);
    }
    if(digitalRead(B2) == LOW) {
      counter++;
      i = 10;
    }
  }
  aHrs = hrs;
  aMins = mins;
  alarm = true;
  lcd.noBlink();
}
