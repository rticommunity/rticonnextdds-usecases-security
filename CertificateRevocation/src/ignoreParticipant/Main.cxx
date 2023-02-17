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

#include <thread>

#include "IgnoreParticipant.hpp"

// ----------------------------------------------------------------------------
// PrintHelp:
// Function that prints out the help if somebody runs the application with
// --help
//
void print_help()
{
    std::cout << "Valid options are: " << std::endl;
    std::cout << "    --participant [string]" << std::endl
              << "       Default is ShapesDemoAParticipant." << std::endl
              << "    --ignore" << std::endl
              << "       Just ignore the participant. Default is to ignore "
              << "and banish." << std::endl;
}

int main(int argc, char *argv[])
{
    std::string problem_participant = "ShapesDemoAParticipant";
    bool banish = true;

    for (int i = 0; i < argc; i++) {
        // Look for a Domain ID tag
        if (0 == strcmp(argv[i], "--participant")) {
            ++i;
            if (i == argc) {
                std::cout << "Bad parameter: Did not pass a participant"
                          << std::endl;
                return -1;
            }
            problem_participant = argv[i];
        } else if (0 == strcmp(argv[i], "--ignore")) {
            banish = false;
        } else if (0 == strcmp(argv[i], "--help")) {
            print_help();
            return 0;
        } else if (i > 0) {
            // If we have a parameter that is not the first one, and is not
            // recognized, return an error.
            std::cout << "Bad parameter: " << argv[i] << std::endl;
            print_help();
            return -1;
        }
    }

    // To turn on additional logging, include <rti/config/Logger.hpp> and
    // uncomment the following line:
    // rti::config::Logger::instance().verbosity(rti::config::Verbosity::STATUS_ALL);

    try {
        auto publisher = IgnoreParticipant(IDS::QOS_IGNORE_PARTICIPANT, banish);

        publisher.kick(problem_participant);
    } catch (const std::exception& ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in execute(): " << ex.what() << std::endl;
        return -1;
    }

    dds::domain::DomainParticipant::finalize_participant_factory();

    return 0;
}
