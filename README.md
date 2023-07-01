[![GitHub license](https://dmlc.github.io/img/apache2.svg)](LICENSE) 
![Gitter](https://img.shields.io/gitter/room/mobile-swam/swam) ![GitHub repo size](https://img.shields.io/github/repo-size/mobile-swam/swam) ![GitHub issues](https://img.shields.io/github/issues/mobile-swam/swam) ![GitHub pull requests](https://img.shields.io/github/issues-pr/mobile-swam/swam) ![GitHub contributors](https://img.shields.io/github/contributors/mobile-swam/swam)

SWAM kernel
============

This page details the methods for building and running the SWAM kernel. 

## Pre-requisites
We assume that you use Ubuntu 22.04.2 (64bit) LTS distribution. Firs of all you must install the repo package to maintain the related git repositories. 

```bash
sudo apt -y update
sudo apt -y install curl
python3 --version
  Python 3.10.6
mkdir ~/bin/
curl http://commondatastorage.googleapis.com/git-repo-downloads/repo > ~/bin/repo
chmod a+x ~/bin/repo
vim ~/.bashrc
  export PATH="~/bin:$PATH"
```

## How to build 

To use SWAM, you must first build the SWAM kernel source. 
SWAM's features are not dependent on the smartphone device, so they can be utilized in the general Linux system environment as well as in embedded systems such as smartphones. 

#### How to build the Android Linux kernel
We assume that you use the reference smartphone such as the Pixel 6 (Android 12).
This existing SWAM source can be merged with higher Linux kernel versions using the git merge command.
* Pixel6-based Android Kernel: private/gs-google (Linux 5.10.43)
* Pixel6-based kernel config.: private/gs-google/arch/arm64/configs/slider_gki.fragment
```bash
$ cd /work/swam/
$ mkdir android-kernel && cd android-kernel
$ repo init -u https://android.googlesource.com/kernel/manifest -b android-gs-raviole-5.10-android12-qpr1-d
$ repo sync -j$(nproc)
$ mv aosp aosp.disable
$ cd private
$ mv gs-google gs-google.disable
$ git clone https://github.com/mobile-swam/swam.git swam.git
$ ln -s swam.git gs-google
$ cd ..
$ cat ./private/gs-google/Makefile | head

VERSION = 5
PATCHLEVEL = 10
SUBLEVEL = 43
EXTRAVERSION =
NAME = Dare mighty things
```

We've done the work to compile the SWAM kernel source, now we'll work on building the kernel source ourselves. 
```bash
$ time build/build.sh -j$(nproc)
    ... Case1: Wait for 56 minutes with Intel i5-3570@3.40GHz + DDR3 16GB + SSD 1TB  
    ... Case2: wait for 32 minutes with Intel Xeon E3-1275v5@3.60GHz + DDR4 32GB + SSD 512GB  
```
 
In order to complete the kernel source build quickly, you'll need a high-performance computer. If the kernel source build completes successfully, you should see binary files like the ones below. 
```bash
$ ls -al ./out/android-gs-pixel-5.10/dist/*.img
-rw-rw-r-- 1 invain invain 67,108,864  6 29 22:19 ./out/android-gs-pixel-5.10/dist/boot.img
-rw-rw-r-- 1 invain invain    493,712  6 29 22:18 ./out/android-gs-pixel-5.10/dist/dtb.img
-rw-rw-r-- 1 invain invain  21,686,18  6 29 20:24 ./out/android-gs-pixel-5.10/dist/dtbo.img
-rw-rw-r-- 1 invain invain 14,785,901  6 29 22:17 ./out/android-gs-pixel-5.10/dist/initramfs.img
-rw-rw-r-- 1 invain invain         38  6 29 22:19 ./out/android-gs-pixel-5.10/dist/vendor-bootconfig.img
-rw-rw-r-- 1 invain invain 34,746,368  6 29 22:19 ./out/android-gs-pixel-5.10/dist/vendor_boot.img
-rw-rw-r-- 1 invain invain 55,689,216  6 29 22:17 ./out/android-gs-pixel-5.10/dist/vendor_dlkm.img
$ ls -al ./out/android-gs-pixel-5.10/dist/*.lz4
-rw-rw-r-- 1 invain invain 23,461,879  6 29 21:34 ./out/android-gs-pixel-5.10/dist/Image.lz4
-rw-rw-r-- 1 invain invain 194,56,178  6 29 22:19 ./out/android-gs-pixel-5.10/dist/ramdisk.lz4
```

When you must modify specific kernel source codes, you can save the compilation time
(e.g., from 17 minutes to 53 seconds) by compiling the modified files only as follows.
```bash
* Option1: $ vi ./build/build.sh --> Add "exit 999" after Line 656 ("Building kernel") (To build kernel image only)
* Option2: $ time SKIP_MRPROPER=1  SKIP_DEFCONFIG=1  SKIP_EXT_MODULES=1 SKIP_CP_KERNEL_HDR=1 IN_KERNEL_MODULES=0  build/build.sh -j4 
* Option3: $ ... V=1 M=./mm/ (To compile specific kernel modules of ./mm/ folder only)
    ... wait for 53 seconds ...
```

#### Unlock the reference SmartPhone
enable oem unlock in development mode
```bash
$ adb reboot bootloader
$ fastboot flashing unlock
```


#### How to flash the ‘dirty’ kernel
When you execute the "LTO=thin ./build/build.sh" command, the 'dirty' kernel image would be generated into ./out/ folder. 
Then, pack boot images as follows.
```bash
$ cd ./out/.../dist
$ fastboot flash boot boot.img
$ fastboot flash vendor_boot vendor_boot.img
$ fastboot reboot fastboot
$ fastboot flash vendor_dlkm vendor_dlkm.img
```

The device can boot into recovery now. 
But if you want to boot into the system, you also need to disable the verified boot by running the below commands.
```bash
$ ./build/build-tools/path/linux-x86/avbtool make_vbmeta_image --flags 2 --padding_size 4096 --output vbmeta_disabled.img
$ fastboot flash vbmeta --disable-verity --disable-verification ./vbmeta_disabled.img
```


#### How to build a custom Android platform image

If you need to modify the Android platform to try various experiments, you can compile the entire code of the Android platform as follows. 
However, compiling the entire Android platform will take more than 10 hours on average in a normal desktop PC environment. 

```bash
$ mkdir aosp
$ cd aosp
$ repo init -u https://android.googlesource.com/platform/manifest -b master
$ repo sync -c -j16
$ wget https://dl.google.com/dl/android/aosp/google_devices-oriole-sq1d.220205.004-a2628da5.tgz
$ source build/envsetup.sh
$ lunch aosp_oriole_hwasan-userdebug
$ m
$ cp ../android-kernel/out/android-gs-pixel-5.10/dist/Image.lz4 ./device/google/raviole-kernel/
$ cp ../android-kernel/out/android-gs-pixel-5.10/dist/*.ko ./device/google/raviole-kernel/
$ cd out/target/product/oriole
$ adb reboot bootloader
$ ANDROID_PRODUCT_OUT=`pwd` fastboot flashall -w
```


## Reference
You can refer to the following websites to modify and manipulate the Linux kernel used on the Android platform. 
* https://source.android.com/docs/setup/build/building-kernels 
* https://source.android.com/setup/build/building-kernels
* https://android.googlesource.com/kernel/manifest
* https://www.xda-developers.com/


## Publication

* Geunsik Lim, Donghyun Kang, MyungJoo Ham, and Young Ik Eom, "[SWAM: Revisiting Swap and OOMK for Improving Application Responsiveness on Mobile Devices](https://arxiv.org/abs/2306.08345)," MobiCom 2023 (29th Annual International Conference On Mobile Computing And Networking), Oct-2023.
