"""
Test script for pyserial functionality
"""
import sys
print(f"Python version: {sys.version}")
print(f"Python executable: {sys.executable}")

try:
    import serial
    print(f"Serial module: {serial}")
    print(f"Serial module location: {serial.__file__}")
    print(f"Serial module has Serial: {hasattr(serial, 'Serial')}")
    
    if hasattr(serial, 'Serial'):
        print(f"Serial.Serial class: {serial.Serial}")
        
    import serial.tools.list_ports
    print("Successfully imported serial.tools.list_ports")
    
    ports = list(serial.tools.list_ports.comports())
    print(f"Found {len(ports)} serial ports")
    for i, p in enumerate(ports):
        print(f"{i+1}. {p.device} - {p.description}")
        
except Exception as e:
    print(f"Error: {e}")
    print(f"Error type: {type(e).__name__}")
