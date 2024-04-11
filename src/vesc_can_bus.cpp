#include "vesc_can_bus.h"
#include <Arduino.h>
#include <SPI.h>
#include <mcp_can.h>
#include <stdint.h>

MCP_CAN CAN0(5);

void init_vesc_can() {
  byte result = CAN0.begin(MCP_ANY, CAN_250KBPS, MCP_8MHZ);
  Serial.println(result == CAN_OK ? "CAN BUS Shield init ok!"
                                  : "CAN BUS Shield init fail");
  CAN0.setMode(MCP_NORMAL);
}

// Implementation for sending extended ID CAN-frames
void can_transmit_eid(uint32_t id, const uint8_t *data, uint8_t len) {
  CAN0.sendMsgBuf(id, (byte)0x01, len, (byte *)data);
}

void buffer_append_int16(uint8_t *buffer, int16_t number, int32_t *index) {
  buffer[(*index)++] = number >> 8;
  buffer[(*index)++] = number;
}

void buffer_append_int32(uint8_t *buffer, int32_t number, int32_t *index) {
  buffer[(*index)++] = number >> 24;
  buffer[(*index)++] = number >> 16;
  buffer[(*index)++] = number >> 8;
  buffer[(*index)++] = number;
}

void buffer_append_float16(uint8_t *buffer, float number, float scale,
                           int32_t *index) {
  buffer_append_int16(buffer, (int16_t)(number * scale), index);
}

void buffer_append_float32(uint8_t *buffer, float number, float scale,
                           int32_t *index) {
  buffer_append_int32(buffer, (int32_t)(number * scale), index);
}

void comm_can_set_duty(uint8_t controller_id, float duty) {
  int32_t send_index = 0;
  uint8_t buffer[4];
  buffer_append_int32(buffer, (int32_t)(duty * 100000.0), &send_index);
  can_transmit_eid(controller_id | ((uint32_t)CAN_PACKET_SET_DUTY << 8), buffer,
                   send_index);
}

void comm_can_set_current(uint8_t controller_id, float current) {
  int32_t send_index = 0;
  uint8_t buffer[4];
  buffer_append_int32(buffer, (int32_t)(current * 1000.0), &send_index);
  can_transmit_eid(controller_id | ((uint32_t)CAN_PACKET_SET_CURRENT << 8),
                   buffer, send_index);
}

void comm_can_set_current_off_delay(uint8_t controller_id, float current,
                                    float off_delay) {
  int32_t send_index = 0;
  uint8_t buffer[6];
  buffer_append_int32(buffer, (int32_t)(current * 1000.0), &send_index);
  buffer_append_float16(buffer, off_delay, 1e3, &send_index);
  can_transmit_eid(controller_id | ((uint32_t)CAN_PACKET_SET_CURRENT << 8),
                   buffer, send_index);
}

void comm_can_set_current_brake(uint8_t controller_id, float current) {
  int32_t send_index = 0;
  uint8_t buffer[4];
  buffer_append_int32(buffer, (int32_t)(current * 1000.0), &send_index);
  can_transmit_eid(controller_id |
                       ((uint32_t)CAN_PACKET_SET_CURRENT_BRAKE << 8),
                   buffer, send_index);
}

void comm_can_set_rpm(uint8_t controller_id, float rpm) {
  int32_t send_index = 0;
  uint8_t buffer[4];
  buffer_append_int32(buffer, (int32_t)rpm, &send_index);
  can_transmit_eid(controller_id | ((uint32_t)CAN_PACKET_SET_RPM << 8), buffer,
                   send_index);
}

void comm_can_set_pos(uint8_t controller_id, float pos) {
  int32_t send_index = 0;
  uint8_t buffer[4];
  buffer_append_int32(buffer, (int32_t)(pos * 1000000.0), &send_index);
  can_transmit_eid(controller_id | ((uint32_t)CAN_PACKET_SET_POS << 8), buffer,
                   send_index);
}

void comm_can_set_current_rel(uint8_t controller_id, float current_rel) {
  int32_t send_index = 0;
  uint8_t buffer[4];
  buffer_append_float32(buffer, current_rel, 1e5, &send_index);
  can_transmit_eid(controller_id | ((uint32_t)CAN_PACKET_SET_CURRENT_REL << 8),
                   buffer, send_index);
}

/**
 * Same as above, but also sets the off delay. Note that this command uses 6
 * bytes now. The off delay is useful to set to keep the current controller
 * running for a while even after setting currents below the minimum current.
 */
void comm_can_set_current_rel_off_delay(uint8_t controller_id,
                                        float current_rel, float off_delay) {
  int32_t send_index = 0;
  uint8_t buffer[6];
  buffer_append_float32(buffer, current_rel, 1e5, &send_index);
  buffer_append_float16(buffer, off_delay, 1e3, &send_index);
  can_transmit_eid(controller_id | ((uint32_t)CAN_PACKET_SET_CURRENT_REL << 8),
                   buffer, send_index);
}

void comm_can_set_current_brake_rel(uint8_t controller_id, float current_rel) {
  int32_t send_index = 0;
  uint8_t buffer[4];
  buffer_append_float32(buffer, current_rel, 1e5, &send_index);
  can_transmit_eid(controller_id |
                       ((uint32_t)CAN_PACKET_SET_CURRENT_BRAKE_REL << 8),
                   buffer, send_index);
}

void comm_can_set_handbrake(uint8_t controller_id, float current) {
  int32_t send_index = 0;
  uint8_t buffer[4];
  buffer_append_float32(buffer, current, 1e3, &send_index);
  can_transmit_eid(controller_id |
                       ((uint32_t)CAN_PACKET_SET_CURRENT_HANDBRAKE << 8),
                   buffer, send_index);
}

void comm_can_set_handbrake_rel(uint8_t controller_id, float current_rel) {
  int32_t send_index = 0;
  uint8_t buffer[4];
  buffer_append_float32(buffer, current_rel, 1e5, &send_index);
  can_transmit_eid(controller_id |
                       ((uint32_t)CAN_PACKET_SET_CURRENT_HANDBRAKE_REL << 8),
                   buffer, send_index);
}

void comm_can_read(byte *buffer, byte size) {
  ulong id = 0;
  byte len = 0;
  byte rxBuffer[128];
  byte result = CAN0.readMsgBuf(&id, &len, rxBuffer);

  if (result == CAN_NOMSG) {
    return;
  }

  byte vesc_id = ((byte *)&id)[0];
  byte command_id = ((byte *)&id)[1];

  Serial.printf("vesc_id: : %0x | command_id %0x\n", vesc_id, command_id);

  if (vesc_id == 0x0a) {
    return;
  }

  switch (command_id) {
  // see https://github.com/vedderb/bldc/blob/master/documentation/comm_can.md
  // for documentation and structure of the status messages
  case 0x10: {
    CAN_PACKET_STATUS_4 status_4_temp;
    status_4_temp.temp_fet = rxBuffer[0] << 8 | rxBuffer[1];
    status_4_temp.temp_motor = rxBuffer[2] << 8 | rxBuffer[3];
    status_4_temp.current_in = rxBuffer[4] << 8 | rxBuffer[5];
    status_4_temp.pid_pos = rxBuffer[6] << 8 | rxBuffer[7];

    Serial.printf("temp_fet: %d\n", status_4_temp.temp_fet);
    Serial.printf("temp_motor: %d\n", status_4_temp.temp_motor);
    Serial.printf("current_in: %d\n", status_4_temp.current_in);
    Serial.printf("pid_pos: %d\n", status_4_temp.pid_pos);
    break;
  }

  case 0x09: {
    Serial.printf("Received status 9\n");
    break;
  }

  default:
    Serial.printf("Unknown command: %0x\n", command_id);
    break;
  }
}
