//
// FlagsDatapointEventDispatcher.h
//
// Library: IoT/Devices
// Package: Generated
// Module:  FlagsDatapointEventDispatcher
//
// This file has been generated.
// Warning: All changes to this will be lost when the file is re-generated.
//
// Copyright (c) 2014-2022, Applied Informatics Software Engineering GmbH.
// All rights reserved.
// 
// SPDX-License-Identifier: GPL-3.0-only
//


#ifndef IoT_Devices_FlagsDatapointEventDispatcher_INCLUDED
#define IoT_Devices_FlagsDatapointEventDispatcher_INCLUDED


#include "IoT/Devices/FlagsDatapointRemoteObject.h"
#include "Poco/RemotingNG/EventDispatcher.h"


namespace IoT {
namespace Devices {


class FlagsDatapointEventDispatcher: public Poco::RemotingNG::EventDispatcher
	/// The base class for datapoints holding a variable number of
	/// flag bits, internally stored as a std::vector<bool>.
	///
	/// An implementation should handle conversion between a
	/// flag bit and the corresponding symbolic name of
	/// a flag.
	///
	/// In addition to the methods defined in this interface,
	/// a FlagsDatapoint implementation should expose the following
	/// properties:
	///   - displayValue (string, optional): The current value of the datapoint,
	///     formatted as string for display purposes.
{
public:
	FlagsDatapointEventDispatcher(FlagsDatapointRemoteObject* pRemoteObject, const std::string& protocol);
		/// Creates a FlagsDatapointEventDispatcher.

	virtual ~FlagsDatapointEventDispatcher();
		/// Destroys the FlagsDatapointEventDispatcher.

	void event__invalidated(const void* pSender);

	void event__statusChanged(const void* pSender, const IoT::Devices::DeviceStatusChange& data);

	void event__validated(const void* pSender, const std::vector < bool >& data);

	void event__valueChanged(const void* pSender, const std::vector < bool >& data);

	void event__valueUpdated(const void* pSender, const std::vector < bool >& data);

	virtual const Poco::RemotingNG::Identifiable::TypeId& remoting__typeId() const;

private:
	void event__invalidatedImpl(const std::string& subscriberURI);

	void event__statusChangedImpl(const std::string& subscriberURI, const IoT::Devices::DeviceStatusChange& data);

	void event__validatedImpl(const std::string& subscriberURI, const std::vector < bool >& data);

	void event__valueChangedImpl(const std::string& subscriberURI, const std::vector < bool >& data);

	void event__valueUpdatedImpl(const std::string& subscriberURI, const std::vector < bool >& data);

	static const std::string DEFAULT_NS;
	FlagsDatapointRemoteObject* _pRemoteObject;
};


inline const Poco::RemotingNG::Identifiable::TypeId& FlagsDatapointEventDispatcher::remoting__typeId() const
{
	return IFlagsDatapoint::remoting__typeId();
}


} // namespace Devices
} // namespace IoT


#endif // IoT_Devices_FlagsDatapointEventDispatcher_INCLUDED

