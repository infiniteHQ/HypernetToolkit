The transmission
=====

----

### What is a Transmission?

A transmission refers to an applicative communication designed to convey information. In the context of Hypernet, this communication is entirely programmable, covering various aspects such as the nature of the information (sensor data, audio or video streams, files, etc.), the method of transmission (subscription, streaming, events, state, timers, etc.), and the entire context of the communication (security, formatting, structuring, communication segmentation, sequencing, targeting, etc.).

In essence, transmissions facilitate the communication of applicative data in a programmed and modular manner. This includes defining what type of data is being transmitted, specifying how it is transmitted, and establishing the complete framework for the communication. The flexibility extends to aspects like security measures, formatting rules, communication segmentation, sequencing of data, and targeting specific components or entities within the system.

By being programmable and modular, transmissions in Hypernet allow for a high degree of customization, enabling users to tailor the communication process to meet specific requirements and scenarios. This approach enhances the adaptability and efficiency of data exchange within the Hypernet environment.


### Autopsy of Notifications

La transmission est composée

<span style="color:#FF3156"> **t:**</span>
<span style="color:#BD31FF">**v1:**</span>{
<span style="color:#4F895A">**::channelIdentifier**</span>
<span style="color:#4F89FA">**::elementIdentifier**</span>
<span style="color:#FAA14F">**(parameters)**</span>
<span style="color:#F64FFA">**[buffer]**</span>}


- **Communication Identifier (<span style="color:#FF3156"> **t:**</span> ):** This identifier easily distinguishes the type of communication. Simply put, if it's "n," it's a notification; if it's "t," it's a transmission, and if it's "c," it's a custom communication.


- **Version Identifier (<span style="color:#BD31FF">**v1:**</span>):** This identifier readily indicates the version of the formatting used. For example, "v1" pertains to version 1 of the Hypernet Communication Protocol notification formatting. "v1s" corresponds to a significantly streamlined version of the notification formatting.


- **Channel Identifier (<span style="color:#4F895A">::channelIdentifier**</span>):    The ::channelIdentifier in Hypernet plays a crucial role in precisely locating the communication pathway for a transmission. It acts as a unique label associated with a specific channel, functioning much like an address for seamless navigation within the Hypernet infrastructure. This identifier is integral to the transmission's metadata, providing crucial information for routing data flow along the designated channel. The uniqueness of the ::channelIdentifier is paramount, preventing confusion and ensuring efficient routing to the correct channels. Like other elements in Hypernet, this identifier is programmable, offering users the flexibility to customize and configure channels based on specific needs, such as data type, security protocols, or communication characteristics. In summary, the ::channelIdentifier is a key element in Hypernet's communication landscape, providing a targeted and identifiable path for the smooth flow of transmissions within its programmable and modular framework.


- **Element Identifier (<span style="color:#4F89FA">::elementIdentifier**</span>): The ::elementIdentifier is a crucial component in Hypernet's communication, serving as a unique label for identifying the origin of a transmission. Much like a signature or address, it distinguishes one transmitting entity from another, providing clarity on the responsible element. This identifier is programmable, allowing users to customize it for sensors, data sources, or other elements. Its uniqueness aids in effective tracking, becoming a fundamental part of transmission metadata. In summary, ::elementIdentifier enhances traceability and accountability in Hypernet, attributing each communication event to the specific responsible element within its programmable framework.

- **Parameters (<span style="color:#FAA14F">(parameters)**</span>): Parameters are used to fine-tune transmissions in Hypernet by specifying essential details like packet identifiers for segmentation, section numbers within channels, or other formatting specifics. These customizable attributes accompany each transmission, allowing users to tailor the communication to specific needs. In essence, parameters provide a dynamic toolkit for adjusting and enhancing the behavior of transmissions within the programmable framework of Hypernet.

- **Buffer (<span style="color:#F64FFA">[buffer]**</span>): The buffer in Hypernet consists of raw data ready for distribution.

