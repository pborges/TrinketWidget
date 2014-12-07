#include <Adafruit_NeoPixel.h>

#define NEOPIXEL_PIN     6

#define BLUETOOTH_KEY    A2
#define BLUETOOTH_STATE  A3
#define BLUETOOTH_TX     2
#define BLUETOOTH_RX     1

#define MAX_REGISTERED_FUNCTIONS  10

typedef void (*func_ptr)();
func_ptr callbacks[256];

Adafruit_NeoPixel neopixel = Adafruit_NeoPixel(1, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
boolean debug = false;

void setup() {
  // SHOW NO CONNECT
  pinMode(13,OUTPUT);
  digitalWrite(13, HIGH);
  memset(&callbacks,0,sizeof(callbacks));
  neopixelSetup();
  btSetup(); 
  callbacks['l'] = &setColor;
  callbacks['d'] = &writeDigital;
  callbacks['a'] = &writeAnalog;
  callbacks['r'] = &readDigital;
  callbacks['R'] = &readAnalog;
  callbacks['h'] = &help;
  callbacks['H'] = &toggleDebug;
}

void loop() {
  while(!digitalRead(BLUETOOTH_STATE)){} // wait for connect
  digitalWrite(13,LOW);
  Serial.println("#READY;");
  while(digitalRead(BLUETOOTH_STATE)){
    btLoop();
  }
  digitalWrite(13,HIGH);
}

void neopixelSetup() {
  neopixel.begin();
  pixel(0x00,0x00,0x00);
}
void pixel(uint8_t r, uint8_t g, uint8_t b) {
  neopixel.setPixelColor(0,neopixel.Color(r,g,b));
  neopixel.show(); 
}
static const unsigned long rates[] = {9600,19200,57600,115200,38400};
void btSetup(){
  pinMode(BLUETOOTH_KEY, OUTPUT);
  pinMode(BLUETOOTH_STATE, INPUT); 
  // FIND BAUD
  digitalWrite(BLUETOOTH_KEY, HIGH);
  delay(100);
  int recvd = 0;
  char _buffer[128];
  for(int rn = 0; rn < 1; rn++)
  {
      Serial.begin(rates[rn]);
      Serial.setTimeout(100);
      Serial.write("AT\r\n");
      recvd = Serial.readBytes(_buffer,128);
      if (recvd > 0)
      {
          //pixel(0,0,0xff);
          digitalWrite(BLUETOOTH_KEY, LOW);
          break;
      }
      else {
        //pixel(0xff,0,0);
      }
      delay(100);
  }
  digitalWrite(BLUETOOTH_KEY, LOW);
}
char c;
void btLoop(){
 if(Serial.available()) {
       c = Serial.read();
       if(callbacks[c] != 0) {
         (*callbacks[c])(); 
       }
   }
}
void help() {
  Serial.println("[Help]                    h"); 
  Serial.println("[ToggleDebug]             H"); 
  Serial.println("[Set the neopixel color]  l <red> <green> <blue>");
  Serial.println("[DigitalWrite]            d <pin> <0,1>"); 
  Serial.println("[AnalogWrite]             a <pin> <pwm>"); 
  Serial.println("[DigitalRead]             r <pin>"); 
  Serial.println("[AnalogRead]              R <pin>"); 
}

void setColor() {
  if(debug) { Serial.print("#PIXEL;"); }
  Serial.setTimeout(100);
  int r = Serial.parseInt();
  if(debug) { Serial.print(" R: "); Serial.print(r); }
  int g = Serial.parseInt();
  if(debug) { Serial.print(", G: "); Serial.print(g); }
  int b = Serial.parseInt();
  if(debug) { Serial.print(", B: "); Serial.println(b); }
 
  pixel(r,g,b); 
  Serial.println("OK");
}
void setPinHigh() {
  if(debug) { Serial.print("#SET PIN HIGH;"); }
  Serial.setTimeout(100);
  int pin = Serial.parseInt(); 
  if(debug) { Serial.print(" PIN: "); Serial.println(pin); }
  pinMode(pin,OUTPUT);
  digitalWrite(pin,HIGH);
  Serial.println("OK");
}

void writeDigital() {
  if(debug) { Serial.print("#SET PIN LOW;"); }
  Serial.setTimeout(100);
  int pin = Serial.parseInt(); 
  if(debug) { Serial.print(" PIN: "); Serial.print(pin); }
  boolean state = Serial.parseInt() > 0; 
  if(debug) { 
    Serial.print(", STATE: "); 
    if(state) {
        Serial.println("HIGH");
    } else {
       Serial.println("LOW"); 
    }
  }

  pinMode(pin,OUTPUT);
  digitalWrite(pin,state);
  Serial.println("OK");
}
void readDigital() {
  if(debug) { Serial.print("#READ PIN STATE;"); }
  Serial.setTimeout(100);
  int pin = Serial.parseInt(); 
  if(debug) { Serial.print(" PIN: "); Serial.println(pin); }
  pinMode(pin,INPUT);
  Serial.println(digitalRead(pin));
}
void writeAnalog() {
  if(debug) { Serial.print("#SET ANALOG ON;"); }
  Serial.setTimeout(100);
  int pin = Serial.parseInt(); 
  if(debug) { Serial.print(" PIN: A"); Serial.print(pin); }
  pin +=14; // for analog
  int pwm = Serial.parseInt(); 
  if(debug) { Serial.print(", PWM: "); Serial.println(pwm); }
  pinMode(pin,OUTPUT);
  analogWrite(pin,pwm);
  Serial.println("OK");
}
void readAnalog() {
  if(debug) { Serial.print("#READ ANALOG;"); }
  Serial.setTimeout(100);
  int pin = Serial.parseInt(); 
  if(debug) { Serial.print(" PIN: A"); Serial.println(pin); }
  pin +=14; // for analog
  pinMode(pin,INPUT);
  Serial.println(analogRead(pin));
}
void toggleDebug() {
  debug = !debug;
}
