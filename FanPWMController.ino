#include <stdint.h>
#include <inttypes.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

uint8_t POT_PIN = 34;
uint8_t READ_PIN = 33;               // GPIO pin to read FG signal
uint8_t PWM_PIN = 32;                // GPIO pin for PWM signal
uint8_t RESOLUTION = 9;              // Resolution in bits (0-255 for 8-bit)
uint32_t PWM_FREQ = 25000;           // Frequency in Hz (25 kHz for fan control)
uint16_t duty_cycle = 0;             // Duty cycle value (0-255 for 8-bit)
time_t t;
int old_pot_value = 0;
int tot_pot = 0;
int pot_value = 0;
unsigned long count = 0;
unsigned long count_pot = 0;
bool read_pin_state = false;


void printTextDisplay(){
  display.clearDisplay();
  display.setCursor(0,0);  
  Serial.println((int)(duty_cycle/(pow(2,RESOLUTION) / 100)));
  display.print((int)(duty_cycle/(pow(2,RESOLUTION) / 100)));
  display.println("%");
  display.print("RPM:");
  if(count>0){
    //display.println((uint16_t) (30/ ( (micros()-t) /count)));
    display.println(15*1000000/ ((micros()-t) /count));
  }else{
    display.println(count);
  }
  display.display();      // Show initial text
}

void setup() {
  Serial.begin(115200);
  pinMode(READ_PIN, INPUT);
  // Configure the PWM channel and attach the pin
  if (!ledcAttach(PWM_PIN, PWM_FREQ, RESOLUTION)) {
    Serial.println("ERROR: PWM setup failed");
    for(;;);
  }
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  delay(2000);
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);

  // Set initial duty cycle (e.g., 5%)
  duty_cycle = (pow(2,RESOLUTION) / 100) * 20;  // 20% of the maximum resolution value
  analogReadResolution(RESOLUTION);
  ledcWrite(PWM_PIN, duty_cycle);
  t=micros();
  //Serial.println("PWM initialized and running at 5% duty cycle.");
}

void loop() {
  tot_pot += analogRead(POT_PIN);
  count_pot++;
  if(count_pot >= 500){
    pot_value = tot_pot/count_pot;
    if( pot_value != old_pot_value && pot_value != old_pot_value-1 && pot_value != old_pot_value+1){
      old_pot_value = pot_value;
      duty_cycle = pot_value;
      ledcWrite(PWM_PIN, duty_cycle);
      printTextDisplay();
    }
    count_pot = 0;
    tot_pot = 0;
  }


  if( digitalRead(READ_PIN) != read_pin_state ){
    count++;
    read_pin_state = digitalRead(READ_PIN);
  }
  if( micros()-t >= 1000000 ){
    printTextDisplay();
    t=micros();
    count=0;
  }
  
}
