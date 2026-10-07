# Various test vectors

## mainnet_alt.json

One retarget period (144 blocks) of real TwistCoin mainnet proof-of-work on top
of the real genesis block, used by `mining_mainnet.py` to test difficulty
adjustment. The blocks are timestamped 20 seconds apart, so the first retarget
hits the 4x clamp: blocks 1-143 are at the genesis difficulty, block 144 at four
times that.

TwistCoin mainnet starts at a difficulty (~36322) that a CPU cannot reach, so the
chain was mined by a real SHA256d ASIC through a small single-purpose Stratum
server that hands it exactly the blocks the test rebuilds.

Each block's coinbase is `create_coinbase(height)` paying to
`COINBASE_SCRIPT_PUBKEY` (nLockTime 0, final sequence), with a 4-byte
extranonce chosen by the ASIC appended to the BIP34 height in its scriptSig, and
no other transaction. Each block is therefore deterministic except for its
timestamp, nonce, version (the ASIC uses BIP310 version-rolling) and extranonce.
Those four values are stored in `mainnet_alt.json` and used to reconstruct the
chain without redoing the proof-of-work.

The payout address is derived from first BIP32 test vector master key:

```
pkh(xprv9s21ZrQH143K3QTDL4LXw2F7HEK3wJUD2nW2nRk4stbPy6cq3jPPqjiChkVvvNKmPGJxWUtg6LnF5kejMRNNU3TGtRBeJgk33yuGBxrMPHi/44h/0h/0h/<0;1>/*)#fkjtr0yn
```
