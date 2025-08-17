#include "config.h"
#include "yaml_parser.h"
#include <stdexcept>
#include <iostream>

namespace fidelity {

    Configuration::Configuration()
        : currentProfileIndex(0)
    {
        // Default configuration - will be overridden if config file is provided
        // No default WiFi profiles - must be configured via config file
        // monitoringConfig and interfaceConfig use default values from header
    }
    
    Configuration::Configuration(const std::string& configFile)
        : currentProfileIndex(0)
    {
        if (!loadFromFile(configFile)) {
            throw std::runtime_error("Failed to load configuration from: " + configFile);
        }
    }
    
    bool Configuration::loadFromFile(const std::string& configFile) {
        try {
            auto config = SimpleYamlParser::parseConfiguration(configFile);
            
            if (config.profiles.empty()) {
                std::cerr << "No WiFi profiles found in configuration file\n";
                return false;
            }
            
            profiles.clear();
            for (const auto& profile : config.profiles) {
                profiles.emplace_back(profile.ssid, profile.psk);
            }
            
            monitoringConfig = config.monitoring;
            interfaceConfig = config.interfaces;
            
            currentProfileIndex = 0;
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error loading configuration: " << e.what() << "\n";
            return false;
        }
    }

    const std::vector<WifiProfile> &Configuration::getProfiles() const {
        return profiles;
    }

    size_t Configuration::getCurrentProfileIndex() const {
        return currentProfileIndex;
    }

    void Configuration::switchToNextProfile() {
        if (profiles.empty()) {
            throw std::runtime_error("No WiFi profiles configured");
        }
        currentProfileIndex = (currentProfileIndex + 1) % profiles.size();
    }

    const WifiProfile &Configuration::getCurrentProfile() const {
        if (profiles.empty()) {
            throw std::runtime_error("No WiFi profiles configured");
        }
        return profiles[currentProfileIndex];
    }

    const MonitoringConfig &Configuration::getMonitoringConfig() const {
        return monitoringConfig;
    }

    const InterfaceConfig &Configuration::getInterfaceConfig() const {
        return interfaceConfig;
    }
}
