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

#include "IgnoreParticipant.hpp"

IgnoreParticipant::IgnoreParticipant(const std::string& profile, bool banish)
        : ConnextCommunicator(0, profile, ParticipantType::writer)
{
    // Choose Topic
    if (banish)
        topic_ = IDS::TOPIC_BANISH_PARTICIPANT;
    else
        topic_ = IDS::TOPIC_IGNORE_PARTICIPANT;
 
    // Initialize writer
    data_writer<IDS::ProblemParticipant>(
            topic_,
            IDS::QOS_IGNORE_PARTICIPANT);
}

void IgnoreParticipant::kick(const std::string& participant_name)
{
    std::cout << "Press ENTER to kick " << participant_name << "..."
              << std::endl;
    std::cin.get();

    IDS::ProblemParticipant sample(participant_name);

    auto writer = data_writer<IDS::ProblemParticipant>(
            topic_,
            IDS::QOS_IGNORE_PARTICIPANT);

    writer.write(sample);

    std::cout << "Kick message sent. Press ENTER to exit." << std::endl;
    std::cin.get();

    writer.wait_for_acknowledgments(dds::core::Duration(1));

}
