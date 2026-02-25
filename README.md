# DIT-Transfer-Tools v1.0

## Polish #1-52 + Features Complete

**HIGH Priority Fixes #1-10:** All implemented with tests
**Polish #11-52:** UI/UX, error handling, performance
**Features:** Live preview v3, parallel transfers, settings

## Key Capabilities
- Drive monitoring + auto-detection
- Multi-target folder transfers
- Qt/C++ cross-platform
- Concurrent + atomic writes
- Unicode + >4TB + power-loss safe
- Live preview + settings UI
- Queue management with active/waiting lists, reorder functionality, and configurable parallel transfers

## Build & Run
```bash
# Windows
build.bat

# Linux/Mac
qmake DIT.pro
make
./DIT --queue-size 5 /source /dest

# Tests
g++ -std=c++17 -o tests transfer_app/tests.cpp transfer_app/Task.cpp -pthread
./tests
```

**Status:** v1.0 Release Ready 🚀