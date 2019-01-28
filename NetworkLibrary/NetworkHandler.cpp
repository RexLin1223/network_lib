
#include <iostream>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>

#include <boost/date_time/posix_time/ptime.hpp>

#include "NetworkHandler.h"
#include "authentication.h"

namespace network
{
	namespace tcp
	{
		base_client::base_client()
			: io_context_()
			, io_worker_(boost::asio::make_work_guard(io_context_))
			, timer_(io_context_)
			, socket_(io_context_)
			, data_callback_(nullptr)
			, error_callback_(nullptr)
		{
			thread_ = boost::thread(std::bind(&boost::asio::io_context::run, std::ref(io_context_)));
		}

		bool base_client::connect(const std::string& ip_address, size_t port)
		{
			try {
				auto address_object = boost::asio::ip::make_address_v4(ip_address);
				auto endpoint = boost::asio::ip::tcp::endpoint(address_object, static_cast<unsigned short>(port));

				socket_.connect(endpoint);
				return socket_.is_open();
			}
			catch (std::exception& ex)
			{
				std::cout << "Get error: " << ex.what() << std::endl;
				if (error_callback_)
					error_callback_(ex.what(), std::strlen(ex.what()));
			}
			return false;
		}

		void base_client::async_connect(const std::string& ip_address, size_t port)
		{
			try {
				auto address_object = boost::asio::ip::make_address_v4(ip_address);
				auto endpoint = boost::asio::ip::tcp::endpoint(address_object, static_cast<unsigned short>(port));

				socket_.async_connect(endpoint,
					[this](boost::system::error_code ec)
				{
					if (ec)
					{
						std::cout << "Get an error when async connect: " << ec.message() << std::endl;
						if (error_callback_)
						{
							error_callback_(ec.message().c_str(), ec.message().size());
						}
					}
					else
					{
						if (data_callback_)
						{
							std::string connected = "connected";
							data_callback_(connected.c_str(), connected.size());
						}
					}
				});
			}
			catch (boost::system::error_code& ec)
			{
				std::cout << "Get an error when async connect: " << ec.message() << std::endl;
				if (error_callback_)
					error_callback_(ec.message().c_str(), ec.message().size());
			}
			return;
		}

		bool base_client::disconnect()
		{
			if (socket_.is_open())
			{
				socket_.close();
			}
			return true;
		}

		bool base_client::send(const std::string& data)
		{
			if (!socket_.is_open()) return false;

			size_t transferred_size = socket_.send(boost::asio::buffer(data));
			return transferred_size == data.size();
		}

		void base_client::async_send(const std::string& data)
		{
			if (!socket_.is_open()) return;

			try {
				socket_.async_send(std::move(boost::asio::buffer(data, data.size())),
					std::bind(&http_client::async_handler, this, std::placeholders::_1, std::placeholders::_2));
			}
			catch (std::exception& ex)
			{
				std::cout << "Get an error when async send: " << ex.what() << std::endl;
				if (error_callback_)
					error_callback_(ex.what(), std::strlen(ex.what()));
			}
			return;
		}

		bool base_client::read(std::string& data)
		{
			if (!socket_.is_open()) return false;

			try {
				size_t length = boost::asio::read(socket_, boost::asio::buffer(data));
				return length > 0;
			}
			catch (std::exception& ex)
			{
				std::cout << "Get an error when read data: " << ex.what() << std::endl;
				if (error_callback_)
					error_callback_(ex.what(), std::strlen(ex.what()));
			}
			return false;
		}

		void base_client::async_read()
		{
			if (!socket_.is_open()) return;

			boost::asio::async_read(socket_, boost::asio::buffer(receive_buffer_),
				std::bind(&base_client::async_handler, this, std::placeholders::_1, std::placeholders::_2));

			return;
		}

		void base_client::set_data_callback(data_callback data_content_callback)
		{
			try {
				data_callback_ = data_content_callback;
			}
			catch (...)
			{
				data_callback_ = nullptr;
			}
		}

		void base_client::set_error_callback(data_callback error_message_callback)
		{
			try {
				error_callback_ = error_message_callback;
			}
			catch (...)
			{
				error_callback_ = nullptr;
			}
		}

		void base_client::set_option_linger(bool enable_linger, size_t timeout_sec)
		{
			// Set linger option
			if (socket_.is_open()) {
				boost::asio::socket_base::linger linger_option(enable_linger, timeout_sec);
				socket_.set_option(linger_option);
			}
		}

		void base_client::set_option_keepalive(bool is_keepalive)
		{
			// Set keep alive option
			if (socket_.is_open()) {
				boost::asio::socket_base::keep_alive keep_alive_option(is_keepalive);
				socket_.set_option(keep_alive_option);
			}
		}

		void base_client::set_option_receive_buffer_size(size_t buffer_size)
		{
			// Set receive buffer size option
			if (socket_.is_open()) {
				boost::asio::socket_base::receive_buffer_size receive_buffer_size(buffer_size);
				socket_.set_option(receive_buffer_size);
			}
		}

		void base_client::set_option_send_buffer_size(size_t buffer_size)
		{
			if (socket_.is_open()) {
				// Set send buffer size option
				boost::asio::socket_base::send_buffer_size send_buffer_size(buffer_size);
				socket_.set_option(send_buffer_size);
			}
		}

		void base_client::async_handler(boost::system::error_code ec, size_t data_size)
		{
			if (ec)
			{
				std::cout << "Get error:" << ec.message() << std::endl;
				if (error_callback_)
				{
					error_callback_(ec.message().c_str(), ec.message().size());
				}
				socket_.close();
			}

			if (data_callback_ && receive_buffer_.size() > 0)
			{
				data_callback_(receive_buffer_.c_str(), receive_buffer_.size());
				receive_buffer_.clear();
			}
		}

		http_client::http_client()
			: timeout_(8000)
		{
			// Set default header
			http_send_header_["User-Agent"] = "HTTP Client";
			http_send_header_["Connection"] = "Keep-Alive";
			http_send_header_["Pragma"] = "no-cache";
			http_send_header_["Cache-Control"] = "no-cache";
			http_send_header_["Content-Type"] = "text/plain";
		}

		http_client::~http_client()
		{

		}

		bool http_client::send_get_command(const std::string& ip_address, size_t port, const std::string& url, std::string& response)
		{
			if (connect(ip_address, port))
			{
				prefix_ = boost::str(boost::format("GET %s HTTP/1.1") % url);
				if (send_data(""))
				{
					if (read(response))
					{
						return true;
					}
				}
			}

			return false;
		}

		bool http_client::send_post_command(const std::string& ip_address, size_t port, const std::string& url, const std::string& payload, std::string& response)
		{
			if (connect(ip_address, port))
			{
				prefix_ = boost::str(boost::format("POST %s HTTP/1.1\r\n") % url);

				if (send_data(payload))
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(1000));
					if (read(response))
					{
						return true;
					}
				}
			}

			return false;
		}

		void http_client::set_timeout(size_t timeout_millisecond)
		{
			timeout_ = timeout_millisecond;
		}

		bool http_client::is_running()
		{
			return socket_.is_open();
		}

		void http_client::add_header(const std::map<std::string, std::string>& header_map)
		{
			if (header_map.empty()) return;
			http_send_header_.insert(header_map.begin(), header_map.end());
		}

		void http_client::add_send_data_prefix(const std::string& prefix)
		{
			prefix_ = prefix;
		}

		void http_client::add_send_data_suffix(const std::string& suffix)
		{
			suffix_ = suffix;
		}

		void http_client::add_authentication(const std::string& account, const std::string& password, const std::string& method /*= "Basic"*/)
		{
			if (boost::iequals(method, "Basic"))
			{
				auto encoded_string = authentication::base64_encode(boost::str(boost::format("%s:%s") % account % password));
				// Base64 Encode
				http_send_header_["Authorization"] = boost::str(boost::format("%s %s") % method % encoded_string);
			}
			else if (boost::iequals(method, "Digest"))
			{
				// TODO
			}
		}

		bool http_client::send_data(const std::string& data)
		{
			std::string request_string;
			if (!prefix_.empty())
			{
				bool has_CRLF = prefix_.find("\r\n") != std::string::npos;
				request_string += has_CRLF ? prefix_ : prefix_ + "\r\n";
			}

			request_string += get_header_to_string(data);
			request_string += data;

			if (!suffix_.empty())
			{
				request_string += "\r\n";
				bool has_CRLF = suffix_.find("\r\n") != std::string::npos;
				request_string += has_CRLF ? suffix_ : suffix_ + "\r\n";
			}
			
			size_t transffered_size = socket_.send(boost::asio::buffer(request_string));
			return transffered_size == request_string.size();
		}

		bool http_client::read(std::string& data)
		{
			if (!socket_.is_open()) return false;

			std::string header;
			std::string body;
			try {
				size_t content_length = 0;
				if (read_header(header, content_length))
				{
					data.append(header);
					if (content_length > 0 && read_body(content_length, body))
					{
						// Header + body
						data.append(body);
					}
					return true;
				}
			}
			catch (boost::system::error_code& ec)
			{
				std::cout << "Get an error when read data: " << ec.message() << std::endl;
				if (error_callback_) {
					error_callback_(ec.message().c_str(), ec.message().size());
				}
			}
			return false;
		}

		void http_client::async_read()
		{
			if (!socket_.is_open()) return;
			read_header_async();
		}

		bool http_client::read_header(std::string& receive_buffer, size_t& content_length)
		{
			if (socket_.is_open())
			{
				boost::system::error_code ec;
				size_t header_size = boost::asio::read_until(socket_, receive_strem_buf_, "\r\n\r\n", ec);
				if (ec)
				{
					std::cout << ec.message() << std::endl;
					if (error_callback_)
					{
						error_callback_(ec.message().c_str(), ec.message().size());
					}
					return false;
				}

				// Casting streambuf to std::string.
				receive_buffer = std::string(boost::asio::buffers_begin(receive_strem_buf_.data()), boost::asio::buffers_begin(receive_strem_buf_.data()) + header_size);
				receive_strem_buf_.consume(header_size);

				std::map<std::string, std::string> http_received_header;
				parse_http_header(receive_buffer, http_received_header);
				if (http_received_header.size() > 0)
				{
					// Get content length.
					auto itor = http_received_header.find("Content-Length");
					if (itor != http_received_header.end())
					{
						const std::string& content_length_string = itor->second;
						try {
							content_length = boost::lexical_cast<size_t>(content_length_string);
						}
						catch (std::exception& ex)
						{
							std::cout << "Casting content length error: " << ex.what() << std::endl;
							if (error_callback_)
								error_callback_(ex.what(), std::strlen(ex.what()));
						}
					}
				}
			}

			return true;
		}

		bool http_client::read_body(const size_t expect_body_size, std::string& receive_buffer)
		{
			if (socket_.is_open())
			{
				size_t body_size = boost::asio::read(socket_, receive_strem_buf_.prepare(expect_body_size));

				receive_strem_buf_.commit(expect_body_size);
				receive_buffer += std::string(boost::asio::buffers_begin(receive_strem_buf_.data()), boost::asio::buffers_begin(receive_strem_buf_.data()) + body_size);
				receive_strem_buf_.consume(body_size);

				return expect_body_size == body_size;
			}
			return false;
		}

		void http_client::read_header_async()
		{
			boost::asio::async_read_until(socket_, receive_strem_buf_, "\r\n\r\n", 
				[this](boost::system::error_code ec, size_t read_size) {
				if (ec)
				{
					std::cout << "Async_read header error: " << ec.message() << std::endl;
					if (error_callback_)
					{
						error_callback_(ec.message().c_str(), ec.message().size());
					}
					socket_.close();
					return;
				}

				std::string header = std::string(boost::asio::buffers_begin(receive_strem_buf_.data()), boost::asio::buffers_begin(receive_strem_buf_.data()) + read_size);
				receive_strem_buf_.consume(header.size());

				std::map<std::string, std::string> header_map;
				parse_http_header(header, header_map);
				auto itor = header_map.find("Content-Length");
				if (itor != header_map.end())
				{
					const std::string& content_length_string = itor->second;
					try
					{
						size_t content_length = boost::lexical_cast<int>(content_length_string);
						if (content_length > 0)
						{
							read_body_async(content_length);
						}
					}
					catch (...)
					{
					}
				}

			});
		}

		void http_client::read_body_async(const size_t expect_body_size)
		{
			boost::asio::async_read(socket_, receive_strem_buf_.prepare(expect_body_size), 
				[this](boost::system::error_code ec, size_t read_size)
			{
				if (ec)
				{
					std::cout << "Async_read body error: " << ec.message() << std::endl;
					if (error_callback_)
					{
						error_callback_(ec.message().c_str(), ec.message().size());
					}
					socket_.close();
					return;
				}

				receive_strem_buf_.commit(read_size);
				std::string body = std::string(boost::asio::buffers_begin(receive_strem_buf_.data()), boost::asio::buffers_begin(receive_strem_buf_.data()) + read_size);
				receive_strem_buf_.consume(read_size);

				if (body.size() > 0 && data_callback_)
				{
					data_callback_(body.c_str(), body.size());
				}

				// Read next header
				read_header_async();
			});
		}

		void http_client::parse_http_header(const std::string& header_string, std::map<std::string, std::string>& received_header_map)
		{
			auto parse_header_content = [&received_header_map](const std::string& new_line)
			{
				size_t delimiter_pos = new_line.find(":");
				if (delimiter_pos == std::string::npos) return;

				const std::string key = new_line.substr(0, delimiter_pos);
				const std::string value = new_line.substr(delimiter_pos + 2);

				received_header_map[key] = value;
			};

			auto get_status_code = [&received_header_map](const std::string& reponse)
			{
				// Find HTTP/1.1 or HTTP/1.0 to get first line.
				size_t start_pos = reponse.find("HTTP/1.1 ");
				if (start_pos == std::string::npos) return;
				size_t end_pos = reponse.find("\r\n", start_pos);
				if (end_pos == std::string::npos) return;

				std::string status_code = reponse.substr(start_pos, end_pos);
				received_header_map["Status-Code"] = status_code;
			};

			get_status_code(header_string);

			std::vector<std::string> lines;
			boost::split(lines, header_string, boost::is_any_of("\r\n"), boost::token_compress_on);

			for (auto itor = lines.begin(); itor != lines.end(); ++itor)
			{
				const std::string& new_line = *itor;
				parse_header_content(new_line);
			}
		}

		std::string http_client::get_header_to_string(const std::string& data/*=""*/)
		{
			std::string header_string;
			for (auto itor = http_send_header_.begin(); itor != http_send_header_.end(); ++itor)
			{
				header_string += boost::str(boost::format("%s: %s") % itor->first % itor->second);
				header_string += "\r\n";
			}
			header_string += boost::str(boost::format("Content-Length: %d\r\n") % data.size());
			header_string += "\r\n";
			return header_string;
		}
	}
}
