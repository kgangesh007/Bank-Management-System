
#include <iostream>
#include <mysql.h>
#include <sstream>
#include <string>
#include <limits>
#include <cstdlib> // For rand() and srand()
#include <ctime>   // For time()

#define HOST "127.0.0.1"
#define USER "root"
#define PASS "1234567890@My" // Define your MySQL password here
#define DATABASE "bank"
#define PORT 3306

using namespace std;

MYSQL *conn;
MYSQL_RES *res_set;
MYSQL_ROW row;
stringstream stmt;
string query;

class bank
{
    string name;
    int balance;
    int pass;

public:
    int create_account(); // Change return type to int
    void check_balance();
    void delete_account();
    void transfer_money();
};


int bank::create_account() {
    // Generate a random 9-digit account number
    srand(time(0));
    int account_number = rand() % 9000000000 + 10000000000;
    
    // Clear input buffer
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    
    cout << "Enter your name: ";
    getline(cin, name);
    
    cout << "Add balance: ";
    cin >> balance;
    
    cout << "Enter Pass(Numbers Only): ";
    cin >> pass;
    
    stmt.str("");
    stmt << "INSERT INTO account(account_number, name, balance, pass) VALUES(" << account_number << ", '" << name << "', " << balance << ","<<pass<<")";
    query = stmt.str();
    
    if (mysql_query(conn, query.c_str()) == 0) {
        cout << "Account created successfully" << endl;
        return account_number; // Return the generated account number
    } else {
        cout << "Failed to create account" << endl;
        return -1; // Return -1 to indicate failure
    }
}

void bank::check_balance() {
    int account_number;
    cout << "Enter account number to check balance: ";
    cin >> account_number;
    
    stmt.str("");
    stmt << "SELECT balance FROM account WHERE account_number = " << account_number;
    query = stmt.str();
    
    if (mysql_query(conn, query.c_str()) == 0) {
        res_set = mysql_store_result(conn);
        if (res_set) {
            row = mysql_fetch_row(res_set);
            if (row) {
                cout << "Account balance: " << row[0] << endl;
            } else {
                cout << "Account not found" << endl;
            }
            mysql_free_result(res_set);
        } else {
            cout << "Failed to fetch result" << endl;
        }
    } else {
        cout << "Failed to query database" << endl;
    }
}

void bank::delete_account() {
    int account_number;
    cout << "Enter account number to delete: ";
    cin >> account_number;
    
    stmt.str("");
    stmt << "DELETE FROM account WHERE account_number = " << account_number;
    query = stmt.str();
    
    if (mysql_query(conn, query.c_str()) == 0) {
        cout << "Account deleted successfully" << endl;
    } else {
        cout << "Failed to delete account" << endl;
    }
}

void bank::transfer_money() {
    int sender_account, receiver_account;
    int amount;
    int password;

    cout << "Enter sender's account number: ";
    cin >> sender_account;

    cout << "Enter sender's password: ";
    cin >> password;

    // Check sender's password
    stmt.str("");
    stmt << "SELECT pass FROM account WHERE account_number = " << sender_account;
    query = stmt.str();

    if (mysql_query(conn, query.c_str()) != 0) {
        cout << "Failed to query sender's password" << endl;
        return;
    }

    res_set = mysql_store_result(conn);
    if (res_set == NULL) {
        cout << "Failed to get sender's password" << endl;
        return;
    }

    row = mysql_fetch_row(res_set);
    if (row == NULL) {
        cout << "Sender's account not found" << endl;
        mysql_free_result(res_set);
        return;
    }

    int correct_password = atoi(row[0]);
    mysql_free_result(res_set);

    if (password != correct_password) {
        cout << "Incorrect password" << endl;
        return;
    }

    cout << "Enter receiver's account number: ";
    cin >> receiver_account;

    cout << "Enter amount to transfer: ";
    cin >> amount;

    // Check sender's balance
    stmt.str("");
    stmt << "SELECT balance FROM account WHERE account_number = " << sender_account;
    query = stmt.str();

    if (mysql_query(conn, query.c_str()) != 0) {
        cout << "Failed to query sender's balance" << endl;
        return;
    }

    res_set = mysql_store_result(conn);
    if (res_set == NULL) {
        cout << "Failed to get sender's balance" << endl;
        return;
    }

    row = mysql_fetch_row(res_set);
    if (row == NULL) {
        cout << "Sender's account not found" << endl;
        mysql_free_result(res_set);
        return;
    }

    int sender_balance = atoi(row[0]);
    mysql_free_result(res_set);

    if (sender_balance < amount) {
        cout << "Insufficient balance in sender's account" << endl;
        return;
    }

    // Update sender's balance
    sender_balance -= amount;
    stmt.str("");
    stmt << "UPDATE account SET balance = " << sender_balance << " WHERE account_number = " << sender_account;
    query = stmt.str();

    if (mysql_query(conn, query.c_str()) != 0) {
        cout << "Failed to update sender's balance" << endl;
        return;
    }

    // Update receiver's balance
    stmt.str("");
    stmt << "SELECT balance FROM account WHERE account_number = " << receiver_account;
    query = stmt.str();

    if (mysql_query(conn, query.c_str()) != 0) {
        cout << "Failed to query receiver's balance" << endl;
        return;
    }

    res_set = mysql_store_result(conn);
    if (res_set == NULL) {
        cout << "Failed to get receiver's balance" << endl;
        return;
    }

    row = mysql_fetch_row(res_set);
    if (row == NULL) {
        cout << "Receiver's account not found" << endl;
        mysql_free_result(res_set);
        return;
    }

    int receiver_balance = atoi(row[0]);
    mysql_free_result(res_set);

    receiver_balance += amount;
    stmt.str("");
    stmt << "UPDATE account SET balance = " << receiver_balance << " WHERE account_number = " << receiver_account;
    query = stmt.str();

    if (mysql_query(conn, query.c_str()) != 0) {
        cout << "Failed to update receiver's balance" << endl;
        return;
    }

    cout << "Transaction completed successfully" << endl;


}

int main()
{
    conn = mysql_init(NULL);
    if (conn == NULL)
    {
        cout << "MySQL Initialization Failed" << endl;
        return 1;
    }
    conn = mysql_real_connect(conn, HOST, USER, PASS, DATABASE, PORT, NULL, 0);
    if (conn == NULL)
    {
        cout << "MySQL Connection Failed" << endl;
        return 1;
    }
    cout << "Connection Successful" << endl << endl;

    bank b;
    int choice;
    do
    {
        cout << "1. Create Account" << endl;
        cout << "2. Check Balance" << endl;
        cout << "3. Delete Account" << endl;
        cout << "4. Transfer Money" << endl;
        cout << "5. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        // Clear input buffer
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice)
        {
        case 1:
            int account_number;
            account_number = b.create_account();
            if (account_number != -1) {
                cout << "Generated Account Number: " << account_number << endl; // Display generated account number
            }
            break;
        case 2:
            b.check_balance();
            break;
        case 3:
            b.delete_account();
            break;
        case 4:
             b.transfer_money();
             break;

        case 5:
            cout << "Goodbye!" << endl;
            break;
        default:
            cout << "Invalid choice!" << endl;
            break;
        }
    } while (choice != 5);

    mysql_close(conn);
    return 0;
}
