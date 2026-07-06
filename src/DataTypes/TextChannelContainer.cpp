#include "DataTypes.h"
#include "TextChannel.h"
#include "Server.h"

// Public contructor
TextChannelContainer::TextChannelContainer(DataTypes datatype, TextChannel* channel) : Container(datatype, UUID_BYTE_SIZE*2) {
    this->serverID = *channel->getServer()->getID();
    this->channelID = *channel->getID();
    serialize();
}

// Private Constructor
TextChannelContainer::TextChannelContainer(std::string serverid, std::string channelid) : Container(DataTypes::EMPTY, 0) {
    this->serverID = serverid;
    this->channelID = channelid;
}

void TextChannelContainer::serialize() {
    unsigned char uuid[UUID_BYTE_SIZE];

    // ServerID
    ID::BytesFromString(this->serverID, uuid);
    memcpy(data+offset, uuid, UUID_BYTE_SIZE);
    offset += UUID_BYTE_SIZE;

    // ChannelID
    ID::BytesFromString(this->channelID, uuid);
    memcpy(data+offset, uuid, UUID_BYTE_SIZE);
    offset += UUID_BYTE_SIZE;
}

TextChannelContainer TextChannelContainer::deserialize(unsigned char* serializedData) {
    // The dataType has been removed by now
    int offset = sizeof(DataTypes);

    // ServerID
    std::string serverID = ID::stringFromBytes(serializedData+offset);
    offset += UUID_BYTE_SIZE;

    // ChannelID
    std::string channelID = ID::stringFromBytes(serializedData+offset);
    offset += UUID_BYTE_SIZE;

    return TextChannelContainer(serverID, channelID);
}

std::string TextChannelContainer::getServerID() {
    return this->serverID;
}
std::string TextChannelContainer::getChannelID() {
    return this->channelID;
}