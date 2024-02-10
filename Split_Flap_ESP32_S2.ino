#include "Stepper.h"
#include "RTClib.h"
#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <esp_task_wdt.h>


WiFiClientSecure client;
HTTPClient http;
RTC_DS3231 rtc;

#define WDT_TIMEOUT 3
#define TIME_OFFSET 1

const char* ssid     = "";     // your network SSID (name of wifi network)
const char* password = ""; // your network password

// Adafruit IO Time API
const char* time_api = "";
// const char* time_api_seconds ="https://io.adafruit.com/api/v2/time/seconds";

static DateTime now;
static int hour_now;
static int min_now;
static bool half_min;
static bool fresh_start;

static int min_count = 0;
static int hour_count = 0;
static int min_offset_count = 0;
static int hour_offset_count = 0;

int hour_pins[4] = {18, 17, 9, 8};
int minute_pins[4] = {35, 37, 36, 16};
// 4096 Steps // Always doublestep
Stepper hour_motor(4096, hour_pins[0], hour_pins[1], hour_pins[2], hour_pins[3]);  
Stepper minute_motor(4096, minute_pins[0], minute_pins[1], minute_pins[2], minute_pins[3]);

int hour_sensor = 6;
int min_sensor = 5;
int debug_button = 0;

const int STEPS_PER_HOUR = 23;
const int STEPS_PER_MIN = 18;

bool DEBUG = false;

Adafruit_NeoPixel *pixels;

bool wifi_connect() {
  int retry = 0;

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Retries: ");
    Serial.println(retry);

    // wait 1 second for re-trying
    delay(1000);
  
    if (retry > 5) {
      Serial.println("WiFi not connected");
      WiFi.mode(WIFI_OFF);
      return false;
    }
    retry++;
  }

  return true;      
}

void set_time() {
  http.useHTTP10(true);
  http.begin(client, time_api);
  // http.begin(client, time_api_seconds);
  http.GET();

  StaticJsonDocument<200> doc;
  deserializeJson(doc, http.getStream());

  // Read values for struct DateTime
  int year = doc["year"];
  int mon = doc["mon"];
  int mday = doc["mday"];
  int hour = doc["hour"];
  int min = doc["min"];
  int sec = doc["sec"];

  // + for less, - for more
  rtc.adjust(DateTime(year, mon, mday, hour, min, sec) + TimeSpan(TIME_OFFSET));

  // Disconnect
  http.end();
  
  // Serial.print(hour);
  // Serial.print("/");
  // Serial.print(min);
  // Serial.print("/");
  // Serial.println(sec);
  Serial.println("TIME SET!!!");

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

void set_zero() {
  
  while (true) {
    if (!digitalRead(hour_sensor)) {
      hour_motor.step(2);
      stop_motor(hour_pins);
    }

    if (!digitalRead(min_sensor)) {
      minute_motor.step(2);
      stop_motor(minute_pins);
    }

    if (digitalRead(hour_sensor) && digitalRead(min_sensor)) {
      break;
    }
  }

  while (true) {
    // Serial.print(digitalRead(hour_sensor));
    if (digitalRead(hour_sensor)) {
      hour_motor.step(2);
      stop_motor(hour_pins);
    }

    if (digitalRead(min_sensor)) {
      minute_motor.step(2);
      stop_motor(minute_pins);
    }

    if (!digitalRead(hour_sensor) && !digitalRead(min_sensor)) {

            // offset for hour = 22; min = 4 (doublestep)
      for (int i = 0; i < 14; i++) {
        hour_motor.step(2);
        stop_motor(hour_pins);
        
        if (i > 7) {
          minute_motor.step(2);
          stop_motor(minute_pins);
        }
      }    
      break;
    }
  }
}

void set_clock(int hour = now.hour(), int min = now.minute()) {
  bool hour_set = false;
  bool min_set = false;
  bool default_set = true;

  int current_hour = 0;
  int current_min = 0;
  int set_hour;
  int set_min;

  if (hour != now.hour() || min != now.minute()) {
    default_set = false;
  }

  set_hour = min > 29 ? (hour * 2) + 1 : hour * 2;
  set_min = min;
  half_min = now.minute() > 29 ? true : false;

  while (true) {
    if (current_hour >= set_hour) {
      hour_set = true;          
    }
    if (!hour_set) {
      advance_hour();
      current_hour++;
    }
    
    if (current_min >= set_min) {
      min_set = true;         
    }   
    if (!min_set) {
      advance_min();
      current_min++;
    }

    if (hour_set && min_set) {
      if (!default_set) {
        return;
      }
      
      now = rtc.now();    
      if (now.hour() != hour) {
          advance_hour();
        }          
      if (now.minute() != min) {
        if (now.minute() == 0) {
          advance_min();     
        }
        else {
          for (int i = min; i < now.minute(); i++) {
            advance_min(); 
         }
         if (!half_min && now.minute() > 29) {
          advance_hour();
          half_min = true;           
         }               
        }
      }
      hour_now = now.hour();
      min_now = now.minute();
      return;          
    }    
  }
}

void stop_motor(int motor_pins[]) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(motor_pins[i], LOW);
  }
}

void advance_min() {
  int steps = STEPS_PER_MIN;
  
  if (min_count == 34) {
    steps = 10;
    min_offset_count = 0;
  }
  if (min_count == 51) {
    steps = 10;
    min_offset_count = 0;
  }
  else if (min_offset_count == 4) {
    steps = 16;
    min_offset_count = 0;
  }
  

  if (min_count == 59) {
    while(digitalRead(min_sensor)) {
      minute_motor.step(2);
      stop_motor(minute_pins);      
    }        
    for (int i = 0; i < 4; i++) {
      minute_motor.step(2);
      stop_motor(minute_pins);
    }
    min_offset_count = 0;
    min_count = 0;
  }
  else {
    for (int i = 0; i < steps; i++) {
      minute_motor.step(2);
      stop_motor(minute_pins);
    }
    min_count++;
    min_offset_count++;
  }
}

void advance_hour() {
  int steps = STEPS_PER_HOUR;
  
  if (hour_offset_count == 4) {
    steps = 16;
    hour_offset_count = 0;
  }

  if (hour_count == 47) { 
    for (int i = 0; i < 19; i++) {
      hour_motor.step(2);
      stop_motor(hour_pins);
    }
    hour_offset_count = 0;
    hour_count = 0;
  }
  else {
    for (int i = 0; i < steps; i++) {
      hour_motor.step(2);
      stop_motor(hour_pins);
    }
    hour_count++;
    hour_offset_count++;   
  }

}

void setup() {
  Serial.begin(115200);
  pinMode(hour_sensor, INPUT_PULLUP);
  pinMode(min_sensor, INPUT_PULLUP);
  pinMode(debug_button, INPUT_PULLUP);

  delay(1000);

  if(!digitalRead(debug_button)) DEBUG = true;

  if (! rtc.begin(&Wire1)) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  // attempt to connect to Wifi network
  WiFi.begin(ssid, password);
  
  if (wifi_connect()) {
    client.setInsecure();
    set_time();  
  }
    
  hour_motor.setSpeed(1);
  minute_motor.setSpeed(1);

  set_zero();

  if (DEBUG) {
    int pixel_pin = PIN_NEOPIXEL;
    int numPixels = 1; 
    int pixelFormat = NEO_GRB + NEO_KHZ800;
      
    pixels = new Adafruit_NeoPixel(numPixels, pixel_pin, pixelFormat);
    pixels->setBrightness(10);
    pixels->begin();
  }
  else {
    now = rtc.now();
    set_clock();
    fresh_start = true;
  }
}

void loop() {

  if (DEBUG) {
    pixels->clear(); // Set all pixel colors to 'off'
    while (true) {
      advance_min();
      advance_hour();

      if (digitalRead(hour_sensor) == 0 && digitalRead(min_sensor) == 0) {
        pixels->setPixelColor(0, pixels->Color(150, 150, 150));
      }
      else if (digitalRead(hour_sensor) == 0) {
        pixels->setPixelColor(0, pixels->Color(0, 0, 150));
      }
      else if (digitalRead(min_sensor) == 0) {
        pixels->setPixelColor(0, pixels->Color(150, 0, 0));
      }
      else {
        pixels->setPixelColor(0, pixels->Color(0, 0, 0));    
      }
      
      pixels->show(); // Send the updated pixel colors to the hardware.
      delay(500);
    }
  }

  now = rtc.now();
  
  if (now.hour() != hour_now && now.minute() != min_now) {
    advance_hour();
    advance_min();
    min_now = now.minute();
    hour_now = now.hour();
    half_min = false;      
  }
  else if (now.minute() != min_now) {
    if (!half_min && now.minute() > 29) {
      advance_min();
      advance_hour();
      half_min = true;
      min_now = now.minute();
    }
    else {
      advance_min();
      min_now = now.minute();  
    }
  }

  if (!fresh_start) {
    if (now.hour() == 2 || now.hour() == 13) {
      if (now.minute() == 0) {
        esp_task_wdt_init(WDT_TIMEOUT, true); //enable panic so ESP32 restarts
        esp_task_wdt_add(NULL); //add current thread to WDT watch
        delay(5000);
      }
    }
  }
  
  if (fresh_start) {
    fresh_start = false;
  }

  now = rtc.now();
  int sleep_timer = 59 - now.second();
  esp_sleep_enable_timer_wakeup(sleep_timer * 1000 * 1000);
  esp_light_sleep_start();
  
  while (now.minute() == min_now) {
    now = rtc.now();
  }

  // Serial.print(now.hour());
  // Serial.print(" / ");
  // Serial.print(now.minute());
  // Serial.print(" / ");
  // Serial.println(now.second());
  // delay(1000);
}
