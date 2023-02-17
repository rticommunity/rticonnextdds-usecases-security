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

#include "ShapePublisher.hpp"
#include <chrono>
#include <random>
#include <rti/domain/discovery.hpp>

ShapePublisher::ShapePublisher(const std::string& profile)
        : ConnextCommunicator(0, profile, ParticipantType::both)
{
    // The Reader needs a datareader QoS that is different from the participant
    // QoS specified in the constructor argument.
    data_reader<IDS::ProblemParticipant>(
            IDS::TOPIC_IGNORE_PARTICIPANT,
            IDS::QOS_IGNORE_PARTICIPANT);
    data_reader<IDS::ProblemParticipant>(
            IDS::TOPIC_BANISH_PARTICIPANT,
            IDS::QOS_IGNORE_PARTICIPANT);
    run_processes_ = true;

    auto&& common_data_state = dds::sub::status::DataState(
            dds::sub::status::SampleState::not_read(),
            dds::sub::status::ViewState::any(),
            dds::sub::status::InstanceState::alive());
    

    // Get the DataReader for ParticipantBuiltinTopicData
    // First get the builtin subscriber.
    dds::sub::Subscriber builtin_subscriber =
            dds::sub::builtin_subscriber(domain_participant());

    // Then get builtin subscriber's datareader for participants.
    std::vector<dds::sub::DataReader<dds::topic::ParticipantBuiltinTopicData>>
            participant_reader;
    dds::sub::find<
            dds::sub::DataReader<dds::topic::ParticipantBuiltinTopicData>>(
            builtin_subscriber,
            dds::topic::participant_topic_name(),
            std::back_inserter(participant_reader));
    auto discovery_reader = 
            dds::sub::DataReader<dds::topic::ParticipantBuiltinTopicData>(
                    participant_reader[0]);

    dds::sub::cond::ReadCondition read_condition_discovery(
            discovery_reader,
            dds::sub::status::DataState::new_instance(),
            [this, &discovery_reader]() {
                auto&& samples = discovery_reader.take();

                std::cout << "New Participant." << std::endl;

                for (const auto& sample : samples) {
                    if (!sample.info().valid()) {
                        continue;
                    }
                    if (sample.info().state().instance_state()
                        != dds::sub::status::InstanceState::alive()) {
                        continue;
                    }
                    auto handle = sample.info().instance_handle();
                    for (int i = 0; i < banished_participants_.size(); i++) {
                        // compare the first 6 bytes from the handle which contain the GUID
                        // with the array recording banished participant GUIDs
                        if (memcmp((char *) &handle,
                                   (char *) &banished_participants_[i],
                                   6) 
                            != 0) {
                            continue;
                        }
                        std::cout << "Caught trying to rejoin." << std::endl;
                        std::cout << "Subject Name: ";
                        std::cout << banished_subjects_[i] << std::endl;
                        dds::domain::ignore(domain_participant(), handle);
                        rti::domain::banish_ignored_participants(domain_participant());
                        std::cout << "Re-banished." << std::endl;
                    }
                }
            });
    waitset_.attach_condition(read_condition_discovery);

    // This handles when a command is sent in to ignore a participant. We are
    // using the common name of the participant to ignore initially, but a real
    // system would probably use the subject name instead, which will be printed
    // out when the ignore occurs. This is because the participant name can
    // easily be changed in code.
    auto ignore_reader = dds::sub::DataReader<IDS::ProblemParticipant>(
            data_reader<IDS::ProblemParticipant>(
                IDS::TOPIC_IGNORE_PARTICIPANT,
                IDS::QOS_IGNORE_PARTICIPANT));
    auto banish_reader = dds::sub::DataReader<IDS::ProblemParticipant>(
            data_reader<IDS::ProblemParticipant>(
                IDS::TOPIC_BANISH_PARTICIPANT,
                IDS::QOS_IGNORE_PARTICIPANT));

    dds::sub::cond::ReadCondition read_condition_ignore_participant(
            ignore_reader,
            common_data_state,
            [this, &ignore_reader]() {
                auto&& samples = ignore_reader.take();
                // The sample(s) contain the Participant Names to kick.
                for (auto&& sample : samples) {
                    problem_participant(sample, false);
                }
            });
    waitset_.attach_condition(read_condition_ignore_participant);

    dds::sub::cond::ReadCondition read_condition_banish_participant(
            banish_reader,
            common_data_state,
            [this, &banish_reader]() {
                auto&& samples = banish_reader.take();
                // The sample(s) contain the Participant Names to kick.
                for (auto&& sample : samples) {
                    problem_participant(sample, true);
                }
            });
    waitset_.attach_condition(read_condition_banish_participant);
}

void ShapePublisher::problem_participant(
        rti::sub::LoanedSample<IDS::ProblemParticipant> sample,
        bool banish)
{
    if (!sample.info().valid()) {
        return;
    }
    // Go through all remote participants
    for (auto discovered_participant :
         dds::domain::discovered_participants(domain_participant())) {
            // Pull the Participant name for the remote participant
            auto name = dds::domain::discovered_participant_data(
                    domain_participant(),
                    discovered_participant)
                    ->participant_name()
                    .name();
            if (!name.has_value()) {
                continue;
            }
            // Check to see if the Participant Name to kick matches
            if (name.value() != sample.data().participant()) {
                continue;
            }
            // This is storage for reconnect attempts. Instead of using the 
            // Participant Name (which can easily be changed), we are using the
            // subject name, which is unique and attached to a specific
            // Identity Cert and signed Permissions document.
            auto subject_name =
                    rti::domain::
                            discovered_participant_subject_name(
                                    domain_participant(),
                                    discovered_participant)
                                    .value();
            banished_participants_.push_back(discovered_participant);
            banished_subjects_.push_back(subject_name);
            std::cout << "IGNORED!!!" << std::endl;
            std::cout << "Participant Name: " << name.value()
                      << std::endl;
            std::cout << "Subject Name: " << subject_name
                      << std::endl;
            dds::domain::ignore(
                    domain_participant(),
                    discovered_participant);
            if (banish) {
                std::cout << "BANISHED!!!" << std::endl;
                rti::domain::banish_ignored_participants(domain_participant());
            }
        }
}

void ShapePublisher::stop_publisher()
{
    run_processes_ = false;
}

void ShapePublisher::participant_lister()
{
    bool named_participant_found = false;

    while (run_processes_) {
        named_participant_found = false;

        for (auto discovered_participant :
             dds::domain::discovered_participants(domain_participant())) {
            auto name = dds::domain::discovered_participant_data(
                                domain_participant(),
                                discovered_participant)
                                ->participant_name()
                                .name();
            if (name.has_value()) {
                std::cout << name.value() << std::endl;
                named_participant_found = true;
            }
        }

        if (!named_participant_found) {
            std::cout << "No Named Participants Found.\n";
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    }
}

void ShapePublisher::publish_shape(
        ShapeToPublish shape,
        const std::string& profile)
{
    dds::pub::DataWriter<ShapeType> writer = dds::core::null;

    switch (shape) {
    case ShapeToPublish::square:
        data_writer<ShapeType>(std::string("Square"), profile);
        writer = dds::pub::DataWriter<ShapeType>(
                data_writer<ShapeType>(std::string("Square"), profile));
        break;
    case ShapeToPublish::circle:
        data_writer<ShapeType>(std::string("Circle"), profile);
        writer = dds::pub::DataWriter<ShapeType>(
                data_writer<ShapeType>(std::string("Circle"), profile));
        break;
    case ShapeToPublish::triangle:
        data_writer<ShapeType>(std::string("Triangle"), profile);
        writer = dds::pub::DataWriter<ShapeType>(
                data_writer<ShapeType>(std::string("Triangle"), profile));
        break;
    }

    // There are a lot of magic numbers here. These have to do with how the
    // shape will be visualized in the Shapes Demo. Changing the size range or
    // speed is definitely possible, but going too big in size or too fast in
    // velocity could break the demo.
    ShapeType sample;
    sample.color(get_random_color());
    sample.shapesize(get_random_int(20, 40));
    float x_speed = get_random_float(-3.0, 3.0);
    float y_speed = get_random_float(-3.0, 3.0);
    const float min_limit = (float) ((float) sample.shapesize() / 2.0);
    const float x_max_limit =
            static_cast<float>(237.0 - (float) sample.shapesize() / 2.0);
    const float y_max_limit =
            static_cast<float>(267.0 - (float) sample.shapesize() / 2.0);
    float x = get_random_float(min_limit, x_max_limit);
    float y = get_random_float(min_limit, y_max_limit);

    while (run_processes_) {
        limit_check(min_limit, x_max_limit, x, x_speed);
        limit_check(min_limit, y_max_limit, y, y_speed);

        sample.x(static_cast<int32_t>(x));
        sample.y(static_cast<int32_t>(y));

        writer->write(sample);

        waitset_.dispatch(std::chrono::milliseconds(50));
    }

    writer->dispose_instance(writer->lookup_instance(sample));
}

void ShapePublisher::limit_check(
        const float min_limit,
        const float max_limit,
        float& val,
        float& speed)
{
    val = val + speed;
    if (val > max_limit) {
        val = 2 * max_limit - val;
        speed = -speed;
    } else if (val < min_limit) {
        val = 2 * min_limit - val;
        speed = -speed;
    }
}

float ShapePublisher::get_random_float(float min, float max)
{
    static bool needs_seed = true;
    static std::default_random_engine e;
    static std::uniform_real_distribution<> dis(min, max);

    if (needs_seed) {
        std::random_device rd;
        e.seed(rd());
        needs_seed = false;
    }

    return static_cast<float>(dis(e));
}

int ShapePublisher::get_random_int(int min, int max)
{
    static bool needs_seed = true;
    static std::mt19937 generator;
    static std::uniform_int_distribution<int> distribution(min, max);

    if (needs_seed) {
        std::random_device rd;
        generator.seed(rd());
        needs_seed = false;
    }

    return distribution(generator);
}

std::string ShapePublisher::get_random_color()
{
    static std::vector<std::string> in {
        "BLUE", "RED", "GREEN", "YELLOW", "ORANGE", "PURPLE", "CYAN", "MAGENTA"
    };
    srand(time(0));
    return in[rand() % in.size()];
}