sudo apt install gcc -y
gcc -o port_scanner_beta port_scanner_beta.c -lpthread
sudo chmod +x port_scanner_beta
sudo cp port_scanner_beta /usr/bin
