// ctap2-sign.cpp
// Create local signatures based on credentials registered with WebAuthn

#include <stdio.h>
#include <Windows.h>
#include <conio.h>
#include <webauthn.h>
#include <wincrypt.h>

HWND GetConsoleHwnd(void)
{
#define MY_BUFSIZE 1024 // Buffer size for console window titles.
    HWND hwndFound;         // This is what is returned to the caller.
    TCHAR pszNewWindowTitle[MY_BUFSIZE]; // Contains fabricated
                                        // WindowTitle.
    TCHAR pszOldWindowTitle[MY_BUFSIZE]; // Contains original
                                        // WindowTitle.

    // Fetch current window title.

    GetConsoleTitle(pszOldWindowTitle, MY_BUFSIZE);

    // Format a "unique" NewWindowTitle.

    wsprintf(pszNewWindowTitle, L"%llu/%u",
        GetTickCount64(),
        GetCurrentProcessId());

    // Change current window title.

    SetConsoleTitle(pszNewWindowTitle);

    // Ensure window title has been updated.

    Sleep(40);

    // Look for NewWindowTitle.

    hwndFound = FindWindow(NULL, pszNewWindowTitle);

    // Restore original window title.

    SetConsoleTitle(pszOldWindowTitle);

    return(hwndFound);
}

void printBinaryAsBase64Url(LPCWSTR header, DWORD binaryDataLength, PBYTE binaryData) {
    TCHAR resultString[1024];
    DWORD resultStringLength = 999;
    if (!CryptBinaryToStringW(binaryData, 
                              binaryDataLength,
                              CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF,
                              resultString,
                              &resultStringLength)) {
        wprintf(L"Base64 encoding failed\n");
        exit(3);
    }
    for (DWORD i = 0; i < resultStringLength; i++) {
        if (resultString[i] == L'/') {
            resultString[i] = L'_';
        } else if (resultString[i] == L'+') {
            resultString[i] = L'-';
        } else if (resultString[i] == L'=') {
            resultString[i] = 0;
        }
    }
    wprintf(L"%s: %s\n", header, resultString);
}

void decodeBase64Url(LPCWSTR base64UrlString, PDWORD outDataLength, PBYTE outData) {
    TCHAR base64[1000];
    lstrcpyW(base64, base64UrlString);
    int i = -1;
    while (base64[++i]) {
        if (base64[i] == L'_') {
            base64[i] = L'/';
        }
        else if (base64[i] == L'-') {
            base64[i] = L'+';
        }
    }
    *outDataLength = 999;
    if (!CryptStringToBinaryW(base64,
                              0,
                              CRYPT_STRING_BASE64,
                              outData,
                              outDataLength,
                              NULL,
                              NULL)) {
        wprintf(L"Base64 decoding failed\n");
        exit(3);
    }
}

void doFidoSign(DWORD clientDataLength,
                PBYTE clientData,
                LPCWSTR pwszRpId, 
                WEBAUTHN_CREDENTIALS credentials) {
    HWND hWnd = GetConsoleHwnd();
    wprintf(L"WebAuthn API Version: %u\n", WebAuthNGetApiVersionNumber());
    WEBAUTHN_CLIENT_DATA webAuthnClientData { 
        WEBAUTHN_CLIENT_DATA_CURRENT_VERSION,
        clientDataLength,
        clientData,
        WEBAUTHN_HASH_ALGORITHM_SHA_256
    };
    WEBAUTHN_EXTENSIONS extensions {
        // None
    };
    WEBAUTHN_AUTHENTICATOR_GET_ASSERTION_OPTIONS assertionOptions {
        WEBAUTHN_AUTHENTICATOR_GET_ASSERTION_OPTIONS_VERSION_2,
        120000, // 2 minute timeout
        credentials,
        extensions,
        WEBAUTHN_AUTHENTICATOR_ATTACHMENT_ANY,
        WEBAUTHN_USER_VERIFICATION_REQUIREMENT_DISCOURAGED
    };
    PWEBAUTHN_ASSERTION pWebAssertion;
    HRESULT hresult = WebAuthNAuthenticatorGetAssertion(hWnd,
                                                        pwszRpId,
                                                        &webAuthnClientData,
                                                        &assertionOptions,
                                                        &pWebAssertion);
    if (hresult != S_OK) {
        wprintf(L"Failed :(\n");
        exit(3);
    }
    printBinaryAsBase64Url(L"AuthenticatorData", 
                           pWebAssertion->cbAuthenticatorData, pWebAssertion->pbAuthenticatorData);
    printBinaryAsBase64Url(L"Signature",
                           pWebAssertion->cbSignature, pWebAssertion->pbSignature);
    WebAuthNFreeAssertion(pWebAssertion);
}

int wmain(int argc, TCHAR **argv) {
    if (argc != 4) {
        wprintf(L"ctap2-sign data2SignB64U issuerDomain credentialIdB64U\n");
        exit(3);
    }
    BYTE data2Sign[1000];
    DWORD data2SignLength;
    decodeBase64Url(argv[1], &data2SignLength, data2Sign);
    BYTE credentialId[1000];
    DWORD credentialIdLength;
    decodeBase64Url(argv[3], &credentialIdLength, credentialId);
    WEBAUTHN_CREDENTIAL credential {
        WEBAUTHN_CREDENTIAL_CURRENT_VERSION,
        credentialIdLength,
        credentialId,
        WEBAUTHN_CREDENTIAL_TYPE_PUBLIC_KEY
    };
    WEBAUTHN_CREDENTIALS credentials {
        1,
        &credential
    };
    doFidoSign(data2SignLength, data2Sign, argv[2], credentials);
}
