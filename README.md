# About PortScan

The "PortScan" program is a C tool that scans ports on a domain or IP. Users can choose to check common ports, a specific one, or a range. It employs multithreading for speed. After each scan, it displays the port status and asks if the user wants to continue.

**Update PortScan v0.2**

Release: Aug 31, 2023

Description:
PortScan v0.2 is a tool written in C to determine open ports on a target system by sending data packets to various ports and analyzing their responses. The tool utilizes C's native socket capabilities for sending and receiving packets, allowing it to infer whether a port is open or closed based on the system's reply.

Key Features:

Socket Programming: Leverages C's socket library for creating connections and sending packets to target ports.
Port Status Identification: Deduction of port states (open or closed) based on response packets.
Multithreading: Uses multithreading to accelerate the scanning process by concurrently testing multiple ports, significantly increasing efficiency.
Usage Requirements:
Ensure you have the necessary C libraries and compilers correctly set up.
Note: Unauthorized port scanning may violate legal and privacy standards. Obtain proper authorization before using this tool.

**Update PortScan Beta v1.0**

Release: Jan 28, 2025

Description:
PortScan Beta v1.0 builds upon the capabilities of version 0.2 with enhanced functionality, reliability, and support for IPv6. This version introduces significant improvements to usability, performance, and code clarity.

Key Features and Improvements:

Input Validation: Added validation for user input, including ports and menu options, to ensure correct operation and prevent invalid values.
IPv6 Support: Functions resolve_domain and check_port now support IPv6 addresses, expanding compatibility beyond IPv4.
Thread Limitation: Maximum concurrent threads limited to 50 to optimize resource usage and prevent system overload.
Memory Management: Implemented memory release mechanisms to prevent memory leaks during execution.
Configurable Timeout: check_port now supports configurable timeouts, allowing users to adjust the wait time for responses from scanned ports.
Improved Code Readability: Code refactored into smaller, modular functions for better readability and maintenance.
Enhanced Error Handling: Increased robustness with additional error handling for socket failures and invalid operations.
Usage Requirements:
Ensure all required libraries and compilers are set up correctly.
Note: Unauthorized port scanning remains subject to legal and ethical considerations. Always secure proper authorization.

**Summary of Changes from v0.2 to Beta v1.0:**

Input Validation: Beta v1.0 validates user inputs, reducing errors from invalid port numbers or menu selections.
IPv6 Support: Adds compatibility for IPv6, extending functionality beyond IPv4-only scanning in v0.2.
Thread Limitation: Introduced a 50-thread cap for safer resource management and consistent performance.
Memory Management: Addressed potential memory leaks by adding explicit memory release in Beta v1.0.
Timeout Configuration: Added adjustable timeouts in port scanning for greater user control.
Code Readability: Refactored the code into smaller, clearer functions, improving maintainability.
Error Handling: Enhanced error management to make the tool more robust and reliable in unexpected conditions.
Both versions retain their core functionality of scanning open ports, but Beta v1.0 significantly improves usability, efficiency, and adaptability.

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


