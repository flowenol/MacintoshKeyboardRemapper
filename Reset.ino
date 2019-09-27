void kbd_hard_reset() {
  kbd_reset();
  model_number = BAD_CMD;
  kbd_last_data_read_index = -1;
  keyboard_state = REQUIRE_MODEL_NUMBER;
}

void kbd_reset() {
  kbd_clock_count = 0;
  kbd_clock_detected = false;
  kbd_clock_state = HIGH;
  kbd_clock_start = 0;
  kbd_clock_transition_start = 0;
  kbd_delay_start = 0;
  kbd_data_read = 0x00;
}

void mac_reset() {
  mac_ready_detected = false;
  mac_delay_start = 0;
  mac_clock_state = HIGH;
  mac_clock_started = false;
  mac_clock_count = 0;
  mac_clock_delay_start = 0;
  mac_cmd_response_delay_start = 0;
  mac_data_read = 0x00;
  mac_data_write = 0x00;
}
