#include "escrowuser.hpp"

#include <ion/transaction.hpp>
#include <ion/system.hpp>
#include <ion/print.hpp>

//======================================================================================================================
escrowuser::escrowuser(name receiver, name code, ion::datastream<const char*> ds)
  : contract(receiver, code, ds)
  , paramsGlobal_(receiver, code.value)
{
  paramsCurrent_ = paramsGlobal_.get_or_default();
}

//----------------------------------------------------------------------------------------------------------------------
escrowuser::~escrowuser()
{
  // Save global params
  paramsGlobal_.set( paramsCurrent_, get_self());
}

//----------------------------------------------------------------------------------------------------------------------
void escrowuser::open(uint64_t dealId, name payer, name receiver, const std::string& quantity)
{
  require_auth(payer);

  ion::action act(
        ion::permission_level{paramsCurrent_.callee,"active"_n},
        paramsCurrent_.callee,
        "openself"_n,
        std::make_tuple(dealId, payer, receiver, quantity)
        );

  act.send();
}

//----------------------------------------------------------------------------------------------------------------------
void escrowuser::closedeal(uint64_t dealId)
{
  TableDeals deals_(paramsCurrent_.callee, paramsCurrent_.callee.value);
  auto iter = deals_.find(dealId);

  if (deals_.end() == iter)
  {
    check(false, std::string("Deal ") + plasma::to_string(dealId) + " is not opened");
  }

  require_auth(iter->payer);

  ion::action act(
        ion::permission_level{paramsCurrent_.callee,"active"_n},
        paramsCurrent_.callee,
        "closeself"_n,
        std::make_tuple(dealId)
        );

  act.send();
}

//----------------------------------------------------------------------------------------------------------------------
void escrowuser::setcallee(name callee)
{
  require_auth(get_self());
  ion::check(ion::is_account(callee), "Callee account is invalid");

  paramsCurrent_.callee = callee;
}

//----------------------------------------------------------------------------------------------------------------------
ION_DISPATCH(escrowuser,
             (open)
             (closedeal)
             (setcallee))
