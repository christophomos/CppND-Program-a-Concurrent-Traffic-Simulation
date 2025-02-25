#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include <chrono>
#include <random>

#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;

enum class TrafficLightPhase
{
    red, green
};

// COMPLETE
// FP.3 Define a class „MessageQueue“ which has the public methods send and receive. 
// Send should take an rvalue reference of type TrafficLightPhase whereas receive should return this type. 
// Also, the class should define an std::dequeue called _queue, which stores objects of type TrafficLightPhase. 
// Also, there should be an std::condition_variable as well as an std::mutex as private members. 

template <class T>
class MessageQueue
{
public:
    void send(const T && traffic_light_phase);
    T && receive();
private:
    std::condition_variable _cond;
    std::mutex _mutex;
    std::deque<T> _queue;
};

// COMPLETE
// FP.1 : Define a class „TrafficLight“ which is a child class of TrafficObject. 
// The class shall have the public methods „void waitForGreen()“ and „void simulate()“ 
// as well as „TrafficLightPhase getCurrentPhase()“, where TrafficLightPhase is an enum that 
// can be either „red“ or „green“. Also, add the private method „void cycleThroughPhases()“. 
// Furthermore, there shall be the private member _currentPhase which can take „red“ or „green“ as its value. 

class TrafficLight : public TrafficObject
{
public:
    // constructor / destructor
    TrafficLight();

    // getters / setters
    TrafficLightPhase getCurrentPhase();

    // typical behaviour methods
    void waitForGreen();
    void simulate();

    static constexpr long double minLightChangeSeconds = 4.0;
    static constexpr long double maxLightChangeSeconds = 6.0;
private:
    // How long ago did the traffic light last change phase?
    std::chrono::time_point<std::chrono::high_resolution_clock> _last_phase_change_time;
    std::chrono::duration<long double> _seconds_to_wait;
    // typical behaviour methods
    void cycleThroughPhases();

    // COMPLETE
    // FP.4b : create a private member of type MessageQueue for messages of type TrafficLightPhase 
    // and use it within the infinite loop to push each new TrafficLightPhase into it by calling 
    // send in conjunction with move semantics.
    MessageQueue<TrafficLightPhase> _messageQueue;

    TrafficLightPhase _currentPhase;
    static const std::chrono::duration<long double> LOOP_SLEEP;

    std::mt19937 _gen;
    std::uniform_real_distribution<long double> _dist;


    void randomizeSecondsToWait();
};

#endif