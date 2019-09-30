#define CLK_MAC 7
#define DATA_MAC 8

#define CLK_KBD 5
#define DATA_KBD 6

#define CLOCK_START_MSEC 500
#define CLOCK_TRANSITION_USEC 300
#define CLOCK_READ_LOW_USEC 180
#define CLOCK_READ_HIGH_USEC 200
#define CLOCK_WRITE_LOW_USEC 180
#define CLOCK_WRITE_HIGH_USEC 200
#define MAC_RESP_DELAY_MSEC 240
#define MAC_DETECT_READY_USEC 300

// command constants
const byte MODEL_NUMBER_CMD = 0x16;
const byte TEST_CMD = 0x36;
const byte INQUIRY_CMD = 0x10;
const byte INSTANT_CMD = 0x14;

// command response constants
const byte NULL_TRANSITION = 0x7B;
const byte TEST_ACK = 0x7D;
const byte TEST_NAK = 0x77;
const byte BAD_CMD = 0x00;

enum kbd_state { 
  REQUIRE_MODEL_NUMBER,
  READ_MODEL_NUMBER,
  REQUIRE_TEST,
  READ_TEST,
  REQUIRE_INQUIRY,
  READ_INQUIRY,
  DELAY_INQUIRY
};

enum mac_state {
  DETECT_READY_FOR_CMD,
  READ_CMD,
  DETECT_READY_FOR_DATA,
  WRITE_DATA
};

// keyboard state variables
kbd_state keyboard_state = REQUIRE_MODEL_NUMBER;
boolean kbd_clock_detected;
boolean kbd_clock_state;
boolean kbd_clock_read;
unsigned int kbd_clock_count;
unsigned long kbd_clock_start;
unsigned long kbd_clock_transition_start;
unsigned long kbd_delay_start;

byte kbd_data_read;
byte kbd_last_data_read[4];
short kbd_last_data_read_index = -1;
byte model_number;

// macintosh state variables
mac_state macintosh_state = DETECT_READY_FOR_CMD;
boolean mac_ready_detected;
unsigned long mac_delay_start;
boolean mac_clock_started;
boolean mac_clock_state;
unsigned int mac_clock_count;
unsigned long mac_clock_delay_start;
unsigned long mac_cmd_response_delay_start;

byte mac_data_read;
byte last_mac_cmd;
byte mac_data_write;


void setup() {
  pinMode(CLK_MAC, OUTPUT);
  digitalWrite(CLK_MAC, HIGH);
  pinMode(CLK_KBD, INPUT);
  kbd_hard_reset();
  //Serial.begin(19200);
}

void loop() {
  switch (keyboard_state) {
    case REQUIRE_MODEL_NUMBER:
      send_data_to_kbd(MODEL_NUMBER_CMD, READ_MODEL_NUMBER);
      break;
    case READ_MODEL_NUMBER:
      read_data_from_kbd(REQUIRE_TEST);
      break;
    case REQUIRE_TEST:
      send_data_to_kbd(TEST_CMD, READ_TEST);
      break;
    case READ_TEST:
      read_data_from_kbd(REQUIRE_INQUIRY);
      break;
    case REQUIRE_INQUIRY:
      send_data_to_kbd(INQUIRY_CMD, READ_INQUIRY);
      break;
    case READ_INQUIRY:
      read_data_from_kbd(REQUIRE_INQUIRY);
      break;
  }

  switch (macintosh_state) {
    case DETECT_READY_FOR_CMD:
      mac_detect_ready_to_send_cmd();
      break;
    case READ_CMD:
      mac_read_data();
      break;
    case DETECT_READY_FOR_DATA:
      mac_detect_ready_to_recv_data();
      break;
    case WRITE_DATA:
      mac_write_data();
      break;
  }
  
}
