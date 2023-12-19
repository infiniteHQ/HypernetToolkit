# The notification

## Introduction

### What is a notification ?

A notification is a brief and straightforward communication that doesn't necessitate a dedicated channel, allowing for the rapid transfer of information related to the application and the programming of the Internet protocol. Notifications quickly inform another element within the network of a change, an update, or a request for information related to the protocol.

#### Elaboration and Clarification:

In the context of Hypernet, a notification serves as a concise form of communication designed for quick and direct information exchange. Unlike transmissions that utilize dedicated channels, notifications provide a swift means of notifying another network element about changes, updates, or requests for information related to the Internet protocol.

For example, a notification may be used to inform other elements of the network about a change in settings, a software update, or a request for specific information relevant to the protocol. This streamlined form of communication is efficient for conveying essential information without the need for an extensive channel setup.

### The Goal of Notifications

The goal is to convey brief and clear information related to the programming and understanding of all elements within a matrix or channel. Notifications facilitate agreement on an encryption protocol (via ::handshake, for example), requesting to join a channel or matrix (via ::ask), or inquiring about configurations or the number of elements present in a service (via ::get).

Notifications are exclusively employed for transmitting information directly linked to Hypernet and are never used for conveying data unrelated to the Hypernet framework.

#### Explication :

Notifications within Hypernet serve a specific purpose—to quickly and clearly transmit information essential for the programming and comprehension of elements within a matrix or channel. Some common goals of notifications include:

- Encryption Protocol Agreement: Using notifications, elements can quickly agree on an encryption protocol, for instance, through a process like ::handshake.

- Joining Channels or Matrices: Elements can utilize notifications, such as ::ask, to request access or join a specific channel or matrix.

- Configuration and Element Inquiry: Notifications like ::get can be employed to request information about configurations or the number of elements present in a service.

It's crucial to note that notifications in Hypernet are exclusively intended for communication directly related to the Hypernet framework, ensuring a focused and efficient exchange of essential information.

### Autopsy of Notifications

A notification is composed of several parts: the communication identifier, the version identifier, the tag, the type, parameters, and the buffer.

<span style="color:#FF3156"> **n:**</span>
<span style="color:#BD31FF">**v1:**</span>{
<span style="color:#4F89FA">**::tag**</span>
<span style="color:#4FFA75">**\<type>**</span>
<span style="color:#FAA14F">**(parameters)**</span>
<span style="color:#F64FFA">**[buffer]**</span>}


- **Communication Identifier (<span style="color:#FF3156"> **n:**</span> ):** This identifier easily distinguishes the type of communication. Simply put, if it's "n," it's a notification; if it's "t," it's a transmission, and if it's "c," it's a custom communication.

- **Version Identifier (<span style="color:#BD31FF">**v1:**</span>):** This identifier readily indicates the version of the formatting used. For example, "v1" pertains to version 1 of the Hypernet Communication Protocol notification formatting. "v1s" corresponds to a significantly streamlined version of the notification formatting.

- **Tag (<span style="color:#4F89FA">::tag**</span>**)**: The tag identifies the specific purpose of the notification. For instance, a ::ask notification serves a different purpose than a ::get notification. Tagging allows clear communication of the intended use of the notification.

- **Type (<span style="color:#4FFA75">\<type></span>)**: The type identifies the nature of the communication. A ::ask notification, for instance, does not explicitly communicate the type of request being made. It might be a request for a channel or a matrix. However, a ::ask<joinMatrix> notification explicitly communicates that the request is to join a matrix.

- **Parameters (<span style="color:#FAA14F">(parameters)</span>) :** Parameters allow the addition of specific fields to notifications to specify and clarify them. For example, in the case of ::ask<joinMatrix>, parameters such as "rec" for receiver help specify who is making the request and how the response should be delivered.

- **Buffer (<span style="color:#F64FFA">[buffer]</span>) :** The buffer is a specific field that easily facilitates the transmission of content, a file, or a JSON body, for instance. 

IMPORTANT: *While the buffer exists, it is not advisable to use this field to transmit application data or information. The primary purpose of the notification is to provide a framework for elements using the HCP, and using this field for application data transmission is not recommended.*



## Hypernet Standard Library Definitions
The theory provides crucial information about the modular functioning of notitifications. Now, let's explore more practically how Hypernet's standard library implements this theory. To implement everything we have seen and will see, refer to the documentation and tutorials of the Hypernet Toolkit.

#### Basic notifications
Now that we've explored how a notification is structured, let's proceed to list the standard notifications. (Note that you can implement your elements and matrices with new notifications that you create.)

----
**<span style="color:#4F89FA">::ping & ::pong</span> :**

(ping, pong) : This notification serves the simple purpose of testing the connection between two elements. When an element sends a ::ping, it awaits a ::pong in response. This exchange helps ensure that nodes are active and elements are responsive, contributing to the overall health of the network.

----
**<span style="color:#4F89FA">::error</span> :**

(error) : This notification serves the purpose of announcing any encountered errors. It is used to communicate and handle various error conditions within the network.

----
**<span style="color:#4F89FA">::return</span> :**

(return) : This notification is used to respond to specific requests, often employed in responses to ::get requests and similar queries. It facilitates the retrieval of requested information or the acknowledgment of completed tasks.

----
**<span style="color:#4F89FA">::ask</span> :**

(ask) : This notification is utilized to make a request related to a matrix, a channel, an element, or any other relevant aspect within the network. It enables the initiation of inquiries and interactions within the Hypernet technology.

----
**<span style="color:#4F89FA">::welcome</span> :**

(welcome) : This notification is employed to convey the fundamental package of a matrix or a channel, outlining the rules to follow, encryption methods, present elements, and other pertinent information. It serves as an introductory communication to establish a common understanding among network participants.

----
**<span style="color:#4F89FA">::get</span> :**

(get) : This notification is used to attempt the retrieval of a resource or information from a matrix, an element, or a channel. It serves as a request to obtain specific data or details within the Hypernet network.

----
**<span style="color:#4F89FA">::update</span> :**

(update) : This notification is employed to update crucial information within a channel, matrix, or element. It facilitates the communication of changes or modifications that are pertinent to the Hypernet network.

----
**<span style="color:#4F89FA">::handshake</span> :**

(handshake) : This notification is used to reach an agreement on various subjects, such as the chosen encryption protocol, the maximum size of possible buffers, or the communication method.

----
**<span style="color:#4F89FA">::hello</span> :**

(hello) : This notification is used to signal one's presence and identity.

----
**<span style="color:#4F89FA">::bye</span> :**

(bye) : This notification is used to signal a disconnection or the termination of a service.
