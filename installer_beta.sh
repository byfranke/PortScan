sudo apt install gcc -y
gcc -o portscan_beta portscan_beta.c -lpthread
sudo chmod +x portscan_beta
sudo cp portscan_beta /usr/bin
