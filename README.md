Daily Crypto Giveaways (DCG) Core — official repository
=======================================================

DCG is back: version 3.0.0 "Reborn", based on PIVX 5.6.1, continues the original 2020 Daily Crypto Giveaways project
(same chain, same coins, same binaries `dailycrypto-qt`, `dailycryptod`, `dailycrypto-cli`).

## Official sources

This repository (`Julien59247787/DCG-Crypto`, branch `dcg-3.0`) is the **official source** of DCG Core, maintained by the relaunch team.
Only trust binaries published here, checked against the `SHA256SUMS` file of the release and its signature `SHA256SUMS.asc`.

| | |
|---|---|
| Downloads | https://github.com/Julien59247787/DCG-Crypto/releases |
| Dashboard | https://dashboard.dailycryptogiveaways.eu |
| Explorer | https://explorer.dailycryptogiveaways.eu |
| Discord | https://discord.gg/ZvHB9WRTwt |
| Seed node | `seed.dailycryptogiveaways.eu:6744` |
| Release signing key (GPG) | `E579 9640 87AE 4B7F C41C  8720 400B 5E10 CAB9 C6AE` ([contrib/gpg/dcg-release-signing-key.asc](contrib/gpg/dcg-release-signing-key.asc)) |
| Network spork public key | `032f60ba8dfcc412635e411b7834789af646236146de3f3aebcf9935eeb997e35f` |

Copies or forks of this code are not official, even if they use the DCG name. The old `crypto-giveaways.cash` domain no longer
belongs to the project. The team will never ask you for coins, keys or your seed in a private message.

## Credits

DCG Core is built on [PIVX Core](https://github.com/PIVX-Project/PIVX) (itself derived from Dash and Bitcoin Core) and on the
original DCG code by the 2020 team ([Steve-Coding/DCG-Crypto](https://github.com/Steve-Coding/DCG-Crypto)). Thank you to all of them.

## License
PIVX Core is released under the terms of the MIT license. See [COPYING](https://github.com/PIVX-Project/PIVX/blob/master/COPYING) for more information or see https://opensource.org/licenses/MIT.

## Development Process

The master branch is regularly built (see doc/build-*.md for instructions) and tested, but it is not guaranteed to be completely stable. [Tags](https://github.com/PIVX-Project/PIVX/tags) are created regularly from release branches to indicate new official, stable release versions of PIVX Core.

The contribution workflow is described in [CONTRIBUTING.md](https://github.com/PIVX-Project/PIVX/blob/master/CONTRIBUTING.md) and useful hints for developers can be found in [doc/developer-notes.md](https://github.com/PIVX-Project/PIVX/blob/master/doc/developer-notes.md).

## Testing

Testing and code review is the bottleneck for development; we get more pull requests than we can review and test on short notice. Please be patient and help out by testing other people's pull requests, and remember this is a security-critical project where any mistake might cost people a lot of money.

## Automated Testing

Developers are strongly encouraged to write [unit tests](https://github.com/PIVX-Project/PIVX/blob/master/src/test/README.md) for new code, and to submit new unit tests for old code. Unit tests can be compiled and run (assuming they weren't disabled in configure) with: make check. Further details on running and extending unit tests can be found in [/src/test/README.md](https://github.com/PIVX-Project/PIVX/blob/master/src/test/README.md).

There are also regression and integration tests, written in Python. These tests can be run (if the test dependencies are installed) with: test/functional/test_runner.py`

The CI (Continuous Integration) systems make sure that every pull request is built for Windows, Linux, and macOS, and that unit/sanity tests are run automatically.

## Manual Quality Assurance (QA) Testing

Changes should be tested by somebody other than the developer who wrote the code. This is especially important for large or high-risk changes. It is useful to add a test plan to the pull request description if testing the changes is not straightforward.

## Translations

Changes to translations as well as new translations can be submitted to PIVX Core's Transifex page.

Translations are periodically pulled from Transifex and merged into the git repository. See the [translation process](https://github.com/PIVX-Project/PIVX/blob/master/doc/translation_process.md) for details on how this works.

Important: We do not accept translation changes as GitHub pull requests because the next pull from Transifex would automatically overwrite them again.
