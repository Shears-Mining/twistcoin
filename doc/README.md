TwistCoin Core
=============

Setup
---------------------
TwistCoin Core is the reference node client for the TwistCoin network. It downloads and, by default, stores the entire history of TwistCoin transactions. Depending on the speed of your computer and network connection, synchronization time will vary.

See the top-level [README](/README.md) for what TwistCoin is and how it differs from Bitcoin.

Running
---------------------
The following are some helpful notes on how to run TwistCoin Core on your native platform. The underlying binary targets are unchanged from upstream Bitcoin Core (`bitcoind`, `bitcoin-cli`, etc. - see the top-level [README](/README.md) for why), but the build automatically creates `twistcoind`/`twistcoin-cli` symlinks next to them so the files you actually run are named for TwistCoin, no extra step needed.

### Unix

Unpack the files into a directory and run:

- `bin/twistcoind` (headless node)
- `bin/twistcoin-cli` (RPC client)

The GUI (`bitcoin-qt`/`twistcoin-qt`) is not built by default for TwistCoin (`-DBUILD_GUI=OFF`).

### Need Help?

This is a small, independent project - there's no Bitcoin Wiki/StackExchange/IRC-channel equivalent yet. Open an issue at https://gitlab.com/TwistCoin/twistcoin-core/issues.

Building
---------------------
The following are developer notes on how to build TwistCoin Core on your native platform. These are inherited unchanged from upstream Bitcoin Core and remain accurate for the build system itself (library dependencies, compile flags, etc.) - TwistCoin's changes are in consensus parameters and chain identity, not the build tooling.

- [Dependencies](dependencies.md)
- [macOS Build Notes](build-osx.md)
- [Unix Build Notes](build-unix.md)
- [Windows Build Notes](build-windows-msvc.md)
- [FreeBSD Build Notes](build-freebsd.md)
- [OpenBSD Build Notes](build-openbsd.md)
- [NetBSD Build Notes](build-netbsd.md)

Development
---------------------
Most of the documents below are inherited unchanged from upstream Bitcoin Core and describe engineering practices, not TwistCoin-specific policy or community process.

- [Developer Notes](developer-notes.md)
- [Productivity Notes](productivity.md)
- [JSON-RPC Interface](JSON-RPC-interface.md)
- [Unauthenticated REST Interface](REST-interface.md)
- [BIPS](bips.md) (the Bitcoin protocol specs TwistCoin's codebase still implements, being a Bitcoin Core fork)
- [Dnsseed Policy](dnsseed-policy.md)
- [Benchmarking](benchmarking.md)
- [Internal Design Docs](design/)

### Miscellaneous
- [CJDNS Support](cjdns.md)
- [Files](files.md)
- [Fuzz-testing](fuzzing.md)
- [I2P Support](i2p.md)
- [Init Scripts (systemd/upstart/openrc)](init.md)
- [Managing Wallets](managing-wallets.md)
- [Multisig Tutorial](multisig-tutorial.md)
- [Offline Signing Tutorial](offline-signing-tutorial.md)
- [P2P bad ports definition and list](p2p-bad-ports.md)
- [PSBT support](psbt.md)
- [Reduce Memory](reduce-memory.md)
- [Reduce Traffic](reduce-traffic.md)
- [Tor Support](tor.md)
- [Transaction Relay Policy](policy/README.md)
- [ZMQ](zmq.md)

License
---------------------
Distributed under the [MIT software license](/COPYING).
