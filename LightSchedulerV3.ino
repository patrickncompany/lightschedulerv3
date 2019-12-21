#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

char* sw1on="F0F0FFFF0101";
char* sw1off="F0F0FFFF0110";
char* sw2on="F0F0FFFF1001";
char* sw2off="F0F0FFFF1010";
char* sw3on="F0F0FFF10001";
char* sw3off="F0F0FFF10010";
char* sw4on="F0F0FF1F0001";
char* sw4off="F0F0FF1F0010";
char* sw5on="F0F0F1FF0001";
char* sw5off="F0F0F1FF0010";

long unsigned onTime;
long unsigned lastOnTime;
long unsigned offTime;
long unsigned lastOffTime;
long unsigned elapsedTime;
long unsigned sw1LastPressed;
long unsigned offsetTime;
boolean firstrun;

int ledpin=5; //8266 Pin D1 GPIO5 / 32 Pin 5 GPIO5
//int ledpin=2; //onboard led
int sw1pin=4; //8266 Pin D2 / 32 Pin 4 GPIO4
int sw1state=0;
int txpin=33; //8266 Pin SD3 (SDD) GPIO10 / 32 Pin D33 GPIO33

// setting PWM properties
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8; //Resolution 8, 10, 12, 15

// to force bit low, AND 0
// to force bit high, OR 1

boolean autoMode=true;
boolean manualMode=true;
int onoffPeriod=6; // hours
int offset=0; // hours

boolean ctlMode=false;
int ctlCode=0;
boolean ctlAbort=false;

void setup() {
  Serial.begin(115200);
  pinMode(ledpin,OUTPUT);
  // PWM setup and attach
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(ledpin, ledChannel);
  Serial.println("Booting");
  ledcWrite(ledChannel,255);
  pinMode(sw1pin,INPUT);
  mySwitch.enableTransmit(txpin);
  mySwitch.setPulseLength(185);
  mySwitch.setRepeatTransmit(8);
  firstrun=true;
  bootBlink();
  if (firstrun){ Serial.println("First Run"); }

  offsetTime = offset * 60 * 60 *1000;
  onTime = onoffPeriod * 60 * 60 * 1000 + offset;
  offTime =  onoffPeriod * 60 * 60 * 1000 + offset;
  autoMode = true;

  Serial.print("offset (h) : ");
  Serial.println(offset);
  Serial.print("onoffPeriod (h) : ");
  Serial.println(onoffPeriod);
  Serial.print("autoMode : ");
  Serial.println(autoMode);
  Serial.print("manualMode : ");
  Serial.println(manualMode);  
  
  allOn(); 
  
  Serial.println("Starting Loop");  
}

void loop() {
  ledcWrite(ledChannel,50);
  //Serial.println("Loop");
  if (ctlMode){
    Serial.println("ctlMode is active");
    sw1state=digitalRead(sw1pin);
    //Serial.println("digitalRead1");
    if (sw1state == HIGH && !ctlAbort) {
      ledcWrite(ledChannel,255);
      ctlCode++;
      sw1LastPressed=millis();
      Serial.println(ctlCode);
      delay(300);
      ledcWrite(ledChannel,50);
      delay(300);
      if (ctlCode>10 || ctlAbort){
        Serial.println("Control Aborted.");
        ctlMode=false;
        ctlCode=0;
        ctlAbort=true;
        sw1LastPressed=millis(); 
      }      
    } else {
       // if abort requested refuse input for short time
       if (ctlAbort){
          ledcWrite(ledChannel,1);
          delay(1000);
          ledcWrite(ledChannel,10);
          delay(1000);
          ledcWrite(ledChannel,1);
          delay(1000);
          ledcWrite(ledChannel,50);
          delay(1000);
       }
       Serial.println("waiting for ctlMode to end");
      if (millis() - sw1LastPressed > 1000){
        ctlMode=false;
        ctlAbort=false;
        switch(ctlCode){
          case 1:
          // toggle manualMode - independent of autoMode
            Serial.println("1");
            if (manualMode){
              Serial.println("Manual Off");
              allOff();
              manualMode=false;
            } else {
              Serial.println("Manual On");
              allOn();
              manualMode=true;    
            }  
            break;
          case 2:
          // toggle manualMode - sync autoMode
            Serial.println("2");
            if (manualMode){
              Serial.println("All Off");
              allOff();
              manualMode=false;
              autoMode=false; // force autoMode sync to manual mode
            } else {
              Serial.println("All On");
              allOn();
              manualMode=true;
              autoMode=true; // force autoMode sync to manual mode  
            }              
            break;
          case 3:
            // increment offset
            Serial.println("3");
            offset++;
            if (offset>5){
              offset=0;
            }
            Serial.println("offset = "+offset);
            for (int t=0;t<offset;t++){ 
              ledcWrite(ledChannel,255);
              delay(150);
              ledcWrite(ledChannel,1);
              delay(300);
            }
            break;
          default:
            Serial.println("Unknown Control Code(1,2,3 only");
            break;                          
        }
        ctlMode=false;
        ctlCode=0;
      }      
    }
  } else {
    Serial.println("ctlMode NOT active");
    sw1state=digitalRead(sw1pin);
    //Serial.println("digitalRead2");
    if (sw1state == HIGH) {
      Serial.println("ctlMode requested");
      ctlMode=true;
      ctlCode=0;
      sw1LastPressed=millis();
    }
  }

  // if autoMode(true)
  if (autoMode){
    elapsedTime=millis()-lastOnTime;
    //Serial.print("Lights On : ");
    //Serial.print(elapsedTime);
    //Serial.print(" of ");
    //Serial.println(onTime);    
    if (elapsedTime>onTime){
      ledcWrite(ledChannel,255);
      allOff();
      lastOffTime=millis();
      elapsedTime=0;
      autoMode=false;
      manualMode=false;
      ledcWrite(ledChannel,50);
    }
  } else {
    elapsedTime=millis()-lastOffTime;
    //Serial.print("Lights Off : ");
    //Serial.print(elapsedTime);
    //Serial.print(" of ");
    //Serial.println(offTime);    
    if (elapsedTime>offTime){
      ledcWrite(ledChannel,255);
      allOn();
      lastOnTime=millis();
      elapsedTime=0;
      autoMode=true;
      manualMode=true;
      ledcWrite(ledChannel,50);
    }    
  }

}

void allOff(){
  // send tri state off
  mySwitch.sendTriState(sw1off);
  mySwitch.sendTriState(sw2off);
  mySwitch.sendTriState(sw3off);
  mySwitch.sendTriState(sw4off);
  mySwitch.sendTriState(sw5off);  
}

void allOn(){
  // send tri state on
  mySwitch.sendTriState(sw1on);
  mySwitch.sendTriState(sw2on);
  mySwitch.sendTriState(sw3on);
  mySwitch.sendTriState(sw4on);
  mySwitch.sendTriState(sw5on);  
}

void bootBlink(){
  // Blink Lights  at power up
  int blinkOn = 2000;
  int blinkOff = 1000;
  Serial.println("Lights should blink at power up.");
  allOff();
  delay(blinkOff);  
  allOn();
  delay(blinkOn);
  allOff();
  delay(blinkOff);  
}

