// Copyright (c) 2013-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/test/unit_test.hpp>

#include <clientversion.h>
#include <key.h>
#include <key_io.h>
#include <streams.h>
#include <test/util/setup_common.h>
#include <util/strencodings.h>

#include <string>
#include <vector>

namespace {

struct TestDerivation {
    std::string pub;
    std::string prv;
    unsigned int nChild;
};

struct TestVector {
    std::string strHexMaster;
    std::vector<TestDerivation> vDerive;

    explicit TestVector(std::string strHexMasterIn) : strHexMaster(strHexMasterIn) {}

    TestVector& operator()(std::string pub, std::string prv, unsigned int nChild) {
        vDerive.emplace_back();
        TestDerivation &der = vDerive.back();
        der.pub = pub;
        der.prv = prv;
        der.nChild = nChild;
        return *this;
    }
};

TestVector test1 =
  TestVector("000102030405060708090a0b0c0d0e0f")
    ("Tpub1yAfVPyKT8wPQqZ7HKjRKFabXiBmoP3T68cKTs2JRKJf3XeMVtoo5fNgwqvfSH5YYXSreNgip4rsw4jVs7WWsdAyhbXawVumSxHbqdr43hME",
     "Tprv1swNauzmAPKf7CxtS4Nt74KbjDVvQVq1EHWUnP4cJjWW1ojFA6StPnJ7hsU9pehMntdMFp5dxDSBrEHXDkyz4s5RPufNEKGo1AwqtmbQiEAv",
     0x80000000)
    ("Tpub1yCvuPgcvECWPzAEzXVHx8che31L9m7w6pDkMq7gYHVrKtvegK3LAnX3MdCE92PT5Xf9DEvGEoVx238U14YFFh4SRoAPCRDffUDjPwWbRjzm",
     "Tprv1sydzui4dUan6Ma29G8kjwMhqYKUksuVEy7ugM9zRhhhJB1YLWgRUuSU7ejWA4QjkbgExBNxpbo2ix3YwQaGFf9VuyY3NZEVtsxUkCk7mUKU",
     1)
    ("Tpub1yF72bUAocuPTpcHSFkBpjMbJ6PKFm9fPkYRCRPTME8eY8U9p2qkUk5wb3GgiSLARVNmG2LiLv6SVdSCpXsR9NiGN6NE3trfw6xGBEUnBnpj",
     "Tprv1t1p87VcWsHfAC24azPecY6bVbhTrswDXuSaWwRmEeLVWQZ3UEUqns1NM4nhDfLWo3kzdzCZFgVz95z5AXv4ESJzMVK16czGkYko7je8vksn",
     0x80000002)
    ("Tpub1yHiJdzzfKnEsgn5XWLbC6MdSKYBn2hFP65esfikWQ11yYKLvqbaueXVWbHsYtCbjqtrzdigxJ31U5mM7ExxgSARCgGCByojcpeBKKfwm8An",
     "Tprv1t4RQA2SNaAWa4BrgEz3yu6ddprLP9UoXEypCBm4PpCrwpQEb3EgDmSvGcpuqda2HZE22afWB6up3KBov7vyRZQS1qvZemvWhA2YLNExZbiY",
     2)
    ("Tpub1yKwhUS7cWFurmFZFCboGyrnkQMUWfHZaXZYAvgGyiSaauQyHuNHGr6xSGcrwHzkUKCzAmjAuLmuaeBh7LGPt9ktTb3kNozdSqL6AULjG2t7",
     "Tprv1t6enzTZKkeBZ8fLPwFG4nbnwufd7n57igThVSias8eRZBVrx71Nay2PCJ9KJ6ERid29jCajk1jGCnNiNKMWmko24FAekL1LQK11VsoU1QJh",
     1000000000)
    ("Tpub1yMfTx7irdPJ4HbLhK9N1HZnkCb3pUnNcKovjAxf7vWy5cYNgRnXw7D7VuSmt9FqBnuGiH56hsycBEc68S7tj123aTbATcUD5eHi5iQqxQhw",
     "Tprv1t8NZU9AZsmZkf17r3npo6JnwhuCRbZvkUi63gzy1Lip3tdGLdRdFE8YFw16W19YoERYyywigrmoKnYijrRXVskfbafQySv67bLZrctDDvRB",
     0);

TestVector test2 =
  TestVector("fffcf9f6f3f0edeae7e4e1dedbd8d5d2cfccc9c6c3c0bdbab7b4b1aeaba8a5a29f9c999693908d8a8784817e7b7875726f6c696663605d5a5754514e4b484542")
    ("Tpub1yAfVPyKT8wPQqAccRtVbhnKHxNvE4fhupC7C9y3FHgmupdNh4xa26GrYC2kQzKuWS16SHMNyTwPBnXE6EhS2m5yeTTnQLQG7GfGwupMjEnq",
     "Tprv1swNauzmAPKf7CaPmAXxPWXKVTh4qBTG3y6GWg1M8htct6iGMGbfLDCHJDXvsqCut8g6gZN6QVYE3KpFKoQNmUSkAN5SUTchjkCU88XCLRAL",
     0)
    ("Tpub1yDwEfh1QcxDVMSCuZfsUmaipKLfUmeCCNMrZKn4hN6dsWbpfeDv6F7RnmViJnQuweKac9aNis2E9wgen7r8T6gdvjWX5EkQVQ9PcLyQzyzF",
     "Tprv1szeLBiT7sLVBiqz4JKLGaKj1pep5tRkLXG1sqpNanJUqngiKqs1QN2rYo3CgUGkiYLLWTNW8vheAgKt9MgspQfuZNFrXr6sUscNgVs4EJCX",
     0xFFFFFFFF)
    ("Tpub1yF6HvJ2vzrPbXJR5z7s8EvQNi5tPK8MtTWw6FAyU1qQFxu48G7LQTBDPNsxRY9X3RqW9oRELAEekcMX7tkcZpq6ZGb98bUfWGXfDBkXrADx",
     "Tprv1t1oPSKUeFEfHtiCEimKv3fQaDQ2zRuv2cR6QmDHMS3FEEywnTkRia6e9QPgfCXVCSZfQEPhmyi9H7Zcv64UwyyM7nz5sccbjfPiDGEAA9pd",
     1)
    ("Tpub1yHuGLK1sAVgncv6E3n1hCsLc3cktRKH9mxRx6EdoKikhdivHZcs7M5bmDg9ZL2DL21xhe4KZoMxah9n8of3X6AZTrsLyp26iCJLASGttMNL",
     "Tprv1t4cMrLTaQsxUzKsNnRUV1cLoYvuVY6qHvrbGcGwgjvbfuoowmFxRU12XFBtHEEgtkX26ygnAPFpVSGQ58x1KzTAQvkcgTia89C8wjugUSWG",
     0xFFFFFFFE)
    ("Tpub1yK5JFGxDmteVusKpnFqEaP2nxMEFUHueof9qcxLDAV1x65FDfHo7sbxrrLH4XPjmyTcLSZPGqyHSLnwXVYA2oXFv8mfUNKC84XQjyCue3PN",
     "Tprv1t5nPmJPw2GvCHH6yWuJ2P82zTfNrb5TnxZKA8ze6agrvNA8srvtRzXPcstcfrf5BJ6JoHWZ9mdQ5FPVSetupgHqakh9zbvbkUn9TojJoQPx",
     2)
    ("Tpub1yLSLCiAjJ4xjf8WiWKxvZ8QmMQmMicpjTpmj2jghoGsYAGanpvit4AurvjMLey6UVWpZiUtgjgJRqE12Kdqw7n7VmwVZJgUrjpbvdtJqgxR",
     "Tprv1t79RijcSYTES2YHsEyRiMsQxriuxqQNsciw3YmzbDUiWSMUT2ZpCB6LcxJHg1gAFLQAXaGByDjEmHwNxPDH3jGaJZYfVfLjUB9V1tEAKpcL",
     0);

TestVector test3 =
  TestVector("4b381541583be4423346c643850da4b320e46a87ae3d2a4e6da11eba819cd4acba45d239319ac14f863b8d5ab5a0d0c64d2e8a1e7d1457df2e5a3c51c73235be")
    ("Tpub1yAfVPyKT8wPQpE4mxGzPumaKq2FKpXsBXXskZG4oZNrhnsbxj6oz3Y64g2HPovV9c5rJ7MFyrcD3Dps15QUUBRdnahgd4xhx4NneSE1ypTn",
     "Tprv1swNauzmAPKf7BdqvgvTBiWaXLLPvwKRKgS355JNgyahg4xVcvjuJATWphXf3nE9DDNmS8HnWPxxkgTre2qXCFvFQtN17f43i2v8UnWpXgtV",
      0x80000000)
    ("Tpub1yD2h8w9apJkm1mABBRKty2KzXNrhyPH5w27hNVkeVgn53tjRJbar7zsmyk7uYH5toTM8tK1ihZQXrHoM1eQekfujFodWcpCkYZ9Z6hHVzUn",
     "Tprv1syjnexbJ4h2TPAwKv4ngmmLC2h1K6AqE5vH1tY4Xutd3Kyd5WEgAEvJY1J1Q8BU8fXQr9EaP3hbDL1Vnc46x1gT3KVvAQDKX9w68jpLw6dH",
      0);

TestVector test4 =
  TestVector("3ddd5602285899a946114506157c7997e5444528f3003f6134712147db19b678")
    ("Tpub1yAfVPyKT8wPQrHaLo1MCiX3DTFqVjnGvmfa9vzkPRuhPdmRHw1QkFkeAN9Y4qN5DBcyrG2VAoNyn2he8VcVPGHVhM7yhdGfkxyTsX5r1NWd",
     "Tprv1swNauzmAPKf7DhMVXeozXG3QxZz6rZq4vZjUT34Gr7YMurJx8eW4Ng4vPgwAnxYPnqj4EUfeLJAXKUUvgvn7LjuoPbs9hvwNK3zdYcB2Kx7",
     0x80000000)
    ("Tpub1yDpbnMS7WhVrVAbURLGyiv8yuBimT5oUWyBpGxvN5MmN8JUQVLqUPoKWXjoprzSLbikDD4snQJv2ZdLCCPwnMbYjPSQfW5m91n1vKT4sEvx",
     "Tprv1szXhJNspm5mYraNd9yjmXf9BQVsNZsMcfsM8o1EFVZcLQPN4gyvnWikGZErQqmikkpkVm3qsekGcG8g2JJSkMNnjEzApurPNJK2SHTSxQK5",
     0x80000001)
    ("Tpub1yFxHSLq82RYTAKT7ZcBspniq57tqx1WHAuEe3iSZjYrouPnvMqKBcvHPRgaMiB52BWMa6qotrmG1Y7f6ezkw4tV6jEFiTpF5tCNpP1NroCS",
     "Tprv1t2fNxNGqGop9XjEGJFefdXj2aS3T4o4RKoPxZkkT9khnBUgaZUQVjqi9TCpVuAkaqb9bwefqRPNUy6VjhEPsdmFxVKsr1tdh9za8P6tTemu",
     0);

const std::vector<std::string> TEST5 = {
    "Tpub1yAfVPyKT8wPQpD1jpm57Pv3vk7AE9SFUisu8R8MNqF6TQY62eATVGXYjPEi21wtJFrgiyPgfFnDTCTpAQUsSCLWThCfFDaVt5Wdv56WAG29",
    "Tprv1swNauzmAPKf7BcntZQXuCf48FRJqGDocsn4SwAfGFSwRgcygqoYoPSyVQz7J8Y5ehRKKWg1TrDL84yH4ykXn1zgGhprL7xJ1wk8NUn6DkkR",
    "Tpub1yAfVPyKT8wPQpD1jpm57Pv3vk7AE9SFUisu8R8MNqF6TQY62eATVGXYjPNUzisgRfZPdWg1Ut6hT1kDG4wLJo9sxqkwgCBFU48fU3XQVUTp",
    "Tprv1swNauzmAPKf7BcntZQXuCf48FRJqGDocsn4SwAfGFSwRgcygqoYoPSyVQzUQXCm9YYLBYZpr1DcDNNsy7z9buEjaRb8pKjya73d2Ta333Yt",
    "Tpub1yAfVPyKT8wPQpD1jpm57Pv3vk7AE9SFUisu8R8MNqF6TQY62eATVGXYjPGemCBLL7HMx7DGNAMqT9nQwKbV9r3MLUqjMTjBn5AeJZijNHir",
    "Tprv1swNauzmAPKf7BcntZQXuCf48FRJqGDocsn4SwAfGFSwRgcygqoYoPSyVQteAzWR3zGJW975jHUkDWR5eNeJSx8Cx4fvVbHut85bryfT5N4p",
    "Tprv1swP1jjFFnR7QFptpDfnxP6UMorBL1WXmN47XdYhZNvZPsx76y7sTqu6YqRDEv8J4mRboJxn2gzgqY4wYFLDGPEFzj3bNPF7F3CHCFVRUNb4",
    "Tpub1yAfvDhoYY2qhtR7fV2LAaMUAJY2itiydD9xD7WPfxiiRbsDSmUn9iyfnovdhS4KyT2MHnTU73ByyhK52iWF1ziiaa1QB2VTAobrXqExzN69",
    "Tprv1swNauzmAT7gG3Gn9hf5QUSjjgGvPo5EBGUixhbn5knzKsmnoB2k3y91j93hjbYdVejos5h79PXt9hmkQDD1U92qJ4tRuT4HfAEqdwiCGD91",
    "Tpub1yAfVPyKTCjQZfrzzy1ccfhjYAxmngHg37aZeBZUCLb9Mbgu8yPejrDay7Z8C7UfQLLZMZBoDjjBHs1stgP3DkXHsurEi6JdaveQyXU1AvY7",
    "DMwo58pR1QLEFihHiXPVykYB6fJmsTeHvyTp7hRThAtCX8CvYzgPcn8XnmdfHGMQzT7ayAmfo4z3gY5KfbrZWZ6St24UVf2Qgo6oujFktLHdHY4",
    "DMwo58pR1QLEFihHiXPVykYB6fJmsTeHvyTp7hRThAtCX8CvYzgPcn8XnmdfHPmHJiEDXkTiJTVV9rHEBUem2mwVbbNfvT2MTcAqj3nesx8uBf9",
    "Tprv1swNauzmAPKf7BcntZQXuCf48FRJqGDocsn4SwAfGFSwRgcygqoYoPSyVQrhRpGy28qdH1HW2Nu8DZ6UsTXgjJRN5H2rPM9Dz8RbUTkgPqGK",
    "Tprv1swNauzmAPKf7BcntZQXuCf48FRJqGDocsn4SwAfGFSwRgcygqoYoPSyVQteAzWR3zGJW975jHUkDWR5ZqmwYR6xAbxfWLSCZRnaVBvLSG4A",
    "Tpub1yAfVPyKT8wPQpD1jpm57Pv3vk7AE9SFUisu8R8MNqF6TQY62eATVGXYjPJbWNQnMxi3BF2r54wTT771iEi6sVkCDGUoThssg4peh3idUZMD",
    "xprv9s21ZrQH143K3QTDL4LXw2F7HEK3wJUD2nW2nRk4stbPy6cq3jPPqjiChkVvvNKmPGJxWUtg6LnF5kejMRNNU3TGtRBeJgk33yuGBxrMPHL"
};

void RunTest(const TestVector& test)
{
    std::vector<std::byte> seed{ParseHex<std::byte>(test.strHexMaster)};
    CExtKey key;
    CExtPubKey pubkey;
    key.SetSeed(seed);
    pubkey = key.Neuter();
    for (const TestDerivation &derive : test.vDerive) {
        unsigned char data[74];
        key.Encode(data);
        pubkey.Encode(data);

        // Test private key
        BOOST_CHECK(EncodeExtKey(key) == derive.prv);
        BOOST_CHECK(DecodeExtKey(derive.prv) == key); //ensure a base58 decoded key also matches

        // Test public key
        BOOST_CHECK(EncodeExtPubKey(pubkey) == derive.pub);
        BOOST_CHECK(DecodeExtPubKey(derive.pub) == pubkey); //ensure a base58 decoded pubkey also matches

        // Derive new keys
        CExtKey keyNew;
        BOOST_CHECK(key.Derive(keyNew, derive.nChild));
        CExtPubKey pubkeyNew = keyNew.Neuter();
        if (!(derive.nChild & 0x80000000)) {
            // Compare with public derivation
            CExtPubKey pubkeyNew2;
            BOOST_CHECK(pubkey.Derive(pubkeyNew2, derive.nChild));
            BOOST_CHECK(pubkeyNew == pubkeyNew2);
        }
        key = keyNew;
        pubkey = pubkeyNew;
    }
}

}  // namespace

BOOST_FIXTURE_TEST_SUITE(bip32_tests, BasicTestingSetup)

BOOST_AUTO_TEST_CASE(bip32_test1) {
    RunTest(test1);
}

BOOST_AUTO_TEST_CASE(bip32_test2) {
    RunTest(test2);
}

BOOST_AUTO_TEST_CASE(bip32_test3) {
    RunTest(test3);
}

BOOST_AUTO_TEST_CASE(bip32_test4) {
    RunTest(test4);
}

BOOST_AUTO_TEST_CASE(bip32_test5) {
    for (const auto& str : TEST5) {
        auto dec_extkey = DecodeExtKey(str);
        auto dec_extpubkey = DecodeExtPubKey(str);
        BOOST_CHECK_MESSAGE(!dec_extkey.key.IsValid(), "Decoding '" + str + "' as xprv should fail");
        BOOST_CHECK_MESSAGE(!dec_extpubkey.pubkey.IsValid(), "Decoding '" + str + "' as xpub should fail");
    }
}

BOOST_AUTO_TEST_CASE(bip32_max_depth) {
    CExtKey key_parent{DecodeExtKey(test1.vDerive[0].prv)}, key_child;
    CExtPubKey pubkey_parent{DecodeExtPubKey(test1.vDerive[0].pub)}, pubkey_child;

    // We can derive up to the 255th depth..
    for (auto i = 0; i++ < 255;) {
        BOOST_CHECK(key_parent.Derive(key_child, 0));
        std::swap(key_parent, key_child);
        BOOST_CHECK(pubkey_parent.Derive(pubkey_child, 0));
        std::swap(pubkey_parent, pubkey_child);
    }

    // But trying to derive a non-existent 256th depth will fail!
    BOOST_CHECK(key_parent.nDepth == 255 && pubkey_parent.nDepth == 255);
    BOOST_CHECK(!key_parent.Derive(key_child, 0));
    BOOST_CHECK(!pubkey_parent.Derive(pubkey_child, 0));
}

BOOST_AUTO_TEST_SUITE_END()
