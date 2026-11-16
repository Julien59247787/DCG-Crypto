// Copyright (c) 2014-2021 The Dash Core developers
// Copyright (c) 2015-2022 The PIVX Core developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef ACTIVEMASTERNODE_H
#define ACTIVEMASTERNODE_H

#include "key.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "evo/deterministicmns.h"
#include "operationresult.h"
#include "sync.h"
#include "validationinterface.h"

class CActiveDeterministicMasternodeManager;
class CBLSPublicKey;
class CBLSSecretKey;

#define ACTIVE_MASTERNODE_INITIAL 0 // initial state
#define ACTIVE_MASTERNODE_SYNC_IN_PROCESS 1
#define ACTIVE_MASTERNODE_NOT_CAPABLE 3
#define ACTIVE_MASTERNODE_STARTED 4

extern CActiveDeterministicMasternodeManager* activeMasternodeManager;

struct CActiveMasternodeInfo
{
    // Keys for the active Masternode
    CBLSPublicKey pubKeyOperator;
    CBLSSecretKey keyOperator;
    // Initialized while registering Masternode
    uint256 proTxHash{UINT256_ZERO};
    CService service;
};

class CActiveDeterministicMasternodeManager : public CValidationInterface
{
public:
    enum masternode_state_t {
        MASTERNODE_WAITING_FOR_PROTX,
        MASTERNODE_POSE_BANNED,
        MASTERNODE_REMOVED,
        MASTERNODE_OPERATOR_KEY_CHANGED,
        MASTERNODE_PROTX_IP_CHANGED,
        MASTERNODE_READY,
        MASTERNODE_ERROR,
    };

private:
    masternode_state_t state{MASTERNODE_WAITING_FOR_PROTX};
    std::string strError;
    CActiveMasternodeInfo info;

public:
    ~CActiveDeterministicMasternodeManager() override = default;
    void UpdatedBlockTip(const CBlockIndex* pindexNew, const CBlockIndex* pindexFork, bool fInitialDownload) override;

    void Init(const CBlockIndex* pindexTip);
    void Reset(masternode_state_t _state, const CBlockIndex* pindexTip);
    // Sets the Deterministic Masternode Operator's private/public key
    OperationResult SetOperatorKey(const std::string& strMNOperatorPrivKey);
    // If the active masternode is ready, and the keyID matches with the registered one,
    // return private key, keyID, and pointer to dmn.
    OperationResult GetOperatorKey(CBLSSecretKey& key, CDeterministicMNCPtr& dmn) const;
    // Directly return the operator secret key saved in the manager, without performing any validation
    const CBLSSecretKey* OperatorKey() const { return &info.keyOperator; }
    void SetNullProTx() { info.proTxHash = UINT256_ZERO; }
    const uint256 GetProTx() const { return info.proTxHash; }

    const CActiveMasternodeInfo* GetInfo() const { return &info; }
    masternode_state_t GetState() const { return state; }
    std::string GetStatus() const;
    bool IsReady() const { return state == MASTERNODE_READY; }

    static bool IsValidNetAddr(const CService& addrIn);
};

// Responsible for initializing the masternode
OperationResult initMasternode(const std::string& strMasterNodePrivKey, const std::string& strMasterNodeAddr, bool isFromInit);


// Responsible for activating the Masternode and pinging the network (legacy MN list)
class CActiveMasternode
{
private:
    int status{ACTIVE_MASTERNODE_INITIAL};
    std::string notCapableReason;

public:
    CActiveMasternode() = default;

    // Initialized by init.cpp
    // Keys for the main Masternode
    CPubKey pubKeyMasternode;
    CKey privKeyMasternode;

    // Initialized while registering Masternode
    Optional<CTxIn> vin{nullopt};
    CService service;

    /// Manage status of main Masternode
    void ManageStatus();
    void ResetStatus();
    std::string GetStatusMessage() const;
    int GetStatus() const { return status; }

    /// Ping Masternode
    bool SendMasternodePing(std::string& errorMessage);
    /// Enable cold wallet mode (run a Masternode with no funds)
    bool EnableHotColdMasterNode(CTxIn& vin, CService& addr);

    void GetKeys(CKey& privKeyMasternode, CPubKey& pubKeyMasternode) const;
};

// DCG : plusieurs masternodes (legacy) servis par un meme demon et une meme IP:port.
// Fichier <datadir>/activemasternode.conf, une ligne par masternode : "alias masternodeprivkey".
// Si -masternodeprivkey est absent, la premiere ligne devient le masternode principal (activeMasternode).
struct DcgExtraMasternode {
    std::string strAlias;
    CActiveMasternode mn;
};
extern std::vector<std::unique_ptr<DcgExtraMasternode>> vDcgExtraMasternodes;
// Lit activemasternode.conf (alias, cle) ; renvoie false et strError si une ligne est invalide.
bool DcgReadActiveMasternodeConf(std::vector<std::pair<std::string, std::string>>& vEntries, std::string& strError);
// Initialise les masternodes supplementaires (meme adresse de service que le principal).
OperationResult DcgInitExtraMasternodes(const std::vector<std::pair<std::string, std::string>>& vEntries);
// ManageStatus() du masternode principal et de tous les supplementaires.
void DcgManageAllMasternodes();
// Activation distante : une annonce (mnb) correspond a l'une de nos cles.
void DcgEnableIfOurs(const CPubKey& pubKeyMasternode, const CTxIn& vin, const CService& addr);

// Compatibility code: get vin and keys for either legacy or deterministic masternode
bool GetActiveMasternodeKeys(CTxIn& vin, Optional<CKey>& key, CBLSSecretKey& blsKey);
// Get active masternode BLS operator keys for DMN
bool GetActiveDMNKeys(CBLSSecretKey& key, CTxIn& vin);

#endif
