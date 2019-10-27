#pragma once

#include <ion/ion.hpp>

#include "../core/asset.hpp"
#include "../core/contract.hpp"
#include "../core/token_types.hpp"

//======================================================================================================================
namespace plasma
{

CONTRACT token : public plasma::contractBase
{
public:
  using string = std::string;
  using BigAsset = plasma::big_asset;

  token(ion::name receiver, ion::name code, ion::datastream<const char*> ds)
    : plasma::contractBase(receiver, code, ds) {}

  ACTION version();
  ACTION create(ion::name issuer, const std::string& maximum_supply);
  ACTION issue(ion::name to, const std::string& quantity, const std::string& memo);
  ACTION retire(const std::string& quantity, const std::string& memo);
  ACTION transfer(ion::name from, ion::name to, const std::string& quantity, const std::string& memo);
  ACTION open(ion::name owner, const ion::symbol& symbol, ion::name ram_payer);
  ACTION close(ion::name owner, const ion::symbol& symbol);
  ACTION subbalance(const std::string& quantity);

  static BigAsset get_supply( ion::name token_contract_account, const ion::symbol_code& sym_code )
  {
     stats statstable( token_contract_account, sym_code.raw() );
     const auto& st = statstable.get( sym_code.raw() );
     return st.supply;
  }

  static BigAsset get_balance( ion::name token_contract_account, ion::name owner, const ion::symbol_code& sym_code )
  {
     accounts accountstable( token_contract_account, owner.value );
     const auto& ac = accountstable.get( sym_code.raw() );
     return ac.balance;
  }

private:
  void sub_balance(ion::name owner, const BigAsset& value);
  void add_balance(ion::name owner, const BigAsset& value, ion::name ram_payer);

  inline uint64_t get_symbol_key(const ion::symbol& symbol) const { return symbol.code().raw(); }

  void transfer_(ion::name from, ion::name to, const BigAsset& value, const string& memo);

  ACCOUNT_STRUCT(Account, BigAsset)
  using accounts = ion::multi_index<plasma::tokenTypes::accountsTableName, Account>;

  STATS_STRUCT(CurrencyStat, BigAsset)
  using stats = ion::multi_index<plasma::tokenTypes::statTableName, CurrencyStat>;
};

} // namespace plasma
