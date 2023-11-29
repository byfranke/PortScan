# About
PortScan v0.2 is a tool written in C, designed to determine open ports on a given system. It functions by sending data packets to different ports and evaluating the responses received. Depending on the response, it deduces whether a port is open (accessible) or closed (unreachable).

The program harnesses C's native socket capabilities for creating connections and dispatching packets to the target ports. If a port is receptive, the system will respond with a packet; otherwise, the program gets no response.

Incorporating multithreading techniques, PortScan v0.2 accelerates the scanning process by concurrently checking multiple ports, ensuring efficiency.

Ensure you have the necessary C libraries and compilers set up correctly for PortScan v0.2 to run efficiently. Unauthorized port scanning can be illegal and an infringement on others' privacy and security. Always secure the proper authorization before initiating such actions.

# Installation For Linux

Step : 1 Download

```
git clone https://github.com/byfranke/portscan/
```
Step : 2 Move to directory
```
cd portscan
```
Step : 3 For installing tools in directory
```
bash installer.sh
```
Step : 4 Run
```
portscan
```

# Example

