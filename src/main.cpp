#include <Arduino.h>
#include <Wire.h>
String gsm_send_serial(String command, int delay);

#define TINY_GSM_MODEM_SIM7600
#define SerialMon Serial
#define SerialAT Serial1
#define GSM_PIN ""

// Your GPRS credentials, if any
// const char apn[]      = "YourAPN";
const char apn[] = "cmnet";
const char gprsUser[] = "";
const char gprsPass[] = "";
// MQTT details
// const char *broker = "broker.hivemq.com";
String broker = "broker.emqx.io";
String MQTTport="8883";
// const char *topicLed = "GsmClientTest/led";
// const char *topicInit = "GsmClientTest/init";
// const char *topicLedStatus = "GsmClientTest/ledStatus";


#define uS_TO_S_FACTOR 1000000ULL // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP 60          // Time ESP32 will go to sleep (in seconds)

#define UART_BAUD 115200
#define MODEM_DTR 25
#define MODEM_TX 27
#define MODEM_RX 26
#define MODEM_PWRKEY 4
#define MODEM_DTR 32
#define MODEM_RI 33
#define MODEM_FLIGHT 25
#define MODEM_STATUS 34

#define SD_MISO 2
#define SD_MOSI 15
#define SD_SCLK 14
#define SD_CS 13
#define LED_PIN 12

void Init(void)
{// Connecting with the newtwor and GPRS
    delay(5000);
    gsm_send_serial("AT+CFUN=1", 10000);
    gsm_send_serial("AT+CPIN?", 10000);
    gsm_send_serial("AT+CSQ", 1000);
    gsm_send_serial("AT+CREG?", 1000);
    gsm_send_serial("AT+COPS?", 1000);
    gsm_send_serial("AT+CGATT?", 1000);
    gsm_send_serial("AT+CPSI?", 500);
    gsm_send_serial("AT+CGDCONT=1,\"IP\",\"cmnet\"", 1000);
    gsm_send_serial("AT+CGACT=1,1", 1000);
    gsm_send_serial("AT+CGATT?", 1000);
    gsm_send_serial("AT+CGPADDR=1", 500);
    gsm_send_serial("AT+NETOPEN", 500);
    gsm_send_serial("AT+NETSTATE", 500);
}

void getMQTTS()
{
    gsm_send_serial("AT+CMQTTSTART", 1000);
    // gsm_send_serial("AT+CMQTTACCQ=0,\"client test0\",1");
    gsm_send_serial("AT+CMQTTACCQ=0,\"client test0\",1", 1000);
    gsm_send_serial("AT+CMQTTWILLTOPIC=0,10", 1000);
    gsm_send_serial("0123456789\x1A", 1000);
    // SerialAT.write(0X1A);
    gsm_send_serial("AT+CMQTTWILLMSG=0,6,1", 1000);
    gsm_send_serial("qwerty\x1A", 1000);
    // SerialAT.write(0X1A);
    gsm_send_serial("AT+CMQTTCONNECT=0,\"tcp://"+broker+":"+MQTTport+"\",60,1", 20000);
    gsm_send_serial("AT+CMQTTSUB=0,10,1", 1000);
    gsm_send_serial("simcomtest\x1A", 100);
    gsm_send_serial("AT+CMQTTTOPIC=0,10", 1000);
    gsm_send_serial("simcomtest\x1A", 1000);
    gsm_send_serial("AT+CMQTTPAYLOAD=0,9", 1000);
    gsm_send_serial("mqtt_test\x1A", 1000);
    gsm_send_serial("AT+CMQTTPUB=0,1,60", 1000);
}
String gsm_send_serial(String command, int delay)
{
    String buff_resp = "";
    Serial.println("Send ->: " + command);
    SerialAT.println(command);
    long wtimer = millis();
    while (wtimer + delay > millis())
    {
        while (SerialAT.available())
        {
            buff_resp = SerialAT.readString();
            Serial.println(buff_resp);
        }
    }
    Serial.println();
    return buff_resp;
}

void setup()
{
    // Set console baud rate
    Serial.begin(115200);
    delay(10);
    SerialAT.begin(UART_BAUD, SERIAL_8N1, MODEM_RX, MODEM_TX);
    // Set LED OFF
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);

    pinMode(MODEM_PWRKEY, OUTPUT);

    digitalWrite(MODEM_PWRKEY, HIGH);
    delay(1000);
    digitalWrite(MODEM_PWRKEY, LOW);
    delay(1000);
    digitalWrite(MODEM_PWRKEY, HIGH);

    /*
    MODEM_FLIGHT IO:25 Modulator flight mode control,
    need to enable modulator, this pin must be set to high
    */
    pinMode(MODEM_FLIGHT, OUTPUT);
    digitalWrite(MODEM_FLIGHT, HIGH);

    delay(2000);
    Init();
    getMQTTS();
    unsigned long prevMillis=0;
    while (1)
    {
        /* code */
         if (SerialAT.available())
        {
         
            Serial.println(SerialAT.readString());

        }
        if(millis()-prevMillis>5000)
        {



String payload=(String)(millis()/1000L);
//  gsm_send_serial("AT+CMQTTWILLTOPIC=0,10", 1000);
//     gsm_send_serial("0123456789\x1A", 1000);
//     // SerialAT.write(0X1A);
//     gsm_send_serial("AT+CMQTTWILLMSG=0,6,1", 1000);
//  gsm_send_serial("simcomtest\x1A", 100);
// gsm_send_serial("AT+CMQTTPAYLOAD=0,"+(String)(payload.length()), 1000);
//     gsm_send_serial(payload+"\x1A", 1000);
//       gsm_send_serial("AT+CMQTTPUB=0,1,60", 1000);
     // gsm_send_serial("AT+CMQTTPUB=0,0,60,\"simcomtest\",\"" + (String)"hello" + "\"",1000);
  gsm_send_serial("AT+CMQTTTOPIC=0,10", 1000);
    gsm_send_serial("simcomtest\x1A", 1000);
    gsm_send_serial("AT+CMQTTPAYLOAD=0,"+(String)(payload.length()), 1000);
    gsm_send_serial(payload+(String)"\x1A", 1000);
    gsm_send_serial("AT+CMQTTPUB=0,1,60", 1000);
      prevMillis=millis();
        }
    }
}

void loop()
{
}