# 🧠 ThanatOS  
Runs under moonlight and broken dreams.

---

---
ThanatOS is light, easy and open-source!
---
## 📜 Attribution Notice
Any redistribution, whether modified or unmodified, **must include clear attribution** to the original author and project name.  
This includes documentation, user interfaces, and public-facing materials.

---
## Versions
Releases are named after some silly names...

**Here are the available releases so far:**
- Stupei (Alpha a0.1) CODENAME: stupei
- Tartarus (Beta b0.1) *unavailable at the moment* CODENAME: d1
- Orpheus (Release v1.0) *unavailable at the moment* CODENAME: orpheus
---

## 🚀 How to Run ThanatOS (EclipseCore)

### ♣️ Step 0: Install the dependencies
This Usually doesn't require much, but you do need **QEMU** and **git** on your system.
In most cases, just run:

**Fedora/DNF based distros**
> sudo dnf install git qemu

**Ubuntu/Apt based distros**
> sudo apt install git qemu

**Anything else**
Add the following packages to your system with your prefered package manager.
> git qemu
> 
### 💿 Step 1: Create a virtual disk with QEMU
Usually, you just need to run this command in the terminal in the directory where the baseImg is located. You can get one by BUILDING the OS with Make (or find one in pre/prerealease), then run the following:
```bash
qemu-img create -f qcow2 disk.img 100M
qemu-system-x86_64 -m 2048 -hda disk.img -cdrom [where ever is ThanatOS.iso] -boot d
