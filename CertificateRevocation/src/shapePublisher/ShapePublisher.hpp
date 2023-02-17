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

#ifndef SHAPEPUBLISHER_H
#define SHAPEPUBLISHER_H

#include <dds/dds.hpp>
#include <thread>
#include <atomic>

#include "../generated/Shapes.hpp"
#include "../generated/IDS.hpp"
#include "../common/ConnextCommunicator.hpp"

// Used by the ShapePublisher Class to determine which shape to publish
enum class ShapeToPublish { square, circle, triangle };

// Publishes the specified shape. The size, color, initial position and
// velocity are determined randomly.
class ShapePublisher : private ConnextCommunicator {
public:
    // Constructor
    ShapePublisher(const std::string& profile);
    // Command used to stop all threads.
    void stop_publisher();
    // This function is meant to be called within a thread to provide
    // continuous updates to the shape topic.
    void publish_shape(ShapeToPublish shape, const std::string& profile);
    // This function is meant to be called within a thread to provide a
    // continuous update to std out of all discovered participants. These
    // names can be used to specify which participant to banish.
    void participant_lister();

private:
    // Tracking of subject names and participant handles that have been
    // explicitly banished, so that newly discovered participants can be
    // checked to see if they are trying to sneak back in.
    std::vector<std::string> banished_subjects_;
    std::vector<dds::core::InstanceHandle> banished_participants_;

    // Threading Variable.
    std::atomic<bool> run_processes_ = true;

    // Waitset Variable
    dds::core::cond::WaitSet waitset_;
    
    // Waitset Trigger Functions
    void problem_participant(rti::sub::LoanedSample<IDS::ProblemParticipant> sample, bool banish);

    // Private Utility Functions
 
    // Random floating point number between the two values
    float get_random_float(float min, float max);
    // Random integer between the two values
    int get_random_int(int min, int max);
    // Random color of the choices that the Shapes Demo understands
    std::string get_random_color();
    // When the shape hits the given limit, it bounces off
    void limit_check(
            const float min_limit,
            const float max_limit,
            float& val,
            float& speed);
};

#endif