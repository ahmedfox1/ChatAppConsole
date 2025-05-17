#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include <ctime>
using namespace std;

class Story {
private:
    string userID;
    time_t publishedTime;
    string storyText;
    string storyPhotoPath;

public:
    
    Story(const string& uid, const string& text, const string& photo = "");

    string getUserID() const;
    time_t getPublishedTime() const;
    string getStoryText() const;
    string getStoryPhotoPath() const;

    bool isExpired() const;

    string serialize() const;
    static Story deserialize(const string& data);
};

class User {
private:
    string userID;
    string mobileNumber;
    string password;
    string firstName;
    string lastName;
    vector<string> contacts;
    vector<string> chatRooms;
    vector<Story> stories;
    string profilePhoto;
    string about;
    bool profileVisibleToAll;

public:
    User() = default;
    User(const string& uid, const string& mobile, const string& pwd,
        const string& fname, const string& lname);

    // Getters
    string getID() const;
    string getMobileNumber() const;
    string getPassword() const;
    string getFirstName() const;
    string getLastName() const;
    const vector<string>& getContacts() const;
    const vector<string>& getChatRooms() const;
    vector<Story> getActiveStories() const;
    string getProfilePhoto() const;
    string getAbout() const;
    bool isProfileVisibleToAll() const;

    // Setters
    void setPassword(const string& pwd);
    void setProfileInfo(const string& photo, const string& aboutDesc, bool visibility);

    // Functions
    void addContact(const string& contactID);
    void removeContact(const string& contactID);
    void addChatRoom(const string& chatRoomID);
    void removeChatRoom(const string& chatRoomID);
    void createStory(const string& text, const string& photoPath = "");

    bool canViewProfile(const string& viewerID) const;

    string serialize() const;
    static User deserialize(const string& data);
};

#endif