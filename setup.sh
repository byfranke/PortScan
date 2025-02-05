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
    
    shopt -s extglob

    GITHUB_REPO="https://github.com/byfranke/PortScan"
    TEMP_DIR=$(mktemp -d)

    git clone "$GITHUB_REPO" "$TEMP_DIR"

    mkdir -p Obsolete
    find . -maxdepth 1 ! -name "er.sh" ! -name "Obsolete" -type f -exec mv {} Obsolete/ \;

    mv "$TEMP_DIR"/* ./ 2>/dev/null
    rm -rf "$TEMP_DIR"

    echo "[+] Updated to the latest version. Old files moved to 'Obsolete'."
}

show_menu() {
    while true; do
        print_banner
        echo "Choose an option below:"
        echo
        echo "1) Install Current Version"
        echo "2) Install Beta Version"
        echo "3) Install Obsolete Version"
        echo "4) Check for Latest Version and Update"
        echo "5) Exit"
        echo
        read -p "Enter your choice: " choice

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
                OBSOLETE_DIR="Obsolete"
                if [ -d "$OBSOLETE_DIR" ]; then
                    echo "Available old versions:"
                    files=("$OBSOLETE_DIR"/*.c)
                    if [ ${#files[@]} -eq 0 ]; then
                        echo "[!] No old versions found."
                        continue
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
                    echo "[!] Obsolete folder not found!"
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
                echo "[!] Invalid option. Please try again."
                ;;
        esac
        echo
        read -p "Press Enter to return to the menu..."
    done
}

show_menu
