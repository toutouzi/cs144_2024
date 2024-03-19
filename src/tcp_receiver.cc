#include "tcp_receiver.hh"

using namespace std;

void TCPReceiver::receive( TCPSenderMessage message )
{
  // Your code here.
  // ISN_.has_value()功能是+1， 不直接+1的是因为第一个报文为0；
  const uint64_t checkpoint = reassembler_.writer().bytes_pushed() + ISN_.has_value();
  if ( message.SYN ) {
    if ( !ISN_.has_value() ) {
      ISN_ = message.seqno;
    }
  } else if ( message.seqno == ISN_ ) {
    return;
  }

  if ( message.RST ) {
    TCPReceiver::reader().set_error(); // 不能调用writer(),因为writer()没有non_const的函数，set_error是non_error
  }

  const uint64_t abseqno = message.seqno.unwrap( *ISN_, checkpoint );
  TCPReceiver::reassembler_.insert( abseqno == 0 ? abseqno : abseqno - 1, move( message.payload ), message.FIN );
}

TCPReceiverMessage TCPReceiver::send() const
{
  // Your code here.
  TCPReceiverMessage rev_message {};
  if ( ISN_.has_value() ) {
    rev_message.ackno = Wrap32::wrap(
      reassembler_.writer().bytes_pushed() + ISN_.has_value() + reassembler_.writer().is_closed(), *ISN_ );
  }

  uint64_t capacity = reassembler_.writer().available_capacity();
  rev_message.window_size = capacity > UINT16_MAX ? UINT16_MAX : capacity;

  if ( TCPReceiver::writer().has_error() ) {
    rev_message.RST = true;
  }

  return rev_message;
}
