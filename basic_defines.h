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

#define KEY_LENGTH 92
#define KEY_CRC_LENGTH 1
#define DEBUGGING 1

#define ADDRESS_I2C_LCD 0x3F

struct Tcommand {
  byte Key [KEY_LENGTH];
  byte CRC;
};
