#!/usr/bin/env python3
# Copyright (c) 2019-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test basic signet functionality"""

from decimal import Decimal
import shutil

from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import assert_equal

# TwistCoin's default signet challenge: 1-of-1 multisig of the signet operator's key.
SIGNET_DEFAULT_CHALLENGE = '512103dbea5b222cebda5dbd5e53346560e721afa0e6823dc10d26fad0d9f185dafb3251ae'
# Challenge used for the pregenerated blocks below: 1-of-2 multisig of two test keys
# (private keys 2 and 3), generated with contrib/signet/miner.
SIGNET_TEST_CHALLENGE = '512102c6047f9441ed7d6d3045406e95c07cd85c778e4b8cef3ca7abac09b95c709ee52102f9308a019258c31049344f85f89d5229b531c845836f99b08601f113bce036f952ae'

signet_blocks = [
    '00000020ab3f5aeb25fb2364d53bbdcab1155eeebb80c410e17cda8332f2666b8002000033eb28b21b1f91ae7f045ba936cd1440f960cf6a41a7cdecd967f4a71a55610c005e806aae77031ea3b1560001020000000001010000000000000000000000000000000000000000000000000000000000000000ffffffff025100feffffff0200943577000000001976a9142b4569203694fc997e13f2c0a1383b9e16c77a0d88ac0000000000000000776a24aa21a9ede2f61c3f71d1defd3fa999dfa36953755c690689799962b48bebd836974e8cf94c4fecc7daa2490047304402201b22842ac84e6b02bc2f118f2a7bfe96df4602f2f1ea6f5566a6d749dd534d08022012598c83a23d3dca37a554ef7d8796514eee8b3a17030f8f68697e923411058001000120000000000000000000000000000000000000000000000000000000000000000000000000',
    '000000208ed8be85451f17c2f534d9c824af17d4dcff3dd76366c86a1260ecdadc010000845448a65d96e04e60ab789613da99d9f1f9b8b0084a4153f57441d7f484afb8785e806aae77031e1e2c2e0001020000000001010000000000000000000000000000000000000000000000000000000000000000ffffffff025200feffffff0200943577000000001976a9142b4569203694fc997e13f2c0a1383b9e16c77a0d88ac0000000000000000776a24aa21a9ede2f61c3f71d1defd3fa999dfa36953755c690689799962b48bebd836974e8cf94c4fecc7daa2490047304402205a61b73d8c4c491d0bf213261456ed1bb467c82dfe3d1ea6c6c27a2ad495234e022067b254d9021b71afa2a7012d660b5eff09e3b8358e014ec4cce001c36cdea97e01000120000000000000000000000000000000000000000000000000000000000000000001000000',
    '0000002061ed49378107dc50a6cf679d05661fab1e31684ba94f8051df679eb4d302000060ffaa35a841941f8866d9d2392f703ac026b2d84a0e80c03dcf75be74dec7eff05e806aae77031e74773f0001020000000001010000000000000000000000000000000000000000000000000000000000000000ffffffff025300feffffff0200943577000000001976a9142b4569203694fc997e13f2c0a1383b9e16c77a0d88ac0000000000000000776a24aa21a9ede2f61c3f71d1defd3fa999dfa36953755c690689799962b48bebd836974e8cf94c4fecc7daa249004730440220476e2eeb41493598116c3ca3a2db2d208e489a584c0b9c288ce5f58f61133c3e02207457f5851ccb0c7898a1b082443a6ce5f4bdeb41735ba408f7acb3451b6c652201000120000000000000000000000000000000000000000000000000000000000000000002000000',
    '00000020f87669153de77fb4d4fd26bf9b0c3f1cf4ab645f40e6873d084bef88d601000031507b4d936c2117dfc4943e2dfe102984416eec60e939da9244d428052b721d685f806aae77031eaaaa250001020000000001010000000000000000000000000000000000000000000000000000000000000000ffffffff025400feffffff0200943577000000001976a9142b4569203694fc997e13f2c0a1383b9e16c77a0d88ac0000000000000000776a24aa21a9ede2f61c3f71d1defd3fa999dfa36953755c690689799962b48bebd836974e8cf94c4fecc7daa24900473044022006ac0e8654d139a137d6bd28b892ee2b696244b375105e920a37a304e893fff702207fb56a48f4391df9b43482dbc97896f447d72a74251f4d61bd31eb625d67466101000120000000000000000000000000000000000000000000000000000000000000000003000000',
    '00000020d5db4644a3a43368c87c14a81a89dfd8a4088122ff1fe39535d64698df01000044ec31085b46b9553893f878403d5aac42bd238c894341681ba85d4e9ef3f58fe05f806aae77031eae5e5c0001020000000001010000000000000000000000000000000000000000000000000000000000000000ffffffff025500feffffff0200943577000000001976a9142b4569203694fc997e13f2c0a1383b9e16c77a0d88ac0000000000000000776a24aa21a9ede2f61c3f71d1defd3fa999dfa36953755c690689799962b48bebd836974e8cf94c4fecc7daa249004730440220448862100ee8999ce8db87789e42ca91eee34169b38cf023d275052e70c198b1022035b949e3913ab021ebb0b752f12ef067977d309566f6eecab2ccb103a97c5b9a01000120000000000000000000000000000000000000000000000000000000000000000004000000',
    '00000020a6afd6a713d27478a3c733a08f3d0a936fc3190183fee08a5e7811af850000006149ec7318c027ed5b08c746c16e406546783911c21d79035914d0c5445299355860806aae77031e799f180001020000000001010000000000000000000000000000000000000000000000000000000000000000ffffffff025600feffffff0200943577000000001976a9142b4569203694fc997e13f2c0a1383b9e16c77a0d88ac0000000000000000776a24aa21a9ede2f61c3f71d1defd3fa999dfa36953755c690689799962b48bebd836974e8cf94c4fecc7daa2490047304402200f77b688f74652ff947954c124a4b07f04638f1f1729ad4cbbe15c144209e99d022019d47dce191a486e5369945d06f401a4bdce4a77bd52de70791178260c73022401000120000000000000000000000000000000000000000000000000000000000000000005000000',
    '0000002093fa457e18bf3c25ba65e561e5f5b512a9c8f5ffc57e0d0ac38b3edbee020000ae6b55c9d21dbf7043442d288be229a96776c821decdd720c33492ecba7126ebd060806aae77031eae4a4d0001020000000001010000000000000000000000000000000000000000000000000000000000000000ffffffff025700feffffff0200943577000000001976a9142b4569203694fc997e13f2c0a1383b9e16c77a0d88ac0000000000000000776a24aa21a9ede2f61c3f71d1defd3fa999dfa36953755c690689799962b48bebd836974e8cf94c4fecc7daa2490047304402202a1ec3a8cb4a176590b221515a0e608a93d1b14786bee307c385c263d318df72022021edc4bca88a63b506da6d50fe0bf9fe6962a217668ee3e39d7fcd5697cbe45601000120000000000000000000000000000000000000000000000000000000000000000006000000',
    '0000002029198e8e6721c37370ad750264db58abebf2d13803919f2a7c68b289950200000969dc35f9f72e81a500389dfa49e7b092e7dab3c50ac1a0f0733b302966d7bb4861806aae77031e8190300101020000000001010000000000000000000000000000000000000000000000000000000000000000ffffffff025800feffffff0200943577000000001976a9142b4569203694fc997e13f2c0a1383b9e16c77a0d88ac0000000000000000776a24aa21a9ede2f61c3f71d1defd3fa999dfa36953755c690689799962b48bebd836974e8cf94c4fecc7daa2490047304402206403bfc8fcbff91f6df39251c755b3f926b0e61cb8f301bea74448e36b93ab1a02206831a893bc130bab18fd67b159c66abd21660e227209791f060ad19440c8088001000120000000000000000000000000000000000000000000000000000000000000000007000000',
    '00000020ece4dc89ea837c1ad063d12b0e894bed1c013eaad494f0f88dbf262b8b0100002a54aadb559ba9175051e0af7831fc44bc99850ffc2925e7bcbe38bd5fbe5923c061806aae77031e4132320001020000000001010000000000000000000000000000000000000000000000000000000000000000ffffffff025900feffffff0200943577000000001976a9142b4569203694fc997e13f2c0a1383b9e16c77a0d88ac0000000000000000776a24aa21a9ede2f61c3f71d1defd3fa999dfa36953755c690689799962b48bebd836974e8cf94c4fecc7daa249004730440220289dca0d719485db76f64ec6ceb8279dfdb5e29c77e083d29347a197a74e026802201645dcc0520855c767e1e923228a391375c0450e79725d78e363a467df8d9bdc01000120000000000000000000000000000000000000000000000000000000000000000008000000',
    '00000020c1ab45bc6201a2cb46e84792c9d7bc6ddd659a7512154d605f939d4373000000c086a4291d5439e70c895d29d8b518849da5584ec14e74ab7d7fb1dc0c4d23a73862806aae77031e0bc22a0001020000000001010000000000000000000000000000000000000000000000000000000000000000ffffffff025a00feffffff0200943577000000001976a9142b4569203694fc997e13f2c0a1383b9e16c77a0d88ac0000000000000000776a24aa21a9ede2f61c3f71d1defd3fa999dfa36953755c690689799962b48bebd836974e8cf94c4fecc7daa24900473044022057d41506425140c7fb759cc8bcc4d677ec4390a6cc21d98b60d4cd36967821d102206d8280fc152ccfc5f6b5cfe7bddae87fcb33e76a154de8f3098ce33a22d7dfa801000120000000000000000000000000000000000000000000000000000000000000000009000000',
]

class SignetParams:
    def __init__(self, challenge=None):
        # Prune to prevent disk space warning on CI systems with limited space,
        # when using networks other than regtest.
        if challenge is None:
            self.challenge = SIGNET_DEFAULT_CHALLENGE
            self.shared_args = ["-prune=550"]
        else:
            self.challenge = challenge
            self.shared_args = ["-prune=550", f"-signetchallenge={challenge}"]

class SignetBasicTest(BitcoinTestFramework):
    def set_test_params(self):
        self.chain = "signet"
        self.num_nodes = 6
        self.setup_clean_chain = True
        self.signets = [
            SignetParams(challenge='51'), # OP_TRUE
            SignetParams(challenge=SIGNET_TEST_CHALLENGE),
            # test challenge as a 2-of-2, which means it should fail
            SignetParams(challenge='52' + SIGNET_TEST_CHALLENGE[2:])
        ]

        self.extra_args = [
            self.signets[0].shared_args, self.signets[0].shared_args,
            self.signets[1].shared_args, self.signets[1].shared_args,
            self.signets[2].shared_args, self.signets[2].shared_args,
        ]

    def setup_network(self):
        self.setup_nodes()

        # Setup the three signets, which are incompatible with each other
        self.connect_nodes(0, 1)
        self.connect_nodes(2, 3)
        self.connect_nodes(4, 5)

    def run_test(self):
        self.log.info("basic tests using OP_TRUE challenge")

        self.log.info('getblockchaininfo')
        def check_getblockchaininfo(node_idx, signet_idx):
            blockchain_info = self.nodes[node_idx].getblockchaininfo()
            assert_equal(blockchain_info['chain'], 'signet')
            assert_equal(blockchain_info['signet_challenge'], self.signets[signet_idx].challenge)
        check_getblockchaininfo(node_idx=1, signet_idx=0)
        check_getblockchaininfo(node_idx=2, signet_idx=1)
        check_getblockchaininfo(node_idx=5, signet_idx=2)

        self.log.info('getmininginfo')
        def check_getmininginfo(node_idx, signet_idx):
            mining_info = self.nodes[node_idx].getmininginfo()
            assert_equal(mining_info['blocks'], 0)
            assert_equal(mining_info['chain'], 'signet')
            assert 'currentblocktx' not in mining_info
            assert 'currentblockweight' not in mining_info
            assert_equal(mining_info['networkhashps'], Decimal('0'))
            assert_equal(mining_info['pooledtx'], 0)
            assert_equal(mining_info['signet_challenge'], self.signets[signet_idx].challenge)
        check_getmininginfo(node_idx=0, signet_idx=0)
        check_getmininginfo(node_idx=3, signet_idx=1)
        check_getmininginfo(node_idx=4, signet_idx=2)

        self.generate(self.nodes[0], 1, sync_fun=self.no_op)

        self.log.info("pregenerated signet blocks check")

        height = 0
        for block in signet_blocks:
            assert_equal(self.nodes[2].submitblock(block), None)
            height += 1
            assert_equal(self.nodes[2].getblockcount(), height)

        self.log.info("pregenerated signet blocks check (incompatible solution)")

        assert_equal(self.nodes[4].submitblock(signet_blocks[0]), 'bad-signet-blksig')

        self.log.info("test that the default signet uses TwistCoin's challenge")
        # Fresh chain directory: the existing one belongs to another signet (different magic).
        self.stop_node(5)
        shutil.rmtree(self.nodes[5].chain_path)
        self.start_node(5, extra_args=["-prune=550"])
        assert_equal(self.nodes[5].getblockchaininfo()['signet_challenge'], SIGNET_DEFAULT_CHALLENGE)

        self.log.info("test that signet logs the network magic on node start")
        with self.nodes[0].assert_debug_log(["Signet derived magic (message start)"]):
            self.restart_node(0)
        self.stop_node(0)
        self.nodes[0].assert_start_raises_init_error(extra_args=["-signetchallenge=abc"], expected_msg="Error: -signetchallenge must be hex, not 'abc'.")
        self.nodes[0].assert_start_raises_init_error(extra_args=["-signetchallenge=abc"] * 2, expected_msg="Error: -signetchallenge cannot be multiple values.")


if __name__ == '__main__':
    SignetBasicTest(__file__).main()
