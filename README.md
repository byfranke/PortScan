# About PortScan

The "PortScan" program is a C tool that scans ports on a domain or IP. Users can choose to check common ports, a specific one, or a range. It employs multithreading for speed. After each scan, it displays the port status and asks if the user wants to continue.

Note: Unauthorized port scanning may violate legal and privacy standards. Obtain proper authorization before using this tool.

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

**Example**


![image](https://github.com/byfranke/portscan/assets/131370932/c070685e-0018-4ae5-8590-d7b964e2d564)


# Update PortScan v0.2

Release: Aug 31, 2023

**Description:**

PortScan v0.2 is a tool written in C to determine open ports on a target system by sending data packets to various ports and analyzing their responses. The tool utilizes C's native socket capabilities for sending and receiving packets, allowing it to infer whether a port is open or closed based on the system's reply.

**Key Features:**

Socket Programming: Leverages C's socket library for creating connections and sending packets to target ports.

Port Status Identification: Deduction of port states (open or closed) based on response packets.

Multithreading: Uses multithreading to accelerate the scanning process by concurrently testing multiple ports, significantly increasing efficiency.

**Usage Requirements:**

Ensure you have the necessary C libraries and compilers correctly set up.

# Update PortScan v0.3

Release: Jan 28, 2025

**Description:**

PortScan v0.3 builds upon the capabilities of version 0.2 with enhanced functionality, reliability, and support for IPv6. This version introduces significant improvements to usability, performance, and code clarity.

**Key Features and Improvements:**

Input Validation: Added validation for user input, including ports and menu options, to ensure correct operation and prevent invalid values.

IPv6 Support: Functions resolve_domain and check_port now support IPv6 addresses, expanding compatibility beyond IPv4.

Thread Limitation: Maximum concurrent threads limited to 50 to optimize resource usage and prevent system overload.

Memory Management: Implemented memory release mechanisms to prevent memory leaks during execution.

Configurable Timeout: check_port now supports configurable timeouts, allowing users to adjust the wait time for responses from scanned ports.

Improved Code Readability: Code refactored into smaller, modular functions for better readability and maintenance.

Enhanced Error Handling: Increased robustness with additional error handling for socket failures and invalid operations.

**Usage Requirements:**

Ensure all required libraries and compilers are set up correctly.


# PortScan v0.4

Release: Jan 28, 2025

Description:
The new PortScan Beta Version introduces several major updates inspired by tools like nmap, enhancing interactivity, flexibility, and performance. However, as this is a beta release, it may still contain bugs or unexpected behavior. Users are encouraged to report any issues encountered during usage.

**Key Updates and Features:**

Interactive Command-Line Arguments:

You can now specify options directly when executing the program:
-o 1: Scan common ports (e.g., 80, 443, 22, etc.).
-o 2 <port>: Check a specific port.
-o 3 <start-end>: Scan a range of ports (e.g., 20-9090).
Example: ./portscan example.com -o 3 20-80 -t 2
Configurable Timeout:

Adjust the scan timeout using -t. The default timeout is 1 second, but now you can specify longer intervals (e.g., -t 2, -t 3) to avoid triggering IP protection mechanisms.
Domain and IP Resolution:

Automatically resolves domains to their respective IPv4 or IPv6 addresses before scanning.
Range-Based Port Scanning:

Added support for specifying port ranges (e.g., 20-8080) for better flexibility in custom scans.
Improved Thread Management:

Thread limit remains at 50 for efficient resource usage, ensuring performance remains stable during range scans.
User-Friendly Interface:

Enhanced error handling and validation for incorrect inputs, such as invalid port numbers, ranges, or missing parameters.

**Usage Example:**

```
./portscan example.com -o 3 20-80 -t 2
```
This command scans ports 20 through 80 on example.com with a 2-second timeout between requests.

# PortScan Beta Version Update

**Important Note:**

As this is a beta version, the program is still in testing and may contain bugs or unexpected behavior. Please use cautiously and only on systems you have permission to scan. Report any issues to help improve future versions!

# Disclaimer

This tool is designed for educational and testing purposes only. The creator strictly discourages and disclaims any responsibility for its use in unauthorized or malicious activities. Always obtain explicit permission before deploying this tool in any environment.

# Donations

If you find these tools useful and would like to support ongoing development and maintenance, please consider making a donation. Your contribution helps ensure that these tools are regularly updated and improved, benefiting the cybersecurity community. Any amount is greatly appreciated and will make a significant difference in supporting this project. Thank you for considering supporting this work!

**Address Bitcoin:**  bc1qkdh3eqpj87q5hlhc7pvm025hmsd9zp2kadxf76
