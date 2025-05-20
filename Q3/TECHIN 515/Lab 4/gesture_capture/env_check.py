import sys
print(f"Python version: {sys.version}")
print(f"Python path: {sys.path}")
print("Available modules:")
for m in sys.modules:
    print(f" - {m}")

print("\nTrying to import pyserial")
try:
    import serial
    print(f"Imported serial module from {serial.__file__}")
    print(f"Has Serial attribute: {hasattr(serial, 'Serial')}")
except Exception as e:
    print(f"Error importing serial: {e}")
