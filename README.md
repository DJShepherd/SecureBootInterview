# Introduction
* Offer water/bathroom break
* Introduce self
* Ask about themselves

## General Questions

### Game Consoles

Q: What are the main objectives of device security on a device such as a game
console?

A:
* Protect IP
* Prevent piracy
* Prevent cheating in online games
* Community content assurance

Q: What are common things hackers do to cheat in video games?

A:
* DDoS via network flooding or remote system crashing of opponents
* Network manipulation via forcing host console, standbying/lag switching, etc
* Stat modification: health, speed, ammo, currency, etc
* Content/map/gametype modifications

### Mobile Phones

TODO...

### IoT Devices

TODO...

### Misc Device Security

Q: How would you prioritize a code exec vulnerability in the different parts of
a device? Kernel, system process, bootrom, user application, hypervisor

A: In order of severity:
* Bootrom
* Hypervisor
* Kernel
* System process
* User application

Q: What about different types of vulnerabilities in different parts of a
device? Example: Code exec in a service process vs null deref in kernel?

A: Providing the null deref in the kernel is only an assertion/check error, the
code exec in the service process would have more impact on the system than just
a panic in the kernel.

Q: What attack surfaces would you look at when trying to pene test a device and
break into it?

A: Surfaces can vary, dependent on their experience, but these are some of my
ideas
* Peripherals
* Storage
* Glitching components
* Network
* Web browser/apps
* User application, application files, etc

Q: What parts of the system should you have the most focus on when hardening
security on a device?

A:
* Secure boot and chain of trust
* Hypervisor and kernel calls, validating parameters
* System process and IPC calls, validating parameters
* Hardware and database interfaces exposed to the public, network, peripherals, file system, etc
* User applications, application files, communication protocols, etc

Q: If you are familiar with glitching, please describe the general concept of
glitching.

Q: What are the basic pro's and con's of asymmetric and symmetric cryptography?

A:
* Asymmetric provides strong encryption such that only one party can decrypt the payload because half of the key is never released, but it requires more CPU power/time for decryption
* Symmetric allows for high performance encryption, but anyone can encrypt and decrypt data if they have the key thus allowing for man in the middle attacks and has higher potential for the key to be computed/brute forced

Q: What is the most common key exchange algorithm to have 2 parties securely
agree on a symmetric key?

A: Diffe-hellman exchange

Q: Provide a simple alternative to diffe-hellman using asymmetric cryptography
to agree on a symmetric key.

A: Not looking for anything in particular.  My most simple idea is:
* A generates a random symmetric key
* B sends A their public key
* A encrypts the symmetric key with B's public key and sends it back
* B decrypts the symmetric key with their private key
* Both A and B have the shared symmetric key

Q: Why use diffe-hellman over RSA to establish secure communication between 2
parties?


A: Diffe-hellman is more performant

## Secure Boot Test

Q: This is an intentionally insecure implementation of a "secure bootrom" with
various flaws.  Find as many flaws as you can and explain their impact or a
theoretical attack. 

A: This will help gauge their knowledge around general device security from how
many flaws/attacks they can see and how well they understand the implications
of such a flaw.
* Since the EMMC is writable, payload can be manipulated
* Header values are not secure
* MD5 is not secure
* Key appears to be plaintext, no derivation
* ECB is not very secure
* Downgrade possible because assertion only on debug
* Memcmp is an insecure way of comparing memory for crypto algorithms
* Subject to stack overflow with the bootloader header size
* Even though there are random delays, the 2nd EMMC read can be sensed and thus, the RSA signature check can be glitched to succeed
* Even though entry offset is checked to be inside IRAM, offset can be computed to be anywhere in IRAM, executing outside of the bootloader
* Entry address validation is converted to uint32_t.  Fine on 32 bit platforms, but on 64, it will only compare the lower 32 bits
* Entry address is "validated" after decryption. Value validation should be done as early as possible.
* No memory initialization/clearing before jumping into the bootloader

Q: What major fixes would you apply to secure the boot?

A:
* Secure the header values by hashing the header and using the resulting hash as the signature validation
* Validate all parameters as soon as possible
* Use stronger hash and encryption algorithms
* Better fault detection/mitigation's