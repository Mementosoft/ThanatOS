#!/bin/bash

ISO_PATH="build/EclipseOS.iso"
DISK_IMG="build/disk.img"
RAM_SIZE=128

# Vérifie si le disque existe, sinon le crée
if [ ! -f "$DISK_IMG" ]; then
  echo "Disk image not found. Creating 5GB disk image..."
  qemu-img create -f raw "$DISK_IMG" 5G
fi

qemu-system-i386 \
  -cdrom "$ISO_PATH" \
  -drive file="$DISK_IMG",format=raw,if=ide \
  -m "${RAM_SIZE}M" \
  -boot d \
  -vga std
