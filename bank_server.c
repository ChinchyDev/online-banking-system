#include "bank_common.h"
#include <asm-generic/socket.h>

Account accounts[MAX_ACCOUNTS];
int accountCount = 0;

// Function to add a transaction to an account
void addTransaction(Account *account, TransactionType type, double amount, const char *description)
{
    if (account->transactionCount >= MAX_TRANSACTIONS)
    {
        // Shift transactions to make room for the new one
        for (int i = 0; i < MAX_TRANSACTIONS - 1; i++)
        {
            account->transactions[i] = account->transactions[i + 1];
        }
        account->transactionCount = MAX_TRANSACTIONS - 1;
    }

    Transaction transaction;
    transaction.timestamp = time(NULL);
    transaction.type = type;
    transaction.amount = amount;
    strncpy(transaction.description, description, sizeof(transaction.description) - 1);
    transaction.description[sizeof(transaction.description) - 1] = '\0';

    account->transactions[account->transactionCount] = transaction;
    account->transactionCount++;
}

// Function to find an account by account number
Account *findAccount(const char *accountNumber)
{
    for (int i = 0; i < accountCount; i++)
    {
        if (strcmp(accounts[i].accountNumber, accountNumber) == 0 && accounts[i].isActive)
        {
            return &accounts[i];
        }
    }
    return NULL;
}

// Function to validate PIN
int validatePIN(const Account *account, const char *pin)
{
    return strcmp(account->pin, pin) == 0;
}

// Function to handle account opening
Response openAccount(const Request *request)
{
    Response response = {0};

    if (accountCount >= MAX_ACCOUNTS)
    {
        response.success = 0;
        strcpy(response.message, "Error: Maximum account limit reached.");
        return response;
    }

    if (request->amount < MIN_BALANCE)
    {
        response.success = 0;
        sprintf(response.message, "Error: Initial deposit must be at least %.2f.", (double)MIN_BALANCE);
        return response;
    }

    Account newAccount;
    strncpy(newAccount.name, request->name, MAX_NAME_LENGTH);
    newAccount.name[MAX_NAME_LENGTH] = '\0';

    strncpy(newAccount.nationalID, request->nationalID, ID_LENGTH);
    newAccount.nationalID[ID_LENGTH] = '\0';

    newAccount.type = request->accountType;
    newAccount.balance = request->amount;
    newAccount.transactionCount = 0;
    newAccount.isActive = 1;

    generateAccountNumber(newAccount.accountNumber);
    generatePIN(newAccount.pin);

    addTransaction(&newAccount, DEPOSIT, request->amount, "Initial deposit");

    accounts[accountCount] = newAccount;
    accountCount++;

    response.success = 1;
    strcpy(response.accountNumber, newAccount.accountNumber);
    strcpy(response.pin, newAccount.pin);
    sprintf(response.message, "Account created successfully!\nAccount Number: %s\nPIN: %s\nInitial Balance: %.2f",
            newAccount.accountNumber, newAccount.pin, newAccount.balance);

    return response;
}

// Function to handle account closing
Response closeAccount(const Request *request)
{
    Response response = {0};

    Account *account = findAccount(request->accountNumber);
    if (!account)
    {
        response.success = 0;
        strcpy(response.message, "Error: Account not found.");
        return response;
    }

    if (!validatePIN(account, request->pin))
    {
        response.success = 0;
        strcpy(response.message, "Error: Invalid PIN.");
        return response;
    }

    account->isActive = 0;

    response.success = 1;
    response.balance = account->balance;
    sprintf(response.message, "Account closed successfully. Remaining balance: %.2f", account->balance);

    return response;
}

// Function to handle withdrawals
Response withdraw(const Request *request)
{
    Response response = {0};

    if (request->amount < MIN_TRANSACTION)
    {
        response.success = 0;
        sprintf(response.message, "Error: Minimum withdrawal amount is %.2f.", (double)MIN_TRANSACTION);
        return response;
    }

    if ((int)request->amount % MIN_TRANSACTION != 0)
    {
        response.success = 0;
        sprintf(response.message, "Error: Withdrawal amount must be in units of %.2f.", (double)MIN_TRANSACTION);
        return response;
    }

    Account *account = findAccount(request->accountNumber);
    if (!account)
    {
        response.success = 0;
        strcpy(response.message, "Error: Account not found.");
        return response;
    }

    if (!validatePIN(account, request->pin))
    {
        response.success = 0;
        strcpy(response.message, "Error: Invalid PIN.");
        return response;
    }

    if (account->balance - request->amount < MIN_BALANCE)
    {
        response.success = 0;
        sprintf(response.message, "Error: Insufficient funds. Minimum balance of %.2f must be maintained.", (double)MIN_BALANCE);
        return response;
    }

    account->balance -= request->amount;
    addTransaction(account, WITHDRAWAL, request->amount, "Withdrawal");

    response.success = 1;
    response.balance = account->balance;
    sprintf(response.message, "Withdrawal successful. New balance: %.2f", account->balance);

    return response;
}

// Function to handle deposits
Response deposit(const Request *request)
{
    Response response = {0};

    if (request->amount < MIN_TRANSACTION)
    {
        response.success = 0;
        sprintf(response.message, "Error: Minimum deposit amount is %.2f.", (double)MIN_TRANSACTION);
        return response;
    }

    Account *account = findAccount(request->accountNumber);
    if (!account)
    {
        response.success = 0;
        strcpy(response.message, "Error: Account not found.");
        return response;
    }

    if (!validatePIN(account, request->pin))
    {
        response.success = 0;
        strcpy(response.message, "Error: Invalid PIN.");
        return response;
    }

    account->balance += request->amount;
    addTransaction(account, DEPOSIT, request->amount, "Deposit");

    response.success = 1;
    response.balance = account->balance;
    sprintf(response.message, "Deposit successful. New balance: %.2f", account->balance);

    return response;
}

// Function to check balance
Response checkBalance(const Request *request)
{
    Response response = {0};

    Account *account = findAccount(request->accountNumber);
    if (!account)
    {
        response.success = 0;
        strcpy(response.message, "Error: Account not found.");
        return response;
    }

    if (!validatePIN(account, request->pin))
    {
        response.success = 0;
        strcpy(response.message, "Error: Invalid PIN.");
        return response;
    }

    response.success = 1;
    response.balance = account->balance;
    sprintf(response.message, "Current balance: %.2f", account->balance);

    return response;
}

// Function to get account statement
Response getStatement(const Request *request)
{
    Response response = {0};

    Account *account = findAccount(request->accountNumber);
    if (!account)
    {
        response.success = 0;
        strcpy(response.message, "Error: Account not found.");
        return response;
    }

    if (!validatePIN(account, request->pin))
    {
        response.success = 0;
        strcpy(response.message, "Error: Invalid PIN.");
        return response;
    }

    response.success = 1;
    response.balance = account->balance;

    int start = account->transactionCount > MAX_TRANSACTIONS_IN_STATEMENT ? account->transactionCount - MAX_TRANSACTIONS_IN_STATEMENT : 0;

    response.transactionCount = account->transactionCount - start;

    for (int i = 0; i < response.transactionCount; i++)
    {
        response.transactions[i] = account->transactions[start + i];
    }

    strcpy(response.message, "Statement retrieved successfully.");

    return response;
}

// Process client request and generate response
Response processRequest(const Request *request)
{
    switch (request->type)
    {
    case OPEN_ACCOUNT:
        return openAccount(request);
    case CLOSE_ACCOUNT:
        return closeAccount(request);
    case WITHDRAW:
        return withdraw(request);
    case DEPOSIT_FUNDS:
        return deposit(request);
    case CHECK_BALANCE:
        return checkBalance(request);
    case GET_STATEMENT:
        return getStatement(request);
    default:
        Response response = {0};
        response.success = 0;
        strcpy(response.message, "Error: Invalid request type.");
        return response;
    }
}

int main()
{
    srand(time(NULL));

    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket to the address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_fd, 3) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Bank server started on port %d...\n", PORT);

    // Server main loop
    while (1)
    {
        printf("Waiting for connections...\n");

        // Accept a new connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("Accept failed");
            continue;
        }

        printf("Client connected\n");

        // Handle communication with the client
        while (1)
        {
            Request request;
            ssize_t bytes_received = recv(new_socket, &request, sizeof(request), 0);

            if (bytes_received <= 0)
            {
                printf("Client disconnected\n");
                break;
            }

            Response response = processRequest(&request);
            send(new_socket, &response, sizeof(response), 0);
        }

        close(new_socket);
    }

    close(server_fd);
    return 0;
}