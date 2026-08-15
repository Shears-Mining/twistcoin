TwistCoin Core
===============

TwistCoin is an independent SHA256d proof-of-work blockchain (ASIC-compatible), forked from [Bitcoin Core](https://github.com/bitcoin/bitcoin) at tag v31.1. It has its own genesis block, network identity, and consensus parameters.

TwistCoin Core connects to the TwistCoin peer-to-peer network to download and fully validate blocks and transactions. It also includes a wallet, which can be optionally built.

Building
--------

```
cmake -B build -DBUILD_GUI=OFF -DWITH_ZMQ=ON -DENABLE_WALLET=ON
cmake --build build
```

See [doc/build-unix.md](doc/build-unix.md) and the other `doc/build-*.md` files (inherited from upstream Bitcoin Core, still accurate for the build system itself) for platform-specific instructions and dependencies.

License
-------

TwistCoin Core is released under the terms of the MIT license, same as the Bitcoin Core codebase it's forked from. See [COPYING](COPYING) for more information or see https://opensource.org/license/MIT.

Relationship to Bitcoin Core
-----------------------------

This is a source fork, not a network fork: TwistCoin has its own genesis block and shares no chain history or connectivity with Bitcoin. The codebase, however, is Bitcoin Core's, and most of the underlying implementation, developer documentation in [doc/](doc), and test tooling in [test/](test) is unchanged from upstream and still describes Bitcoin Core's engineering practices accurately. For the original project, see https://github.com/bitcoin/bitcoin.
