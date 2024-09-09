#if !defined(__RADIO_H__)
#define __RADIO_H__

#include "libs/RF24/RF24.h"
#include <functional>
#include <pico/stdlib.h>

#define CE_PIN            22
#define CSN_PIN           17
#define IRQ_PIN           6
#define RADIO_CHANNEL     92
#define RADIO_BUFFER_SIZE 32u * 6u * 3u

uint8_t address[][6] = {"1Node", "2Node", "3Node", "4Node"};
const int radio_num = 0;

using radio_callback_t = std::function<void(const uint8_t *, uint)>;

class Radio {
private:
  RF24 *_radio;
  radio_callback_t _callback = nullptr;

public:
  uint8_t buffer[RADIO_BUFFER_SIZE];
  uint8_t pa_size = 32;

  Radio() { _radio = new RF24(CE_PIN, CSN_PIN); }
  virtual ~Radio() { delete _radio; }

  bool init() {
    if (!_radio->begin()) return false;

    _radio->setDataRate(RF24_1MBPS);
    _radio->setPALevel(RF24_PA_LOW);
    _radio->setChannel(RADIO_CHANNEL);
    _radio->setAutoAck(false);
    _radio->openWritingPipe(address[0]);
    pa_size = _radio->getPayloadSize();

    for (size_t i = 1; i < 4; ++i) {
      _radio->openReadingPipe(i, address[i]);
      _radio->setAutoAck(i, false);
    }

    _radio->startListening();
    return true;
  }

  inline void add_read_callback(radio_callback_t fn) {
    _callback = std::move(fn);
  }

  void try_read() {
    if (not _radio->available()) return;

    uint rx_bytes_num = 0;
    memset(buffer, 0, RADIO_BUFFER_SIZE);
    while (_radio->available() && rx_bytes_num < RADIO_BUFFER_SIZE) {
      _radio->read(buffer, pa_size);
      rx_bytes_num += pa_size;
    }

    if (rx_bytes_num > 0 and _callback) _callback(buffer, rx_bytes_num);
  }
};

#endif // __RADIO_H__
