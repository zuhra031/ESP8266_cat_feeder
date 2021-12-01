#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h> 
#include <ESP8266mDNS.h>
#include <WiFiClient.h>

#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "LittleFS.h"

#include <html.h>


ESP8266WiFiMulti wifiMulti; 
AsyncWebServer server(80);

const int ledPin1 = D0; //btn for 5sec feed
const int ledPin2 = D5; //btn for 10sec feed
const int btnPin1 = D6; //btn for 5sec feed
const int btnPin2 = D7; //btn for 10sec feed
const int motorPin = D8; //servo motor

bool run;
long int runtime;
uint8_t state{0b10000000}; 

const char* PARAM_INPUT_1 = "feed";

//run motor for 5 sec
void handleMotor5() {
  Serial.println("run handleMotor5()");
  digitalWrite(motorPin,!digitalRead(motorPin));
  digitalWrite(ledPin1,!digitalRead(ledPin1));
  delay(5000); //wait for 5sec
  digitalWrite(motorPin,!digitalRead(motorPin));
  digitalWrite(ledPin1,!digitalRead(ledPin1));
}

//run motor for 10 sec
void handleMotor10() {
  Serial.println("run handleMotor10()");
  digitalWrite(motorPin,!digitalRead(motorPin));
  digitalWrite(ledPin1,!digitalRead(ledPin1));
  delay(10000); //wait for 10sec
  digitalWrite(motorPin,!digitalRead(motorPin));
  digitalWrite(ledPin1,!digitalRead(ledPin1));
}



uint8_t state_to_number(const uint8_t s) {
  uint8_t ret = 1;
  switch ((s & 0b11111000)) {
  // clean cases
  case 0b00000001:
    ret = 1;
    break;
  case 0b00000010:
    ret = 2;
    break;
  case 0b00000011:
    ret = 3;
    break;
  case 0b00000100:
    ret = 4;
    break;
  case 0b00000101:
    ret = 5;
    break;
  default:
    break;
  }
  return ret;
}


void write_to_file(String s, String file_name) {
  Serial.println("Writing to file.\n" + s);

  File f = LittleFS.open(file_name, "w");
  for (uint i = 0; i < s.length(); i++) {
    f.write(s[i]);
  }
  f.close();
}

String read_from_file(String file_name) {
  String buff{""};
  if (LittleFS.exists(file_name)) {
    File f = LittleFS.open(file_name, "r");
    buff = f.readString();
    Serial.println("Read: " + buff);
    f.close();
  } else {
    Serial.println("Error reading.\nFile >" + file_name + "< does not exist");
  }
  return buff;
}



constexpr char STATE_txt[]{"State.txt"};
void save_state(uint8_t s) {
  state = s;
  write_to_file((String)s, STATE_txt);
  return;
}
void load_state() {
  String data = read_from_file(STATE_txt);
  if (data == "") {
    Serial.println("New state file");
    save_state(state);
    return;
  }
  state = (uint8_t)(data.toInt());
  Serial.println("Loaded state: " + data);
}


uint8_t *change_state(AsyncWebServerRequest *request) {
  Serial.println("State value: " + request->getParam(0)->value());
  long shift = request->getParam(0)->value().toInt();
  state = ((state & 0b00000111) | (1 << (8 - shift))); // clear states and set new
  save_state(state);
  static uint8_t ret{0};
  ret = state_to_number(state);
  return &ret;
}


String processor(const String &var) {
  // replace place holder values in HTML page

  if (var == "TIMER_STATE") {
    return (String)state_to_number(state);
  }
  return String();
}



//404 page
void notFound(AsyncWebServerRequest *request) {
  request->send(404, "html/text", "Not found - <a href=\"/\">Go back!</a>");
}

void setup() {
  Serial.begin(9600);
  delay(10);
  Serial.println('\n');

  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(btnPin1, INPUT_PULLUP);
  pinMode(btnPin2, INPUT_PULLUP);
  pinMode(motorPin, OUTPUT);
  
  wifiMulti.addAP("elMakina", "Greg0102");
  wifiMulti.addAP("whoami", "stekadjeligovno");
  wifiMulti.addAP("tockainfo.info", "Greg0102");
  
  Serial.println("Connecting ...");
  while (wifiMulti.run() != WL_CONNECTED) { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
    delay(250);
    Serial.print('.');
  }
  Serial.println('\n');
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());              // Tell us what network we're connected to
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());
  
  if (MDNS.begin("catfeed")) {              // Start the mDNS responder for catfeed.local
    Serial.println("mDNS responder started");
  } else {
    Serial.println("Error setting up MDNS responder!");
  }
  if (!LittleFS.begin()) {
    Serial.println("An Error has occurred while mounting LittleFS");
    ESP.reset();
  }
  load_state();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", html, processor);
  });

  // manual feed
  // Send a GET request to <ESP_IP>/run?feed=<inputMessage1>
  server.on("/run", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage1;
    if (request->hasParam(PARAM_INPUT_1)) {
      inputMessage1 = request->getParam(PARAM_INPUT_1)->value();
      //digitalWrite(inputMessage1.toInt(), inputMessage2.toInt());
      runtime = inputMessage1.toInt();
      run = true;
    }
    else {
      run = false;
      inputMessage1 = "No message sent";
    }
    //Serial.println(inputMessage1);
    request->send(200, "text/plain", "OK");
  });

   // change timer settings
  server.on("/change_timer", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("GET: Change state");
    request->send_P(200, "text/plain", change_state(request), 1);
  });


  server.onNotFound(notFound);
  server.begin();
  Serial.println(state, BIN);
}

void loop() {

  //run motor from web
  if (run == true) {
    int time = (runtime * 1000); //sec to ms
    digitalWrite(motorPin,!digitalRead(motorPin));
    digitalWrite(ledPin1,!digitalRead(ledPin1));
    Serial.println("time var = ");
    Serial.println(time);
    delay(time);
    digitalWrite(motorPin,!digitalRead(motorPin));
    digitalWrite(ledPin1,!digitalRead(ledPin1));
    run = false;
  }

  // run motor from gpio btns
  if (digitalRead(btnPin1) == LOW) {
    Serial.println("Btn1 press - feed 5sec\t");
    handleMotor5();  
  }
  if (digitalRead(btnPin2) == LOW) {
    Serial.println("Btn2 press - feed 10sec\t");
    handleMotor10();
  }

  // int timerOn = load_state();
  // if (timerOn == 1) {
  //   int time = (1 * 1000); //sec to ms
  //   // digitalWrite(motorPin,!digitalRead(motorPin));
  //   // digitalWrite(ledPin1,!digitalRead(ledPin1));
  //   Serial.println("TIMER STATE:");
  //   Serial.println(time);
  //   Serial.println("--------");
  //   // delay(time);
  //   // digitalWrite(motorPin,!digitalRead(motorPin));
  //   // digitalWrite(ledPin1,!digitalRead(ledPin1));
  // } else if (timerOn == 2) {
  //   int time = (2 * 1000); //sec to ms
  //   // digitalWrite(motorPin,!digitalRead(motorPin));
  //   // digitalWrite(ledPin1,!digitalRead(ledPin1));
  //   Serial.println("TIMER STATE:");
  //   Serial.println(time);
  //   Serial.println("--------");
  //   // delay(time);
  //   // digitalWrite(motorPin,!digitalRead(motorPin));
  //   // digitalWrite(ledPin1,!digitalRead(ledPin1));
  // } else if (timerOn == 3) {
  //   int time = (3 * 1000); //sec to ms
  //   // digitalWrite(motorPin,!digitalRead(motorPin));
  //   // digitalWrite(ledPin1,!digitalRead(ledPin1));
  //   Serial.println("TIMER STATE:");
  //   Serial.println(time);
  //   Serial.println("--------");
  //   // delay(time);
  //   // digitalWrite(motorPin,!digitalRead(motorPin));
  //   // digitalWrite(ledPin1,!digitalRead(ledPin1));
  // } else if (timerOn == 4) {
  //   int time = (4 * 1000); //sec to ms
  //   // digitalWrite(motorPin,!digitalRead(motorPin));
  //   // digitalWrite(ledPin1,!digitalRead(ledPin1));
  //   Serial.println("TIMER STATE:");
  //   Serial.println(time);
  //   Serial.println("--------");
  //   // delay(time);
  //   // digitalWrite(motorPin,!digitalRead(motorPin));
  //   // digitalWrite(ledPin1,!digitalRead(ledPin1));
  // } else if (timerOn == 5) {
  //   int time = (5 * 1000); //sec to ms
  //   // digitalWrite(motorPin,!digitalRead(motorPin));
  //   // digitalWrite(ledPin1,!digitalRead(ledPin1));
  //   Serial.println("TIMER STATE:");
  //   Serial.println(time);
  //   Serial.println("--------");
  //   // delay(time);
  //   // digitalWrite(motorPin,!digitalRead(motorPin));
  //   // digitalWrite(ledPin1,!digitalRead(ledPin1));
  // }
  delay(100);
}

