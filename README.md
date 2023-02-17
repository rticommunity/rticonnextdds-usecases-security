# RTI Connext DDS Use Cases for Security

## Certificate Revocation

Demonstrates how to banish and ignore DomainParticipants in an active system
without restarting them.

## Cloning Repository

To clone the repository you will need to run git clone as follows to download
both the repository and its submodule dependencies:

```sh
git clone --recurse-submodule https://github.com/rticommunity/rticonnextdds-usecases-security.git
```

If you forget to clone the repository with ``--recurse-submodule``, simply run
the following command to pull all the dependencies:

```sh
git submodule update --init --recursive
```