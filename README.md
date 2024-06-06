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
# PortScan for Windows

**Basic properties**

MD5
6ee09285e8bb9b4908df70cdd3ce8711 
SHA-1
53784ee14a389cd40a1ac955cf6dd4c8e83f6f67 
SHA-256
0aeec39a07f923393055a8d2bda1f829f6264cb00a3dc16c65c96262878f3b44

![link](https://www.virustotal.com/gui/file/0aeec39a07f923393055a8d2bda1f829f6264cb00a3dc16c65c96262878f3b44?nocache=1)

# Example
![image](https://github.com/byfranke/portscan/assets/131370932/c070685e-0018-4ae5-8590-d7b964e2d564)


