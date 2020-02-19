#
# Copyright (C) 2021 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

## Common Path
COMMON_PATH := device/huawei/hi6250-9-common

# Init
PRODUCT_PACKAGES += \
    fstab.hi6250 \
    fstab.hi6250_ramdisk \
    fstab.modem \
    init.audio.rc \
    init.balong_modem.rc \
    init.connectivity.bcm43455.rc \
    init.connectivity.gps.rc \
    init.connectivity.hi1102.rc \
    init.connectivity.rc \
    init.device.rc \
    init.hi6250.rc \
    init.hisi.rc \
    init.manufacture.rc \
    init.performance.rc \
    init.platform.rc \
    init.post-fs-data.rc \
    init.recovery.hi6250.rc \
    init.tee.rc \
    init.vowifi.rc

# Treble
PRODUCT_USE_VNDK_OVERRIDE := true
