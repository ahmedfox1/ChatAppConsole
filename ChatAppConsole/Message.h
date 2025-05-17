#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <ctime>
#include <vector>
using namespace std;
class Message {
private:
    string messageID;
    string senderID;
    string senderName;

    string text;
    time_t sentTime;
    bool isSeen;
    vector<string> seenBy;

public:
    Message(const string& id, const string& sender, const string& msg);

    string getID() const;
    string getSenderID() const;
    string getSenderName() const;
    string getText() const;
    time_t getSentTime() const;
    bool getIsSeen() const;

    void markAsSeen(const string& userID);
    bool isSeenByAll(const vector<string>& participants) const;

    string serialize() const;
    static Message deserialize(const string& data);
};

#endif