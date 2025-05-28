#!/bin/bash

# ANSI color codes for better readability
GREEN='\033[0;32m'
BLUE='\033[0;34m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${BLUE}╔════════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║           TCI Task Manager Setup           ║${NC}"
echo -e "${BLUE}╚════════════════════════════════════════════╝${NC}"
echo

# Check for required dependencies
echo -e "${YELLOW}Checking dependencies...${NC}"

MISSING_DEPS=0

# Check for git
if ! command -v git &> /dev/null; then
    echo -e "${RED}✗ Git is not installed. Please install git.${NC}"
    MISSING_DEPS=1
fi

# Check for gcc
if ! command -v gcc &> /dev/null; then
    echo -e "${RED}✗ GCC is not installed. Please install gcc.${NC}"
    MISSING_DEPS=1
fi

# Check for make
if ! command -v make &> /dev/null; then
    echo -e "${RED}✗ Make is not installed. Please install make.${NC}"
    MISSING_DEPS=1
fi

# Check for ncurses
if ! pkg-config --exists ncurses 2>/dev/null || ! ldconfig -p | grep -q libncurses; then
    echo -e "${RED}✗ ncurses development library is not installed. Please install libncurses-dev.${NC}"
    MISSING_DEPS=1
fi

if [ $MISSING_DEPS -eq 1 ]; then
    echo
    echo -e "${RED}Please install missing dependencies and run this script again.${NC}"
    echo -e "${YELLOW}On Ubuntu/Debian:${NC} sudo apt-get install git gcc make libncurses-dev"
    echo -e "${YELLOW}On Fedora:${NC} sudo dnf install git gcc make ncurses-devel"
    echo -e "${YELLOW}On CentOS/RHEL:${NC} sudo yum install git gcc make ncurses-devel"
    echo -e "${YELLOW}On Arch Linux:${NC} sudo pacman -S git gcc make ncurses"
    exit 1
fi

echo -e "${GREEN}✓ All dependencies are installed.${NC}"
echo

# Create temporary directory for building
TEMP_DIR=$(mktemp -d)
echo -e "${YELLOW}Creating temporary directory for building: ${TEMP_DIR}${NC}"
cd "$TEMP_DIR" || exit 1

# Clone the repository
echo -e "${YELLOW}Cloning the repository from GitHub...${NC}"
if git clone https://github.com/ayouz0/taski.git; then
    echo -e "${GREEN}✓ Repository cloned successfully.${NC}"
else
    echo -e "${RED}✗ Failed to clone repository. Please check your internet connection.${NC}"
    exit 1
fi

# Enter the repository directory
cd taski || exit 1

# Build the application
echo -e "\n${YELLOW}Building the application...${NC}"
if make; then
    echo -e "${GREEN}✓ Build completed successfully.${NC}"
else
    echo -e "${RED}✗ Build failed. Please check the error messages above.${NC}"
    exit 1
fi

# Install the executable to /bin
echo -e "\n${YELLOW}Installing the executable to /bin...${NC}"

if [ -x "./taskmanager" ]; then
    # Check if we have sudo access
    if command -v sudo &> /dev/null; then
        echo -e "${YELLOW}Requesting administrator privileges to install to /bin...${NC}"
        if sudo cp ./taskmanager /bin/taskmanager; then
            sudo chmod 755 /bin/taskmanager
            echo -e "${GREEN}✓ Installation successful! You can now run 'taskmanager' from anywhere.${NC}"
        else
            echo -e "${RED}✗ Failed to install to /bin. You might not have sufficient permissions.${NC}"
            echo -e "${YELLOW}You can still run the program from: ${TEMP_DIR}/taski/taskmanager${NC}"
            exit 1
        fi
    else
        echo -e "${RED}✗ The 'sudo' command is not available. Installation to /bin requires administrator privileges.${NC}"
        echo -e "${YELLOW}You can still run the program from: ${TEMP_DIR}/taski/taskmanager${NC}"
        exit 1
    fi
else
    echo -e "${RED}✗ The executable was not built correctly or could not be found.${NC}"
    exit 1
fi

# Create a data directory in user's home directory
echo -e "\n${YELLOW}Creating data directory in your home folder...${NC}"
mkdir -p "$HOME/.taskmanager"
cp -f "$TEMP_DIR/taski/README.md" "$HOME/.taskmanager/" 2>/dev/null

# Clean up
echo -e "\n${YELLOW}Cleaning up temporary files...${NC}"
cd
rm -rf "$TEMP_DIR"

echo
echo -e "${BLUE}╔════════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║           Installation Complete!           ║${NC}"
echo -e "${BLUE}╚════════════════════════════════════════════╝${NC}"
echo
echo -e "${GREEN}You can now run TCI Task Manager by typing:${NC}"
echo -e "${YELLOW}  taskmanager${NC}"
echo
echo -e "${GREEN}Your task data will be stored in:${NC}"
echo -e "${YELLOW}  $HOME/.taskmanager/${NC}"
echo
echo -e "${GREEN}Thank you for installing TCI Task Manager!${NC}"
