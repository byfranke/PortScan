# About PortScan

# Update PortScan v0.2 

Release: Aug 31, 2023

PortScan v0.2 is a tool written in C, designed to determine open ports on a given system. It functions by sending data packets to different ports and evaluating the responses received. Depending on the response, it deduces whether a port is open (accessible) or closed (unreachable).

The program harnesses C's native socket capabilities for creating connections and dispatching packets to the target ports. If a port is receptive, the system will respond with a packet; otherwise, the program gets no response.

Incorporating multithreading techniques, PortScan v0.2 accelerates the scanning process by concurrently checking multiple ports, ensuring efficiency.

Ensure you have the necessary C libraries and compilers set up correctly for PortScan v0.2 to run efficiently. Unauthorized port scanning can be illegal and an infringement on others' privacy and security. Always secure the proper authorization before initiating such actions.

# Update PortScan Beta V 1.0 

Release: 28 jan 2025

**Major Improvements Applied:**

**Input Validation:** Added validation for ports and menu choices.

**IPv6 Support:** The resolve_domain and check_port functions have been updated to support IPv6.

**Thread Limitation:** The maximum number of concurrent threads has been limited to 50.

**Memory Release:** Added memory release to prevent leaks.

**Configurable Timeout:** The check_port function now allows you to configure the timeout.

**Readability Improvements:** The code has been divided into smaller, clearer functions.

**Error Handling:** Added more error handling to increase robustness.

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
bash installer.sh or installer_beta.sh
```
Step : 4 Run
```
portscan or portscan_beta
```
# PortScan for Windows

**Basic properties**

MD5
```
6ee09285e8bb9b4908df70cdd3ce8711 
```
SHA-1
```
53784ee14a389cd40a1ac955cf6dd4c8e83f6f67
```
SHA-256
```
0aeec39a07f923393055a8d2bda1f829f6264cb00a3dc16c65c96262878f3b44
```

# Example
![image](https://github.com/byfranke/portscan/assets/131370932/c070685e-0018-4ae5-8590-d7b964e2d564)


