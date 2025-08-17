#ifndef FIDELITY_NETWORK_WRITER_H
#define FIDELITY_NETWORK_WRITER_H

#include "config.h"
#include <string>

namespace fidelity {

    class NetworkInterfaceWriter {
    public:
        static constexpr const char* INTERFACES_FILE = "/etc/network/interfaces";
        static constexpr const char* INTERFACES_BACKUP = "/etc/network/interfaces.bak";

        static bool writeConfiguration(const WifiProfile& profile, const InterfaceConfig& interfaces);

    private:
        static std::string getHeaderContent(const InterfaceConfig& interfaces);
        static std::string getWifiConfiguration(const WifiProfile& profile, const InterfaceConfig& interfaces);
    };

} // namespace fidelity

#endif