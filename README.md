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
## TCP Client
- **connect(ip_address, port)** / **async_connect(ip_address, port)**
  - Open a TCP connection with destination which composed by **ip_address** and **port**.
- **disconnect()**
  - Teardown an exist TCP connection with destination.
- **send(data)** / **async_send(data)**
  - Send data to destination.
  - Before you call **send()**/**async_send()**, must be sure **connect()**/**async_connect** operation was succeeded.
- **read(&receive_buffer)** / **async_read()**
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
  
## HTTP Client
- **send_get_command(ip_address, port, url, &response)**
  - Send a HTTP request with GET method to destination which composed by ip_address and port.
- send_post_command(ip_address, port, url, payload, &response)
  - Send a HTTP request with POST method to destination which composed by ip_address and port.

- **send_data(data)**
  - Send a HTTP request to desination.
  - Must call connect() before send data.

- **read(&receive_buffer)** / **async_read()**
  - Read HTTP response from socket buffer.

- **add_authentication(account, password, method = "Basic")**
  - Add authtication information.
  
- **add_header(header_map)**
  - Add specific HTTP header.

- **add_send_data_prefix(string)**
  - Add specific string before send data.
- **add_send_data_suffix(string)**
  - Add specific string after send data.

- **set_timeout(timeout_millisecond)**
  - Set connection timeout.
