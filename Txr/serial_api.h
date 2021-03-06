#ifndef SERIAL_API_H
#define SERIAL_API_H

#include "Arduino.h"

const int SERIAL_API_IN_BUFFER_SIZE         = 128;
const int SERIAL_API_OUT_BUFFER_SIZE        = 128;

struct serial_api_state_t {
    char in_buffer[SERIAL_API_IN_BUFFER_SIZE];
    char out_buffer[SERIAL_API_OUT_BUFFER_SIZE];
    int in_index;
    int out_index;
};

// Used letters - A B C D E F G I J L M N O P Q R S T U W X Y
// Unused letters - K Z


enum {
    SERIAL_ECHO                 = 'h',
    SERIAL_VERSION              = 'v',
    SERIAL_ROLE                 = 'r',
    SERIAL_REMOTE_VERSION       = 'w',
    SERIAL_REMOTE_ROLE          = 's',
    SERIAL_PRESET_INDEX_GET     = 'q',
    SERIAL_PRESET_INDEX_SET     = 'Q',
    SERIAL_ID_GET               = 'i',
    SERIAL_ID_SET               = 'I',
    SERIAL_NAME_GET             = 'n',
    SERIAL_NAME_SET             = 'N',
    SERIAL_CHANNEL_GET          = 'c',
    SERIAL_CHANNEL_SET          = 'C',
    SERIAL_REMOTE_CHANNEL_GET   = 'd',
    SERIAL_REMOTE_CHANNEL_SET   = 'D',
    SERIAL_START_STATE_GET      = 't',
    SERIAL_START_STATE_SET      = 'T',
    SERIAL_MAX_SPEED_GET        = 'm',
    SERIAL_MAX_SPEED_SET        = 'M',
    SERIAL_ACCEL_GET            = 'a',
    SERIAL_ACCEL_SET            = 'A',
    SERIAL_POT_GET              = 'p',
    SERIAL_ENCODER_GET          = 'e',
    SERIAL_LEDS                 = 'l',
    SERIAL_SAVE_CONFIG          = 'u',
    SERIAL_RELOAD_CONFIG        = 'x',
    SERIAL_TARGET_POSITION_GET  = 'o',
    SERIAL_TARGET_POSITION_SET  = 'O',
    SERIAL_EEPROM_IMPORT        = 'G',
    SERIAL_EEPROM_EXPORT        = 'g',
    SERIAL_DEBUG_FAIL_ASSERT    = 'B',
    SERIAL_DEBUG_STRING_GET     = 'b',
    SERIAL_FACTORY_RESET        = 'Y',
    SERIAL_IGNORE               = '_',
    SERIAL_CURRENT_LEVEL_GET    = 'f',
    SERIAL_CURRENT_LEVEL_SET    = 'F',
    SERIAL_MOTOR_DRIVER_GET     = 'j',
    SERIAL_MOTOR_DRIVER_SET     = 'J',    
};

struct serial_api_echo_command_t {
    char type;
    char *input;
    int length;
};

struct serial_api_command_t {
    union {
        char type;
        serial_api_echo_command_t echo;
    };
};

struct serial_api_response_t {
    char *buffer;
    int length;
};

serial_api_response_t serial_api_read_response();
void serial_api_queue_byte(char byte);
void serial_api_queue_output(const char *message);
void serial_api_queue_output_len(char *message,
                                 int length);

inline void log_value(char key, long value)
{
    char buffer[32];
    sprintf(buffer, "%c=%ld", key, value);

    serial_api_queue_output(buffer);
}

#endif
