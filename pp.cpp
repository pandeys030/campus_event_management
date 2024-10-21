#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <numeric>

using namespace std;

enum class EventCategory { ACADEMIC, CULTURAL, TECHNICAL, SPORTS };

class User {
private:
    string username;
    string password;
    bool isAdmin;
    vector<int> registeredEvents;
    map<int, int> eventRatings; // Event ID to rating

public:
    User(const string& u, const string& p, bool admin = false)
        : username(u), password(p), isAdmin(admin) {}

    bool authenticate(const string& p) const {
        return password == p;
    }

    bool isAdminUser() const {
        return isAdmin;
    }

    string getUsername() const {
        return username;
    }

    void registerForEvent(int eventId) {
        registeredEvents.push_back(eventId);
    }

    void rateEvent(int eventId, int rating) {
        eventRatings[eventId] = rating;
    }

    const vector<int>& getRegisteredEvents() const {
        return registeredEvents;
    }

    const map<int, int>& getEventRatings() const {
        return eventRatings;
    }

    static string trim(const string& str) {
        size_t first = str.find_first_not_of(' ');
        size_t last = str.find_last_not_of(' ');
        return (first == string::npos) ? "" : str.substr(first, last - first + 1);
    }
};

class Event {
protected:
    int id;
    string name;
    string date;
    int capacity;
    EventCategory category;
    vector<string> registeredUsers;
    vector<string> waitlist;
    vector<int> ratings;

public:
    static int nextId;

    Event(const string& n, const string& d, int c, EventCategory cat)
        : id(nextId++), name(n), date(d), capacity(c), category(cat) {}

    virtual void displayInfo() const {
        cout << "Event ID: " << id << "\nEvent: " << name 
             << "\nDate: " << date << "\nCapacity: " << capacity 
             << "\nCategory: " << getCategoryString(category);
        if (!ratings.empty()) {
            double avgRating = accumulate(ratings.begin(), ratings.end(), 0.0) / ratings.size();
            cout << "\nAverage Rating: " << fixed << setprecision(1) << avgRating;
        }
        cout << endl;
    }

    virtual bool register_participant(const string& username) {
        if (registeredUsers.size() < capacity) {
            registeredUsers.push_back(username);
            cout << "Participant registered successfully." << endl;
            return true;
        } else {
            waitlist.push_back(username);
            cout << "Event is full. You've been added to the waitlist." << endl;
            return false;
        }
    }

    void addRating(int rating) {
        ratings.push_back(rating);
    }

    static string getCategoryString(EventCategory cat) {
        switch (cat) {
            case EventCategory::ACADEMIC: return "Academic";
            case EventCategory::CULTURAL: return "Cultural";
            case EventCategory::TECHNICAL: return "Technical";
            case EventCategory::SPORTS: return "Sports";
            default: return "Unknown";
        }
    }

    string getName() const { return name; }
    string getDate() const { return date; }
    int getId() const { return id; }
    EventCategory getCategory() const { return category; }

    virtual ~Event() = default;
};

int Event::nextId = 1;

class Seminar : public Event {
private:
    string speaker;

public:
    Seminar(const string& n, const string& d, int c, const string& s)
        : Event(n, d, c, EventCategory::ACADEMIC), speaker(s) {}

    void displayInfo() const override {
        Event::displayInfo();
        cout << "Speaker: " << speaker << endl;
    }
};

class Workshop : public Event {
private:
    string topic;
    bool isOnline;

public:
    Workshop(const string& n, const string& d, int c, const string& t, bool online)
        : Event(n, d, c, EventCategory::TECHNICAL), topic(t), isOnline(online) {}

    void displayInfo() const override {
        Event::displayInfo();
        cout << "Topic: " << topic << "\nFormat: " << (isOnline ? "Online" : "Offline") << endl;
    }
};

class SportsEvent : public Event {
private:
    string sportType;

public:
    SportsEvent(const string& n, const string& d, int c, const string& s)
        : Event(n, d, c, EventCategory::SPORTS), sportType(s) {}

    void displayInfo() const override {
        Event::displayInfo();
        cout << "Sport Type: " << sportType << endl;
    }
};

class EventManager {
private:
    vector<unique_ptr<Event>> events;
    vector<User> users;

public:
    void addEvent(unique_ptr<Event> event) {
        events.push_back(move(event));
    }

    void displayAllEvents() const {
        for (const auto& event : events) {
            event->displayInfo();
            cout << "------------------------" << endl;
        }
    }

    bool registerParticipant(int eventId, const string& username) {
        auto it = find_if(events.begin(), events.end(),
            [eventId](const auto& event) { return event->getId() == eventId; });
        if (it != events.end()) {
            return (*it)->register_participant(username);
        } else {
            cout << "Invalid event ID." << endl;
            return false;
        }
    }

    void addUser(const User& user) {
        users.push_back(user);
    }

    User* authenticateUser(const string& username, const string& password) {
        auto it = find_if(users.begin(), users.end(),
            [&](const User& u) { return u.getUsername() == username && u.authenticate(password); });
        return (it != users.end()) ? &(*it) : nullptr;
    }

    void rateEvent(int eventId, int rating, User& user) {
        auto it = find_if(events.begin(), events.end(),
            [eventId](const auto& event) { return event->getId() == eventId; });
        if (it != events.end()) {
            (*it)->addRating(rating);
            user.rateEvent(eventId, rating);
            cout << "Thank you for rating the event!" << endl;
        } else {
            cout << "Invalid event ID." << endl;
        }
    }

    void searchEvents(const string& query) const {
        cout << "Search results for '" << query << "':" << endl;
        bool found = false;
        for (const auto& event : events) {
            if (event->getName().find(query) != string::npos ||
                event->getDate().find(query) != string::npos ||
                Event::getCategoryString(event->getCategory()).find(query) != string::npos) {
                event->displayInfo();
                cout << "------------------------" << endl;
                found = true;
            }
        }
        if (!found) {
            cout << "No events found matching the query." << endl;
        }
    }

    void displayUserProfile(const User& user) const {
        cout << "User Profile for " << user.getUsername() << endl;
        cout << "Registered Events:" << endl;
        for (int eventId : user.getRegisteredEvents()) {
            auto it = find_if(events.begin(), events.end(),
                [eventId](const auto& event) { return event->getId() == eventId; });
            if (it != events.end()) {
                cout << "- " << (*it)->getName() << " (ID: " << eventId << ")" << endl;
            }
        }
        cout << "Event Ratings:" << endl;
        for (const auto& [eventId, rating] : user.getEventRatings()) {
            auto it = find_if(events.begin(), events.end(),
                [eventId](const auto& event) { return event->getId() == eventId; });
            if (it != events.end()) {
                cout << "- " << (*it)->getName() << ": " << rating << "/5" << endl;
            }
        }
    }

    void addNewEvent() {
        string name, date;
        int capacity;
        string categoryInput;
        EventCategory category;

        cout << "Enter event name: ";
        cin.ignore();
        getline(cin, name);
        cout << "Enter event date (YYYY-MM-DD): ";
        getline(cin, date);
        cout << "Enter event capacity: ";
        cin >> capacity;

        cout << "Enter event category (Academic, Technical, Sports): ";
        cin >> categoryInput;

        if (categoryInput == "Academic") {
            category = EventCategory::ACADEMIC;
        } else if (categoryInput == "Technical") {
            category = EventCategory::TECHNICAL;
        } else if (categoryInput == "Sports") {
            category = EventCategory::SPORTS;
        } else {
            cout << "Invalid category. Event not added." << endl;
            return;
        }

        unique_ptr<Event> event;
        if (category == EventCategory::ACADEMIC) {
            string speaker;
            cout << "Enter speaker's name: ";
            cin.ignore();
            getline(cin, speaker);
            event = make_unique<Seminar>(name, date, capacity, speaker);
        } else if (category == EventCategory::TECHNICAL) {
            string topic;
            bool isOnline;
            cout << "Enter workshop topic: ";
            getline(cin, topic);
            cout << "Is it online? (1 for Yes, 0 for No): ";
            cin >> isOnline;
            event = make_unique<Workshop>(name, date, capacity, topic, isOnline);
        } else if (category == EventCategory::SPORTS) {
            string sportType;
            cout << "Enter sport type: ";
            cin.ignore();
            getline(cin, sportType);
            event = make_unique<SportsEvent>(name, date, capacity, sportType);
        }

        if (event) {
            addEvent(move(event));
            cout << "Event added successfully." << endl;
        }
    }
};

int main() {
    EventManager manager;

    // Add users with new credentials
    manager.addUser(User("sit", "pune1234", true));  // Admin user
    manager.addUser(User("lavale", "hillbase"));
    manager.addUser(User("209", "pass123"));

    // Add some events
    manager.addEvent(make_unique<Seminar>("Career in CyberSecurity and Digital Forensics", "2024-11-15", 100, "Mr. Nikhil Mahadeshwar"));
    manager.addEvent(make_unique<Workshop>("Web Development", "2024-11-01", 50, "Full-stack JavaScript", true));
    manager.addEvent(make_unique<SportsEvent>("Prota", "2024-08-20", 500, "All"));

    User* currentUser = nullptr;

    while (true) {
        if (!currentUser) {
            string username, password;
            cout << "Enter username: ";
            cin >> username;
            username = User::trim(username);
            cout << "Enter password: ";
            cin >> password;
            password = User::trim(password);

            currentUser = manager.authenticateUser(username, password);
            if (!currentUser) {
                cout << "Invalid credentials. Please try again." << endl;
                continue;
            }
        }

        int choice;
        cout << "\nCampus Event Management System" << endl;
        cout << "1. Display All Events" << endl;
        cout << "2. Register for an Event" << endl;
        cout << "3. Search Events" << endl;
        cout << "4. Rate an Event" << endl;
        cout << "5. View User Profile" << endl;
        if (currentUser->isAdminUser()) {
            cout << "6. Add New Event" << endl;
        }
        cout << "0. Logout" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                manager.displayAllEvents();
                break;
            case 2: {
                int eventId;
                cout << "Enter the event ID: ";
                cin >> eventId;
                if (manager.registerParticipant(eventId, currentUser->getUsername())) {
                    currentUser->registerForEvent(eventId);
                }
                break;
            }
            case 3: {
                string query;
                cout << "Enter search query: ";
                cin.ignore();
                getline(cin, query);
                manager.searchEvents(query);
                break;
            }
            case 4: {
                int eventId, rating;
                cout << "Enter the event ID: ";
                cin >> eventId;
                cout << "Enter your rating (1-5): ";
                cin >> rating;
                if (rating >= 1 && rating <= 5) {
                    manager.rateEvent(eventId, rating, *currentUser);
                } else {
                    cout << "Invalid rating. Please enter a number between 1 and 5." << endl;
                }
                break;
            }
            case 5:
                manager.displayUserProfile(*currentUser);
                break;
            case 6:
                if (currentUser->isAdminUser()) {
                    manager.addNewEvent();
                } else {
                    cout << "Invalid choice. Please try again." << endl;
                }
                break;
            case 0:
                currentUser = nullptr; // Logout
                cout << "Logged out successfully." << endl;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
    }

    return 0;
}