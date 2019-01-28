#include "../NetworkLibrary/NetworkHandler.h"
#include <boost/format.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/chrono.hpp>
#include <boost/date_time.hpp>

#include <set>
#include <iostream>

#include <gtest/gtest.h>

class event_subscription_client : public testing::Test
{
public:
	struct server_info
	{
	public:
		std::string ip_address;
		size_t port;
		std::string account;
		std::string password;

		server_info() : port(0)
		{}
	};

	event_subscription_client()
		: http_client_(new network::tcp::http_client)
		, io_context_()
		, io_worker_(boost::asio::make_work_guard(io_context_))
		, timer_(io_context_)
	{
		thread_ = boost::thread(std::bind(&boost::asio::io_context::run, std::ref(io_context_)));
	}

	~event_subscription_client()
	{
		timer_.cancel();

		io_worker_.reset();
		io_context_.stop();

		http_client_.reset();
	}

	void set_connection_info(const std::string& ip_address, const size_t port)
	{
		server_info_.ip_address = ip_address;
		server_info_.port = port;
	}
	void set_authentication(const std::string& account, const std::string& password)
	{
		server_info_.account = account;
		server_info_.password = password;
	}

	bool start()
	{
		if (!http_client_)
		{
			return false;
		}

		http_client_->add_authentication(server_info_.account, server_info_.password);
		http_client_->add_send_data_prefix("--myboundary");
		http_client_->add_send_data_suffix("--myboundary--");

		std::string event_filter_json = "{\"filters\": {\"channel\": [{\"NvrID\": \"1\", \"channels\": \"1, 2, 3, 4 ,5, 6, 7\"}, {\"NvrID\": \"2\", \"channels\": \"1, 2, 3, 4 ,5, 6\"}, {\"NvrID\": \"3\", \"channels\": \"1, 2, 3, 4 ,5\"}, {\"NvrID\": \"4\", \"channels\": \"all\"} ], \"event\": {\"types\": \"Motion, DigitalInput, FaceDetection\"} } }";
		std::string resp;
		if (http_client_->send_post_command(server_info_.ip_address, server_info_.port, "/subscription/event", "", resp))
		{
			http_client_->send_data(event_filter_json);
		}

		count_events();
		return resp.size() > 0;
	}
	void stop()
	{
		if (http_client_)
		{
			http_client_->disconnect();
		}
	}

	bool read(std::string& data_buffer)
	{
		if (http_client_)
		{
			bool ret = http_client_->read(data_buffer);

			if (ret)
			{
				std::unique_lock<std::mutex> lock(list_mutex_);
				event_data_list_.emplace_back(data_buffer);
			}

			return ret;
		}
		return false;
	}

	void async_read()
	{
		if (http_client_)
		{
			http_client_->set_data_callback(std::bind(&event_subscription_client::on_received_event, this, std::placeholders::_1, std::placeholders::_2));
			http_client_->async_read();
		}
	}

	void on_received_event(const char* data, size_t data_len)
	{
		if (!data || data_len == 0) return;

		std::unique_lock<std::mutex> lock(list_mutex_);
		event_data_list_.emplace_back(std::string(data, data_len));
	}

	void count_events()
	{
		timer_.expires_after(std::chrono::milliseconds(1000));
		timer_.async_wait([this](boost::system::error_code ec)
		{
			if (ec)
			{
				std::cout << "Async wait error: " << ec.message() << std::endl;
			}

			size_t event_counter = 0;
			{
				std::unique_lock<std::mutex> lock(list_mutex_);
				event_counter = event_data_list_.size();
			}

			std::string time_string = boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time());
			std::cout << "Current time" << time_string << " events count: ." << event_counter << std::endl;
		});
	}

private:
	std::unique_ptr<network::tcp::http_client> http_client_;
	server_info server_info_;

	std::list<std::string> event_data_list_;
	std::mutex list_mutex_;

	boost::asio::io_context io_context_;
	boost::asio::executor_work_guard<boost::asio::io_context::executor_type> io_worker_;
	boost::thread thread_;
	boost::asio::steady_timer timer_;

};

TEST_F(event_subscription_client, event_subscription_tester)
{
	set_connection_info("127.0.0.1", 7003);
	set_authentication("sanzhiniao", "jijijiao");

	bool ret = start();
	EXPECT_EQ(ret, true);

	std::string data_buffer;
	while (true)
	{
		data_buffer.clear();
		std::this_thread::sleep_for(std::chrono::milliseconds(30));
		if (read(data_buffer))
		{
			std::cout << data_buffer << std::endl;
		}
		else
		{
			// Error
			std::cout << "Error" << std::endl;
			break;
		}
	}
}

int main(int argc, char** argv) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}