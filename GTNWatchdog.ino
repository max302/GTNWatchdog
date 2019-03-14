//Global variable definitions

#define PHOTORESISTOR A1
#define SWITCH 4
#define POWERCUT 1
#define LOOPDELAY 10000
#define DESIREDSTATUS 2

#if defined(ARDUINO_SAMD_ZERO) && defined(SERIAL_PORT_USBVIRTUAL)
  // Required for Serial on Zero based boards
  #define Serial SERIAL_PORT_USBVIRTUAL
#endif

int fetch_light() {
  Serial.print(analogRead(PHOTORESISTOR));
  if (analogRead(PHOTORESISTOR) > 500) {   //Adjust as needed.
      Serial.print(" LIGHT=1\n");
      return 1;
  }
  Serial.print(" LIGHT=0\n");
  return 0;
}

void set_power(int status) {
  if (status == 1) {digitalWrite(POWERCUT, HIGH);}
  else if (status == 0) {digitalWrite(POWERCUT, LOW);}
}

void button_press(){
  digitalWrite(SWITCH, HIGH);
  delay(200);
  digitalWrite(SWITCH, LOW);
}

void reset(){
  Serial.print("REBOOTING\n");
  digitalWrite(SWITCH, HIGH); // Press and hold 7 seconds
  delay(7000);
  digitalWrite(SWITCH, LOW);
  delay(2000);                //Wait
  digitalWrite(SWITCH, HIGH); // Press and hold 2 seconds
  delay(2000);
  digitalWrite(SWITCH, LOW);

}

// Return values: 0 = DEAD  1 = Normal/paired 2 = Relay  3 = Unpaired
int check_status(){
  int status = 0;
  int lightcount = 0;

  set_power(0); //Turn off USB power to shut down red light

  // Check if paired
  Serial.print("Checking if paired.\n");
  digitalWrite(SWITCH, HIGH);  // We can't use the buttonpress() function here since when paired,
  delay(200);
  status += fetch_light();
  digitalWrite(SWITCH, LOW);
  Serial.print("\n");

  delay(2000);

  // Check if in relay mode
  Serial.print("Checking if in relay mode\n");
  button_press();
  lightcount = 0;
  for (int i = 0; i < 3; i++){
    if (fetch_light()) {lightcount++;}
    delay(333);
  }
  if (lightcount == 3) {status++;}
  Serial.print("\n");

  delay(2000);

  // Check if unpaired (constant flashing) with two more light polling counts
  Serial.print("Check if unpaired.\n");
  button_press();
  lightcount = 0;
  for (int i = 0; i < 2; i++) {
    if (fetch_light()) {lightcount++;}
    delay(200);
  }
  if (lightcount == 2) {status++;}
  Serial.print("\n");

  set_power(1); // Turn power back on
  Serial.print("\n");
  return status;
}

void setup() {

  //Set pin modes
  pinMode(SWITCH, INPUT_PULLUP);
  pinMode(POWERCUT, INPUT_PULLUP);

  Serial.print("Waiting on GoTenna bootup...\n");
  delay(60000); // Wait a minute for GoTenna to boot up
  Serial.print("LAUNCHING!\n\n");
}

void loop() {
  // put your main code here, to run repeatedly:
  int currentstatus = check_status();
  if (currentstatus != DESIREDSTATUS) {
    reset();

  }
  else {delay(LOOPDELAY);}
}
