#ifndef CHATAPPLICATION_H
#define CHATAPPLICATION_H

#include "User.h"
#include "ChatRoom.h"
#include <map>
#include <string>
#include <fstream>
using namespace std;

class ChatApplication {
private:
    map<string, User> users;
    map<string, ChatRoom> chatRooms;
    string currentUserID;

    void loadUsers();
    void loadChatRooms();
    void saveUsers();
    void saveChatRooms();

public:
    ChatApplication();
    ~ChatApplication();

    // User management
    bool registerUser(const string& mobile, const string& password,
        const string& firstName, const string& lastName);
    bool login(const string& mobile, const string& password);
    void logout();

    // Chat functionality
    string createChatRoom(const vector<string>& participants, bool isGroup = false);
    void sendMessage(const string& chatRoomID, const string& text);
    void undoMessage(const string& chatRoomID, const string& messageID);

    // Contact management
    void addContact(const string& contactMobile);
    void removeContact(const string& contactMobile);

    // Story functionality
    void createStory(const string& text, const string& photoPath = "");
    vector<Story> getActiveStories() const;

    // Profile functionality
    void updateProfile(const string& photoPath, const string& about, bool visibility);
    bool canViewProfile(const string& profileUserID) const;
    string getProfileInfo(const string& profileUserID) const;

    // Getters
    const User* getCurrentUser() const;
    const map<string, ChatRoom>& getChatRooms() const;
    const ChatRoom* getChatRoom(const string& chatRoomID) const;
    string getUserName(const string& userID) const;
};

#endif