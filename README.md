# ctap2-sign

Small Windows command-line program which signs arbitrary clientData 
using FIDO/CTAP2.

## Usage
```
ctap2-sign data2sign domain credentialid
```
`data2sign` holds arbitrary data encoded in Base64Url.<br>
`domain` holds an issuer domain like `test.webpki.org`.<br>
`credentialid` holds the requested credentialId encoded in Base64Url.

The command returns Base64Url encoded values of `signature` and `authenticatorData`.

## FIDO Web Pay
Applied to https://fido-web-pay.github.io/specification/ FIDO/CTAP2
signatures enables CBOR encoded payment authorizations like:

```code
{
  1: "1.0",
  2: {
    1: "Space Shop",
    2: "20220817.0001",
    3: "435.00",
    4: "EUR"
  },
  3: "test.webpki.org",
  4: "FR7630002111110020050014382",
  5: "20500211",
  6: "https://bankdirect.com",
  8: {
    1: {
      3: "Windows",
      4: "N/A"
    },
    2: {
      3: "Chrome",
      4: "104"
    }
  },
  9: "2022-08-17T11:24:41+02:00",
  / ENVELOPED SIGNATURE CONTAINER /
  10: {
    / ALGORITHM (COSE) /
    1: -257,
    / PUBLIC KEY (COSE) /
    2: {
      1: 3,
      -1: h'd399ae53ee39c12a1d1d7b40aef068872c762995c22d7a4bd08742423ef2fb79fb23a0b073f76c69a6934f6aa9036e8ff991435ca2eaf7b92c77455e3560448a395a4d65f294d3c9186cbaea77477f7e96995ff86236e2069c106b065fe6b4a59fe373ce8a688256228923b3740b438ac563e93ac7950ae7199b6d1821acf69dc8ebd7421efb10a74116b4b05835433392d06ea6e9a05decee0246940f40684e7d5ffce28ffd8d268aa5d7935b13b5e2e3bc71eb1a86c80dce1298d5f159cf613dc874718bd0cf50a2e3ef92c3ef5cb164dc9061df7a9777dc60560e7d097dd500396d8fd950c68585df0770b4298d08903aeca215de5b442c408641e4da89e7',
      -2: h'010001'
    },
    / FIDO AUTHENTICATOR DATA /
    3: h'29a61b658fc4de4d14ce7fbaf1bcd67ec37773fe8b6549118c729a6cdb37b11b050000001c',
    / FIDO SIGNATURE /
    4: h'9ba4bab6fde7fd121cc70e2d8bf553771b2a280d287018694d0c5d6af69da55fa289e7374fd1021db52b45078607f3ff5955a9d2991b54d7a3e612d982c22f7f465615b296d86999cc0eafa5f50a8d6dd8ae3507cdda3a05990bd4e801037c75476d12ba43b8f581cc192fc2831ad399aee96b653cda5ded315cefe1bd51c531f4e3889999f6239a469f8ca128813bdb2743dd5fc5e8b9c70faf8b55c1a855362e182ea0c123ad8ae6e4b38702bea6b1bdc5469eeb78d1a3acef829da89dcb15446a4c2b8b7a5a299c269a171b8efdcc2d710d7d286678d09a540538e45d395fd60e2c15e97a74bddd8ea1403b33b7ac9dfc0586738856b33256c4e6c2b8e5c7'
  }
}
```
That is, the WebAuthn `clientDataJSON` element and associated JSON parsing is _eliminated_.

Although the solution above uses an *enveloped* signature scheme that depends on the
*deterministic mode* of CBOR, nothing prevents you from modifying JOSE
and COSE libraries, to cope with `authenticatorData`.
