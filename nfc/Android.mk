# Copyright (C) 2011 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#Enable NXP Specific
D_CFLAGS += -DNXP_EXTNS=TRUE

#variables for NFC_NXP_CHIP_TYPE
PN547C2 := 1
PN548C2 := 2
PN551   := 3
PN553   := 4
PN557   := 5

NQ110 := $PN547C2
NQ120 := $PN547C2
NQ210 := $PN548C2
NQ220 := $PN548C2

#Enable HCE-F specific
D_CFLAGS += -DNXP_NFCC_HCE_F=TRUE

#NXP PN547 Enable
ifeq ($(PN547C2),1)
D_CFLAGS += -DPN547C2=1
endif
ifeq ($(PN548C2),2)
D_CFLAGS += -DPN548C2=2
endif
ifeq ($(PN551),3)
D_CFLAGS += -DPN551=3
endif
ifeq ($(PN553),4)
D_CFLAGS += -DPN553=4
endif
ifeq ($(PN557),5)
D_CFLAGS += -DPN557=5
endif

#### Select the CHIP ####
ifneq ($(filter pn547 nq110 nq120,$(BOARD_NFC_CHIPSET)),)
NXP_CHIP_TYPE := $(PN547C2)
else ifneq ($(filter pn548 nq210 nq220,$(BOARD_NFC_CHIPSET)),)
NXP_CHIP_TYPE := $(PN548C2)
else ifeq ($(BOARD_NFC_CHIPSET),pn551)
NXP_CHIP_TYPE := $(PN551)
else ifeq ($(BOARD_NFC_CHIPSET),pn553)
NXP_CHIP_TYPE := $(PN553)
else ifeq ($(BOARD_NFC_CHIPSET),pn557)
NXP_CHIP_TYPE := $(PN557)
endif

ifeq ($(NXP_CHIP_TYPE),$(PN547C2))
D_CFLAGS += -DNFC_NXP_CHIP_TYPE=PN547C2
else ifeq ($(NXP_CHIP_TYPE),$(PN548C2))
D_CFLAGS += -DNFC_NXP_CHIP_TYPE=PN548C2
else ifeq ($(NXP_CHIP_TYPE),$(PN551))
D_CFLAGS += -DNFC_NXP_CHIP_TYPE=PN551
else ifeq ($(NXP_CHIP_TYPE),$(PN553))
D_CFLAGS += -DNFC_NXP_CHIP_TYPE=PN553
else ifeq ($(NXP_CHIP_TYPE),$(PN557))
D_CFLAGS += -DNFC_NXP_CHIP_TYPE=PN557
endif

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := nfc_nci.nqx.default
ifeq (true,$(TARGET_IS_64_BIT))
LOCAL_MULTILIB := 64
else
LOCAL_MULTILIB := 32
endif
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_SRC_FILES := \
	$(call all-c-files-under, common) \
	$(call all-c-files-under, dnld) \
	$(call all-c-files-under, hal) \
	$(call all-c-files-under, log) \
	$(call all-c-files-under, self-test) \
	$(call all-c-files-under, tml) \
	$(call all-c-files-under, utils) \
	$(call all-cpp-files-under, utils) \
	nfc_nci.c
LOCAL_SHARED_LIBRARIES := liblog libcutils libhardware_legacy libdl
LOCAL_MODULE_TAGS := optional
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := nxp

LOCAL_CFLAGS := $(D_CFLAGS)
LOCAL_C_INCLUDES += \
    $(LOCAL_PATH)/utils \
    $(LOCAL_PATH)/inc \
    $(LOCAL_PATH)/common \
    $(LOCAL_PATH)/dnld \
    $(LOCAL_PATH)/hal \
    $(LOCAL_PATH)/log \
    $(LOCAL_PATH)/tml \
    $(LOCAL_PATH)/configs \
    $(LOCAL_PATH)/self-test

LOCAL_CFLAGS += -DANDROID \
        -DNXP_HW_SELF_TEST
LOCAL_CFLAGS += -DNFC_NXP_HFO_SETTINGS=FALSE
NFC_NXP_ESE:= TRUE
ifeq ($(NFC_NXP_ESE),TRUE)
D_CFLAGS += -DNFC_NXP_ESE=TRUE
ifeq ($(NXP_CHIP_TYPE),$(PN553))
D_CFLAGS += -DJCOP_WA_ENABLE=FALSE
else
D_CFLAGS += -DJCOP_WA_ENABLE=TRUE
endif
else
D_CFLAGS += -DNFC_NXP_ESE=FALSE
endif
LOCAL_CFLAGS += $(D_CFLAGS)
#LOCAL_CFLAGS += -DFELICA_CLT_ENABLE
#-DNXP_PN547C1_DOWNLOAD

LOCAL_HEADER_LIBRARIES += libhardware_headers
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE       := libnfc-nci.conf
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)
LOCAL_SRC_FILES    := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)

#include $(CLEAR_VARS)
#LOCAL_MODULE       := libnfc-nxp.conf
#LOCAL_MODULE_TAGS  := optional
#LOCAL_MODULE_CLASS := ETC
#LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)
#ifeq ($(strip $(NQ3XX_PRESENT)),true)
#LOCAL_SRC_FILES    := libnfc-nxp-PN80T_example.conf
#else
#LOCAL_SRC_FILES    := libnfc-nxp-PN66T_example.conf
#endif
#include $(BUILD_PREBUILT)

#include $(CLEAR_VARS)
#LOCAL_MODULE       := libnfc-nxp_default.conf
#LOCAL_MODULE_TAGS  := optional
#LOCAL_MODULE_CLASS := ETC
#LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)

#ifeq ($(strip $(NQ3XX_PRESENT)),true)
#LOCAL_SRC_FILES    := libnfc-nxp-PN80T_example.conf
#else
#LOCAL_SRC_FILES    := libnfc-nxp-PN66T_example.conf
#endif
#include $(BUILD_PREBUILT)
