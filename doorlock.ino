#include <SoftwareSerial.h>
#include <Servo.h>
#include <SeeedRFID.h>

#define RFID_RX_PIN 11
#define RFID_TX_PIN 10
#define SERVO_PIN 9
#define BUTTON_PIN 13
#define RLED_PIN 2
#define GLED_PIN 3

#define NUM_CARDS 2

SeeedRFID RFID(RFID_RX_PIN, RFID_TX_PIN);
RFIDdata tag;
long currentCard;
long acceptedCards[NUM_CARDS] = {12200105, 108280};
boolean doorLocked;
byte buttonState;
byte lastButtonState;
Servo myservo;

void setup() {
    Serial.begin(57600);
    // Setup for pins
    pinMode(BUTTON_PIN, INPUT);
    pinMode(RLED_PIN, OUTPUT);
    pinMode(GLED_PIN, OUTPUT);
    myservo.attach(SERVO_PIN);
    // State the door in the locked state
    doorLocked = false;
    toggleLock();
    // Set the last button state to something that cannot occur
    lastButtonState = 2;
}

void loop() { 
    if (RFID.isAvailable()){
        tag = RFID.data();
        currentCard = RFID.cardNumber();
        Serial.println(currentCard);
        // if the card is valid toggle the lock
        if (valid(currentCard)){
          toggleLock();
        }
        else{
          // card is invalid, blink the red LED to indicate
          blinkLED(RLED_PIN);
          if (!doorLocked) {
            digitalWrite(RLED_PIN, LOW);
          }
        }
        // wait 2 seconds between RFID readings
        delay(2000);
    }
    buttonState = digitalRead(BUTTON_PIN);
    // if the button has change states since the last check
    // and the button is now being pushed toggle the lock
    if (buttonState != lastButtonState){
      if (buttonState){
        toggleLock();
      }
      lastButtonState = buttonState;
    }
}

boolean valid(long cardNumber){
  boolean valid = false;
  for (int i = 0; i < NUM_CARDS; i++){
    if (cardNumber == acceptedCards[i]){
      valid = true;
    }
  }
  return valid;
}
void toggleLock(){
  if (doorLocked){
    // Door is currently locked, unlock the door
    digitalWrite(RLED_PIN, LOW);
    digitalWrite(GLED_PIN, HIGH);
    myservo.write(175);
    blinkLED(GLED_PIN);
    doorLocked = !doorLocked;
    delay(500);
    if (myservo.read() != 175) {
      toggleLock();
    }
  }
  else{
    // Lock the door
    digitalWrite(RLED_PIN, HIGH);
    digitalWrite(GLED_PIN, LOW);
    myservo.write(5);
    blinkLED(RLED_PIN);
    doorLocked = !doorLocked;
  }
}
void blinkLED(int PIN){
  digitalWrite(PIN, LOW);
  delay(200);
  digitalWrite(PIN, HIGH);
  delay(200);
  digitalWrite(PIN, LOW);
  delay(200);
  digitalWrite(PIN, HIGH);
}
