// Common definitions for both client and server

#ifndef BANK_COMMON_H
#define BANK_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_BUFFER 1024
#define MAX_ACCOUNTS 100
#define MIN_BALANCE 1000
#define MIN_TRANSACTION 500
#define MAX_TRANSACTIONS 100
#define MAX_NAME_LENGTH 50
#define ID_LENGTH 20
#define PIN_LENGTH 6
#define ACC_NUM_LENGTH 10
#define MAX_TRANSACTIONS_IN_STATEMENT 5

// Account type enum
typedef enum
{
    SAVINGS,
    CHECKING
} AccountType;

// Transaction type enum
typedef enum
{
    DEPOSIT,
    WITHDRAWAL
} TransactionType;

// Transaction structure
typedef struct
{
    time_t timestamp;
    TransactionType type;
    double amount;
    char description[100];
} Transaction;

// Account structure
typedef struct
{
    char accountNumber[ACC_NUM_LENGTH + 1];
    char pin[PIN_LENGTH + 1];
    char name[MAX_NAME_LENGTH + 1];
    char nationalID[ID_LENGTH + 1];
    AccountType type;
    double balance;
    Transaction transactions[MAX_TRANSACTIONS];
    int transactionCount;
    int isActive;
} Account;

// Request type enum
typedef enum
{
    OPEN_ACCOUNT,
    CLOSE_ACCOUNT,
    WITHDRAW,
    DEPOSIT_FUNDS,
    CHECK_BALANCE,
    GET_STATEMENT,
    INVALID_REQUEST
} RequestType;

// Request structure
typedef struct
{
    RequestType type;
    char accountNumber[ACC_NUM_LENGTH + 1];
    char pin[PIN_LENGTH + 1];
    char name[MAX_NAME_LENGTH + 1];
    char nationalID[ID_LENGTH + 1];
    AccountType accountType;
    double amount;
} Request;

// Response structure
typedef struct
{
    int success;
    char message[MAX_BUFFER];
    char accountNumber[ACC_NUM_LENGTH + 1];
    char pin[PIN_LENGTH + 1];
    double balance;
    Transaction transactions[MAX_TRANSACTIONS_IN_STATEMENT];
    int transactionCount;
} Response;

// Helper functions
void generateAccountNumber(char *accountNumber)
{
    sprintf(accountNumber, "%010ld", rand() % 10000000000L);
}

void generatePIN(char *pin)
{
    sprintf(pin, "%06d", rand() % 1000000);
}

const char *getAccountTypeString(AccountType type)
{
    return type == SAVINGS ? "Savings" : "Checking";
}

const char *getTransactionTypeString(TransactionType type)
{
    return type == DEPOSIT ? "Deposit" : "Withdrawal";
}

#endif // BANK_COMMON_H