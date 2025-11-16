#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <DS1302.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); //set LCD

#define DHTpin A0 //Set DHT pin
#define DHTtype 11 //Set DHT type
DHT dht(DHTpin, DHTtype);

#define LDR A1 //set LDR pin

#define lights 13 //relay pin for the growlights 
#define motors 12 //relay pin for the exhaust

//RTC pins
#define RST 8
#define DAT 7
#define CLK 6
DS1302 rtc(RST, DAT, CLK);

int light_state0, light_state1, light_state2, light_state3; //LDR light readings
bool light_available = false; //light availability

void setup() {
  Serial.begin(9600); //Start serial operations

  rtc.halt(false);
  //rtc.writeProtect(false);
  //rtc.setTime(10, 7, 0);  

  light_available = true; //initialize light availability

  //I/O
  pinMode(lights, OUTPUT);
  pinMode(motors, OUTPUT);
  pinMode(LDR, INPUT);

  dht.begin(); //Initiate DHT sensor

  //Initiate lcd
  lcd.init();
  lcd.backlight();
}

//light state algorithm
void set_light() {

  digitalWrite(lights, LOW);
  light_state0 = analogRead(LDR);
  Serial.print(light_state2);
  Serial.print(" ");
  delay(500);
  digitalWrite(lights, HIGH);
  delay(1000);

  //get first ldr reading
  light_state1 = analogRead(LDR);
  digitalWrite(lights, LOW);
  delay(500);
  Serial.print(light_state1);
  Serial.print(" ");
  digitalWrite(lights, HIGH);
  delay(500);

  //get second ldr reading
  digitalWrite(lights, LOW);
  light_state2 = analogRead(LDR);
  Serial.print(light_state2);
  Serial.print(" ");
  delay(500);
  digitalWrite(lights, HIGH);
  delay(500);

  //get third ldr reading
  digitalWrite(lights, LOW);
  light_state3 = analogRead(LDR);
  Serial.print(light_state3);
  Serial.print(" ");
  delay(500);
  digitalWrite(lights, HIGH);
  delay(500);

  //compare light states
  if (light_state1 > light_state2 && light_state1 > light_state3){
    Serial.println("L1");
    digitalWrite(lights, LOW);
    delay(500);
  } 
  else if (light_state2 > light_state1 && light_state2 > light_state3){
    Serial.println("L2");
    digitalWrite(lights, LOW);
    delay(500);
    digitalWrite(lights, HIGH);
    delay(500);
    digitalWrite(lights, LOW);
    delay(500);
  } 
  else {
    Serial.println("L3");
    digitalWrite(lights, LOW);
    delay(500);
    digitalWrite(lights, HIGH);
    delay(500);
    digitalWrite(lights, LOW);
    delay(500);
    digitalWrite(lights, HIGH);
    delay(500);
    digitalWrite(lights, LOW);
    delay(500);
  }
  digitalWrite(lights, HIGH);
  delay(500);
  light_available = false; //disable light availability
}

  void loop() {
  float humidity = dht.readHumidity(); //get humidity
  float temperature = dht.readTemperature(); //get temperature

  if (isnan(humidity) || isnan(temperature)) { //check if humidity and temperature are not letters
    Serial.println(F("Failed to read from DHT sensor!")); //print error if they are letters
    return;
    }

  Time t = rtc.getTime(); // returns a Time struct
  int hour = t.hour; // current hour
  int mint = t.min; // current minute
  int secc = t.sec; //current second

  //reset light availability at midnight
  if (hour == 0){
    light_available = true;
  }

  //open light at 6AM to 2PM
  if (hour >= 6 && hour < 14) {
        if (light_available) {
      set_light(); 
    }
    
  } else {
    digitalWrite(lights, LOW); //disable light after 2PM and before 6AM
  }

  //open motors every 20mins for 5mins
  if (mint >= 0 && mint < 5){
    digitalWrite(motors, HIGH);
  } else if (mint >= 20 && mint < 25){
    digitalWrite(motors, HIGH);
  } else if (mint >= 40 && mint < 45){
    digitalWrite(motors, HIGH);
  } else {
    digitalWrite(motors, LOW);
  }


  int lightLVL = analogRead(LDR); //get LDR reading


  //print data to Serial
  Serial.print(rtc.getDOWStr());
  Serial.print(" ");
  Serial.print(rtc.getDateStr());
  Serial.print(" ");
  Serial.println(rtc.getTimeStr());
  Serial.print("Humidity: ");
  Serial.println(humidity);
  Serial.print("Temperature: ");
  Serial.println(temperature);
  Serial.print("LDR: ");
  Serial.println(lightLVL);
  Serial.println(light_available);
  Serial.println("____________________");

  //print data to LCD
  lcd.setCursor(0,0);
  lcd.print("H:");
  lcd.print(humidity);
  lcd.print("~");
  lcd.print("T");
  lcd.print(temperature);
  lcd.setCursor(0, 1);
  lcd.print(lightLVL);
  delay(1000);
}
