#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <iomanip>
#include <ctime>
#include <algorithm>

using namespace std;

// Function for encryption
string encrypt(const string& password) {
    string encrypted = password;
    for (char& c : encrypted) {
        c = c + 3; // Shift each character by 3
    }
    return encrypted;
}

// Function for decryption
string decrypt(const string& encryptedPassword) {
    string decrypted = encryptedPassword;
    for (char& c : decrypted) {
        c = c - 3; // Reverse the shift by 3
    }
    return decrypted;
}

class Task {
public:
    string description;
    string deadline;
    bool isComplete;

    Task(string desc, string date) : description(desc), deadline(date), isComplete(false) {}
};

class ExpenseTracker {
private:
    string password; // Stores plain-text password temporarily during setup
    string encryptedKey; // Stores the encrypted key
    float overallBudget;
    unordered_map<string, float> expenses; // Track expenses by category
    vector<Task> tasks; // List of tasks for to-do list

public:
    void run() {
        cout << "Welcome to the Expense Tracker for Hostelers!\n";
        cout << "Do you have an account? (yes/no): ";
        string response;
        cin >> response;

        if (response == "yes") {
            login();
        } else {
            setupAccount();
        }
    }

private:
    void setupAccount() {
        cout << "Set a password: ";
        cin >> password;

        encryptedKey = encrypt(password);
        cout << "Account setup complete. Your encrypted key is: " << encryptedKey << "\n";

        cout << "Enter your overall budget for the month: ";
        cin >> overallBudget;
        cout << "Setup successful! You can now log in using your encrypted key.\n";
    }

    void login() {
        cout << "Enter your encrypted key: ";
        string inputKey;
        cin >> inputKey;

        if (inputKey == encryptedKey) {
            cout << "Login successful!\n";
            showMenu();
        } else {
            cout << "Invalid key. Access denied.\n";
        }
    }

    void showMenu() {
        int choice;
        do {
            cout << "\nMenu:\n";
            cout << "1. View Budget\n";
            cout << "2. Logout\n";
            cout << "Enter your choice: ";
            cin >> choice;

            switch (choice) {
                case 1:
                    viewBudget();
                    break;
                case 2:
                    cout << "Logging out...\n";
                    break;
                default:
                    cout << "Invalid choice. Please try again.\n";
            }
        } while (choice != 2);
    }

    void viewBudget() {
        cout << "Your overall budget is: $" << overallBudget << "\n";
    }

    // Menu for hosteler
    void hostelerMenu() {
        int choice;
        do {
            cout << "\nHosteler Menu:\n";
            cout << "1. Track Expenses\n";
            cout << "2. View Savings\n";
            cout << "3. Generate Receipt\n";
            cout << "4. Logout\n";
            cout << "Enter your choice: ";
            cin >> choice;

            switch (choice) {
                case 1:
                    trackExpenses();
                    break;
                case 2:
                    viewSavings();
                    break;
                case 3:
                    generateExpenseReceipt(overallBudget - calculateTotalExpenses());
                    break;
                case 4:
                    cout << "Logging out...\n";
                    break;
                default:
                    cout << "Invalid choice. Please try again.\n";
            }
        } while (choice != 4);
    }

    // Track expenses
    void trackExpenses() {
        string category;
        float amount;

        cout << "\n--- Expense Tracker ---\n";
        while (true) {
            cout << "Enter expense category (or type 'exit' to finish): ";
            cin >> category;

            if (category == "exit") {
                break;
            }

            cout << "Enter amount for " << category << ": ";
            cin >> amount;

            expenses[category] += amount;
        }
    }

    // Calculate total expenses
    float calculateTotalExpenses() {
        float total = 0;
        for (const auto& entry : expenses) {
            total += entry.second;
        }
        return total;
    }

    // View savings based on budget and expenses
    void viewSavings() {
        float savings = overallBudget - calculateTotalExpenses();
        cout << "\nYou have saved: $" << fixed << setprecision(2) << savings << "\n";
    }

    // Generate receipt
    void generateExpenseReceipt(float savings) {
        cout << "\n--- Expense Receipt ---\n";
        for (const auto& entry : expenses) {
            cout << entry.first << ": $" << fixed << setprecision(2) << entry.second << "\n";
        }
        cout << "Total Savings: $" << fixed << setprecision(2) << savings << "\n";
        cout << "=======================\n";
    }

    // Manage to-do list
    void manageToDoList() {
        int choice;
        string description, deadline;
        int taskNumber;

        do {
            cout << "\nTo-Do List Menu:\n";
            cout << "1. Add Task\n";
            cout << "2. View Tasks\n";
            cout << "3. Mark Task Complete\n";
            cout << "4. Exit To-Do Menu\n";
            cout << "Enter your choice: ";
            cin >> choice;
            cin.ignore(); // Clear input buffer

            switch (choice) {
                case 1:
                    cout << "Enter task description: ";
                    getline(cin, description);
                    cout << "Enter task deadline (e.g., YYYY-MM-DD): ";
                    getline(cin, deadline);
                    tasks.push_back(Task(description, deadline));
                    cout << "Task added successfully.\n";
                    break;
                case 2:
                    viewTasks();
                    break;
                case 3:
                    cout << "Enter task number to mark as complete: ";
                    cin >> taskNumber;
                    markTaskComplete(taskNumber);
                    break;
                case 4:
                    cout << "Exiting To-Do Menu.\n";
                    break;
                default:
                    cout << "Invalid choice. Please try again.\n";
            }
        } while (choice != 4);
    }

    // View tasks
    void viewTasks() {
        if (tasks.empty()) {
            cout << "No tasks in the list.\n";
            return;
        }

        cout << "\n--- To-Do List ---\n";
        for (size_t i = 0; i < tasks.size(); ++i) {
            cout << (i + 1) << ". [" << (tasks[i].isComplete ? "X" : " ") << "] "
                 << tasks[i].description << " (Deadline: " << tasks[i].deadline << ")\n";
        }
    }

    // Mark task as complete
    void markTaskComplete(int index) {
        if (index < 1 || index > tasks.size()) {
            cout << "Invalid task number.\n";
            return;
        }

        tasks[index - 1].isComplete = true;
        cout << "Task marked as complete: " << tasks[index - 1].description << "\n";
    }
};

int main() {
    ExpenseTracker app; // Changed from ExpenseAndToDoApp to ExpenseTracker
    app.run();
    return 0;
}
