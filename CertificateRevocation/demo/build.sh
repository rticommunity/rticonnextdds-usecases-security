# (c) 2023 Copyright, Real-Time Innovations, Inc. (RTI) All rights reserved.
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the Software solely in combination with RTI Connext DDS. Licensee
# may redistribute copies of the Software provided that all such copies are
# subject to this License. The Software is provided "as is", with no warranty
# of any type, including any warranty for fitness for any purpose. RTI is
# under no obligation to maintain or support the Software. RTI shall not be
# liable for any incidental or consequential damages arising out of the use or
# inability to use the Software. For purposes of clarity, nothing in this
# License prevents Licensee from using alternate versions of DDS, provided
# that Licensee may not combine or link such alternate versions of DDS with
# the Software.

#!/bin/sh

# Create a CMakeBuild directory and go into it
mkdir -p CMakeBuild

cd CMakeBuild

echo -e 'Looking for Visual Studio target libraries'
# run CMake to generate files and build if it works
if [ -d "${NDDSHOME}/lib/x64Win64VS2019" ]
then
	echo -e 'Found VS2019'
	cmake -DCONNEXTDDS_ARCH=x64Win64VS2019 ../..
elif [ -d "${NDDSHOME}/lib/x64Win64VS2017" ]
then
	echo -e 'Found VS2017'
	cmake -DCONNEXTDDS_ARCH=x64Win64VS2017 ../..
elif [ -d "${NDDSHOME}/lib/x64Win64VS2015" ]
then
	echo -e 'Found VS2015'
	cmake -DCONNEXTDDS_ARCH=x64Win64VS2015 ../..
else
	echo -e "Didn't find Visual Studio Target"
	cmake ../..
fi
	
cmake --build .
