#ifndef _AS3935_Library_Definitions_h
#define _AS3935_Library_Definitions_h

#include "AS3935.h"

// Set to true if interuption detected on IRQ GPIO
volatile bool signalDetected = false;

// number of interuption to measure frequency
volatile static unsigned long interuptionsCounter = 0;

// measuring frequecny on if true
volatile static bool counterEnabled = false;

// Handler for Signal IRQs
void ICACHE_RAM_ATTR handleSignalDetectionInteruption() {
  signalDetected = true;
}

// Handler for Antena frequency tuning IRQs
void ICACHE_RAM_ATTR handleAntenaFrequencyInteruption() {
  if (counterEnabled) {
    interuptionsCounter++;
  }
}

AS3935Sensor::AS3935Sensor() {}
AS3935Sensor::~AS3935Sensor(void) {}

void AS3935Sensor::begin() {
  I2CBus.begin(configuration.SDA, configuration.SCL);
  pinMode(configuration.IRQ, INPUT);
  attachInterrupt(digitalPinToInterrupt(configuration.IRQ),
                  handleSignalDetectionInteruption, RISING);

  setAFEGainBoost(configuration.AFE_GB);
  setWatchdogThreshold(configuration.WDTH);
  setNoiseFloorLevel(configuration.NF_LEV);
  setSpikeRejection(configuration.SREJ);
  setMinimumNumberOfLightning(configuration.MIN_NUM_LIGH);
  setTuningCapacitors(configuration.TUN_CAP);
  delay(5);

  /* Disabling all Oscillators from showing on IRQ */
  writeRegister(REGISTER_OSCILATORS, MASK_OSCILATORS, POSITION_OSCILATORS, 0);

}

boolean AS3935Sensor::detected(void) {
  if (signalDetected) {
    delay(2);
    event.reason = readIntrruptReason();
    event.distance = readDistanceToStorm();
    signalDetected = false;
    return true;
  }
  return false;
}

AS3935_EVENT AS3935Sensor::get(void) {
  signalDetected = false;
  AS3935_EVENT _temp = event;
  event.distance = 0;
  event.reason = 0;
  return _temp;
}

uint8_t AS3935Sensor::readRegister(uint8_t registerNumber) {
  I2CBus.beginTransmission(configuration.ADDRESS);
  I2CBus.write(registerNumber);
  I2CBus.endTransmission(false);
  I2CBus.requestFrom((int)configuration.ADDRESS, 1);
  return I2CBus.read();
}

uint8_t AS3935Sensor::readRegister(uint8_t registerNumber, uint8_t mask,
                                   uint8_t readFrom) {

  uint8_t _register = readRegister(registerNumber);
  /*#ifdef DEBUG
    if (_register != 0) {
      Serial << endl
             << "INFO: Register: 0x" << _HEX(registerNumber) << " : "
             << _BIN(_register)
             << ", Content: BIN: " << _BIN((_register &= ~mask) >> readFrom)
             << ", HEX: " << _HEX((_register &= ~mask) >> readFrom) << endl
             << "Value: ";
    }
  #endif*/
  return ((_register &= ~mask) >> readFrom);
}

void AS3935Sensor::writeRegister(uint8_t registerNumber, uint8_t mask,
                                 uint8_t writeFrom, uint8_t value) {
  uint8_t _register = readRegister(registerNumber);
  /*#ifdef DEBUG
    Serial << endl
           << "INFO: Updating register 0x" << _HEX(registerNumber) << " : "
           << _BIN(_register);
  #endif*/
  _register &= mask;
  _register |= (value << writeFrom);
  /*#ifdef DEBUG
    Serial << " : New value : " << _BIN(_register);
  #endif*/
  I2CBus.beginTransmission(configuration.ADDRESS);
  I2CBus.write(registerNumber);
  I2CBus.write(_register);
  I2CBus.endTransmission();
}

uint8_t AS3935Sensor::readIntrruptReason(void) {
  return readRegister(REGISTER_INT, MASK_INT, POSITION_INT);
}

uint8_t AS3935Sensor::readAFEGainBoost() {
  return readRegister(REGISTER_AFE_GAIN, MASK_AFE_GAIN, POSITION_AFE_GAIN);
}

void AS3935Sensor::setAFEGainBoost(uint8_t value) {
  writeRegister(REGISTER_AFE_GAIN, MASK_AFE_GAIN, POSITION_AFE_GAIN, value);
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
    Serial << endl
           << "ERROR: MIN_NUM_LIGH out of the range [1,5,9,16], value: "
           << value;
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

void AS3935Sensor::setNoiseFloorLevel(uint8_t value) {
  if (value < 0 || value > 7) {
#ifdef DEBUG
    Serial << endl << "ERROR: NF_LEV out of the range [0:7], value: " << value;
#endif
    return;
  }
  writeRegister(REGISTER_NF_LEV, MASK_NF_LEV, POSITION_NF_LEV, value);
}

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

  if (value < 0 || value > 11) {
#ifdef DEBUG
    Serial << endl << "ERROR: SREJ out of the range [0:11], value: " << value;
#endif
    return;
  }
  writeRegister(REGISTER_SREJ, MASK_SREJ, POSITION_SREJ, value);
}

uint8_t AS3935Sensor::readWatchdogThreshold() {
  return readRegister(REGISTER_WDTH, MASK_WDTH, POSITION_WDTH);
}

void AS3935Sensor::setWatchdogThreshold(uint8_t value) {

  if (value < 0 || value > 10) {
#ifdef DEBUG
    Serial << endl << "ERROR: WDTH out of the range [0:10], value: " << value;
#endif
    return;
  }
  writeRegister(REGISTER_WDTH, MASK_WDTH, POSITION_WDTH, value);
}

boolean AS3935Sensor::readMaskDisturber() {
  return readRegister(REGISTER_MASK_DIST, MASK_MASK_DIST, POSITION_MASK_DIST);
}

void AS3935Sensor::setMaskDisturber(bool enable) {
  writeRegister(REGISTER_MASK_DIST, MASK_MASK_DIST, POSITION_MASK_DIST, enable);
}

uint8_t AS3935Sensor::readTuningCapacitors() {
  return readRegister(REGISTER_TUN_CAP, MASK_TUN_CAP, POSITION_TUN_CAP);
}

void AS3935Sensor::setTuningCapacitors(uint8_t value) {
  if (value < 0 || value > 15) {
#ifdef DEBUG
    Serial << endl
           << "ERROR: TUN_CAP out of the range [0:15], value: " << value;
#endif
    return;
  }
  writeRegister(REGISTER_TUN_CAP, MASK_TUN_CAP, POSITION_TUN_CAP, value);
  delay(2);
  calibrateInternalRSOscillators();
}

void AS3935Sensor::setDefautSettings() {
  writeRegister(REGISTER_PRESET_DEFAULT, MASK_PRESET_DEFAULT,
                POSITION_PRESET_DEFAULT, AS3935_DIRECT_COMMAND);
  delay(2);
}

void AS3935Sensor::calibrateInternalRSOscillators() {
  writeRegister(REGISTER_CALIB_RCO, MASK_CALIB_RCO, POSITION_CALIB_RCO,
                AS3935_DIRECT_COMMAND);
  displayTRCOonIRQ(true);
  delay(2);
  displayTRCOonIRQ(false);
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
    Serial << endl
           << "ERROR: LCO_FDIV out of the range [16,32,64,128], value: "
           << value;
#endif
    return;
    break;
  }
  writeRegister(REGISTER_LCO_FDIV, MASK_LCO_FDIV, POSITION_LCO_FDIV,
                _valueToSave);
}

void AS3935Sensor::displayLCOonIRQ(boolean value) {
  writeRegister(REGISTER_OSCILATORS, MASK_DISP_LCO, POSITION_DISP_LCO,
                value ? 1 : 0);
}

void AS3935Sensor::displayTRCOonIRQ(boolean value) {
  Serial << endl << "INFO: Display TRCO: " << (value ? "yes" : "no");
  writeRegister(REGISTER_OSCILATORS, MASK_DISP_TRCO, POSITION_DISP_TRCO,
                value ? 1 : 0);
}

uint8_t AS3935Sensor::getTuningCapacitor() {
  uint8_t _capacitor = AS3935_OUT_OF_RANGE;
  int32_t maxtune = 17500; // 3.5% tolerance
  int32_t frequency;

#ifdef DEBUG
  Serial << endl << "INFO: AS3935: Calibrating ...";
#endif

  detachInterrupt(digitalPinToInterrupt(configuration.IRQ));
  setFrequencyDivisionForAntennaTuning(128);
  delay(2);
  for (uint8_t tune = 0; tune < 16; tune++) {
    writeRegister(REGISTER_TUN_CAP, MASK_TUN_CAP, POSITION_TUN_CAP, tune);
    delay(2);
    displayLCOonIRQ(true);
    delay(1);
    counterEnabled = true;
    interuptionsCounter = 0;
#ifdef DEBUG
    Serial << endl << " - Checking: " << tune * 8 << "pF";
#endif
    Serial << endl
           << "INFO: AS3935: Register 0x" << _HEX(REGISTER_TUN_CAP) << " : "
           << _BIN(readRegister(REGISTER_TUN_CAP, 0, 0));
    attachInterrupt(digitalPinToInterrupt(configuration.IRQ),
                    handleAntenaFrequencyInteruption, RISING);
    delay(250);
    counterEnabled = false;
    detachInterrupt(digitalPinToInterrupt(configuration.IRQ));
    displayLCOonIRQ(false);
#ifdef DEBUG

#endif
    frequency = 500000 - (interuptionsCounter * 4 * 128);

    if (frequency < 0)
      frequency = -frequency;

    if (maxtune > frequency) {
      maxtune = frequency;
      _capacitor = tune;
      actualFrequency = interuptionsCounter * 4 * 128;
    }

#ifdef DEBUG
    Serial << endl
           << " - Interupts: " << (interuptionsCounter * 4 * 128)
           << ", Difference: " << frequency << ", "
           << (frequency * 100) / 500000 << "%";
#endif
  }

#ifdef DEBUG
  Serial << endl << " - MaxTune: " << maxtune;
#endif

  if (maxtune >= 17500) // max. 3.5%
    _capacitor = AS3935_OUT_OF_RANGE;

#ifdef DEBUG
  Serial << endl << "INFO: AS3935: Tuning Capacitor: " << _capacitor;
#endif

  attachInterrupt(digitalPinToInterrupt(configuration.IRQ),
                  handleSignalDetectionInteruption, RISING);

  return _capacitor;
}

#endif
