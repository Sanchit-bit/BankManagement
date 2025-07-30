# Bank Management System in C++

A simple bank management system built in C++ for learning file handling, classes, and basic banking operations.

## Features

- Create new bank accounts
- Deposit and withdraw money
- Check account balance
- Transfer money between accounts
- Save account data to files
- Transaction history in CSV format
- Password protection for accounts

## How to Run

1. Compile the code:
   ```
   g++ bank_management.cpp -o bank
   ```

2. Run the program:
   ```
   ./bank
   ```

## What I Learned

- File handling in C++ (reading/writing data)
- Working with classes and objects
- CSV file creation and management
- Basic password security
- Menu-driven programming
- Error handling

## Files Created

- `bank_accounts.txt` - Stores all account information
- `transaction_log.csv` - Keeps track of all transactions

## How to Use

1. **Create Account**: Choose option 1, enter your details and initial deposit (minimum ₹500)
2. **Deposit Money**: Use your account number and password to add money
3. **Withdraw**: Take out money (if you have enough balance)
4. **Transfer**: Send money to another account
5. **View Details**: Check your account information

## Admin Access

- Use password `admin123` to view all accounts (option 7)

## Sample Menu

```
🏦 ===== BANK MANAGEMENT SYSTEM =====
1. 🆕 Create Account
2. 💰 Deposit Money
3. 💸 Withdraw Money
4. 📋 View Account Details
5. 🔄 Transfer Money
6. 📊 View Transaction Log
7. 👥 All Accounts (Admin)
8. 🚪 Exit
```

## Things to Improve

- Better password security
- More account types
- Interest calculation
- GUI interface
- Database instead of text files

## Note

This is a learning project for understanding C++ concepts like file I/O, classes, and basic data management. Not for actual banking use! 😄

---

**Made for learning C++ programming**
