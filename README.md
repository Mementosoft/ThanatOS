# ThanatOS
A homemade OS, light, easy and open-source!

Attribution Notice:

This project, ThanatOS, was originally created by FakeLouna.
Any redistribution, whether modified or unmodified, must include clear attribution to the original author and project name.
This includes documentation, user interfaces, and public-facing materials.

# How to run?
## To run ThanatOS (EclipseOS), you have to run this command : 
---
💿 Create a virtual disk on qemu
⚠️ You must have qemu installed in your system.
''' code
qemu-img create -f qcow2 disk.img 100M
'''
---

To Run ThanatOS : 
'''
qemu-system-x86_64 -m 2048 -hda disk.img -cdrom path/to/ThanatOS.iso -boot d
'''
---
