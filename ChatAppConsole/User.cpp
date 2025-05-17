#include "User.h"
#include <algorithm>
#include <ctime>
#include <sstream>
using namespace std;
Story::Story(const string& uid, const string& text, const string& photo)
    : userID(uid), storyText(text), storyPhotoPath(photo) {
    publishedTime = time(nullptr);
}

string Story::getUserID() const { return userID; }
time_t Story::getPublishedTime() const { return publishedTime; }
string Story::getStoryText() const { return storyText; }
string Story::getStoryPhotoPath() const { return storyPhotoPath; }

bool Story::isExpired() const {
    return difftime(time(nullptr), publishedTime) > 24 * 3600;
}

string Story::serialize() const {
    ostringstream oss;
    oss << userID << "|" << publishedTime << "|" << storyText << "|" << storyPhotoPath;
    return oss.str();
}

Story Story::deserialize(const string& data) {
    istringstream iss(data);
    string part;
    vector<string> parts;

    while (getline(iss, part, '|')) {
        parts.push_back(part);
    }

    if (parts.size() < 4) {
        throw runtime_error("Invalid story data format");
    }

    Story story(parts[0], parts[2], parts[3]);
    story.publishedTime = static_cast<time_t>(stol(parts[1]));
    return story;
}

User::User(const string& uid, const string& mobile, const string& pwd,
    const string& fname, const string& lname)
    : userID(uid), mobileNumber(mobile), password(pwd),
    firstName(fname), lastName(lname), profileVisibleToAll(false) {}

string User::getID() const { return userID; }
string User::getMobileNumber() const { return mobileNumber; }
string User::getPassword() const { return password; }
string User::getFirstName() const { return firstName; }
string User::getLastName() const { return lastName; }
const vector<string>& User::getContacts() const { return contacts; }
const vector<string>& User::getChatRooms() const { return chatRooms; }
string User::getProfilePhoto() const { return profilePhoto; }
string User::getAbout() const { return about; }
bool User::isProfileVisibleToAll() const { return profileVisibleToAll; }

void User::setPassword(const string& pwd) { password = pwd; }

void User::setProfileInfo(const string& photo, const string& aboutDesc, bool visibility) {
    profilePhoto = photo;
    about = aboutDesc;
    profileVisibleToAll = visibility;
}

void User::addContact(const string& contactID) {
    if (find(contacts.begin(), contacts.end(), contactID) == contacts.end()) {
        contacts.push_back(contactID);
    }
}

void User::removeContact(const string& contactID) {
    contacts.erase(remove(contacts.begin(), contacts.end(), contactID), contacts.end());
}

void User::addChatRoom(const string& chatRoomID) {
    if (find(chatRooms.begin(), chatRooms.end(), chatRoomID) == chatRooms.end()) {
        chatRooms.push_back(chatRoomID);
    }
}

void User::removeChatRoom(const string& chatRoomID) {
    chatRooms.erase(remove(chatRooms.begin(), chatRooms.end(), chatRoomID), chatRooms.end());
}

void User::createStory(const string& text, const string& photoPath) {
    stories.emplace_back(userID, text, photoPath);
}

vector<Story> User::getActiveStories() const {
    vector<Story> activeStories;
    for (const auto& story : stories) {
        if (!story.isExpired()) {
            activeStories.push_back(story);
        }
    }
    return activeStories;
}

bool User::canViewProfile(const string& viewerID) const {
    if (profileVisibleToAll) return true;
    return find(contacts.begin(), contacts.end(), viewerID) != contacts.end();
}

string User::serialize() const {
    ostringstream oss;
    oss << userID << "|" << mobileNumber << "|" << password << "|"
        << firstName << "|" << lastName << "|" << profileVisibleToAll << "|"
        << profilePhoto << "|" << about << "|";

    for (size_t i = 0; i < contacts.size(); ++i) {
        if (i != 0) oss << ",";
        oss << contacts[i];
    }

    oss << "|";

    for (size_t i = 0; i < chatRooms.size(); ++i) {
        if (i != 0) oss << ",";
        oss << chatRooms[i];
    }

    oss << "|";

    for (size_t i = 0; i < stories.size(); ++i) {
        if (i != 0) oss << ";";
        oss << stories[i].serialize();
    }

    return oss.str();
}

User User::deserialize(const string& data) {
    istringstream iss(data);
    string part;
    vector<string> parts;

    while (getline(iss, part, '|')) {
        parts.push_back(part);
    }

    if (parts.size() < 9) {
        throw runtime_error("Invalid user data format");
    }

    User user(parts[0], parts[1], parts[2], parts[3], parts[4]);
    user.profileVisibleToAll = parts[5] == "1";
    user.profilePhoto = parts[6];
    user.about = parts[7];

    if (!parts[8].empty()) {
        istringstream contIss(parts[8]);
        while (getline(contIss, part, ',')) {
            user.contacts.push_back(part);
        }
    }

    if (parts.size() > 9 && !parts[9].empty()) {
        istringstream roomIss(parts[9]);
        while (getline(roomIss, part, ',')) {
            user.chatRooms.push_back(part);
        }
    }

    if (parts.size() > 10 && !parts[10].empty()) {
        istringstream storyIss(parts[10]);
        while (getline(storyIss, part, ';')) {
            user.stories.push_back(Story::deserialize(part));
        }
    }

    return user;
}