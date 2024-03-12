#pragma once

#include "byte_stream.hh"

#include <list>
#include <set>
#include <string>
#include <tuple>

class Reassembler
{
public:
  // Construct Reassembler to write into given ByteStream.
  explicit Reassembler( ByteStream&& output ) : output_( std::move( output ) ) {}

  /*
   * Insert a new substring to be reassembled into a ByteStream.
   *   `first_index`: the index of the first byte of the substring
   *   `data`: the substring itself
   *   `is_last_substring`: this substring represents the end of the stream
   *   `output`: a mutable reference to the Writer
   *
   * The Reassembler's job is to reassemble the indexed substrings (possibly out-of-order
   * and possibly overlapping) back into the original ByteStream. As soon as the Reassembler
   * learns the next byte in the stream, it should write it to the output.
   *
   * If the Reassembler learns about bytes that fit within the stream's available capacity
   * but can't yet be written (because earlier bytes remain unknown), it should store them
   * internally until the gaps are filled in.
   *
   * The Reassembler should discard any bytes that lie beyond the stream's available capacity
   * (i.e., bytes that couldn't be written even if earlier gaps get filled in).
   *
   * The Reassembler should close the stream after writing the last byte.
   */
  void insert( uint64_t first_index, std::string data, bool is_last_substring );

  // How many bytes are stored in the Reassembler itself?
  uint64_t bytes_pending() const;

  // Access output stream reader
  Reader& reader() { return output_.reader(); }
  const Reader& reader() const { return output_.reader(); }

  // Access output stream writer, but const-only (can't write from outside)
  const Writer& writer() const { return output_.writer(); }

private:
  ByteStream output_; // the Reassembler writes to this ByteStream

  // denghaowen
  uint64_t next_assembel_index_ {}; // 下一个期待的字节序号
  uint64_t num_bytes_pending_ {};
  uint64_t unacceptable_index_ {};

  // using TupleType = std::tuple<uint64_t, std::string, bool>;
  std::list<std::tuple<uint64_t, std::string, bool>> Unassembled_ {}; // 一个有序的、无重复的缓冲区
  //  std::vector<std::tuple<uint64_t, std::string, bool>> Unassembled_ {};

  void push_bytes( uint64_t first_index, std::string data, bool is_last_substring );
  void cache_bytes( uint64_t first_index, std::string data, bool is_last_substring );
  void flush_buffer(); // 刷新缓冲区，把能推入的数据推入流中

  // int merge_substring(uint64_t index, std::string &data, uint64_t index2, std::string data2);
};
