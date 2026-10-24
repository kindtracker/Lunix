#!/bin/bash
set -x -e

sudo mount $SDCARD3DS /mnt
sudo cp output/Lunix.firm /mnt/luma/payloads/Lunix.firm
sudo umount /mnt
