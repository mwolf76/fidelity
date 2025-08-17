#ifndef FIDELITY_CONFIG_H
#define FIDELITY_CONFIG_H

#include <string>
#include <utility>
#include <vector>

namespace fidelity {

    struct WifiProfile {
        std::string ssid;
        std::string psk;

        WifiProfile(std::string ssid, std::string psk)
            : ssid(std::move(ssid))
            , psk(std::move(psk))
        {}
    };

    struct MonitoringConfig {
        int check_interval = 10;        // Seconds between connectivity checks
        int failure_threshold = 3;      // Number of failed checks before switching
        int ping_timeout = 5;           // Timeout for each ping attempt
        std::string target_host = "8.8.8.8";  // Host to ping for connectivity check
    };

    struct InterfaceConfig {
        std::string wlan = "wlp2s0";    // Wireless interface name
        std::string lan = "enp0s25";    // Ethernet interface name
    };

    class Configuration {
    public:
        Configuration();
        explicit Configuration(const std::string& configFile);

        [[nodiscard]] const std::vector<WifiProfile> &getProfiles() const;
        [[nodiscard]] size_t getCurrentProfileIndex() const;
        [[nodiscard]] const WifiProfile &getCurrentProfile() const;
        [[nodiscard]] const MonitoringConfig &getMonitoringConfig() const;
        [[nodiscard]] const InterfaceConfig &getInterfaceConfig() const;

        void switchToNextProfile();
        bool loadFromFile(const std::string& configFile);

    private:
        std::vector<WifiProfile> profiles;
        size_t currentProfileIndex;
        MonitoringConfig monitoringConfig;
        InterfaceConfig interfaceConfig;
    };
}

#endif
