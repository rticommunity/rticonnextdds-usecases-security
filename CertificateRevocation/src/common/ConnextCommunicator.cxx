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

#include "ConnextCommunicator.hpp"

using namespace std;

ConnextCommunicator::ConnextCommunicator(
        const int domain_id,
        const std::string& profile,
        ParticipantType type)
{
    // Figure our Participant capability. This is used to validate calls for
    // Readers and Writers.
    read_capable_participant_ =
            (type == ParticipantType::reader || type == ParticipantType::both);
    write_capable_participant_ =
            (type == ParticipantType::writer || type == ParticipantType::both);

    // Set up the QOS Provider with default configuration
    qos_ = dds::core::QosProvider::Default();

    // Specify the Entity name for the participant
    auto qos_participant = qos_->participant_qos(profile);

    // Initialize the Participant
    participant_ = dds::domain::DomainParticipant(domain_id, qos_participant);

    // Initialize the Subscriber and Publisher
    if (read_capable_participant_) {
        subscriber_ = dds::sub::Subscriber(participant_, qos_.subscriber_qos());
    }
    if (write_capable_participant_) {
        publisher_ = dds::pub::Publisher(participant_, qos_.publisher_qos());
    }
}

dds::domain::DomainParticipant& ConnextCommunicator::domain_participant()
{
    return participant_;
}
