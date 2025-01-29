
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>

constexpr uint8_t RST_PIN = 9;        //Refer to the above pin layout
constexpr uint8_t SS_PIN = 10;         //Refer to the above pin layout
MFRC522 rfid(SS_PIN, RST_PIN);         //Instance of the class
MFRC522::MIFARE_Key key;
String tag;
int i = 0;

char KEY;
#include <Keypad.h>
const byte ROWS = 4;
const byte COLS = 3;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
char customKey;
byte rowPins[ROWS] = {8, 7, 6, 5};
byte colPins[COLS] = {4, A1, A2};
Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

#include <Adafruit_Fingerprint.h>

SoftwareSerial mySerial(2, 3);

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

int IR_Sensor = A0;
int IR_result;

int buzzer = A3;

int count1 = 0;
int count2 = 0;

String password = "2356";
String mypassword;
int counter = 0;

void setup()
{
  lcd.init();               // initialize the lcd
  lcd.clear();
  lcd.backlight();
  SPI.begin();                      //Initialise SPI Bus
  rfid.PCD_Init();
  pinMode(buzzer, OUTPUT);
  Serial.begin(9600);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit finger detect test");

  // set the data rate for the sensor serial port
  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) {
      delay(1);
    }
  }

  finger.getTemplateCount();
  Serial.print("Sensor contains ");
  Serial.print(finger.templateCount);
  Serial.println(" templates");
  Serial.println("Waiting for valid finger...");
}





void loop()                     // run over and over again
{

  IR_result = digitalRead(IR_Sensor);
  Serial.print("IR sensor:");
  Serial.println(IR_result);

  if (IR_result == 0)
  {
    lcd.setCursor(0, 0);
    lcd.print("Person Detected");

    delay(1500);
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Scan Finger");

    while ( count1 != 1)
    {
      getFingerprintIDez();
      getFingerprintID();
    }

    lcd.setCursor(0, 0);
    lcd.print("Scan RFID Tag");

    while (count2 != 1)
    {
      RFID_Scanner();
    }


    lcd.setCursor(0, 0);
    lcd.print("Enter Password");
    Password();

    count1 = 0;
    count2 = 0;

  }


}


void SMSSend1()
{
  Serial.println("AT+CMGF=1");
  delay(1000);
  Serial.println("AT+CMGS=\"+91918828538\"\r");
  delay(1000);
  Serial.println("Finger Print not Match");
  delay(200);
  Serial.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
}


uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      //Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      Serial.println("Could not find fingerprint features");


      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    digitalWrite(buzzer, HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Finger Not Match");
    SMSSend1();
    delay(1500);
    lcd.clear();
    noTone(buzzer);
    return p;
  } else {
    Serial.println("Unknown error");
    lcd.setCursor(0, 0);
    lcd.print("Finger Not Match");
    delay(1500);
    lcd.clear();
    return p;
  }


  // found a match!
  Serial.print("Found ID #");
  Serial.print(finger.fingerID);


  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);


  if ((finger.fingerID == 1))
  {
    count1++;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Finger Match");
    delay(1500);
    lcd.clear();
    Serial.println("finger Print found 1");

  }

  return finger.fingerID;
}



void RFID_Scanner()
{

  if ( ! rfid.PICC_IsNewCardPresent())
    return;
  if (rfid.PICC_ReadCardSerial()) {
    for (int i = 0; i < 4; i++) {
      tag += rfid.uid.uidByte[i];

    }

    Serial.print("tag: ");

    Serial.println(tag);
    if (tag == "13017622")
    {
      lcd.clear();
      count2++;
      Serial.println(tag);
      Serial.println("Card is accessed");
      lcd.setCursor(0, 0);
      lcd.print("RFID Match");

      delay(1500);
      lcd.clear();


    }

    else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("RFID not Match");
      Serial.println("Unknown Tag Detected");
      Serial.println("Access Denied!");
      delay(1500);
      lcd.clear();

    }
    tag = "";
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
}



void Password()
{
char key;
  while (key != '#')
  {
     key = keypad.getKey();

    if (key) {

      Serial.println("enter opt");
      Serial.println(key);
      counter = counter + 1;
      delay(1000);
      lcd.clear();
      lcd.setCursor(counter, 1);
      lcd.print("*");
    }
    if (key == '1')
    {

      mypassword = mypassword + 1;
    }

    if (key == '2')
    {

      mypassword = mypassword + 2;
    }

    if (key == '3')
    {

      mypassword = mypassword + 3;
    }

    if (key == '4')
    {

      mypassword = mypassword + 4;
    }

    if (key == '5')
    {

      mypassword = mypassword + 5;
    }

    if (key == '6')
    {

      mypassword = mypassword + 6;
    }

    if (key == '7')
    {

      mypassword = mypassword + 7;
    }

    if (key == '8')
    {

      mypassword = mypassword + 8;
    }

    if (key == '9')
    {

      mypassword = mypassword + 9;
    }

    if (key == '0')
    {

      mypassword = mypassword + 0;
    }

  }



  if ( password == mypassword )
  {
    Serial.print("password correct");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Password Correct");
         lcd.setCursor(0,1);
       lcd.print("You can Access");
    delay(1500);
    lcd.clear();
  }
  else
  {
    digitalWrite(buzzer, HIGH);
    lcd.clear();
     lcd.setCursor(0,0);
    lcd.print("Password Not");
     lcd.setCursor(3,1);
       lcd.print("Correct");
    delay(1500);
    lcd.clear();
    noTone(buzzer);
  }

  mypassword = "";
  counter = 0;


}
