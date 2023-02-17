# Certificate Revocation Demo

This demo shows how certificate revocation works in RTI Connext Secure 7.0.0.

There are 2 main applications in this case + code, the ShapePublishers and the IgnoreParticipant.

The ShapePublishers use the same type as RTI Shapes Demo, and publish 3 different shapes.
These are subscribed to in two separate subscriber windows of RTI Shapes Demo, ShapeDemoA and ShapeDemoB.

This is supposed to represent your overall RTI Connext Secure system, with multiple participants, publishers, and subscribers.

The example uses the same Patient Monitoring Innovations (PMI) as the *RTI Connext Secure Getting Started Guide*.

The IgnoreParticipant application is supposed to simulate an intrusion detection system (IDS)
and uses a simple IDS datatype found in `config/idl/` to notify the participants of a problematic
participant that needs to be ignored or ignored and banished.

The ConnextCommunicator is a helper class in `source/common` that helps simplify
initializing the multiple entities in both applications.

## System Requirements

* This has been fully tested on Windows 10.
* RTI Connext Secure 7.0.0
* cmake installed on your machine.
* In Windows a bash shell, like Git Bash.

## Build Instructions

Start by cloning the repo onto your computer. It is assumed that you have
cloned it into the rti_workspace directory created during the installation of
RTI Connext. If you have done so, your path should look like
```rti_workspace/rticonnextdds-usecases-security/CertificateRevocation```.

It is also recommended that you use the Connext DDS Professional Terminal. It
can be found in the Utilities tab of the RTI Launcher. Specify the working
directory as ```<path>/rti_workspace/rticonnextdds-usecases-security/CertificateRevocation```
and click Run.

When you are in one of your new terminal windows perform the following step:
1. Run ```build.sh```

This should complete the build.

## Certificate Creation Instructions

To generate all of your certificates you need to run the certificate creation
script.

1. Run ```SetupSecurityFiles.sh```.

## Running the Demo
All of this needs to be done from the ```demo``` directory. When executing
these programs for the first time you may need to give firewall permission.
Please do so.

### Shapes Demo

1. Launch two instances of RTI Shapes Demo by typing `rtishapesdemo` in a terminal window.
   (If this does not work, ensure your PATH is set correctly.)
2. In the Shapes Demo, goto Controls»Configuration...
3. Click Stop.
4. Deselect Enable Distributed Logger.
5. In the Choose the profile pulldown, select pmiLibrary::ShapeDemoA.
   (If you don't see this, you probably didn't run the shapes demo from the
   build directory after building the applications.)
6. Click Start.
7. Repeat these steps for the second demo window, but choose pmiLibrary::ShapeDemoB.
8. Subscribe to all shapes in both demo windows.

### Shape Publishers

1. In the terminal, execute the RunAllPublishers script, either
   ```RunAllPublishers.bat``` or ```RunAllPublishers.sh```

This launches 3 instances of ShapePublisher, which publish data.

At this point you should see 3 shapes in each of the Shapes Demos.

### Ignore

1. Run ```IgnoreParticipant.exe --participant ShapeDemoAParticipant```

This simulates an intrusion detection system (IDS) and notifies all 
subscribing participants of the problematic participant (in this case ShapeDemoAParticipant),
and that it should be ignored and banished.

If you want to only ignore the participant, add the `--ignore` command-line argument.

The main difference is that ignoring only prevents the local DomainParticipant
from processing traffic from the remote DomainParticipant. Banishing prevents
already ignored DomainParticipants from processing traffic from the local
DomainParticipant by regenerating and redistributing keys, removing the banished
participant from the system.

Once you do this, you should see all of the publishers notify you that they
ignored or banished ShapeDemoAParticipant and that one of the Shapes Demos
is not updating any more.

Try restarting the demo by going into Controls»Configuration... and clicking
Stop and Start. See what happens in the Publisher terminals.

## Conclusions

This Case + Code example demonstrates how certificate revocation can be used to
ignore and banish participants in RTI Connext Secure 7.0.0.

The code that is doing the heavy lifting here can be found in
```src/ShapePublisher/ShapePublisher.cxx```, primarily in the two Read
Conditions.

You can see the code in the read conditions performing certificate revocation
ignoring and banishing the problematic participants, and preventing the
banished participants from rejoining the system.

You can now use this as a reference implementation for your own RTI Connext Secure
system, with the new functionality added in the 7.0.0 Release.
