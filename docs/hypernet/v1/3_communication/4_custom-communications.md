The custom communication
----

### What is Custom Communication?

A customized notification provides the flexibility to fully manage the transmission format and packet handling. This enables the incorporation of classic networking principles into this communication method.

### Goal?
The goal of this communication is to empower developers with complete control over the protocol, packets, targeting, and how they manage their communications.

### Custom Communication & Hypernet Concepts
While communication and its transmission are entirely customizable based on the user's preferences, the matrix system, channels, or providers can still exist if the user appreciates Hypernet's way of organizing networks and sessions. Alternatively, customized communication can be used without channels or matrices, directly in a bare-metal configuration.


### Autopsy of a custom communication

<span style="color:#FF3156"> **c:**</span>
{
<span style="color:#F64FFA">**[buffer]**</span>}

- **Communication Identifier (<span style="color:#FF3156"> **c:**</span> ):** This identifier serves as a straightforward way to differentiate between types of communication. In simple terms, if it's "n," it indicates a notification; if it's "t," it signifies a transmission, and if it's "c," it denotes a custom communication.

- **Buffer (<span style="color:#F64FFA">[buffer]**</span>): The buffer in Hypernet is composed of raw data prepared for distribution.

