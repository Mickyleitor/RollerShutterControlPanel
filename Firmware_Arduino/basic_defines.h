// Macros for PIN and PORTs
#define PIN_BUZZER 5
#define PIN_RF_TX 3
#define PORT_RF_TX PORTD
#define PIN_RF_RX 2
#define PORT_RF_RX PIND
#define PIN_BUTTON_LEFT 0
#define PIN_BUTTON_CANCEL 1
#define PIN_BUTTON_OK 2
#define PIN_BUTTON_RIGHT 3

// Macros for buzzer volume selections
#define BUZZER_LOW_VOLUME 24464
#define BUZZER_MEDIUM_VOLUME 300
#define BUZZER_HIGH_VOLUME 5000

// Macros for Task handling
#define TASK_BUTTON_LEFT 0
#define TASK_BUTTON_CANCEL 1
#define TASK_BUTTON_OK 2
#define TASK_BUTTON_RIGHT 3
#define TASK_UPDATE_LCD 4
#define TASK_UPDATE_IDLE 5

// Macros for System FSM handling
#define SYSTEM_FSM_DIGITALCLOCK 0
#define SYSTEM_FSM_SHUTTER 1
#define SYSTEM_FSM_OPTIONS 2
#define SYSTEM_FSM_FUNC_SLEEP 3

// Misc
#define DEBUGGING 1
#define REPEAT_MSG_TIMES 2
#define SHUTTER_CICLETIME 20000

// Macros for LCD handing
#define ADDRESS_I2C_LCD 0x3F
#define LCD_CHAR_ARROWDOWN byte(0)
#define LCD_CHAR_ARROWUP byte(1)
#define LCD_CHAR_ARROWLEFT byte(2)
#define LCD_CHAR_ARROWRIGHT byte(3)
#define LCD_CHAR_STOP byte(4)
#define LCD_CHAR_DEGREE byte(5)
