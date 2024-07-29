#include <iostream>
#include <mysql.h>
#include <string>
#include <sstream>
#include <iomanip>

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

using namespace std;

// MySQL Connection settings
const char* HOST = "localhost";
const char* USER = "root";
const char* PASS = "yash@123";
const char* DB_NAME = "library_db";

MYSQL* conn;

// Function prototypes
bool connectToDB();
void disconnectFromDB();
void addBook();
void addMember();
void deleteBook();
void deleteMember();
void borrowBook();
void returnBook();
void viewBooks();
void viewMembers();
void viewTransactions();
void searchBooks();
void searchMembers();

// Book class
class Book {
public:
    int id;
    string title;
    string author;
    string isbn;
    int availableQuantity;

    Book(string title, string author, string isbn, int availableQuantity)
        : title(title), author(author), isbn(isbn), availableQuantity(availableQuantity) {}
};

// Member class
class Member {
public:
    int id;
    string name;
    string address;
    string phone;

    Member(string name, string address, string phone)
        : name(name), address(address), phone(phone) {}
};

// Transaction class
class Transaction {
public:
    int id;
    int memberId;
    int bookId;
    string borrowDate;
    string returnDate;

    Transaction(int memberId, int bookId, string borrowDate, string returnDate)
        : memberId(memberId), bookId(bookId), borrowDate(borrowDate), returnDate(returnDate) {}
};

int main() {
    if (!connectToDB()) {
        cout << "Failed to connect to MySQL." << endl;
        return 1;
    }

    int choice;
    do {
        cout << "\n=== Library Management System ===" << endl;
        cout << "1. Add Book" << endl;
        cout << "2. Add Member" << endl;
        cout << "3. Borrow Book" << endl;
        cout << "4. Return Book" << endl;
        cout << "5. View Books" << endl;
        cout << "6. View Members" << endl;
        cout << "7. View Transactions" << endl;
        cout << "8. Delete Book" << endl;
        cout << "9. Delete Member" << endl;
        cout << "10. Search Books" << endl;
        cout << "11. Search Members" << endl;
        cout << "0. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1:
                addBook();
                break;
            case 2:
                addMember();
                break;
            case 3:
                borrowBook();
                break;
            case 4:
                returnBook();
                break;
            case 5:
                viewBooks();
                break;
            case 6:
                viewMembers();
                break;
            case 7:
                viewTransactions();
                break;
            case 8:
                deleteBook();
                break;
            case 9:
                deleteMember();
                break;
            case 10:
                searchBooks();
                break;
            case 11:
                searchMembers();
                break;
            case 0:
                cout << "Exiting..." << endl;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }

    } while (choice != 0);

    disconnectFromDB();

    return 0;
}

bool connectToDB() {
    conn = mysql_init(NULL);
    if (conn == NULL) {
        cout << "Error initializing MySQL: " << mysql_error(conn) << endl;
        return false;
    }

    if (!mysql_real_connect(conn, HOST, USER, PASS, DB_NAME, 0, NULL, 0)) {
        cout << "Error connecting to MySQL: " << mysql_error(conn) << endl;
        return false;
    }

    cout << "Connected to MySQL database: " << DB_NAME << endl;
    return true;
}

void disconnectFromDB() {
    mysql_close(conn);
    cout << "Disconnected from MySQL." << endl;
}

void addBook() {
    string title, author, isbn;
    int availableQuantity;

    cout << "Enter book title: ";
    getline(cin, title);
    cout << "Enter author name: ";
    getline(cin, author);
    cout << "Enter ISBN: ";
    getline(cin, isbn);
    cout << "Enter available quantity: ";
    cin >> availableQuantity;
    cin.ignore();

    stringstream ss;
    ss << "INSERT INTO books (title, author, isbn, available_quantity) VALUES ('"
       << title << "', '" << author << "', '" << isbn << "', " << availableQuantity << ")";

    if (mysql_query(conn, ss.str().c_str())) {
        cout << "Error adding book: " << mysql_error(conn) << endl;
    } else {
        cout << "Book added successfully." << endl;
    }
}

void addMember() {
    string name, address, phone;

    cout << "Enter member name: ";
    getline(cin, name);
    cout << "Enter address: ";
    getline(cin, address);
    cout << "Enter phone number: ";
    getline(cin, phone);

    stringstream ss;
    ss << "INSERT INTO members (name, address, phone) VALUES ('"
       << name << "', '" << address << "', '" << phone << "')";

    if (mysql_query(conn, ss.str().c_str())) {
        cout << "Error adding member: " << mysql_error(conn) << endl;
    } else {
        cout << "Member added successfully." << endl;
    }
}

void borrowBook() {
    int memberId, bookId;
    string borrowDate, returnDate;

    cout << "Enter member ID: ";
    cin >> memberId;
    cout << "Enter book ID: ";
    cin >> bookId;
    cin.ignore();
    cout << "Enter borrow date (YYYY-MM-DD): ";
    getline(cin, borrowDate);
    cout << "Enter return date (YYYY-MM-DD): ";
    getline(cin, returnDate);

    stringstream ssUpdate;
    ssUpdate << "UPDATE books SET available_quantity = available_quantity - 1 WHERE id = " << bookId << " AND available_quantity > 0";
    
    if (mysql_query(conn, ssUpdate.str().c_str())) {
        cout << "Error updating book quantity: " << mysql_error(conn) << endl;
        return;
    }

    if (mysql_affected_rows(conn) == 0) {
        cout << "Book is not available for borrowing." << endl;
        return;
    }

    stringstream ss;
    ss << "INSERT INTO transactions (member_id, book_id, borrow_date, return_date) VALUES ("
       << memberId << ", " << bookId << ", '" << borrowDate << "', '" << returnDate << "')";

    if (mysql_query(conn, ss.str().c_str())) {
        cout << "Error borrowing book: " << mysql_error(conn) << endl;
    } else {
        cout << "Book borrowed successfully." << endl;
    }
}

void returnBook() {
    int transactionId, bookId;

    cout << "Enter transaction ID: ";
    cin >> transactionId;
    cin.ignore();

    stringstream ssFetch;
    ssFetch << "SELECT book_id FROM transactions WHERE id = " << transactionId;

    if (mysql_query(conn, ssFetch.str().c_str())) {
        cout << "Error fetching transaction details: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (res == NULL) {
        cout << "Error storing result: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_ROW row = mysql_fetch_row(res);
    if (row == NULL) {
        cout << "Invalid transaction ID." << endl;
        mysql_free_result(res);
        return;
    }

    bookId = atoi(row[0]);
    mysql_free_result(res);

    stringstream ssUpdateTransaction;
    ssUpdateTransaction << "UPDATE transactions SET return_date = NOW() WHERE id = " << transactionId;

    if (mysql_query(conn, ssUpdateTransaction.str().c_str())) {
        cout << "Error updating transaction: " << mysql_error(conn) << endl;
        return;
    }

    stringstream ssUpdateBook;
    ssUpdateBook << "UPDATE books SET available_quantity = available_quantity + 1 WHERE id = " << bookId;

    if (mysql_query(conn, ssUpdateBook.str().c_str())) {
        cout << "Error updating book quantity: " << mysql_error(conn) << endl;
    } else {
        cout << "Book returned successfully." << endl;
    }
}

	
    void deleteBook() {
    int bookId;

    cout << "Enter book ID to delete: ";
    cin >> bookId;
    cin.ignore();

    // Check if there are transactions referencing this book
    stringstream ssCheck;
    ssCheck << "SELECT COUNT(*) FROM transactions WHERE book_id = " << bookId;

    if (mysql_query(conn, ssCheck.str().c_str())) {
        cout << "Error checking transactions: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES* resCheck = mysql_store_result(conn);
    if (resCheck == NULL) {
        cout << "Error storing result: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_ROW rowCheck = mysql_fetch_row(resCheck);
    int transactionCount = atoi(rowCheck[0]);
    mysql_free_result(resCheck);

    if (transactionCount > 0) {
        cout << "Cannot delete the book. There are " << transactionCount << " transaction(s) associated with it." << endl;
        return;
    }

    // If no transactions, proceed with deleting the book
    stringstream ssDelete;
    ssDelete << "DELETE FROM books WHERE id = " << bookId;

    if (mysql_query(conn, ssDelete.str().c_str())) {
        cout << "Error deleting book: " << mysql_error(conn) << endl;
    } else {
        cout << "Book deleted successfully." << endl;
    }
}

void deleteMember() {
    int memberId;

    cout << "Enter member ID to delete: ";
    cin >> memberId;
    cin.ignore();

    // Check if there are transactions associated with this member
    stringstream ssCheck;
    ssCheck << "SELECT COUNT(*) FROM transactions WHERE member_id = " << memberId;

    if (mysql_query(conn, ssCheck.str().c_str())) {
        cout << "Error checking transactions: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES* resCheck = mysql_store_result(conn);
    if (resCheck == NULL) {
        cout << "Error storing result: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_ROW rowCheck = mysql_fetch_row(resCheck);
    int transactionCount = atoi(rowCheck[0]);
    mysql_free_result(resCheck);

    if (transactionCount > 0) {
        cout << "Cannot delete the member. There are " << transactionCount << " transaction(s) associated with them." << endl;
        return;
    }

    // If no transactions, proceed with deleting the member
    stringstream ssDelete;
    ssDelete << "DELETE FROM members WHERE id = " << memberId;

    if (mysql_query(conn, ssDelete.str().c_str())) {
        cout << "Error deleting member: " << mysql_error(conn) << endl;
    } else {
        cout << "Member deleted successfully." << endl;
    }
}


void viewBooks() {
    if (mysql_query(conn, "SELECT id, title, author, isbn, available_quantity FROM books")) {
        cout << "Error fetching books: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (res == NULL) {
        cout << "Error storing result: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_ROW row;
    cout << left << setw(10) << "ID" << setw(30) << "Title" << setw(20) << "Author" << setw(20) << "ISBN" << setw(10) << "Quantity" << endl;
    while ((row = mysql_fetch_row(res))) {
        cout << left << setw(10) << row[0] << setw(30) << row[1] << setw(20) << row[2] << setw(20) << row[3] << setw(10) << row[4] << endl;
    }

    mysql_free_result(res);
}

void viewMembers() {
    if (mysql_query(conn, "SELECT id, name, address, phone FROM members")) {
        cout << "Error fetching members: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (res == NULL) {
        cout << "Error storing result: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_ROW row;
    cout << left << setw(10) << "ID" << setw(30) << "Name" << setw(50) << "Address" << setw(20) << "Phone" << endl;
    while ((row = mysql_fetch_row(res))) {
        cout << left << setw(10) << row[0] << setw(30) << row[1] << setw(50) << row[2] << setw(20) << row[3] << endl;
    }

    mysql_free_result(res);
}

void viewTransactions() {
    if (mysql_query(conn, "SELECT id, member_id, book_id, borrow_date, return_date FROM transactions")) {
        cout << "Error fetching transactions: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (res == NULL) {
        cout << "Error storing result: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_ROW row;
    cout << left << setw(10) << "ID" << setw(10) << "Member ID" << setw(10) << "Book ID" << setw(20) << "Borrow Date" << setw(20) << "Return Date" << endl;
    while ((row = mysql_fetch_row(res))) {
        cout << left << setw(10) << row[0] << setw(10) << row[1] << setw(10) << row[2] << setw(20) << row[3] << setw(20) << (row[4] ? row[4] : "Not returned") << endl;
    }

    mysql_free_result(res);
}

void searchBooks() {
    string keyword;

    cout << "Enter book title keyword: ";
    getline(cin, keyword);

    stringstream ss;
    ss << "SELECT id, title, author, isbn, available_quantity FROM books WHERE title LIKE '%" << keyword << "%'";

    if (mysql_query(conn, ss.str().c_str())) {
        cout << "Error searching books: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (res == NULL) {
        cout << "Error storing result: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_ROW row;
    cout << left << setw(10) << "ID" << setw(30) << "Title" << setw(20) << "Author" << setw(20) << "ISBN" << setw(10) << "Quantity" << endl;
    while ((row = mysql_fetch_row(res))) {
        cout << left << setw(10) << row[0] << setw(30) << row[1] << setw(20) << row[2] << setw(20) << row[3] << setw(10) << row[4] << endl;
    }

    mysql_free_result(res);
}

void searchMembers() {
    string keyword;

    cout << "Enter member name keyword: ";
    getline(cin, keyword);

    stringstream ss;
    ss << "SELECT id, name, address, phone FROM members WHERE name LIKE '%" << keyword << "%'";

    if (mysql_query(conn, ss.str().c_str())) {
        cout << "Error searching members: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (res == NULL) {
        cout << "Error storing result: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_ROW row;
    cout << left << setw(10) << "ID" << setw(30) << "Name" << setw(50) << "Address" << setw(20) << "Phone" << endl;
    while ((row = mysql_fetch_row(res))) {
        cout << left << setw(10) << row[0] << setw(30) << row[1] << setw(50) << row[2] << setw(20) << row[3] << endl;
    }

    mysql_free_result(res);
}

