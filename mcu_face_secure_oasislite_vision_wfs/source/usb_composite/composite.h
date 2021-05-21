/*
 * Copyright 2017-2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _USB_DEVICE_COMPOSITE_H_
#define _USB_DEVICE_COMPOSITE_H_ 1
#include "FreeRTOS.h"
#include "semphr.h"
#include "event_groups.h"

#include "virtual_com.h"
#include "video_camera.h"
#include "disk_card.h"
/*******************************************************************************
* Definitions
******************************************************************************/
#if defined(USB_DEVICE_CONFIG_EHCI) && (USB_DEVICE_CONFIG_EHCI > 0)
#define CONTROLLER_ID kUSB_ControllerEhci0
#endif
#if defined(USB_DEVICE_CONFIG_KHCI) && (USB_DEVICE_CONFIG_KHCI > 0)
#define CONTROLLER_ID kUSB_ControllerKhci0
#endif
#if defined(USB_DEVICE_CONFIG_LPCIP3511FS) && (USB_DEVICE_CONFIG_LPCIP3511FS > 0U)
#define CONTROLLER_ID kUSB_ControllerLpcIp3511Fs0
#endif
#if defined(USB_DEVICE_CONFIG_LPCIP3511HS) && (USB_DEVICE_CONFIG_LPCIP3511HS > 0U)
#define CONTROLLER_ID kUSB_ControllerLpcIp3511Hs0
#endif


typedef struct _usb_device_composite_struct
{
    usb_device_handle deviceHandle;                           /* USB device handle. */
    serial_usb_cdc_state_t cdcVcom[USB_DEVICE_CONFIG_CDC_ACM]; /* CDC virtual com device structure. */
#if RTFFI_USB_VIDEO
    usb_video_camera_struct_t g_UsbDeviceVideoCamera; 
#endif
#if RTFFI_USB_DISK
    usb_msc_struct_t mscDisk; /* MSC disk device structure. */
#endif
    TaskHandle_t applicationTaskHandle;                       /* Application task handle. */
    TaskHandle_t deviceTaskHandle;                            /* USB device task handle. */
    uint8_t speed;  /* Speed of USB device. USB_SPEED_FULL/USB_SPEED_LOW/USB_SPEED_HIGH.                 */
    uint8_t attach; /* A flag to indicate whether a usb device is attached. 1: attached, 0: not attached */
    uint8_t currentConfiguration; /* Current configuration value. */
    uint8_t
        currentInterfaceAlternateSetting[USB_INTERFACE_COUNT]; /* Current alternate setting value for each interface. */
    uint8_t instance;                                                       /* The instance of the interface */
    uint8_t irqNumber;                                                      /* The IRQ number of the interface */
} usb_device_composite_struct_t;

/*******************************************************************************
* API
******************************************************************************/
/*!
 * @brief CDC class specific callback function.
 *
 * This function handles the CDC class specific requests.
 *
 * @param handle          The CDC ACM class handle.
 * @param event           The CDC ACM class event type.
 * @param param           The parameter of the class specific request.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
extern usb_status_t USB_DeviceCdcVcomCallback(class_handle_t handle, uint32_t event, void *param);

/*!
 * @brief Virtual COM device set configuration function.
 *
 * This function sets configuration for CDC class.
 *
 * @param handle The CDC ACM class handle.
 * @param configure The CDC ACM class configure index.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
extern usb_status_t USB_DeviceCdcVcomSetConfigure(class_handle_t handle, uint8_t configure);
/*!
 * @brief Virtual COM device initialization function.
 *
 * This function initializes the device with the composite device class information.
 *
 * @param deviceComposite The pointer to the composite device structure.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
extern usb_status_t USB_DeviceCdcVcomInit(usb_device_composite_struct_t *deviceComposite);
/*!
 * @brief Application task function.
 *
 * This function runs the task for application.
 *
 * @return None.
 */
extern void USB_DeviceCdcVcomTask(void);
#if RTFFI_USB_VIDEO

/*!
 * @brief Video class specific callback function.
 *
 * This function handles the Video class specific requests.
 *
 * @param handle          The Video class handle.
 * @param event           The Video class event type.
 * @param param           The parameter of the class specific request.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
extern usb_status_t USB_DeviceVideoCallback(class_handle_t handle, uint32_t event, void *param);

/*!
 * @brief Set configuration of the Video class.
 *
 * Set the configuration for the Video application to default
 *.
 */
extern void USB_DeviceVideoApplicationSetDefault(void);

/*!
 * @brief Prepare the transfer buffer for Video.
 *
 * Prepare the transfer buffer and set header for the USB pakage.
 *
 */
extern void USB_DeviceVideoPrepareVideoData(void);
#endif

#if RTFFI_USB_DISK
/*!
 * @brief MSC class specific callback function.
 *
 * This function handles the MSCclass specific requests.
 *
 * @param handle          The MSC class handle.
 * @param event           The MSC class event type.
 * @param param           The parameter of the class specific request.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
extern usb_status_t USB_DeviceMscCallback(class_handle_t handle, uint32_t event, void *param);
/*!
 * @brief MSC disk device set configuration function.
 *
 * This function sets configuration for MSC class.
 *
 * @param handle The MSC class handle.
 * @param configure The MSC class configure index.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
extern usb_status_t USB_DeviceMscDiskSetConfigure(class_handle_t handle, uint8_t configure);
/*!
 * @brief MSC disk device initialization function.
 *
 * This function initializes the device with the composite device class information.
 *
 * @param deviceComposite The pointer to the composite device structure.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
extern usb_status_t USB_DeviceMscDiskInit(usb_device_composite_struct_t *deviceComposite);
#endif

#endif /* _USB_DEVICE_COMPOSITE_H_ */
