
byte remap(byte kbd_data) {
  switch (kbd_data) {
    
    // Return
    case 0xD5:
      return 0xC9;
    case 0x55:
      return 0x49;

    // backslash
    case 0xC9:
      return 0xD5;
    case 0x49:
      return 0x55;

    // space
    case 0xE9:
      return 0xE3;
    case 0x69:
      return 0x63;

    // Enter
    case 0xE3:
      return 0xE9;
    case 0x63:
      return 0x69;

    // z
    case 0xA1:
      return 0x8D;
    case 0x21:
      return 0x0D;

    // y
    case 0x8F:
      return 0xA1;
    case 0x0F:
      return 0x21;

    // x
    case 0x91:
      return 0x8F;
    case 0x11:
      return 0x0F;

    // c
    case 0x93:
      return 0x91;
    case 0x13:
      return 0x11;

    // v
    case 0x97:
      return 0x93;
    case 0x17:
      return 0x13;

    // b
    case 0xDB:
      return 0x97;
    case 0x5B:
      return 0x17;
    
    // n
    case 0xDD:
      return 0xDB;
    case 0x5D:
      return 0x5B;

    // m
    case 0xD7:
      return 0xDD;
    case 0x57:
      return 0x5D;

    // .
    case 0xD9:
      return 0xDF;
    case 0x59:
      return 0x5F;

    // /
    case 0x95:
      return 0xD9;
    case 0x15:
      return 0x59; 
    
    // ,
    case 0xDF:
      return 0xD7;
    case 0x5F:
      return 0x57;   
     
    default:
    return kbd_data;
  }
}
