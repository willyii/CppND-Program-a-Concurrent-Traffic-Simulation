#include "TrafficLight.h"

#include <chrono>
#include <ctime>
#include <iostream>
#include <random>
#include <system_error>

/* Implementation of class "MessageQueue" */

template <typename T> T MessageQueue<T>::receive() {
  // FP.5a : The method receive should use std::unique_lock<std::mutex> and
  // _condition.wait()
  // to wait for and receive new messages and pull them from the queue using
  // move semantics.
  // The received object should then be returned by the receive function.
  std::unique_lock<std::mutex> ulock(_mutex);
  _cond.wait(ulock, [this] { return !this->_message.empty(); });
  auto phase = std::move(_message.back());
  _message.pop_back();
  return phase;
}

template <typename T> void MessageQueue<T>::send(T &&msg) {
  // FP.4a : The method send should use the mechanisms
  // std::lock_guard<std::mutex>
  // as well as _condition.notify_one() to add a new message to the queue and
  // afterwards send a notification.
  std::lock_guard<std::mutex> ulock(_mutex);
  _message.emplace_back(std::move(msg));
  _cond.notify_one();
}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight() { _currentPhase = TrafficLightPhase::red; }

void TrafficLight::waitForGreen() {
  // FP.5b : add the implementation of the method waitForGreen, in which an
  // infinite while-loop
  // runs and repeatedly calls the receive function on the message queue.
  // Once it receives TrafficLightPhase::green, the method returns.
  while (true) {
    TrafficLightPhase p = _msg.receive();
    if (p == TrafficLightPhase::green)
      return;
  }
}

TrafficLightPhase TrafficLight::getCurrentPhase() { return _currentPhase; }

void TrafficLight::simulate() {
  threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases() {
  auto lastUpdate = std::chrono::system_clock::now();
  double duration = rand() % 6 + 4;
  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    auto passed = std::chrono::duration_cast<std::chrono::seconds>(
                      std::chrono::system_clock::now() - lastUpdate)
                      .count();
    if (passed > duration) {
      if (getCurrentPhase() == TrafficLightPhase::red)
        setPahse(TrafficLightPhase::green);
      else
        setPahse(TrafficLightPhase::red);

      lastUpdate = std::chrono::system_clock::now();
      _msg.send(std::move(_currentPhase));
    }
  }
}
