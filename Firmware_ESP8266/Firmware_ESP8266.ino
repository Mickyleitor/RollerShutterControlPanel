#include <Ticker.h>
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <ESP8266WiFi.h>
#include <time.h>                       // time() ctime()
#include "wifidata.h" // No incluir en proyecto final

#define MYTZ TZ_Europe_Madrid
#define ADDRESS_I2C_LCD 0x3F
#define TO_IDLE_SECONDS 10
#define UPDATE_SCREEN_SECONDS 30

enum State {
  IDLE,
  SHUTTERMANAGER,
  SETTINGS,
  MODE_JOB_TASK,
  MODE_SLEEP_TASK,
  MODE_ON_SLEEP_TASK,
  MODE_OFF_SLEEP_TASK
} SystemState, SubSystemState ;

enum ButtonInput {
  NONE,
  LEFT,
  UP,
  DOWN,
  RIGHT
} currentButton;

enum MenuSeleccion {
  PERSIANA_IZQUIERDA,
  PERSIANA_CENTRAL,
  PERSIANA_DERECHA,
  OPTION
} seleccionMenu ;

Ticker flipper,Buttons;
LiquidCrystal_PCF8574 lcd(ADDRESS_I2C_LCD);
const char* ssid = default_ssid;
const char* password = default_password;
int timezone = 2;
int dst = 0;

void goIdleState(){ Serial.println("Going to IDLE"); SystemState = IDLE; };

void setup() {
  SystemState = IDLE;
  SubSystemState = MODE_JOB_TASK;
  seleccionMenu = PERSIANA_CENTRAL;
  currentButton = NONE;
  Serial.begin(115200);
  initLCDFunction();
  initWifiFunction();
  initTimeFunction();
  initButtonsFunction();
  sendAudioFeedback('C');
}

void loop(){
  // Serial.println(digitalRead(2));
  switch(SystemState) {
    case IDLE : {
      apagarBrilloPantalla();
      flipper.detach();
      flipper.attach(UPDATE_SCREEN_SECONDS,mostrarHoraPantalla);
      while(SystemState == IDLE){
        switch (buttonPressed()) {
          case LEFT : {
            seleccionarAnterior();
            SystemState = SHUTTERMANAGER;
            break;
          }
          case RIGHT : {
            seleccionarSiguiente();
            SystemState = SHUTTERMANAGER;
            break;
          }
          case UP : {
            subirPersianaSeleccionada();
            SystemState = SHUTTERMANAGER;
            break;
          }
          case DOWN : {
            bajarPersianaSeleccionada();
            SystemState = SHUTTERMANAGER;
            break;
          }
        }
      }
      encenderBrilloPantalla();
      break;
    }
    case SHUTTERMANAGER : {
      // setFunctionAfter(goIdleState,60);
      mostrarPersianaSeleccionada();
      while(SystemState == SHUTTERMANAGER){
        switch (buttonPressed()) {
          case LEFT : {
            seleccionarAnterior();
            if(seleccionMenu == OPTION) SystemState = SETTINGS;
            else mostrarPersianaSeleccionada();
            break;
          }
          case RIGHT : {
            seleccionarSiguiente();
            if(seleccionMenu == OPTION) SystemState = SETTINGS;
            else mostrarPersianaSeleccionada();
            break;
          }
          case UP : {
            subirPersianaSeleccionada();
            mostrarPersianaSeleccionada();
            break;
          }
          case DOWN : {
            bajarPersianaSeleccionada();
            mostrarPersianaSeleccionada();
            break;
          }
        }
      }
      break;
    }
    case SETTINGS : {
      // setFunctionAfter(goIdleState,60);
      actualizarMenuPantalla();
      while(SystemState == SETTINGS){
        switch (SubSystemState) {
          case MODE_JOB_TASK : {
            switch(buttonPressed()){
              case UP : {
                activarModoTrabajo();
                break;
              }
              case DOWN : {
                SubSystemState = MODE_ON_SLEEP_TASK;
                actualizarMenuPantalla();
                break;
              }
              case LEFT : {
                SystemState = SHUTTERMANAGER;
                seleccionMenu = PERSIANA_DERECHA;
                break;
              }
              case RIGHT : {
                SystemState = SHUTTERMANAGER;
                seleccionMenu = PERSIANA_IZQUIERDA;
                break;
              }
            }
            break;
          }
          case MODE_ON_SLEEP_TASK : {
            switch(buttonPressed()){
              case RIGHT : {
                procesoDesactivarTarea();
                break;
              }
              case DOWN : {
                SubSystemState = MODE_OFF_SLEEP_TASK;
                actualizarMenuPantalla();
                break;
              }
              case LEFT : {
                SystemState = SHUTTERMANAGER;
                SubSystemState = MODE_JOB_TASK;
                seleccionMenu = PERSIANA_DERECHA;
                break;
              }
              case UP : {
                SubSystemState = MODE_JOB_TASK;
                actualizarMenuPantalla();
                break;
              }
            }
            break;
          }
          case MODE_OFF_SLEEP_TASK : {
            switch(buttonPressed()){
              case RIGHT : {
                procesoActivarTarea();
                break;
              }
              case UP : {
                SubSystemState = MODE_ON_SLEEP_TASK;
                actualizarMenuPantalla();
                break;
              }
              case LEFT : {
                SystemState = SHUTTERMANAGER;
                SubSystemState = MODE_JOB_TASK;
                seleccionMenu = PERSIANA_DERECHA;
                break;
              }
            }
            break;
          }
        }
      }
      break;
    }
  }
}

void deBounceFunction(){
  currentButton = NONE;
  if(digitalRead(4) == LOW){
    sendAudioFeedback('A');
    currentButton = LEFT;
  }else if(digitalRead(5) == LOW){
    sendAudioFeedback('A');
    currentButton = DOWN;
  }else if(digitalRead(12) == LOW){
    sendAudioFeedback('A');
    currentButton = UP;
  }else if(digitalRead(13) == LOW){
    sendAudioFeedback('A');
    currentButton = RIGHT;
  }
  Buttons.detach();
}

void IRAM_ATTR isrButtons(){
  Buttons.detach();
  Buttons.attach_ms(100, deBounceFunction);
}

int buttonPressed(){
  int buttonPress = 0;
  while ( Serial.available() ) {
    char c = (char)Serial.read();
    if( c == '1' ) currentButton = LEFT;
    if( c == '2' ) currentButton = DOWN;
    if( c == '3' ) currentButton = UP;
    if( c == '4' ) currentButton = RIGHT;
  }
  if(currentButton != NONE){
    Serial.print("------- [DEBUG] PULSADO : ");
    Serial.println(currentButton);
    flipper.detach();
    flipper.attach(TO_IDLE_SECONDS, goIdleState);
  }
  buttonPress = currentButton;
  currentButton = NONE;
  return buttonPress;
}

void mostrarPersianaSeleccionada()
{
  if(seleccionMenu == PERSIANA_IZQUIERDA){
    lcd.clear(); lcd.home();
    lcd.print(" PERSIANA IZQDA ");
    lcd.setCursor(0,1);
    lcd.print("<   UP  DOWN   >");
    Serial.println(" PERSIANA IZQUIERDA SELECCIONADA ");
  }
  else if(seleccionMenu == PERSIANA_CENTRAL){
    lcd.clear(); lcd.home();
    lcd.print("PERSIANA CENTRAL");
    lcd.setCursor(0,1);
    lcd.print("<   UP  DOWN   >");
    Serial.println(" PERSIANA CENTRAL SELECCIONADA");
  }
  else if(seleccionMenu == PERSIANA_DERECHA){
    lcd.clear(); lcd.home();
    lcd.print("PERSIANA DERECHA");
    lcd.setCursor(0,1);
    lcd.print("<   UP  DOWN   >");
    Serial.println(" PERSIANA DERECHA SELECCIONADA ");
  }
  // else if(seleccionMenu == OPTION) Serial.println("OPCIONES [NO DEBERIA ACABAR AQUI]");
  
}

void seleccionarAnterior()
{
  switch (seleccionMenu) {
    case PERSIANA_IZQUIERDA : { seleccionMenu = OPTION; break; };
    case PERSIANA_CENTRAL : { seleccionMenu = PERSIANA_IZQUIERDA; break; };
    case PERSIANA_DERECHA : { seleccionMenu = PERSIANA_CENTRAL; break; };
    case OPTION : { seleccionMenu = PERSIANA_DERECHA; break; };
  }
}
void seleccionarSiguiente()
{
  switch (seleccionMenu) {
    case PERSIANA_IZQUIERDA : { seleccionMenu = PERSIANA_CENTRAL; break; };
    case PERSIANA_CENTRAL : { seleccionMenu = PERSIANA_DERECHA; break; };
    case PERSIANA_DERECHA : { seleccionMenu = OPTION; break; };
    case OPTION : { seleccionMenu = PERSIANA_IZQUIERDA; break; };
  }
}

void apagarBrilloPantalla(){ Serial.println("Brillo pantalla apagado"); lcd.setBacklight(0); };
void encenderBrilloPantalla(){ Serial.println("Brillo pantalla encendido"); lcd.setBacklight(255); };
void subirPersianaSeleccionada(){ Serial.print("Subiendo persiana : "); Serial.println(seleccionMenu); };
void bajarPersianaSeleccionada(){ Serial.print("Bajando persiana : "); Serial.println(seleccionMenu); };
void activarModoTrabajo(){ Serial.println("Modo trabajo activado"); sendAudioFeedback('C'); };
void procesoActivarTarea(){ Serial.println("Simular proceso activar Tarea"); sendAudioFeedback('B'); };
void procesoDesactivarTarea(){ Serial.println("Simular proceso Desactivar Tarea"); sendAudioFeedback('B'); };

void actualizarMenuPantalla()
{
  if(SubSystemState == MODE_JOB_TASK){
    lcd.clear(); lcd.home();
    lcd.print("  MODO TRABAJO  ");
    lcd.setCursor(0,1);
    lcd.print("<  SIG.   OK   >");
    Serial.println("Activar modo trabajo [UP]");
  }
  else if(SubSystemState == MODE_ON_SLEEP_TASK){
    lcd.clear(); lcd.home();
    lcd.print("ACT. MODO DORMIR");
    lcd.setCursor(0,1);
    lcd.print("<  SIG.  ANT. OK");
    Serial.println("Activar modo sueño [RIGHT]");
  }
  else if(SubSystemState == MODE_OFF_SLEEP_TASK){
    lcd.clear(); lcd.home();
    lcd.print("DES. MODO DORMIR");
    lcd.setCursor(0,1);
    lcd.print("<        ANT. OK");
    Serial.println("Desactivar modo sueño [RIGHT]");
  }
}

int initLCDFunction(){
  int status = -1;
  int failed = 0;
  // Search LCD into I2C line:
  while ( status != 0 ) {
    Wire.begin(2,14);
    Wire.beginTransmission(ADDRESS_I2C_LCD);
    status = Wire.endTransmission();

    if(failed > 5){
      return -1;
    }else if(status != 0){
      // wait 5 seconds for reconnection:
      delay(5000);
    }
    failed++;
  }
  
  lcd.begin(16,2);
  lcd.setBacklight(255);
  int customArrayChar[6][8] = {{0x00,0x00,0x1F,0x1F,0x0E,0x04,0x00,0x00},{0x00,0x00,0x04,0x0E,0x1F,0x1F,0x00,0x00},{0x00,0x00,0x06,0x0E,0x1E,0x0E,0x06,0x00},{0x00,0x00,0x0C,0x0E,0x0F,0x0E,0x0C,0x00},{0x00,0x0E,0x1F,0x11,0x11,0x1F,0x0E,0x00},{0x06,0x09,0x09,0x06,0x00,0x00,0x00,0x00}};
  for ( byte i = 0 ; i < 6 ; i ++ ) lcd.createChar(i,customArrayChar[i]);
  lcd.home(); lcd.clear();
  lcd.print("...INICIANDO...");
  return 0;
}

void mostrarHoraPantalla(){
  time_t now;
  struct tm * timeinfo;
  time(&now);
  timeinfo = localtime(&now);  
  lcd.clear();
  lcd.setCursor(1, 0);
  if ((timeinfo->tm_hour)<10) lcd.print('0');
  lcd.print((timeinfo->tm_hour), DEC);
  
  lcd.print(':');
  if ((timeinfo->tm_min)<10) lcd.print('0');
  lcd.print((timeinfo->tm_min), DEC);
  
  lcd.print(':');
  if ((timeinfo->tm_sec)<10) lcd.print('0');
  lcd.print((timeinfo->tm_sec), DEC);

  lcd.setCursor(0, 1);
  if ((timeinfo->tm_mday)<10) lcd.print('0');
  lcd.print((timeinfo->tm_mday), DEC);
  
  lcd.print('/');
  if ((timeinfo->tm_mon)<10) lcd.print('0');
  lcd.print((timeinfo->tm_mon), DEC);
  
  lcd.print('/');
  lcd.print((timeinfo->tm_year) + 1900, DEC);
  lcd.print(' ');
  
  lcd.setCursor(12, 1);
  int dayofweek = (timeinfo->tm_wday);
  switch(dayofweek){
    case 0:
      lcd.print("Dom.");
      break;
    case 1:
      lcd.print("Lun.");
      break;
    case 2:
      lcd.print("Mar.");
    break;
    case 3:
      lcd.print("Mie.");
      break;
    case 4:
      lcd.print("Jue.");
      break;
    case 5:
      lcd.print("Vie.");
      break;
    case 6:
      lcd.print("Sab.");
      break;
    default:
      lcd.print("Err.");
      break;
  }
  Serial.println("Hora pantalla actualizada");
}

void initWifiFunction(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("\nConnecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
}

void initTimeFunction(){
  configTime(timezone * 3600, dst * 0, "pool.ntp.org", "time.nist.gov");
  Serial.println("\nWaiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");
}

void initButtonsFunction(){
  pinMode(4,INPUT);
  pinMode(5,INPUT);
  pinMode(12,INPUT);
  pinMode(13,INPUT);
  attachInterrupt(4, isrButtons, FALLING);
  attachInterrupt(5, isrButtons, FALLING);
  attachInterrupt(12, isrButtons, FALLING);
  attachInterrupt(13, isrButtons, FALLING);
}

void sendAudioFeedback(char type){
  Wire.beginTransmission(8); // transmit to device #8
  Wire.write(type);        // sends five bytes
  Wire.endTransmission();    // stop transmitting
}
