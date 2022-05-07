#include <iostream>
#include <random>
#include "TrafficLight.h"
#include <thread>
#include <chrono>

using namespace std::chrono_literals;

/* Implementation of class "MessageQueue" */

template <typename T>
T && MessageQueue<T>::receive()
{
    // COMPLETE
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the "receive" function.
    std::unique_lock<std::mutex> uniqueLock(_mutex);
    _cond.wait(uniqueLock, [this] { return !this->_queue.empty(); });
    T message = std::move(_queue.back());
    _queue.pop_back();
    _queue.clear(); // Make sure cars aren't receiving old messages

    return std::move(message);
}

template <typename T>
void MessageQueue<T>::send(const T && msg)
{
    // COMPLETE
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
    std::lock_guard<std::mutex> uLock(_mutex);
    _queue.emplace_back(std::move(msg));
    _cond.notify_one();
}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight()
{
    _last_phase_change_time = std::chrono::high_resolution_clock::now();
    _currentPhase = TrafficLightPhase::red;

    std::random_device rd;
    _gen = std::mt19937 (rd());
    _dist = std::uniform_real_distribution<long double>(minLightChangeSeconds, maxLightChangeSeconds);
    _last_phase_change_time = std::chrono::high_resolution_clock::now();
    randomizeSecondsToWait();
}

void TrafficLight::waitForGreen()
{
    // COMPLETE
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
    while (true)
    {
        auto message = _messageQueue.receive();
        if (message == TrafficLightPhase::green)
        {
            return;
        }
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::randomizeSecondsToWait()
{
    long double s = _dist(_gen);
    _seconds_to_wait = std::chrono::duration<long double>(s);
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called.
    // To do this, use the thread queue in the base class.
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles.
    while (true)
    {
        auto current_time = std::chrono::high_resolution_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::seconds>(current_time - _last_phase_change_time);
        if (diff < _seconds_to_wait)
        {
            continue;
        }

        _currentPhase =
                _currentPhase == TrafficLightPhase::green ? TrafficLightPhase::red : TrafficLightPhase::green;
        auto return_value = _currentPhase;
        _messageQueue.send(std::move(return_value));

        _last_phase_change_time = std::chrono::high_resolution_clock().now();
        randomizeSecondsToWait();

        std::this_thread::sleep_for(TrafficLight::LOOP_SLEEP);
    }
}

const std::chrono::duration<long double> TrafficLight::LOOP_SLEEP = 1ms;