// Copyright (c) 2020-2026 The DailyCrypto developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
//
// Regles de consensus propres a DCG (Daily Crypto Giveaways), portees sur la base PIVX 5.6.
// Elles reproduisent a l'identique l'historique de la chaine DCG :
//  - v1.0.8.2 (base PIVX 4.0.2) : premine 60 000, PoS a 0,1 DCG/bloc, masternode 0,075 DCG, garantie 2 500 ;
//  - v2.0.0 "Reborn" (bloc 326 700) : plafond 1 000 000, emission decroissante (option B), 60 % au masternode,
//    bloc de reserve unique (250 000 DCG vers 4 adresses publiees), gel des adresses de l'ancienne generation.
// La profondeur de stake (200 puis 60 a partir de 325 730) est dans Consensus::Params::StakeMinDepth().

#ifndef DCG_DCG_H
#define DCG_DCG_H

#include "amount.h"
#include "chainparams.h"
#include "key_io.h"
#include "primitives/transaction.h"
#include "script/standard.h"
#include "util/system.h"

#include <algorithm>
#include <limits>
#include <set>
#include <string>
#include <utility>
#include <vector>

static const int DCG_LAST_POW_BLOCK = 500;
static const int DCG_REBORN_HEIGHT_MAIN = 326700;
static const int DCG_BLOCKS_PER_YEAR = 525600;

// Hauteur d'activation Reborn. -rebornheighttest n'est pris en compte que sur un noeud isole (-connect=0) : tests uniquement.
inline int DcgRebornHeight()
{
    static const int nRebornHeight = []() {
        int nHeight = (Params().NetworkIDString() == CBaseChainParams::MAIN) ? DCG_REBORN_HEIGHT_MAIN : std::numeric_limits<int>::max();
        const int nTest = gArgs.GetArg("-rebornheighttest", 0);
        if (nTest > 0 && gArgs.GetArg("-connect", "") == "0")
            nHeight = nTest;
        return nHeight;
    }();
    return nRebornHeight;
}

inline bool DcgIsReborn(int nHeight) { return nHeight >= DcgRebornHeight(); }

// Recompense v1.0.8.2 pour le bloc de hauteur nHeight.
// Le code d'origine decrementait la hauteur en interne (GetBlockValue(pprev->nHeight + 1) puis nHeight--) :
// la premine de 60 000 DCG est donc portee par le bloc 2, et le PoS paie 0,1 DCG a partir du bloc 502.
inline CAmount DcgLegacyBlockValue(int nHeight)
{
    const int h = nHeight - 1;
    if (h <= 0) return 0;
    if (h == 1) return 60000 * COIN;
    if (h <= DCG_LAST_POW_BLOCK) return 0;
    return COIN / 10;
}

// Emission Reborn (option B) : 0,20 DCG/bloc la 1re annee puis -0,02/an jusqu'a 0,06 la 10e, ensuite 0,02.
inline CAmount DcgRebornBlockValue(int nHeight)
{
    static const int64_t vCentiDCG[10] = {20, 18, 16, 14, 12, 10, 9, 8, 7, 6};
    int nYear = (nHeight - DcgRebornHeight()) / DCG_BLOCKS_PER_YEAR;
    if (nYear < 0) nYear = 0;
    if (nYear < 10) return vCentiDCG[nYear] * COIN / 100;
    return 2 * COIN / 100;
}

inline CAmount DcgRebornReserveTotal();

// Masse monetaire a la fin du bloc Reborn - 1 (relevee sur la chaine ; sert au plafond deterministe).
static const CAmount DCG_SUPPLY_BEFORE_REBORN = 0; // TODO : renseigner apres le bloc 326 700 (moneysupply du bloc 326 699)

// Emission theorique cumulee des blocs [Reborn, nHeight[ (hors reserve).
inline CAmount DcgRebornEmittedBefore(int nHeight)
{
    CAmount nTotal = 0;
    int h = DcgRebornHeight();
    for (int nYear = 0; h < nHeight; nYear++) {
        const int nYearEnd = DcgRebornHeight() + (nYear + 1) * DCG_BLOCKS_PER_YEAR;
        const int nBlocks = std::min(nHeight, nYearEnd) - h;
        nTotal += (CAmount)nBlocks * DcgRebornBlockValue(h);
        h += nBlocks;
    }
    return nTotal;
}

// Valeur du bloc de hauteur nHeight, plafond de 1 000 000 DCG inclus (calcul deterministe : masse avant Reborn
// + reserve + emission theorique ; la masse reelle ne peut qu'etre inferieure, le plafond est donc toujours respecte).
inline CAmount DcgBlockValue(int nHeight)
{
    if (!DcgIsReborn(nHeight)) return DcgLegacyBlockValue(nHeight);
    const CAmount nValue = DcgRebornBlockValue(nHeight);
    if (DCG_SUPPLY_BEFORE_REBORN <= 0) return nValue;
    const CAmount nUsed = DCG_SUPPLY_BEFORE_REBORN + DcgRebornReserveTotal() + DcgRebornEmittedBefore(nHeight);
    const CAmount nLeft = Params().GetConsensus().nMaxMoneyOut - nUsed;
    if (nLeft <= 0) return 0;
    return std::min(nValue, nLeft);
}

// Paiement du masternode : 0,075 DCG (v1.0.8.2, rien pendant le PoW), puis 60 % de la valeur du bloc (Reborn).
inline CAmount DcgMasternodePayment(int nHeight, CAmount nBlockValue)
{
    if (DcgIsReborn(nHeight)) return nBlockValue * 60 / 100;
    if (nHeight <= DCG_LAST_POW_BLOCK) return 0;
    return 75 * COIN / 1000;
}

inline CScript DcgScript(const std::string& strAddress)
{
    return GetScriptForDestination(DecodeDestination(strAddress));
}

// Reserve du projet (bloc Reborn ; adresses du wallet de controle, libelles DCG-RESERVE-*).
inline const std::vector<std::pair<std::string, CAmount> >& DcgRebornReserve()
{
    static const std::vector<std::pair<std::string, CAmount> > v = {
        {"D8VVWXUqwhsh6hNyuMBoiuKFVCTA7rc4EG", 100000 * COIN}, // equipe / dev (dont 5 masternodes)
        {"DANn9cMg8uFM8diES2tKMFdK2X9Xwz3NQP", 90000 * COIN},  // robot de giveaways / communaute
        {"D7bjbv2M3NoP1CcE9951FGbAojyUQVPneU", 50000 * COIN},  // marketing / exchanges / liquidite
        {"DFvQtjjL5tjKW2MiWRhcBMAaXHP8dLMPLm", 10000 * COIN},  // ancien staff (4 masternodes sur demande)
    };
    return v;
}

inline CAmount DcgRebornReserveTotal()
{
    CAmount n = 0;
    for (const auto& p : DcgRebornReserve()) n += p.second;
    return n;
}

// Le coinstake du bloc Reborn doit contenir chacune des sorties de reserve (adresse et montant exacts).
inline bool DcgCheckRebornReserve(const CTransaction& tx)
{
    std::vector<bool> vUsed(tx.vout.size(), false);
    for (const auto& p : DcgRebornReserve()) {
        const CScript script = DcgScript(p.first);
        bool fFound = false;
        for (unsigned int i = 0; i < tx.vout.size(); i++) {
            if (!vUsed[i] && tx.vout[i].scriptPubKey == script && tx.vout[i].nValue == p.second) {
                vUsed[i] = true;
                fFound = true;
                break;
            }
        }
        if (!fFound) return false;
    }
    return true;
}

// Adresses gelees a partir du bloc Reborn (scan du bloc 325 811 : >= 1 000 DCG ou issues directement de la premine).
inline bool DcgIsFrozenScript(const CScript& script)
{
    static const std::set<CScript> setFrozen = []() {
        std::set<CScript> setF;
        static const char* vFrozen[] = {
            "DAZaQzuAB5mo9cRtMwjdQdpQ9LHMpfz9UH", "DHuJ1zsd9LG42h9bAwxZHi7BMLUoKUU25W", "DE2YRvi434AnP9c8quj49acN2vkCiBZFf2",
            "DSiNBMFkaFeJJAFFRvJWsTicDoekUAAuMp", "DEmLvxs96oegKohbmjRadh5EvSXMwhGNaQ", "DPNYT9A6iP26y8eXJ7wKaCddJbkedKd23Y",
            "DBuo8ZeBa7th6Ek3LhjWhMwSSm8ym5DJEW", "D9tV2zLfJJR5QmfTFiqLGZ9MoLJwJW9LrY", "DSdEmhxJPaQSM6sSvzZKiZSUvbiEQzB3qb",
            "DDgbXcCkasujcoVEoLEkkiX47fwerj1jvy", "DMV7jgaFfuGeQA7KxjDm4VgpaTQwKvCbNE", "DNV7Hx2MnyRAHmX76qnrCgh4ZBEkSbcHHS",
            "DSsbpnLGyTAuTfKaL5YrX9vp7K2AnX2HsA", "DH2ok9KJWw7A23qyEdcmY8gDx85PnNTpb4", "DB9117hbfTqfWXGgViPbhcqGUpCGA4hZ9a",
            "D8nX8LBDvBydZGsdYn4oQtFfHUESboYcaH", "DTckhwNDcrKi1H2kwYXojiG5uFTHzu3TMe",
            // adresses de la premine sans solde au scan (gelees par precaution)
            "DKugpE9jNpBKRokX8dwiR1Kov2sqdgSGp4", "DTwP1CxyCHd6FfRmp49HTHErCXS2uEGYZJ", "DN4L4WgKoNMQG6nJNzXiru5r9mEmycysDe",
            "D7v9EAUJ6iLeEspKLrWUb17cKuL6MA2pET",
        };
        for (const char* psz : vFrozen) setF.insert(DcgScript(psz));
        return setF;
    }();
    return setFrozen.count(script) > 0;
}

inline bool DcgIsFrozenInput(const CScript& scriptPubKey, int nSpendHeight)
{
    return DcgIsReborn(nSpendHeight) && DcgIsFrozenScript(scriptPubKey);
}

#endif // DCG_DCG_H
