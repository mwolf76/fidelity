#!/bin/sh
# autogen.sh - Run this to generate all the initial makefiles, etc.

set -e

echo "Running autoreconf..."
autoreconf --install --force --verbose || {
    echo "autoreconf failed. You may need to install autoconf, automake, and libtool."
    exit 1
}

echo ""
echo "Autotools files generated successfully."
echo "You can now run:"
echo "  ./configure"
echo "  make"
echo "  sudo make install"