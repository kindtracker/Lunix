#!/bin/bash
set -x -e

sudo mount /dev/sdd1 /mnt
sudo cp output/Lunix.firm /mnt/luma/payloads/Lunix.firm
sudo umount /mnt
