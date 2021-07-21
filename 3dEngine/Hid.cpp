#include "Hid.h"

/*********************************************************************
* Constructor
*********************************************************************/
Hid::Hid()
{
    hdk2DeviceHandle = nullptr;
    HDK2 = false;
}

/*********************************************************************
* Destructor
*********************************************************************/
Hid::~Hid(void)
{
    // Close HDK2 device
    CloseHandle(hdk2DeviceHandle);
}

/*********************************************************************
* Show deviceinfo
*********************************************************************/
std::string Hid::DeviceInfo()
{
    std::string returnString = "";
    bool result;
    
    wchar_t wstr[MAX_STR];

    HID_DEVICE_INFO hid_device_info;

    // Windows objects for interacting with the driver
    GUID InterfaceClassGuid = { 0x4d1e55b2, 0xf16f, 0x11cf, {0x88, 0xcb, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30} };

    SP_DEVINFO_DATA devinfo_data;
    SP_DEVICE_INTERFACE_DATA device_interface_data;
    SP_DEVICE_INTERFACE_DETAIL_DATA_A *device_interface_detail_data = NULL;
    HDEVINFO device_info_set = INVALID_HANDLE_VALUE;

    // Initialize the Windows objects
    memset(&devinfo_data, 0x0, sizeof(devinfo_data));
    devinfo_data.cbSize = sizeof(SP_DEVINFO_DATA);
    device_interface_data.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    // Get information for all the devices belonging to the HID class
    device_info_set = SetupDiGetClassDevsA(&InterfaceClassGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

    // Iterate over each device in the HID class, looking for the right one
    unsigned int device_index = 0;
    bool ready = false;
    while (!ready)
    {
        DWORD required_size = 0;
        memset(&hid_device_info, 0x0, sizeof(hid_device_info));

        result = SetupDiEnumDeviceInterfaces(device_info_set, NULL,    &InterfaceClassGuid, device_index, &device_interface_data);
        if (result)
        {
            // Call with 0-sized detail size, and let the function tell us how long the detail struct needs to be. The size is put in &required_size
            SetupDiGetDeviceInterfaceDetailA(device_info_set, &device_interface_data, NULL, 0, &required_size, NULL);

            // Allocate a long enough structure for device_interface_detail_data
            device_interface_detail_data = (SP_DEVICE_INTERFACE_DETAIL_DATA_A*)malloc(required_size);
            device_interface_detail_data->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_A);

            // Get the detailed data for this device
            result = SetupDiGetDeviceInterfaceDetailA(device_info_set, &device_interface_data, device_interface_detail_data, required_size, NULL, NULL);
            if (result)
            {
                // Populate devinfo_data
                result = SetupDiEnumDeviceInfo(device_info_set, device_index, &devinfo_data);
                if (result)
                {
                    // Make sure this device is of Setup Class "HIDClass" and has a driver bound to it
                    char driver_name[MAX_STR];

                    result = SetupDiGetDeviceRegistryPropertyA(device_info_set, &devinfo_data, SPDRP_CLASS, NULL, (PBYTE)driver_name, sizeof(driver_name), NULL);
                    if (!result)
                    {
                        std::string message = "Could not get a driver name from the device";
                        returnString += message;
                    }

                    // See if there's a driver bound
                    if (strcmp(driver_name, "HIDClass") == 0)
                    {
                        result = SetupDiGetDeviceRegistryPropertyA(device_info_set, &devinfo_data, SPDRP_DRIVER, NULL, (PBYTE)driver_name, sizeof(driver_name), NULL);
                        if (!result)
                        {
                            std::string message = "Could not establish this driver bound to the device";
                            returnString += message;
                        }
                    }

                    // Fill out the record
                    const char *str;
                    size_t len;
                    str = device_interface_detail_data->DevicePath;
                    if (str != nullptr)
                    {
                        len = strlen(str);
                        hid_device_info.path = (char*)calloc(len + 1, sizeof(char));
                        for (int i = 0; i < len; i++)
                        {
                            hid_device_info.path[i] = str[i];
                        }

                        hid_device_info.path[len] = '\0';
                    } else
                    {
                        hid_device_info.path = NULL;
                    }

                    // Add info (so far collected)
                    returnString += "Device Nr. " + Utils::IntToStr(device_index) + "\r\n";
                    returnString += "    Path: ";
                    returnString.append(hid_device_info.path);
                    returnString += "\r\n";
                    returnString += "    Driver Name: ";
                    returnString.append(driver_name);
                    returnString += "\r\n";

                    // Open the device 
                    HANDLE hidDeviceHandle = CreateFileA(hid_device_info.path, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);
                    if (hidDeviceHandle != INVALID_HANDLE_VALUE)
                    {
                        HIDD_ATTRIBUTES attrib;
                        memset(&attrib, 0x00, sizeof(attrib));

                        // Get the Vendor ID and Product ID for this device
                        attrib.Size = sizeof(HIDD_ATTRIBUTES);
                        result = HidD_GetAttributes(hidDeviceHandle, &attrib);
                        if (!result)
                        {
                            returnString += "*Could not get attributes (Vendor ID and Product ID) from device \r\n";
                        }

                        // Get the Usage Page and Usage for this device
                        PHIDP_PREPARSED_DATA pp_data = NULL;
                        NTSTATUS nt_res;
                        HIDP_CAPS caps;
                        result = HidD_GetPreparsedData(hidDeviceHandle, &pp_data);
                        if (result) 
                        {
                            nt_res = HidP_GetCaps(pp_data, &caps);
                            if (nt_res == HIDP_STATUS_SUCCESS) 
                            {
                                hid_device_info.usage_page = caps.UsagePage;
                                hid_device_info.usage = caps.Usage;
                            }

                            HidD_FreePreparsedData(pp_data);
                        } else
                        {
                            returnString += "*Could not get usage (page) from device \r\n";
                        }

                        // Serial Number 
                        result = HidD_GetSerialNumberString(hidDeviceHandle, wstr, sizeof(wstr));
                        wstr[MAX_STR - 1] = 0x0000;
                        if (result)
                        {
                            hid_device_info.serial_number = _wcsdup(wstr);
                        } else
                        {
                            returnString += "*Could not get serial number from device \r\n";
                        }

                        // Manufacturer String 
                        result = HidD_GetManufacturerString(hidDeviceHandle, wstr, sizeof(wstr));
                        wstr[MAX_STR - 1] = 0x0000;
                        if (result)
                        {
                            hid_device_info.manufacturer_string = _wcsdup(wstr);
                        } else
                        {
                            returnString += "*Could not get manufacturer string from device \r\n";
                        }

                        // Product String 
                        result = HidD_GetProductString(hidDeviceHandle, wstr, sizeof(wstr));
                        wstr[MAX_STR - 1] = 0x0000;
                        if (result)
                        {
                            hid_device_info.product_string = _wcsdup(wstr);
                        } else
                        {
                            returnString += "*Could not get product string from device \r\n";
                        }

                        // Vedor Id / Product Id / Version Number
                        hid_device_info.vendor_id = attrib.VendorID;
                        hid_device_info.product_id = attrib.ProductID;
                        hid_device_info.version_number = attrib.VersionNumber;

                        // Interface Number 
                        // It can sometimes be parsed out of the path on Windows if a device has multiple interfaces
                        // See http://msdn.microsoft.com/en-us/windows/hardware/gg487473 or search for "Hardware IDs for HID Devices" at MSDN
                        hid_device_info.interface_number = -1;
                        if (hid_device_info.path)
                        {
                            char *interface_component = strstr(hid_device_info.path, "&mi_");
                            if (interface_component)
                            {
                                char *hex_str = interface_component + 4;
                                char *endptr = NULL;
                                hid_device_info.interface_number = strtol(hex_str, &endptr, 16);
                                if (endptr == hex_str)
                                {
                                    // The parsing failed. Set interface_number to -1
                                    hid_device_info.interface_number = -1;
                                }
                            }
                        }

                        // Add additional info
                        returnString += "    Type: 0x" + Utils::IntToStr(hid_device_info.vendor_id, 16) + "\r\n";
                        returnString += "    Serial Number " + (hid_device_info.serial_number != nullptr ? Utils::ConvertWCSToMBS(hid_device_info.serial_number, (long)wcslen(hid_device_info.serial_number)) : "-") + "\r\n";
                        returnString += "    Manufacturer: " + (hid_device_info.manufacturer_string != nullptr ? Utils::ConvertWCSToMBS(hid_device_info.manufacturer_string, (long)wcslen(hid_device_info.manufacturer_string)) : "-") + "\r\n";
                        returnString += "    Product: " + (hid_device_info.product_string != nullptr ? Utils::ConvertWCSToMBS(hid_device_info.product_string, (long)wcslen(hid_device_info.product_string)) : "-") + "\r\n";
                        returnString += "    Version Number: " + Utils::IntToStr(hid_device_info.version_number, 10) + "\r\n";
                        std::string interfacenr = hid_device_info.interface_number != -1 ? Utils::IntToStr(hid_device_info.interface_number, 10) : "-";
                        returnString += "    Interface: " + interfacenr + "\r\n";

                        // Check if this id HDK2 device
                        if (hid_device_info.vendor_id == 0x1532)
                        {
                            hdk2DeviceHandle = hidDeviceHandle;
                        } else
                        {
                            // Close device
                            CloseHandle(hidDeviceHandle);
                        }
                    } else
                    {
                        std::string message = "*Could not open the device\r\n";
                        returnString += message;
                    }

                    // Next device
                    returnString += "\r\n------------------------------------------------------------------------------------------------------------\r\n";

                    device_index++;
                } else
                {
                    std::string message = "*Could not get info from the device\r\n";
                    returnString += message;
                }
            } else
            {
                std::string message = "*Could not get additional info from the device\r\n";
                returnString += message;
            }

            // We no longer need the detail data
            free(device_interface_detail_data);
        } else
        {
            std::string message = "*End of enumeration, no more devices\r\n";
            returnString += message;
            ready = true;
        }
    }

    // Report hdk2 could (not) be found
    if (hdk2DeviceHandle == nullptr)
    {
        HDK2 = false;
        returnString += "\r\n*HDK2 Not Found\r\n";
    } else
    {
        HDK2 = true;
        returnString += "\r\n*HDK2 Found\r\n";
    }

    return (returnString);
}

/*********************************************************************
* Get HDK2 device data
*********************************************************************/
bool Hid::GetData(PACKET *packet)
{
    std::string strResult = "";
    std::string returnString = "";
    byte data[HDK_DATA_SIZE];

    // Read
    memset(data, 0x00, HDK_DATA_SIZE);
    strResult = DeviceRead(hdk2DeviceHandle, data, HDK_DATA_SIZE);
    if (strResult.substr(0, 2) == "OK")
    {
        packet->first        = data[ 0]; // Not sure what this is (TODO: sorting out)
        packet->command        = data[ 1];
        packet->sequence    = data[ 2];
        packet->quat[0]        = data[ 3] + (data[ 4] << 8);
        packet->quat[1]        = data[ 5] + (data[ 6] << 8);
        packet->quat[2]        = data[ 7] + (data[ 8] << 8);
        packet->quat[3]        = data[ 9] + (data[10] << 8);
        packet->accel[0]    = data[11] + (data[12] << 8);
        packet->accel[1]    = data[13] + (data[14] << 8);
        packet->accel[2]    = data[15] + (data[16] << 8);
    } else
    {
        return (false);
    }

    return (true);
}

/*********************************************************************
* Write to device
*********************************************************************/
std::string Hid::DeviceWrite(HANDLE hidDeviceHandle, unsigned char *data, size_t length)
{
    DWORD bytes_written = 0;
    bool result;
    std::string message;

    OVERLAPPED overlapped;
    memset(&overlapped, 0, sizeof(overlapped));
    overlapped.hEvent = CreateEvent(NULL, false, false, NULL);
	if (overlapped.hEvent != NULL)
	{
		SetLastError(0);
		result = WriteFile(hidDeviceHandle, data, (DWORD)length, NULL, &overlapped);
		if (!result)
		{
			void *pMsgBuf;
			DWORD error = GetLastError();
			if (error != ERROR_IO_PENDING)
			{
				message = "*Could not write to device: ";
				FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&pMsgBuf, 0, NULL);
				message.append((char*)pMsgBuf);

				SetLastError(0);
				result = CancelIo(hidDeviceHandle);
				if (!result)
				{
					void *pMsgBuf;
					DWORD error = GetLastError();
					FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&pMsgBuf, 0, NULL);

					message = "*Could not cancel write to device: ";
					message.append((char*)pMsgBuf);
					message.append("\r\n");
				}

				return (message);
			}
		}

		// See if there is any data yet within the selected timeout
		DWORD waitResult = WaitForSingleObject(overlapped.hEvent, TIMEOUT);
		switch (waitResult)
		{
			void *pMsgBuf;
			DWORD error;

		case WAIT_FAILED:
			error = GetLastError();
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&pMsgBuf, 0, NULL);

			message = "*Could not wait for write to device: ";
			message.append((char*)pMsgBuf);
			message.append("\r\n");
			return (message);
			break;

		case WAIT_OBJECT_0:
			SetLastError(0);
			result = GetOverlappedResult(hidDeviceHandle, &overlapped, &bytes_written, FALSE);
			if (!result)
			{
				void *pMsgBuf;
				DWORD error = GetLastError();
				FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&pMsgBuf, 0, NULL);

				message = "*Could not get overlaped result: ";
				message.append((char*)pMsgBuf);

				SetLastError(0);
				result = CancelIo(hidDeviceHandle);
				if (!result)
				{
					void *pMsgBuf;
					DWORD error = GetLastError();
					FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&pMsgBuf, 0, NULL);

					message.append("*Could not cancel write to device: ");
					message.append((char*)pMsgBuf);
					message.append("\r\n");
				}

				return (message);
			}
			return ("OK, Written " + Utils::IntToStr(bytes_written) + "\r\n");
			break;

		case WAIT_TIMEOUT:
			message = "*Timeout in write to device";
			return (message);
			break;

		case WAIT_ABANDONED:
			message = "*Waiting abandoned in write to device";
			return (message);
			break;

		default:
			message = "*Unknown wait-result status in write to device";
			return (message);
			break;
		}
	}

	return ("*Could not write to device");
}

/*********************************************************************
* Read from device
*********************************************************************/
std::string Hid::DeviceRead(HANDLE hidDeviceHandle, unsigned char *data, size_t length)
{
    DWORD bytes_read = 0;
    BOOL result;
    std::string message;

    OVERLAPPED overlapped;
    memset(&overlapped, 0, sizeof(overlapped));
    overlapped.hEvent = CreateEvent(NULL, false, false, NULL);

	if (overlapped.hEvent != NULL)
	{
		SetLastError(0);
		result = ReadFile(hidDeviceHandle, data, (DWORD)length, &bytes_read, &overlapped);
		if (!result)
		{
			void *pMsgBuf;
			DWORD error = GetLastError();
			if (error != ERROR_IO_PENDING)
			{
				message = "*Could not read from device: ";
				FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&pMsgBuf, 0, NULL);
				message.append((char*)pMsgBuf);

				SetLastError(0);
				result = CancelIo(hidDeviceHandle);
				if (!result)
				{
					void *pMsgBuf;
					DWORD error = GetLastError();
					FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&pMsgBuf, 0, NULL);

					message = "*Could not cancel read from device: ";
					message.append((char*)pMsgBuf);
					message.append("\r\n");
				}

				return (message);
			}
		}

		// See if there is any data yet within the selected timeout
		DWORD waitResult = WaitForSingleObject(overlapped.hEvent, TIMEOUT);
		switch (waitResult)
		{
			void *pMsgBuf;
			DWORD error;
        
			case WAIT_FAILED:
				error = GetLastError();
				FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&pMsgBuf, 0, NULL);

				message = "*Could not wait for read from device: ";
				message.append((char*)pMsgBuf);
				message.append("\r\n");
				return (message);
				break;

			case WAIT_OBJECT_0:
				SetLastError(0);
				result = GetOverlappedResult(hidDeviceHandle, &overlapped, &bytes_read, FALSE);
				if (!result)
				{
					void *pMsgBuf;
					DWORD error = GetLastError();
					FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&pMsgBuf, 0, NULL);

					message = "*Could not get overlaped result: ";
					message.append((char*)pMsgBuf);

					SetLastError(0);
					result = CancelIo(hidDeviceHandle);
					if (!result)
					{
						void *pMsgBuf;
						DWORD error = GetLastError();
						FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&pMsgBuf, 0, NULL);

						message.append("*Could not cancel read from device: ");
						message.append((char*)pMsgBuf);
						message.append("\r\n");
					}

					return (message);
				}
				return ("OK, Read " + Utils::IntToStr(bytes_read) + "\r\n");
				break;

			case WAIT_TIMEOUT:
				message = "*Timeout in read from device";
				return (message);
				break;

			case WAIT_ABANDONED:
				message = "*Waiting abandoned in read from device";
				return (message);
				break;

			default:
				message = "*Unknown wait-result status in read from device";
				return (message);
				break;
		}
	}

	return ("*Could not read from device");
}

