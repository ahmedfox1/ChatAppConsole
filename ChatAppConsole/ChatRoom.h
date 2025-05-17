#ifndef CHATROOM_H
#define CHATROOM_H

#include "Message.h"
#include <vector>
#include <string>
using namespace std;

class ChatRoom {
private:
    string chatRoomID;
    vector<string> participants;
    vector<Message> messages;
    bool isGroupChat;

public:
    ChatRoom() = default;
    ChatRoom(const string& id, const vector<string>& users, bool isGroup = false);

    string getID() const;
    bool getIsGroupChat() const;
    const vector<string>& getParticipants() const;
    const vector<Message>& getMessages() const;

    void addMessage(const Message& msg);
    void removeMessage(const string& messageID);
    void markMessagesAsSeen(const string& userID);

    string serialize() const;
    static ChatRoom deserialize(const string& data);
};

#endif 