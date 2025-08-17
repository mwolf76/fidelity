#include "network_service.h"
#include <cstdlib>
#include <iostream>

namespace fidelity {

    bool NetworkService::restartNetworking()
    {
        std::cout << "Restarting networking service...\n";

        if (executeCommand("systemctl restart networking")) {
            std::cout << "Networking service restarted successfully\n";
            return true;
        }

        std::cerr << "Failed to restart networking service, trying ifdown/ifup...\n";

        executeCommand("ifdown wlp2s0");
        if (executeCommand("ifup wlp2s0")) {
            std::cout << "Network interface restarted successfully\n";
            return true;
        }

        std::cerr << "Failed to restart network interface\n";
        return false;
    }

    bool NetworkService::executeCommand(const char* command)
    {
        return std::system(command) == 0;
    }

} // namespace fidelity