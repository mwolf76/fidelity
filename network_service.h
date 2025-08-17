#ifndef FIDELITY_NETWORK_SERVICE_H
#define FIDELITY_NETWORK_SERVICE_H

namespace fidelity {

    class NetworkService {
    public:
        static bool restartNetworking();

    private:
        static bool executeCommand(const char* command);
    };

} // namespace fidelity

#endif