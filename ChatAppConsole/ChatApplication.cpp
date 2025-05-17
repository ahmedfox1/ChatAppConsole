#include "ChatApplication.h"
#include <algorithm>
#include <ctime>
#include <sstream>
#include <random>
using namespace std;
ChatApplication::ChatApplication() {
    loadUsers();
    loadChatRooms();
    currentUserID = "";
}

ChatApplication::~ChatApplication() {
    saveUsers();
    saveChatRooms();
}

void ChatApplication::loadUsers() {
    ifstream file("data/users.dat");
    if (!file.is_open()) return;

    string line;
    while (getline(file, line)) {
        try {
            User user = User::deserialize(line);
            users[user.getID()] = user;
        }
        catch (...) {
        }
    }
    file.close();
}

void ChatApplication::loadChatRooms() {
    ifstream file("data/chatrooms.dat");
    if (!file.is_open()) return;

    string line;
    while (getline(file, line)) {
        try {
            ChatRoom room = ChatRoom::deserialize(line);
            chatRooms[room.getID()] = room;
        }
        catch (...) {
        }
    }
    file.close();
}

void ChatApplication::saveUsers() {

    ofstream file("data/users.dat");
    if (!file.is_open()) return;

    for (const auto& pair : users) {
        file << pair.second.serialize() << "\n";
    }
    file.close();
}

void ChatApplication::saveChatRooms() {
    ofstream file("data/chatrooms.dat");
    if (!file.is_open()) return;

    for (const auto& pair : chatRooms) {
        file << pair.second.serialize() << "\n";
    }
    file.close();
}

bool ChatApplication::registerUser(const string& mobile, const string& password,
    const string& firstName, const string& lastName) {
    for (const auto& pair : users) {
        if (pair.second.getMobileNumber() == mobile) {
            return false;
        }
    }

    // Generate unique user ID
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(100000, 999999);
    string userID = "U" + to_string(dis(gen));

    // Create and add new user
    users[userID] = User(userID, mobile, password, firstName, lastName);
    saveUsers();
    return true;
}

bool ChatApplication::login(const string& mobile, const string& password) {
    for (const auto& pair : users) {
        if (pair.second.getMobileNumber() == mobile && pair.second.getPassword() == password) {
            currentUserID = pair.first;
            return true;
        }
    }
    return false;
}

void ChatApplication::logout() {
    currentUserID = "";
}

string ChatApplication::createChatRoom(const vector<string>& participants, bool isGroup) {
    // Generate unique chat room ID
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(100000, 999999);
    string chatRoomID = "CR" + to_string(dis(gen));

    // Create chat room
    chatRooms[chatRoomID] = ChatRoom(chatRoomID, participants, isGroup);

    // Add chat room to participants
    for (const auto& participant : participants) {
        if (users.find(participant) != users.end()) {
            users[participant].addChatRoom(chatRoomID);
        }
    }
    saveChatRooms(); 
    saveUsers();     
    return chatRoomID;
}

void ChatApplication::sendMessage(const string& chatRoomID, const string& text) {
    if (currentUserID.empty() || chatRooms.find(chatRoomID) == chatRooms.end()) return;

    // Generate unique message ID
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(100000, 999999);
    string messageID = "M" + to_string(dis(gen));

    // Create and add message
    Message msg(messageID, currentUserID, text);
    chatRooms[chatRoomID].addMessage(msg);
    saveChatRooms(); 
}

void ChatApplication::undoMessage(const string& chatRoomID, const string& messageID) {
    if (currentUserID.empty() || chatRooms.find(chatRoomID) == chatRooms.end()) return;

    // Find and remove message if sent by current user
    const ChatRoom& room = chatRooms[chatRoomID];
    const vector<Message>& messages = room.getMessages();

    auto it = find_if(messages.begin(), messages.end(),
        [&messageID, this](const Message& msg) {
            return msg.getID() == messageID && msg.getSenderID() == currentUserID;
        });

    if (it != messages.end()) {
        chatRooms[chatRoomID].removeMessage(messageID);
        saveChatRooms();
    }
    saveChatRooms(); 
}

void ChatApplication::addContact(const string& contactMobile) {
    if (currentUserID.empty()) return;

    for (auto& pair : users) {
        if (pair.second.getMobileNumber() == contactMobile) {
            users[currentUserID].addContact(pair.first);
            pair.second.addContact(currentUserID);
            saveUsers(); 
            return;
        }
    }
}

void ChatApplication::removeContact(const string& contactMobile) {
    if (currentUserID.empty()) return;

    for (auto& pair : users) {
        if (pair.second.getMobileNumber() == contactMobile) {
            users[currentUserID].removeContact(pair.first);
            pair.second.removeContact(currentUserID);
            saveUsers(); // Save after change
            return;
        }
    }
}

void ChatApplication::createStory(const string& text, const string& photoPath) {
    if (currentUserID.empty()) return;
    users[currentUserID].createStory(text, photoPath);
    saveUsers(); // Save after change
}

vector<Story> ChatApplication::getActiveStories() const {
    vector<Story> activeStories;
    if (currentUserID.empty()) return activeStories;

    const User& currentUser = users.at(currentUserID);
    for (const auto& contactID : currentUser.getContacts()) {
        if (users.find(contactID) != users.end()) {
            const auto& contactStories = users.at(contactID).getActiveStories();
            activeStories.insert(activeStories.end(), contactStories.begin(), contactStories.end());
        }
    }

    return activeStories;
}

void ChatApplication::updateProfile(const string& photoPath, const string& about, bool visibility) {
    if (currentUserID.empty()) return;
    users[currentUserID].setProfileInfo(photoPath, about, visibility);
    saveUsers(); // Save after change
}

bool ChatApplication::canViewProfile(const string& profileUserID) const {
    if (currentUserID.empty() || users.find(profileUserID) == users.end()) return false;
    return users.at(profileUserID).canViewProfile(currentUserID);
}

string ChatApplication::getProfileInfo(const string& profileUserID) const {
    if (!canViewProfile(profileUserID)) return "";

    const User& user = users.at(profileUserID);
    ostringstream oss;
    oss << "Name: " << user.getFirstName() << " " << user.getLastName() << "\n";
    oss << "About: " << user.getAbout() << "\n";
    if (!user.getProfilePhoto().empty()) {
        oss << "Photo: " << user.getProfilePhoto() << "\n";
    }
    return oss.str();
}

const User* ChatApplication::getCurrentUser() const {
    if (currentUserID.empty() || users.find(currentUserID) == users.end()) return nullptr;
    return &users.at(currentUserID);
}

const map<string, ChatRoom>& ChatApplication::getChatRooms() const {
    return chatRooms;
}

const ChatRoom* ChatApplication::getChatRoom(const string& chatRoomID) const {
    if (chatRooms.find(chatRoomID) == chatRooms.end()) return nullptr;
    return &chatRooms.at(chatRoomID);
}

string ChatApplication::getUserName(const string& userID) const {
    auto it = users.find(userID);
    if (it != users.end()) {
        return it->second.getFirstName() + " " + it->second.getLastName();
    }
    return "Unknown";
}