#include <stdint.h>
#include <inttypes.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128        // OLED display width, in pixels
#define SCREEN_HEIGHT 64        // OLED display height, in pixels
#define OLED_RESET     -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C     ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32 (in my case it was 0x3C for 128x64)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); // Display declaration

uint8_t POT_PIN = 34;                // GPIO pin to read Potentiometer value
uint8_t READ_PIN = 33;               // GPIO pin to read FG signal
uint8_t PWM_PIN = 32;                // GPIO pin for PWM signal
uint8_t RESOLUTION = 9;              // Resolution in bits used by the PWM and Potentiometer (must be between 9-12 for ADC of ESP32)
uint32_t PWM_FREQ = 25000;           // Frequency in Hz of the PWM (25 kHz for fan control)
uint16_t duty_cycle = 0;             // Duty cycle value
time_t t;                            // Variable to track time (this will eventually overflow as described in the micros() doc.)
int old_pot_value = 0;               // Old value of the potentiometer to track changes
int tot_pot = 0;                     // Sum of Potentiometer values for averaging
int pot_value = 0;                   // Current Potentiometer value
unsigned long count = 0;             // Counter for the F00/FG pulses
unsigned long count_pot = 0;         // Counter for the number of Potentiometer samples
bool read_pin_state = false;         // Old state of the READ_PIN


// Function to write the informations on the OLED Display
void printTextDisplay(){
  // Resets the display to blank
  display.clearDisplay();
  display.setCursor(0,0);

  // Print the percentage of the duty cycle both in Serial and on the Display 
  Serial.print((int)(duty_cycle/(pow(2,RESOLUTION) / 100)));
  Serial.println("%");
  display.print((int)(duty_cycle/(pow(2,RESOLUTION) / 100)));
  display.println("%");

  display.print("RPM:");
  Serial.print("RPM:");
  if(count>0){
    // The formula on the documentation is RPS = 60 / FS  , where FS is the time for two pulses (2 HIGH and 2 LOW)
    display.println(15*1000000/ ((micros()-t) /count));
    Serial.println(15*1000000/ ((micros()-t) /count));
  }else{
    // If the count is 0 we want to avoid a division by zero
    display.println(0);
    Serial.println(0);
  }
  // Show the text
  display.display();
}




void setup() {
  Serial.begin(115200);
  
  // Set the pin that reads the F00/FG signal as input
  pinMode(READ_PIN, INPUT);

  // Configure the PWM pin
  if (!ledcAttach(PWM_PIN, PWM_FREQ, RESOLUTION)) {
    Serial.println("ERROR: PWM setup failed");
    for(;;); // Don't proceed, loop forever
  }

  // Configure the Display
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Give time to the display to properly start
  delay(300);

  // Set the display text parameters
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  // Set initial duty cycle (e.g., 20%)
  duty_cycle = (pow(2,RESOLUTION) / 100) * 20;  // 20% of the maximum resolution value

  // Set the potentiometer to the same precision as the PWM signal (ATTENTION!! Resolution min 9 and max 12)
  analogReadResolution(RESOLUTION);

  // Start the PWM signal
  ledcWrite(PWM_PIN, duty_cycle);

  // Set starting time
  t=micros();
}

void loop() {

  // To have a more stable Potentiometer input i do an average on 500 samples
  // Read the Potentiometer and add the value to the total
  tot_pot += analogRead(POT_PIN);
  count_pot++;
  // When at the 500th sample check if value changed
  if(count_pot >= 500){
    // Average of the 500 samples
    pot_value = tot_pot/count_pot;
    // Check if value changed by more than 1 (ADC on ESP32 has high noise so this i necessary to have more stable output)
    if( pot_value != old_pot_value && pot_value != old_pot_value-1 && pot_value != old_pot_value+1){
      old_pot_value = pot_value;
      duty_cycle = pot_value;
      // Change the PWM duty cycle accordingly
      ledcWrite(PWM_PIN, duty_cycle);
      // Update the display output
      printTextDisplay();
    }
    // Reset variables
    count_pot = 0;
    tot_pot = 0;
  }

  // Read the F00/FG signal input to check for a pulse
  // This works as a toggle as to count as pulse it needs a change in state
  // This counts the pulses two times (both low-->high and high--low changes)
  // This is accounted for in the display function by using 15 instead of 60 (4 = two pulses per rotation times two)
  if( digitalRead(READ_PIN) != read_pin_state ){
    count++;
    read_pin_state = digitalRead(READ_PIN);
  }
  // Every 0.1S update the display (to update the RPM value)
  if( micros()-t >= 1000000 ){
    printTextDisplay();
    // Reset the variables
    t=micros();
    count=0;
  }
  
}
