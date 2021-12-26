#!/bin/env python3

#
# Copyright (C) 2021 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

#
# This releasetools extension blocks TWRP from installing this ROM.
#

import common
import re

def FullOTA_Assertions(info):
  OTA_Assertions(info)
  return

def IncrementalOTA_Assertions(info):
  OTA_Assertions(info)
  return

def FullOTA_InstallEnd(info):
  OTA_InstallEnd(info)
  return

def IncrementalOTA_InstallEnd(info):
  OTA_InstallEnd(info)
  return

def AddImage(info, dir, basename, dest):
  data = info.input_zip.read(dir + "/" + basename)
  common.ZipWriteStr(info.output_zip, basename, data)
  info.script.Print("Patching {} image unconditionally...".format(dest.split('/')[-1]))
  info.script.AppendExtra('package_extract_file("%s", "%s");' % (basename, dest))

def OTA_Assertions(info):
  info.script.AppendExtra('assert(getprop("ro.twrp.boot") == "" || abort("ERROR: TWRP detected! Please use provided Lineage Recovery! For more info, read t.me/hi6250_oss/12."););')
  return

def OTA_InstallEnd(info):
  AddImage(info, "RADIO", "recovery_vendor.img", "/dev/block/platform/hi_mci.0/by-name/recovery_vendor")
