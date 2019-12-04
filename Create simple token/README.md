# Create token on PlasmaDLT

## Requirements
* SOL - is a command line tool that interfaces with the REST API exposed by ionode
* PLK - used to store private keys that SOL will use to sign transactions sent to the block chain
* ION.CDT - is a suite of tools used to build PLASMA contracts


## Networks name
Testnet - Friedman
Mainet - Liberty (mainet)
Public Explorer - https://plasmadlt.com/
Official wallet - https://plasmapay.com/
Github - https://github.com/plasmadlt

## Install
Create directories for producer node data

```
mkdir /data
mkdir /data/plasma
mkdir /data/plasma/ioncdt
mkdir /data/plasma/bios
mkdir /data/plasma/bios/wallet
```
Get the docker image ION.CDT
```
docker pull plasmachain/ion.cdt
```
Get the docker image PLK- wallet, SOL - cli
```
docker pull plasmachain/plasma-local-node
```

## Setting up Configurations

Create account
* If you run Testnet Node you need to create an official testnet account in official wallet PlasmaPay settings: https://app.plasmapay.com/id/profile/developers
* If you run Mainet Node, you need to activate the main account in official wallet PlasmaPay on the dashboard: https://app.plasmapay.com/dashboard and decrypt your private keys (key icon on the wallet).
**Attention:** you can create only one account in PlasmaPay and the account's name will be your node's name.

## Tips
```
ion.token.abi — abi is short for “application binary interface”, it’s basically a JSON file that knows where the hooks live once we compile our source into webassembly, more on this later.
ion.token.cpp — c++ file, this is where our C++ source lives, describing general rules and regulations, fully enough to provide user's with custom tokens creation. An example of source code, based on custom tokens can be created and manipulated.
ion.token.hpp — h++ file, normal header file stuff. Definitions and types declarations, used for describing custom token. An example, demonstrating - easy and user-friendly development process.
CMakeLists.txt — Delete this, we’re not going to use CMake right now
```
## Rename our example token files with respect to Your's 12 symbols token name
```
$ mv ion.token.cpp yourtoken111.cpp
$ mv ion.token.hpp yourtoken111.hpp
$ mv ion.token.abi yourtoken111.abi
```

## Build The Contract:
```
$ :~/ yourtoken111$ make
#Building
docker run -it --entrypoint /contacts/build.sh -v <contract source folder>:/contacts  plasmachain/ion.cdt:latest
~/ yourtoken111 ls
Makefile  yourtoken111.abi  yourtoken111.cpp  yourtoken111.hpp  yourtoken111.wasm
```

## Tips
```
yourtoken111.abi — abi is short for “application binary interface”, it’s basically a JSON file that knows where the hooks live once we compile our source into webassembly, more on this later.
yourtoken111.cpp — c++ file, this is where our C++ source lives. Briefly described above.
yourtoken111.hpp — h++ file, normal header file stuff. Briefly described above.
CMakeLists.txt — Delete this, we’re not going to use CMake right now
```

## Deploy contract in Mainet using active key
Choice producer from the network testnet or mainet from http://plasmadlt.com/monitor
```
docker exec -i <network> sol --url  --wallet-url  set contract accountname /host-share/yourtoken111

Publishing contract...
executed transaction: 95c92e2e***...  672 bytes  4891 us
#           ion <= ion::setcode                 {"account":"accountname","vmtype":0,"vmversion":0,"code":"006173****...
#           ion <= ion::setabi                  {"account":"accountname","abi":"0c696f6e3a3a6****...
```

Setup your create token

```
docker exec -i <network> sol --url --wallet-url  push action yourtoken111 create '{"issuer": «accountname», "maximum_supply": "10000.0000 <YOURTOKENTICKER>"}' -p yourtoken111@active
```

Set up your token issue

```
docker exec -i <network> sol --url --wallet-url  sol push action <YOURTOKENTICKER> issue '{"to":"aaaaaaaaaaaa", "quantity":"10000", "paySysCms": false, "memo": "test issue"}' -p yourtoken111@active
```


Test deployed contract

```
docker exec -i <network> sol --url --wallet-url  push action yourtoken111 <action name>  '{"user": "accountname"}' -p yourtoken111@active
```
