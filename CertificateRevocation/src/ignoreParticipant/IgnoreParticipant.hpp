/*
 * (c) 2022 Copyright, Real-Time Innovations, Inc. (RTI) All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the software solely for use with RTI Connext DDS.  Licensee may
 * redistribute copies of the software provided that all such copies are
 * subject to this license. The software is provided "as is", with no warranty
 * of any type, including any warranty for fitness for any purpose. RTI is
 * under no obligation to maintain or support the software.  RTI shall not be
 * liable for any incidental or consequential damages arising out of the use or
 * inability to use the software.
 */

#ifndef IDS_H
#define IDS_H

#include <dds/dds.hpp>
#include <thread>
#include <atomic>

#include "../generated/IDS.hpp"
#include "../common/ConnextCommunicator.hpp"

/* IgnoreParticipant Class
* Class simulating the operation of an Intrusion Detection System (IDS). 
* When a "problematic participant" is identified, all subscribing participants
* are notified that the specified participant profile should be ignored 
* or ignored and banished.
*/
class IgnoreParticipant : private ConnextCommunicator {
public:
    // ---- Contructor ----
    IgnoreParticipant(const std::string& profile, bool banish);
    // Send the notification to kick the specified participant.
    void kick(const std::string& participant_name);

private:
    // Topic string that is set in the Constructor based on whether to ignore
    // a participant or to ignore and banish a participant.
    std::string topic_;
};

#endif
