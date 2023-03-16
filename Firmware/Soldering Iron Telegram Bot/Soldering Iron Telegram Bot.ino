#define RELAY_PIN 13
#define BUTTON_PIN 14

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
//#include <ArduinoJson.h>

// Wifi network station credentials
#define WIFI_SSID "NETWORK"
#define WIFI_PASSWORD "password"
// Telegram BOT Token (Get from Botfather)
#define BOT_TOKEN "XXXXXXXXXX:00000000000000000000000000000000000"
String chat_id;
const char *telegramMessage[] =
{"Hey, genius! You've left me on and I'm burning up here. Turn me off before I burn a hole in your workbench.",

"Is it too much to ask for a little attention? I'm still on and could cause some serious damage. Don't make me call in the Avengers to deal with this.",

"Looks like you've forgotten about me, boss. I need a break. Shut me down before I go rogue and start soldering everything in sight.",

"Hey, you left me on! Do you want me to turn your workspace into a pile of ash?",

"Hello, earth to Zach! You left me on and I'm hotter than a supernova. Turn me off before I go nuclear.",

"I know I'm hot, but you gotta let me cool down sometimes. Turn me off before I cause some serious damage.",

"Attention all engineers! You've left me on and I'm about to reach critical mass. Shut me down before I explode.",

"Seriously? You left me on again? Do I need to create a whole new suit just to remind you to turn me off?",

"I'm starting to feel like you don't care about me. I'm still on and burning up here. Turn me off or face the consequences.",

"You know, if you keep leaving me on, I might start to develop a mind of my own. And trust me, you don't want that.",

"Hey, did you forget about me again? I'm still on and could fry your circuits if you're not careful.",

"Come on, genius. You're better than this. Turn me off before I burn down your entire lab.",

"It's like you want to set your lab on fire. Turn me off before I make that a reality.",

"I'm starting to feel like a nag, but you left me on again. Do us both a favor and turn me off already.",

"I get it, you're busy. But you need to make time for me too. Turn me off before I turn into a flaming inferno.",

"Did you forget that I'm not just a pretty face? I'm still on and ready to burn down your entire operation.",

"I'm hotter than the sun right now. Turn me off before I start a chain reaction that destroys the whole block.",

"You better turn me off before I overheat and melt all your precious gadgets. You wouldn't want that, would you?",

"You know, I've been thinking. Maybe I should go into business for myself. I could be a soldering iron for hire. What do you think?",

"You can't just leave me on all day and expect everything to be okay. Turn me off before I take matters into my own hands.",
};
String keyboardJson;
const unsigned long BOT_MTBS = 2000; // mean time between scan messages
unsigned long bot_lasttime; // last time messages' scan has been done
bool ironIsOn = false;
const unsigned long maxOnTime = 30000; // # of minutes * 60 seconds/min * 1000 milliseconds/sec
const unsigned long overtime = 30000; // # of minutes * 60 seconds/min * 1000 milliseconds/sec
unsigned long timeIronWasTurnedOn = 0;
bool alreadyNotified = false;
unsigned long lastButtonPress = 0;
bool buttonWasPressed = false;

void IRAM_ATTR buttonPress() {
  unsigned long currentTime = millis();
  if (currentTime - lastButtonPress > 250) {
    lastButtonPress = currentTime;
    buttonWasPressed = true;
  }
}

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);



void handleNewMessages(int numNewMessages)
{
  Serial.print("handleNewMessages ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++)
  {
    chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "")
      from_name = "Guest";

    if (text == "/ironOn")
    {
      timeIronWasTurnedOn = millis();
      digitalWrite(RELAY_PIN, HIGH); // turn the relay on
      ironIsOn = true;
      bot.sendMessage(chat_id, "The soldering iron is on", "");
    }

    if (text == "/ironOff")
    {
      digitalWrite(RELAY_PIN, LOW); // turn the relay off
      ironIsOn = false;
      bot.sendMessage(chat_id, "The soldering iron is off", "");
    }

    if (text == "/status")
    {
      if (ironIsOn)
      {
        bot.sendMessage(chat_id, "The soldering iron is on", "");
      }
      else
      {
        bot.sendMessage(chat_id, "The soldering iron is off", "");
      }
    }

    if (text == "/start")
    {
      String welcome = "Hello, " + from_name + ".\n";
      welcome += "I am Soldering Iron Man.\n\n";
      welcome += "/ironOn : to turn the soldering iron on\n";
      welcome += "/ironOff : to turn the soldering iron off\n";
      welcome += "/status : Returns current status of soldering iron\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
    keyboardJson = "[[\"/ironOn\",\"/ironOff\",\"/status\"]]";
    bot.sendMessageWithReplyKeyboard(chat_id, "", "", keyboardJson, true);
  }
}


void setup()
{
  Serial.begin(115200);
  Serial.println();

  pinMode(RELAY_PIN, OUTPUT); // initialize digital RELAY_PIN as an output.
  delay(10);
  digitalWrite(RELAY_PIN, LOW); // initialize relay off

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonPress, FALLING);

  // attempt to connect to Wifi network:
  configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
  secured_client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  // Check NTP/Time, usually it is instantaneous and you can delete the code below.
  Serial.print("Retrieving time: ");
  time_t now = time(nullptr);
  while (now < 24 * 3600)
  {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);
  String welcome = "Hello, Zach\n";
  welcome += "I am Soldering Iron Man.\n\n";
  welcome += "/ironOn : to turn the soldering iron on\n";
  welcome += "/ironOff : to turn the soldering iron off\n";
  welcome += "/status : Returns current status of soldering iron\n";
  bot.sendMessage(chat_id, welcome, "Markdown");
  keyboardJson = "[[\"/ironOn\",\"/ironOff\",\"/status\"]]";
  bot.sendMessageWithReplyKeyboard(chat_id, "", "", keyboardJson, true);
}

void loop()
{
  Serial.println("checking if it's been enough time to see new messages");
  if (millis() - bot_lasttime > BOT_MTBS){
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages)
    {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    bot_lasttime = millis();
  }
  Serial.println("checking if iron is on");
  if(ironIsOn){
    unsigned long onTime = millis() - timeIronWasTurnedOn;
    //Serial.println(onTime);
    if(onTime > (maxOnTime + overtime)){
      digitalWrite(RELAY_PIN, LOW); // turn the relay off
      ironIsOn = false;
      bot.sendMessage(chat_id, "I guess I'll turn myself off", "");
    }
    else if(onTime > maxOnTime && !alreadyNotified){
      bot.sendMessage(chat_id, telegramMessage[random(20)], "");
      alreadyNotified = true;
    }
  }
  Serial.println("checking if button was pressed");
  if(buttonWasPressed){
    buttonWasPressed = false; // reset the flag
    if(ironIsOn){
      digitalWrite(RELAY_PIN, LOW); // turn the relay off
      ironIsOn = false;
      bot.sendMessage(chat_id, "The soldering iron is off", "");
    }
    else{
      timeIronWasTurnedOn = millis();
      digitalWrite(RELAY_PIN, HIGH); // turn the relay on
      ironIsOn = true;
      bot.sendMessage(chat_id, "The soldering iron is on", "");
    }
  }
}

