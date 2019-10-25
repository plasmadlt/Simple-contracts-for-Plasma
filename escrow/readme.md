# PlasmaDLT Escrow base protocol

## About
* This is a basic contract that will allow you to execute the logic of method calls of plasma stable tokens, to build the logic of safe transfers.
This example allows you to process user payments with additional logic.
Options:
The user sends money to another user with a time delay, the user can cancel the payment within 24 hours, otherwise the payment will be automatic, the sender can confirm the payment instantly.
Looks example flow with front-end
https://escrow.plasmapay.com/escrow/

## Create accounts
* You will need sol elements, you will have them when installing the ionode and ion.cdt check https://hub.docker.com/r/plasmachain/ion.cdt
- Create account for contract on https://plasmapay.com/


## Get permissions 
- Your contract requires permissions from the contract Escrow base protocol , ask the administrator to issue permissions to you.
```
# the administrator will run this command for your contract account
docker exec -i <network> sol --url http://barcluk.liberty.plasmadlt.com --wallet-url http://127.0.0.1:9999 set account permission rockyescrow1 active '{"keys": [{"key": "PLASMA*******************************", "weight": 1}], "threshold": 1, "accounts": [{"permission": {"actor": "escrowuser11", "permission": "ion.code"}, "weight": 1}, {"permission": {"actor": "rockyescrow1", "permission": "ion.code"}, "weight": 1}], "waits": []}' -p rockyescrow1@active
```

## Deploy contract
* You will need sol elements, you will have them when installing the ionode and ion.cdt check https://hub.docker.com/r/plasmachain/ion.cdt
- follow example
* Create Contract Hello world

```
mkdir hello
cd hello
```
Create contract in directory hello.cpp, collected contracts will be in the folder <contract source folder>/build

```
#include <ion/ion.hpp>

using namespace ion;

class [[ion::contract]] hello : public contract {
  public:
      using contract::contract;

      [[ion::action]]
      void hi( name user ) {
         print( "Hello, ", user);
      }
};
```

* Publishing contract
```
docker exec -i <network>-bios-node sol set contract hello .../hello -p hello@active
Reading WASM from  ..../hello/hello.wasm...
Publishing contract...
executed transaction: 95c92e2e***...  672 bytes  4891 us
#           ion <= ion::setcode                 {"account":"hello","vmtype":0,"vmversion":0,"code":"006173****...
#           ion <= ion::setabi                  {"account":"hello","abi":"0c696f6e3a3a6****...
```

* Push Action on contract
```
docker exec -i <network>-bios-node sol push action hello hi '["123"]' -p hello@active
executed transaction: 7bd013....***  104 bytes  1499 us
#         hello <= hello::hi                    {"user":"123"}
>> Hello, 123
```

# Deploy contract in Testnet  or Mainet using active key from network
*  Choice producer from the network testnet or mainet from http://plasmadlt.com/monitor.
```
docker exec -i <network> sol --url http://barcluk.liberty.plasmadlt.com --wallet-url http://127.0.0.1:9999 set contract accountname /host-share/helllloworld
```


* Test deployed contract

```
docker exec -i <network> sol --url http://barcluk.liberty.plasmadlt.com --wallet-url http://127.0.0.1:9999 push action accountname  <action name> '{"user": "accountname"}' -p accountname@active
```

```
docker exec -i <network> ol --url http://barcluk.liberty.plasmadlt.com --wallet-url http://127.0.0.1:9999 push action accountname <action name> '{"user": "accountname"}' -p accountname@active
```
