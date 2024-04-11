#ifndef VESC_CAN_BUS_H
#define VESC_CAN_BUS_H

#include <stdint.h>

typedef enum {
  CAN_PACKET_SET_DUTY = 0,
  CAN_PACKET_SET_CURRENT,
  CAN_PACKET_SET_CURRENT_BRAKE,
  CAN_PACKET_SET_RPM,
  CAN_PACKET_SET_POS,
  CAN_PACKET_SET_CURRENT_REL = 10,
  CAN_PACKET_SET_CURRENT_BRAKE_REL,
  CAN_PACKET_SET_CURRENT_HANDBRAKE,
  CAN_PACKET_SET_CURRENT_HANDBRAKE_REL,
  CAN_PACKET_MAKE_ENUM_32_BITS = 0xFFFFFFFF,
} CAN_PACKET_ID;

void init_vesc_can();

void buffer_append_int16(uint8_t *buffer, int16_t number, int32_t *index);

void buffer_append_int32(uint8_t *buffer, int32_t number, int32_t *index);

void buffer_append_float16(uint8_t *buffer, float number, float scale,
                           int32_t *index);

void buffer_append_float32(uint8_t *buffer, float number, float scale,
                           int32_t *index);

void comm_can_set_duty(uint8_t controller_id, float duty);

void comm_can_set_current(uint8_t controller_id, float current);

void comm_can_set_current_off_delay(uint8_t controller_id, float current,
                                    float off_delay);

void comm_can_set_current_brake(uint8_t controller_id, float current);

void comm_can_set_rpm(uint8_t controller_id, float rpm);

void comm_can_set_pos(uint8_t controller_id, float pos);

void comm_can_set_current_rel(uint8_t controller_id, float current_rel);

void comm_can_set_current_rel_off_delay(uint8_t controller_id,
                                        float current_rel, float off_delay);

void comm_can_set_current_brake_rel(uint8_t controller_id, float current_rel);

void comm_can_set_handbrake(uint8_t controller_id, float current);

void comm_can_set_handbrake_rel(uint8_t controller_id, float current_rel);

#endif // VESC_CAN_BUS_H