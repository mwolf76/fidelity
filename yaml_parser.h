#ifndef FIDELITY_YAML_PARSER_H
#define FIDELITY_YAML_PARSER_H

#include <string>
#include <vector>
#include "config.h"

namespace fidelity {

class SimpleYamlParser {
public:
    struct WiFiConfig {
        std::string ssid;
        std::string psk;
    };

    struct FullConfiguration {
        std::vector<WiFiConfig> profiles;
        MonitoringConfig monitoring;
        InterfaceConfig interfaces;
    };
    
    static std::vector<WiFiConfig> parseWiFiProfiles(const std::string& filename);
    static FullConfiguration parseConfiguration(const std::string& filename);
};

}

#endif