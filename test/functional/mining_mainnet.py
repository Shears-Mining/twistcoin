#!/usr/bin/env python3
# Copyright (c) 2025-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test mining on an alternate mainnet

Test mining related RPCs that involve difficulty adjustment, which
regtest doesn't have.

It uses an alternate mainnet chain. See data/README.md for how it was generated.

Mine one retarget period worth of blocks with a short interval in
order to maximally raise the difficulty. Verify this using the getmininginfo RPC.

"""

from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
    assert_equal,
)
from test_framework.blocktools import (
    create_coinbase,
    nbits_str,
    script_BIP34_coinbase_height,
    target_str
)
from test_framework.script import CScript
from test_framework.messages import uint256_from_compact

from test_framework.messages import (
    CBlock,
    SEQUENCE_FINAL,
)

import json
import os

# See data/README.md
COINBASE_SCRIPT_PUBKEY="76a914eadbac7f36c37e39361168b7aaee3cb24a25312d88ac"

# TwistCoin: one retarget period is 144 blocks, starting at the mainnet genesis
# difficulty. The alternate chain's first period was mined fast enough to hit
# the 4x clamp, so the next target is a quarter of the genesis one.
RETARGET_PERIOD = 144
GENESIS_N_BITS = 0x1b01cde6
GENESIS_TARGET = uint256_from_compact(GENESIS_N_BITS)
NEXT_TARGET = GENESIS_TARGET * (144 * 120 // 4) // (144 * 120)


def compact_from_target(target):
    """Mirrors arith_uint256::GetCompact()."""
    size = (target.bit_length() + 7) // 8
    compact = target << (8 * (3 - size)) if size <= 3 else target >> (8 * (size - 3))
    if compact & 0x00800000:
        compact >>= 8
        size += 1
    return compact | (size << 24)


def get_difficulty(bits):
    """Mirrors GetDifficulty() in src/rpc/blockchain.cpp."""
    shift = (bits >> 24) & 0xff
    diff = 0x0000ffff / (bits & 0x00ffffff)
    while shift < 29:
        diff *= 256.0
        shift += 1
    while shift > 29:
        diff /= 256.0
        shift -= 1
    return diff


NEXT_N_BITS = compact_from_target(NEXT_TARGET)
NEXT_TARGET = uint256_from_compact(NEXT_N_BITS)  # as rounded by the compact encoding


def assert_difficulty(actual, bits):
    expected = get_difficulty(bits)
    assert abs(float(actual) - expected) <= expected * 1e-12, f"{actual} != {expected}"

class MiningMainnetTest(BitcoinTestFramework):

    def set_test_params(self):
        self.num_nodes = 1
        self.setup_clean_chain = True
        self.chain = "" # main

    def add_options(self, parser):
        parser.add_argument(
            '--datafile',
            default='data/mainnet_alt.json',
            help='Block data file (default: %(default)s)',
        )

    def mine(self, height, prev_hash, blocks, node):
        self.log.debug(f"height={height}")
        block = CBlock()
        block.nVersion = blocks['versions'][height - 1]  # the ASIC used version-rolling
        block.hashPrevBlock = int(prev_hash, 16)
        block.nTime = blocks['timestamps'][height - 1]
        block.nBits = GENESIS_N_BITS if height < RETARGET_PERIOD else NEXT_N_BITS
        block.nNonce = blocks['nonces'][height - 1]
        block.vtx = [create_coinbase(height=height, script_pubkey=bytes.fromhex(COINBASE_SCRIPT_PUBKEY))]
        # The ASIC filled a 4-byte extranonce right after the BIP34 height.
        block.vtx[0].vin[0].scriptSig = CScript(bytes(script_BIP34_coinbase_height(height)) + bytes.fromhex(blocks['extranonces'][height - 1]))
        # The alternate mainnet chain was mined with non-timelocked coinbase txs.
        block.vtx[0].nLockTime = 0
        block.vtx[0].vin[0].nSequence = SEQUENCE_FINAL
        block.hashMerkleRoot = block.calc_merkle_root()
        block_hex = block.serialize(with_witness=False).hex()
        self.log.debug(block_hex)
        assert_equal(node.submitblock(block_hex), None)
        prev_hash = node.getbestblockhash()
        assert_equal(prev_hash, block.hash_hex)
        return prev_hash


    def run_test(self):
        node = self.nodes[0]
        # Clear disk space warning
        node.stderr.seek(0)
        node.stderr.truncate()
        self.log.info("Load alternative mainnet blocks")
        path = os.path.join(os.path.dirname(os.path.realpath(__file__)), self.options.datafile)
        prev_hash = node.getbestblockhash()
        blocks = None
        with open(path) as f:
            blocks = json.load(f)
            n_blocks = len(blocks['timestamps'])
            assert_equal(n_blocks, RETARGET_PERIOD)

        # Mine up to the last block of the first retarget period
        for i in range(RETARGET_PERIOD - 1):
            prev_hash = self.mine(i + 1, prev_hash, blocks, node)

        assert_equal(node.getblockcount(), RETARGET_PERIOD - 1)

        self.log.info("Check difficulty adjustment with getmininginfo")
        mining_info = node.getmininginfo()
        assert_difficulty(mining_info['difficulty'], GENESIS_N_BITS)
        assert_equal(mining_info['bits'], nbits_str(GENESIS_N_BITS))
        assert_equal(mining_info['target'], target_str(GENESIS_TARGET))

        assert_equal(mining_info['next']['height'], RETARGET_PERIOD)
        assert_difficulty(mining_info['next']['difficulty'], NEXT_N_BITS)
        assert_equal(mining_info['next']['bits'], nbits_str(NEXT_N_BITS))
        assert_equal(mining_info['next']['target'], target_str(NEXT_TARGET))

        # Mine first block of the second retarget period
        height = RETARGET_PERIOD
        prev_hash = self.mine(height, prev_hash, blocks, node)
        assert_equal(node.getblockcount(), height)

        mining_info = node.getmininginfo()
        assert_difficulty(mining_info['difficulty'], NEXT_N_BITS)

        self.log.info("getblock RPC should show historical target")
        block_info = node.getblock(node.getblockhash(1))

        assert_difficulty(block_info['difficulty'], GENESIS_N_BITS)
        assert_equal(block_info['bits'], nbits_str(GENESIS_N_BITS))
        assert_equal(block_info['target'], target_str(GENESIS_TARGET))


if __name__ == '__main__':
    MiningMainnetTest(__file__).main()
