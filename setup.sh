#!/bin/bash

# ANSI color codes for better readability
GREEN='\033[0;32m'
BLUE='\033[0;34m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${BLUE}╔════════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║             Tasky Installer                ║${NC}"
echo -e "${BLUE}╚════════════════════════════════════════════╝${NC}"
echo

# Check if we're in the correct directory
if [ ! -f "Makefile" ] || [ ! -d "src" ] || [ ! -d "include" ]; then
    echo -e "${RED}Error: This script must be run from the Tasky root directory.${NC}"
    echo -e "${YELLOW}Please run this script from the directory where you cloned the repository.${NC}"
    exit 1
fi

# Build the application
echo -e "${YELLOW}Building Tasky...${NC}"
if make clean && make; then
    echo -e "${GREEN}✓ Build completed successfully.${NC}"
else
    echo -e "${RED}✗ Build failed. Please check the error messages above.${NC}"
    exit 1
fi

# Create necessary directories
echo -e "\n${YELLOW}Setting up Tasky in your home directory...${NC}"
mkdir -p "$HOME/.tasky"
mkdir -p "$HOME/.local/bin"

# Install the executable to user's bin directory
if [ -x "./taskmanager" ]; then
    if cp ./taskmanager "$HOME/.local/bin/tasky"; then
        chmod 755 "$HOME/.local/bin/tasky"
        echo -e "${GREEN}✓ Installation successful!${NC}"
    else
        echo -e "${RED}✗ Failed to install.${NC}"
        echo -e "${YELLOW}You can still run the program locally with ./taskmanager${NC}"
        exit 1
    fi
else
    echo -e "${RED}✗ The executable was not built correctly or could not be found.${NC}"
    exit 1
fi

# Add ~/.local/bin to PATH if it's not already there
if [[ ":$PATH:" != *":$HOME/.local/bin:"* ]]; then
    echo -e "\n${YELLOW}Adding ~/.local/bin to your PATH...${NC}"
    
    # Determine which shell config file to use
    SHELL_CONFIG=""
    if [ -f "$HOME/.bash_profile" ]; then
        SHELL_CONFIG="$HOME/.bash_profile"
    elif [ -f "$HOME/.bashrc" ]; then
        SHELL_CONFIG="$HOME/.bashrc"
    elif [ -f "$HOME/.zshrc" ]; then
        SHELL_CONFIG="$HOME/.zshrc"
    fi
    
    if [ -n "$SHELL_CONFIG" ]; then
        echo 'export PATH="$HOME/.local/bin:$PATH"' >> "$SHELL_CONFIG"
        echo -e "${GREEN}✓ Added ~/.local/bin to your PATH in $SHELL_CONFIG${NC}"
        echo -e "${YELLOW}Please run 'source $SHELL_CONFIG' or restart your terminal to use tasky command${NC}"
    else
        echo -e "${YELLOW}Could not find shell config file. You may need to add ~/.local/bin to your PATH manually.${NC}"
    fi
fi

echo
echo -e "${BLUE}╔════════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║           Installation Complete!           ║${NC}"
echo -e "${BLUE}╚════════════════════════════════════════════╝${NC}"
echo
echo -e "${GREEN}You can run Tasky by typing:${NC}"

if [[ ":$PATH:" == *":$HOME/.local/bin:"* ]]; then
    echo -e "${YELLOW}  tasky${NC}"
else
    echo -e "${YELLOW}  $HOME/.local/bin/tasky${NC}"
    echo -e "${GREEN}Or after restarting your terminal:${NC}"
    echo -e "${YELLOW}  tasky${NC}"
fi

echo
echo -e "${GREEN}Your task data will be stored in:${NC}"
echo -e "${YELLOW}  $HOME/.tasky/${NC}"
echo
echo -e "${GREEN}Thank you for installing Tasky!${NC}"