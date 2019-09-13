#define PIN_BUZZER 5
#define PIN_RF_TX 3
#define PORT_RF_TX PORTD
#define PIN_RF_RX 2
#define PORT_RF_RX PIND
#define PIN_BUTTON_LEFT 0
#define PIN_BUTTON_CANCEL 1
#define PIN_BUTTON_OK 2
#define PIN_BUTTON_RIGHT 3
#define BUZZER_LOW_VOLUME 24464
#define BUZZER_MEDIUM_VOLUME 300
#define BUZZER_HIGH_VOLUME 5000

#define TASK_UPDATE_LCD 4

#define KEY_LENGTH 92
#define KEY_CRC_LENGTH 1
#define DEBUGGING 1
#define REPEAT_MSG_TIMES 2
#define SHUTTER_CICLETIME 20000

#define ADDRESS_I2C_LCD 0x3F
#define LCD_CHAR_ARROWDOWN byte(0)
#define LCD_CHAR_ARROWUP byte(1)
#define LCD_CHAR_ARROWLEFT byte(2)
#define LCD_CHAR_ARROWRIGHT byte(3)
#define LCD_CHAR_STOP byte(4)


struct Tcommand {
  byte Key [KEY_LENGTH];
  byte CRC;
};
