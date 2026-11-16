// Copyright (c) 2026 The DailyCrypto developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
//
// DCG : formats de wallet HD des versions DCG 1.x / 2.x (base PIVX 4.0.2 + HD "a la Dash"),
// relus par la 3.0 pour convertir les cles. Lecture seule : la 3.0 n'ecrit jamais ces entrees.

#ifndef DCG_WALLET_LEGACYHD_H
#define DCG_WALLET_LEGACYHD_H

#include "pubkey.h"
#include "serialize.h"
#include "uint256.h"

#include <map>
#include <vector>

// Valeurs >= a ce seuil dans "minversion" : numerotation des wallets DCG 1.x/2.x (ex. 1000400)
static const int DCG_LEGACY_WALLET_VERSION_MIN = 1000000;
// Type de monnaie BIP44 de DCG (EXT_COIN_TYPE 0x80000651)
static const uint32_t DCG_BIP44_COIN_TYPE = 1617;

struct DcgLegacyHDAccount {
    uint32_t nExternalChainCounter{0};
    uint32_t nInternalChainCounter{0};
    SERIALIZE_METHODS(DcgLegacyHDAccount, obj) { READWRITE(obj.nExternalChainCounter, obj.nInternalChainCounter); }
};

// Entree "hdchain" (en clair) ou "chdchain" (graine chiffree) des wallets DCG 1.x/2.x
struct DcgLegacyHDChainRecord {
    int nVersion{0};
    uint256 id;
    bool fCrypted{false};
    std::vector<unsigned char> vchSeed;
    std::vector<unsigned char> vchMnemonic;
    std::vector<unsigned char> vchMnemonicPassphrase;
    std::map<uint32_t, DcgLegacyHDAccount> mapAccounts;
    SERIALIZE_METHODS(DcgLegacyHDChainRecord, obj)
    {
        READWRITE(obj.nVersion, obj.id, obj.fCrypted, obj.vchSeed, obj.vchMnemonic, obj.vchMnemonicPassphrase, obj.mapAccounts);
    }
};

// Valeur d'une entree "hdpubkey" (cle : paire ("hdpubkey", CPubKey))
struct DcgLegacyHDPubKeyRecord {
    int nVersion{0};
    CExtPubKey extPubKey;
    uint256 hdchainID;
    uint32_t nAccountIndex{0};
    uint32_t nChangeIndex{0};
    SERIALIZE_METHODS(DcgLegacyHDPubKeyRecord, obj)
    {
        READWRITE(obj.nVersion, obj.extPubKey, obj.hdchainID, obj.nAccountIndex, obj.nChangeIndex);
    }
};

#endif // DCG_WALLET_LEGACYHD_H
