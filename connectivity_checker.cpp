#include "connectivity_checker.h"
#include <cstdlib>
#include <sstream>

namespace fidelity {

    bool ConnectivityChecker::checkConnectivity(const std::string& host, int timeout)
    {
        std::ostringstream command;
        command << "ping -c 1 -W " << timeout << " " << host << " > /dev/null 2>&1";

        return system(command.str().c_str()) == 0;
    }

} // namespace fidelity