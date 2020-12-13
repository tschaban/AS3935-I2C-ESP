#ifndef _AS3935_Library_Defs_h
#define _AS3935_Library_Defs_h

#define AS3935_OUT_OF_RANGE 255
#define AS3935_DIRECT_COMMAND 0x96

enum AS3935_REGISTERS {
  REGISTER_AFE_GAIN = 0x00,
  REGISTER_NF_LEV = 0x01,
  REGISTER_WDTH = 0x01,
  REGISTER_MIN_NUM_LIGH = 0x02,
  REGISTER_SREJ = 0x02,
  REGISTER_CL_STAT = 0x02,
  REGISTER_INT = 0x03,
  REGISTER_MASK_DIST = 0x03,
  REGISTER_LCO_FDIV = 0x03,
  REGISTER_DISTANCE = 0x07,
  REGISTER_OSCILATORS = 0x08,
  REGISTER_TUN_CAP = 0x08,
  REGISTER_PRESET_DEFAULT = 0x3C,
  REGISTER_CALIB_RCO = 0x3D
};

enum AS3935_REGISTER_MASKS {
  MASK_PRESET_DEFAULT = B11111110,
  MASK_CALIB_RCO = B00000000,
  MASK_INT = B11110000,
  MASK_SREJ = B11110000,
  MASK_WDTH = B11110000,
  MASK_TUN_CAP = B11110000,
  MASK_NF_LEV = B10001111,
  MASK_AFE_GAIN = B11000001,
  MASK_DISTANCE = B11100000,
  MASK_LCO_FDIV = B00111111,
  MASK_MIN_NUM_LIGH = B11001111,
  MASK_MASK_DIST = B11011111,
  MASK_CL_STAT = B10111111,
  MASK_OSCILATORS = B00011111,
  MASK_DISP_LCO = B01111111,
  MASK_DISP_TRCO = B11011111
};

enum AS3935_REGISTER_PARAMS_POSITION {
  POSITION_DISTANCE = 0,
  POSITION_INT = 0,
  POSITION_SREJ = 0,
  POSITION_WDTH = 0,
  POSITION_TUN_CAP = 0,
  POSITION_PRESET_DEFAULT = 0,
  POSITION_CALIB_RCO = 0,
  POSITION_OSCILATORS = 0,
  POSITION_AFE_GAIN = 1,
  POSITION_MIN_NUM_LIGH = 4,
  POSITION_NF_LEV = 4,
  POSITION_MASK_DIST = 5,
  POSITION_DISP_TRCO = 5,
  POSITION_CL_STAT = 6,
  POSITION_LCO_FDIV = 6,
  POSITION_DISP_LCO = 7
};

enum AS3935_AFE_GAIN { AFE_GAIN_INDOOR = 0x12, AFE_GAIN_OUTDOOR = 0xE };

enum AS3936_INT { INT_NOISE = 0x01, INT_DISTURBER = 0x04, INT_STRIKE = 0x08 };

struct AS3935_CONFIGURATION {
  uint8_t ADDRESS;
  uint8_t SDA;
  uint8_t SCL;
  uint8_t AFE_GB;
  uint8_t IRQ;
  uint8_t WDTH = B0010;
  uint8_t NF_LEV;
  uint8_t SREJ;
  uint8_t MIN_NUM_LIGH;
  // uint8_t CL_STAT;
  uint8_t MASK_DIST;
  // uint8_t LCO_FDIV;
  uint8_t TUN_CAP;
};

struct AS3935_EVENT {
  uint8_t reason;
  uint8_t distance;
};

#endif
