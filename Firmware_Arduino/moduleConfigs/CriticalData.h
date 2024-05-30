#ifndef __CRITICAL_DATA_H_INCLUDED__
#define __CRITICAL_DATA_H_INCLUDED__

#define REMOTE_PREAMBLE_PATTERN                                              (1)
#define REMOTE_INTERFRAME_PATTERN                                            (1)
#define REMOTE_PREAMBLE_LENGTH                                               (1)
#define REMOTE_PACKET_LENGTH                                                 (1)
#define REMOTE_SYMBOL_US                                                     (1)

const byte KeyCommands[3][3][REMOTE_PACKET_LENGTH] PROGMEM = {
    { /* COMANDO_STOP_LEFT    */ { 0xFF },
     /* COMANDO_UP_LEFT      */ { 0xFF },
     /* COMANDO_DOWN_LEFT    */ { 0xFF } },
    { /* COMANDO_STOP_CENTER  */ { 0xFF },
     /* COMANDO_UP_CENTER    */ { 0xFF },
     /* COMANDO_DOWN_CENTER  */ { 0xFF } },
    { /* COMANDO_STOP_RIGHT   */ { 0xFF },
     /* COMANDO_UP_RIGHT     */ { 0xFF },
     /* COMANDO_DOWN_RIGHT   */ { 0xFF } }
};

#endif /* __CRITICAL_DATA_H_INCLUDED__ */
