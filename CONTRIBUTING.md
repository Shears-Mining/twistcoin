# Contributing to TwistCoin Core

TwistCoin Core is a small, independent fork of [Bitcoin Core](https://github.com/bitcoin/bitcoin). There's no established contributor community, review process, or CI infrastructure of its own yet - this file will grow as the project does.

In the meantime:

- Open issues and pull requests against https://gitlab.com/TwistCoin/twistcoin-core.
- Coding style, commit hygiene, and general software-engineering practices largely follow Bitcoin Core's own conventions (see [doc/developer-notes.md](doc/developer-notes.md), inherited unchanged from upstream and still accurate). Bitcoin Core's own [CONTRIBUTING.md](https://github.com/bitcoin/bitcoin/blob/master/CONTRIBUTING.md) is a good reference for the philosophy behind peer review and PR workflow in this codebase, even though its specific links (IRC channels, PR review club, Transifex) are Bitcoin Core's own infrastructure, not TwistCoin's.
- Unit tests: `ctest`. Functional/integration tests: `build/test/functional/test_runner.py`. See [src/test/README.md](src/test/README.md) and [test/](test).
