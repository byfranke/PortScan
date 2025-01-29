#!/bin/bash

print_banner() {
    echo
    echo "     _ \\               |     ___| "
    echo "    |   |  _ \\    __|  __| \\___ \\    __|   _\` |  __ \\ "
    echo "    ___/  (   |  |     |         |  (     (   |  |   | "
    echo "   _|    \\___/  _|    \\__| _____/  \\___| \\__,_| _|  _| "
    echo
    echo "              github.com/byfranke/portscan"
    echo
}

print_banner

echo "Choose an option below:"
echo
echo "1) Install Current Version"
echo "2) Install Beta Version"
echo "3) Install Old Version"
echo "4) Check for Latest Version and Update"
echo "5) Exit"
echo

read -p "Enter your choice: " choice

install_portscan() {
    local source_file=$1
    local output_name="portscan"

    echo "[*] Compiling $source_file..."
    gcc "$source_file" -o "$output_name"

    if [ $? -eq 0 ]; then
        echo "[+] Compilation successful!"
        
        sudo chmod +x "$output_name"

        sudo mv "$output_name" /usr/bin/

        echo "[+] PortScan installed successfully!"
    else
        echo "[!] Compilation failed. Please check for errors."
    fi
}

check_and_update() {
    echo "[*] Checking for the latest version on GitHub..."
    
    # Habilita extglob para evitar erro de sintaxe
    shopt -s extglob

    GITHUB_REPO="https://github.com/byfranke/PortScan"
    TEMP_DIR=$(mktemp -d)

    # Clone the latest version
    git clone "$GITHUB_REPO" "$TEMP_DIR"

    # Move old files to obsolete, exceto installer.sh e obsolete folder
    mkdir -p obsolete
    find . -maxdepth 1 ! -name "installer.sh" ! -name "obsolete" -type f -exec mv {} obsolete/ \;

    # Move new files to current directory
    mv "$TEMP_DIR"/* ./
    rm -rf "$TEMP_DIR"

    echo "[+] Updated to the latest version. Old files moved to 'obsolete'."
}

case $choice in
    1)
        if [ -f "portscan.c" ]; then
            install_portscan "portscan.c"
        else
            echo "[!] portscan.c not found!"
        fi
        ;;
    2)
        if [ -f "portscan_beta.c" ]; then
            install_portscan "portscan_beta.c"
        else
            echo "[!] portscan_beta.c not found!"
        fi
        ;;
    3)
        OLD_VERSIONS_DIR="Old Version"
        if [ -d "$OLD_VERSIONS_DIR" ]; then
            echo "Available old versions:"
            files=("$OLD_VERSIONS_DIR"/*.c)
            if [ ${#files[@]} -eq 0 ]; then
                echo "[!] No old versions found."
                exit 1
            fi

            for i in "${!files[@]}"; do
                echo "$((i+1))) ${files[$i]##*/}"
            done

            read -p "Select a version to install (1-${#files[@]}): " version_choice
            if [[ "$version_choice" =~ ^[0-9]+$ ]] && [ "$version_choice" -ge 1 ] && [ "$version_choice" -le "${#files[@]}" ]; then
                selected_file="${files[$((version_choice-1))]}"
                install_portscan "$selected_file"
            else
                echo "[!] Invalid selection!"
            fi
        else
            echo "[!] Old Version folder not found!"
        fi
        ;;
    4)
        check_and_update
        ;;
    5)
        echo "Exiting..."
        exit 0
        ;;
    *)
        echo "[!] Invalid option. Exiting..."
        exit 1
        ;;
esac
