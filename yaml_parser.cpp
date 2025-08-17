#include "yaml_parser.h"
#include <iostream>
#include <stdexcept>
#include <yaml-cpp/yaml.h>

namespace fidelity {

    std::vector<SimpleYamlParser::WiFiConfig> SimpleYamlParser::parseWiFiProfiles(const std::string& filename)
    {
        std::vector<WiFiConfig> profiles;

        try {
            YAML::Node config = YAML::LoadFile(filename);

            if (!config["profiles"]) {
                throw std::runtime_error("No 'profiles' section found in configuration file");
            }

            const YAML::Node& wifiProfiles = config["profiles"];

            if (!wifiProfiles.IsSequence()) {
                throw std::runtime_error("'profiles' must be a list");
            }

            for (const auto& profile : wifiProfiles) {
                WiFiConfig wifiConfig;

                if (!profile["ssid"]) {
                    throw std::runtime_error("Missing 'ssid' in WiFi profile");
                }
                if (!profile["psk"]) {
                    throw std::runtime_error("Missing 'psk' in WiFi profile");
                }

                wifiConfig.ssid = profile["ssid"].as<std::string>();
                wifiConfig.psk = profile["psk"].as<std::string>();

                profiles.push_back(wifiConfig);
            }

        } catch (const YAML::Exception& e) {
            throw std::runtime_error("Error parsing YAML file: " + std::string(e.what()));
        }

        return profiles;
    }

    SimpleYamlParser::FullConfiguration SimpleYamlParser::parseConfiguration(const std::string& filename)
    {
        FullConfiguration fullConfig;

        try {
            YAML::Node config = YAML::LoadFile(filename);

            // Parse WiFi profiles
            if (config["profiles"]) {
                const YAML::Node& wifiProfiles = config["profiles"];

                if (!wifiProfiles.IsSequence()) {
                    throw std::runtime_error("'profiles' must be a list");
                }

                for (const auto& profile : wifiProfiles) {
                    WiFiConfig wifiConfig;

                    if (!profile["ssid"]) {
                        throw std::runtime_error("Missing 'ssid' in WiFi profile");
                    }
                    if (!profile["psk"]) {
                        throw std::runtime_error("Missing 'psk' in WiFi profile");
                    }

                    wifiConfig.ssid = profile["ssid"].as<std::string>();
                    wifiConfig.psk = profile["psk"].as<std::string>();

                    fullConfig.profiles.push_back(wifiConfig);
                }
            }

            // Parse monitoring configuration
            if (config["monitoring"]) {
                const YAML::Node& monitoring = config["monitoring"];
                
                if (monitoring["check_interval"]) {
                    fullConfig.monitoring.check_interval = monitoring["check_interval"].as<int>();
                }
                if (monitoring["failure_threshold"]) {
                    fullConfig.monitoring.failure_threshold = monitoring["failure_threshold"].as<int>();
                }
                if (monitoring["ping_timeout"]) {
                    fullConfig.monitoring.ping_timeout = monitoring["ping_timeout"].as<int>();
                }
                if (monitoring["target_host"]) {
                    fullConfig.monitoring.target_host = monitoring["target_host"].as<std::string>();
                }
            }

            // Parse interface configuration
            if (config["interface"]) {
                const YAML::Node& interfaces = config["interface"];
                
                if (interfaces["wlan"]) {
                    fullConfig.interfaces.wlan = interfaces["wlan"].as<std::string>();
                }
                if (interfaces["lan"]) {
                    fullConfig.interfaces.lan = interfaces["lan"].as<std::string>();
                }
            }

        } catch (const YAML::Exception& e) {
            throw std::runtime_error("Error parsing YAML file: " + std::string(e.what()));
        }

        return fullConfig;
    }

} // namespace fidelity