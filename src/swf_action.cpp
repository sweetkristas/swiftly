#include <algorithm>
#include <codecvt>
#include <locale>
#include <map>

#include "asserts.hpp"
#include "swf_action.hpp"
#include "swf_character.hpp"
#include "swf_player.hpp"

namespace swf
{
	namespace 
	{
		typedef std::map<ActionCode, std::string> action_type;

		void init_action_to_string(action_type& a)
		{
			a[ActionCode::End] = "End";
            a[ActionCode::NextFrame] = "NextFrame";
            a[ActionCode::PreviousFrame] = "PreviousFrame";
            a[ActionCode::Play] = "Play";
            a[ActionCode::Stop] = "Stop";
            a[ActionCode::ToggleQuality] = "ToggleQuality";
            a[ActionCode::StopSounds] = "StopSounds";
            a[ActionCode::GotoFrame] = "GotoFrame";
            a[ActionCode::GetURL] = "GetURL";
            a[ActionCode::WaitForFrame] = "WaitForFrame";
            a[ActionCode::SetTarget] = "SetTarget";
            a[ActionCode::GotoLabel] = "GotoLabel";
            a[ActionCode::Push] = "Push";
            a[ActionCode::Pop] = "Pop";
            a[ActionCode::Add] = "Add";
            a[ActionCode::Subtract] = "Subtract";
            a[ActionCode::Multiply] = "Multiply";
            a[ActionCode::Divide] = "Divide";
            a[ActionCode::Equals] = "Equals";
            a[ActionCode::Less] = "Less";
            a[ActionCode::And] = "And";
            a[ActionCode::Or] = "Or";
            a[ActionCode::Not] = "Not";
            a[ActionCode::StringEquals] = "StringEquals";
            a[ActionCode::StringLength] = "StringLength";
            a[ActionCode::StringAdd] = "StringAdd";
            a[ActionCode::StringExtract] = "StringExtract";
            a[ActionCode::StringLess] = "StringLess";
            a[ActionCode::MBStringLength] = "MBStringLength";
            a[ActionCode::MBStringExtract] = "MBStringExtract";
            a[ActionCode::ToInteger] = "ToInteger";
            a[ActionCode::CharToAscii] = "CharToAscii";
            a[ActionCode::AsciiToChar] = "AsciiToChar";
            a[ActionCode::MBCharToAscii] = "MBCharToAscii";
            a[ActionCode::MBAsciiToChar] = "MBAsciiToChar";
            a[ActionCode::Jump] = "Jump";
            a[ActionCode::If] = "If";
            a[ActionCode::Call] = "Call";
            a[ActionCode::GetVariable] = "GetVariable";
            a[ActionCode::SetVariable] = "SetVariable";
            a[ActionCode::GetURL2] = "GetURL2";
            a[ActionCode::GotoFrame2] = "GotoFrame2";
            a[ActionCode::SetTarget2] = "SetTarget2";
            a[ActionCode::GetProperty] = "GetProperty";
            a[ActionCode::SetProperty] = "SetProperty";
            a[ActionCode::CloneSprite] = "CloneSprite";
            a[ActionCode::RemoveSprite] = "RemoveSprite";
            a[ActionCode::StartDrag] = "StartDrag";
            a[ActionCode::EndDrag] = "EndDrag";
            a[ActionCode::WaitForFrame2] = "WaitForFrame2";
            a[ActionCode::Trace] = "Trace";
            a[ActionCode::GetTime] = "GetTime";
            a[ActionCode::RandomNumber] = "RandomNumber";
            a[ActionCode::CallFunction] = "CallFunction";
            a[ActionCode::CallMethod] = "CallMethod";
            a[ActionCode::ConstantPool] = "ConstantPool";
            a[ActionCode::DefineFunction] = "DefineFunction";
            a[ActionCode::DefineLocal] = "DefineLocal";
            a[ActionCode::DefineLocal2] = "DefineLocal2";
            a[ActionCode::Delete] = "Delete";
            a[ActionCode::Delete2] = "Delete2";
            a[ActionCode::Enumerate] = "Enumerate";
            a[ActionCode::Equals2] = "Equals2";
            a[ActionCode::GetMember] = "GetMember";
            a[ActionCode::InitArray] = "InitArray";
            a[ActionCode::InitObject] = "InitObject";
            a[ActionCode::NewMethod] = "NewMethod";
            a[ActionCode::NewObject] = "NewObject";
            a[ActionCode::SetMember] = "SetMember";
            a[ActionCode::TargetPath] = "TargetPath";
            a[ActionCode::With] = "With";
            a[ActionCode::ToNumber] = "ToNumber";
            a[ActionCode::ToString] = "ToString";
            a[ActionCode::TypeOf] = "TypeOf";
            a[ActionCode::Add2] = "Add2";
            a[ActionCode::Less2] = "Less2";
            a[ActionCode::Modulo] = "Modulo";
            a[ActionCode::BitAnd] = "BitAnd";
            a[ActionCode::BitLShift] = "BitLShift";
            a[ActionCode::BitOr] = "BitOr";
            a[ActionCode::BitRShift] = "BitRShift";
            a[ActionCode::BitURShift] = "BitURShift";
            a[ActionCode::BitXor] = "BitXor";
            a[ActionCode::Decrement] = "Decrement";
            a[ActionCode::Increment] = "Increment";
            a[ActionCode::PushDuplicate] = "PushDuplicate";
            a[ActionCode::Return] = "Return";
            a[ActionCode::StackSwap] = "StackSwap";
            a[ActionCode::StoreRegister] = "StoreRegister";
            a[ActionCode::InstanceOf] = "InstanceOf";
            a[ActionCode::Enumerate2] = "Enumerate2";
            a[ActionCode::StrictEquals] = "StrictEquals";
            a[ActionCode::Greater] = "Greater";
            a[ActionCode::StringGreater] = "StringGreater";
            a[ActionCode::DefineFunction2] = "DefineFunction2";
            a[ActionCode::Extends] = "Extends";
            a[ActionCode::CastOp] = "CastOp";
            a[ActionCode::ImplementsOp] = "ImplementsOp";
            a[ActionCode::Try] = "Try";
            a[ActionCode::Throw] = "Throw";
		}

		action_type& action_map()
		{
			static action_type res;
			if(res.empty()) {
				init_action_to_string(res);
			}
			return res;
		}

		const std::string& action_to_string(ActionCode a)
		{
			auto it = action_map().find(a);
			ASSERT_LOG(it != action_map().end(), "Unable to find action instruction: " << static_cast<int>(a));
			return it->second;
		}

		std::wstring to_wide_string(const std::string& str)
		{
			std::wstring wstr(str.size(), L' ');
			std::copy(str.begin(), str.end(), wstr.begin());
			return wstr;
		}

		std::string to_narrow_string(const std::wstring& str)
		{
			std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
			return convert.to_bytes(str);
		}

		enum class StandardProperties
		{
			X,
			Y,
			XSCALE,
			YSCALE,
			CURRENT_FRAME,
			TOTAL_FRAMES,
			ALPHA,
			VISIBLE,
			WIDTH,
			HEIGHT,
			ROTATION,
			TARGET,
			FRAMES_LOADED,
			NAME,
			DROP_TARGET,
			URL,
			HIGH_QUALITY,
			FOCUS_RECT,
			SOUND_BUF_TIME,
			QUALITY,
			XMOUSE,
			YMOUSE,
		};

		void std_property_name_map_init(std::map<StandardProperties,std::string>* res)
		{
			(*res)[StandardProperties::X] = "_x";
			(*res)[StandardProperties::Y] = "_y";
			(*res)[StandardProperties::XSCALE] = "_xscale";
			(*res)[StandardProperties::YSCALE] = "_yscale";
			(*res)[StandardProperties::CURRENT_FRAME] = "_currentframe";
			(*res)[StandardProperties::TOTAL_FRAMES] = "_totalframes";
			(*res)[StandardProperties::ALPHA] = "_alpha";
			(*res)[StandardProperties::VISIBLE] = "_visible";
			(*res)[StandardProperties::WIDTH] = "_width";
			(*res)[StandardProperties::HEIGHT] = "_height";
			(*res)[StandardProperties::ROTATION] = "_rotation";
			(*res)[StandardProperties::TARGET] = "_target";
			(*res)[StandardProperties::FRAMES_LOADED] = "_framesloaded";
			(*res)[StandardProperties::NAME] = "_name";
			(*res)[StandardProperties::DROP_TARGET] = "_droptarget";
			(*res)[StandardProperties::URL] = "_url";
			(*res)[StandardProperties::HIGH_QUALITY] = "_highquality";
			(*res)[StandardProperties::FOCUS_RECT] = "_focusrect";
			(*res)[StandardProperties::SOUND_BUF_TIME] = "_soundbuftime";
			(*res)[StandardProperties::QUALITY] = "_quality";
			(*res)[StandardProperties::XMOUSE] = "_xmouse";
			(*res)[StandardProperties::YMOUSE] = "_ymouse";
		}

		const std::string& get_std_property_name(int index) {
			static std::map<StandardProperties,std::string> res;
			if(res.empty()) {
				std_property_name_map_init(&res);
			}
			auto it = res.find(static_cast<StandardProperties>(index));
			ASSERT_LOG(it != res.end(), "Unable to fine standard property with index: " << index);
			return it->second.
		}
	}

	action::action(const std::shared_ptr<bit_stream>& bits)
	{
		bool done = false;
		while(!done) {
			uint8_t code = bits->read_unsigned8();
			codestream_.emplace_back(code);
			if(code == 0) {
				done = true;
			} else {
				if(code >= 128) {
					uint16_t len = bits->read_unsigned16();
					auto new_code = bits->read_unsigned8(len);
					codestream_.emplace_back(static_cast<uint8_t>(len & 0xff));
					codestream_.emplace_back(static_cast<uint8_t>(len >> 8));
					codestream_.insert(codestream_.end(), new_code.begin(), new_code.end());
				}
			}
		}
	}

	void action::init()
	{
	}


	std::string action::read_string(codestream_iterator& it)
	{
		std::string res;
		char ch = *it++;
		while(ch != 0) {
			res += ch;
			ch = *it++;
		}
		return res;
	}

	int action::read_u8(codestream_iterator& it)
	{
		return *it++;
	}

	int action::read_u16(codestream_iterator& it)
	{
		int value = *it++;
		return value + (*(it++) << 8);
	}

	float action::read_float(codestream_iterator& it)
	{
		unsigned value = *it++;
		value += (*(it++) << 8);
		value += (*(it++) << 16);
		value += (*(it++) << 24);
		return *reinterpret_cast<float*>(&value);
	}

	double action::read_double(codestream_iterator& it)
	{
		uint8_t value[8];
		for(int n = 0; n != 8; ++n) {
			value[n] = *it++;
		}
		return *reinterpret_cast<double*>(value);
	}

	int action::read_s32(codestream_iterator& it)
	{
		unsigned value = *it++;
		value += (*(it++) << 8);
		value += (*(it++) << 16);
		value += (*(it++) << 24);
		return *reinterpret_cast<int*>(&value);
	}

	bool action::read_bool(codestream_iterator& it)
	{
		return read_u8(it) != 0;
	}

	void action::execute(const character_ptr& target)
	{
		with_stack wstack;

		character_ptr ch = target;
		auto& env = ch->get_environment();
		int version = ch->get_player()->get_version();
		std::vector<uint8_t>::const_iterator ip = codestream_.begin();
		bool running = true;
		while(running) {
			if(ip == codestream_.end()) {
				break;
			}
			ActionCode current_ins = static_cast<ActionCode>(*ip);
			uint16_t len = 0;
			if(*ip >= 128) {
				len = *(++ip);
				len += *(++ip) << 8;
			}
			std::vector<uint8_t>::const_iterator args = ++ip;
			ip += len;
			LOG_DEBUG("\t\t" << action_to_string(current_ins));
			switch(current_ins) {
			case ActionCode::End:
				running = false;
				break;
			case ActionCode::NextFrame:
				ch->next_frame();
				break;
			case ActionCode::PreviousFrame:
				ch->prev_frame();
				break;
			case ActionCode::Play:
				ch->set_play_state(PlayState::PLAYING);
				break;
			case ActionCode::Stop:
				ch->set_play_state(PlayState::STOPPED);
				break;
			case ActionCode::ToggleQuality:
				ch->toggle_quality();
				break;
			case ActionCode::StopSounds:
				ch->stop_sounds();
				break;
			case ActionCode::GotoFrame: {
				ch->set_current_frame(read_u16(args));
				break;
			}
			case ActionCode::GetURL: {
				std::string url = read_string(args);
				std::string target_str = read_string(args);
				LOG_DEBUG("Read URL: " << url << " into target: " << target_str);
				break;
			}
			case ActionCode::WaitForFrame: {
				int frame_to_wait = read_u16(args);
				int actions_to_skip = read_u8(args);
				if(!ch->wait_frame(frame_to_wait)) {
					// XXX is this instructions to skip or actions -- needs clarifying.
					ip += actions_to_skip;
				}
				break;
			}
			case ActionCode::SetTarget: {
				std::string target_str = read_string(args);
				if(target_str.empty()) {
					ch = target;
				} else {
					ch = ch->get_player()->get_root_movie()->get_named_character(target_str);
				}
				env = ch->get_environment();
				break;
			}
			case ActionCode::GotoLabel: {
				std::string label = read_string(args);
				int frame = ch->get_definition()->get_frame_from_label(label);
				ch->set_current_frame(frame);
				break;
			}
			case ActionCode::Push: {
				PushType type = static_cast<PushType>(read_u8(args));
				switch(type)
				{
				case PushType::STRING: 
					env.push(as_value::create(read_string(args))); 
					break;
				case PushType::FLOATING_POINT:
					env.push(as_value::create(read_float(args))); 
					break;
				case PushType::NULL_VALUE:
					env.push(as_value::create(as_object_ptr())); 
					break;
				case PushType::UNDEFINED:
					env.push(as_value::create()); 
					break;
				case PushType::REGISTER: {
					int reg = read_u8(args);
					env.push(as_value::create(reg));
				}
				case PushType::BOOLEAN:
					env.push(as_value::create(read_bool(args))); 
					break;
				case PushType::DOUBLE:
					env.push(as_value::create(read_double(args))); 
					break;
				case PushType::INTEGER:
					env.push(as_value::create(read_s32(args))); 
					break;
				case PushType::CONSTANT8:
					env.push(as_value::create(read_u8(args))); 
					break;
				case PushType::CONSTANT16:
					env.push(as_value::create(read_u16(args))); 
					break;
				default: 
					ASSERT_LOG(false, "Unhandled value type for pushing on stack: " << static_cast<int>(type));
					break;
				}
				break;
			}
			case ActionCode::Pop:
				// pop from stack and discard value.
				env.pop();
				break;
			case ActionCode::Add: {
				auto a = env.pop()->to_number();
				auto b = env.pop()->to_number();
				env.push(as_value::create(a+b));
				break;
			}
			case ActionCode::Subtract: {
				auto a = env.pop()->to_number();
				auto b = env.pop()->to_number();
				env.push(as_value::create(b-a));
				break;
			}
			case ActionCode::Multiply: {
				auto a = env.pop()->to_number();
				auto b = env.pop()->to_number();
				env.push(as_value::create(b*a));
				break;
			}
			case ActionCode::Divide: {
				auto a = env.pop()->to_number();
				auto b = env.pop()->to_number();
				if(std::abs(a) < DBL_EPSILON) {
					if(version == 4) {
						env.push(as_value::create("#ERROR#"));
					} else {
						env.push(as_value::create(std::numeric_limits<double>::infinity()));
					}
				} else {
					env.push(as_value::create(b/a));
				}
				break;
			}
			case ActionCode::Equals: {
				auto a = env.pop()->to_number();
				auto b = env.pop()->to_number();
				if(a == b) {
					if(version == 4) {
						env.push(as_value::create(1));
					} else {
						env.push(as_value::create(true));
					}
				} else {
					if(version == 4) {
						env.push(as_value::create(0));
					} else {
						env.push(as_value::create(false));
					}
				}
				break;
			}
			case ActionCode::Less: {
				auto a = env.pop()->to_number();
				auto b = env.pop()->to_number();
				if(b < a) {
					if(version == 4) {
						env.push(as_value::create(1));
					} else {
						env.push(as_value::create(true));
					}
				} else {
					if(version == 4) {
						env.push(as_value::create(0));
					} else {
						env.push(as_value::create(false));
					}
				}
				break;
			}
			case ActionCode::And: {
				auto a = env.pop()->to_number();
				auto b = env.pop()->to_number();
				if(a != 0 && b != 0) {
					if(version == 4) {
						env.push(as_value::create(1));
					} else {
						env.push(as_value::create(true));
					}
				} else {
					if(version == 4) {
						env.push(as_value::create(0));
					} else {
						env.push(as_value::create(false));
					}
				}
				break;
			}
			case ActionCode::Or: {
				auto a = env.pop()->to_number();
				auto b = env.pop()->to_number();
				if(a != 0 || b != 0) {
					if(version == 4) {
						env.push(as_value::create(1));
					} else {
						env.push(as_value::create(true));
					}
				} else {
					if(version == 4) {
						env.push(as_value::create(0));
					} else {
						env.push(as_value::create(false));
					}
				}
				break;
			}
			case ActionCode::Not: {
				auto a = env.pop()->to_number();
				if(a != 0) {
					if(version == 4) {
						env.push(as_value::create(1));
					} else {
						env.push(as_value::create(true));
					}
				} else {
					if(version == 4) {
						env.push(as_value::create(0));
					} else {
						env.push(as_value::create(false));
					}
				}				
				break;
			}
			case ActionCode::StringEquals: {
				auto a = env.pop()->to_std_string();
				auto b = env.pop()->to_std_string();
				if(a == b) {
					if(version == 4) {
						env.push(as_value::create(1));
					} else {
						env.push(as_value::create(true));
					}
				} else {
					if(version == 4) {
						env.push(as_value::create(0));
					} else {
						env.push(as_value::create(false));
					}
				}
				break;
			}
			case ActionCode::StringLength: {
				auto a = env.pop()->to_std_string();
				env.push(as_value::create(static_cast<int>(a.size())));
				break;
			}
			case ActionCode::StringAdd: {
				auto a = env.pop()->to_std_string();
				auto b = env.pop()->to_std_string();
				env.push(as_value::create(b+a));
				break;
			}
			case ActionCode::StringExtract: {
				auto count = env.pop()->to_int32();
				auto start = env.pop()->to_int32();
				auto str = env.pop()->to_std_string();
				if(start < 0 || count < 0 || start >= static_cast<int>(str.size()) || start+count >= static_cast<int>(str.size())) {
					env.push(as_value::create(""));
				} else {
					env.push(as_value::create(str.substr(start, count)));
				}
				break;
			}
			case ActionCode::StringLess: {
				auto a = env.pop()->to_std_string();
				auto b = env.pop()->to_std_string();
				if(a < b) {
					if(version == 4) {
						env.push(as_value::create(1));
					} else {
						env.push(as_value::create(true));
					}
				} else {
					if(version == 4) {
						env.push(as_value::create(0));
					} else {
						env.push(as_value::create(false));
					}
				}				
				break;
			}
			case ActionCode::MBStringLength: {
				auto a = to_wide_string(env.pop()->to_std_string());
				env.push(as_value::create(static_cast<int>(a.size())));
				break;
			}
			case ActionCode::MBStringExtract: {
				auto count = env.pop()->to_int32();
				auto start = env.pop()->to_int32();
				auto str = to_wide_string(env.pop()->to_std_string());
				if(start < 0 || count < 0 || start >= static_cast<int>(str.size()) || start+count >= static_cast<int>(str.size())) {
					env.push(as_value::create(""));
				} else {
					env.push(as_value::create(to_narrow_string(str.substr(start, count))));
				}
				break;
			}
			case ActionCode::ToInteger: {
				env.push(as_value::create(env.pop()->to_integer()));
				break;
			}
			case ActionCode::CharToAscii: {
				auto str = env.pop()->to_string();
				env.push(as_value::create(static_cast<int>(str[0])));
				break;
			}
			case ActionCode::AsciiToChar: {
				auto n = std::min(255, std::max(0, env.pop()->to_integer()));
				env.push(as_value::create(std::string({static_cast<char>(n)})));
				break;
			}
			case ActionCode::MBCharToAscii: {
				auto str = to_wide_string(env.pop()->to_string());
				env.push(as_value::create(static_cast<int>(str[0])));
				break;
			}
			case ActionCode::MBAsciiToChar: {
				auto n = env.pop()->to_integer();
				auto str = to_narrow_string(std::wstring{static_cast<wchar_t>(n)});
				env.push(as_value::create(str));
				break;
			}
			case ActionCode::Jump: {
				int distance = static_cast<int16_t>(read_u16(args));
				ip += distance;
				break;
			}
			case ActionCode::If: {
				bool branch = false;;
				if(version == 4) {
					branch = env.pop()->to_integer() != 0;
				} else {
					branch = env.pop()->to_boolean();
				}
				if(branch) {
					int distance = static_cast<int16_t>(read_u16(args));
					ip += distance;
				}
				break;
			}
			case ActionCode::Call: {
				ch->call_frame_actions(env.pop());
				break;
			}
			case ActionCode::GetVariable: {
				env.push(env.get_variable(env.pop()->to_string(), wstack));
				break;
			}
			case ActionCode::SetVariable: {
				auto value = env.pop();
				env.set_variable(env.pop()->to_string(), value, wstack);
				break;
			}
			case ActionCode::GetURL2: {
				int ub = read_u8(args);
				auto target = env.pop();
				auto url = env.pop();
				ASSERT_LOG(false, "XXX GetURL2: bits: " << static_cast<int>(ub) << " target:" << target << " url: " << url);
				break;
			}
			case ActionCode::GotoFrame2: {
				int ub = read_u8(args);
				if(ub & 1) {
					// scene bias bit set
					int scene_bias = read_u16(args);
				}
				auto frame = env.pop();
				ASSERT_LOG(false, "XXX: goto_frame2: bits: " << ub << ", frame: " << frame->to_std_string());
				break;
			}
			case ActionCode::SetTarget2: {
				std::string target_str = env.pop()->to_std_string();
				if(target_str.empty()) {
					ch = target;
				} else {
					ch = ch->get_player()->get_root_movie()->get_named_character(target_str);
				}
				env = ch->get_environment();
				break;
			}
			case ActionCode::GetProperty: {
				int index = env.pop()->to_integer();
				auto obj = env.pop()->to_object();
				as_value_ptr prop = obj->get_member(get_std_property_name(index));
				env.push(prop);
				break;
			}
			case ActionCode::SetProperty: {
				as_value_ptr value = env.pop();
				int index = env.pop()->to_integer();
				auto obj = env.pop()->to_object();
				obj->set_member(get_std_property_name(index), value);
				break;
			}
			case ActionCode::CloneSprite: {
				int depth = env.pop()->to_integer();
				std::string newname = env.pop()->to_std_string();
				as_value_ptr obj = env.pop();
				if(obj->is_object()) {
					obj->to_object()->clone_display_object(newname, depth);
				} else if(obj->is_string()) {
					obj = env.get_variable(obj->to_std_string(), wstack);
					if(obj) {
						obj->to_object()->clone_display_object(newname, depth);
					}
				}
				break;
			}
			case ActionCode::RemoveSprite: {
				auto target = env.find_target(env.pop());
				if(target && target->get_parent()) {
					target->get_parent()->remove_display_object(target);
				}
				break;
			}
			case ActionCode::StartDrag: {
				break;
			}
			case ActionCode::EndDrag: {
				break;
			}
			case ActionCode::WaitForFrame2: {
				break;
			}
			case ActionCode::Trace: {
				break;
			}
			case ActionCode::GetTime: {
				break;
			}
			case ActionCode::RandomNumber: {
				break;
			}
			case ActionCode::CallFunction: {
				break;
			}
			case ActionCode::CallMethod: {
				break;
			}
			case ActionCode::ConstantPool: {
				int num_consts = read_u16(args);
				std::vector<std::string> consts;
				consts.reserve(num_consts);
				for(int n = 0; n != num_consts; ++n) {
					consts.emplace_back(read_string(args));
				}
				env.set_constant_pool(consts);
				break;
			}
			case ActionCode::DefineFunction: {
				break;
			}
			case ActionCode::DefineLocal: {
				break;
			}
			case ActionCode::DefineLocal2: {
				break;
			}
			case ActionCode::Delete: {
				break;
			}
			case ActionCode::Delete2: {
				break;
			}
			case ActionCode::Enumerate: {
				break;
			}
			case ActionCode::Equals2: {
				break;
			}
			case ActionCode::GetMember: {
				break;
			}
			case ActionCode::InitArray: {
				break;
			}
			case ActionCode::InitObject: {
				break;
			}
			case ActionCode::NewMethod: {
				break;
			}
			case ActionCode::NewObject: {
				break;
			}
			case ActionCode::SetMember: {
				break;
			}
			case ActionCode::TargetPath: {
				break;
			}
			case ActionCode::With: {
				break;
			}
			case ActionCode::ToNumber: {
				break;
			}
			case ActionCode::ToString: {
				break;
			}
			case ActionCode::TypeOf: {
				break;
			}
			case ActionCode::Add2: {
				break;
			}
			case ActionCode::Less2: {
				break;
			}
			case ActionCode::Modulo: {
				break;
			}
			case ActionCode::BitAnd: {
				break;
			}
			case ActionCode::BitLShift: {
				break;
			}
			case ActionCode::BitOr: {
				break;
			}
			case ActionCode::BitRShift: {
				break;
			}
			case ActionCode::BitURShift: {
				break;
			}
			case ActionCode::BitXor: {
				break;
			}
			case ActionCode::Decrement: {
				break;
			}
			case ActionCode::Increment: {
				break;
			}
			case ActionCode::PushDuplicate: {
				break;
			}
			case ActionCode::Return: {
				break;
			}
			case ActionCode::StackSwap: {
				break;
			}
			case ActionCode::StoreRegister: {
				break;
			}
			case ActionCode::InstanceOf: {
				break;
			}
			case ActionCode::Enumerate2: {
				break;
			}
			case ActionCode::StrictEquals: {
				break;
			}
			case ActionCode::Greater: {
				break;
			}
			case ActionCode::StringGreater: {
				break;
			}
			case ActionCode::DefineFunction2: {
				break;
			}
			case ActionCode::Extends: {
				break;
			}
			case ActionCode::CastOp: {
				break;
			}
			case ActionCode::ImplementsOp: {
				break;
			}
			case ActionCode::Try: {
				break;
			}
			case ActionCode::Throw: {
				break;
			}
			default:
				break;
			}
		}
	}
}
