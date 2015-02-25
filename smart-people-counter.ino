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
#define SIGFOX_INTERVAL 900000 //ms. Send msg every 15 mins


int STATE;
unsigned long timestamp;
int prev_distance;

void setup() {
  Serial.begin (SERIAL_BAUD);
  Akeru.begin();
  timestamp = millis();
  prev_distance = 0;
  
  
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
  //Handle distance 0 as an "over the limit" info
  if (STATE == WAITING_DEPARTURE && (distance <= 0 || distance >= TRIGGER_DISTANCE)){
    STATE = WAITING_ARRIVAL;
    data.count++;
    Serial.print("Visitor #");
    Serial.println(data.count);
  }
  else if (STATE == WAITING_ARRIVAL && (distance > 0 && distance < TRIGGER_DISTANCE)){
    STATE = WAITING_DEPARTURE;
    if (DEBUG){
      Serial.println("Someone arrived");
    }
  }      
  //Will send data .. or not. Depends on the time of last update
  sendData();
  delay(POLL_DELAY);
}

boolean sendData(){
  unsigned long diff = millis() - timestamp;
  //Serial.println("Is it time to send ?");
  if (diff < SIGFOX_INTERVAL){
    //Serial.print("No, last message was sent ");
    //Serial.print(diff);
    //Serial.println("ms ago");
    
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
  if (!Akeru.send(&data, sizeof(data))){
    Serial.println("An error occured while sending message");
    digitalWrite(SIGFOX_LED_PIN, LOW);
    return false;
  }
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
  
  
  if (DEBUG && prev_distance != distance){
    Serial.print("Distance ");
    Serial.println(distance);
    prev_distance = distance;
  }
  return distance;
}   
/* 
* RED light is object is close
* GREEN if object if > distance
**/
void setLedStatus(int distance){
  if (distance <= 0){
    digitalWrite(GREEN_LED_PIN, HIGH);
    digitalWrite(RED_LED_PIN, HIGH);
    return;
  }

  if (distance < TRIGGER_DISTANCE){
   digitalWrite(GREEN_LED_PIN, HIGH);
   digitalWrite(RED_LED_PIN, LOW);
  }
  else{
    digitalWrite(GREEN_LED_PIN, LOW);
    digitalWrite(RED_LED_PIN, HIGH);
  }
}
