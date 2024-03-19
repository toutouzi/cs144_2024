#include "wrapping_integers.hh"

using namespace std;

Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
  // Your code here.
  return Wrap32( zero_point + static_cast<uint32_t>( n ) );
}

uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{
  // Your code here.
  const constexpr uint64_t Upper_32 = 1l << 32;
  const constexpr uint64_t half_32 = 1l << 31;
  const uint32_t ckpt = wrap( checkpoint, zero_point ).raw_value_;
  uint32_t offset = raw_value_ - ckpt;

  if ( offset < half_32 || checkpoint + offset < Upper_32 ) {
    return checkpoint + offset;
  }
  return checkpoint + offset - Upper_32;
}
