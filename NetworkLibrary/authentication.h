#pragma once

#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>

#include <string>
#include <iostream>
#include <sstream>

namespace network
{
	namespace authentication
	{
		static std::string base64_encode(const std::string& input)
		{
			typedef boost::archive::iterators::base64_from_binary<boost::archive::iterators::transform_width<std::string::const_iterator, 6, 8> > Base64EncodeIterator;
			std::stringstream result;
			std::copy(Base64EncodeIterator(input.begin()), Base64EncodeIterator(input.end()), std::ostream_iterator<char>(result));
			size_t equal_count = (3 - input.length() % 3) % 3;
			for (size_t i = 0; i < equal_count; i++) {
				result.put('=');
			}
			std::string output = result.str();
			return output;
		}

		static std::string base64_decode(const std::string& input)
		{
			typedef boost::archive::iterators::transform_width<boost::archive::iterators::binary_from_base64<std::string::const_iterator>, 8, 6> Base64DecodeIterator;
			std::stringstream result;
			try {
				std::copy(Base64DecodeIterator(input.begin()), Base64DecodeIterator(input.end()), std::ostream_iterator<char>(result));
			}
			catch (...) {
				return false;
			}

			std::string output = result.str();
			return output;
		}
	}

}