#pragma once

#include <ion/ion.hpp>
#include <ion/singleton.hpp>

#include "big_asset.hpp"

using namespace ion;

//======================================================================================================================
CONTRACT escrowuser : public contract
{
public:
  escrowuser(name receiver, name code, ion::datastream<const char*> ds);
  ~escrowuser();

  ACTION open(uint64_t dealId, name payer, name receiver, const std::string& quantity);
  ACTION closedeal(uint64_t dealId);
  ACTION setcallee(name callee);

private:
  static constexpr ion::name accountEscrowBase = ion::name("escrow");

  TABLE params
  {
    name callee = accountEscrowBase;
  };
  using TableParams = ion::singleton<name("params"), params>;

  TABLE deal
  {
    uint64_t id;
    name payer;
    name receiver;
    plasma::big_asset quantity;
    uint64_t closeAfter;

    uint64_t primary_key() const { return id; }
    uint64_t by_close() const { return closeAfter; }
  };

  static constexpr ion::name nmByCloseAfter = ion::name("dealsbyclose");
  using fnByCloseAfter = ion::const_mem_fun<deal, uint64_t, &deal::by_close>;
  using TableDeals = ion::multi_index<name("deals"), deal, ion::indexed_by<nmByCloseAfter, fnByCloseAfter>>;

  TableParams paramsGlobal_;
  params paramsCurrent_;
};
