#if !defined(__RADIO_H__)
#define __RADIO_H__

#include "libs/RF24/RF24.h"
#include <pico/stdlib.h>
#include "observer.hpp"

#define CE_PIN 22
#define CSN_PIN 17
#define IRQ_PIN 6
#define RADIO_CHANNEL 120
#define RADIO_BUFFER_SIZE 64

uint8_t address[][6] = {"1Node", "2Node"};

class Radio: public Publisher {
private:
  uint8_t _buffer[RADIO_BUFFER_SIZE];
  RF24 *_radio;

public:
  Radio() { _radio = new RF24(CE_PIN, CSN_PIN); }
  ~Radio() { delete _radio; }

  bool init() {
    if (!_radio->begin())
      return false;

    _radio->setPALevel(RF24_PA_MIN);
    _radio->setChannel(RADIO_CHANNEL);
    _radio->openWritingPipe(address[0]);
    _radio->openReadingPipe(1, address[1]);
    _radio->startListening();

    return true;
  }

  void try_read() {
    if (_radio->available()) {
      _radio->read(_buffer, _radio->getPayloadSize());
      notify();
    }
  }
};

#endif // __RADIO_H__
