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

cd ../config/cert/pmi/ca

# Generate Identity Certificates
keygen() {
if [ -f ../identities/pmi$1.cnf ]
then
	echo =======
	echo Generate Identity Certificate for $1
	mkdir ../identities/$1
	mkdir ../identities/$1/private
	# Create Private Key and Certificate Signing Request
	openssl req -verbose -nodes -new -newkey ec -pkeyopt ec_paramgen_curve:prime256v1 -config ../identities/pmi$1.cnf -keyout ../identities/$1/private/pmi$1Key.pem -out ../identities/$1/pmi$1.csr
	# Create Signed Certificate
	openssl ca -config pmiIdentityCa.cnf -batch -in ../identities/$1/pmi$1.csr -out ../identities/$1/pmi$1Cert.pem
	#openssl x509 -req -days 3650 -text -CAserial $certPath/ca/database/pmiIdentityCaSerial -CA $certPath/ca/pmiIdentityCaCert.pem -CAkey $Identity_CA_Key -in $certPath/identities/$1/pmi$1.csr -out $certPath/identities/$1/pmi$1Cert.pem
else
    echo -e '********\n********'
	echo Configuration file pmi$1.cnf not found.
	echo -e '********\n********'
fi
}

keygen $1