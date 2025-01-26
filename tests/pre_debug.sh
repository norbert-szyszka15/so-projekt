#!/bin/bash

echo "=== Lista semaforów (ipcs -s) ==="
ipcs -s
echo "=== Lista pamięci współdzielonej (ipcs -m) ==="
ipcs -m
