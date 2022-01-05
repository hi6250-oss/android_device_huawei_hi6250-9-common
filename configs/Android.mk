#
# Copyright (C) 2021 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

LOCAL_PATH := $(call my-dir)

#
# init
#

include $(CLEAR_VARS)
LOCAL_MODULE := fstab.hi6250
LOCAL_SRC_FILES := init/fstab.hi6250
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_ETC)
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := fstab.hi6250_ramdisk
LOCAL_SRC_FILES := init/fstab.hi6250
LOCAL_MODULE_STEM := fstab.hi6250
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_RAMDISK_OUT)
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := init.recovery.hi6250.rc
LOCAL_SRC_FILES := init/init.recovery.hi6250.rc
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_RECOVERY_OUT)/root
include $(BUILD_PREBUILT)
