#include "ChatRoom.h"
#include <algorithm>
#include <sstream>
using namespace std;

ChatRoom::ChatRoom(const string& id, const vector<string>& users, bool isGroup)
    : chatRoomID(id), participants(users), isGroupChat(isGroup) {}

string ChatRoom::getID() const { return chatRoomID; }
bool ChatRoom::getIsGroupChat() const { return isGroupChat; }
const vector<string>& ChatRoom::getParticipants() const { return participants; }
const vector<Message>& ChatRoom::getMessages() const { return messages; }

void ChatRoom::addMessage(const Message& msg) {
    messages.push_back(msg);
}

void ChatRoom::removeMessage(const string& messageID) {
    messages.erase(remove_if(messages.begin(), messages.end(),
        [&messageID](const Message& msg) { return msg.getID() == messageID; }),
        messages.end());
}

void ChatRoom::markMessagesAsSeen(const string& userID) {
    for (auto& msg : messages) {
        if (msg.getSenderID() != userID && !msg.getIsSeen()) {
            msg.markAsSeen(userID);
        }
    }
}

string ChatRoom::serialize() const {
    ostringstream oss;
    oss << chatRoomID << "-" << isGroupChat << "|";

    for (size_t i = 0; i < participants.size(); ++i) {
        if (i != 0) oss << ",";
        oss << participants[i];
    }

    oss << "|";

    for (size_t i = 0; i < messages.size(); ++i) {
        if (i != 0) oss << ";";
        oss << messages[i].serialize();
    }

    return oss.str();
}

ChatRoom ChatRoom::deserialize(const string& data) {
    istringstream iss(data);
    string part;
    vector<string> parts;

    while (getline(iss, part, '|')) {
        parts.push_back(part);
    }

    if (parts.size() < 3) {
        throw runtime_error("Invalid chatroom data format");
    }

    // Parse participants
    vector<string> participants;
    istringstream partIss(parts[2]);
    while (getline(partIss, part, ',')) {
        if (!part.empty()) {
            participants.push_back(part);
        }
    }

    ChatRoom room(parts[0], participants, parts[1] == "1");

    // Parse messages if they exist
    if (parts.size() > 3 && !parts[3].empty()) {
        istringstream msgIss(parts[3]);
        while (getline(msgIss, part, ';')) {
            room.addMessage(Message::deserialize(part));
        }
    }

    return room;
}