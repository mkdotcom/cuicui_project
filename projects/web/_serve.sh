#!/bin/sh
# Serves the "build" folder on http://localhost:8080 (needs Python 3).
cd "$(dirname "$0")/build" && python3 -m http.server 8080
