/*
 * This file is part of a CODESKIN library that is being made available
 * as open source under the GNU Lesser General Public License.
 *
 * Copyright 2005-2017 by CodeSkin LLC, www.codeskin.com.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * ERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CAN_ADAPTER_H_
#define CAN_ADAPTER_H_

#ifdef SCONS_TARGET_WIN
#include <stdint.h>
#endif

#include <boost/shared_ptr.hpp>

#include "CanPort.h"

class CanAdapter;
typedef boost::shared_ptr<CanAdapter> SharedCanAdapter;

class CanAdapterWrapper;
typedef boost::shared_ptr<CanAdapterWrapper> SharedCanAdapterWrapper;

class CanAdapter : public CanPort {

public:
	enum CanAdapterType {
		None = -1,
		Echo = 0,
		NiCan = 1,
		LawicelCan = 2,
		PeakCan = 3,
		KvaserCan = 4,
		VectorCan = 5,
		EmsWuenscheCan = 6,
		SLCan = 7,
		SocketCan = 8,
		NiXnetCan = 9
	};

	enum CanAdapterState {
		Unknown = -1,
		Closed = 0,
		ErrorActive = 1,
		ErrorPassive = 2,
		BusOff = 3,
	};

	static SharedCanAdapter getInstance(CanAdapterType aType, std::string aChannelName);
	static bool getFirstChannelName(CanAdapter::CanAdapterType aType, std::string &aName);
	static bool getNextChannelName(CanAdapter::CanAdapterType aType, std::string &aName);

	/* This is mostly a virtual class */
	CanAdapter() {};
	virtual ~CanAdapter() {};

	/**
	 * Sets generic parameter.
	 */
	virtual bool setParameter(std::string aKey, std::string aValue) = 0;

	/**
	 * Sets baud-rate.
	 * @param baudrate baudrate
	 * @return true if successful
	 */
	virtual bool setBaudRate(uint32_t aBaudrate) = 0;

	/**
	 * Returns the number of acceptance filters available.
	 * @return Number of available filters
	 */
	virtual int getNumberOfFilters() = 0;

	/**
	 * Sets acceptance filter.
	 * Different filters can be configured, depending on implementation.
	 * A "1" in the mask means that the corresponding bit in code is relevant.
	 *
	 * @param fid filter id
	 * @param code acceptance code
	 * @param mask acceptance mask
	 * @param isExt true for extended message filter
	 * @return true if successful
	 */
	virtual bool setAcceptanceFilter(int aFid, uint32_t aCode, uint32_t aMask, bool IsExt) = 0;

	/**
	 * Open interface.
	 * Must be called after all interface parameters are configured.
	 */
	virtual bool open() = 0;

	/**
	 * Go bus on.
	 * Must be called before communication can be started
	 * @return true if successful
	 */
	virtual bool goBusOn() = 0;

	/**
	 * Go bus off.
	 * @return true if successful
	 */
	virtual bool goBusOff() = 0;

	/**
	 * Send message.
	 * Queues CAN message into transmit buffer. A transaction id is associated with
	 * the pending transmission, and can be used in conjunction with getSendAcknMessage()
	 * to subsequently confirm a successful transmission.
	 *
	 * @param aMsg CAN message
	 * @param aTransactionId id associated with transmission (set to nullptr if not used)
	 * @return true if successful
	 */
	virtual bool sendMessage(SharedCanMessage aMsg, uint16_t *aTransactionId) = 0;

	/**
	 * Get number of messages in receive buffer.
	 * Received messages are retrieved by means of getReceivedMessage().
	 * @param number of messages in receive buffer
	 */
	virtual int numReceivedMessagesAvailable() = 0;

	/**
	 * Retrieve message from receive buffer.
	 *
	 * @param aMsg object to store received message
	 * @param aTimeoutMs time to wait for received message
	 * @return true when valid message is returned by function
	 */
	virtual bool getReceivedMessage(SharedCanMessage& aMsg, uint32_t aTimeoutMs) = 0;

	/**
	 * Get number of successfully sent messages stored in transmit acknowledge buffer.
	 * Messages transmitted are stored in the transmit acknowledge buffer and can
	 * be retrieved my means of getSendAcknMessage().
	 * @param number of transmit acknowledge buffer
	 */
	virtual int numSendAcknMessagesAvailable() = 0;

	/**
	 * Retrieve message from transmit acknowledge buffer.
	 * If a non-zero transaction id is specified, the function will block until
	 * the message corresponding to the transaction id as been transmitted, and
	 * stored in the transmit acknowledge buffer, or until the specified timeout
	 * expires.
	 *
	 * @param aMsg object to store transmitted message
	 * @param aTransactionId desired transaction id
	 * @param aTimeoutMs time to wait for received message
	 * @return true when valid message is returned by function
	 */
	virtual bool getSendAcknMessage(SharedCanMessage& aMsg, uint16_t aTransactionId, uint32_t aTimeoutMs) = 0;

	/**
	 * Close interface.
	 */
	virtual void close() = 0;

	/**
	 * Getter for adapter state
	 * @return state
	 */
	virtual enum CanAdapterState getState() = 0;

	/**
	 *  Getter for error code corresponding to last error that occurred.
	 *  Use getErrorDescription() to obtain textual description of error code.
	 *  @return error code
	 */
	virtual int getErrorCode() = 0;

	/**
	 * Convert numerical error code to error description.
	 *
	 * @param aErrorCode error code obtained from getErrorCoder()
	 * @param aErrorString textual error description
	 * @param aSecondaryCode secondary error code (if applicable)
	 */
	virtual void getErrorDescription(int aErrorCode, std::string &aErrorString, int32_t *aSecondaryCode) = 0;

	/**
	 *  Return CAN error counters.
	 *  @param aTxErrorCounter number of transmit errors
	 *  @param aRxErrorCounter number of receive errors
	 */
	virtual void getErrorCounters(int *aTxErrorCounter, int *aRxErrorCounter) = 0;

private:
	static std::map<int, SharedCanAdapterWrapper> mWrapperMap;
	static SharedCanAdapterWrapper getWrapper(CanAdapterType aType);
};

#endif /* CAN_ADAPTER_H_ */
