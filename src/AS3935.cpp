#ifndef _AS3935_Library_Definitions_h
#define _AS3935_Library_Definitions_h

#include "AS3935.h"

AS3935Sensor::AS3935Sensor() {}
AS3935Sensor::~AS3935Sensor(void) {}

void AS3935Sensor::begin(uint8_t sda, uint8_t scl, uint8_t address,
                         uint8_t irq) {
  sensorAddress = address;
  sensorIRQ = irq;
  I2CBus.begin(sda, scl);
  pinMode(sensorIRQ, INPUT);
  /* Disabling all Oscillators from showing on IRQ */
  writeRegister(REGISTER_OSCILATORS, MASK_OSCILATORS, POSITION_OSCILATORS, 0);
}

uint8_t AS3935Sensor::readRegister(uint8_t registerNumber) {
  I2CBus.beginTransmission(sensorAddress);
  I2CBus.write(registerNumber);
  I2CBus.endTransmission(false);
  I2CBus.requestFrom((int)sensorAddress, 1);
  return I2CBus.read();
}

uint8_t AS3935Sensor::readRegister(uint8_t registerNumber, uint8_t mask,
                                   uint8_t readFrom) {

  uint8_t _register = readRegister(registerNumber);
#ifdef DEBUG
  if (_register != 0) {
    Serial << endl
           << "INFO: Register: " << _BIN(_register)
           << ", Content: BIN: " << _BIN((_register &= ~mask) >> readFrom)
           << ", HEX: " << _HEX((_register &= ~mask) >> readFrom) << endl << "Value: ";
  }
#endif
  return ((_register &= ~mask) >> readFrom);
}

void AS3935Sensor::writeRegister(uint8_t registerNumber, uint8_t mask,
                                 uint8_t writeFrom, uint8_t value) {
  uint8_t _register = readRegister(registerNumber);
  _register &= mask;
  _register |= (value << writeFrom);
#ifdef DEBUG
  Serial << endl << "INFO: New content to write: " << _BIN(_register);
#endif
  I2CBus.beginTransmission(sensorAddress);
  I2CBus.write(registerNumber);
  I2CBus.write(_register);
  I2CBus.endTransmission();
}

uint8_t AS3935Sensor::readIntrruptReason(void) {
  delay(2);
  return readRegister(REGISTER_INT, MASK_INT, POSITION_INT);
}

boolean AS3935Sensor::isIndoor(void) {
  return readRegister(REGISTER_AFE_GAIN, MASK_AFE_GAIN, POSITION_AFE_GAIN) ==
         AFE_GAIN_INDOOR;
}

boolean AS3935Sensor::isOutdoor(void) {
  return readRegister(REGISTER_AFE_GAIN, MASK_AFE_GAIN, POSITION_AFE_GAIN) ==
         AFE_GAIN_OUTDOOR;
}

void AS3935Sensor::setIndoor(bool enable) {
  writeRegister(REGISTER_AFE_GAIN, MASK_AFE_GAIN, POSITION_AFE_GAIN,
                enable ? AFE_GAIN_INDOOR : AFE_GAIN_OUTDOOR);
}

uint8_t AS3935Sensor::readDistanceToStorm(void) {
  uint8_t _distance =
      readRegister(REGISTER_DISTANCE, MASK_DISTANCE, POSITION_DISTANCE);
  return _distance == 63 ? AS3935_OUT_OF_RANGE : _distance;
}

uint8_t AS3935Sensor::readMinimumNumberOfLightning(void) {
  uint8_t _ret;
  switch (readRegister(REGISTER_MIN_NUM_LIGH, MASK_MIN_NUM_LIGH,
                       POSITION_MIN_NUM_LIGH)) {
  case 0x00:
    _ret = 1;
    break;
  case 0x01:
    _ret = 5;
    break;
  case 0x02:
    _ret = 9;
    break;
  case 0x03:
    _ret = 16;
    break;
  default:
    _ret = AS3935_OUT_OF_RANGE;
#ifdef DEBUG
    Serial << endl << "ERROR: MIN_NUM_LIGH out of the range";
#endif
    break;
  }
  return _ret;
}

void AS3935Sensor::setMinimumNumberOfLightning(uint8_t value) {
  uint8_t _valueToSave;
  switch (value) {
  case 1:
    _valueToSave = 0x00;
    break;
  case 5:
    _valueToSave = 0x01;
    break;
  case 9:
    _valueToSave = 0x02;
    break;
  case 16:
    _valueToSave = 0x03;
    break;
  default:
#ifdef DEBUG
    Serial << endl << "ERROR: MIN_NUM_LIGH out of the range";
#endif
    return;
    break;
  }
  writeRegister(REGISTER_MIN_NUM_LIGH, MASK_MIN_NUM_LIGH, POSITION_MIN_NUM_LIGH,
                _valueToSave);
}

uint8_t AS3935Sensor::readNoiseFloorLevel(void) {
  return readRegister(REGISTER_NF_LEV, MASK_NF_LEV, POSITION_NF_LEV);
}

void AS3935Sensor::setNoiseFloorLevel(uint8_t value) {}

void AS3935Sensor::increaseNoiseFloorLevel(void) {
  uint8_t _level = readNoiseFloorLevel();
  if (_level < 7) {
    setNoiseFloorLevel(_level + 1);
  }
}

void AS3935Sensor::descreseNoiseFloorLevel(void) {
  uint8_t _level = readNoiseFloorLevel();
  if (_level > 0) {
    setNoiseFloorLevel(_level - 1);
  }
}

uint8_t AS3935Sensor::readSpikeRejection() {
  return readRegister(REGISTER_SREJ, MASK_SREJ, POSITION_SREJ);
}

void AS3935Sensor::setSpikeRejection(uint8_t value) {
  if (value > 0 || value < 12) {
    writeRegister(REGISTER_SREJ, MASK_SREJ, POSITION_SREJ, value);
  }
}

uint8_t AS3935Sensor::readWatchdogThreshold() {
  return readRegister(REGISTER_WDTH, MASK_WDTH, POSITION_WDTH);
}

void AS3935Sensor::setWatchdogThreshold(uint8_t value) {
  if (value > 0 || value < 11) {
    writeRegister(REGISTER_WDTH, MASK_WDTH, POSITION_WDTH, value);
  }
}

boolean AS3935Sensor::readMaskDisturber() {
  return readRegister(REGISTER_MASK_DIST, MASK_MASK_DIST, POSITION_MASK_DIST);
}

void AS3935Sensor::setMaskDisturber(bool enable) {
  writeRegister(REGISTER_MASK_DIST, MASK_MASK_DIST, POSITION_MASK_DIST, enable);
}

void AS3935Sensor::setDefautSettings() {
  writeRegister(REGISTER_PRESET_DEFAULT, MASK_PRESET_DEFAULT,
                POSITION_PRESET_DEFAULT, AS3935_DIRECT_COMMAND);
}

void AS3935Sensor::calibrateInternalRSOscillators() {
  writeRegister(REGISTER_CALIB_RCO, MASK_CALIB_RCO, POSITION_CALIB_RCO,
                AS3935_DIRECT_COMMAND);
}

void AS3935Sensor::clearStatistics() {
  writeRegister(REGISTER_CL_STAT, MASK_CL_STAT, POSITION_CL_STAT, 1);
  delay(2);
  writeRegister(REGISTER_CL_STAT, MASK_CL_STAT, POSITION_CL_STAT, 0);
  delay(2);
  writeRegister(REGISTER_CL_STAT, MASK_CL_STAT, POSITION_CL_STAT, 1);
  delay(2);
}

uint8_t AS3935Sensor::readFrequencyDivisionForAntennaTuning() {
  uint8_t _ret;
  switch (readRegister(REGISTER_LCO_FDIV, MASK_LCO_FDIV, POSITION_LCO_FDIV)) {
  case 0x00:
    _ret = 16;
    break;
  case 0x01:
    _ret = 32;
    break;
  case 0x02:
    _ret = 64;
    break;
  case 0x03:
    _ret = 128;
    break;
  default:
    _ret = AS3935_OUT_OF_RANGE;
#ifdef DEBUG
    Serial << endl << "ERROR: LCO_FDIV out of the range";
#endif
    break;
  }
  return _ret;
}

void AS3935Sensor::setFrequencyDivisionForAntennaTuning(uint8_t value) {
  uint8_t _valueToSave;
  switch (value) {
  case 16:
    _valueToSave = 0x00;
    break;
  case 32:
    _valueToSave = 0x01;
    break;
  case 64:
    _valueToSave = 0x02;
    break;
  case 128:
    _valueToSave = 0x03;
    break;
  default:
#ifdef DEBUG
    Serial << endl << "ERROR: LCO_FDIV out of the range";
#endif
    return;
    break;
  }
  writeRegister(REGISTER_LCO_FDIV, MASK_LCO_FDIV, POSITION_LCO_FDIV,
                _valueToSave);
}

#endif
