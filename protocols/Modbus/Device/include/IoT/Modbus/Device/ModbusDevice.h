//
// ModbusDevice.h
//
// Library: IoT/Modbus/Device
// Package: Device
// Module:  ModbusDevice
//
// Basic definitions for the IoT Modbus Device library.
// This file must be the first file included by every other Modbus
// Device header file.
//
// Copyright (c) 2023, Applied Informatics Software Engineering GmbH.
// All rights reserved.
//
// SPDX-License-Identifier: GPL-3.0-only
//


#ifndef IoT_Modbus_Device_ModbusDevice_INCLUDED
#define IoT_Modbus_Device_ModbusDevice_INCLUDED


#include "Poco/Poco.h"


//
// The following block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the IoTModbusDevice_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// IoTModbusDevice_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
//
#if defined(_WIN32) && defined(POCO_DLL)
	#if defined(IoTModbusDevice_EXPORTS)
		#define IoTModbusDevice_API __declspec(dllexport)
	#else
		#define IoTModbusDevice_API __declspec(dllimport)
	#endif
#endif


#if !defined(IoTModbusDevice_API)
	#define IoTModbusDevice_API
#endif


//
// Automatically link IoTModbusDevice library.
//
#if defined(_MSC_VER)
	#if !defined(POCO_NO_AUTOMATIC_LIBS) && !defined(IoTModbusDevice_EXPORTS)
		#pragma comment(lib, "IoTModbusDevice" POCO_LIB_SUFFIX)
	#endif
#endif


#endif // IoT_Modbus_Device_ModbusDevice_INCLUDED
