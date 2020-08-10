// Domotic Shutter sketch master by Mickyleitor
// This sketch is in development and uses ESP13-WROOM-02.
// 
// Libraries used and need to be installed from Arduino IDE
// - Shield manager ESP8266 by ESP8266 Community version 2.6.3 : http://arduino.esp8266.com/stable/package_esp8266com_index.json
// - NTPClient by Fabrice Weinberg 3.2.0 : https://github.com/arduino-libraries/NTPClient
// - LiquidCrystal_PCF8574 by mathertel 1.2.0  : https://github.com/mathertel/LiquidCrystal_PCF8574
// - Time by Michael Margolis version 1.6.0  : https://github.com/PaulStoffregen/Time


#include <Ticker.h>
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <ESP8266WiFi.h>
#include <time.h>                       // time() ctime()
#include "wifidata.h" // No incluir en proyecto final

#define MYTZ TZ_Europe_Madrid
#define TO_IDLE_SECONDS 10
#define ADDRESS_I2C_LCD 0x3F
#define ADDRESS_I2C_SLAVE 0x8
#define UPDATE_SCREEN_SECONDS 1
#define DEBOUNCE_TIME_MILLIS 100
#define COMMAND_BUZZER_LOW_VOLUME 0x80
#define COMMAND_BUZZER_MEDIUM_VOLUME 0xA0
#define COMMAND_BUZZER_HIGH_VOLUME 0xC0

enum State {
  IDLE,
  SHUTTERMANAGER,
  MODE_JOB_TASK,
  MODE_SLEEP_TASK,
  MODE_ON_SLEEP_TASK,
  MODE_OFF_SLEEP_TASK
} SystemState ;

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

int ShutterStatus [3] = {0,0,0};

Ticker TimeOutTask,ButtonsISRTask;
const char* ssid = default_ssid;
const char* password = default_password;
int timezone = 2;
int dst = 0;
LiquidCrystal_PCF8574 lcd(ADDRESS_I2C_LCD);

void goIdleState(){ Serial.println("Buttons Time Out"); SystemState = IDLE; };

void setup() {
  SystemState = IDLE;
  seleccionMenu = PERSIANA_CENTRAL;
  currentButton = NONE;
  Serial.begin(115200);
  initLCDFunction();
  initWifiFunction();
  initTimeFunction();
  initButtonsFunction();
  checkSlaveConnection();
}

void loop(){
  // Serial.println(digitalRead(2));
  switch(SystemState) {
    case IDLE : {
      apagarBrilloPantalla();
      mostrarHoraPantalla();
      TimeOutTask.detach();
      TimeOutTask.attach(UPDATE_SCREEN_SECONDS,mostrarHoraPantalla);
      while(SystemState == IDLE){
        if(buttonPressed() != NONE){
          SystemState = SHUTTERMANAGER;
        }
        yield();
      }
      encenderBrilloPantalla();
      mostrarPersianaSeleccionada();
      break;
    }
    case SHUTTERMANAGER : {
      switch (buttonPressed()) {
        case LEFT : {
          seleccionarAnterior();
          if(seleccionMenu == OPTION){
            SystemState = MODE_JOB_TASK;
            actualizarMenuPantalla();
          }
          else mostrarPersianaSeleccionada();
          break;
        }
        case RIGHT : {
          seleccionarSiguiente();
          if(seleccionMenu == OPTION){
            SystemState = MODE_JOB_TASK;
            actualizarMenuPantalla();
          }
          else mostrarPersianaSeleccionada();
          break;
        }
        case UP : {
          if(ShutterStatus[seleccionMenu] != 0){
            PararPersianaSeleccionada();
          }else{
            subirPersianaSeleccionada();
          }
          mostrarPersianaSeleccionada();
          break;
        }
        case DOWN : {
          if(ShutterStatus[seleccionMenu] != 0){
            PararPersianaSeleccionada();
          }else{
            bajarPersianaSeleccionada();
          }
          mostrarPersianaSeleccionada();
          break;
        }
      }
      break;
    }
    case MODE_JOB_TASK : {
      switch(buttonPressed()){
        case UP : {
          activarModoTrabajo();
          break;
        }
        case DOWN : {
          SystemState = MODE_SLEEP_TASK;
          actualizarMenuPantalla();
          break;
        }
        case LEFT : {
          SystemState = SHUTTERMANAGER;
          seleccionMenu = PERSIANA_DERECHA;
          mostrarPersianaSeleccionada();
          break;
        }
        case RIGHT : {
          SystemState = SHUTTERMANAGER;
          seleccionMenu = PERSIANA_IZQUIERDA;
          mostrarPersianaSeleccionada();
          break;
        }
      }
      break;
    }
    case MODE_SLEEP_TASK : {
      switch(buttonPressed()){
        case LEFT : {
          SystemState = SHUTTERMANAGER;
          seleccionMenu = PERSIANA_DERECHA;
          mostrarPersianaSeleccionada();
          break;
        }
        case UP : {
          SystemState = MODE_JOB_TASK;
          actualizarMenuPantalla();
          break;
        }
        case RIGHT : {
          SystemState = MODE_ON_SLEEP_TASK;
          actualizarMenuPantalla();
          break;
        }
      }
      break;
    }
    case MODE_ON_SLEEP_TASK : {
      switch(buttonPressed()){
        case RIGHT : {
          procesoActivarTarea();
          actualizarMenuPantalla();
          break;
        }
        case DOWN : {
          SystemState = MODE_OFF_SLEEP_TASK;
          actualizarMenuPantalla();
          break;
        }
        case LEFT : {
          SystemState = MODE_SLEEP_TASK;
          actualizarMenuPantalla();
          break;
        }

      }
      break;
    }
    case MODE_OFF_SLEEP_TASK : {
      switch(buttonPressed()){
        case RIGHT : {
          procesoDesactivarTarea();
          actualizarMenuPantalla();
          break;
        }
        case UP : {
          SystemState = MODE_ON_SLEEP_TASK;
          actualizarMenuPantalla();
          break;
        }
        case LEFT : {
          SystemState = MODE_SLEEP_TASK;
          actualizarMenuPantalla();
          break;
        }

      }
      break;
    }
  }
  yield();
}

void deBounceFunction(){
  currentButton = NONE;
  if(digitalRead(4) == LOW){
    currentButton = LEFT;
  }else if(digitalRead(5) == LOW){
    currentButton = DOWN;
  }else if(digitalRead(12) == LOW){
    currentButton = UP;
  }else if(digitalRead(13) == LOW){
    currentButton = RIGHT;
  }
  ButtonsISRTask.detach();
}

void IRAM_ATTR isrButtons(){
  ButtonsISRTask.detach();
  ButtonsISRTask.attach_ms(DEBOUNCE_TIME_MILLIS, deBounceFunction);
}

int buttonPressed(){
  int buttonPress = 0;
  while ( Serial.available() ) {
    char c = (char)Serial.read();
    if( c == '1' ) currentButton = LEFT;
    if( c == '2' ) currentButton = UP;
    if( c == '3' ) currentButton = DOWN;
    if( c == '4' ) currentButton = RIGHT;
  }
  if(currentButton != NONE){
    Serial.print("------- [DEBUG] PULSADO : ");
    Serial.println(currentButton);
    TimeOutTask.detach();
    TimeOutTask.attach(TO_IDLE_SECONDS, goIdleState);
  }
  buttonPress = currentButton;
  currentButton = NONE;
  return buttonPress;
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

void apagarBrilloPantalla()  {  lcd.setBacklight(0);    };
void encenderBrilloPantalla(){  lcd.setBacklight(255);  };
void subirPersianaSeleccionada()
{
  ShutterStatus[seleccionMenu] = 1;
  sendRollerCommand(seleccionMenu,1);
};
void bajarPersianaSeleccionada()
{
   ShutterStatus[seleccionMenu] = 2;
   sendRollerCommand(seleccionMenu,2);
};
void PararPersianaSeleccionada()
{
   ShutterStatus[seleccionMenu] = 0;
   sendRollerCommand(seleccionMenu,0);
};

void activarModoTrabajo()
{ 
  Serial.println("Modo trabajo activado"); 
  sendAudioFeedback(COMMAND_BUZZER_MEDIUM_VOLUME); 
};
int procesoSeleccionarFecha(int & sday, int & smonth)
{
  int SleepTaskState = 0;
  bool FechaConfirmada = false;
  int daysOfMonths []= {31,28,31,30,31,30,31,31,30,31,30,31};
  while(SystemState == MODE_ON_SLEEP_TASK || SystemState == MODE_OFF_SLEEP_TASK)
  {
    switch(SleepTaskState) {
      case 0 : {
          lcd.clear(); lcd.home();
          lcd.print("SEL. FECHA ");
          lcd.setCursor(11,0);
          mostrarFechaPantalla(sday,smonth);
          lcd.setCursor(0,1);
          lcd.print("<    +    -  Mes");
          SleepTaskState = 1;
        break;
      }
      case 1 : {
        switch(buttonPressed()){
          case UP : {
            if( (sday+1) > daysOfMonths[smonth-1]){
              if(smonth == 12) smonth = 1;
              else smonth++;
              sday = 1;
            }else sday += 1;
            lcd.setCursor(11,0);
            mostrarFechaPantalla(sday,smonth);
            break;
          }
          case DOWN : {
            if( (sday-1) == 0){
              if(smonth == 1) smonth = 12;
              else smonth--;
              sday = daysOfMonths[smonth-1];
            }else sday -= 1;
            lcd.setCursor(11,0);
            mostrarFechaPantalla(sday,smonth);
            break;
          }
          case LEFT : {
            SystemState = MODE_SLEEP_TASK;
            break;
          }
          case RIGHT : {
            SleepTaskState = 2;
            lcd.setCursor(0,1);
            lcd.print("Dia  +    -   OK");
            break;
          }
        }
        break;
      }
      case 2 : {
        switch(buttonPressed()){
          case UP : {
            if(smonth == 12) smonth = 1;
            else smonth++;
            if(sday > daysOfMonths[smonth-1]) sday = daysOfMonths[smonth-1];
            lcd.setCursor(11,0);
            mostrarFechaPantalla(sday,smonth);
            break;
          }
          case DOWN : {
            if(smonth == 1) smonth = 12;
            else smonth--;
            if(sday > daysOfMonths[smonth-1]) sday = daysOfMonths[smonth-1];
            lcd.setCursor(11,0);
            mostrarFechaPantalla(sday,smonth);
            break;
          }
          case LEFT : {
            lcd.setCursor(0,1);
            lcd.print("<    +    -  Mes");
            SleepTaskState = 1;
            break;
          }
          case RIGHT : {
            lcd.clear(); lcd.home();
            lcd.print("CONFIRMAR  FECHA");
            lcd.setCursor(0,1);
            lcd.print("<    ");
            mostrarFechaPantalla(sday,smonth);
            lcd.print("    OK");
            SleepTaskState = 3;
            break;
          }
        }
        break;
      }
      case 3 : {
        switch(buttonPressed()){
          case LEFT : {
            SleepTaskState = 0;
            break;
          }
          case RIGHT : {
            // Guardar fecha
            FechaConfirmada = true;
            SystemState = MODE_SLEEP_TASK;
            break;
          }
        }
        break;
      }
    }
    yield();
  }
  return FechaConfirmada;
};

// Adjust date by a number of days +/-
void mostrarFechaPantalla( int day, int month){
    if (day<10) lcd.print('0');
    lcd.print(day, DEC);
    lcd.print("/");
    if (month<10) lcd.print('0');
    lcd.print(month, DEC);
    Serial.print(day);
    Serial.print(" - ");
    Serial.println(month);
}

void procesoDesactivarTarea(){ 
  int newday = 1,newmonth = 1;
  if(procesoSeleccionarFecha(newday,newmonth)){
    Serial.println("Desactivar tarea en fecha seleccionada");
    Serial.print(newday);
    Serial.print("-");
    Serial.println(newmonth);
  }
}

void procesoActivarTarea(){ 
  int newday = 1,newmonth = 1;
  if(procesoSeleccionarFecha(newday,newmonth)){
    Serial.println("Activar tarea en fecha seleccionada");
    Serial.print(newday);
    Serial.print("-");
    Serial.println(newmonth);
  }
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
  // Serial.println("Hora pantalla actualizada");
}

int mostrarPersianaSeleccionada(){
  if(seleccionMenu > 2) return -1;
  String namePersiana [] = {" PERSIANA IZQDA ","PERSIANA CENTRAL","PERSIANA DERECHA","  ERROR MENU  "};
  lcd.clear(); lcd.home();
  lcd.print(namePersiana[seleccionMenu]);
  lcd.setCursor(0,1);
  lcd.print("<");
  lcd.setCursor(5,1);
  if(ShutterStatus[seleccionMenu] == 1){
    lcd.write(4);
  }else{
    lcd.write(1);
  }
  lcd.setCursor(10,1);
  if(ShutterStatus[seleccionMenu] == 2){
    lcd.write(4);
  }else{
    lcd.write(2);
  }
  lcd.setCursor(15,1);
  lcd.print(">");
  Serial.println(namePersiana[seleccionMenu]);  
}

void actualizarMenuPantalla(){
  if(SystemState == MODE_JOB_TASK){
    lcd.clear(); lcd.home();
    lcd.print("  MODO TRABAJO  ");
    lcd.setCursor(0,1);
    lcd.print("<   OK   SIG.  >");
  }
  else if(SystemState == MODE_SLEEP_TASK){
    lcd.clear(); lcd.home();
    lcd.print("  MODO DORMIR   ");
    lcd.setCursor(0,1);
    lcd.print("<  ANT.       OK");
  }
  else if(SystemState == MODE_ON_SLEEP_TASK){
    lcd.clear(); lcd.home();
    lcd.print("ACT. MODO DORMIR");
    lcd.setCursor(0,1);
    lcd.print("<        SIG. OK");
  }
  else if(SystemState == MODE_OFF_SLEEP_TASK){
    lcd.clear(); lcd.home();
    lcd.print("DES. MODO DORMIR");
    lcd.setCursor(0,1);
    lcd.print("<  ANT.       OK");
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
  int customArrayChar[6][8] = 
  {
/* Flecha izquierda */  {0x00,0x07,0x0E,0x1C,0x1C,0x0E,0x07,0x00},
/* Flecha arriba    */  {0x00,0x04,0x0E,0x1F,0x1B,0x11,0x00,0x00},
/* Flecha abajo     */  {0x00,0x00,0x11,0x1B,0x1F,0x0E,0x04,0x00},
/* Flecha derecha   */  {0x00,0x1C,0x0E,0x07,0x07,0x0E,0x1C,0x00},
/* Flecha STOP      */  {0x00,0x0E,0x1B,0x11,0x11,0x1B,0x0E,0x00},
                        {0x06,0x09,0x09,0x06,0x00,0x00,0x00,0x00}
  };
  for ( byte i = 0 ; i < 6 ; i ++ ) lcd.createChar(i,customArrayChar[i]);
  lcd.home(); lcd.clear();
  lcd.print("...INICIANDO...");
  return 0;
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

void checkSlaveConnection(){
  bool slaveStatusRunning = false;
  Wire.requestFrom(ADDRESS_I2C_SLAVE, 1);
  unsigned long timeOut = millis();
  while(!slaveStatusRunning){
    if(Wire.available() > 0)
    {
      char c = Wire.read();
      if(c != 0x3) Wire.requestFrom(ADDRESS_I2C_SLAVE, 1);
      else slaveStatusRunning = true;
    }
    if( (millis() - timeOut) > 5000)
    {
      lcd.home(); lcd.clear();
      lcd.print(" ERROR CONEXION ");
      lcd.setCursor(0,1);
      lcd.print(" DISP. HARDWARE ");
      Wire.requestFrom(ADDRESS_I2C_SLAVE, 1);
      timeOut = millis();
    }
    yield();
  }
  for(int i = 0; i < 3; i++){ sendAudioFeedback(COMMAND_BUZZER_HIGH_VOLUME); delay(300); yield(); };
}

void sendAudioFeedback(char type){
  Wire.beginTransmission(ADDRESS_I2C_SLAVE); // transmit to device #8
  Wire.write(1);        // This should wake up the device
  Wire.write(type);        // sends five bytes
  Wire.endTransmission();    // stop transmitting
}

void sendRollerCommand(int persiana, int comando){
  char type = 1 << (persiana + 2) | (0x3  & comando);
  Wire.beginTransmission(ADDRESS_I2C_SLAVE); // transmit to device #8
  Wire.write(1);        // This should wake up the device
  Wire.write(type);        // sends five bytes
  Wire.endTransmission();    // stop transmitting
}
