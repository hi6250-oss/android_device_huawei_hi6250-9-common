#
# Copyright (C) 2021 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

## Common Path
COMMON_PATH := device/huawei/hi6250-9-common

# Arch
TARGET_ARCH := arm64
TARGET_ARCH_VARIANT := armv8-a
TARGET_CPU_ABI := arm64-v8a
TARGET_CPU_VARIANT := cortex-a53

TARGET_2ND_ARCH := arm
TARGET_2ND_ARCH_VARIANT := armv8-a
TARGET_2ND_CPU_ABI := armeabi-v7a
TARGET_2ND_CPU_ABI2 := armeabi
TARGET_2ND_CPU_VARIANT := cortex-a53

# Bootloader
TARGET_BOOTLOADER_BOARD_NAME := hi6250

# HIDL
DEVICE_MANIFEST_FILE := $(COMMON_PATH)/manifest.xml
DEVICE_MATRIX_FILE := $(COMMON_PATH)/compatibility_matrix.xml

# Kernel
BOARD_CUSTOM_BOOTIMG := true
BOARD_CUSTOM_BOOTIMG_MK := $(COMMON_PATH)/mkbootimg.mk
BOARD_KERNEL_BASE := 0x00478000
BOARD_KERNEL_CMDLINE := loglevel=4 coherent_pool=512K page_tracker=on slub_min_objects=12 unmovable_isolate1=2:192M,3:224M,4:256M printktimer=0xfff0a000,0x534,0x538 androidboot.selinux=permissive
BOARD_KERNEL_IMAGE_NAME := Image.gz
BOARD_KERNEL_PAGESIZE := 2048
BOARD_MKBOOTIMG_ARGS := --kernel_offset 0x00008000 --ramdisk_offset 0x0ff88000 --second_offset 0x00e88000 --tags_offset 0x07988000
TARGET_KERNEL_CLANG_COMPILE := true

# Kernel Source
TARGET_KERNEL_CONFIG := merge_hi6250_defconfig
TARGET_KERNEL_SOURCE := kernel/huawei/hi6250-9

# NFC
NXP_CHIP_TYPE := 2
TARGET_USES_NQ_NFC := true

# Partitions
BOARD_CACHEIMAGE_FILE_SYSTEM_TYPE := ext4
BOARD_FLASH_BLOCK_SIZE := 4096 # blockdev --getbsz /dev/block/mmcblk0
BOARD_ODMIMAGE_FILE_SYSTEM_TYPE := ext4
BOARD_VENDORIMAGE_FILE_SYSTEM_TYPE := ext4
TARGET_COPY_OUT_ODM := odm
TARGET_COPY_OUT_VENDOR := vendor
TARGET_USERIMAGES_USE_EXT4 := true

# Platform
TARGET_BOARD_PLATFORM := hi6250

# Props
TARGET_VENDOR_PROP += $(COMMON_PATH)/vendor.prop

# Recovery
BOARD_USES_FULL_RECOVERY_IMAGE := true
TARGET_RECOVERY_FSTAB := $(COMMON_PATH)/configs/init/fstab.hi6250

# Releasetools
TARGET_RELEASETOOLS_EXTENSIONS := $(COMMON_PATH)/releasetools

# RIL
BOARD_PROVIDES_LIBRIL := true
ENABLE_VENDOR_RIL_SERVICE := true

# Releasetools
TARGET_RELEASETOOLS_EXTENSIONS := $(COMMON_PATH)/releasetools

# Root
BOARD_ROOT_EXTRA_FOLDERS += \
    3rdmodem \
    3rdmodemnvm \
    3rdmodemnvmbkp \
    cust \
    modem_log \
    sec_storage \
    splash2 \
    version

# SELinux
BOARD_VENDOR_SEPOLICY_DIRS += $(COMMON_PATH)/sepolicy/vendor

# Soong namespaces
PRODUCT_SOONG_NAMESPACES += $(COMMON_PATH)

# Vendor
VENDOR_SECURITY_PATCH := 2018-06-19 # FIG-LX1 9.1.0.163(C432E8R1P5T8)

# Wifi
BOARD_HOSTAPD_DRIVER := NL80211
BOARD_HOSTAPD_PRIVATE_LIB := lib_driver_cmd_bcmdhd
BOARD_WLAN_DEVICE := bcmdhd
BOARD_WPA_SUPPLICANT_DRIVER := NL80211
BOARD_WPA_SUPPLICANT_PRIVATE_LIB := lib_driver_cmd_bcmdhd
WIFI_HIDL_UNIFIED_SUPPLICANT_SERVICE_RC_ENTRY := true
WPA_SUPPLICANT_VERSION := VER_0_8_X
