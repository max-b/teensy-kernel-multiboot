#!/bin/bash

if grub-file --is-x86-multiboot ./build/myos.bin; then
  echo multiboot confirmed
  exit 0
else
  echo the file is not multiboot
  exit 1
fi
