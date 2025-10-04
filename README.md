# 🧠 ThanatOS  
A homemade OS — light, easy, and open-source!

---

## 📜 Attribution Notice

This project, **ThanatOS**, was originally created by **FakeLouna**, and later helped by **linkfandosVF**.  
Any redistribution, whether modified or unmodified, **must include clear attribution** to the original author and project name.  
This includes documentation, user interfaces, and public-facing materials.

---

## 🚀 How to Run ThanatOS (EclipseOS)

### 💿 Step 1: Create a virtual disk with QEMU

> ⚠️ You must have QEMU installed on your system.

```bash
qemu-img create -f qcow2 disk.img 100M
qemu-system-x86_64 -m 2048 -hda disk.img -cdrom path/to/ThanatOS.iso -boot d
