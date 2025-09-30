#include <iostream>
#include <iomanip>
#include <vector>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/err.h>

// Convert hex string to byte vector
std::vector<uint8_t> hexToBytes(const std::string& hex) {
    std::vector<uint8_t> bytes;
    for (size_t i = 0; i < hex.length(); i += 2)
        bytes.push_back(static_cast<uint8_t>(std::stoi(hex.substr(i, 2), nullptr, 16)));
    return bytes;
}

// Construct IV: 8-byte meter address + 8-byte access number repeated
std::vector<uint8_t> constructIV(const std::vector<uint8_t>& meterAddress, uint8_t accessNumber) {
    std::vector<uint8_t> iv = meterAddress;
    for (int i = 0; i < 8; ++i)
        iv.push_back(accessNumber);
    return iv;
}

// AES-128-CBC decryption
std::vector<uint8_t> decryptAES128CBC(const std::vector<uint8_t>& ciphertext,
                                      const std::vector<uint8_t>& key,
                                      const std::vector<uint8_t>& iv) {
    std::vector<uint8_t> plaintext(ciphertext.size());
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();

    EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, key.data(), iv.data());

    int len;
    int plaintext_len;
    EVP_DecryptUpdate(ctx, plaintext.data(), &len, ciphertext.data(), ciphertext.size());
    plaintext_len = len;

    EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len);
    plaintext_len += len;

    plaintext.resize(plaintext_len);
    EVP_CIPHER_CTX_free(ctx);
    return plaintext;
}

int main() {
    std::string keyHex = "4255794d3dccfd46953146e701b7db68";
    std::string telegramHex = "a144c5142785895070078c20607a9d00902537ca231fa2da5889be8df3673ec136aebfb80d4ce395ba98f6b3844a115e4be1b1c9f0a2d5ffbb92906aa388deaa82c929310e9e5c4c0922a784df89cf0ded833be8da996eb5885409b6c9867978dea24001d68c603408d758a1e2b91c42ebad86a9b9d287880083bb0702850574d7b51e9c209ed68e0374e9b01febfd92b4cb9410fdeaf7fb526b742dc9a8d0682653";

    std::vector<uint8_t> key = hexToBytes(keyHex);
    std::vector<uint8_t> telegram = hexToBytes(telegramHex);

    std::vector<uint8_t> meterAddress(telegram.begin() + 2, telegram.begin() + 10); // Adjust as needed
    uint8_t accessNumber = telegram[10]; // Adjust index based on actual telegram structure

    std::vector<uint8_t> iv = constructIV(meterAddress, accessNumber);
    std::vector<uint8_t> encryptedPayload(telegram.begin() + 11, telegram.end()); // Adjust start index

    std::vector<uint8_t> decrypted = decryptAES128CBC(encryptedPayload, key, iv);

    std::cout << "Decrypted Payload:\n";
    for (uint8_t byte : decrypted)
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte << " ";
    std::cout << std::endl;

    return 0;
}