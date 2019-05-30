#pragma once
#include <windows.h>
#include <winnt.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <string.h>
#include <setupapi.h>
#include <winioctl.h>
#include <hidsdi.h>
#include "Error.h"

class Hid
{
    #define MAX_STR 256
    #define DATA_SIZE 256
    #define HDK_DATA_SIZE 17
    #define TIMEOUT 50 // in milliseconds

    public:

        typedef struct 
        {
            char            *path                = nullptr;    // Platform-specific device path 
            unsigned short   vendor_id           = 0;        // Device Vendor ID 
            unsigned short   product_id          = 0;        // Device Product ID
            wchar_t         *serial_number       = nullptr;    // Serial Number
            unsigned short   version_number      = 0;        // Device Version Number 
            wchar_t         *manufacturer_string = nullptr;    // Manufacturer String 
            wchar_t         *product_string      = nullptr;    // Product string 
            unsigned short   usage_page          = 0;        // Usage Page for this Device/Interface
            unsigned short   usage               = 0;        // Usage for this Device/Interface
            int              interface_number    = 0;        // The HID interface which this logical device represents
        } HID_DEVICE_INFO;

        typedef struct 
        {
            unsigned char first;
            unsigned char command;
            unsigned char sequence;
            short          quat[4];
            short          accel[3];
        } PACKET;

        bool HDK2;

    private:

        // If hdk2 found, this is the handle to the device
        HANDLE hdk2DeviceHandle;

        // Write to device
        std::string DeviceWrite(HANDLE hidDeviceHandle, unsigned char *data, size_t length);

        // Read from device
        std::string DeviceRead(HANDLE hidDeviceHandle, unsigned char *data, size_t length);

    public:

        // Constructor
        Hid(void);

        // Destructor
        ~Hid(void);

        // Show deviceinfo
        std::string DeviceInfo(void);

        // Get HDK2 device data
        bool GetData(PACKET *packet);
};
