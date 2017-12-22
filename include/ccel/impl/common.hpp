#ifndef _CCEL_IMPL_COMMON_HPP
#define _CCEL_IMPL_COMMON_HPP

// Invoking Boost headers
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp> // For enabling shared smart ptrs from objects inside
#include <boost/function.hpp>
#include <boost/pool/object_pool.hpp> // For boost::object_pool
#include <boost/smart_ptr.hpp> // For boost::shared_ptr,boost::scoped_ptr,boost::unique_ptr
#include <boost/system/error_code.hpp>
#include <boost/thread.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include <limits> // For easy detection of intergral types maxium
// CMMENTS ABOUT WHY NOT USING STANDARDS SMART POINTERS
// The main aim is to fit different version of compilers -- if a compiler is
// NOT fully C++11 supported, invoking Boost's smart pointer is the only
// convinient solution to ensure RTTR pointers

namespace ccel {

class uuid_t : public boost::uuids::uuid {
public:
  uuid_t() : boost::uuids::uuid(rand_uuid()()){};
  uuid_t(int) : boost::uuids::uuid(boost::uuids::nil_uuid()){};
  uuid_t(const char *str) : boost::uuids::uuid(string_uuid()(str)){};
  explicit uuid_t(const boost::uuids::uuid &u) : boost::uuids::uuid(u){};

private:
  static boost::uuids::random_generator &rand_uuid() {
    static boost::uuids::random_generator __gen;
    return __gen;
  }

  static boost::uuids::string_generator &string_uuid() {
    static boost::uuids::string_generator __gen;
    return __gen;
  }
  operator boost::uuids::uuid();
  operator boost::uuids::uuid() const;
};

class base_handler : boost::noncopyable { // A handler must not be copyable
  // A base handler that most every type of protocol sessions inhires from
public:
  base_handler(boost::asio::io_service &io, std::size_t buffer_size = 0)
      : __handler_io(io), _handler_uuid(), _buffer_size(buffer_size){};
  // The return value holds a error code, 0 for success
  virtual int open() = 0; // open a handler,make it avaliable (can receive data)
  virtual int close() = 0;             // close a handler
  virtual int refresh() { return 0; }; // refresh its buffer
  virtual const std::size_t buffer_size() noexcept { return _buffer_size; }
  virtual const bool opened() noexcept { return _open; }
  const uuid_t uuid() noexcept { return _handler_uuid; }
  boost::asio::io_service &get_handler_io() { return __handler_io; }

protected:
  boost::asio::io_service &__handler_io;
  uuid_t _handler_uuid;
  std::size_t _buffer_size; // buffer size in Byte
  bool _open;               // Is it opened?
};

template <typename _T_Handler> // The type of the handler
class handler_pool : boost::object_pool<_T_Handler>,
                     boost::enable_shared_from_this<handler_pool<_T_Handler>> {
public:
  typedef boost::object_pool<_T_Handler> __handler_type;
  handler_pool(std::size_t pool_size = 0, std::size_t handler_limit = 0)
      : _pool_size(0), _handler_limit(0){};
  template <typename... Args> // The list of parameters
  _T_Handler *
  construct(Args &&... __args) { // The exception handling wrapper of
                                 // object_pool's construct func;
    if (_pool_size)              // there is a limitation of pool size (!=0)
      if (_size_allocated + sizeof(__handler_type) > _pool_size)
        throw std::logic_error("Pool size limitation readched.");
    // no limitation of pool size, need to avoid run out of memory
    if (_handler_limit) { // there is limitaions of handler numbers
      if (_handler_limit == std::numeric_limits<std::size_t>::max()) {
        // Ok,the limitation is the max of size_t
        if (_handler_count + 1 < _handler_limit) // reached the maxium
          throw std::logic_error("Pools handler limitation reached.");
      } else if (_handler_count + 1 > _handler_limit) // reaced maxium
        throw std::bad_alloc();
    }
    try {
      auto __tmp_ptr_holder = __handler_type::construct(
          std::forward<Args>(__args)...); // this may cause std::bad_alloc
      _size_allocated += sizeof(__handler_type);
      _handler_count++;
      return __tmp_ptr_holder;
    } catch (std::bad_alloc &e) { // oops,no memory
      return nullptr;             // no need to do handling
      // cuz there're no resource applied by this pool in the thrown
    } catch (...) {
      throw;
    } // rethrow other I-DONT-CARE exceptions
  }
  // TODO: Add hooking Functions
  const std::size_t allocated() noexcept {
    return _size_allocated;
  }; // return allocated size in Bytes
  const std::size_t size() noexcept {
    return _handler_count;
  } // return the number of handles in pool

  ~handler_pool() = default;

private:
  boost::asio::io_service _io_service; // The io_service
  boost::thread_group _workers_group;  // The worker thread group
  long long _size_allocated;           // Allocated size
  std::size_t _handler_count;          // The number of living handlers in pool
  std::size_t _pool_size;              // The pool size limitation(in Byte)
  std::size_t _handler_limit; // The limitation numbers of handlers in pool
};
}

#endif
