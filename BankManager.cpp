#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <conio.h> // For password masking (Windows)

using namespace std;

class BankAccount {
private:
    int accountNumber;
    string accountHolderName;
    string password;
    double balance;
    string accountType;
    string dateCreated;

public:
    // Constructor
    BankAccount(int accNo, string name, string pass, double bal, string type) {
        accountNumber = accNo;
        accountHolderName = name;
        password = pass;
        balance = bal;
        accountType = type;
        
        // Set current date
        time_t now = time(0);
        char* dt = ctime(&now);
        dateCreated = string(dt);
        dateCreated.pop_back(); // Remove newline
    }
    
    // Default constructor
    BankAccount() {
        accountNumber = 0;
        balance = 0.0;
    }
    
    // Getters
    int getAccountNumber() const { return accountNumber; }
    string getAccountHolderName() const { return accountHolderName; }
    double getBalance() const { return balance; }
    string getAccountType() const { return accountType; }
    string getDateCreated() const { return dateCreated; }
    
    // Password verification
    bool verifyPassword(string inputPassword) const {
        return password == inputPassword;
    }
    
    // Deposit money
    bool deposit(double amount) {
        if (amount > 0) {
            balance += amount;
            return true;
        }
        return false;
    }
    
    // Withdraw money
    bool withdraw(double amount) {
        if (amount > 0 && amount <= balance) {
            balance -= amount;
            return true;
        }
        return false;
    }
    
    // Convert account to string for file storage
    string toString() const {
        stringstream ss;
        ss << accountNumber << "," << accountHolderName << "," << password << "," 
           << balance << "," << accountType << "," << dateCreated;
        return ss.str();
    }
    
    // Create account from string (file loading)
    static BankAccount fromString(const string& data) {
        stringstream ss(data);
        string item;
        vector<string> tokens;
        
        while (getline(ss, item, ',')) {
            tokens.push_back(item);
        }
        
        if (tokens.size() >= 6) {
            BankAccount acc;
            acc.accountNumber = stoi(tokens[0]);
            acc.accountHolderName = tokens[1];
            acc.password = tokens[2];
            acc.balance = stod(tokens[3]);
            acc.accountType = tokens[4];
            acc.dateCreated = tokens[5];
            return acc;
        }
        return BankAccount();
    }
};

class TransactionLogger {
private:
    string logFileName;
    
public:
    TransactionLogger(string fileName = "transaction_log.csv") {
        logFileName = fileName;
        
        // Create header if file doesn't exist
        ifstream file(logFileName);
        if (!file.good()) {
            ofstream outFile(logFileName);
            outFile << "Date,Time,Account_Number,Transaction_Type,Amount,Balance_After,Description\n";
            outFile.close();
        }
        file.close();
    }
    
    void logTransaction(int accountNumber, string transactionType, double amount, 
                       double balanceAfter, string description = "") {
        ofstream file(logFileName, ios::app);
        if (file.is_open()) {
            time_t now = time(0);
            struct tm* timeinfo = localtime(&now);
            
            file << setfill('0') << setw(2) << timeinfo->tm_mday << "/"
                 << setw(2) << (timeinfo->tm_mon + 1) << "/"
                 << (timeinfo->tm_year + 1900) << ","
                 << setw(2) << timeinfo->tm_hour << ":"
                 << setw(2) << timeinfo->tm_min << ":"
                 << setw(2) << timeinfo->tm_sec << ","
                 << accountNumber << "," << transactionType << ","
                 << fixed << setprecision(2) << amount << ","
                 << balanceAfter << "," << description << "\n";
            file.close();
        }
    }
};

class BankManagementSystem {
private:
    vector<BankAccount> accounts;
    int nextAccountNumber;
    string accountsFileName;
    TransactionLogger logger;
    
    // Mask password input
    string getPasswordInput() {
        string password = "";
        char ch;
        cout << "Enter Password: ";
        while ((ch = _getch()) != '\r') { // '\r' is Enter key
            if (ch == '\b') { // Backspace
                if (!password.empty()) {
                    password.pop_back();
                    cout << "\b \b";
                }
            } else {
                password += ch;
                cout << '*';
            }
        }
        cout << endl;
        return password;
    }
    
public:
    BankManagementSystem(string fileName = "bank_accounts.txt") {
        accountsFileName = fileName;
        nextAccountNumber = 1001; // Starting account number
        loadAccountsFromFile();
    }
    
    // Generate next account number
    int generateAccountNumber() {
        if (!accounts.empty()) {
            int maxAccNo = 0;
            for (const auto& acc : accounts) {
                if (acc.getAccountNumber() > maxAccNo) {
                    maxAccNo = acc.getAccountNumber();
                }
            }
            nextAccountNumber = maxAccNo + 1;
        }
        return nextAccountNumber++;
    }
    
    // Create new account
    void createAccount() {
        string name, password, accountType;
        double initialDeposit;
        
        cout << "\n=== CREATE NEW ACCOUNT ===\n";
        cout << "Enter Account Holder Name: ";
        cin.ignore();
        getline(cin, name);
        
        password = getPasswordInput();
        
        cout << "Select Account Type:\n";
        cout << "1. Savings\n2. Current\n";
        cout << "Choice: ";
        int choice;
        cin >> choice;
        
        accountType = (choice == 1) ? "Savings" : "Current";
        
        cout << "Enter Initial Deposit (Min Rs. 500): Rs. ";
        cin >> initialDeposit;
        
        if (initialDeposit < 500) {
            cout << "❌ Minimum deposit of Rs. 500 required!\n";
            return;
        }
        
        int newAccountNumber = generateAccountNumber();
        BankAccount newAccount(newAccountNumber, name, password, initialDeposit, accountType);
        accounts.push_back(newAccount);
        
        // Log account creation
        logger.logTransaction(newAccountNumber, "ACCOUNT_CREATED", initialDeposit, 
                             initialDeposit, "Account opened with initial deposit");
        
        saveAccountsToFile();
        
        cout << "\n✅ Account Created Successfully!\n";
        cout << "Your Account Number: " << newAccountNumber << "\n";
        cout << "Please remember your account number and password.\n";
    }
    
    // Find account by number
    BankAccount* findAccount(int accountNumber) {
        for (auto& account : accounts) {
            if (account.getAccountNumber() == accountNumber) {
                return &account;
            }
        }
        return nullptr;
    }
    
    // Authenticate user
    bool authenticateUser(int accountNumber, string password) {
        BankAccount* account = findAccount(accountNumber);
        if (account != nullptr) {
            return account->verifyPassword(password);
        }
        return false;
    }
    
    // Deposit money
    void depositMoney() {
        int accountNumber;
        double amount;
        string password;
        
        cout << "\n=== DEPOSIT MONEY ===\n";
        cout << "Enter Account Number: ";
        cin >> accountNumber;
        
        password = getPasswordInput();
        
        if (!authenticateUser(accountNumber, password)) {
            cout << "❌ Invalid Account Number or Password!\n";
            return;
        }
        
        BankAccount* account = findAccount(accountNumber);
        
        cout << "Enter Deposit Amount: Rs. ";
        cin >> amount;
        
        if (account->deposit(amount)) {
            logger.logTransaction(accountNumber, "DEPOSIT", amount, 
                                 account->getBalance(), "Cash deposit");
            saveAccountsToFile();
            cout << "✅ Rs. " << fixed << setprecision(2) << amount 
                 << " deposited successfully!\n";
            cout << "Current Balance: Rs. " << account->getBalance() << "\n";
        } else {
            cout << "❌ Invalid deposit amount!\n";
        }
    }
    
    // Withdraw money
    void withdrawMoney() {
        int accountNumber;
        double amount;
        string password;
        
        cout << "\n=== WITHDRAW MONEY ===\n";
        cout << "Enter Account Number: ";
        cin >> accountNumber;
        
        password = getPasswordInput();
        
        if (!authenticateUser(accountNumber, password)) {
            cout << "❌ Invalid Account Number or Password!\n";
            return;
        }
        
        BankAccount* account = findAccount(accountNumber);
        
        cout << "Current Balance: Rs. " << account->getBalance() << "\n";
        cout << "Enter Withdrawal Amount: Rs. ";
        cin >> amount;
        
        if (account->withdraw(amount)) {
            logger.logTransaction(accountNumber, "WITHDRAWAL", amount, 
                                 account->getBalance(), "Cash withdrawal");
            saveAccountsToFile();
            cout << "✅ Rs. " << fixed << setprecision(2) << amount 
                 << " withdrawn successfully!\n";
            cout << "Remaining Balance: Rs. " << account->getBalance() << "\n";
        } else {
            cout << "❌ Insufficient balance or invalid amount!\n";
        }
    }
    
    // View account details
    void viewAccountDetails() {
        int accountNumber;
        string password;
        
        cout << "\n=== ACCOUNT DETAILS ===\n";
        cout << "Enter Account Number: ";
        cin >> accountNumber;
        
        password = getPasswordInput();
        
        if (!authenticateUser(accountNumber, password)) {
            cout << "❌ Invalid Account Number or Password!\n";
            return;
        }
        
        BankAccount* account = findAccount(accountNumber);
        
        cout << "\n📋 Account Information:\n";
        cout << "Account Number: " << account->getAccountNumber() << "\n";
        cout << "Account Holder: " << account->getAccountHolderName() << "\n";
        cout << "Account Type: " << account->getAccountType() << "\n";
        cout << "Current Balance: Rs. " << fixed << setprecision(2) 
             << account->getBalance() << "\n";
        cout << "Date Created: " << account->getDateCreated() << "\n";
    }
    
    // Transfer money between accounts
    void transferMoney() {
        int fromAccount, toAccount;
        double amount;
        string password;
        
        cout << "\n=== MONEY TRANSFER ===\n";
        cout << "Enter Your Account Number: ";
        cin >> fromAccount;
        
        password = getPasswordInput();
        
        if (!authenticateUser(fromAccount, password)) {
            cout << "❌ Invalid Account Number or Password!\n";
            return;
        }
        
        cout << "Enter Recipient Account Number: ";
        cin >> toAccount;
        
        BankAccount* senderAccount = findAccount(fromAccount);
        BankAccount* receiverAccount = findAccount(toAccount);
        
        if (receiverAccount == nullptr) {
            cout << "❌ Recipient account not found!\n";
            return;
        }
        
        cout << "Your Balance: Rs. " << senderAccount->getBalance() << "\n";
        cout << "Enter Transfer Amount: Rs. ";
        cin >> amount;
        
        if (senderAccount->withdraw(amount)) {
            receiverAccount->deposit(amount);
            
            // Log both transactions
            logger.logTransaction(fromAccount, "TRANSFER_OUT", amount, 
                                 senderAccount->getBalance(), 
                                 "Transfer to Acc: " + to_string(toAccount));
            logger.logTransaction(toAccount, "TRANSFER_IN", amount, 
                                 receiverAccount->getBalance(), 
                                 "Transfer from Acc: " + to_string(fromAccount));
            
            saveAccountsToFile();
            cout << "✅ Rs. " << fixed << setprecision(2) << amount 
                 << " transferred successfully!\n";
            cout << "Your Remaining Balance: Rs. " << senderAccount->getBalance() << "\n";
        } else {
            cout << "❌ Insufficient balance or invalid amount!\n";
        }
    }
    
    // Save accounts to file
    void saveAccountsToFile() {
        ofstream file(accountsFileName);
        if (file.is_open()) {
            for (const auto& account : accounts) {
                file << account.toString() << "\n";
            }
            file.close();
        }
    }
    
    // Load accounts from file
    void loadAccountsFromFile() {
        ifstream file(accountsFileName);
        if (file.is_open()) {
            string line;
            accounts.clear();
            while (getline(file, line)) {
                if (!line.empty()) {
                    BankAccount account = BankAccount::fromString(line);
                    if (account.getAccountNumber() != 0) {
                        accounts.push_back(account);
                    }
                }
            }
            file.close();
        }
    }
    
    // Display main menu
    void displayMenu() {
        cout << "\n🏦 ===== BANK MANAGEMENT SYSTEM ===== 🏦\n";
        cout << "1. 🆕 Create Account\n";
        cout << "2. 💰 Deposit Money\n";
        cout << "3. 💸 Withdraw Money\n";
        cout << "4. 📋 View Account Details\n";
        cout << "5. 🔄 Transfer Money\n";
        cout << "6. 📊 View Transaction Log\n";
        cout << "7. 👥 All Accounts (Admin)\n";
        cout << "8. 🚪 Exit\n";
        cout << "========================================\n";
        cout << "Enter your choice: ";
    }
    
    // View transaction log
    void viewTransactionLog() {
        cout << "\n📊 Recent Transactions (Last 10):\n";
        cout << "=====================================\n";
        
        ifstream file("transaction_log.csv");
        if (file.is_open()) {
            vector<string> lines;
            string line;
            
            // Read all lines
            while (getline(file, line)) {
                lines.push_back(line);
            }
            file.close();
            
            // Display header
            if (!lines.empty()) {
                cout << "Date\t\tTime\t\tAcc No\tType\t\tAmount\t\tBalance\n";
                cout << "------------------------------------------------------------------------\n";
                
                // Display last 10 transactions (excluding header)
                int start = max(1, (int)lines.size() - 10);
                for (int i = start; i < lines.size(); i++) {
                    stringstream ss(lines[i]);
                    string date, time, accNo, type, amount, balance, desc;
                    
                    getline(ss, date, ',');
                    getline(ss, time, ',');
                    getline(ss, accNo, ',');
                    getline(ss, type, ',');
                    getline(ss, amount, ',');
                    getline(ss, balance, ',');
                    getline(ss, desc);
                    
                    cout << date << "\t" << time << "\t" << accNo << "\t" 
                         << type << "\t\t" << amount << "\t\t" << balance << "\n";
                }
            }
        } else {
            cout << "No transaction log found!\n";
        }
    }
    
    // Admin function to view all accounts
    void viewAllAccounts() {
        string adminPassword;
        cout << "\n🔐 Admin Access Required\n";
        cout << "Enter Admin Password: ";
        cin >> adminPassword;
        
        if (adminPassword != "admin123") {
            cout << "❌ Access Denied!\n";
            return;
        }
        
        cout << "\n👥 All Bank Accounts:\n";
        cout << "=====================\n";
        
        if (accounts.empty()) {
            cout << "No accounts found!\n";
            return;
        }
        
        cout << "Acc No\t\tName\t\t\tType\t\tBalance\n";
        cout << "--------------------------------------------------------\n";
        
        for (const auto& account : accounts) {
            cout << account.getAccountNumber() << "\t\t" 
                 << account.getAccountHolderName() << "\t\t\t"
                 << account.getAccountType() << "\t\t"
                 << "Rs. " << fixed << setprecision(2) << account.getBalance() << "\n";
        }
        
        cout << "\nTotal Accounts: " << accounts.size() << "\n";
    }
    
    // Main system loop
    void run() {
        int choice;
        
        cout << "🚀 Welcome to Advanced Bank Management System! 🚀\n";
        cout << "System loaded with " << accounts.size() << " existing accounts.\n";
        
        do {
            displayMenu();
            cin >> choice;
            
            switch (choice) {
                case 1:
                    createAccount();
                    break;
                case 2:
                    depositMoney();
                    break;
                case 3:
                    withdrawMoney();
                    break;
                case 4:
                    viewAccountDetails();
                    break;
                case 5:
                    transferMoney();
                    break;
                case 6:
                    viewTransactionLog();
                    break;
                case 7:
                    viewAllAccounts();
                    break;
                case 8:
                    cout << "\n💾 Saving data and exiting...\n";
                    saveAccountsToFile();
                    cout << "✅ Thank you for using Bank Management System!\n";
                    break;
                default:
                    cout << "❌ Invalid choice! Please try again.\n";
            }
            
            if (choice != 8) {
                cout << "\nPress Enter to continue...";
                cin.ignore();
                cin.get();
            }
            
        } while (choice != 8);
    }
};

// Main function
int main() {
    try {
        BankManagementSystem bank;
        bank.run();
    } catch (const exception& e) {
        cout << "❌ System Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}