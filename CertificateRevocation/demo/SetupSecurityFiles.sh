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

# This section is going to be from the ca directory. This will have to change when
# we get to the Permissions signing section.
cd ../config/cert/pmi/ca

# Get rid of temporary_files, if they exist
if [ -d temporary_files ] 
then
	rm -rf temporary_files
fi
mkdir -p temporary_files

# Initialize the OpenSSL CA Database
echo -e '=======\nInitialize the OpenSSL CA Database.'
if [ -d database ] 
then
	rm -rf database
fi
mkdir database
touch database/pmiIdentityCaIndex
echo 01 > database/pmiIdentityCaSerial

# Initialize the OpenSSL crl Database
if [ -d crl ]
then
	rm -rf crl
fi
mkdir crl
echo 1000 > crl/crlNumber

# Limit Access of the CA's Private Key
echo -e '=======\nLimit Access of the CAs Private Key.'
if [ -d private ]
then
	rm -rf private
fi
mkdir private
chmod 700 private

# Generate new Identity CA
echo -e '=======\nGenerate new Identity CA.'
openssl req -verbose -nodes -x509 -days 7300 -text -sha256 -newkey ec -pkeyopt ec_paramgen_curve:prime256v1 -keyout private/pmiIdentityCaKey.pem -out pmiIdentityCaCert.pem -config pmiIdentityCa.cnf

# Generate what should be a crl without entries
openssl ca -config pmiIdentityCa.cnf -gencrl -out crl/pmiIdentityCa.crl

# Generate new Permissions CA
echo -e '=======\nGenerate new Permissions CA.'
openssl req -verbose -nodes -x509 -days 7300 -text -sha256 -newkey ec -pkeyopt ec_paramgen_curve:prime256v1 -keyout private/pmiPermissionsCaKey.pem -out pmiPermissionsCaCert.pem -config pmiPermissionsCa.cnf

# Go back to the demo directory
cd ../../../../demo

# Shape Demo A
./keygen.sh ShapesDemoA
# Shape Demo B
./keygen.sh ShapesDemoB
# Shape Publisher A
./keygen.sh ShapePublisherA
# Shape Publisher B
./keygen.sh ShapePublisherB
# Shape Publisher C
./keygen.sh ShapePublisherC
# IDS
./keygen.sh IDS

# For signing all of our xml files we will operate out of the config directory
cd ../config

mkdir -p xml/security/signed

# Sign the Governance Document with the new Permissions CA
echo -e '=======\nSign the Governance Document with the new Permissions CA.'
openssl smime -sign -in xml/security/pmiGovernance.xml -text -out xml/security/signed/pmiSigned_pmiGovernance.p7s -signer cert/pmi/ca/pmiPermissionsCaCert.pem -inkey cert/pmi/ca/private/pmiPermissionsCaKey.pem

# Sign the Permissions files with the Permissions CA
signFile() {
if [ -f xml/security/$1.xml ]
then
	echo =======
	echo Sign Permission File for $1.
	openssl smime -sign -in xml/security/$1.xml -text -out xml/security/signed/signed_$1.p7s -signer cert/pmi/ca/pmiPermissionsCaCert.pem -inkey cert/pmi/ca/private/pmiPermissionsCaKey.pem
else
    echo -e '********\n********'
	echo Configuration file $1.xml not found.
	echo '********\n********'
fi
}
# Shape Demo A
signFile pmiShapesDemoA
# Shape Demo B
signFile pmiShapesDemoB
# Shape Publisher A
signFile pmiShapePublisherA
# Shape Publisher B
signFile pmiShapePublisherB
# Shape Publisher C
signFile pmiShapePublisherC
# IDS
signFile pmiIDS
