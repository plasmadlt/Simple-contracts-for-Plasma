#pragma once
#include <ion/serialize.hpp>
#include <ion/print.hpp>
#include <ion/symbol.hpp>
#include <ion/check.hpp>
#include <ion/system.hpp>

#include <tuple>
#include <limits>

#include "string_format.hpp"
#include "string_utils.hpp"

namespace plasma
{

/**
 *  Defines %CPP API for managing assets
 *  @addtogroup asset Asset CPP API
 *  @ingroup cpp_api
 *  @{
 */

//======================================================================================================================

/**
  * @struct Stores information for owner of asset
  */
class big_asset
{
public:
  using amount_t = int128_t;
  using unamount_t = uint128_t;

  /**
     * Maximum amount possible exponent value for this asset.
     */
  static constexpr auto max_exponent = ((sizeof(amount_t) * 8) - 2);

  static constexpr amount_t max_amount    = (amount_t(1) << max_exponent) - 1;

  /**
     * Construct a new asset from its string representation
     *
     * @param from - string representation
     */
  static big_asset from_string(const std::string& from);

  big_asset();
  big_asset(const big_asset&);

  /**
     * Construct a new asset given the symbol name and the amount
     *
     * @param a - The amount of the asset
     * @param s - The name of the symbol
     */
  big_asset( amount_t a, ion::symbol s );

  big_asset& operator=(const big_asset&);

  /**
     * Check if the amount doesn't exceed the max amount
     *
     * @return true - if the amount doesn't exceed the max amount
     * @return false - otherwise
     */
  bool is_amount_within_range() const { return -max_amount <= amount && amount <= max_amount; }

  /**
     * Check if the asset is valid. %A valid asset has its amount <= max_amount and its symbol name valid
     *
     * @return true - if the asset is valid
     * @return false - otherwise
     */
  bool is_valid() const { return is_amount_within_range() && symbol.is_valid(); }

  /**
     * Get the amount of the asset
     */
  const auto& get_amount() const { return amount; }

  /**
     * Set the amount of the asset
     *
     * @param a - New amount for the asset
     */
  void set_amount( amount_t a )
  {
    amount = a;
    value = to_string();
    ion::check( is_amount_within_range(), "magnitude of asset amount must be less than 2^" + std::to_string(max_exponent) );
  }

  /**
     * Get string value of the asset
     */
  const auto& get_value() const { return value; }

  /**
     * Unary minus operator
     *
     * @return asset - New asset with its amount is the negative amount of this asset
     */
  big_asset operator-()const
  {
    big_asset r = *this;
    r.amount = -r.amount;
    value = to_string();
    return r;
  }

  /**
     * Subtraction assignment operator
     *
     * @param a - Another asset to subtract this asset with
     * @return asset& - Reference to this asset
     * @post The amount of this asset is subtracted by the amount of asset a
     */
  big_asset& operator-=( const big_asset& a )
  {
    ion::check( a.symbol == symbol, "attempt to subtract asset with different symbol" );
    amount -= a.amount;
    value = to_string();
    ion::check( -max_amount <= amount, "subtraction underflow" );
    ion::check( amount <= max_amount,  "subtraction overflow" );
    return *this;
  }

  /**
     * Addition Assignment  operator
     *
     * @param a - Another asset to subtract this asset with
     * @return asset& - Reference to this asset
     * @post The amount of this asset is added with the amount of asset a
     */
  big_asset& operator+=( const big_asset& a )
  {
    ion::check( a.symbol == symbol, "attempt to add asset with different symbol" );
    amount += a.amount;
    value = to_string();
    ion::check( -max_amount <= amount, "addition underflow" );
    ion::check( amount <= max_amount,  "addition overflow" );
    return *this;
  }

  /**
     * Addition operator
     *
     * @param a - The first asset to be added
     * @param b - The second asset to be added
     * @return asset - New asset as the result of addition
     */
  inline friend big_asset operator+( const big_asset& a, const big_asset& b )
  {
    big_asset result = a;
    result += b;
    return result;
  }

  /**
     * Subtraction operator
     *
     * @param a - The asset to be subtracted
     * @param b - The asset used to subtract
     * @return asset - New asset as the result of subtraction of a with b
     */
  inline friend big_asset operator-( const big_asset& a, const big_asset& b )
  {
    big_asset result = a;
    result -= b;
    return result;
  }

  /**
     * @brief Multiplication assignment operator, with a number
     *
     * @details Multiplication assignment operator. Multiply the amount of this asset with a number and then assign the value to itself.
     * @param a - The multiplier for the asset's amount
     * @return asset - Reference to this asset
     * @post The amount of this asset is multiplied by a
     */
  big_asset& operator*=( amount_t a )
  {
    ion::check( amount <= max_amount / a, "multiplication overflow" );
    ion::check( amount >= (-max_amount) / a, "multiplication underflow" );
    amount = amount * a;
    value = to_string();
    ion::check( is_amount_within_range(), "invalid multiplication");
    return *this;
  }

  /**
     * Multiplication operator, with a number proceeding
     *
     * @brief Multiplication operator, with a number proceeding
     * @param a - The asset to be multiplied
     * @param b - The multiplier for the asset's amount
     * @return asset - New asset as the result of multiplication
     */
  friend big_asset operator*( const big_asset& a, amount_t b )
  {
    big_asset result = a;
    result *= b;
    return result;
  }

  /**
     * Multiplication operator, with a number preceeding
     *
     * @param a - The multiplier for the asset's amount
     * @param b - The asset to be multiplied
     * @return asset - New asset as the result of multiplication
     */
  friend big_asset operator*( amount_t b, const big_asset& a )
  {
    big_asset result = a;
    result *= b;
    return result;
  }

  /**
     * @brief Division assignment operator, with a number
     *
     * @details Division assignment operator. Divide the amount of this asset with a number and then assign the value to itself.
     * @param a - The divisor for the asset's amount
     * @return asset - Reference to this asset
     * @post The amount of this asset is divided by a
     */
  big_asset& operator/=( amount_t a )
  {
    ion::check( a != 0, "divide by zero" );
    ion::check( -max_amount <= a && a <= max_amount, "Invalid divider" );
    // ion::check( !(amount == std::numeric_limits<amount_t>::min() && a == -1), "signed division overflow" );
    amount /= a;
    value = to_string();
    ion::check( is_amount_within_range(), "Invalid division" );
    return *this;
  }

  /**
     * Division operator, with a number proceeding
     *
     * @param a - The asset to be divided
     * @param b - The divisor for the asset's amount
     * @return asset - New asset as the result of division
     */
  friend big_asset operator/( const big_asset& a, amount_t b )
  {
    big_asset result = a;
    result /= b;
    return result;
  }

  /**
     * Division operator, with another asset
     *
     * @param a - The asset which amount acts as the dividend
     * @param b - The asset which amount acts as the divisor
     * @return amount_t - the resulted amount after the division
     * @pre Both asset must have the same symbol
     */
  friend amount_t operator/( const big_asset& a, const big_asset& b )
  {
    ion::check( b.amount != 0, "divide by zero" );
    ion::check( a.symbol == b.symbol, "comparison of assets with different symbols is not allowed" );
    return a.amount / b.amount;
  }

  /**
     * Equality operator
     *
     * @param a - The first asset to be compared
     * @param b - The second asset to be compared
     * @return true - if both asset has the same amount
     * @return false - otherwise
     * @pre Both asset must have the same symbol
     */
  friend bool operator==( const big_asset& a, const big_asset& b )
  {
    ion::check( a.symbol == b.symbol, "comparison of assets with different symbols is not allowed" );
    return a.amount == b.amount;
  }

  /**
     * Inequality operator
     *
     * @param a - The first asset to be compared
     * @param b - The second asset to be compared
     * @return true - if both asset doesn't have the same amount
     * @return false - otherwise
     * @pre Both asset must have the same symbol
     */
  friend bool operator!=( const big_asset& a, const big_asset& b )
  {
    return !( a == b);
  }

  /**
     * Less than operator
     *
     * @param a - The first asset to be compared
     * @param b - The second asset to be compared
     * @return true - if the first asset's amount is less than the second asset amount
     * @return false - otherwise
     * @pre Both asset must have the same symbol
     */
  friend bool operator<( const big_asset& a, const big_asset& b )
  {
    ion::check( a.symbol == b.symbol, "comparison of assets with different symbols is not allowed" );
    return a.amount < b.amount;
  }

  /**
     * Less or equal to operator
     *
     * @param a - The first asset to be compared
     * @param b - The second asset to be compared
     * @return true - if the first asset's amount is less or equal to the second asset amount
     * @return false - otherwise
     * @pre Both asset must have the same symbol
     */
  friend bool operator<=( const big_asset& a, const big_asset& b )
  {
    ion::check( a.symbol == b.symbol, "comparison of assets with different symbols is not allowed" );
    return a.amount <= b.amount;
  }

  /**
     * Greater than operator
     *
     * @param a - The first asset to be compared
     * @param b - The second asset to be compared
     * @return true - if the first asset's amount is greater than the second asset amount
     * @return false - otherwise
     * @pre Both asset must have the same symbol
     */
  friend bool operator>( const big_asset& a, const big_asset& b )
  {
    ion::check( a.symbol == b.symbol, "comparison of assets with different symbols is not allowed" );
    return a.amount > b.amount;
  }

  /**
     * Greater or equal to operator
     *
     * @param a - The first asset to be compared
     * @param b - The second asset to be compared
     * @return true - if the first asset's amount is greater or equal to the second asset amount
     * @return false - otherwise
     * @pre Both asset must have the same symbol
     */
  friend bool operator>=( const big_asset& a, const big_asset& b )
  {
    ion::check( a.symbol == b.symbol, "comparison of assets with different symbols is not allowed" );
    return a.amount >= b.amount;
  }

  /**
     * %asset to std::string
     *
     * @brief %asset to std::string
     */
  std::string to_string() const;

  /**
     * %Print the asset
     *
     * @brief %Print the asset
     */
  void print() const
  {
    ion::print(to_string());
  }

  /**
    * The symbol name of the asset
    */
  ion::symbol symbol;

private:
  void validate_() const;
  void init_(const big_asset&);

  /**
    * The amount of the asset
    */
  amount_t amount = 0;

  /**
    * String representation of the asset value
    */
  mutable std::string value;

  IONLIB_SERIALIZE( big_asset, (symbol)(amount)(value) )
};

//----------------------------------------------------------------------------------------------------------------------
big_asset big_asset::from_string(const std::string& from)
{
  std::string s = plasma::trim_copy(from);

  // Find space in order to split amount and symbol
  const auto space_pos = s.find(' ');
  ion::check((space_pos != std::string::npos), "Asset's amount and symbol should be separated with space");

  // Amount and the symbol parts
  const auto symbol_str = plasma::trim_copy(s.substr(space_pos + 1));
  const auto amount_str = s.substr(0, space_pos);

  // Ensure that if decimal point is used (.), decimal fraction is specified
  std::string fractional_part;
  auto dot_pos = amount_str.find('.');
  if (dot_pos != std::string::npos)
  {
    ion::check((dot_pos != amount_str.size() - 1), "Missing decimal fraction after decimal point");
    fractional_part = amount_str.substr(dot_pos + 1);
  }
  else
  {
    ion::check((dot_pos != amount_str.size() - 1), "Missing decimal fraction after decimal point");
  }

  // constexpr symbol( std::string_view ss, uint8_t precision )
  ion::symbol sym(symbol_str, amount_str.size() - dot_pos - 1);
  ion::check(sym.is_valid(), "Invalid symbol");

  bool ok;
  amount_t v = plasma::to_number<amount_t>(amount_str.substr(0, dot_pos) + fractional_part, ok);
  ion::check(ok, "Asset is an invalid number");
  ion::check(-max_amount <= v && v <= max_amount, "Invalid to number conversion");

  return big_asset(v, sym);
}

//----------------------------------------------------------------------------------------------------------------------
big_asset::big_asset()
{
}

//----------------------------------------------------------------------------------------------------------------------
big_asset::big_asset(const big_asset& rhs)
{
  init_(rhs);
}

//----------------------------------------------------------------------------------------------------------------------
big_asset::big_asset(amount_t a, ion::symbol s)
  : amount(a)
  , symbol(s)
  , value(to_string())
{
  validate_();
}

//----------------------------------------------------------------------------------------------------------------------
big_asset& big_asset::operator=(const big_asset& rhs)
{
  init_(rhs);
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
std::string big_asset::to_string() const
{
  const auto sym = symbol.code().to_string();

  // Put symbol and decimal point
  std::string ret(sym.crbegin(), sym.crend());

  if (ret.size())
    ret += ' ';

  std::string partInt;
  std::string partDec;
  std::string* currentPart = &partDec;
  auto p = symbol.precision();
  for (amount_t tmp = amount < 0 ? -amount : amount; tmp != 0; --p)
  {
    if (0 == p)
      currentPart = &partInt;

    (*currentPart) += "0123456789"[tmp % 10];
    tmp /= 10;
  }

  // Complement decimal part
  if (partDec.size() < symbol.precision())
    partDec += std::string(symbol.precision() - partDec.size(), '0');

  // Complement integer part
  if (!partInt.size())
    partInt = '0';

  if (amount < 0)
    partInt += '-';

  ret += partDec;
  ret += ".";
  ret += partInt;

  return std::string(ret.crbegin(), ret.crend());
}

//----------------------------------------------------------------------------------------------------------------------
void big_asset::validate_() const
{
  static std::string errMsg = "magnitude of asset amount must be less than 2^" + std::to_string(max_exponent);
  ion::check( is_amount_within_range(), errMsg.c_str());
  ion::check( symbol.is_valid(),        "invalid symbol name" );
}

//----------------------------------------------------------------------------------------------------------------------
void big_asset::init_(const big_asset& rhs)
{
  amount = rhs.get_amount();
  symbol = rhs.symbol;
  value  = rhs.to_string();

  validate_();
}

//======================================================================================================================
/**
 * @struct Extended asset which stores the information of the owner of the asset
 */
struct extended_big_asset {
  /**
     * The asset
     */
  big_asset quantity;

  /**
     * The owner of the asset
     */
  ion::name contract;

  /**
     * Get the extended symbol of the asset
     *
     * @return extended_symbol - The extended symbol of the asset
     */
  ion::extended_symbol get_extended_symbol()const { return ion::extended_symbol{ quantity.symbol, contract }; }

  /**
     * Default constructor
     */
  extended_big_asset() = default;

  /**
     * Construct a new extended asset given the amount and extended symbol
     */
  //    extended_asset( plasma::asset::amount_t v, ion::extended_symbol s ):quantity(v,s.get_symbol()),contract(s.get_contract()){}
  /**
     * Construct a new extended asset given the asset and owner name
     */
  extended_big_asset( big_asset a, ion::name c ):quantity(a),contract(c){}

  /**
     * %Print the extended asset
     */
  void print()const {
     quantity.print();
     ::ion::print("@", contract);
  }

  /**
     *  Unary minus operator
     *
     *  @return extended_asset - New extended asset with its amount is the negative amount of this extended asset
     */
  extended_big_asset operator-()const {
    return {-quantity, contract};
  }

  /**
     * @brief Subtraction operator
     *
     * @details Subtraction operator. This subtracts the amount of the extended asset.
     * @param a - The extended asset to be subtracted
     * @param b - The extended asset used to subtract
     * @return extended_asset - New extended asset as the result of subtraction
     * @pre The owner of both extended asset need to be the same
     */
  friend extended_big_asset operator - ( const extended_big_asset& a, const extended_big_asset& b ) {
    ion::check( a.contract == b.contract, "type mismatch" );
    return {a.quantity - b.quantity, a.contract};
  }

  /**
     * @brief Addition operator
     *
     * @details Addition operator. This adds the amount of the extended asset.
     * @param a - The extended asset to be added
     * @param b - The extended asset to be added
     * @return extended_asset - New extended asset as the result of addition
     * @pre The owner of both extended asset need to be the same
     */
  friend extended_big_asset operator + ( const extended_big_asset& a, const extended_big_asset& b ) {
    ion::check( a.contract == b.contract, "type mismatch" );
    return {a.quantity + b.quantity, a.contract};
  }

  /// Addition operator.
  friend extended_big_asset& operator+=( extended_big_asset& a, const extended_big_asset& b ) {
    ion::check( a.contract == b.contract, "type mismatch" );
    a.quantity += b.quantity;
    return a;
  }

  /// Subtraction operator.
  friend extended_big_asset& operator-=( extended_big_asset& a, const extended_big_asset& b ) {
    ion::check( a.contract == b.contract, "type mismatch" );
    a.quantity -= b.quantity;
    return a;
  }

  /// Less than operator
  friend bool operator<( const extended_big_asset& a, const extended_big_asset& b ) {
    ion::check( a.contract == b.contract, "type mismatch" );
    return a.quantity < b.quantity;
  }


  /// Comparison operator
  friend bool operator==( const extended_big_asset& a, const extended_big_asset& b ) {
    return std::tie(a.quantity, a.contract) == std::tie(b.quantity, b.contract);
  }

  /// Comparison operator
  friend bool operator!=( const extended_big_asset& a, const extended_big_asset& b ) {
    return std::tie(a.quantity, a.contract) != std::tie(b.quantity, b.contract);
  }

  /// Comparison operator
  friend bool operator<=( const extended_big_asset& a, const extended_big_asset& b ) {
    ion::check( a.contract == b.contract, "type mismatch" );
    return a.quantity <= b.quantity;
  }

  /// Comparison operator
  friend bool operator>=( const extended_big_asset& a, const extended_big_asset& b ) {
    ion::check( a.contract == b.contract, "type mismatch" );
    return a.quantity >= b.quantity;
  }

  IONLIB_SERIALIZE( extended_big_asset, (quantity)(contract) )
};

/// @} asset type
}
