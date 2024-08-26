#include<LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <Wire.h>
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

SoftwareSerial gsm_g(2, 3);

#define splash splash1

#define acl A0
#define key1 A1
#define key2 A2
#define key3 A3
#define buz 4


String latout = "12.8749694", lonout = "80.2253954"; //12.8749694,80.2253954
String smsmsg, gpsval, text;
string usernum = "+919360607932";
int C_status;
int c_tim = 10;
int Comp_tim = 60, comoff_s;
float cc_tim = c_tim;
float em_tim, en_tim;
const int ACC = 0x68; //I2C address of the MPU-6050
unsigned int AcX, AcY, AcZ;
int fStatus, f_Status;

int acl_Status, key_Status;
int acl_r, k1_r, k2_r, k3_r;
int alert_Status;



void setup() {
    //put your setup code here, to run once:
    Serial.begin(9600);
    pinMode(key1, INPUT_PULLUP);
    pinMode(key2, INPUT_PULLUP);
    pinMode(key3, INPUT_PULLUP);
    pinMode(acl, INPUT);
    pinMode(key2, INPUT_PULLUP);
    pinMode(key3, INPUT_PULLUP);
    pinMode(acl, INPUT);
    pinMode(buz, OUTPUT);


    gsm_g.begin(9600);
    Wire.begin();
    Wire.beginTransmission(0x68);
    Wire.write(0x6B); //PWR_MGMT_1 register
    Wire.write(0); //set to zero (wakes upp the MPU-6050)
    Wire.endTransmission(true);
    LcDSet();
      SendSms( usernum, "GSM Initialized!!!", "", "Sending SMS.....");


}
void LcDSet() {

    lcd.begin(16, 2);
    splash(0, "Two Wheeler");
    splash(1, "Safety");

    delay(2000);
    lcd.clear();

}

void(* resetFunc) (void) = 0;
void loop() {
    //put your main code here, to run repeatedly://<500>800
    f_Status = getAcc();

    acl_r = digitalRead(acl);
    k1_r = digitalRead(key1);
    k2_r = digitalRead(key2);
    k3_r = digitalRead(key3);
    Serial.println(acl_r);
    if (k1_r == 0 and k2_r == 0) {
        key_Status = 1;

        Serial.println("Engine started");
    }
    else {
        key_Status = 0;
    }
    if (!acl_r) {
        acl_Status = 1;
    } else {
        acl_Status = 0;
    }
    if (acl_Status == 1) {
        splash (0, "Alcohol");
        splash (1, "Detected");
    }
    else if (key_Status == 0 and en_tim == 0) {

        splash(0, "Please Wear");
        splash(1, "Your Helmet");
    }
    else if (key_Status == 1 and en_tim == 0) {

        //analogWrite(eng, 250);

        splash(0, "Engine");
        splash(1, "started");
        delay(1000);
        splash(0, "Have A");
        splash(1, "Safe Ride");
        if (f_Status == 1) {
            splash(0, "Fall Detected");
            digitalWrite(buz, HIGH);

            splash(0, "");
            en_tim = 1;
            delay(100);

        }
    }
    if (k3_r == 0) {
        splash(0, "Glad You're");
        splash(1, "Safe");
        en_tim = 0;
        em_tim = 0;

        digitalWrite(buz, LOW);
        delay(1000);
        resetFunc();
    }
    if (en_tim == 1) {
        
        lcd.setCursor(0, 1);
        lcd.print("              ");
        lcd.setCursor(12, 1);
        lcd.print(em_tim);
        em_tim += 0.20;
        if(em_tim > c_tim) {
            alert("Accident Detected");
            em_tim = 0;
            c_tim = 60;
            alert_Status = 0;
        }
    }
    delay(100);
}
void alert(String msg) {
    if (alert_Status == 0) {

        gpsval = " https://maps.google.com/maps?q=";
        gpsval += latout;
        gpsval += ",";
        gpsval += lonout;
        smsmsg = "Emergency";
        smsmsg += msg;
        Serial.println(smsmsg + gpsval);
        SendSms( usernum, smsmsg, gpsval, "Sending SMS......");
        alert_Status = 1;
    }
}
int getAcc() {
    Wire.beginTransmission(Acc);
    Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(ACC, 14, true); // request a total of 14 registers
    AcX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
    AcY = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
    AcZ = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)




    if (AcX > 49000 && AcX < 58000)
    {
        fStatus = 1;

        delay(20);

    }
    else if (AcX > 7000 && AcX <15000)
    {
        fStatus = 1;
        delay(20);

    }
    else if (AcY > 7000 && AcY < 15000)
    {
        fStatus = 1;
        delay(20);
    }
    else if (AcY > 49000 && AcY < 57000)
    {
        fStatus = 1;
        delay(20);
    }
    else {
        fStatus = 0;
        delay(20);
    }
    Serial.print("X = "); Serial.print(AcX);
    Serial.print(" | Y = "); Serial.println(AcY);

    return fStatus;
    delay(333);
}

void SendSms( String NuM, string MsG, String gps, String LcD)
{
    lcd.setCursor(0, 0);
    lcd.print(LcD);
    lcd.setCursor(0, 1);
    lcd.print(".");
    gsm_g.println("AT");
    lcd.print(".");
    delay(1000);
    lcd.print(".");
    gsm_g.println("AT+CMGF=1");  //Sets the gsm_g Module in Text Mode
    lcd.print(".");
    delay(1000);
    lcd.print("."); // Delay of 1000 milli seconds or 1 second
    gsm_g.println("AT+CMGS=\"" + NuM + "\"\r");
    lcd.print(".");
    delay(1000);
    lcd.print(".");
    // gsm_g.print("Hello");
    gsm_g.print(MsG);
    gsm_g.print(gps);
    delay(1000);
    lcd.print(".");
    gsm_g.println((char)26);
    lcd.print(".");
    delay(1000);
    lcd.print(".");
    delay(1000);
    lcd.print(".");
    delay(1000);
    lcd.print(".");
    delay(1000);
    lcd.print(".");
    delay(1000);
    lcd.print(".");
    delay(1000);
    lcd.print(".");
    delay(1000);
    lcd.print(".");
    delay(1000);
    lcd.print(".");
    lcd.clear();
    
}