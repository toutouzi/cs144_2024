#include "reassembler.hh"
#include <algorithm>

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  // Your code here.
  unacceptable_index_ = next_assembel_index_ + output_.writer().available_capacity();
  uint64_t last_index_ = first_index + data.size();

  if ( first_index >= unacceptable_index_ || last_index_ < next_assembel_index_ || output_.writer().is_closed()
       || output_.writer().available_capacity() == 0 )
    return;

  if ( last_index_ >= unacceptable_index_ ) {
    data.resize( unacceptable_index_ - first_index );
    if ( is_last_substring )
      is_last_substring = false;
  }

  if ( first_index <= next_assembel_index_ ) {
    push_bytes( first_index, move( data ), is_last_substring );
  } else {
    cache_bytes( first_index, move( data ), is_last_substring );
  }
  flush_buffer();
}

uint64_t Reassembler::bytes_pending() const
{
  // Your code here.
  return num_bytes_pending_;
}

// denghoawen
void Reassembler::push_bytes( uint64_t first_index, std::string data, bool is_last_substring )
{
  if ( next_assembel_index_ > first_index )
    data.erase( 0, next_assembel_index_ - first_index );
  next_assembel_index_ += data.size();
  output_.writer().push( move( data ) );

  // 处理最后一个
  if ( is_last_substring ) {
    output_.writer().close();
    Unassembled_.clear();
    num_bytes_pending_ = 0;
  }
}

void Reassembler::cache_bytes( uint64_t first_index, std::string data, bool is_last_substring )
{
  // left是第一个重叠的，right是可能重叠的下一个
  const uint64_t next_data_idx = first_index + data.size();
  auto end = Unassembled_.end();
  auto left = lower_bound( Unassembled_.begin(), end, first_index, []( auto& e, uint64_t idx ) -> bool {
    return idx > ( get<0>( e ) + get<1>( e ).size() );
  } );
  auto right = upper_bound(
    left, end, next_data_idx, []( uint64_t nxt_idx, auto& e ) -> bool { return nxt_idx < get<0>( e ); } );

  //
  while ( left != right ) {
    auto& [left_idx, dat, last] = *left;
    uint64_t right_idx = left_idx + dat.size() - 1;

    if ( first_index >= left_idx && next_data_idx - 1 <= right_idx ) {
      return;
    }
    num_bytes_pending_ -= dat.size();
    is_last_substring |= last;
    if ( first_index <= left_idx && next_data_idx - 1 >= right_idx ) {
      // 不能删除
    } else if ( first_index >= left_idx ) {
      data.insert( 0, string_view( dat.c_str(), first_index - left_idx ) );
    } else {
      data.resize( left_idx - first_index );
      data.append( dat );
    }
    first_index = min( first_index, left_idx );
    left = Unassembled_.erase( left );
  }
  num_bytes_pending_ += data.size();
  Unassembled_.insert( left, { first_index, move( data ), is_last_substring } );
}

void Reassembler::flush_buffer()
{
  while ( !Unassembled_.empty() ) {
    auto [idx, dat, last] = Unassembled_.front();
    if ( idx > next_assembel_index_ ) {
      return;
    }

    num_bytes_pending_ -= dat.size();
    push_bytes( idx, move( dat ), last );
    if ( !Unassembled_.empty() )
      Unassembled_.pop_front(); //
  }
}