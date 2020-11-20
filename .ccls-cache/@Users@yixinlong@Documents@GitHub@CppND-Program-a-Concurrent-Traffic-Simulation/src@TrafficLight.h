#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <condition_variable>
#include <deque>
#include <mutex>

#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;

template <class T> class MessageQueue {
public:
  MessageQueue() {}
  T receive();
  void send(T &&v);

private:
  std::mutex _mutex;
  std::condition_variable _cond;
  std::deque<T> _message;
};

enum TrafficLightPhase { red, green };

class TrafficLight : public TrafficObject {
public:
  // constructor / desctructor
  TrafficLight();
  ~TrafficLight(){};

  // getters / setters
  void setPahse(TrafficLightPhase phase) { _currentPhase = phase; }

  // typical behaviour methods
  void waitForGreen();
  void simulate();
  TrafficLightPhase getCurrentPhase();

private:
  // typical behaviour methods
  void cycleThroughPhases();
  TrafficLightPhase _currentPhase;

  MessageQueue<TrafficLightPhase> _msg;
  std::condition_variable _condition;
  std::mutex _mutex;
};

#endif
