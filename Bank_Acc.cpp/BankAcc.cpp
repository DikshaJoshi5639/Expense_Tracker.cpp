#include <iostream>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <string>
#include <vector>
#include <iomanip>

using namespace std;

const int AES_KEY_LENGTH = 256;
const int AES_BLOCK_SIZE = 16;

bool generateAESKey(unsigned char* key, unsigned char* iv) {
    if (!RAND_bytes(key, AES_KEY_LENGTH / 8)) {
        cerr << "Error generating AES key" << endl;
        return false;
    }

    if (!RAND_bytes(iv, AES_BLOCK_SIZE)) {
        cerr << "Error generating AES IV" << endl;
        return false;
    }

    return true;
}

bool encryptAES(const unsigned char* plaintext, int plaintext_len, const unsigned char* key, const unsigned char* iv, unsigned char* ciphertext, int& ciphertext_len) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        cerr << "Error creating cipher context" << endl;
        return false;
    }

    if (!EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv)) {
        cerr << "Error initializing encryption" << endl;
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    int len = 0;
    if (!EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len)) {
        cerr << "Error during encryption update" << endl;
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    ciphertext_len = len;

    if (!EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) {
        cerr << "Error during encryption final" << endl;
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    return true;
}

bool decryptAES(const unsigned char* ciphertext, int ciphertext_len, const unsigned char* key, const unsigned char* iv, unsigned char* plaintext, int& plaintext_len) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        cerr << "Error creating cipher context" << endl;
        return false;
    }

    if (!EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv)) {
        cerr << "Error initializing decryption" << endl;
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    int len = 0;
    if (!EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len)) {
        cerr << "Error during decryption update" << endl;
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    plaintext_len = len;

    if (!EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) {
        cerr << "Error during decryption final" << endl;
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    return true;
}

struct Transaction {
    string date;
    string description;
    double amount;
};

class BankAccount {
public:
    BankAccount(const string& accountNumber, const string& ownerName, double balance)
        : accountNumber(accountNumber), ownerName(ownerName), balance(balance) {}

    void addTransaction(const string& date, const string& description, double amount) {
        transactions.push_back({date, description, amount});
        balance += amount;
    }

    void viewTransactions() const {
        cout << "Transaction History for Account: " << accountNumber << endl;
        cout << "Date\t\tDescription\tAmount" << endl;
        for (const auto& transaction : transactions) {
            cout << transaction.date << "\t" << transaction.description << "\t" << fixed << setprecision(2) << transaction.amount << endl;
        }
        cout << "Current Balance: " << balance << endl;
    }

    string getAccountNumber() const { return accountNumber; }

private:
    string accountNumber;
    string ownerName;
    double balance;
    vector<Transaction> transactions;
};

class BankIntegration {
public:
    bool connectToBankAccount(const string& accountNumber) {
        cout << "Connecting to bank account: " << accountNumber << endl;
        return true;
    }

    void viewTransactionHistory(const BankAccount& account) {
        account.viewTransactions();
    }

    bool encryptAndStoreAccountInfo(const string& accountInfo, unsigned char* encryptedInfo, int& encryptedInfoLen, const unsigned char* key, const unsigned char* iv) {
        return encryptAES(reinterpret_cast<const unsigned char*>(accountInfo.c_str()), accountInfo.length(), key, iv, encryptedInfo, encryptedInfoLen);
    }

    bool decryptAndRetrieveAccountInfo(const unsigned char* encryptedInfo, int encryptedInfoLen, string& accountInfo, const unsigned char* key, const unsigned char* iv) {
        unsigned char decryptedInfo[128];
        int decryptedInfoLen;
        if (decryptAES(encryptedInfo, encryptedInfoLen, key, iv, decryptedInfo, decryptedInfoLen)) {
            decryptedInfo[decryptedInfoLen] = '\0';
            accountInfo = string(reinterpret_cast<char*>(decryptedInfo));
            return true;
        }
        return false;
    }
};

int main() {
    unsigned char key[AES_KEY_LENGTH / 8];
    unsigned char iv[AES_BLOCK_SIZE];

    if (!generateAESKey(key, iv)) {
        return 1;
    }
    BankAccount myAccount("1234567890", "John Doe", 5000.0);

    myAccount.addTransaction("2023-08-01", "Grocery Shopping", -150.75);
    myAccount.addTransaction("2023-08-03", "Salary Credit", 2000.00);
    myAccount.addTransaction("2023-08-05", "Restaurant", -50.25);

    BankIntegration bankIntegration;
    if (bankIntegration.connectToBankAccount(myAccount.getAccountNumber())) {
        bankIntegration.viewTransactionHistory(myAccount);
    }

    string accountInfo = "AccountNumber:1234567890;Owner:John Doe;Balance:5000.0";
    unsigned char encryptedInfo[128];
    int encryptedInfoLen;
    if (bankIntegration.encryptAndStoreAccountInfo(accountInfo, encryptedInfo, encryptedInfoLen, key, iv)) {
        cout << "Encrypted account info stored securely." << endl;
    }

    string retrievedAccountInfo;
    if (bankIntegration.decryptAndRetrieveAccountInfo(encryptedInfo, encryptedInfoLen, retrievedAccountInfo, key, iv)) {
        cout << "Retrieved Account Info: " << retrievedAccountInfo << endl;
    }

    return 0;
}
