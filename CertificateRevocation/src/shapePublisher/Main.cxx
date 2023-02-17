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

#include "ShapePublisher.hpp"

// ----------------------------------------------------------------------------
// print_help:
// Function that prints out the help if somebody runs the application with
// --help
//
void print_help()
{
    std::cout << "Valid options are: " << std::endl;
    std::cout << "    --shape [string]"
              << " Valid values Square, Circle, Triangle. Topic this app "
              << std::endl
              << "                               "
              << "publishes to. Default is Square." << std::endl;
}

int main(int argc, char *argv[])
{
    std::string shape_topic = "Square";

    for (int i = 0; i < argc; i++) {
        // Look for a Domain ID tag
        if (0 == strcmp(argv[i], "--shape")) {
            ++i;
            if (i == argc) {
                std::cout << "Bad parameter: Did not pass a shape" << std::endl;
                return -1;
            }
            shape_topic = argv[i];

            // Only currently have 5 types of controller
            if (shape_topic != "Square" && shape_topic != "Circle"
                && shape_topic != "Triangle") {
                std::cout << "Bad parameter: valid shapes are Square, Circle, "
                             "and Triangle"
                          << std::endl;
                return -1;
            }
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
        std::string profile;
        ShapeToPublish shape;

        if (shape_topic == "Circle") {
            profile = "pmiLibrary::ShapePublisherA";
            shape = ShapeToPublish::circle;
        } else if (shape_topic == "Triangle") {
            profile = "pmiLibrary::ShapePublisherC";
            shape = ShapeToPublish::triangle;
        } else {
            profile = "pmiLibrary::ShapePublisherB";
            shape = ShapeToPublish::square;
        }

        ShapePublisher shape_publisher(profile);

        std::thread execute_thread(
                &ShapePublisher::publish_shape,
                &shape_publisher,
                shape,
                profile);

        std::cout << shape_topic << " Publishing. Press Enter to quit." << std::endl;

        std::thread list_thread(
                &ShapePublisher::participant_lister,
                &shape_publisher);

        std::cin.get();

		std::cout << "Shutting down..." << std::endl;

        shape_publisher.stop_publisher();
        execute_thread.join();
        list_thread.join();
        
        std::cout << shape_topic << " Generator Shut Down." << std::endl;
    } catch (const std::exception& ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in execute(): " << ex.what() << std::endl;
        return -1;
    }

    dds::domain::DomainParticipant::finalize_participant_factory();

    return 0;
}
