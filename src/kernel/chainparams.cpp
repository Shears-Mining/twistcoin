// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kernel/chainparams.h>

#include <arith_uint256.h>
#include <chainparamsseeds.h>
#include <consensus/amount.h>
#include <consensus/merkle.h>
#include <consensus/params.h>
#include <crypto/hex_base.h>
#include <hash.h>
#include <kernel/messagestartchars.h>
#include <primitives/block.h>
#include <primitives/transaction.h>
#include <script/interpreter.h>
#include <script/script.h>
#include <uint256.h>
#include <util/chaintype.h>
#include <util/log.h>
#include <util/strencodings.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <map>
#include <span>
#include <utility>

using namespace util::hex_literals;

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.version = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

/**
 * Build the genesis block. Note that the output of its generation
 * transaction cannot be spent since it did not originally exist in the
 * database.
 *
 * CBlock(hash=000000000019d6, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=4a5e1e, nTime=1231006505, nBits=1d00ffff, nNonce=2083236893, vtx=1)
 *   CTransaction(hash=4a5e1e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
 *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73)
 *     CTxOut(nValue=50.00000000, scriptPubKey=0x5F1DF16B2B704C8A578D0B)
 *   vMerkleTree: 4a5e1e
 */
static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "The Times 03/Jan/2009 Chancellor on brink of second bailout for banks";
    const CScript genesisOutputScript = CScript() << "04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f"_hex << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

/**
 * TwistCoin genesis coinbase output script. The pubkey bytes are an
 * arbitrary, cosmetic value unique to TwistCoin (not a real key) - the
 * genesis coinbase output is provably unspendable regardless of content,
 * since Bitcoin Core never connects the genesis block's coinbase into the
 * UTXO set through the normal block-connection path. Shared across
 * testnet/regtest/signet below (mainnet uses a real key instead, see
 * TWIST_MAINNET_GENESIS_OUTPUT_SCRIPT).
 */
static const CScript TWIST_GENESIS_OUTPUT_SCRIPT = CScript() << "0441ecbfbcb6c88c9b0eeabee9bb0ed4ea8ba912f2dc4c6be71625037e410b535cca98f195ccae4c85fa8969d5866a304dca084082c57a5c11921ce8b6250fc863"_hex << OP_CHECKSIG;

/**
 * Real P2PK output script for the mainnet genesis, matching real Bitcoin's
 * own genesis convention (paying to Satoshi's actual pubkey, not a
 * placeholder) - still provably unspendable regardless of whose key it is,
 * same reasoning as TWIST_GENESIS_OUTPUT_SCRIPT above, but this one is a
 * real key the user controls, derived from and cross-checked against
 * address tw1q4rzztuqef4ed4n7yq0f0gahme7gvga2unwrkwu.
 */
static const CScript TWIST_MAINNET_GENESIS_OUTPUT_SCRIPT = CScript() << "04191440969f973f172d72fc7279baba66567eb1d4ae13bcba68cc407c65665adcc8d629962ad6b5b65905e47b44c04c6a038f433fee3a96d0aa9ab4d0412b1878"_hex << OP_CHECKSIG;

/**
 * Main network on which people trade goods and services.
 */
class CMainParams : public CChainParams {
public:
    CMainParams() {
        m_chain_type = ChainType::MAIN;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        consensus.nSubsidyHalvingInterval = 525600; // ~2 years at 120s/block (2x faster than Bitcoin's ~4y)
        // No script_flag_exceptions: those were fixes for specific historical Bitcoin
        // mainnet blocks that predate BIP16/Taproot soft-fork activation; TwistCoin
        // has no such history.
        consensus.BIP34Height = 0; // fresh chain: all soft forks active from genesis
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = 0;
        consensus.BIP66Height = 0;
        consensus.CSVHeight = 0;
        consensus.SegwitHeight = 0;
        consensus.MinBIP9WarningHeight = 0;
        consensus.powLimit = ArithToUint256(arith_uint256().SetCompact(0x1e0ffff0));
        consensus.nPowTargetTimespan = 144 * 120; // 144-block retarget window (17280s)
        consensus.nPowTargetSpacing = 120; // 2 minutes
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.enforce_BIP94 = false;
        consensus.fPowNoRetargeting = false;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0; // No activation delay
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].threshold = 130; // 90% of 144
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].period = 144; // matches retarget interval

        // Taproot is active from genesis on TwistCoin - no live BIP9 signaling
        // rollout is needed for a brand-new chain, modern rules are built in from block 0.
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].threshold = 130;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].period = 144;

        // No assumed-valid checkpoint yet: TwistCoin has no history to assume valid from.
        consensus.nMinimumChainWork = uint256();
        consensus.defaultAssumeValid = uint256();

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * Derived from SHA256("TwistCoin/mainnet/magic")[:4], unique to TwistCoin.
         */
        pchMessageStart[0] = 0x7f;
        pchMessageStart[1] = 0x87;
        pchMessageStart[2] = 0xc0;
        pchMessageStart[3] = 0x69;
        nDefaultPort = 8433;
        nPruneAfterHeight = 100000;
        m_assumed_blockchain_size = 0;
        m_assumed_chain_state_size = 0;

        // Timestamp text ties the genesis to real Bitcoin block 962555 (leading
        // zeros of the block hash dropped - they carry no information, and the
        // full 64-hex-char hash makes the coinbase scriptSig exceed Bitcoin's
        // 100-byte bad-cb-length consensus limit no matter how the surrounding
        // text is trimmed), proving no mining happened before that block existed
        // (same technique as Satoshi's original "Chancellor on brink..."
        // headline). nBits (0x1b01cde6) is calibrated to a real 1.3 TH/s ASIC at
        // 120s/block (D = hashrate * 120 / 2**32 =~ 36322) - genesis itself was
        // found by that same ASIC via tools/genesis_stratum_server.py (real
        // hardware, not a software grind - infeasible in software at this
        // difficulty), so mainnet starts at the real sustained difficulty with no
        // post-launch ramp-up period. nVersion is whatever the ASIC's
        // version-rolling produced for that winning nonce, not 1. Output script
        // is TWIST_MAINNET_GENESIS_OUTPUT_SCRIPT - a real key the user controls
        // (see that constant's comment), not the dummy placeholder pubkey.
        genesis = CreateGenesisBlock("TwistCoin 15/Aug/2026 - Bitcoin bloc 962555 22e46e8ef4eba72391b65a7409148a585a52e82d42106", TWIST_MAINNET_GENESIS_OUTPUT_SCRIPT, 1786825636, 1923790317u, 0x1b01cde6, 204801, 20 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256{"0000000000010248a159720bbe1efa86184e3df2b244a3987d3a7f4632dc2069"});
        assert(genesis.hashMerkleRoot == uint256{"cd8d535e2a83f619cf7ba92fcf7d2845266abdee9bfc43ea6e14c5aef3f1860d"});

        // Static A/AAAA records, not a live crawler-backed seeder - plenty for
        // a network this size, and each entry can be repointed independently
        // if a server's IP ever changes, with no new software release needed.
        vSeeds.emplace_back("node.twistcoin.org");
        vSeeds.emplace_back("node1.twistcoin.org");
        vSeeds.emplace_back("node2.twistcoin.org");

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,65);  // 'T...'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,63);  // 'S...'
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,193);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x7e, 0x5d, 0x5a, 0x89}; // 'Tpub...'
        base58Prefixes[EXT_SECRET_KEY] = {0x7e, 0x5c, 0x65, 0x4a}; // 'Tprv...'

        bech32_hrp = "tw";

        // Last-resort fallback if DNS itself is unreachable - frozen at build
        // time (contrib/seeds/nodes_main.txt -> generate-seeds.py), unlike
        // vSeeds above. Update both if a node's address ever changes.
        vFixedSeeds = std::vector<uint8_t>(std::begin(chainparams_seed_main), std::end(chainparams_seed_main));

        fDefaultConsistencyChecks = false;
        m_is_mockable_chain = false;

        m_assumeutxo_data = {}; // no snapshots for a brand-new chain

        chainTxData = ChainTxData{
            .nTime    = 0,
            .tx_count = 0,
            .dTxRate  = 0,
        };

        // Headers sync memory parameters (HeadersSyncState requires non-zero values).
        // Generated by contrib/devtools/headerssync-params.py on 2026-10-05,
        // with BLOCK_INTERVAL=120s, GENESIS_TIME=2026-08-15 and
        // MINCHAINWORK_HEADERS=30000 (TwistCoin values instead of Bitcoin's).
        m_headers_sync_params = HeadersSyncParams{
            .commitment_period = 209,
            .redownload_buffer_size = 5096, // 5096/209 = ~24.4 commitments
        };
    }
};

/**
 * Testnet (v3): public test network which is reset from time to time.
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        m_chain_type = ChainType::TESTNET;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        consensus.nSubsidyHalvingInterval = 210000;
        consensus.script_flag_exceptions.emplace( // BIP16 exception
            uint256{"00000000dd30457c001f4095d208cc1296b0eed002427aa599874af7a432b105"}, SCRIPT_VERIFY_NONE);
        consensus.BIP34Height = 21111;
        consensus.BIP34Hash = uint256{"0000000023b3a96d3484e5abb3755c413e7d41500f8e2a5c3f0dd01299cd8ef8"};
        consensus.BIP65Height = 581885; // 00000000007f6655f22f98e72ed80d8b06dc761d5da09df0fa1dc4be4f861eb6
        consensus.BIP66Height = 330776; // 000000002104c8c45e99a8853285a3b592602a3ccde2b832481da85e9e4ba182
        consensus.CSVHeight = 770112; // 00000000025e930139bac5c6c31a403776da130831ab85be56578f3fa75369bb
        consensus.SegwitHeight = 834624; // 00000000002b980fcd729daaa248fd9316a5200e9b367f4ff2c42453e84201ca
        consensus.MinBIP9WarningHeight = 836640; // segwit activation height + miner confirmation window
        consensus.powLimit = ArithToUint256(arith_uint256().SetCompact(0x1e0ffff0));
        // Mirror mainnet pacing: with the relaxed powLimit above, Bitcoin's two-week timespan
        // would let powLimit * 4 * nPowTargetTimespan overflow 256 bits in
        // CalculateNextWorkRequired() (caught by pow_tests' chainparams sanity check).
        consensus.nPowTargetTimespan = 144 * 120;
        consensus.nPowTargetSpacing = 120;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.enforce_BIP94 = false;
        consensus.fPowNoRetargeting = false;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0; // No activation delay
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].threshold = 1512; // 75%
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].period = 2016;

        // Deployment of Taproot (BIPs 340-342) - always active, fresh chain, not deployed this round
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 0; // No activation delay
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].threshold = 1512; // 75%
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].period = 2016;

        consensus.nMinimumChainWork = uint256();
        consensus.defaultAssumeValid = uint256();

        pchMessageStart[0] = 0xa4;
        pchMessageStart[1] = 0xc5;
        pchMessageStart[2] = 0x22;
        pchMessageStart[3] = 0xee;
        nDefaultPort = 18633;
        nPruneAfterHeight = 1000;
        m_assumed_blockchain_size = 0;
        m_assumed_chain_state_size = 0;

        genesis = CreateGenesisBlock("TwistCoin testnet 15/Aug/2026 - cosmetic, not deployed this round", TWIST_GENESIS_OUTPUT_SCRIPT, 1786797448, 160984u, 0x1e0ffff0, 1, 20 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256{"00000295845c9fd3e05968b9111a3a83b3018c610c37df22aa0434fe383d6000"});
        assert(genesis.hashMerkleRoot == uint256{"b16c3189954d2c9521810f6cd5bb6dcb2bdc47c264b73adf5dcfa1fae9cc6123"});

        // Not deployed this round: no seed infrastructure of our own, and clearing
        // Bitcoin's real testnet seeds/fixed-seed IPs so this mode never tries to
        // contact real Bitcoin infrastructure.
        vFixedSeeds.clear();
        vSeeds.clear();

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,127); // 't...'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,125); // 's...'
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,255);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "twt";

        fDefaultConsistencyChecks = false;
        m_is_mockable_chain = false;

        m_assumeutxo_data = {
            {
                .height = 2'500'000,
                .hash_serialized = AssumeutxoHash{uint256{"f841584909f68e47897952345234e37fcd9128cd818f41ee6c3ca68db8071be7"}},
                .m_chain_tx_count = 66484552,
                .blockhash = uint256{"0000000000000093bcb68c03a9a168ae252572d348a2eaeba2cdf9231d73206f"},
            },
            {
                .height = 4'840'000,
                .hash_serialized = AssumeutxoHash{uint256{"ce6bb677bb2ee9789c4a1c9d73e6683c53fc20e8fdbedbdaaf468982a0c8db2a"}},
                .m_chain_tx_count = 536078574,
                .blockhash = uint256{"00000000000000f4971a7fb37fbdff89315b69a2e1920c467654a382f0d64786"},
            }
        };

        chainTxData = ChainTxData{
            // Data from RPC: getchaintxstats 4096 000000007a61e4230b28ac5cb6b5e5a0130de37ac1faf2f8987d2fa6505b67f4
            .nTime    = 1772051651,
            .tx_count = 536108416,
            .dTxRate  = 0.02691479016257117,
        };

        // Generated by headerssync-params.py on 2026-02-25.
        m_headers_sync_params = HeadersSyncParams{
            .commitment_period = 673,
            .redownload_buffer_size = 14460, // 14460/673 = ~21.5 commitments
        };
    }
};

/**
 * Testnet (v4): public test network which is reset from time to time.
 */
class CTestNet4Params : public CChainParams {
public:
    CTestNet4Params() {
        m_chain_type = ChainType::TESTNET4;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        consensus.nSubsidyHalvingInterval = 210000;
        consensus.BIP34Height = 1;
        consensus.BIP34Hash = uint256{};
        consensus.BIP65Height = 1;
        consensus.BIP66Height = 1;
        consensus.CSVHeight = 1;
        consensus.SegwitHeight = 1;
        consensus.MinBIP9WarningHeight = 0;
        consensus.powLimit = uint256{"00000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffff"};
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 10 * 60;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.enforce_BIP94 = true;
        consensus.fPowNoRetargeting = false;

        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0; // No activation delay
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].threshold = 1512; // 75%
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].period = 2016;

        // Deployment of Taproot (BIPs 340-342)
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 0; // No activation delay
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].threshold = 1512; // 75%
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].period = 2016;

        consensus.nMinimumChainWork = uint256{"0000000000000000000000000000000000000000000009a0fe15d0177d086304"};
        consensus.defaultAssumeValid = uint256{"0000000002368b1e4ee27e2e85676ae6f9f9e69579b29093e9a82c170bf7cf8a"}; // 123613

        pchMessageStart[0] = 0x4d;
        pchMessageStart[1] = 0x36;
        pchMessageStart[2] = 0x99;
        pchMessageStart[3] = 0xdd;
        nDefaultPort = 48333;
        nPruneAfterHeight = 1000;
        m_assumed_blockchain_size = 31;
        m_assumed_chain_state_size = 2;

        const char* testnet4_genesis_msg = "03/May/2024 000000000000000000001ebd58c244970b3aa9d783bb001011fbe8ea8e98e00e";
        const CScript testnet4_genesis_script = CScript() << "000000000000000000000000000000000000000000000000000000000000000000"_hex << OP_CHECKSIG;
        genesis = CreateGenesisBlock(testnet4_genesis_msg,
                testnet4_genesis_script,
                1714777860,
                393743547,
                0x1d00ffff,
                1,
                50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256{"00000000da84f2bafbbc53dee25a72ae507ff4914b867c565be350b0da8bf043"});
        assert(genesis.hashMerkleRoot == uint256{"7aa0a7ae1e223414cb807e40cd57e667b718e42aaf9306db9102fe28912b7b4e"});

        // Not deployed this round: no seeds of our own, and Bitcoin's real testnet4
        // seeds/fixed-seed IPs are cleared so this mode never contacts real infra.
        vFixedSeeds.clear();
        vSeeds.clear();

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "tb";

        fDefaultConsistencyChecks = false;
        m_is_mockable_chain = false;

        m_assumeutxo_data = {
            {
                .height = 90'000,
                .hash_serialized = AssumeutxoHash{uint256{"784fb5e98241de66fdd429f4392155c9e7db5c017148e66e8fdbc95746f8b9b5"}},
                .m_chain_tx_count = 11347043,
                .blockhash = uint256{"0000000002ebe8bcda020e0dd6ccfbdfac531d2f6a81457191b99fc2df2dbe3b"},
            },
            {
                .height = 120'000,
                .hash_serialized = AssumeutxoHash{uint256{"10b05d05ad468d0971162e1b222a4aa66caca89da2bb2a93f8f37fb29c4794b0"}},
                .m_chain_tx_count = 14141057,
                .blockhash = uint256{"000000000bd2317e51b3c5794981c35ba894ce27d3e772d5c39ecd9cbce01dc8"},
            }
        };

        chainTxData = ChainTxData{
            // Data from RPC: getchaintxstats 4096 0000000002368b1e4ee27e2e85676ae6f9f9e69579b29093e9a82c170bf7cf8a
            .nTime    = 1772013387,
            .tx_count = 14191421,
            .dTxRate  = 0.01848579579528412,
        };

        // Generated by headerssync-params.py on 2026-02-25.
        m_headers_sync_params = HeadersSyncParams{
            .commitment_period = 606,
            .redownload_buffer_size = 16092, // 16092/606 = ~26.6 commitments
        };
    }
};

/**
 * Signet: test network with an additional consensus parameter (see BIP325).
 */
class SigNetParams : public CChainParams {
public:
    explicit SigNetParams(const SigNetOptions& options)
    {
        std::vector<uint8_t> bin;
        vFixedSeeds.clear();
        vSeeds.clear();

        if (!options.challenge) {
            // Default TwistCoin signet: 1-of-1 multisig of the signet operator's key
            // (OP_1 <pubkey> OP_1 OP_CHECKMULTISIG), same shape as Bitcoin's 1-of-2 default.
            // Not deployed yet: no seeds of our own.
            bin = "512103dbea5b222cebda5dbd5e53346560e721afa0e6823dc10d26fad0d9f185dafb3251ae"_hex_v_u8;

            consensus.nMinimumChainWork = uint256{};
            consensus.defaultAssumeValid = uint256{};
            m_assumed_blockchain_size = 0;
            m_assumed_chain_state_size = 0;
            chainTxData = ChainTxData{
                0,
                0,
                0,
            };
        } else {
            bin = *options.challenge;
            consensus.nMinimumChainWork = uint256{};
            consensus.defaultAssumeValid = uint256{};
            m_assumed_blockchain_size = 0;
            m_assumed_chain_state_size = 0;
            chainTxData = ChainTxData{
                0,
                0,
                0,
            };
            LogInfo("Signet with challenge %s", HexStr(bin));
        }

        if (options.seeds) {
            vSeeds = *options.seeds;
        }

        m_chain_type = ChainType::SIGNET;
        consensus.signet_blocks = true;
        consensus.signet_challenge.assign(bin.begin(), bin.end());
        consensus.nSubsidyHalvingInterval = 525600; // mirror mainnet economics
        consensus.BIP34Height = 1;
        consensus.BIP34Hash = uint256{};
        consensus.BIP65Height = 1;
        consensus.BIP66Height = 1;
        consensus.CSVHeight = 1;
        consensus.SegwitHeight = 1;
        consensus.nPowTargetTimespan = 144 * 120; // mirror mainnet pacing
        consensus.nPowTargetSpacing = 120;
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.enforce_BIP94 = false;
        consensus.fPowNoRetargeting = false;
        consensus.MinBIP9WarningHeight = 0;
        consensus.powLimit = uint256{"00000377ae000000000000000000000000000000000000000000000000000000"};
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0; // No activation delay
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].threshold = 1815; // 90%
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].period = 2016;

        // Activation of Taproot (BIPs 340-342)
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 0; // No activation delay
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].threshold = 1815; // 90%
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].period = 2016;

        // message start is defined as the first 4 bytes of the sha256d of the block script
        HashWriter h{};
        h << consensus.signet_challenge;
        uint256 hash = h.GetHash();
        std::copy_n(hash.begin(), 4, pchMessageStart.begin());

        nDefaultPort = 38633;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock("TwistCoin signet 07/Oct/2026 - public test network", TWIST_GENESIS_OUTPUT_SCRIPT, 1786797448, 1858398u, 0x1e0377ae, 1, 20 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256{"000002806b66f23283da7ce110c480bbee5e15b1cabd3bd56423fb25eb5a3fab"});
        assert(genesis.hashMerkleRoot == uint256{"9c7c9159108535c81198c7a1f0bb5f2c51bd63d272df2a15a6c22f78b48b43b9"});

        m_assumeutxo_data = {}; // no snapshots for a brand-new chain

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,127); // 't...'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,125); // 's...'
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,255);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "tws";

        fDefaultConsistencyChecks = false;
        m_is_mockable_chain = false;

        // Same pacing and genesis date as mainnet, so same values as CMainParams
        // (contrib/devtools/headerssync-params.py, BLOCK_INTERVAL=120s, GENESIS_TIME=2026-08-15).
        m_headers_sync_params = HeadersSyncParams{
            .commitment_period = 209,
            .redownload_buffer_size = 5096, // 5096/209 = ~24.4 commitments
        };
    }
};

/**
 * Regression test: intended for private networks only. Has minimal difficulty to ensure that
 * blocks can be found instantly.
 */
class CRegTestParams : public CChainParams
{
public:
    explicit CRegTestParams(const RegTestOptions& opts)
    {
        m_chain_type = ChainType::REGTEST;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        consensus.nSubsidyHalvingInterval = 525600; // mirror mainnet economics (blocks stay instant regardless)
        consensus.BIP34Height = 1; // Always active unless overridden
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = 1;  // Always active unless overridden
        consensus.BIP66Height = 1;  // Always active unless overridden
        consensus.CSVHeight = 1;    // Always active unless overridden
        consensus.SegwitHeight = 0; // Always active unless overridden
        consensus.MinBIP9WarningHeight = 0;
        consensus.powLimit = uint256{"7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"};
        consensus.nPowTargetTimespan = 144 * 120; // mirror mainnet economics
        consensus.nPowTargetSpacing = 120;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.enforce_BIP94 = opts.enforce_bip94;
        consensus.fPowNoRetargeting = true;

        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0; // No activation delay
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].threshold = 108; // 75%
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].period = 144; // Faster than normal for regtest (144 instead of 2016)

        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 0; // No activation delay
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].threshold = 108; // 75%
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].period = 144;

        consensus.nMinimumChainWork = uint256{};
        consensus.defaultAssumeValid = uint256{};

        pchMessageStart[0] = 0x47;
        pchMessageStart[1] = 0x6d;
        pchMessageStart[2] = 0x7a;
        pchMessageStart[3] = 0x51;
        nDefaultPort = 18644;
        nPruneAfterHeight = opts.fastprune ? 100 : 1000;
        m_assumed_blockchain_size = 0;
        m_assumed_chain_state_size = 0;

        for (const auto& [dep, height] : opts.activation_heights) {
            switch (dep) {
            case Consensus::BuriedDeployment::DEPLOYMENT_SEGWIT:
                consensus.SegwitHeight = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_HEIGHTINCB:
                consensus.BIP34Height = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_DERSIG:
                consensus.BIP66Height = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_CLTV:
                consensus.BIP65Height = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_CSV:
                consensus.CSVHeight = int{height};
                break;
            }
        }

        for (const auto& [deployment_pos, version_bits_params] : opts.version_bits_parameters) {
            consensus.vDeployments[deployment_pos].nStartTime = version_bits_params.start_time;
            consensus.vDeployments[deployment_pos].nTimeout = version_bits_params.timeout;
            consensus.vDeployments[deployment_pos].min_activation_height = version_bits_params.min_activation_height;
        }

        genesis = CreateGenesisBlock("TwistCoin regtest 15/Aug/2026 - local dev genesis", TWIST_GENESIS_OUTPUT_SCRIPT, 1786797448, 1u, 0x207fffff, 1, 20 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256{"3d688f5f811fc9134951c610eb349d85b56f6c99da9e4c1e92c7db98854ef309"});
        assert(genesis.hashMerkleRoot == uint256{"62fe252e9c4f1d62ff5508bb0298b816b3bdba16339ff05640c4440f8b5a24af"});

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();
        vSeeds.emplace_back("dummySeed.invalid.");

        fDefaultConsistencyChecks = true;
        m_is_mockable_chain = true;

        m_assumeutxo_data = {
            {   // For use by unit tests
                .height = 110,
                .hash_serialized = AssumeutxoHash{uint256{"9e8257f2f3c2351d304efba05b44bef4af2f0d8bd4d8f7866de9920c6ab43a6a"}},
                .m_chain_tx_count = 111,
                .blockhash = uint256{"6dc101b1937508fe70df7c0bb19058afa2093de131ac4e1aa89b2897bef16fb2"},
            },
            {
                // For use by fuzz target src/test/fuzz/utxo_snapshot.cpp
                .height = 200,
                .hash_serialized = AssumeutxoHash{uint256{"17dcc016d188d16068907cdeb38b75691a118d43053b8cd6a25969419381d13a"}},
                .m_chain_tx_count = 201,
                .blockhash = uint256{"385901ccbd69dff6bbd00065d01fb8a9e464dede7cfe0372443884f9b1dcf6b9"},
            },
            {
                // For use by test/functional/feature_assumeutxo.py and test/functional/tool_bitcoin_chainstate.py
                .height = 299,
                .hash_serialized = AssumeutxoHash{uint256{"def25791e552611798d2efaec05258b1fd4df34ecdc48c078e23af48bffa5aa6"}},
                .m_chain_tx_count = 334,
                .blockhash = uint256{"3230ba076792a9e877d32e647a2aacba469499907c9a2b5ecfc45d118bd54342"},
            },
        };

        chainTxData = ChainTxData{
            .nTime = 0,
            .tx_count = 0,
            .dTxRate = 0.001, // Set a non-zero rate to make it testable
        };

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,127); // 't...'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,125); // 's...'
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,255);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF}; // reused from real Bitcoin testnet, cosmetic-tier
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "twrt";

        // Copied from Testnet4.
        m_headers_sync_params = HeadersSyncParams{
            .commitment_period = 275,
            .redownload_buffer_size = 7017, // 7017/275 = ~25.5 commitments
        };
    }
};

std::unique_ptr<const CChainParams> CChainParams::SigNet(const SigNetOptions& options)
{
    return std::make_unique<const SigNetParams>(options);
}

std::unique_ptr<const CChainParams> CChainParams::RegTest(const RegTestOptions& options)
{
    return std::make_unique<const CRegTestParams>(options);
}

std::unique_ptr<const CChainParams> CChainParams::Main()
{
    return std::make_unique<const CMainParams>();
}

std::unique_ptr<const CChainParams> CChainParams::TestNet()
{
    return std::make_unique<const CTestNetParams>();
}

std::unique_ptr<const CChainParams> CChainParams::TestNet4()
{
    return std::make_unique<const CTestNet4Params>();
}

std::vector<int> CChainParams::GetAvailableSnapshotHeights() const
{
    std::vector<int> heights;
    heights.reserve(m_assumeutxo_data.size());

    for (const auto& data : m_assumeutxo_data) {
        heights.emplace_back(data.height);
    }
    return heights;
}

std::optional<ChainType> GetNetworkForMagic(const MessageStartChars& message)
{
    const auto mainnet_msg = CChainParams::Main()->MessageStart();
    const auto testnet_msg = CChainParams::TestNet()->MessageStart();
    const auto testnet4_msg = CChainParams::TestNet4()->MessageStart();
    const auto regtest_msg = CChainParams::RegTest({})->MessageStart();
    const auto signet_msg = CChainParams::SigNet({})->MessageStart();

    if (std::ranges::equal(message, mainnet_msg)) {
        return ChainType::MAIN;
    } else if (std::ranges::equal(message, testnet_msg)) {
        return ChainType::TESTNET;
    } else if (std::ranges::equal(message, testnet4_msg)) {
        return ChainType::TESTNET4;
    } else if (std::ranges::equal(message, regtest_msg)) {
        return ChainType::REGTEST;
    } else if (std::ranges::equal(message, signet_msg)) {
        return ChainType::SIGNET;
    }
    return std::nullopt;
}
