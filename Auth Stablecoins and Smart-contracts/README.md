# Auth Stablecoin in  Smart contracts
This contract simplifies third-party developers working with PlasmaPay stable coins.
Assumes setting permission **auth.coinpub** if your contract will use stablecoins.

## Following actions
The contract implements the following actions to integrate currency acceptance:

```
version()

transfer(name from, name to, const std::string& quantity, bool paySysCms, const std::string& memo)

withdraw(name account, const std::string& quantity, bool paySysCms)
```

# Description permission actions

## Action "version"
Prints the current version of the **coin.pub** contract to the trace. This account is not authorized, any blockchain account can call it.
## Action "transfer"
The "transfer" action transfers "quantity" tokens from account "from" account "to". The action is authorized by the "from" account.
## Action "withdraw"
The "withdraw" action outputs the "quantity" tokens from the blockchain. Thus, the withdrawal of tokens in the amount of "quantity" from circulation is implemented. This number of tokens compensates for the unemitted balance of blockchain tokens. The action is authorized by the account "account" - this is the owner of the wallet, which displays tokens from circulation.
## Action "version" and "withdraw"
The "version" and "withdraw" actions are authorized by permissions **auth.coinpub**, therefore their authorizing accounts (actors) must additionally add this permission to themselves, both on their own account and on the execution of these actions. Such a measure is intended to prevent the actor from using the default permissions - "active" and therefore its possible compromise by a third party, for example, an application using this contract.

##Installation Examples

The result of the installation of permission to the account (note, not only adding the **auth.coinpub** permission itself, but also installing the **coin.pub@ion.code** service account):

```
sol get account accountname
# permissions:
#         active     1:    1 PLASMA*********, 1 plasma.user@active
#         auth.coinpub     1:    1 PLASMA*********, 1 coin.pub@ion.code
```
Setting account permissions for executing contract actions (note, permissions are set not only on **coin.pub** contract actions, but also on coin contract actions, this is due to the implementation of the mechanism under consideration):

```
# transfer
sol set action permission accountname coin.pub transfer auth.coinpub
# transfer
sol set action permission accountname coin transfer auth.coinpub
# withdraw
sol set action permission accountname coin.pub withdraw auth.coinpub
# withdraw
sol set action permission accountname coin withdraw auth.coinpub
```
