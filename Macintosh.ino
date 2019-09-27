void mac_detect_ready_to_send_cmd() {
  if (!mac_ready_detected) {
    pinMode(DATA_MAC, INPUT);
    if (digitalRead(DATA_MAC) == LOW) {
      mac_ready_detected = true;  
    }
  } else {
    if (mac_delay_start == 0) {
      mac_delay_start = micros(); 
    }

    if (micros() - mac_delay_start >= MAC_DETECT_READY_USEC) {
      mac_reset();
      macintosh_state = READ_CMD;
    }
  }
}

void mac_detect_ready_to_recv_data() {
  if (mac_delay_start == 0) {
    mac_delay_start = micros(); 
    pinMode(DATA_MAC, INPUT);
  }

  if (micros() - mac_delay_start >= 400) {
    if (digitalRead(DATA_MAC) == HIGH) {
      macintosh_state = WRITE_DATA;
    } else {
      macintosh_state = DETECT_READY_FOR_CMD;
    }
    mac_reset();
  }
}

void mac_read_data() {
  if (!mac_clock_started) {
    pinMode(DATA_MAC, INPUT);
    mac_clock_state = LOW;
    mac_clock_started = true;
  }

  if (mac_clock_state == LOW) {
    if (mac_clock_delay_start == 0) {
      digitalWrite(CLK_MAC, LOW);
      mac_clock_delay_start = micros();
    }

    if (micros() - mac_clock_delay_start >= CLOCK_READ_LOW_USEC) {
      mac_clock_state = HIGH;
      mac_clock_delay_start = 0;  
    }
  }

  if (mac_clock_state == HIGH) {
    if (mac_clock_delay_start == 0) {
      digitalWrite(CLK_MAC, HIGH);
      mac_clock_delay_start = micros();
      
      if (digitalRead(DATA_MAC) == HIGH) {
        mac_data_read |= 1<<(7 - mac_clock_count);  
      }
    }

    if (micros() - mac_clock_delay_start >= CLOCK_READ_HIGH_USEC) {
      if (mac_clock_count < 7) {
        mac_clock_state = LOW;
        mac_clock_delay_start = 0;
        mac_clock_count++;
      } else {
        last_mac_cmd = mac_data_read;
        mac_reset();
        macintosh_state = DETECT_READY_FOR_DATA;  
      }
    }
  }
}

void mac_write_data() {
  if (!mac_clock_started) {
    if (last_mac_cmd == INQUIRY_CMD && kbd_last_data_read_index == -1) {
      if (mac_cmd_response_delay_start == 0) {
        mac_cmd_response_delay_start = millis();
      }

      if (millis() - mac_cmd_response_delay_start >= MAC_RESP_DELAY_MSEC) {
        goto WRITE_MAC;
      } else {
        return;
      }
    } else {
      WRITE_MAC:
      
      mac_data_write = mac_cmd_response_data();
      if (mac_data_write == BAD_CMD) {
         mac_reset();
         macintosh_state = DETECT_READY_FOR_CMD;
         return;
      } else {
        pinMode(DATA_MAC, OUTPUT);
        mac_clock_state = LOW;
        mac_clock_started = true;
      }
    }
  }
  
  if (mac_clock_state == LOW) {
    if (mac_clock_delay_start == 0) {
      digitalWrite(CLK_MAC, LOW);
      mac_clock_delay_start = micros();

      digitalWrite(DATA_MAC, mac_data_write>>(7 - mac_clock_count) & 1 > 0);
    }

    if (micros() - mac_clock_delay_start >= CLOCK_WRITE_LOW_USEC) {
      mac_clock_state = HIGH;
      mac_clock_delay_start = 0;  
    }
  }

  if (mac_clock_state == HIGH) {
    if (mac_clock_delay_start == 0) {
      digitalWrite(CLK_MAC, HIGH);
      mac_clock_delay_start = micros();
    }

    if (micros() - mac_clock_delay_start >= CLOCK_WRITE_HIGH_USEC) {
      if (mac_clock_count < 7) {
        mac_clock_state = LOW;
        mac_clock_delay_start = 0;
        mac_clock_count++;
      } else {
        digitalWrite(DATA_MAC, HIGH);
        mac_reset();
        macintosh_state = DETECT_READY_FOR_CMD;
      }
    }
  }
}

byte mac_cmd_response_data() {
  switch (last_mac_cmd) {
    case MODEL_NUMBER_CMD:
      return model_number;
    case TEST_CMD:
      return TEST_ACK;
    case INSTANT_CMD:
    case INQUIRY_CMD:
      if (kbd_last_data_read_index == -1) {
        return NULL_TRANSITION;
      }
      byte inquiry_data = kbd_last_data_read[0];
      kbd_last_data_read[0] = kbd_last_data_read[1];
      kbd_last_data_read[1] = kbd_last_data_read[2];
      kbd_last_data_read[2] = kbd_last_data_read[3];
      kbd_last_data_read_index -= 1;
      return remap(inquiry_data);
  }
}
