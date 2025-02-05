# About PortScan

The "PortScan" program is a C tool that scans ports on a domain or IP. Users can choose to check common ports, a specific one, or a range. It employs multithreading for speed. After each scan, it displays the port status and asks if the user wants to continue.

Note: Unauthorized port scanning may violate legal and privacy standards. Obtain proper authorization before using this tool.

# Installation For Linux

Step : 1 Download

```bash
git clone https://github.com/byfranke/portscan/
```
Step : 2 Move to directory
```bash
cd portscan
```
Step : 3 For installing tools in directory
```bash
bash installer.sh
```
Step : 4 Run
```bash
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

## Key Features:

- Socket Programming: Leverages C's socket library for creating connections and sending packets to target ports.

- Port Status Identification: Deduction of port states (open or closed) based on response packets.

- Multithreading: Uses multithreading to accelerate the scanning process by concurrently testing multiple ports, significantly increasing efficiency.

**Usage Requirements:**

Ensure you have the necessary C libraries and compilers correctly set up.

# Update PortScan v0.3

Release: Jun 08, 2024

**Description:**

PortScan v0.3 builds upon the capabilities of version 0.2 with enhanced functionality, reliability, and support for IPv6. This version introduces significant improvements to usability, performance, and code clarity.

## Key Features and Improvements:

- Input Validation: Added validation for user input, including ports and menu options, to ensure correct operation and prevent invalid values.

- IPv6 Support: Functions resolve_domain and check_port now support IPv6 addresses, expanding compatibility beyond IPv4.

- Thread Limitation: Maximum concurrent threads limited to 50 to optimize resource usage and prevent system overload.

- Memory Management: Implemented memory release mechanisms to prevent memory leaks during execution.

- Configurable Timeout: check_port now supports configurable timeouts, allowing users to adjust the wait time for responses from scanned ports.

- Improved Code Readability: Code refactored into smaller, modular functions for better readability and maintenance.

- Enhanced Error Handling: Increased robustness with additional error handling for socket failures and invalid operations.

**Usage Requirements:**

Ensure all required libraries and compilers are set up correctly.


# Update PortScan v0.4

Release: Dec 21, 2024

**Description:**

The new PortScan v0.4 introduces several major updates inspired by tools like nmap, enhancing interactivity, flexibility, and performance. However, as this is a beta release, it may still contain bugs or unexpected behavior. Users are encouraged to report any issues encountered during usage.

## Key Updates and Features:

**Interactive Command-Line Arguments:**

You can now specify options directly when executing the program.

- **-o 1:** Scan common ports (e.g., 80, 443, 22, etc.).

- **-o 2 <port>:** Check a specific port.

- **-o 3 <start-end>:** Scan a range of ports (e.g., 20-9090).

**Example:**

```bash
./portscan example.com -o 3 20-80 -t 2
```

**Configurable Timeout:**

- Adjust the scan timeout using -t. The default timeout is 1 second, but now you can specify longer intervals (e.g., -t 2, -t 3) to avoid triggering IP protection mechanisms.

**Domain and IP Resolution:**

- Automatically resolves domains to their respective IPv4 or IPv6 addresses before scanning.

**Range-Based Port Scanning:**

- Added support for specifying port ranges (e.g., 20-8080) for better flexibility in custom scans.

**Improved Thread Management:**

- Thread limit remains at 50 for efficient resource usage, ensuring performance remains stable during range scans.

**User-Friendly Interface:**

- Enhanced error handling and validation for incorrect inputs, such as invalid port numbers, ranges, or missing parameters.

## Usage Example

```bash
portscan example.com -o 3 20-80 -t 2
```
This command scans ports 20 through 80 on example.com with a 2-second timeout between requests.

# Update PortScan v0.5

Release: Jan 28, 2025

**Description:**

PortScan v0.5 further improves upon the previous versions by enhancing performance, security, and usability. This update introduces better error handling, safer memory management, optimized scanning methods, and a structured logging system. Additionally, the tool now includes signal handling for a graceful exit, reducing the risk of leaving orphaned threads or open sockets.

## Key Updates and Features:

Logging System for Debugging and Error Handling

- Introduced LOG_DEBUG and LOG_ERROR macros for structured logging, allowing users to track errors and debugging information more efficiently.

- Now, all major operations log detailed error messages to stderr, improving troubleshooting.

Replaced select with poll for Better Performance

- The select function has been replaced with poll, which is more scalable and efficient, especially when dealing with a large number of connections.

- Reduces CPU overhead and allows more accurate timeout handling during port scans.

Signal Handling for Controlled Exit (SIGINT)

- Added a signal handler for SIGINT (Ctrl+C), ensuring a graceful exit.

- Prevents memory leaks, avoids orphaned threads, and properly cleans up resources before termination.

Improved Memory Management and Leak Prevention

- Enhanced resolve_domain() to prevent potential memory leaks when allocating IP address buffers.

- Properly frees allocated memory in case of failures or early exits.

- Eliminated unnecessary malloc calls for small string buffers, reducing unnecessary heap allocations.

Enhanced Input Validation and Error Checking

- Now validates all user input, including invalid ports, incorrectly formatted ranges, and non-numeric values.

- Improved error messages to guide users on correct command usage.

Safer and More Efficient Thread Management

- Introduced thread pool logic to avoid creating excessive threads that could lead to system overload.

- Ensures that pthread_create failures are handled properly.

- Optimized range-based scanning to limit active threads dynamically based on available system resources.

Improved IPv6 and IPv4 Compatibility

- Now fully supports both IPv4 and IPv6, including mixed environments.

- Properly detects whether the target system supports IPv6 and adjusts scanning methods accordingly.

Safer Script Updating Mechanism

- The --update feature now includes checksum validation to prevent executing malicious code if the repository is compromised.

- Enhanced error messages for failed updates to inform users of the exact issue.


## Usage Example:

Basic Scan for Common Ports (21, 22, 80, 443, etc.)

```bash
portscan example.com -o 1 -t 2
```

Scans common ports with a 2-second timeout between requests.

Scanning Specific Ports (80, 443, 8080)

```bash
portscan example.com -o 2 -p 80,443,8080 -t 1
```

Scans ports 80, 443, and 8080 with a 1-second timeout.

Scanning a Port Range (20-1000)

```bash
portscan example.com -o 3 20-1000 -t 1
```

Scans ports 20 through 1000 with a 1-second timeout between each connection attempt.

**Conclusion:**

PortScan v0.5 is a more efficient, secure, and user-friendly tool for network scanning. By implementing better logging, memory management, thread optimization, and input validation, this version significantly enhances reliability and usability.

# PortScan Beta Version Update

**Important Note:**

As this is a beta version, the program is still in testing and may contain bugs or unexpected behavior. Please use cautiously and only on systems you have permission to scan. Report any issues to help improve future versions!

## Disclaimer

This tool is designed for educational and testing purposes only. The creator strictly discourages and disclaims any responsibility for its use in unauthorized or malicious activities. Always obtain explicit permission before deploying this tool in any environment.

