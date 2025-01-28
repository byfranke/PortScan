sudo apt install gcc -y
gcc -o port_scanner_beta port_scanner_beta.c -lpthread
sudo chmod +x portscan
sudo cp port_scanner_beta /usr/bin
