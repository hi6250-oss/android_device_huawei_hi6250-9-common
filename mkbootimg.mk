LOCAL_PATH := $(call my-dir)

#
# Global Flags
#
HW_MKBOOTIMG := $(TARGET_KERNEL_SOURCE)/tools/mkbootimg

#
# kernel.img
#

INTERNAL_CUSTOM_BOOTIMAGE_ARGS := --base $(BOARD_KERNEL_BASE) --pagesize $(BOARD_KERNEL_PAGESIZE) --kernel $(INSTALLED_KERNEL_TARGET) --ramdisk $(BUILT_RAMDISK_TARGET) --cmdline "$(BOARD_KERNEL_CMDLINE) buildvariant=userdebug"

$(INSTALLED_BOOTIMAGE_TARGET): $(HW_MKBOOTIMG) $(INTERNAL_BOOTIMAGE_FILES) $(BOOTIMAGE_EXTRA_DEPS) $(INSTALLED_KERNEL_TARGET)
	$(call pretty,"Target boot image: $@")
	$(hide) $(HW_MKBOOTIMG) $(INTERNAL_CUSTOM_BOOTIMAGE_ARGS) $(INTERNAL_MKBOOTIMG_VERSION_ARGS) $(BOARD_MKBOOTIMG_ARGS) --output $@
	$(hide) $(call assert-max-image-size,$@,$(BOARD_BOOTIMAGE_PARTITION_SIZE))

#
# recovery_ramdisk.img
#

INTERNAL_CUSTOM_RECOVERYIMAGE_ARGS := --kernel /dev/null --ramdisk $(recovery_ramdisk) --cmdline "buildvariant=$(TARGET_BUILD_VARIANT)"
BOARD_CUSTOM_RECOVERY_MKBOOTIMG_ARGS := --kernel_offset 0x00008000 --ramdisk_offset 0x01000000 --second_offset 0x00f00000 --tags_offset 0x00000100

$(INSTALLED_RECOVERYIMAGE_TARGET): $(MKBOOTIMG) $(recovery_ramdisk)
	@echo "----- Making recovery image ------"
	$(hide) $(MKBOOTIMG) $(INTERNAL_CUSTOM_RECOVERYIMAGE_ARGS) $(INTERNAL_MKBOOTIMG_VERSION_ARGS) $(BOARD_INTERNAL_RECOVERY_MKBOOTIMG_ARGS) --output $@
	$(hide) $(call assert-max-image-size,$@,$(BOARD_RECOVERYIMAGE_PARTITION_SIZE))

#
# ramdisk-recovery_vendor.img
#

INSTALLED_RECVENDOR_RAMDISK_TARGET := $(PRODUCT_OUT)/ramdisk-recovery_vendor.img
INSTALLED_RECVENDOR_RAMDISK_PLACEHOLDER_TARGET := $(PRODUCT_OUT)/ramdisk-recovery_vendor

$(INSTALLED_RECVENDOR_RAMDISK_PLACEHOLDER_TARGET):
	$(hide) mkdir -p $@/vendor
	$(hide) touch $@/vendor/placeholder

$(INSTALLED_RECVENDOR_RAMDISK_TARGET): $(MKBOOTFS) $(MINIGZIP) $(INSTALLED_RECVENDOR_RAMDISK_PLACEHOLDER_TARGET)
	$(call pretty,"Target recovery_vendor ramdisk image: $@")
	$(hide) $(MKBOOTFS) -d $(PRODUCT_OUT) $(INSTALLED_RECVENDOR_RAMDISK_PLACEHOLDER_TARGET) | $(MINIGZIP) > $@

#
# recovery_vendor.img
#

INSTALLED_RECVENDORIMAGE_TARGET := $(PRODUCT_OUT)/recovery_vendor.img
INTERNAL_CUSTOM_RECVENDORIMAGE_ARGS := --kernel /dev/null --ramdisk $(INSTALLED_RECVENDOR_RAMDISK_TARGET) --cmdline buildvariant=$(TARGET_BUILD_VARIANT)
BOARD_CUSTOM_RECVENDOR_MKBOOTIMG_ARGS := --kernel_offset 0x00008000 --ramdisk_offset 0x01000000 --second_offset 0x00f00000 --tags_offset 0x00000100

.PHONY: recoveryvendorimage
recoveryvendorimage: $(INSTALLED_RECVENDORIMAGE_TARGET)

$(INSTALLED_RECVENDORIMAGE_TARGET): $(HW_MKBOOTIMG) $(INSTALLED_RECVENDOR_RAMDISK_TARGET)
	$(call pretty,"Target recovery_vendor image: $@")
	$(hide) $(HW_MKBOOTIMG) $(INTERNAL_CUSTOM_RECVENDORIMAGE_ARGS) $(INTERNAL_MKBOOTIMG_VERSION_ARGS) $(BOARD_CUSTOM_RECVENDOR_MKBOOTIMG_ARGS) --output $@
	$(hide) $(call assert-max-image-size,$@,$(call get-hash-image-max-size,$(BOARD_RECVENDORIMAGE_PARTITION_SIZE)))

INSTALLED_RADIOIMAGE_TARGET += $(INSTALLED_RECVENDORIMAGE_TARGET)
