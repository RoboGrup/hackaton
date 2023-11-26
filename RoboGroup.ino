#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>   
#include <ArduinoJson.h>
#include <DHT.h>
#define DHTPIN 4     // DHT Porti
#define DHTTYPE DHT11   

DHT dht(DHTPIN, DHTTYPE);

#define TN A0

// wi-fi moduli ulanishi uchun
const char* ssid = "304-xona AASH";// Wi-Fi nomi
const char* password = "331112005";// Wi-Fi Paroli

#define BOTtoken "6685831201:AAGldlTWRrri5dsVMxJb3QY_X3IZwdYj39Y"  // Bot Tokeni

#define CHAT_ID "1404006656" //Admin ID

#ifdef ESP8266
  X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

float temperature, humidity,TNamlik;
byte On = 0;
String V; 
int vaqt;

void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Siz botdan foydalana olmaysiz, chunki admin emasiz.", "");
      continue;
    }
    
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "Assalomu alaykum, " + from_name + ".\n";
      welcome += "Issiqxonani boshqarishingiz uchun quidagi buyuruqlardan foydalaning.\n\n";
      welcome += "/on Kullerni yoqish uchun \n";
      welcome += "/off Kullerni o'chirish uchun \n";
      welcome += "/holat Issiqxona holatini so'rash uchun \n";
      bot.sendMessage(chat_id, welcome, "");
    }

    if (text == "/holat") {
      delay(100);
      V = "Temperature: "+String(temperature,1)+" C,\n"+"Havo namligi: "+String(humidity,1)+" %,\n"+"Tuproq namligi: "+String(TNamlik,1)+" %\n";
      if(On == 2){
        V += "Rele - ON (majburiy)"; 
        bot.sendMessage(chat_id, V, "");
      }
      if(On == 1){
        V += "Rele - ON"; 
        bot.sendMessage(chat_id, V, "");
      }
      if(On == 0){
        V += "Rele - OFF"; 
        bot.sendMessage(chat_id, V, "");
      }
    }

    if (text == "/on") {
      V = "Kuller majburiy ishhga tushdi.";
      On=2;
      bot.sendMessage(chat_id, V , ""); 
      digitalWrite(5, 1);
      digitalWrite(12,1);
      digitalWrite(13,0);
    }

    if (text == "/off") {
      V = "Kuller to'xtatildi";
      On=0;
      bot.sendMessage(chat_id, V , ""); 
      digitalWrite(5, 0);
      digitalWrite(12,0);
      digitalWrite(13,1);
    }

  }
}

void setup() {
  dht.begin();
  pinMode(5,OUTPUT); //rele
  pinMode(12,OUTPUT); //yashil
  pinMode(13,OUTPUT); //qizil

  digitalWrite(13,1);
  Serial.begin(115200);

  #ifdef ESP8266
    configTime(0, 0, "pool.ntp.org");
    client.setTrustAnchors(&cert); 
  #endif
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  #ifdef ESP32
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  #endif
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Wi-Fi qidirilmoqda...");
  }
  Serial.println(WiFi.localIP());
}

void loop() {
  
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }

      temperature = dht.readTemperature(); // Havo harorati
      humidity = dht.readHumidity(); // Havo namligi
      TNamlik = analogRead(TN); //Tuproq namligi
     // Serial.println(TNamlik);
      TNamlik = map(TNamlik,957,0,0,100); //Qiymatni 0 dan 100 gacha qilib olamiz

if(temperature >= 30 and On == 0){
    digitalWrite(5,1);
    digitalWrite(12,1);
    digitalWrite(13,0);
    bot.sendMessage(CHAT_ID, "Rele avtomatik yoqildi", "");
    On = 1;
  }
  if(temperature <= 25 and On == 1){
    digitalWrite(5,0);
    digitalWrite(12,0);
    digitalWrite(13,1);
    bot.sendMessage(CHAT_ID, "Rele avtomatik o'chirildi", "");
    On = 0;
  }

  if(temperature <= 15 and On == 2){
    digitalWrite(5,0);
    digitalWrite(12,0);
    digitalWrite(13,1);
    bot.sendMessage(CHAT_ID, "Rele avtomatik o'chirildi", "");
    On = 0;
  }
  
}