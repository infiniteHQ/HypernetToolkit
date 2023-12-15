

Matrixes
=====

### What is a Matrix ?
The matrix is an environment, a context for all elements, actors, channels, and components within a network based on Hypernet. It enables the swift and easy establishment of a comprehensive framework, allowing everyone to align on functionality, security, and communication. The matrix provides all relevant information and serves as the foundation for every communication channel. It furnishes the targeting system (directing the flow of communication), as well as element addressing and channel assignment.

### Goal
The goal of a matrix is to provide a general framework and context for all elements, components, content, or actors within it. The role of the matrix is to organize, segment, open or close channels, and ensure the connectivity of components as well as the reliability of communications.

### Flexibility
The goal of a matrix (like all of Hypernet) is to be adaptable and programmable. A matrix can be configured to create a singular connection between two objects for file transfer, or it can be programmed to establish video game sessions, manage players, transmit audio and video streams, or publish home automation events to subscribers. In short, there are no limits to its use.

### High, Low Level
As you may have gathered, Hypernet offers a lot of freedom and possibilities for designing matrices. However, in the realm of computing, freedom does not necessarily equate to better. This is why we introduce a concept of high/low level, allowing everyone to understand each other even if they don't have the same version of the matrix.

The low level encompasses a set of constraints, rules, limits, and features defined by the elements possessing a copy of the desired matrix. Therefore, an element does not need to possess an instance of the matrix for it to function correctly from its perspective.

### Bootstrapping 
The concept of bootstrapping pertains to the initialization of the matrix and the integration of all associated elements. It is crucial to enable optimal service maintenance after an incident. Additionally, providing elements with straightforward access to matrix functions is important, all while ensuring the security and integrity of the system.

#### Portals, QR code, Codes
The "portal" is a method involves providing a portal in the form of a code containing most of the important information to join a Hypernet matrix. This code can includes various crucial details regarding the matrix:

*(Note: Like everything in Hypernet, the portal is a modular component and can be modified to adapt to needs.)*

- The type of matrix
- The address and identifier of the matrix
- The receiver capable of examining a bootstrap request
- General security information.

Here is an example of a matrix:

[po(::HSTD::P_MatrixBuffer), id(53995198555410256-555448101-4514999-575310157-501015550545210299524810156), ty(::HSTD::M_BaseMatrix), pb(), r(r1[protocol:tcpip, ip:127.0.0.122, port:1115])]

    "po" defines the type of portal (code) used.
    "id" defines the identifier of the matrix.
    "ty" defines the type of matrix.
    "r" defines the list of receivers (in this case, a machine reachable via TCP/IP at the address 127.0.0.122:1115).


### Matrix coherence

#### Matrix properties
#### Matrix parameters
#### Matrix contents
#### Casting


### Channel runtime

