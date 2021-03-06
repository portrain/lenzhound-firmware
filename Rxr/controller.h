#ifndef lenzhound_motor_controller_h
#define lenzhound_motor_controller_h
#include "motor.h"
#include "constants.h"

struct controller_state_t {
  bool direction;
  int mode;
  long max_speed;
  long accel;
  long decel_denominator;
  long velocity;
  long calculated_position;
  long motor_position;
  long target_position;
  long run_count;
  long sleeping;
  bool initial_position_set;
  unsigned int current_level;
  unsigned int motor_driver;
};

void controller_init();
void controller_run();
void controller_move_to_position(long position);
void controller_initialize_position(long position);
void controller_uninitialize_position();
void controller_set_speed(long speed);
void controller_set_accel(long accel);
long controller_get_target_position();
long controller_get_speed();
long controller_get_accel();
void controller_set_mode(int mode);
bool controller_is_position_initialized();
void controller_set_current_level(unsigned int current_level);
unsigned int controller_get_current_level();
void controller_set_motor_driver(unsigned int motor_driver);
unsigned int controller_get_motor_driver();

#endif //lenzhound_motor_controller_h
