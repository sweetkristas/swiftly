/*
	Copyright (C) 2013-2014 by Kristina Simpson <sweet.kristas@gmail.com>
	
	This software is provided 'as-is', without any express or implied
	warranty. In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	   1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgement in the product documentation would be
	   appreciated but is not required.

	   2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.

	   3. This notice may not be removed or altered from any source
	   distribution.
*/

#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

#include <algorithm>
#include <map>
#include <string>
#include "asserts.hpp"
#include "Color.hpp"

namespace KRE
{
	namespace 
	{
		template<typename T>
		T clamp(T value, T minval, T maxval)
		{
			return std::min<T>(maxval, std::max(value, minval));
		}

		typedef std::map<std::string, Color> color_table_type;
		void create_color_table(color_table_type& color_table)
		{
			color_table["aliceblue"] = Color(240, 248, 255);
			color_table["antiquewhite"] = Color(250, 235, 215);
			color_table["aqua"] = Color(0, 255, 255);
			color_table["aquamarine"] = Color(127, 255, 212);
			color_table["azure"] = Color(240, 255, 255);
			color_table["beige"] = Color(245, 245, 220);
			color_table["bisque"] = Color(255, 228, 196);
			color_table["black"] = Color(0, 0, 0);
			color_table["blanchedalmond"] = Color(255, 235, 205);
			color_table["blue"] = Color(0, 0, 255);
			color_table["blueviolet"] = Color(138, 43, 226);
			color_table["brown"] = Color(165, 42, 42);
			color_table["burlywood"] = Color(222, 184, 135);
			color_table["cadetblue"] = Color(95, 158, 160);
			color_table["chartreuse"] = Color(127, 255, 0);
			color_table["chocolate"] = Color(210, 105, 30);
			color_table["coral"] = Color(255, 127, 80);
			color_table["cornflowerblue"] = Color(100, 149, 237);
			color_table["cornsilk"] = Color(255, 248, 220);
			color_table["crimson"] = Color(220, 20, 60);
			color_table["cyan"] = Color(0, 255, 255);
			color_table["darkblue"] = Color(0, 0, 139);
			color_table["darkcyan"] = Color(0, 139, 139);
			color_table["darkgoldenrod"] = Color(184, 134, 11);
			color_table["darkgray"] = Color(169, 169, 169);
			color_table["darkgreen"] = Color(0, 100, 0);
			color_table["darkgrey"] = Color(169, 169, 169);
			color_table["darkkhaki"] = Color(189, 183, 107);
			color_table["darkmagenta"] = Color(139, 0, 139);
			color_table["darkolivegreen"] = Color(85, 107, 47);
			color_table["darkorange"] = Color(255, 140, 0);
			color_table["darkorchid"] = Color(153, 50, 204);
			color_table["darkred"] = Color(139, 0, 0);
			color_table["darksalmon"] = Color(233, 150, 122);
			color_table["darkseagreen"] = Color(143, 188, 143);
			color_table["darkslateblue"] = Color(72, 61, 139);
			color_table["darkslategray"] = Color(47, 79, 79);
			color_table["darkslategrey"] = Color(47, 79, 79);
			color_table["darkturquoise"] = Color(0, 206, 209);
			color_table["darkviolet"] = Color(148, 0, 211);
			color_table["deeppink"] = Color(255, 20, 147);
			color_table["deepskyblue"] = Color(0, 191, 255);
			color_table["dimgray"] = Color(105, 105, 105);
			color_table["dimgrey"] = Color(105, 105, 105);
			color_table["dodgerblue"] = Color(30, 144, 255);
			color_table["firebrick"] = Color(178, 34, 34);
			color_table["floralwhite"] = Color(255, 250, 240);
			color_table["forestgreen"] = Color(34, 139, 34);
			color_table["fuchsia"] = Color(255, 0, 255);
			color_table["gainsboro"] = Color(220, 220, 220);
			color_table["ghostwhite"] = Color(248, 248, 255);
			color_table["gold"] = Color(255, 215, 0);
			color_table["goldenrod"] = Color(218, 165, 32);
			color_table["gray"] = Color(128, 128, 128);
			color_table["grey"] = Color(128, 128, 128);
			color_table["green"] = Color(0, 128, 0);
			color_table["greenyellow"] = Color(173, 255, 47);
			color_table["honeydew"] = Color(240, 255, 240);
			color_table["hotpink"] = Color(255, 105, 180);
			color_table["indianred"] = Color(205, 92, 92);
			color_table["indigo"] = Color(75, 0, 130);
			color_table["ivory"] = Color(255, 255, 240);
			color_table["khaki"] = Color(240, 230, 140);
			color_table["lavender"] = Color(230, 230, 250);
			color_table["lavenderblush"] = Color(255, 240, 245);
			color_table["lawngreen"] = Color(124, 252, 0);
			color_table["lemonchiffon"] = Color(255, 250, 205);
			color_table["lightblue"] = Color(173, 216, 230);
			color_table["lightcoral"] = Color(240, 128, 128);
			color_table["lightcyan"] = Color(224, 255, 255);
			color_table["lightgoldenrodyellow"] = Color(250, 250, 210);
			color_table["lightgray"] = Color(211, 211, 211);
			color_table["lightgreen"] = Color(144, 238, 144);
			color_table["lightgrey"] = Color(211, 211, 211);
			color_table["lightpink"] = Color(255, 182, 193);
			color_table["lightsalmon"] = Color(255, 160, 122);
			color_table["lightseagreen"] = Color(32, 178, 170);
			color_table["lightskyblue"] = Color(135, 206, 250);
			color_table["lightslategray"] = Color(119, 136, 153);
			color_table["lightslategrey"] = Color(119, 136, 153);
			color_table["lightsteelblue"] = Color(176, 196, 222);
			color_table["lightyellow"] = Color(255, 255, 224);
			color_table["lime"] = Color(0, 255, 0);
			color_table["limegreen"] = Color(50, 205, 50);
			color_table["linen"] = Color(250, 240, 230);
			color_table["magenta"] = Color(255, 0, 255);
			color_table["maroon"] = Color(128, 0, 0);
			color_table["mediumaquamarine"] = Color(102, 205, 170);
			color_table["mediumblue"] = Color(0, 0, 205);
			color_table["mediumorchid"] = Color(186, 85, 211);
			color_table["mediumpurple"] = Color(147, 112, 219);
			color_table["mediumseagreen"] = Color(60, 179, 113);
			color_table["mediumslateblue"] = Color(123, 104, 238);
			color_table["mediumspringgreen"] = Color(0, 250, 154);
			color_table["mediumturquoise"] = Color(72, 209, 204);
			color_table["mediumvioletred"] = Color(199, 21, 133);
			color_table["midnightblue"] = Color(25, 25, 112);
			color_table["mintcream"] = Color(245, 255, 250);
			color_table["mistyrose"] = Color(255, 228, 225);
			color_table["moccasin"] = Color(255, 228, 181);
			color_table["navajowhite"] = Color(255, 222, 173);
			color_table["navy"] = Color(0, 0, 128);
			color_table["oldlace"] = Color(253, 245, 230);
			color_table["olive"] = Color(128, 128, 0);
			color_table["olivedrab"] = Color(107, 142, 35);
			color_table["orange"] = Color(255, 165, 0);
			color_table["orangered"] = Color(255, 69, 0);
			color_table["orchid"] = Color(218, 112, 214);
			color_table["palegoldenrod"] = Color(238, 232, 170);
			color_table["palegreen"] = Color(152, 251, 152);
			color_table["paleturquoise"] = Color(175, 238, 238);
			color_table["palevioletred"] = Color(219, 112, 147);
			color_table["papayawhip"] = Color(255, 239, 213);
			color_table["peachpuff"] = Color(255, 218, 185);
			color_table["peru"] = Color(205, 133, 63);
			color_table["pink"] = Color(255, 192, 203);
			color_table["plum"] = Color(221, 160, 221);
			color_table["powderblue"] = Color(176, 224, 230);
			color_table["purple"] = Color(128, 0, 128);
			color_table["red"] = Color(255, 0, 0);
			color_table["rosybrown"] = Color(188, 143, 143);
			color_table["royalblue"] = Color(65, 105, 225);
			color_table["saddlebrown"] = Color(139, 69, 19);
			color_table["salmon"] = Color(250, 128, 114);
			color_table["sandybrown"] = Color(244, 164, 96);
			color_table["seagreen"] = Color(46, 139, 87);
			color_table["seashell"] = Color(255, 245, 238);
			color_table["sienna"] = Color(160, 82, 45);
			color_table["silver"] = Color(192, 192, 192);
			color_table["skyblue"] = Color(135, 206, 235);
			color_table["slateblue"] = Color(106, 90, 205);
			color_table["slategray"] = Color(112, 128, 144);
			color_table["slategrey"] = Color(112, 128, 144);
			color_table["snow"] = Color(255, 250, 250);
			color_table["springgreen"] = Color(0, 255, 127);
			color_table["steelblue"] = Color(70, 130, 180);
			color_table["tan"] = Color(210, 180, 140);
			color_table["teal"] = Color(0, 128, 128);
			color_table["thistle"] = Color(216, 191, 216);
			color_table["tomato"] = Color(255, 99, 71);
			color_table["turquoise"] = Color(64, 224, 208);
			color_table["violet"] = Color(238, 130, 238);
			color_table["wheat"] = Color(245, 222, 179);
			color_table["white"] = Color(255, 255, 255);
			color_table["whitesmoke"] = Color(245, 245, 245);
			color_table["yellow"] = Color(255, 255, 0);
			color_table["yellowgreen"] = Color(154, 205, 50);		
		}

		color_table_type& get_color_table() 
		{
			static color_table_type res;
			if(res.empty()) {
				create_color_table(res);
			}
			return res;
		}

		double convert_string_to_number(const std::string& str)
		{
			try {
				double value = boost::lexical_cast<double>(str);
				if(value > 1.0) {
					// Assume it's an integer value.
					return static_cast<float>(value / 255.0);
				} else if(value < 1.0) {
					return static_cast<float>(value);
				} else {
					// value = 1.0 -- check the string to try and disambiguate
					if(str == "1" || str.find('.') == std::string::npos) {
						return 1.0 / 255.0;
					}
					return 1.0;
				}
			} catch(boost::bad_lexical_cast&) {
				ASSERT_LOG(false, "unable to convert value to number: " << str);
			}
		}

		float convert_numeric(const variant& node)
		{
			if(node.is_int()) {
				return clamp<int>(node.as_int32(), 0, 255) / 255.0f;
			} else if(node.is_float()) {
				return clamp<float>(node.as_float(), 0.0f, 1.0f);
			} else if(node.is_string()) {
				return static_cast<float>(convert_string_to_number(node.as_string()));
			}
			ASSERT_LOG(false, "attribute of Color value was expected to be numeric type.");
			return 1.0f;
		}

		uint8_t convert_hex_digit(char d) 
		{
			uint8_t value = 0;
			if(d >= 'A' && d <= 'F') {
				value = d - 'A' + 10;
			} else if(d >= 'a' && d <= 'f') {
				value = d - 'a' + 10;
			} else if(d >= '0' && d <= '9') {
				value = d - '0';
			} else {
				ASSERT_LOG(false, "Unrecognised hex digit: " << d);
			}
			return value;
		}

		Color color_from_hex_string(const std::string& colstr)
		{
			std::string s = colstr;
			ASSERT_LOG(!s.empty(), "No color detail found in string.");
			if(s[0] == '#') {
				s = s.substr(1);
			}
			ASSERT_LOG(s.length() == 3 || s.length() == 6, "Expected length of color definition to be 3 or 6 characters long, found: " << s);
			if(s.length() == 3) {
				int r_hex = convert_hex_digit(s[1]);
				int g_hex = convert_hex_digit(s[2]);
				int b_hex = convert_hex_digit(s[3]);
				return Color((r_hex << 4) | r_hex, (g_hex << 4) | g_hex, (b_hex << 4) | b_hex);
			}
			return Color((convert_hex_digit(s[1]) << 4) | convert_hex_digit(s[2]),
				(convert_hex_digit(s[3]) << 4) | convert_hex_digit(s[4]),
				(convert_hex_digit(s[5]) << 4) | convert_hex_digit(s[6]));
		}

		std::vector<std::string> split(const std::string& input, const std::string& re) {
			// passing -1 as the submatch index parameter performs splitting
			boost::regex regex(re);
			boost::sregex_token_iterator first(input.begin(), input.end(), regex, -1), last;
			return std::vector<std::string>(first, last);
		}	
	}

	Color::Color()
	{
		color_[0] = 1.0f;
		color_[1] = 1.0f;
		color_[2] = 1.0f;
		color_[3] = 1.0f;
	}

	Color::~Color()
	{
	}

	Color::Color(const double r, const double g, const double b, const double a)
	{
		color_[0] = float(r);
		color_[1] = float(g);
		color_[2] = float(b);
		color_[3] = float(a);
	}

	Color::Color(const int r, const int g, const int b, const int a)
	{
		color_[0] = clamp<int>(r,0,255)/255.0f;
		color_[1] = clamp<int>(g,0,255)/255.0f;
		color_[2] = clamp<int>(b,0,255)/255.0f;
		color_[3] = clamp<int>(a,0,255)/255.0f;
	}

	Color::Color(const variant& node)
	{
		color_[0] = color_[1] = color_[2] = 0.0f;
		color_[3] = 1.0f;

		if(node.is_string()) {
			const std::string& colstr = node.as_string();
			auto it = get_color_table().find(colstr);
			if(it == get_color_table().end()) {
				*this = color_from_hex_string(colstr);
			}
			*this = it->second;
		} else if(node.is_list()) {
			ASSERT_LOG(node.num_elements() == 3 || node.num_elements() == 4,
				"Color nodes must be lists of 3 or 4 numbers.");
			for(size_t n = 0; n != node.num_elements(); ++n) {
				color_[n] = convert_numeric(node[n]);
			}
		} else if(node.is_map()) {
			if(node.has_key("red")) {
				color_[0] = convert_numeric(node["red"]);
			} else if(node.has_key("r")) {
				color_[0] = convert_numeric(node["r"]);
			}
			if(node.has_key("green")) {
				color_[1] = convert_numeric(node["green"]);
			} else if(node.has_key("g")) {
				color_[1] = convert_numeric(node["g"]);
			}
			if(node.has_key("blue")) {
				color_[2] = convert_numeric(node["blue"]);
			} else if(node.has_key("b")) {
				color_[2] = convert_numeric(node["b"]);
			}
			if(node.has_key("alpha")) {
				color_[3] = convert_numeric(node["alpha"]);
			} else if(node.has_key("a")) {
				color_[3] = convert_numeric(node["a"]);
			}
		} else {
			ASSERT_LOG(false, "Unrecognised Color value: " << node.to_debug_string());
		}
	}

	Color::Color(unsigned long n, ColorByteOrder order)
	{
		float b0 = (n & 0xff)/255.0f;
		float b1 = ((n >> 8) & 0xff)/255.0f;
		float b2 = ((n >> 16) & 0xff)/255.0f;
		float b3 = ((n >> 24) & 0xff)/255.0f;
		switch (order)
		{
			case ColorByteOrder::RGBA:
				color_[0] = b3;
				color_[1] = b2;
				color_[2] = b1;
				color_[3] = b0;
				break;
			case ColorByteOrder::ARGB:
				color_[0] = b2;
				color_[1] = b1;
				color_[2] = b0;
				color_[3] = b3;
				break;
			case ColorByteOrder::BGRA:
				color_[0] = b1;
				color_[1] = b2;
				color_[2] = b3;
				color_[3] = b0;
				break;
			case ColorByteOrder::ABGR:
				color_[0] = b0;
				color_[1] = b1;
				color_[2] = b2;
				color_[3] = b3;
				break;
			default: 
				ASSERT_LOG(false, "Unknown ColorByteOrder value: " << static_cast<int>(order));
				break;
		}
	}

	Color::Color(const std::string& colstr)
	{
		ASSERT_LOG(!colstr.empty(), "Empty string passed to Color constructor.");
		auto it = get_color_table().find(colstr);
		if(it == get_color_table().end()) {
			if(colstr[0] == '#') {
				*this = color_from_hex_string(colstr);
			} else if(colstr.find(',') != std::string::npos) {
				std::fill(color_, color_+3, 1.0f);
				auto buf = split(colstr, ",| |;");
				unsigned n = 0;
				for(auto& s : buf) {
					color_[n] = static_cast<float>(convert_string_to_number(s));
					if(++n >= 4) {
						break;
					}
				}
			} else {
				ASSERT_LOG(false, "Couldn't parse color '" << colstr << "' in known color list");
			}
		}
		*this = it->second;
	}

	void Color::setAlpha(int a)
	{
		color_[3] = clamp<int>(a, 0, 255) / 255.0f;
	}

	void Color::setAlpha(double a)
	{
		color_[3] = clamp<float>(float(a), 0.0f, 1.0f);
	}

	void Color::setRed(int a)
	{
		color_[0] = clamp<int>(a, 0, 255) / 255.0f;
	}

	void Color::setRed(double a)
	{
		color_[0] = clamp<float>(float(a), 0.0f, 1.0f);
	}

	void Color::setGreen(int a)
	{
		color_[1] = clamp<int>(a, 0, 255) / 255.0f;
	}

	void Color::setGreen(double a)
	{
		color_[1] = clamp<float>(float(a), 0.0f, 1.0f);
	}

	void Color::setBlue(int a)
	{
		color_[2] = clamp<int>(a, 0, 255) / 255.0f;
	}

	void Color::setBlue(double a)
	{
		color_[2] = clamp<float>(float(a), 0.0f, 1.0f);
	}

	ColorPtr Color::factory(const std::string& name)
	{
		auto it = get_color_table().find(name);
		ASSERT_LOG(it != get_color_table().end(), "Couldn't find color '" << name << "' in known color list");
		return ColorPtr(new Color(it->second));
	}

	variant Color::write() const
	{
		// XXX we should store information on how the Color value was given to us, if it was
		// a variant, then output in same format.
		std::vector<variant> v;
		v.reserve(4);
		v.push_back(variant(r()));
		v.push_back(variant(g()));
		v.push_back(variant(b()));
		v.push_back(variant(a()));
		return variant(&v);
	}

	Color operator*(const Color& lhs, const Color& rhs)
	{
		return Color(lhs.r()*rhs.r(), lhs.g()*rhs.g(), lhs.b()*rhs.b(), lhs.a()*rhs.a());
	}
}