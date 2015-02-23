#include <SoftwareSerial.h>
#include <Akeru.h>
struct sigfoxData {
 int count;
} data;


#define DEBUG true

#define SERIAL_BAUD 9600

//pins
#define TRIG_PIN 11
#define ECHO_PIN 12
#define SIGFOX_LED_PIN 13
#define GREEN_LED_PIN 6
#define RED_LED_PIN 7

//Ultrasonic sensor config
#define ECHO_DELAY 10 //µs
#define POLL_DELAY 100 //ms

//States
#define WAITING_ARRIVAL 0
#define WAITING_DEPARTURE 1

//App settings
#define TRIGGER_DISTANCE 50
#define SIGFOX_INTERVAL 15000*60 //ms. Send msg every 15 mins


int STATE;
unsigned long timestamp;

void setup() {
  Serial.begin (SERIAL_BAUD);
  Akeru.begin();
  timestamp = millis();
  
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  pinMode(SIGFOX_LED_PIN, OUTPUT);
  digitalWrite(SIGFOX_LED_PIN, LOW);
  
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  
  digitalWrite(GREEN_LED_PIN, HIGH);
  digitalWrite(RED_LED_PIN, HIGH);
           
 
  STATE = WAITING_ARRIVAL;
}
void loop() {
  long distance = takeMeasure();
  setLedStatus(distance);
  if (distance <= 0){
   if (DEBUG){
     Serial.print("Something weird happened : ");
     Serial.println(distance);
   }
  }
  else{
    if (distance >= TRIGGER_DISTANCE && STATE == WAITING_DEPARTURE){
      STATE = WAITING_ARRIVAL;
      data.count++;
      Serial.print("Visitor #");
      Serial.println(data.count);
      sendData();
    }
    if (distance < TRIGGER_DISTANCE&& STATE == WAITING_ARRIVAL){
      STATE = WAITING_DEPARTURE;
      if (DEBUG){
        Serial.println("Someone arrived");
      }
    }      
  }
  delay(POLL_DELAY);
}

boolean sendData(){
  unsigned long diff = millis() - timestamp;
  Serial.println("Is it time to send ?");
  if (diff < SIGFOX_INTERVAL){
    Serial.print("No, last message was sent ");
    Serial.print(diff);
    Serial.println("ms ago");
    
    return false;
  }
  if (!Akeru.isReady()){
      Serial.println("Cannot send Sigfox message right now"); 
      Serial.println("Probably due to the 1 msg per 10' limit enforced by the lib");
      return false;
  }
  
  digitalWrite(SIGFOX_LED_PIN, HIGH);
  Serial.print("Sending counter (");
  Serial.print(data.count);
  Serial.println(") over Sigfox");
  Akeru.send(&data, sizeof(data));
  timestamp = millis();
  digitalWrite(SIGFOX_LED_PIN, LOW);
  return true;
}

int takeMeasure(){
  long duration, distance;
  
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(ECHO_DELAY);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  //Distance is calculated using approx. speed of sound :  340 m.s
  //True formula shoud be 331.3 m.s-1 + (0.66 * T°C
  //1/340 sec to cover 1m -> 1/34000 sec to cover 1cm -> 1000/34000 µs to cover 1cm)
  //1/34 ≈ 29.41
  distance = (duration/2) / 29.41;
  
  
  if (DEBUG){
    Serial.print("Distance ");
    Serial.println(distance);
  }
  return distance;
}   
/* 
* RED light is object is close
* GREEN if object if > distance
**/
boolean setLedStatus(int distance){
  if (distance <= 0){
    //invalid
    return false;
  }

  if (distance < 10){
   digitalWrite(GREEN_LED_PIN, HIGH);
   digitalWrite(RED_LED_PIN, LOW);
  }
  else{
    digitalWrite(GREEN_LED_PIN, LOW);
    digitalWrite(RED_LED_PIN, HIGH);
  }
}