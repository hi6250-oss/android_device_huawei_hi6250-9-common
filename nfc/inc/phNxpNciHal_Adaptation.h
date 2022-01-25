/*
 * Copyright (c) 2015, The Linux Foundation. All rights reserved.
 * Not a Contribution.
 *
 * Copyright (C) 2015 NXP Semiconductors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _PHNXPNCIHAL_ADAPTATION_H_
#define _PHNXPNCIHAL_ADAPTATION_H_

#include <hardware/hardware.h>
#include <hardware/nfc.h>

#define NFC_NCI_NXP_PN54X_HARDWARE_MODULE_ID "nfc_nci.nqx"

typedef struct
{
    struct nfc_nci_device nci_device;

    /* Local definitions */
    int (*ioctl)(const struct nfc_nci_device *p_dev, long arg, void *p_data);
    int(*check_fw_dwnld_flag)(const struct nfc_nci_device *p_dev, uint8_t* param1);
} pn547_dev_t;

#define MAX_IOCTL_TRANSCEIVE_CMD_LEN  256
#define MAX_IOCTL_TRANSCEIVE_RESP_LEN 256
#define MAX_ATR_INFO_LEN              128

/*
 * Data structures provided below are used of Hal Ioctl calls
 */
/*
 * nfc_nci_ExtnCmd_t shall contain data for commands used for transceive command in ioctl
 */
typedef struct
{
    uint16_t cmd_len;
    uint8_t  p_cmd[MAX_IOCTL_TRANSCEIVE_CMD_LEN];
} nfc_nci_ExtnCmd_t;

/*
 * nfc_nci_ExtnRsp_t shall contain response for command sent in transceive command
 */
typedef struct
{
    uint16_t rsp_len;
    uint8_t  p_rsp[MAX_IOCTL_TRANSCEIVE_RESP_LEN];
} nfc_nci_ExtnRsp_t;
/*
 * InputData_t :ioctl has multiple subcommands
 * Each command has corresponding input data which needs to be populated in this
 */
typedef union {
    uint16_t          bootMode;
    uint8_t           halType;
    nfc_nci_ExtnCmd_t nciCmd;
    uint32_t          timeoutMilliSec;
    long              nfcServicePid;
}InputData_t;
/*
 * nfc_nci_ExtnInputData_t :Apart from InputData_t, there are context data
 * which is required during callback from stub to proxy.
 * To avoid additional copy of data while propagating from libnfc to Adaptation
 * and Nfcstub to ncihal, common structure is used. As a sideeffect, context data
 * is exposed to libnfc (Not encapsulated).
 */
typedef struct {
    /*
     * context to be used/updated only by users of proxy & stub of Nfc.hal
     * i.e NfcAdaptation & Nfc hidl
     */
    void*       context;
    InputData_t data;
}nfc_nci_ExtnInputData_t;

/*
 * outputData_t :ioctl has multiple commands/responses
 * This contains the output types for each ioctl.
 */
typedef union{
    uint32_t            status;
    nfc_nci_ExtnRsp_t   nciRsp;
    uint8_t             nxpNciAtrInfo[MAX_ATR_INFO_LEN];
    uint32_t            p61CurrentState;
    uint16_t            fwUpdateInf;
    uint16_t            fwDwnldStatus;
    uint16_t            fwMwVerStatus;
    uint8_t             chipType;
}outputData_t;

/*
 * nfc_nci_ExtnOutputData_t :Apart from outputData_t, there are other information
 * which is required during callback from stub to proxy.
 * For ex (context, result of the operation , type of ioctl which was completed).
 * To avoid additional copy of data while propagating from libnfc to Adaptation
 * and Nfcstub to ncihal, common structure is used. As a sideeffect, these data
 * is exposed(Not encapsulated).
 */
typedef struct {
    /*
     * ioctlType, result & context to be used/updated only by users of
     * proxy & stub of Nfc.hal.
     * i.e, NfcAdaptation & Nfc hidl
     * These fields shall not be used by libnfc or halimplementation
     */
    uint64_t     ioctlType;
    uint32_t     result;
    void*        context;
    outputData_t data;
}nfc_nci_ExtnOutputData_t;

/*
 * nfc_nci_IoctlInOutData_t :data structure for input & output
 * to be sent for ioctl command. input is populated by client/proxy side
 * output is provided from server/stub to client/proxy
 */
typedef struct {
    nfc_nci_ExtnInputData_t  inp;
    nfc_nci_ExtnOutputData_t out;
}nfc_nci_IoctlInOutData_t;

/* NXP HAL functions */

int phNxpNciHal_open(nfc_stack_callback_t* p_cback,
                     nfc_stack_data_callback_t* p_data_cback);
int phNxpNciHal_write(uint16_t data_len, const uint8_t* p_data);
int phNxpNciHal_ioctl(long arg, void* p_data);
int phNxpNciHal_core_initialized(uint8_t* p_core_init_rsp_params);
int phNxpNciHal_pre_discover(void);
int phNxpNciHal_close(void);
int phNxpNciHal_control_granted(void);
int phNxpNciHal_power_cycle(void);
int phNxpNciHal_MinOpen(nfc_stack_callback_t* p_cback,
                        nfc_stack_data_callback_t* p_data_cback);
int phNxpNciHal_Minclose(void);
int phNxpNciHal_getFWDownloadFlag(uint8_t* fwDnldRequest);
#endif /* _PHNXPNCIHAL_ADAPTATION_H_ */
