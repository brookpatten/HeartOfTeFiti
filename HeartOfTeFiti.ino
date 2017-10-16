#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include "RF24.h"
#include <printf.h>

const int pixelCount=3;
// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(pixelCount, 5, NEO_GRB + NEO_KHZ800);

//nrf24 radio
RF24 radio(10,9);
byte addresses[][6] = {"1Node","2Node"};

//millis of timestamp of last data received
volatile unsigned long lastReceive=0;
//millis of timestamp of last data sent
volatile unsigned long lastSend=0;
//default delay used in patterns
const int defaultDelay=10;

//structure for sending pixel commands over radio
typedef struct {
  byte index;
  byte r;
  byte g;
  byte b;
  bool show;
}
pixel;

//structure for buffering rgb combos for patterns
typedef struct {
  byte r;
  byte g;
  byte b;
}
rgb;

void setup() {
  //Serial.begin(9600);

  //pixel setup
  strip.begin();
  strip.setBrightness(32);
  strip.show(); // Initialize all pixels to 'off'

  //setup the button
  pinMode(3,INPUT);
  attachInterrupt(digitalPinToInterrupt(3), button, RISING);

  //radio setup
  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1,addresses[0]);
  radio.setAutoAck( false ) ;
  radio.startListening();

  //delay(2000);
  //pulsateN(5,50,0,255,0);
  //pulse green on startup
  
//  int channel = radio.getChannel();
//  Serial.print("Channel ");
//  Serial.println(channel);
//  printf_begin();
//  radio.printDetails();

//  Serial.println("Setup Complete");
//  Serial.flush();

  button();
  //pulsateN(5,50,1,255,1);
}

//invoked when the button is pushed
void button(){
  unsigned long now=millis();

  //don't let the user interrupt a pattern in process
  if((lastReceive==0 || now - lastReceive > 5000)
  && (lastSend==0 || now-lastSend > 5000))
  {
    randomPattern();
  }
}

//convenience method to set local and remote pixels to the same values
void setLocalAndRemotePixelColor(byte index,byte r,byte g,byte b,bool show){
//  Serial.print("Set ");
//  Serial.print(index);
//  Serial.print(" to ");
//  Serial.print(r);
//  Serial.print("/");
//  Serial.print(g);
//  Serial.print("/");
//  Serial.print(b);
  
//  if(show){
//    Serial.println(" show");
//  }
//  else{
//    Serial.println("");
//  }
//  
//  Serial.flush();
    
  strip.setPixelColor(index, r, g, b);
  setRemotePixelColor(index,r,g,b,show);

  if(show){
    strip.show();
  }
}

//call at beginning of every pattern
void startPattern(){
  radio.stopListening(); 
}

//call at end of every pattern
void endPattern(){
  radio.startListening();
}

//sends a pixel value to be set on listening devices
void setRemotePixelColor(byte index,byte r,byte g,byte b,bool show){
  pixel p;
  p.index=index;
  p.r=r;
  p.g=g;
  p.b=b;
  p.show=show;

  radio.write( &p, sizeof(pixel) );  
  
  lastSend=millis(); 
}

void loop() {
  if( radio.available()){
      pixel p;
      
      while (radio.available()) {
        radio.read( &p, sizeof(pixel) );

        lastReceive = millis();
        
//        Serial.print("Received ");
//        Serial.print(p.index);
//        Serial.print(" ");
//        Serial.print(p.r);
//        Serial.print("/");
//        Serial.print(p.g);
//        Serial.print("/");
//        Serial.print(p.b);
//
//        if(p.show){
//          Serial.println(" show");
//        }
//        else{
//          Serial.println("");
//        }
//        
//        Serial.flush();
  
        strip.setPixelColor(p.index, p.r, p.g, p.b);

        if(p.show){
          strip.show();
        }
      }
   }
}


//patterns

void randomPattern(){
  byte m=random(6);

  if(m==0){
    randomSolid();
  }
  else if(m==1){
    randomSolidPulsate();
  }
  else if(m==2){
    transitionRotateColorFade(3,50,0,255,0);
  }
  else if(m==3){
    transitionRotateRandomColorFade(3,50);
  }
  else if(m==4){
    insanity(50);
  }
  else if(m==5){
    marble(10,random(255),random(255),random(255));
  }
}

void insanity(int n){
  startPattern();
  for(int i=0;i<n;i++){
    for(int p=0;p<pixelCount;p++){
      setLocalAndRemotePixelColor(p,random(255),random(255),random(255),p==pixelCount-1);
      delay(20);
    }
  }
  endPattern();
}

void marble(int n,byte r, byte g,byte b){
  startPattern();
  //iterations
  for(int i=0;i<n;i++){
    //positions in a full iteration
    for(int p=0;p<pixelCount;p++){
      //leds in current position
      for(int p2=0;p2<pixelCount;p2++){
        setLocalAndRemotePixelColor(p2,p2==p?r:0,p2==p?g:0,p2==p?b:0,p2==pixelCount-1);
      }
      delay(100);
    }
  }
  endPattern();
}

void transitionRotateColorFade(int n,int step, byte r, byte g,byte b){
  
  rgb colors[pixelCount];
  for(int i=0;i<pixelCount;i++){
    rgb a;
    a.r=r-((r/pixelCount)*i);
    a.g=g-((g/pixelCount)*i);
    a.b=b-((b/pixelCount)*i);

    colors[i]=a;
  }

  transitionRotateColors(n,step,colors);
}

void transitionRotateRandomColorFade(int n,int step){
  
  rgb colors[pixelCount];
  for(int i=0;i<pixelCount;i++){
    rgb a;
    a.r=random(255);
    a.g=random(255);
    a.b=random(255);

    colors[i]=a;
  }

  transitionRotateColors(n,step,colors);
}

void transitionRotateColors(int n, int step,rgb colors[]){
  startPattern();
  
  //iterations
  for(int y=0;y<n;y++){
    //position in a complete iteration
    for(int z=0;z<pixelCount;z++){
      //fade to next color in steps
      for(int x=0;x<step;x++){
        //each led in a single step
        for(int i=0;i<pixelCount;i++){
          
          rgb current = colors[(i+z)%pixelCount];
          rgb next = colors[(i+z+1)%pixelCount];
  
          setLocalAndRemotePixelColor(i
          ,current.r + (((next.r-current.r) / step) * x)
          ,current.g + (((next.g-current.g) / step) * x)
          ,current.b + (((next.b-current.b) / step) * x)
          ,i==pixelCount-1);
          
        }
        delay(defaultDelay);
      }
    }
  }
  endPattern();
}

void randomSolidPulsate(){
  pulsateN(5,50,random(255),random(255),random(255));
}

void pulsateN(int n,int step,byte r, byte g, byte b){
  startPattern();
  for(int x=0;x<n;x++){
    pulsateOne(r,g,b,step);
  }
  endPattern();
}

void pulsateOne(int step,byte r,byte g, byte b){
  for(int i=0;i<step;i++){
      setLocalAndRemotePixelColor(0,r - ((r/step)*i),g- ((g/step)*i),b- ((b/step)*i),false);
      setLocalAndRemotePixelColor(1,r- ((r/step)*i),g- ((g/step)*i),b- ((b/step)*i),false);
      setLocalAndRemotePixelColor(2,r- ((r/step)*i),g- ((g/step)*i),b- ((b/step)*i),true);
      delay(defaultDelay);
    }
    for(int i=step;i>0;i--){
      setLocalAndRemotePixelColor(0,r - ((r/step)*i),g- ((g/step)*i),b- ((b/step)*i),false);
      setLocalAndRemotePixelColor(1,r- ((r/step)*i),g- ((g/step)*i),b- ((b/step)*i),false);
      setLocalAndRemotePixelColor(2,r- ((r/step)*i),g- ((g/step)*i),b- ((b/step)*i),true);
      delay(defaultDelay);
    }
}

void randomSolid(){
  byte r=random(255);
  byte g=random(255);
  byte b=random(255);
  solid(r,g,b);
}

void solid(byte r, byte g,byte b){
  startPattern();
  setLocalAndRemotePixelColor(0,r,g,b,false);
  setLocalAndRemotePixelColor(1,r,g,b,false);
  setLocalAndRemotePixelColor(2,r,g,b,true);
  endPattern();
}
