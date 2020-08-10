# network_lib
# Description
The libraray is using for handle unserlying socket process which made by C++ language.

# Dependency Library
We are using Boost ASIO library for implement asynchronous in network IO.
[Boost C++ Library](https://www.boost.org/users/download/)

And we also made a tester for HTTP request, using [googltest](https://github.com/google/googletest).

# Development Platform
MSVC++ 14.0

# API Introduction
## TCP
- **connect(ip_address, port)** / **async_connect(ip_address, port)**
  - Open a TCP connection with destination compose by **ip_address** and **port**.
- **disconnect()**
  - Teardown an exist TCP connection with destination.
- **send(data)** / **async_send(data)**
  - Send data to destination.
  - Before you call **send()**/**async_send()**, must be sure **connect()**/**async_connect** operation was succeeded.
- **read(receive_buffer)** / **async_read()**
  - Read data from socket buffer.
  - **read()** will block in function return until received data in socket layer.
  - **async_read()** will return immediately.
  - Before **async_read()** called, need to **set_data_callback()** to register event.
- **set_data_callback()**
  - Set a functor in obeject.
  - When socket received data, pass through data by functor.
- **set_error_callback()**
  - Set a functor in obeject.
  - When socket occurs error exception, pass through error message by functor.
  
## HTTP
