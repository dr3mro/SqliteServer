import os
import socket

def get_ip():
    hostname = socket.gethostname()
    ip_address = socket.gethostbyname(hostname)
    return ip_address

if __name__ == "__main__":
    ip = get_ip()
    print(f"Container IP Address: {ip}")
