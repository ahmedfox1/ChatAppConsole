#include "ChatApplication.h"
#include <iostream>
#include <limits>
#include <algorithm>
using namespace std;

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pressEnterToContinue() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void displayMainMenu() {
    clearScreen();
    cout << "\nWhatsApp Clone\n";
    cout << "1. Register\n";
    cout << "2. Login\n";
    cout << "3. Exit\n";
    cout << "Choose an option: ";
}

void displayUserMenu(const User* user) {
    clearScreen();
    cout << "\nWelcome, " << user->getFirstName() << " " << user->getLastName() << "!\n";
    cout << "1. View Contacts\n";
    cout << "2. Add Contact\n";
    cout << "3. Remove Contact\n";
    cout << "4. View Chat Rooms\n";
    cout << "5. Open Chat Room\n";
    cout << "6. Start New Chat\n";
    cout << "7. Create Group Chat\n";
    cout << "8. View Stories\n";
    cout << "9. Create Story\n";
    cout << "10. View Profile\n";
    cout << "11. Update Profile\n";
    cout << "12. Logout\n";
    cout << "Choose an option: ";
}

void viewContacts(const ChatApplication& app) {
    const User* user = app.getCurrentUser();
    if (!user) return;

    clearScreen();
    cout << "\nYour Contacts:\n";
    const auto& contacts = user->getContacts();

    if (contacts.empty()) {
        cout << "No contacts found.\n";
        pressEnterToContinue();
        return;
    }

    for (const auto& contactID : contacts) {
        if (app.canViewProfile(contactID)) {
            cout << "- " << app.getProfileInfo(contactID) << "\n";
        }
        else {
            cout << "- [Private Profile]\n";
        }
    }
    pressEnterToContinue();
}

void addContact(ChatApplication& app) {
    clearScreen();
    cout << "\nAdd Contact\n";
    cout << "Enter mobile number: ";

    string mobile;
    getline(cin, mobile);

    app.addContact(mobile);
    cout << "Contact added successfully!\n";
    pressEnterToContinue();
}

void removeContact(ChatApplication& app) {
    clearScreen();
    const User* user = app.getCurrentUser();
    if (!user) return;

    cout << "\nRemove Contact\n";
    const auto& contacts = user->getContacts();

    if (contacts.empty()) {
        cout << "No contacts to remove.\n";
        pressEnterToContinue();
        return;
    }

    cout << "Your Contacts:\n";
    int index = 1;
    for (const auto& contactID : contacts) {
        cout << index++ << ". " << contactID << "\n";
    }

    cout << "Select contact to remove (1-" << contacts.size() << "): ";
    int choice;
    cin >> choice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (choice > 0 && choice <= static_cast<int>(contacts.size())) {
        app.removeContact(contacts[choice - 1]);
        cout << "Contact removed successfully!\n";
    }
    else {
        cout << "Invalid selection.\n";
    }
    pressEnterToContinue();
}

void viewChatRooms(const ChatApplication& app) {
    const User* user = app.getCurrentUser();
    if (!user) return;

    clearScreen();
    cout << "\nYour Chat Rooms:\n";
    const auto& chatRooms = user->getChatRooms();
    const auto& allChatRooms = app.getChatRooms();

    if (chatRooms.empty()) {
        cout << "No chat rooms found.\n";
        pressEnterToContinue();
        return;
    }

    int index = 1;
    for (const auto& roomID : chatRooms) {
        const ChatRoom* room = app.getChatRoom(roomID);
        if (room) {
            cout << index++ << ". ";
            if (room->getIsGroupChat()) {
                cout << "Group Chat with: ";
                bool first = true;
                for (const auto& participant : room->getParticipants()) {
                    if (participant != user->getID()) {
                        if (!first) cout << ", ";
                        cout << app.getUserName(participant) << " (" << participant << ")";
                        first = false;
                    }
                }
                cout << "\n";
            } else {
                for (const auto& participant : room->getParticipants()) {
                    if (participant != user->getID()) {
                        cout << "Chat with " << app.getUserName(participant) << " (" << participant << ")\n";
                        break;
                    }
                }
            }
        }
    }
    pressEnterToContinue();
}

void openChatRoom(ChatApplication& app) {
    const User* user = app.getCurrentUser();
    if (!user) return;

    clearScreen();
    cout << "\nOpen Chat Room\n";
    const auto& chatRooms = user->getChatRooms();

    if (chatRooms.empty()) {
        cout << "No chat rooms found.\n";
        pressEnterToContinue();
        return;
    }

    cout << "Your Chat Rooms:\n";
    int index = 1;
    for (const auto& roomID : chatRooms) {
        const ChatRoom* room = app.getChatRoom(roomID);
        if (room) {
            cout << index++ << ". ";
            if (room->getIsGroupChat()) {
                cout << "Group Chat (" << room->getParticipants().size() << " participants)\n";
            }
            else {
                for (const auto& participant : room->getParticipants()) {
                    if (participant != user->getID()) {
                        cout << "Chat with " << participant << "\n";
                        break;
                    }
                }
            }
        }
    }

    cout << "Select chat room to open (1-" << chatRooms.size() << "): ";
    int choice;
    cin >> choice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (choice > 0 && choice <= static_cast<int>(chatRooms.size())) {
        const string& roomID = chatRooms[choice - 1];

        while (true) {
            clearScreen();
            cout << "\nChat Room\n";

            // Always get the latest chat room pointer
            const ChatRoom* room = app.getChatRoom(roomID);
            if (!room) {
                cout << "Chat room not found.\n";
                pressEnterToContinue();
                break;
            }

            // Mark messages as seen
            const_cast<ChatRoom*>(room)->markMessagesAsSeen(user->getID());

            // Display participants
            if (room->getIsGroupChat()) {
                cout << "Group Chat with " << room->getParticipants().size() << " participants\n";
            }
            else {
                for (const auto& participant : room->getParticipants()) {
                    if (participant != user->getID()) {
                        cout << "Chat with " << participant << "\n";
                        break;
                    }
                }
            }

            // Display messages
            const auto& messages = room->getMessages();
            if (messages.empty()) {
                cout << "\nNo messages yet.\n";
            }
            else {
                for (const auto& msg : messages) {
                    const User* sender = app.getCurrentUser();
                    if (msg.getSenderID() != user->getID()) {
                        sender = app.getCurrentUser(); // fallback
                        for (const auto& u : app.getChatRooms().at(roomID).getParticipants()) {
                            if (u == msg.getSenderID()) {
                                sender = app.getCurrentUser();
                                if (app.getCurrentUser() && app.getCurrentUser()->getID() == u) break;
                            }
                        }
                    }
                    string senderName = sender ? (sender->getFirstName() + " " + sender->getLastName()) : "Unknown";
                    cout << "\n[" << msg.getSenderID() << " | " << app.getUserName(msg.getSenderID()) << "]: " << msg.getText();
                    cout << " (" << (msg.getIsSeen() ? "Seen" : "Unseen") << ")";
                }
            }

            // Chat options
            cout << "\n\n1. Send Message\n";
            cout << "2. Undo Last Message\n";
            cout << "3. Back to Menu\n";
            cout << "Choose an option: ";

            int chatChoice;
            cin >> chatChoice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (chatChoice == 1) {
                cout << "Enter your message: ";
                string messageText;
                getline(cin, messageText);
                app.sendMessage(roomID, messageText);
            }
            else if (chatChoice == 2 && !messages.empty()) {
                app.undoMessage(roomID, messages.back().getID());
            }
            else if (chatChoice == 3) {
                break;
            }
        }
    }
}

void startNewChat(ChatApplication& app) {
    const User* user = app.getCurrentUser();
    if (!user) return;

    clearScreen();
    cout << "\nStart New Chat\n";
    const auto& contacts = user->getContacts();

    if (contacts.empty()) {
        cout << "No contacts available to start a chat.\n";
        pressEnterToContinue();
        return;
    }

    cout << "Select contact to chat with:\n";
    int index = 1;
    for (const auto& contactID : contacts) {
        cout << index++ << ". " << contactID << "\n";
    }

    cout << "Choose contact (1-" << contacts.size() << "): ";
    int choice;
    cin >> choice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (choice > 0 && choice <= static_cast<int>(contacts.size())) {
        vector<string> participants = { user->getID(), contacts[choice - 1] };
        string roomID = app.createChatRoom(participants);
        cout << "Chat created successfully!\n";

        // Open the new chat room
        const ChatRoom* room = app.getChatRoom(roomID);
        if (room) {
            while (true) {
                clearScreen();
                cout << "\nNew Chat with " << contacts[choice - 1] << "\n";

                // Display messages
                const auto& messages = room->getMessages();
                if (messages.empty()) {
                    cout << "\nNo messages yet.\n";
                }
                else {
                    for (const auto& msg : messages) {
                        const User* sender = app.getCurrentUser();
                        if (msg.getSenderID() != user->getID()) {
                            sender = app.getCurrentUser(); // fallback
                            for (const auto& u : app.getChatRooms().at(roomID).getParticipants()) {
                                if (u == msg.getSenderID()) {
                                    sender = app.getCurrentUser();
                                    if (app.getCurrentUser() && app.getCurrentUser()->getID() == u) break;
                                }
                            }
                        }
                        string senderName = sender ? (sender->getFirstName() + " " + sender->getLastName()) : "Unknown";
                        cout << "\n[" << msg.getSenderID() << " | " << app.getUserName(msg.getSenderID()) << "]: " << msg.getText();
                        cout << " (" << (msg.getIsSeen() ? "Seen" : "Unseen") << ")";
                    }
                }

                // Chat options
                cout << "\n\n1. Send Message\n";
                cout << "2. Back to Menu\n";
                cout << "Choose an option: ";

                int chatChoice;
                cin >> chatChoice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                if (chatChoice == 1) {
                    cout << "Enter your message: ";
                    string messageText;
                    getline(cin, messageText);
                    app.sendMessage(roomID, messageText);
                }
                else if (chatChoice == 2) {
                    break;
                }
            }
        }
    }
    else {
        cout << "Invalid selection.\n";
        pressEnterToContinue();
    }
}

void createGroupChat(ChatApplication& app) {
    const User* user = app.getCurrentUser();
    if (!user) return;

    clearScreen();
    cout << "\nCreate Group Chat\n";
    const auto& contacts = user->getContacts();

    if (contacts.size() < 2) {
        cout << "You need at least 2 contacts to create a group chat.\n";
        pressEnterToContinue();
        return;
    }

    vector<string> participants = { user->getID() };

    while (true) {
        clearScreen();
        cout << "\nSelect participants (current: " << participants.size() << ")\n";

        int index = 1;
        for (const auto& contactID : contacts) {
            bool selected = find(participants.begin(), participants.end(), contactID) != participants.end();
            cout << index++ << ". " << contactID << (selected ? " [Added]" : "") << "\n";
        }

        cout << "\n1. Add/Remove Participant\n";
        cout << "2. Create Group\n";
        cout << "3. Cancel\n";
        cout << "Choose an option: ";

        int choice;
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice == 1) {
            cout << "Select contact to toggle (1-" << contacts.size() << "): ";
            int contactChoice;
            cin >> contactChoice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (contactChoice > 0 && contactChoice <= static_cast<int>(contacts.size())) {
                const string& contactID = contacts[contactChoice - 1];
                auto it = find(participants.begin(), participants.end(), contactID);
                if (it != participants.end()) {
                    participants.erase(it);
                }
                else {
                    participants.push_back(contactID);
                }
            }
        }
        else if (choice == 2 && participants.size() >= 3) {
            string roomID = app.createChatRoom(participants, true);
            cout << "Group chat created successfully!\n";
            pressEnterToContinue();
            break;
        }
        else if (choice == 2) {
            cout << "Group chat must have at least 3 participants (including you).\n";
            pressEnterToContinue();
        }
        else if (choice == 3) {
            break;
        }
    }
}

void viewStories(const ChatApplication& app) {
    clearScreen();
    cout << "\nStories from your contacts:\n";

    auto stories = app.getActiveStories();
    if (stories.empty()) {
        cout << "No active stories available.\n";
        pressEnterToContinue();
        return;
    }

    // Sort stories by time (newest first)
    sort(stories.begin(), stories.end(), [](const Story& a, const Story& b) {
        return a.getPublishedTime() > b.getPublishedTime();
    });

    time_t now = time(nullptr);
    for (const auto& story : stories) {
        double hoursRemaining = (24 * 3600 - difftime(now, story.getPublishedTime())) / 3600;
        cout << "\nStory from " << story.getUserID() << " | " << app.getUserName(story.getUserID()) << " (" << hoursRemaining << "h remaining):\n";
        cout << story.getStoryText() << "\n";
        if (!story.getStoryPhotoPath().empty()) {
            cout << "[Photo: " << story.getStoryPhotoPath() << "]\n";
        }
    }
    pressEnterToContinue();
}

void createStory(ChatApplication& app) {
    clearScreen();
    cout << "\nCreate New Story\n";

    cout << "Enter story text: ";
    string text;
    getline(cin, text);

    cout << "Add photo path (optional, press Enter to skip): ";
    string photoPath;
    getline(cin, photoPath);

    app.createStory(text, photoPath);
    cout << "Story published successfully!\n";
    pressEnterToContinue();
}

void viewProfile(const ChatApplication& app) {
    const User* user = app.getCurrentUser();
    if (!user) return;

    clearScreen();
    cout << "\nYour Profile\n";
    cout << "Name: " << user->getFirstName() << " " << user->getLastName() << "\n";
    cout << "Mobile: " << user->getMobileNumber() << "\n";
    cout << "About: " << user->getAbout() << "\n";
    if (!user->getProfilePhoto().empty()) {
        cout << "Photo: " << user->getProfilePhoto() << "\n";
    }
    cout << "Profile Visibility: " << (user->isProfileVisibleToAll() ? "Public" : "Contacts Only") << "\n";
    pressEnterToContinue();
}

void updateProfile(ChatApplication& app) {
    const User* user = app.getCurrentUser();
    if (!user) return;

    clearScreen();
    cout << "\nUpdate Profile\n";

    string photoPath, about;
    bool visibility;

    cout << "Current photo path: " << user->getProfilePhoto() << "\n";
    cout << "Enter new photo path (press Enter to keep current): ";
    getline(cin, photoPath);
    if (photoPath.empty()) {
        photoPath = user->getProfilePhoto();
    }

    cout << "Current about: " << user->getAbout() << "\n";
    cout << "Enter new about description: ";
    getline(cin, about);

    cout << "Make profile visible to everyone? (1 for Yes, 0 for No): ";
    cin >> visibility;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    app.updateProfile(photoPath, about, visibility);
    cout << "Profile updated successfully!\n";
    pressEnterToContinue();
}

int main() {
    ChatApplication app;
    int choice;

    while (true) {
        displayMainMenu();
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice == 3) break;

        if (choice == 1) {
            string mobile, password, firstName, lastName;

            cout << "Enter mobile number: ";
            getline(cin, mobile);

            cout << "Enter password: ";
            getline(cin, password);

            cout << "Enter first name: ";
            getline(cin, firstName);

            cout << "Enter last name: ";
            getline(cin, lastName);

            if (app.registerUser(mobile, password, firstName, lastName)) {
                cout << "Registration successful!\n";
            }
            else {
                cout << "Registration failed. Mobile number may already exist.\n";
            }
            pressEnterToContinue();
        }
        else if (choice == 2) {
            string mobile, password;

            cout << "Enter mobile number: ";
            getline(cin, mobile);

            cout << "Enter password: ";
            getline(cin, password);

            if (app.login(mobile, password)) {
                const User* user = app.getCurrentUser();

                while (user) {
                    displayUserMenu(user);
                    cin >> choice;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');

                    switch (choice) {
                    case 1: viewContacts(app); break;
                    case 2: addContact(app); break;
                    case 3: removeContact(app); break;
                    case 4: viewChatRooms(app); break;
                    case 5: openChatRoom(app); break;
                    case 6: startNewChat(app); break;
                    case 7: createGroupChat(app); break;
                    case 8: viewStories(app); break;
                    case 9: createStory(app); break;
                    case 10: viewProfile(app); break;
                    case 11: updateProfile(app); break;
                    case 12: app.logout(); user = nullptr; break;
                    default: cout << "Invalid option.\n"; pressEnterToContinue();
                    }
                }
            }
            else {
                cout << "Login failed. Invalid mobile or password.\n";
                pressEnterToContinue();
            }
        }
    }

    return 0;
}