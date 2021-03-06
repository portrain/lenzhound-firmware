#include "Arduino.h"
#include "console.h"
#include "serial_api.h"

console_state_t console_state = {0};

void console_run()
{
    while (Serial.available() > 0) {
        serial_api_queue_byte((char)Serial.read());
        console_state.failing_to_write = 0;
    }

    serial_api_response_t response = serial_api_read_response();

    int index = 0;
    int remaining = response.length;

    if (remaining && !console_state.failing_to_write) {
        for (int i = 0; i < 3; ++i)
        {
            int written = Serial.write(response.buffer + index, remaining);
            index += written;
            remaining -= written;
            if (remaining <= 0) {
                break;
            }
        }
        if (!index) {
            console_state.failing_to_write = 1;
        }
    }
}
