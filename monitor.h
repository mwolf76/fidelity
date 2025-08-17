#ifndef FIDELITY_MONITOR_H
#define FIDELITY_MONITOR_H

#include <thread>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include "config.h"

namespace fidelity {

class NetworkMonitor {
public:
    
    NetworkMonitor(Configuration& config);
    ~NetworkMonitor();
    
    void start();
    void stop();
    bool isRunning() const;
    
    bool waitForSwitchRequest(int timeoutSeconds = -1);
    void acknowledgeSwitchComplete();
    
private:
    void monitorThread();
    
    Configuration& configuration;
    std::thread workerThread;
    std::atomic<bool> running;
    std::atomic<bool> stopRequested;
    
    std::mutex switchMutex;
    std::condition_variable switchCondition;
    bool switchRequested;
    
    int consecutiveFailures;
};

}

#endif