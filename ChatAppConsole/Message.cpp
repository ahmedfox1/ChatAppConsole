#include "Message.h"
#include <sstream>
#include <vector>
using namespace std;

Message::Message(const string& id, const string& sender, const string& msg)
    : messageID(id), senderID(sender), text(msg) {
    sentTime = time(nullptr);
    isSeen = false;
}

string Message::getID() const { return messageID; }
string Message::getSenderID() const { return senderID; }
string Message::getSenderName() const { return senderName; }
string Message::getText() const { return text; }
time_t Message::getSentTime() const { return sentTime; }
bool Message::getIsSeen() const { return isSeen; }

void Message::markAsSeen(const string& userID) {
    if (find(seenBy.begin(), seenBy.end(), userID) == seenBy.end()) {
        seenBy.push_back(userID);
    }
    isSeen = true;
}

bool Message::isSeenByAll(const vector<string>& participants) const {
    for (const auto& participant : participants) {
        if (participant != senderID &&
            find(seenBy.begin(), seenBy.end(), participant) == seenBy.end()) {
            return false;
        }
    }
    return true;
}

string Message::serialize() const {
    ostringstream oss;
    oss << messageID << "|" << senderID << "|" << sentTime << "|" << isSeen << "|";
    for (size_t i = 0; i < seenBy.size(); ++i) {
        if (i != 0) oss << ",";
        oss << seenBy[i];
    }
    oss << "|" << text;
    return oss.str();
}

Message Message::deserialize(const string& data) {
    istringstream iss(data);
    string part;
    vector<string> parts;

    while (getline(iss, part, '|')) {
        parts.push_back(part);
    }

    if (parts.size() < 5) {
        throw runtime_error("Invalid message data format");
    }

    Message msg(parts[0], parts[1], parts[5]);
    msg.sentTime = static_cast<time_t>(stol(parts[2]));
    msg.isSeen = parts[3] == "1";

    istringstream seenIss(parts[4]);
    while (getline(seenIss, part, ',')) {
        if (!part.empty()) {
            msg.seenBy.push_back(part);
        }
    }

    return msg;
}