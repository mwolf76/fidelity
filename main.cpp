#include <iostream>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>
#include <sstream>
#include <fstream>

#include "config.h"
#include "network_writer.h"
#include "network_service.h"
#include "connectivity_checker.h"
#include "monitor.h"
#include "logger.h"

std::atomic<bool> g_shutdown(false);

void signalHandler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        LOG_INFO("Shutdown signal received");
        g_shutdown = true;
    }
}

void switchNetworkProfile(fidelity::Configuration& config) {
    config.switchToNextProfile();
    const auto& profile = config.getCurrentProfile();
    
    std::stringstream msg;
    msg << "Switching to WiFi profile: " << profile.ssid;
    LOG_INFO(msg.str());
    
    if (!fidelity::NetworkInterfaceWriter::writeConfiguration(profile, config.getInterfaceConfig())) {
        LOG_ERROR("Failed to write network configuration");
        return;
    }
    
    sleep(2);
    
    if (!fidelity::NetworkService::restartNetworking()) {
        LOG_ERROR("Failed to restart networking service");
        return;
    }
    
    sleep(5);
    
    const auto& monitoringConfig = config.getMonitoringConfig();
    if (fidelity::ConnectivityChecker::checkConnectivity(monitoringConfig.target_host, monitoringConfig.ping_timeout)) {
        LOG_INFO("Network switch successful, connectivity restored");
    } else {
        LOG_WARNING("Network switch complete but connectivity not yet restored");
    }
}

void sleep(const int seconds)
{
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

int main(int argc, char* argv[]) {
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
    
    fidelity::Logger::getInstance().enableConsoleOutput(true);
    
    bool daemonMode = false;
    std::string configFile = "/etc/fidelity/fidelity.yaml";
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--daemon") {
            daemonMode = true;
        } else if (arg == "--config" || arg == "-c") {
            if (i + 1 < argc) {
                configFile = argv[++i];
            } else {
                std::cerr << "Error: --config requires a file path\n";
                std::cerr << "Usage: " << argv[0] << " [--daemon] [--config <file>]\n";
                return 1;
            }
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: " << argv[0] << " [options]\n";
            std::cout << "Options:\n";
            std::cout << "  --daemon        Run in daemon mode (no console output)\n";
            std::cout << "  --config <file> Specify configuration file (default: /etc/fidelity/fidelity.yaml)\n";
            std::cout << "  --help          Show this help message\n";
            return 0;
        }
    }
    
    if (daemonMode) {
        fidelity::Logger::getInstance().enableConsoleOutput(false);
        LOG_INFO("Starting Fidelity in daemon mode");
    } else {
        LOG_INFO("Starting Fidelity in console mode");
    }
    
    LOG_INFO("Fidelity network monitor starting...");
    
    fidelity::Configuration config;
    
    std::ifstream checkFile(configFile);
    if (checkFile.good()) {
        checkFile.close();
        std::stringstream msg;
        msg << "Loading configuration from: " << configFile;
        LOG_INFO(msg.str());
        
        if (!config.loadFromFile(configFile)) {
            LOG_ERROR("Failed to load configuration file");
            return 1;
        }
    } else {
        LOG_WARNING("Configuration file not found, using default profiles");
    }
    
    if (config.getProfiles().empty()) {
        LOG_ERROR("No WiFi profiles configured");
        return 1;
    }
    
    std::stringstream msg;
    msg << "Loaded " << config.getProfiles().size() << " WiFi profiles";
    LOG_INFO(msg.str());
    
    const auto& initialProfile = config.getCurrentProfile();
    msg.str("");
    msg << "Initial profile: " << initialProfile.ssid;
    LOG_INFO(msg.str());
    
    if (!fidelity::NetworkInterfaceWriter::writeConfiguration(initialProfile, config.getInterfaceConfig())) {
        LOG_ERROR("Failed to write initial network configuration");
        return 1;
    }
    
    fidelity::NetworkMonitor monitor(config);
    monitor.start();
    
    LOG_INFO("Entering main daemon loop");
    
    while (!g_shutdown.load()) {
        if (monitor.waitForSwitchRequest(1)) {
            if (!g_shutdown.load()) {
                switchNetworkProfile(config);
                monitor.acknowledgeSwitchComplete();
            }
        }
    }
    
    LOG_INFO("Shutting down Fidelity...");
    monitor.stop();
    
    LOG_INFO("Fidelity stopped");
    
    return 0;
}