//
// BundleActivator.cpp
//
// $Id$
//
// Copyright (c) 2015, Applied Informatics Software Engineering GmbH.
// All rights reserved.
//
// SPDX-License-Identifier: Apache-2.0
//


#include "Poco/OSP/BundleActivator.h"
#include "Poco/OSP/BundleContext.h"
#include "Poco/OSP/Bundle.h"
#include "Poco/OSP/ServiceRegistry.h"
#include "Poco/OSP/ServiceRef.h"
#include "Poco/OSP/ServiceFinder.h"
#include "Poco/OSP/PreferencesService.h"
#include "Poco/RemotingNG/ORB.h"
#include "Poco/Util/Timer.h"
#include "Poco/Util/TimerTask.h"
#include "IoT/Devices/SensorServerHelper.h"
#include "IoT/BtLE/BlueZGATTClient.h"
#include "IoT/BtLE/PeripheralImpl.h"
#include "Poco/Delegate.h"
#include "Poco/ClassLibrary.h"
#include "Poco/Format.h"
#include "Poco/NumberFormatter.h"
#include "Poco/SharedPtr.h"
#include "Poco/String.h"
#include "Poco/StringTokenizer.h"
#include "SensorTag.h"
#include <vector>
#include <map>


using Poco::OSP::BundleContext;
using Poco::OSP::ServiceRegistry;
using Poco::OSP::ServiceRef;
using Poco::OSP::ServiceFinder;
using Poco::OSP::Properties;
using Poco::OSP::PreferencesService;


namespace IoT {
namespace BtLE {
namespace SensorTag {


class BundleActivator: public Poco::OSP::BundleActivator
{
public:
	BundleActivator()
	{
	}
	
	~BundleActivator()
	{
	}
	
	void createSensor(Peripheral::Ptr pPeripheral, const SensorTagSensor::Params& params)
	{
		typedef Poco::RemotingNG::ServerHelper<IoT::Devices::Sensor> ServerHelper;

		Poco::SharedPtr<SensorTagSensor> pSensor;
		if (params.version == 1)
		{
			if (params.physicalQuantity == "ambientTemperature")
				pSensor = new SensorTag1IRAmbientTemperatureSensor(pPeripheral, params, _pTimer);
			else if (params.physicalQuantity == "objectTemperature")
				pSensor = new SensorTag1IRObjectTemperatureSensor(pPeripheral, params, _pTimer);
			else if (params.physicalQuantity == "humidity")
				pSensor = new SensorTagHumiditySensor(pPeripheral, params, _pTimer);
			else if (params.physicalQuantity == "airPressure")
				pSensor = new SensorTag1AirPressureSensor(pPeripheral, params, _pTimer);
			else 
				throw Poco::InvalidArgumentException("Unknown sensor type", params.physicalQuantity);
		}
		else
		{
			if (params.physicalQuantity == "ambientTemperature")
				pSensor = new SensorTag2IRAmbientTemperatureSensor(pPeripheral, params, _pTimer);
			else if (params.physicalQuantity == "objectTemperature")
				pSensor = new SensorTag2IRObjectTemperatureSensor(pPeripheral, params, _pTimer);
			else if (params.physicalQuantity == "humidity")
				pSensor = new SensorTagHumiditySensor(pPeripheral, params, _pTimer);
			else if (params.physicalQuantity == "illuminance")
				pSensor = new SensorTag2LightSensor(pPeripheral, params, _pTimer);
			else if (params.physicalQuantity == "airPressure")
				pSensor = new SensorTag2AirPressureSensor(pPeripheral, params, _pTimer);
			else 
				throw Poco::InvalidArgumentException("Unknown sensor type", params.physicalQuantity);
		}

		std::string oid(SensorTagSensor::SYMBOLIC_NAME);
		oid += ".";
		oid += params.physicalQuantity;
		oid += '#';
		oid += pPeripheral->address();

		ServerHelper::RemoteObjectPtr pSensorRemoteObject = ServerHelper::createRemoteObject(pSensor, oid);
		
		Properties props;
		props.set("io.macchina.device", SensorTagSensor::SYMBOLIC_NAME);
		props.set("io.macchina.physicalQuantity", params.physicalQuantity);
		props.set("io.macchina.btle.address", pPeripheral->address());
		
		ServiceRef::Ptr pServiceRef = _pContext->registry().registerService(oid, pSensorRemoteObject, props);
		_serviceRefs.push_back(pServiceRef);
	}

	void createSensors(Peripheral::Ptr pPeripheral, const std::string& baseKey)
	{
		SensorTagSensor::Params params;
		params.version = _pPrefs->configuration()->getInt(baseKey + ".version", 2);

		// humidity
		params.serviceUUID = "f000aa20-0451-4000-b000-000000000000";
		params.controlUUID = "f000aa22-0451-4000-b000-000000000000";
		params.dataUUID    = "f000aa21-0451-4000-b000-000000000000";
		params.physicalQuantity = "humidity";
		params.physicalUnit = "%RH";
		params.pollInterval = _pPrefs->configuration()->getInt(baseKey + ".humidity.pollInterval", 10000);

		try
		{
			createSensor(pPeripheral, params);
		}
		catch (Poco::Exception& exc)
		{
			_pContext->logger().error(Poco::format("Cannot create SensorTag %s Sensor: %s", params.physicalQuantity, exc.displayText())); 
		}

		// ambient temperature
		params.serviceUUID = "f000aa00-0451-4000-b000-000000000000";
		params.controlUUID = "f000aa02-0451-4000-b000-000000000000";
		params.dataUUID    = "f000aa01-0451-4000-b000-000000000000";
		params.physicalQuantity = "ambientTemperature";
		params.physicalUnit = IoT::Devices::Sensor::PHYSICAL_UNIT_DEGREES_CELSIUS;
		params.pollInterval = _pPrefs->configuration()->getInt(baseKey + ".ambientTemperature.pollInterval", 10000);
		
		try
		{
			createSensor(pPeripheral, params);
		}
		catch (Poco::Exception& exc)
		{
			_pContext->logger().error(Poco::format("Cannot create SensorTag %s Sensor: %s", params.physicalQuantity, exc.displayText())); 
		}

		// object temperature
		params.serviceUUID = "f000aa00-0451-4000-b000-000000000000";
		params.controlUUID = "f000aa02-0451-4000-b000-000000000000";
		params.dataUUID    = "f000aa01-0451-4000-b000-000000000000";
		params.physicalQuantity = "objectTemperature";
		params.physicalUnit = IoT::Devices::Sensor::PHYSICAL_UNIT_DEGREES_CELSIUS;
		params.pollInterval = _pPrefs->configuration()->getInt(baseKey + ".objectTemperature.pollInterval", 10000);
		
		try
		{
			createSensor(pPeripheral, params);
		}
		catch (Poco::Exception& exc)
		{
			_pContext->logger().error(Poco::format("Cannot create SensorTag %s Sensor: %s", params.physicalQuantity, exc.displayText())); 
		}

		// illuminance
		params.serviceUUID = "f000aa70-0451-4000-b000-000000000000";
		params.controlUUID = "f000aa72-0451-4000-b000-000000000000";
		params.dataUUID    = "f000aa71-0451-4000-b000-000000000000";
		params.physicalQuantity = "illuminance";
		params.physicalUnit = IoT::Devices::Sensor::PHYSICAL_UNIT_LUX;
		params.pollInterval = _pPrefs->configuration()->getInt(baseKey + ".illuminance.pollInterval", 10000);
		
		try
		{
			if (params.version == 2)
			{
				createSensor(pPeripheral, params);
			}			
		}
		catch (Poco::Exception& exc)
		{
			_pContext->logger().error(Poco::format("Cannot create SensorTag %s Sensor: %s", params.physicalQuantity, exc.displayText())); 
		}

		// air pressure
		params.serviceUUID = "f000aa40-0451-4000-b000-000000000000";
		params.controlUUID = "f000aa42-0451-4000-b000-000000000000";
		params.dataUUID    = "f000aa41-0451-4000-b000-000000000000";
		params.physicalQuantity = "airPressure";
		params.physicalUnit = "hPa";
		params.pollInterval = _pPrefs->configuration()->getInt(baseKey + ".airPressure.pollInterval", 10000);
		
		try
		{
			createSensor(pPeripheral, params);
		}
		catch (Poco::Exception& exc)
		{
			_pContext->logger().error(Poco::format("Cannot create SensorTag %s Sensor: %s", params.physicalQuantity, exc.displayText())); 
		}
	}

	void start(BundleContext::Ptr pContext)
	{
		_pContext = pContext;
		_pPrefs = ServiceFinder::find<PreferencesService>(pContext);
		_pTimer = new Poco::Util::Timer;
	
		Poco::Util::AbstractConfiguration::Keys keys;
		std::string helperPath = _pPrefs->configuration()->getString("sensortag.bluez.helper");
		_pPrefs->configuration()->keys("sensortag.sensors", keys);
		for (std::vector<std::string>::const_iterator it = keys.begin(); it != keys.end(); ++it)
		{
			std::string baseKey = "sensortag.sensors.";
			baseKey += *it;

			std::string address = _pPrefs->configuration()->getString(baseKey + ".address");
		
			try
			{	
				GATTClient::Ptr pGATTClient = new BlueZGATTClient(helperPath);
				Peripheral::Ptr pPeripheral = new PeripheralImpl(address, pGATTClient);
				pPeripheral->connected += Poco::delegate(this, &BundleActivator::onConnected);
				pPeripheral->disconnected += Poco::delegate(this, &BundleActivator::onDisconnected);	

				PeripheralInfo info;
				info.pPeripheral = pPeripheral;
				info.baseKey = baseKey;
				info.reconnectDelay = MIN_RECONNECT_DELAY;
				info.haveSensors = false;
				_peripherals.push_back(info);		

				pPeripheral->connect(GATTClient::GATT_CONNECT_NOWAIT);
			}
			catch (Poco::Exception& exc)
			{
				_pContext->logger().error(Poco::format("Cannot create peripheral object for device %s: %s", address, exc.displayText()));
			}
		}
	}
		
	void stop(BundleContext::Ptr pContext)
	{
		_pTimer->cancel(true);
		_pTimer = 0;
		for (std::vector<ServiceRef::Ptr>::iterator it = _serviceRefs.begin(); it != _serviceRefs.end(); ++it)
		{
			_pContext->registry().unregisterService(*it);
		}
		_serviceRefs.clear();
		_peripherals.clear();

		_pPrefs = 0;
		_pContext = 0;
	}
	
protected:
	class ReconnectTask: public Poco::Util::TimerTask
	{
	public:
		ReconnectTask(BundleActivator& activator, Peripheral::Ptr pPeripheral):
			_bundleActivator(activator),
			_pPeripheral(pPeripheral)
		{
		}

		void run()
		{
			_bundleActivator.connect(_pPeripheral);
		}

	private:
		BundleActivator& _bundleActivator;
		Peripheral::Ptr _pPeripheral;
	};

	void onConnected(const void* pSender)
	{
		for (std::vector<PeripheralInfo>::iterator it = _peripherals.begin(); it != _peripherals.end(); ++it)
		{
			if (it->pPeripheral.get() == pSender)
			{
				_pContext->logger().information(Poco::format("Peripheral connected: %s", it->pPeripheral->address()));
				try
				{
					if (!it->haveSensors)
					{
						createSensors(it->pPeripheral, it->baseKey);
						it->haveSensors = true;
					}
					it->reconnectDelay = MIN_RECONNECT_DELAY;
				}
				catch (Poco::Exception& exc)
				{
					_pContext->logger().error(Poco::format("Failed to create sensors for device %s: %s", it->pPeripheral->address(), exc.displayText()));
				}
				break;
			}
		}
	}

	void onDisconnected(const void* pSender)
	{
		for (std::vector<PeripheralInfo>::iterator it = _peripherals.begin(); it != _peripherals.end(); ++it)
		{
			if (it->pPeripheral.get() == pSender)
			{
				_pContext->logger().information(Poco::format("Peripheral disconnected: %s", it->pPeripheral->address()));

				it->reconnectDelay *= 2;
				if (it->reconnectDelay > MAX_RECONNECT_DELAY)
					it->reconnectDelay = MAX_RECONNECT_DELAY;
				Poco::Clock clock;
				clock += static_cast<Poco::Clock::ClockDiff>(it->reconnectDelay)*1000;
				_pTimer->schedule(new ReconnectTask(*this, it->pPeripheral), clock);
				break;
			}
		}
	}

	void connect(Peripheral::Ptr pPeripheral)
	{
		try
		{
			pPeripheral->connect(GATTClient::GATT_CONNECT_NOWAIT);
		}
		catch (Poco::Exception& exc)
		{
			_pContext->logger().error(Poco::format("Cannot reconnect to device %s: %s", pPeripheral->address(), exc.displayText()));
		}
	}
	
	enum 
	{
		MIN_RECONNECT_DELAY = 1000,
		MAX_RECONNECT_DELAY = 30000
	};

private:
	struct PeripheralInfo
	{
		Peripheral::Ptr pPeripheral;
		std::string baseKey;
		unsigned reconnectDelay;
		bool haveSensors;
	};

	BundleContext::Ptr _pContext;
	PreferencesService::Ptr _pPrefs;
	Poco::SharedPtr<Poco::Util::Timer> _pTimer;
	std::vector<PeripheralInfo> _peripherals;
	std::vector<ServiceRef::Ptr> _serviceRefs;
};


} } } // namespace IoT::BtLE::SensorTag


POCO_BEGIN_MANIFEST(Poco::OSP::BundleActivator)
	POCO_EXPORT_CLASS(IoT::BtLE::SensorTag::BundleActivator)
POCO_END_MANIFEST