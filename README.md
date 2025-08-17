# Fidelity

A robust network connectivity daemon for Debian-based systems that automatically switches between WiFi networks when internet connectivity is lost.

## Overview

Fidelity is a lightweight C++ daemon that continuously monitors your network connectivity and automatically switches between predefined WiFi configurations when the current connection loses internet access. It's designed for systems that need to maintain reliable internet connectivity by seamlessly failing over to alternative WiFi networks.

## Features

- **Automatic Failover**: Detects internet connectivity loss and switches to alternate WiFi networks
- **Multiple WiFi Profiles**: Support for configuring multiple WiFi networks with priorities
- **Continuous Monitoring**: Background thread constantly checks internet connectivity
- **Debian Integration**: Works with `/etc/network/interfaces` for network configuration
- **Configurable Parameters**: Customizable check intervals, retry counts, and timeouts
- **Robust Logging**: Comprehensive logging for monitoring and debugging
- **Signal Handling**: Graceful shutdown on SIGINT/SIGTERM

## How It Works

1. **Connectivity Monitoring**: Fidelity runs a worker thread that pings `8.8.8.8` every 10 seconds
2. **Failure Detection**: If three consecutive pings fail, it triggers a network switch
3. **Network Switching**: The daemon rewrites `/etc/network/interfaces` with the next WiFi profile
4. **Service Restart**: Restarts the networking service to apply the new configuration
5. **Verification**: Checks if the new connection has internet access

## Requirements

- Debian-based Linux distribution (Ubuntu, Debian, etc.)
- C++20 compatible compiler (g++ 10+ or clang++ 11+)
- Autotools (automake, autoconf)
- libyaml-cpp-dev
- pthread library
- Root/sudo privileges (for network configuration)

## Installation

### From Source

```bash
# Install dependencies
sudo apt-get update
sudo apt-get install -y build-essential autotools-dev libyaml-cpp-dev

# Clone the repository
git clone https://github.com/yourusername/fidelity.git
cd fidelity

# Build and install
./autogen.sh
./configure
make
sudo make install
```

## Configuration

Fidelity uses a YAML configuration file to define WiFi profiles and monitoring parameters.

### Configuration File Location

The default configuration file should be placed at `/etc/fidelity/config.yaml` or specified via command-line argument.

### Example Configuration

```yaml
# Monitoring configuration
monitoring:
  check_interval: 10        # Seconds between connectivity checks
  failure_threshold: 3      # Number of failed checks before switching
  ping_timeout: 5           # Timeout for each ping attempt
  target_host: "8.8.8.8"    # Host to ping for connectivity check

# WiFi profiles (in priority order)
profiles:
  - ssid: "HomeWiFi"
    psk: "your_password_here"
    
  - ssid: "MobileHotspot"
    psk: "hotspot_password"
    
  - ssid: "PublicWiFi"
    psk: ""  # Open network
    
# Network interface settings
interface:
  wlan: "wlp2s0"       # e.g. Your wireless interface name
  lan: "enp0s25"       # e.g. Your ethernet interface name (if applicable)
```

## Usage

### Running as a Service

```bash
# Start the daemon
sudo systemctl start fidelity

# Enable automatic startup
sudo systemctl enable fidelity

# Check status
sudo systemctl status fidelity

# View logs
sudo journalctl -u fidelity -f
```

### Running Manually

```bash
# Run with default config
sudo ./fidelity

# Run with custom config file
sudo ./fidelity -c /path/to/config.yaml

# Run in verbose mode
sudo ./fidelity -v

# Run in daemon mode
sudo ./fidelity -d
```

### Command-Line Options

```
-c, --config <file>    Specify configuration file path
-d, --daemon           Run as daemon in background
-v, --verbose          Enable verbose logging
-h, --help             Show help message
-V, --version          Show version information
```

## Architecture

### Components

- **Main Loop**: Handles signal processing and orchestrates components
- **Monitor**: Worker thread that continuously checks connectivity
- **Connectivity Checker**: Performs ping tests to verify internet access
- **Network Writer**: Manages `/etc/network/interfaces` file updates
- **Network Service**: Handles networking service restart operations
- **Configuration Manager**: Loads and manages WiFi profiles
- **Logger**: Provides structured logging capabilities

### Network Configuration Format

Fidelity generates `/etc/network/interfaces` configurations in the following format:

```bash
# Static header (preserved)
source /etc/network/interfaces.d/*

auto lo
iface lo inet loopback

auto enp0s25
iface enp0s25 inet static
address 192.168.2.1

allow-hotplug wlp2s0

# Dynamic WiFi configuration (updated by Fidelity)
iface wlp2s0 inet dhcp
wpa-ssid YourSSID
wpa-psk YourPassword
```

## Development

### Building for Development

```bash
# Configure with debug flags
./autogen.sh
./configure CXXFLAGS="-g -O0"
make

# Run tests
make check

# Clean build
make clean
make distclean
```

### Project Structure

```
fidelity/
├── src/
│   ├── main.cpp              # Entry point
│   ├── monitor.cpp/h         # Monitoring thread
│   ├── connectivity_checker.cpp/h  # Ping-based connectivity checks
│   ├── network_writer.cpp/h  # Network configuration writer
│   ├── network_service.cpp/h # Service management
│   ├── config.cpp/h          # Configuration management
│   ├── yaml_parser.cpp/h     # YAML parsing utilities
│   └── logger.cpp/h          # Logging infrastructure
├── configure.ac              # Autotools configuration
├── Makefile.am               # Automake configuration
├── autogen.sh                # Autotools bootstrap script
└── README.md                 # This file
```

## Troubleshooting

### Common Issues

1. **Permission Denied**
   - Ensure you're running with sudo/root privileges
   - Check file permissions on `/etc/network/interfaces`

2. **Network Doesn't Switch**
   - Verify WiFi credentials in configuration
   - Check if the wireless interface name is correct
   - Review logs for error messages

3. **Connectivity Check Failures**
   - Ensure firewall allows ICMP packets
   - Try different target hosts if 8.8.8.8 is blocked
   - Adjust timeout values in configuration

### Debug Mode

Enable verbose logging to troubleshoot issues:

```bash
# Run with debug output
sudo ./fidelity -v

# Check system logs
tail -f /var/log/syslog | grep fidelity
```

## Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues for bugs and feature requests.

### Development Guidelines

1. Follow C++20 best practices
2. Maintain existing code style
3. Add tests for new features
4. Update documentation as needed
5. Ensure CI/CD checks pass

## License

[Specify your license here - e.g., MIT, GPL, etc.]

## Security Considerations

- **Credentials**: WiFi passwords are stored in the configuration file. Ensure proper file permissions (600) for `/etc/fidelity/config.yaml`
- **Root Access**: The daemon requires root privileges to modify network configurations
- **Network Security**: Use WPA2/WPA3 encryption for WiFi networks when possible

## Acknowledgments

- Built with yaml-cpp for configuration parsing
- Uses standard Linux networking tools and interfaces
- Inspired by the need for reliable connectivity in unstable network environments

## Support

For issues, questions, or contributions, please visit the [GitHub repository](https://github.com/yourusername/fidelity).