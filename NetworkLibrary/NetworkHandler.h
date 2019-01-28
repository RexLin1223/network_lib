#pragma once
#include <functional>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

namespace network
{
	namespace tcp
	{
		using data_callback = std::function<void(const char*, size_t)>;

		class base_client
		{
		public:
			base_client();
			~base_client() {}

			virtual bool connect(const std::string& ip_address, size_t port);
			virtual void async_connect(const std::string& ip_address, size_t port);

			virtual bool disconnect();

			virtual bool send(const std::string& data);
			virtual void async_send(const std::string& data);

			virtual bool read(std::string& data);
			virtual void async_read();

			void set_data_callback(data_callback data_content_callback);
			void set_error_callback(data_callback error_message_callback);

			void set_option_linger(bool enable_linger, size_t timeout_sec);
			void set_option_keepalive(bool is_keepalive);
			void set_option_receive_buffer_size(size_t buffer_size);
			void set_option_send_buffer_size(size_t buffer_size);

		protected:
			virtual void async_handler(boost::system::error_code ec, size_t data_size = 0);

		protected:
			boost::asio::io_context io_context_;
			boost::asio::executor_work_guard<boost::asio::io_context::executor_type> io_worker_;
			boost::thread thread_;

			boost::asio::steady_timer timer_;
			boost::asio::ip::tcp::socket socket_;
			std::string receive_buffer_;

			data_callback data_callback_;
			data_callback error_callback_;
		};

		class http_client : public base_client
		{
		public:
			http_client();
			~http_client();

			virtual bool send_get_command(const std::string& ip_address, size_t port, const std::string& url, std::string& response);
			virtual bool send_post_command(const std::string& ip_address, size_t port, const std::string& url, const std::string& payload, std::string& response);

			virtual bool send_data(const std::string& data);

			virtual bool read(std::string& data) override;
			virtual void async_read() override;

			void add_authentication(const std::string& account, const std::string& password, const std::string& method = "Basic");
			void add_header(const std::map<std::string, std::string>& header_map);
			void add_send_data_prefix(const std::string& prefix);
			void add_send_data_suffix(const std::string& suffix);

			void set_timeout(size_t timeout_millisecond);

			bool is_running();

		protected:
			void parse_http_header(const std::string& header_string, std::map<std::string, std::string>& received_header_map);
			std::string get_header_to_string(const std::string& data="");

		private:
			bool read_header(std::string& receive_buffer, size_t& content_length);
			bool read_body(const size_t expect_body_size, std::string& receive_buffer);

			void read_header_async();
			void read_body_async(const size_t expect_body_size);

			boost::asio::streambuf receive_strem_buf_;
			std::map<std::string, std::string> http_send_header_;
			
			size_t timeout_;

			std::string prefix_;
			std::string suffix_;
		};
	}
}