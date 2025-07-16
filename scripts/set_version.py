Import("env")
import os

# Lies Umgebungsvariablen
major = os.getenv("VERSION_MAJOR", "0")
minor = os.getenv("VERSION_MINOR", "0")
patch = os.getenv("VERSION_PATCH", "0")

print(f"Setze Version: {major}.{minor}.{patch}")

# Füge defines hinzu
env.Append(
    CPPDEFINES=[
        ("VERSION_MAJOR", major),
        ("VERSION_MINOR", minor),
        ("VERSION_PATCH", patch)
    ]
)
