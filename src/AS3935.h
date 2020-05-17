/** 
 * AS3935 library
 * - for ESP8266/8255
 * - I2C Bus only
 * Based on documentaion AS3935 doc:
 * https://github.com/tschaban/AS3935-I2C-ESP/raw/master/doc/Sensor%20AS3935.pdf
 * 2020 MIT Licencse Tschaban-A https://github.com/tschaban
 **/

#ifndef _AS3935_Library_h
#define _AS3935_Library_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <AS3935-defs.h>
#include <Wire.h>

#ifdef DEBUG
#include <Streaming.h>
#endif

class AS3935Sensor {
public:
  AS3935Sensor();
  ~AS3935Sensor(void);

  void begin(uint8_t, uint8_t, uint8_t, uint8_t);

  uint8_t readIntrruptReason(void);

  boolean isIndoor(void);
  boolean isOutdoor(void);
  void setIndoor(bool);

  uint8_t readDistanceToStorm(void);

  uint8_t readMinimumNumberOfLightning(void);
  void setMinimumNumberOfLightning(uint8_t);

  uint8_t readNoiseFloorLevel(void);
  void setNoiseFloorLevel(uint8_t);
  void increaseNoiseFloorLevel(void);
  void descreseNoiseFloorLevel(void);

  uint8_t readSpikeRejection();
  void setSpikeRejection(uint8_t);

  uint8_t readWatchdogThreshold();
  void setWatchdogThreshold(uint8_t);

  boolean readMaskDisturber();
  void setMaskDisturber(bool);

  uint8_t readFrequencyDivisionForAntennaTuning();
  void setFrequencyDivisionForAntennaTuning(uint8_t);

  void setDefautSettings();
  void calibrateInternalRSOscillators();
  void clearStatistics();

private:
  TwoWire I2CBus;
  uint8_t sensorAddress;
  uint8_t sensorIRQ;
  uint8_t readRegister(uint8_t registerNumber);
  uint8_t readRegister(uint8_t registerNumber, uint8_t mask, uint8_t readFrom);
  void writeRegister(uint8_t registerNumber, uint8_t mask, uint8_t writeFrom,
                     uint8_t value);
};

#endif
