#if !defined(__RADIO_H__)
#define __RADIO_H__

#include "libs/RF24/RF24.h"
#include "observer.hpp"
#include <pico/stdlib.h>

#define CE_PIN 22
#define CSN_PIN 17
#define IRQ_PIN 6
#define RADIO_CHANNEL 92
#define RADIO_BUFFER_SIZE 64

uint8_t address[][6] = {"1Node", "2Node", "3Node", "4Node"};
const int radio_num = 0;

class Radio : public Publisher {
private:
  RF24 *_radio;

public:
  uint8_t buffer[RADIO_BUFFER_SIZE];

  Radio() { _radio = new RF24(CE_PIN, CSN_PIN); }
  virtual ~Radio() { delete _radio; }

  bool init() {
    if (!_radio->begin())
      return false;

    _radio->setDataRate(RF24_1MBPS);
    _radio->enableDynamicAck();
    _radio->setPALevel(RF24_PA_LOW);
    _radio->setChannel(RADIO_CHANNEL);
    _radio->setAutoAck(true);
    _radio->openWritingPipe(address[radio_num]);

    for (size_t i = 1; i < 4; ++i) {
      _radio->openReadingPipe(i-1, address[i]);
    }
    
    _radio->startListening();

    return true;
  }

  void try_read() {
    if (_radio->available()) {
      _radio->read(buffer, _radio->getPayloadSize());
      notify();
    }
  }
};

#endif // __RADIO_H__
