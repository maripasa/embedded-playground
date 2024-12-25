#!/bin/bash
# maripasa
# THIS SCRIPT IS UNTESTED
# This script sets up the SDK and toolchain for the BitDogLab microcontroller

SDK_VERSION="14.2.rel1"
TOOLCHAIN_BASE_URL="https://developer.arm.com/-/media/Files/downloads/gnu/$SDK_VERSION/binrel"
TOOLCHAIN_FILE="arm-gnu-toolchain-${SDK_VERSION}-x86_64-arm-none-eabi.tar.xz"
TOOLCHAIN_INSTALL_PATH="/opt"
TOOLCHAIN_BIN_PATH="${TOOLCHAIN_INSTALL_PATH}/bin"
PICO_DIR="$HOME/pico"
REPOS=(
    "https://github.com/raspberrypi/pico-examples.git"
    "https://github.com/raspberrypi/pico-sdk.git"
    "https://github.com/raspberrypi/picotool.git"
)

if [[ "$SHELL" == *zsh ]]; then
    RC_FILE="$HOME/.zshrc"
elif [[ "$SHELL" == *bash ]]; then
    RC_FILE="$HOME/.bashrc"
else
    echo "Unsupported shell: $SHELL"
    exit 1
fi

# Helper functions
create_directory() {
    local dir="$1"
    if [[ ! -d "$dir" ]]; then
        mkdir -p "$dir"
    fi
}

clone_repo() {
    local url="$1"
    local dest="$2"
    if [[ ! -d "$dest" ]]; then
        git clone "$url" "$dest"
    else
        echo "Repository $url already exists at $dest"
    fi
}

build_tool() {
    local folder="$1"
    echo "Building $folder..."
    cd "$folder" || exit
    rm -rf build 2>/dev/null
    mkdir build && cd build || exit
    cmake ..
    make
    cd ../..
}

create_directory "$PICO_DIR"
cd "$PICO_DIR"

wget -q "$TOOLCHAIN_BASE_URL/$TOOLCHAIN_FILE"
tar -xf "$TOOLCHAIN_FILE"
sudo mv "arm-gnu-toolchain-${SDK_VERSION}-x86_64-arm-none-eabi" "$TOOLCHAIN_INSTALL_PATH"

for repo in "${REPOS[@]}"; do
    repo_name=$(basename "$repo" .git)
    clone_repo "$repo" "$PICO_DIR/$repo_name"
done

{
    echo "export PATH=\$PATH:$TOOLCHAIN_BIN_PATH"
    echo "export PICO_SDK_PATH=$PICO_DIR/pico-sdk"
    echo "export PICO_TOOLCHAIN_PATH=$TOOLCHAIN_INSTALL_PATH"
} >> "$RC_FILE"

build_tool "$PICO_DIR/picotool"

sudo apt update && sudo apt install -y cmake build-essential python3 python3-pip git

echo "Setup completed successfully! Please restart your terminal or source $RC_FILE to apply changes."
