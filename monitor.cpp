#include "monitor.h"
#include "connectivity_checker.h"
#include "logger.h"
#include <chrono>
#include <sstream>

namespace fidelity {

    NetworkMonitor::NetworkMonitor(Configuration& config)
        : configuration(config)
        , running(false)
        , stopRequested(false)
        , switchRequested(false)
        , consecutiveFailures(0)
    {
    }

    NetworkMonitor::~NetworkMonitor()
    {
        stop();
    }

    void NetworkMonitor::start()
    {
        if (running.load()) {
            LOG_WARNING("Monitor already running");
            return;
        }

        stopRequested = false;
        running = true;
        workerThread = std::thread(&NetworkMonitor::monitorThread, this);
        LOG_INFO("Network monitor started");
    }

    void NetworkMonitor::stop()
    {
        if (!running.load()) {
            return;
        }

        LOG_INFO("Stopping network monitor...");
        stopRequested = true;

        if (workerThread.joinable()) {
            workerThread.join();
        }

        running = false;
        LOG_INFO("Network monitor stopped");
    }

    bool NetworkMonitor::isRunning() const
    {
        return running.load();
    }

    void NetworkMonitor::monitorThread()
    {
        LOG_INFO("Monitor thread started");

        while (!stopRequested.load()) {
            const auto& monitoringConfig = configuration.getMonitoringConfig();
            if (ConnectivityChecker::checkConnectivity(monitoringConfig.target_host, monitoringConfig.ping_timeout)) {
                if (consecutiveFailures > 0) {
                    LOG_INFO("Connectivity restored");
                    consecutiveFailures = 0;
                }
            } else {
                consecutiveFailures++;
                std::stringstream msg;
                msg << "Connectivity check failed (" << consecutiveFailures
                    << "/" << configuration.getMonitoringConfig().failure_threshold << ")";
                LOG_WARNING(msg.str());

                if (consecutiveFailures >= configuration.getMonitoringConfig().failure_threshold) {
                    LOG_ERROR("Max consecutive failures reached, requesting network switch");

                    {
                        std::lock_guard lock(switchMutex);
                        switchRequested = true;
                    }

                    switchCondition.notify_one();
                    consecutiveFailures = 0;
                }
            }

            for (int i = 0; i < configuration.getMonitoringConfig().check_interval && !stopRequested.load(); ++i) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }

        LOG_INFO("Monitor thread stopped");
    }

    bool NetworkMonitor::waitForSwitchRequest(int timeoutSeconds)
    {
        std::unique_lock<std::mutex> lock(switchMutex);

        if (timeoutSeconds < 0) {
            switchCondition.wait(lock, [this] { return switchRequested; });
        } else {
            if (!switchCondition.wait_for(lock,
                                          std::chrono::seconds(timeoutSeconds),
                                          [this] { return switchRequested; })) {
                return false;
            }
        }

        return true;
    }

    void NetworkMonitor::acknowledgeSwitchComplete()
    {
        std::lock_guard<std::mutex> lock(switchMutex);
        switchRequested = false;
    }

} // namespace fidelity