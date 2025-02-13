import socket

def check_port(host, port):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.settimeout(1)
    result = sock.connect_ex((host, port))
    sock.close()
    return result

host = input("Enter target address: ")
portscan_type = input("\n[1] Default Scan\n[2] Range Scan\n[3] Custom Scan\n[4] Exit\n\nChoose a PortScan option: ")

if portscan_type == "1":
    default_ports = {
        22: "SSH",
        80: "HTTP",
        443: "HTTPS",
        53: "DNS",
        21: "FTP",
        25: "SMTP",
        110: "POP3",
        143: "IMAP",
        23: "Telnet",
        3306: "MySQL",
        5432: "PostgreSQL",
        8080: "HTTP Proxy",
    }

    for port, service in default_ports.items():
        if check_port(host, port) == 0:
            print(f"Port {port} ({service}) is open.")

elif portscan_type == "2":
    port_range = int(input("Enter number of ports to scan (example: 1000): "))
    for port in range(port_range):
        if check_port(host, port) == 0:
            print(f"Port {port} is open.")

elif portscan_type == "3":
    custom_ports = input("Enter custom ports (comma-separated, e.g., 22,80,443): ")
    ports = [int(p) for p in custom_ports.split(",")]
    for port in ports:
        if check_port(host, port) == 0:
            print(f"Port {port} is open.")

print("\nPort Scan Completed Successfully!")
