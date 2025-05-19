#include "bank_common.h"

// Function to display the main menu
void displayMainMenu() {
    printf("\n===== BANK MANAGEMENT SYSTEM =====\n");
    printf("1. Open Account\n");
    printf("2. Close Account\n");
    printf("3. Withdraw\n");
    printf("4. Deposit\n");
    printf("5. Check Balance\n");
    printf("6. Get Statement\n");
    printf("0. Exit\n");
    printf("Enter your choice: ");
}

// Function to open an account
void openAccount(int sockfd) {
    Request request;
    memset(&request, 0, sizeof(request));
    request.type = OPEN_ACCOUNT;
    
    printf("\n===== OPEN ACCOUNT =====\n");
    
    printf("Enter name: ");
    scanf(" %[^\n]", request.name);
    
    printf("Enter national ID: ");
    scanf(" %[^\n]", request.nationalID);
    
    int accountTypeChoice;
    printf("Select account type (1 for Savings, 2 for Checking): ");
    scanf("%d", &accountTypeChoice);
    request.accountType = (accountTypeChoice == 1) ? SAVINGS : CHECKING;
    
    printf("Enter initial deposit amount (minimum %.2f): ", (double)MIN_BALANCE);
    scanf("%lf", &request.amount);
    
    // Send request to server
    send(sockfd, &request, sizeof(request), 0);
    
    // Receive response from server
    Response response;
    recv(sockfd, &response, sizeof(response), 0);
    
    printf("\n%s\n", response.message);
}

// Function to close an account
void closeAccount(int sockfd) {
    Request request;
    memset(&request, 0, sizeof(request));
    request.type = CLOSE_ACCOUNT;
    
    printf("\n===== CLOSE ACCOUNT =====\n");
    
    printf("Enter account number: ");
    scanf(" %[^\n]", request.accountNumber);
    
    printf("Enter PIN: ");
    scanf(" %[^\n]", request.pin);
    
    // Send request to server
    send(sockfd, &request, sizeof(request), 0);
    
    // Receive response from server
    Response response;
    recv(sockfd, &response, sizeof(response), 0);
    
    printf("\n%s\n", response.message);
}

// Function to withdraw money
void withdraw(int sockfd) {
    Request request;
    memset(&request, 0, sizeof(request));
    request.type = WITHDRAW;
    
    printf("\n===== WITHDRAW =====\n");
    
    printf("Enter account number: ");
    scanf(" %[^\n]", request.accountNumber);
    
    printf("Enter PIN: ");
    scanf(" %[^\n]", request.pin);
    
    printf("Enter withdrawal amount (minimum %.2f, in units of %.2f): ", 
           (double)MIN_TRANSACTION, (double)MIN_TRANSACTION);
    scanf("%lf", &request.amount);
    
    // Send request to server
    send(sockfd, &request, sizeof(request), 0);
    
    // Receive response from server
    Response response;
    recv(sockfd, &response, sizeof(response), 0);
    
    printf("\n%s\n", response.message);
}

// Function to deposit money
void deposit(int sockfd) {
    Request request;
    memset(&request, 0, sizeof(request));
    request.type = DEPOSIT;
    
    printf("\n===== DEPOSIT =====\n");
    
    printf("Enter account number: ");
    scanf(" %[^\n]", request.accountNumber);
    
    printf("Enter PIN: ");
    scanf(" %[^\n]", request.pin);
    
    printf("Enter deposit amount (minimum %.2f): ", (double)MIN_TRANSACTION);
    scanf("%lf", &request.amount);
    
    // Send request to server
    send(sockfd, &request, sizeof(request), 0);
    
    // Receive response from server
    Response response;
    recv(sockfd, &response, sizeof(response), 0);
    
    printf("\n%s\n", response.message);
}

// Function to check balance
void checkBalance(int sockfd) {
    Request request;
    memset(&request, 0, sizeof(request));
    request.type = CHECK_BALANCE;
    
    printf("\n===== CHECK BALANCE =====\n");
    
    printf("Enter account number: ");
    scanf(" %[^\n]", request.accountNumber);
    
    printf("Enter PIN: ");
    scanf(" %[^\n]", request.pin);
    
    // Send request to server
    send(sockfd, &request, sizeof(request), 0);
    
    // Receive response from server
    Response response;
    recv(sockfd, &response, sizeof(response), 0);
    
    printf("\n%s\n", response.message);
}

// Function to get account statement
void getStatement(int sockfd) {
    Request request;
    memset(&request, 0, sizeof(request));
    request.type = GET_STATEMENT;
    
    printf("\n===== ACCOUNT STATEMENT =====\n");
    
    printf("Enter account number: ");
    scanf(" %[^\n]", request.accountNumber);
    
    printf("Enter PIN: ");
    scanf(" %[^\n]", request.pin);
    
    // Send request to server
    send(sockfd, &request, sizeof(request), 0);
    
    // Receive response from server
    Response response;
    recv(sockfd, &response, sizeof(response), 0);
    
    if (response.success) {
        printf("\nAccount Statement\n");
        printf("Current Balance: %.2f\n\n", response.balance);
        printf("Last %d Transactions:\n", response.transactionCount);
        
        if (response.transactionCount == 0) {
            printf("No transactions found.\n");
        } else {
            printf("%-20s %-12s %-10s %s\n", "Date", "Type", "Amount", "Description");
            printf("------------------------------------------------------------------\n");
            
            for (int i = 0; i < response.transactionCount; i++) {
                Transaction *txn = &response.transactions[i];
                char dateStr[20];
                strftime(dateStr, sizeof(dateStr), "%Y-%m-%d %H:%M:%S", localtime(&txn->timestamp));
                
                printf("%-20s %-12s %-10.2f %s\n", 
                       dateStr, 
                       getTransactionTypeString(txn->type), 
                       txn->amount,
                       txn->description);
            }
        }
    } else {
        printf("\n%s\n", response.message);
    }
}

int main() {
    int sockfd;
    struct sockaddr_in serv_addr;
    
    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        return -1;
    }
    
    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        return -1;
    }
    
    printf("Connected to bank server.\n");
    
    int choice;
    do {
        displayMainMenu();
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                openAccount(sockfd);
                break;
            case 2:
                closeAccount(sockfd);
                break;
            case 3:
                withdraw(sockfd);
                break;
            case 4:
                deposit(sockfd);
                break;
            case 5:
                checkBalance(sockfd);
                break;
            case 6:
                getStatement(sockfd);
                break;
            case 0:
                printf("Thank you for using our banking system. Goodbye!\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 0);
    
    close(sockfd);
    return 0;
}
