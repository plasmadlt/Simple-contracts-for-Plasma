#include "token.hpp"

#include "../core/string_format.hpp"

namespace plasma
{

//======================================================================================================================
void token::version()
{
  printVersion("0.3");
  ion::print(" Maximum value is ", BigAsset::max_amount);
}

//----------------------------------------------------------------------------------------------------------------------
void token::create(ion::name issuer, const std::string& maximum_supply)
{
  using namespace ion;
  const auto _maximum_supply = BigAsset::from_string(maximum_supply);

  require_auth( _self );

  auto sym = _maximum_supply.symbol;
  check( sym.is_valid(), "invalid symbol name" );
  check( _maximum_supply.is_valid(), "invalid supply");
  check( _maximum_supply.get_amount() > 0, "max-supply must be positive");

  stats statstable( _self, sym.code().raw() );
  auto existing = statstable.find( get_symbol_key(sym) );
  check( existing == statstable.end(), "token with symbol already exists" );

  statstable.emplace( _self, [&]( auto& s ) {
     s.supply       = BigAsset(0, _maximum_supply.symbol);
     s.max_supply   = _maximum_supply;
     s.issuer       = issuer;
  });
}

//----------------------------------------------------------------------------------------------------------------------
void token::issue(ion::name to, const std::string& quantity, const std::string& memo)
{
  using namespace ion;
  const auto _quantity =BigAsset::from_string(quantity);

  auto sym = _quantity.symbol;
  check( sym.is_valid(), "invalid symbol name" );
  check( memo.size() <= 256, "memo has more than 256 bytes" );

  stats statstable( _self, sym.code().raw() );
  auto existing = statstable.find( get_symbol_key(sym) );
  check( existing != statstable.end(), "token with symbol does not exist, create token before issue" );
  const auto& st = *existing;

  require_auth( st.issuer );
  check( _quantity.is_valid(), "invalid quantity" );
  check( _quantity.get_amount() > 0, "must issue positive quantity" );

  check( _quantity.symbol == st.supply.symbol, "symbol precision mismatch" );
  check( _quantity.get_amount() <= st.max_supply.get_amount() - st.supply.get_amount(), "quantity exceeds available supply");

  statstable.modify( st, same_payer, [&]( auto& s ) {
     s.supply += _quantity;
  });

  add_balance( st.issuer, _quantity, st.issuer );

  if( to != st.issuer ) {
//    SEND_INLINE_ACTION( *this, transfer, { { _self, "active"_n} },
//                        { st.issuer, to, quantity, memo }
//    );
    transfer_(st.issuer, to, _quantity, memo);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void token::retire( const std::string& quantity, const std::string& memo )
{
  using namespace ion;
  const auto _quantity = BigAsset::from_string(quantity);

  auto sym = _quantity.symbol;
  check( sym.is_valid(), "invalid symbol name" );
  check( memo.size() <= 256, "memo has more than 256 bytes" );

  stats statstable( _self, sym.code().raw() );
  auto existing = statstable.find( get_symbol_key(sym) );
  check( existing != statstable.end(), "token with symbol does not exist" );
  const auto& st = *existing;

  require_auth( st.issuer );
  check( _quantity.is_valid(), "invalid quantity" );
  check( _quantity.get_amount() > 0, "must retire positive quantity" );

  check( _quantity.symbol == st.supply.symbol, "symbol precision mismatch" );

  statstable.modify( st, same_payer, [&]( auto& s ) {
    s.supply -= _quantity;
  });

  sub_balance( st.issuer, _quantity );
}

//----------------------------------------------------------------------------------------------------------------------
void token::transfer(ion::name from, ion::name to, const std::string& quantity, const std::string& memo)
{
  using namespace ion;
  const auto _quantity = BigAsset::from_string(quantity);

  require_auth( _self );

  transfer_(from, to, _quantity, memo);
}

//----------------------------------------------------------------------------------------------------------------------
void token::subbalance(const std::string& quantity)
{
  retire(std::move(quantity), "retire");
}

//----------------------------------------------------------------------------------------------------------------------
void token::sub_balance(ion::name owner, const BigAsset& value)
{
  using namespace ion;

  accounts from_acnts( _self, owner.value );

  const auto& from = from_acnts.get( get_symbol_key(value.symbol), "no balance object found" );
  check( from.balance.get_amount() >= value.get_amount(), "overdrawn balance" );

  from_acnts.modify( from, same_payer, [&]( auto& a ) {
        a.balance -= value;
     });
}

//----------------------------------------------------------------------------------------------------------------------
void token::add_balance(ion::name owner, const BigAsset& value, ion::name /*ram_payer*/)
{
  using namespace ion;

  accounts to_acnts( _self, owner.value );
  auto to = to_acnts.find( get_symbol_key(value.symbol) );
  if( to == to_acnts.end() ) {
    to_acnts.emplace( _self, [&]( auto& a ){
      a.balance = value;
    });
  } else {
    to_acnts.modify( to, same_payer, [&]( auto& a ) {
      a.balance += value;
    });
  }
}

//----------------------------------------------------------------------------------------------------------------------
void token::open( ion::name owner, const ion::symbol& symbol, ion::name ram_payer )
{
  using namespace ion;

  require_auth( _self );

  stats statstable( _self, symbol.code().raw() );
  const auto& st = statstable.get( get_symbol_key(symbol), "symbol does not exist" );
  check( st.supply.symbol == symbol, "symbol precision mismatch" );

  accounts acnts( _self, owner.value );
  auto it = acnts.find( get_symbol_key(symbol) );
  if( it == acnts.end() ) {
    acnts.emplace( _self, [&]( auto& a ){
      a.balance = BigAsset{0, symbol};
    });
  }
}

//----------------------------------------------------------------------------------------------------------------------
void token::close( ion::name owner, const ion::symbol& symbol )
{
  using namespace ion;

  require_auth( _self );
  accounts acnts( _self, owner.value );
  auto it = acnts.find( get_symbol_key(symbol) );
  check( it != acnts.end(), "Balance row already deleted or never existed. Action won't have any effect." );
  check( it->balance.get_amount() == 0, "Cannot close because the balance is not zero." );
  acnts.erase( it );
}

//----------------------------------------------------------------------------------------------------------------------
void token::transfer_(ion::name from, ion::name to, const BigAsset& _quantity,  const string& memo)
{
  using namespace ion;

  check( from != to, "cannot transfer to self" );
  check( is_account( to ), "to account does not exist");
  const auto& sym = _quantity.symbol;
  stats statstable( _self, sym.code().raw() );
  const auto& st = statstable.get( get_symbol_key(sym) );

  require_recipient( from );
  require_recipient( to );

  check( _quantity.is_valid(), "invalid quantity" );
  check( _quantity.get_amount() > 0, "must transfer positive quantity" );
  check( _quantity.symbol == st.supply.symbol, "symbol precision mismatch" );
  check( memo.size() <= 256, "memo has more than 256 bytes" );

  auto payer = has_auth( to ) ? to : from;

  sub_balance( from, _quantity );
  add_balance( to, _quantity, payer );
}

} // namespace plasma

ION_DISPATCH(plasma::token, (version)(create)(issue)(transfer)(open)(close)(retire)(subbalance))
