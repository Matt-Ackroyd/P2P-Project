# P2P-Project

This is an unfinished project of mine aiming to create a secure peer to peer social platform

it utilizes openssl's ML-KEM implementation to encrypt most of the data sent between clients & UDP hole punching to allow clients to talk to one another behind most types of NAT's

Currently clients are able to exchange messages and file but I intend to allow support for voice calls and video streams.

Each client stores a copy of the database and clients talk to one another to fill each other in on what they missed when they were offline.


Plans to implement:
- Deletion of messages & files after sending them
- Voice calls
- Video Streams
- Finished UI