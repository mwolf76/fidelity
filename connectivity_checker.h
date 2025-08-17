#ifndef FIDELITY_CONNECTIVITY_CHECKER_H
#define FIDELITY_CONNECTIVITY_CHECKER_H

#include <cstdint>
#include <string>

namespace fidelity {

    class ConnectivityChecker {
    public:
        static bool checkConnectivity(const std::string& host = "8.8.8.8", int timeout = 3);
    };

} // namespace fidelity

#endif