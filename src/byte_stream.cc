#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ) {}

bool Writer::is_closed() const
{
  // Your code here.
  return is_closed_;
}

void Writer::push( string data )
{
  // Your code here.
  if ( is_closed() ) {
    return;
  }

  if ( data.size() > available_capacity() ) {
    data.resize( available_capacity() );
  }

  if ( !data.empty() ) {
    num_bytes_buffered_ += data.size();
    num_bytes_pushed_ += data.size();
    bytes_.emplace( move( data ) );
  }

  // 临界条件,pop空了bytes_
  // if (view_wnd_.empty() && !bytes_.empty() ) {
  //   view_wnd_ = bytes_.front();
  // }
}

void Writer::close()
{
  // Your code here.
  if ( !is_closed_ ) {
    is_closed_ = true;
    bytes_.emplace( string( 1, EOF ) );
  }
}

uint64_t Writer::available_capacity() const
{
  // Your code here.
  return capacity_ - num_bytes_buffered_;
}

uint64_t Writer::bytes_pushed() const
{
  // Your code here.
  return num_bytes_pushed_;
}

bool Reader::is_finished() const
{
  // Your code here.
  return is_closed_ && bytes_buffered() == 0;
}

uint64_t Reader::bytes_popped() const
{
  // Your code here.
  return num_bytes_popped_;
}

string_view Reader::peek() const
{
  // Your code here.
  return view_wnd_;
}

void Reader::pop( uint64_t len )
{
  // Your code here.
  if ( len >= view_wnd_.size() ) {
    bytes_.pop();
    view_wnd_ = bytes_.empty() ? ""sv : bytes_.front();
  } else
    view_wnd_.remove_prefix( len );
  num_bytes_buffered_ -= len;
  num_bytes_popped_ += len;
}

uint64_t Reader::bytes_buffered() const
{
  // Your code here.
  return num_bytes_buffered_;
}
