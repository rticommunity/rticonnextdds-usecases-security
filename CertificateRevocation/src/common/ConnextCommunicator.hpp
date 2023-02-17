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

#ifndef CONNEXT_COMMUNICATOR_H
#define CONNEXT_COMMUNICATOR_H

// DDS Communicator
// Base behavior and initialization for the Connext applications in this C+C. Given
// that they all use the same IDL and QOS file, this simplifies initialization
// to the point where it can be mostly automated. The only difference between
// applications being which topics they publish or subscribe to and their
// behavior as a result of available data.

#include <dds/dds.hpp>

// determines which "type" of participant is being created
enum class ParticipantType { reader, writer, both };


// ConnextCommunicator class which helps automate the creation of DomainParticipants
// Constructor requires the domain id for the participant, QoS profile,
// and ParticipantType enum value determining whether the participant 
// should have readers, writers, or both.
class ConnextCommunicator {
public:
    // --- Constructor ---
    ConnextCommunicator(
            const int domain_id,
            const std::string& profile,
            ParticipantType type);

protected:
    dds::domain::DomainParticipant& domain_participant();

    // DataReader Creation and Fetching Template
    template <typename T>
    dds::sub::DataReader<T> data_reader(
            const std::string& topic_string,
            const std::string& profile)
    {
        if (!read_capable_participant_) {
            return dds::core::null;
        }
        auto it = readers_.find(topic_string);
        if (it != readers_.end()) {
            return readers_.at(topic_string).get<T>();
        }
        else {
            auto topic = dds::topic::find<dds::topic::Topic<T>>(
                    participant_,
                    topic_string);
            if (topic.is_nil()) {
                topic = dds::topic::Topic<T>(participant_, topic_string);
            }

            dds::sub::DataReader<T> reader(
                    subscriber_,
                    topic,
                    qos_->datareader_qos(profile));
            readers_.insert({topic_string, reader});
            return reader;
        }
    }

    // DataWriter Creation and Fetching Template
    template <typename T>
    dds::pub::DataWriter<T> data_writer(
            const std::string& topic_string,
            const std::string& profile)
    {
        if (!write_capable_participant_) {
            return dds::core::null;
        }
        auto it = writers_.find(topic_string);
        if (it != writers_.end()) {
            return writers_.at(topic_string).get<T>();
        }
        else {
            auto topic = dds::topic::find<dds::topic::Topic<T>>(
                    participant_,
                    topic_string);
            if (topic == dds::core::null) {
                topic = dds::topic::Topic<T>(participant_, topic_string);
            }

            dds::pub::DataWriter<T> writer(
                    publisher_,
                    topic,
                    qos_.datawriter_qos(profile));
            writers_.insert({topic_string, writer});
            return writer;
        }
    }

private:
    // --- Private Members ---
    dds::core::QosProvider qos_ = dds::core::null;
    dds::domain::DomainParticipant participant_ = dds::core::null;
    dds::sub::Subscriber subscriber_ = dds::core::null;
    dds::pub::Publisher publisher_ = dds::core::null;

    // DataReaders
    std::map< std::string, dds::sub::AnyDataReader> readers_;
    // DataWriters
    std::map< std::string, dds::pub::AnyDataWriter> writers_;

    bool read_capable_participant_;
    bool write_capable_participant_;
};

#endif
