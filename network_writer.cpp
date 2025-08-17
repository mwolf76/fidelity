#include "network_writer.h"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fidelity {

    std::string NetworkInterfaceWriter::getHeaderContent(const InterfaceConfig& interfaces)
    {
        std::string content = R"(# This file describes the network interfaces available on your system
# and how to activate them. For more information, see interfaces(5).

source /etc/network/interfaces.d/*

# The loopback network interface
auto lo
iface lo inet loopback

# The secondary network interface (LAN)
auto )";
        content += interfaces.lan + "\n";
        content += "iface " + interfaces.lan + R"( inet static
address 192.168.2.1

# The primary network interface
allow-hotplug )";
        content += interfaces.wlan + "\n";
        return content;
    }

    std::string NetworkInterfaceWriter::getWifiConfiguration(const WifiProfile& profile, const InterfaceConfig& interfaces)
    {
        return "iface " + interfaces.wlan + " inet dhcp\n"
               "wpa-ssid " +
               profile.ssid + "\n"
                              "wpa-psk  " +
               profile.psk + "\n";
    }

    bool NetworkInterfaceWriter::writeConfiguration(const WifiProfile& profile, const InterfaceConfig& interfaces)
    {
        try {
            if (std::filesystem::exists(INTERFACES_FILE)) {
                std::filesystem::copy_file(INTERFACES_FILE, INTERFACES_BACKUP,
                                           std::filesystem::copy_options::overwrite_existing);
            }

            std::ofstream file(INTERFACES_FILE);
            if (!file.is_open()) {
                std::cerr << "Failed to open " << INTERFACES_FILE << " for writing\n";
                return false;
            }

            file << getHeaderContent(interfaces);
            file << getWifiConfiguration(profile, interfaces);

            file.close();

            std::filesystem::permissions(INTERFACES_FILE,
                                         std::filesystem::perms::owner_read | std::filesystem::perms::owner_write |
                                             std::filesystem::perms::group_read | std::filesystem::perms::others_read);

            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error writing network configuration: " << e.what() << "\n";
            return false;
        }
    }

} // namespace fidelity