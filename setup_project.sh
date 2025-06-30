#!/bin/bash

set -e  # Stopping the script in case of an error

echo "We are starting to install dependencies..."

sudo apt update && sudo apt upgrade -y

sudo apt install -y build-essential cmake git pkg-config

echo "Installing GoogleTest..."
sudo apt install -y libgtest-dev
sudo apt install -y googletest

echo "Installing lohmann/json..."

if ! command -v pkg-config &> /dev/null; then
    sudo apt install -y pkg-config
fi

echo "We install sdbus-c++ and dependencies..."

sudo apt install -y libsystemd-dev

if ! pkg-config --exists libsystemd; then
    echo "lib systemd not found "
    exit 1
fi

if ! dpkg -l | grep -q libsdbus-c++-dev; then
    sudo apt install -y libsdbus-c++-dev
else
    echo "sdbus-c++ is already installed"
fi

echo "Installing Doxygen and Graphviz..."
sudo apt install -y doxygen graphviz
