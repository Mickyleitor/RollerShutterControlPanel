#include <TimeLib.h>
#include <ListLib.h>
#include <TimeAlarms.h>
#include <EEPROM.h>

// single character message tags
#define TIME_HEADER   'T'   // Header tag for serial time sync message
#define SHUTTER_HEADER 'C'
#define TIME_REQUEST  7     // ASCII bell character requests a time sync message 
#define COURSETIME 27
#define sleep(x) Alarm.delay(x);
#define USE_SPECIALIST_METHODS

bool StandyMode = true;
String lcdContentUpper;
String lcdContentBottom;

struct TShutter {
  uint8_t currentLevel = 0;
  uint8_t targetLevel = 0;
  
} Shutter[3];

struct TAlarm {
  uint8_t idAlarm = 0;
  uint8_t num_shutter;
};

List<TAlarm> ProgrammedAlarms;

void setup()  {
  Serial.begin(9600);
  while (!Serial) ; // Needed for Leonardo only
  setSyncProvider(requestSync);

  printMsg("*** Starting ***","");
}

void loop(){
  if (Serial.available() > 1) {
    char c = Serial.read();
    if( c == TIME_HEADER) {
      processSyncMessage();
    }
    if( c == SHUTTER_HEADER) {
      processControlMessage();
    }
  }
  Alarm.delay(0);
}

void showTime(){
  if (timeStatus()!= timeNotSet) {
    printMsg("Hora",getMsgFromTime(now()));
  }
}

void processSyncMessage() {
  unsigned long pctime;
  const unsigned long DEFAULT_TIME = 0; // Jan 1 2013 - paul, perhaps we define in time.h?

   pctime = Serial.parseInt();
   if( pctime >= DEFAULT_TIME) { // check the integer is a valid time (greater than Jan 1 2013)
     setTime(pctime); // Sync Arduino clock to the time received on the serial port
     printMsg("New saved time",getMsgFromTime(now()));
     Alarm.timerRepeat(1,showTime);
   }
}

void processControlMessage() {
    int shutter = int(Serial.read());
    int level = Serial.parseInt();
    if((level >= 0) && (level <= 100)){
      printWarningMsg(String ("Shutter " + shutter),String ("Going to level " + level),2);
      moveShutterToPosition(shutter,level);
    }else{
      printWarningMsg(String ("Incorrect level"),"",2);
    }
}

void moveShutterToPosition(uint8_t num_shutter, uint8_t level){
  if((Shutter[num_shutter].targetLevel == Shutter[num_shutter].currentLevel)&&((level >= 0) && (level <= 100))){
    uint8_t targetTime = abs(level-Shutter[num_shutter].currentLevel+1)*0.01*COURSETIME;
    Shutter[num_shutter].targetLevel = level;
    sendPulseRelay(num_shutter);
    TAlarm NewAlarm;
    NewAlarm.num_shutter = num_shutter;
    NewAlarm.idAlarm = Alarm.timerOnce(targetTime
    
  }
}

time_t requestSync() {
  printMsg("Sincronizar","Reloj");
  return 0; // the time will be sent later in response to serial mesg
}

void printWarningMsg(const String text1,const String text2,const uint8_t timedelay_s){
  Serial.println(alignTextToCenter(text1));
  Serial.println(alignTextToCenter(text2));
  Alarm.timerOnce(timedelay_s,updateMsg);
}

void updateMsg(){
  Serial.println(lcdContentUpper);
  Serial.println(lcdContentBottom);
}

void printMsg(const String text1,const String text2){
  lcdContentUpper = alignTextToCenter(text1);
  lcdContentBottom = alignTextToCenter(text2);
  updateMsg();
}

String alignTextToCenter(const String text){
  String alignedText = "";
  int i;
  for(i=0;i<((16 - text.length())/2);i++){
    alignedText.concat(" ");
  }
  alignedText.concat(text);
  for(i=0;i<((16 - text.length())/2);i++){
    alignedText.concat(" ");
  }
  return alignedText;
}

String getMsgFromTime (uint32_t Time){
    String msg = "";
    if(((Time / 3600)%24) < 10) msg.concat('0');
    msg.concat((Time / 3600)%24);
    msg.concat(':');
    if(((Time / 60)%60) < 10) msg.concat('0');
    msg.concat((Time / 60)%60);
    msg.concat(':');
    if((Time % 60) < 10) msg.concat('0');
    msg.concat(Time%60);
    return msg;
}
