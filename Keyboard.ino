void send_data_to_kbd(byte data, kbd_state next_state) {
  if (!kbd_clock_detected) {
    pinMode(DATA_KBD, OUTPUT);
    digitalWrite(DATA_KBD, LOW);
    kbd_clock_read = digitalRead(CLK_KBD);
    if (kbd_clock_read == LOW) {
      kbd_clock_detected = true;
      kbd_clock_state=LOW;
    } else {
      check_kbd_clock_start();
      return;
    }
  }
  
  digitalWrite(DATA_KBD, data>>(7 - kbd_clock_count) & 1);
  kbd_clock_read = digitalRead(CLK_KBD);
  if (kbd_clock_read  != kbd_clock_state) {
    kbd_clock_transition_start = 0;
    kbd_clock_state = kbd_clock_read;
    if (kbd_clock_state == LOW) {
      kbd_clock_count++;
    }

    if (kbd_clock_state == HIGH && kbd_clock_count == 7) {
      digitalWrite(DATA_KBD, HIGH);
      kbd_reset();
      keyboard_state = next_state;
    }
  } else {
    check_clock_transition();    
  }  
}

void read_data_from_kbd(kbd_state next_state) {
  if (!kbd_clock_detected) {
    pinMode(DATA_KBD, INPUT);
    kbd_clock_read = digitalRead(CLK_KBD);
    if (kbd_clock_read == LOW) {
      kbd_clock_detected = true;
      kbd_clock_state=LOW;
    } else {
      check_kbd_clock_start();
      return;
    }
  }
   
  kbd_clock_read = digitalRead(CLK_KBD);
  if (kbd_clock_read  != kbd_clock_state) {
    kbd_clock_transition_start = 0;
    kbd_clock_state = kbd_clock_read;
    if (kbd_clock_state == HIGH) {
      if (digitalRead(DATA_KBD) == HIGH) {
        kbd_data_read |= 1<<(7 - kbd_clock_count);
      }
    }
    if (kbd_clock_state == LOW) {
      kbd_clock_count++;
    }

    if (kbd_clock_state == HIGH && kbd_clock_count == 7) {
        if (keyboard_state == READ_MODEL_NUMBER) {
          model_number = kbd_data_read;
        } else if (kbd_data_read != TEST_ACK && kbd_data_read != TEST_NAK) {
          kbd_last_data_read_index = (kbd_last_data_read_index + 1) % KBD_BUFFER_SIZE;
          kbd_last_data_read[kbd_last_data_read_index] = kbd_data_read;
        }
  
        /*if (kbd_data_read != NULL_TRANSITION && kbd_data_read != TEST_ACK) {
          Serial.print("Data read: ");
          Serial.println(kbd_data_read, BIN);
        }*/
          
        kbd_reset();
        keyboard_state = next_state;                    
    }
  } else {
    check_clock_transition(); 
  }
}

void check_kbd_clock_start() {
  if (kbd_clock_start == 0) {
    kbd_clock_start = millis();
  } else if (millis() - kbd_clock_start >= CLOCK_START_MSEC) {
    //Serial.println("Failed to receive clock signal, resetting");
    kbd_hard_reset();
  }
}

void check_clock_transition() {
  if (kbd_clock_transition_start == 0) {
    kbd_clock_transition_start = micros();
  } else if (micros() - kbd_clock_transition_start >= CLOCK_TRANSITION_USEC) {
    //Serial.println("Failed to receive proper clock signal, resetting");
    kbd_hard_reset();
  }
}
