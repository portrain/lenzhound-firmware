#include "controller.h"
#include "constants.h"
#include "util.h"

#define CONVERSION_FACTOR .01

controller_state_t state = {0};

void _controller_sleep()
{
    state.sleeping = true;
    state.run_count = 0;

    motor_sleep();
}

void _controller_wake_up()
{
    state.sleeping = false;
    motor_wake();
}

void controller_init()
{
    state.direction = 1;
    state.max_speed = 0;
    state.current_speed_cap = 0;
    state.accel = 0;
    state.mode = 0;
    state.decel_denominator = 0;
    state.velocity = 0;
    state.calculated_position = 0;
    state.motor_position = 0;
    state.target_position = 0;
    state.run_count = 0;
    state.sleeping = true;
    state.initial_position_set = false;
    _controller_sleep();
    motor_set_steps(EIGHTH_STEPS);
}

bool controller_is_position_initialized()
{
    return state.initial_position_set;
}

void controller_initialize_position(long position)
{
    state.motor_position = position;
    state.calculated_position = position;
    state.target_position = position;
    state.initial_position_set = true;
}

void controller_move_to_position(long position)
{
    if (position != state.target_position) {
        _controller_wake_up();
        state.run_count = 0;
    }
    state.target_position = position;
}

long controller_get_target_position()
{
    return state.target_position;
}

void controller_set_mode(int mode)
{
    state.mode = mode;
}

void controller_refresh_speed()
{
    long max = state.max_speed;

    state.current_speed_cap = min32(
        max32(max * state.speed_percent * CONVERSION_FACTOR, 1L),
        FIXED_ONE);
}

void controller_refresh_accel()
{
    state.accel = max32(
        state.max_accel *
        state.accel_percent *
        CONVERSION_FACTOR, 1L);
    state.decel_denominator = fixed_mult(state.accel, i32_to_fixed(2L));
}

void controller_set_speed_percent(int speed)
{
    state.speed_percent = speed;
    controller_refresh_speed();
}

void controller_set_accel_percent(int accel)
{
    state.accel_percent = accel;
    controller_refresh_accel();
}

void controller_set_speed(long speed)
{
    state.max_speed = speed;
    controller_refresh_speed();
}

void controller_set_accel(long accel)
{
    state.max_accel = accel;
    controller_refresh_accel();
}

long controller_get_speed()
{
    return state.max_speed;
}

long controller_get_accel()
{
    return state.max_accel;
}

long controller_get_decel_threshold()
{
    return fixed_div(
        fixed_mult(state.velocity, state.velocity),
        state.decel_denominator);
}

bool controller_try_sleep()
{
    if (state.motor_position != state.target_position) {
        state.run_count = 0;
        return false;
    }
    if (state.sleeping) {
        return true;
    }
    if (state.run_count > MOTOR_SLEEP_THRESHOLD) {
        _controller_sleep();
        return true;
    }
    state.run_count++;
    return false;
}

void controller_run()
{
    if (controller_try_sleep()) {
        return;
    }
    long steps_to_go = abs32(state.target_position - state.calculated_position);


//  NOTE(doug): inverted elements are highlighted
//  CASE: POSITIVE -------------------------------------------------------------
    if (state.direction) {
//                                     |
//                                     v
        if ((state.calculated_position > state.target_position) ||
            (steps_to_go <= controller_get_decel_threshold())) {
//                         |
//                         v
            state.velocity -= state.accel;
//                                           |
//                                           v
        } else if (state.calculated_position < state.target_position) {
//                             |                  | 
//                             v                  v
            state.velocity = min32(state.velocity + state.accel,
//              |
//              v
                state.current_speed_cap);
        }
        state.calculated_position += state.velocity;
//                               |
//                               v
        if (state.motor_position < state.calculated_position &&
            state.motor_position != state.target_position) {
//                               |
//                               v
            state.motor_position += FIXED_ONE;
            motor_pulse();
        }
//                         |
//                         v
        if (state.velocity < 0) {
//                            |
//                            v
            state.direction = 0;
//                            |
//                            v
            motor_set_dir_backward();
        }
//  CASE: NEGATIVE -------------------------------------------------------------
    } else {
//                                     |
//                                     v
        if ((state.calculated_position < state.target_position) ||
            (steps_to_go <= controller_get_decel_threshold())) {
//                         |
//                         v
            state.velocity += state.accel;
//                                           |
//                                           v
        } else if (state.calculated_position > state.target_position) {
//                             |                  | 
//                             v                  v
            state.velocity = max32(state.velocity - state.accel,
//              |
//              v
                -state.current_speed_cap);
        }
        state.calculated_position += state.velocity;
//                               |
//                               v
        if (state.motor_position > state.calculated_position &&
            state.motor_position != state.target_position) {
//                               |
//                               v
            state.motor_position -= FIXED_ONE;
            motor_pulse();
        }
//                         |
//                         v
        if (state.velocity > 0) {
//                            |
//                            v
            state.direction = 1;
//                            |
//                            v
            motor_set_dir_forward();
        }
    }
//  END ------------------------------------------------------------------------
}