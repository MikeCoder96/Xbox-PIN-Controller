#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cctype>

using namespace std;

// Function to validate PIN
bool isValidPIN(const string& pin) {
    if (pin.length() < 4 || pin.length() > 7)
        return false;

    for (char c : pin) {
        if (!isdigit(c))
            return false;
    }

    return true;
}

// Function to encrypt text using a custom PIN
string encryptText(const string& text, const string& pin) {
    string encryptedText = text;
    for (size_t i = 0; i < text.length(); ++i) {
        encryptedText[i] = text[i] ^ pin[i % pin.length()];
    }
    return encryptedText;
}

// Function to decrypt text using a custom PIN
string decryptText(const string& encryptedText, const string& pin) {
    return encryptText(encryptedText, pin); // XOR operation is its own inverse
}

int main() {
    // Text to be encrypted
    string text;
    cout << "Enter text to encrypt: ";
    getline(cin, text);

    // Custom PIN for encryption
    string pin;
    do {
        cout << "Enter custom PIN (4 to 7 digits only): ";
        getline(cin, pin);
    } while (!isValidPIN(pin));

    // Encrypt the text using the custom PIN
    string encryptedText = encryptText(text, pin);

    // Write the encrypted text to a file
    string filename;
    cout << "Enter filename to save encrypted text: ";
    getline(cin, filename);

    ofstream outFile(filename);
    if (outFile.is_open()) {
        outFile << encryptedText;
        outFile.close();
        cout << "Text encrypted and saved to file successfully." << endl;
    }
    else {
        cout << "Unable to open file." << endl;
        return 1;
    }

    // Decrypt the text using the custom PIN
    string decryptedText = decryptText(encryptedText, pin);
    cout << "Decrypted Text: " << decryptedText << endl;

    return 0;
}
