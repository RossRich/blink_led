#if !defined(__OBSERVER_BASE_H__)
#define __OBSERVER_BASE_H__

class Subscriber {
public:
  Subscriber() {}
  virtual ~Subscriber() {}
  virtual void update() {};
};

class Publisher {
private:
  Subscriber *_subs[3];

protected:
  uint8_t subs_num = 0;

public:
  Publisher() {}
  virtual ~Publisher() {}

  void add_sub(Subscriber *sub) {
    if (subs_num > 2)
      return;

    _subs[subs_num] = sub;
    subs_num++;
  }

  void notify() {
    if (_subs != nullptr and subs_num > 0) {
      for (size_t i = 0; i < subs_num; ++i) {
        _subs[i]->update();
      }
    }
  }
};

#endif // __OBSERVER_BASE_H__
