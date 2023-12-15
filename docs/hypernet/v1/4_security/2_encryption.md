Encryption
----
*Note: The specification of the Hypernet Security Protocol is still a work in progress; contents are subject to changes.*

### Why?
Encrypting data and transmissions is fundamental to ensure a secure context for the user, their data, and the services they utilize.

This encryption serves as a protective barrier against unauthorized access, maintaining the confidentiality and integrity of sensitive information. By implementing robust encryption measures, Hypernet establishes a secure environment that instills confidence in users, safeguarding their data and interactions with various services. The "why" behind encryption lies in creating a trustworthy and secure digital ecosystem, promoting user privacy and mitigating the risks associated with potential security threats.

### Encryption of Communications
There are several methods to establish encryption for communications, and Hypernet allows for the modularization and adaptation of protection mechanisms. One solution involves setting up a security framework through Channels. Once an element joins the channel, it retrieves the security rules pertaining to it. If these rules are incompatible with its operational modes, it can propose a handshake (via the notification ::handshake). The channel will then evaluate and make a decision based on its configuration. This approach within Hypernet ensures a dynamic and adaptable encryption mechanism, enhancing the overall security of communications.


### Encryption of Data
During the processing, storage, transmission, or reception of data, Hypernet enables secure encryption of the data to prevent compromise in the event of theft, loss, or malicious attempts.