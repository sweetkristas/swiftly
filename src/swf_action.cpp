#include <map>

#include "asserts.hpp"
#include "swf_action.hpp"

namespace swf
{
	namespace 
	{
		typedef std::map<ActionCode, std::string> action_type;

		void init_action_to_string(action_type& a)
		{
            a[ActionCode::NextFrame] = "DefineNextFrame";
            a[ActionCode::PreviousFrame] = "DefinePreviousFrame";
            a[ActionCode::Play] = "DefinePlay";
            a[ActionCode::Stop] = "DefineStop";
            a[ActionCode::ToggleQuality] = "DefineToggleQuality";
            a[ActionCode::StopSounds] = "DefineStopSounds";
            a[ActionCode::GotoFrame] = "DefineGotoFrame";
            a[ActionCode::GetURL] = "DefineGetURL";
            a[ActionCode::WaitForFrame] = "DefineWaitForFrame";
            a[ActionCode::SetTarget] = "DefineSetTarget";
            a[ActionCode::GotoLabel] = "DefineGotoLabel";
            a[ActionCode::Push] = "DefinePush";
            a[ActionCode::Pop] = "DefinePop";
            a[ActionCode::Add] = "DefineAdd";
            a[ActionCode::Subtract] = "DefineSubtract";
            a[ActionCode::Multiply] = "DefineMultiply";
            a[ActionCode::Divide] = "DefineDivide";
            a[ActionCode::Equals] = "DefineEquals";
            a[ActionCode::Less] = "DefineLess";
            a[ActionCode::And] = "DefineAnd";
            a[ActionCode::Or] = "DefineOr";
            a[ActionCode::Not] = "DefineNot";
            a[ActionCode::StringEquals] = "DefineStringEquals";
            a[ActionCode::StringLength] = "DefineStringLength";
            a[ActionCode::StringAdd] = "DefineStringAdd";
            a[ActionCode::StringExtract] = "DefineStringExtract";
            a[ActionCode::StringLess] = "DefineStringLess";
            a[ActionCode::MBStringLength] = "DefineMBStringLength";
            a[ActionCode::MBStringExtract] = "DefineMBStringExtract";
            a[ActionCode::ToInteger] = "DefineToInteger";
            a[ActionCode::CharToAscii] = "DefineCharToAscii";
            a[ActionCode::AsciiToChar] = "DefineAsciiToChar";
            a[ActionCode::MBCharToAscii] = "DefineMBCharToAscii";
            a[ActionCode::MBAsciiToChar] = "DefineMBAsciiToChar";
            a[ActionCode::Jump] = "DefineJump";
            a[ActionCode::If] = "DefineIf";
            a[ActionCode::Call] = "DefineCall";
            a[ActionCode::GetVariable] = "DefineGetVariable";
            a[ActionCode::SetVariable] = "DefineSetVariable";
            a[ActionCode::GetURL2] = "DefineGetURL2";
            a[ActionCode::GotoFrame2] = "DefineGotoFrame2";
            a[ActionCode::SetTarget2] = "DefineSetTarget2";
            a[ActionCode::GetProperty] = "DefineGetProperty";
            a[ActionCode::SetProperty] = "DefineSetProperty";
            a[ActionCode::CloneSprite] = "DefineCloneSprite";
            a[ActionCode::RemoveSprite] = "DefineRemoveSprite";
            a[ActionCode::StartDrag] = "DefineStartDrag";
            a[ActionCode::EndDrag] = "DefineEndDrag";
            a[ActionCode::WaitForFrame2] = "DefineWaitForFrame2";
            a[ActionCode::Trace] = "DefineTrace";
            a[ActionCode::GetTime] = "DefineGetTime";
            a[ActionCode::RandomNumber] = "DefineRandomNumber";
            a[ActionCode::CallFunction] = "DefineCallFunction";
            a[ActionCode::CallMethod] = "DefineCallMethod";
            a[ActionCode::ConstantPool] = "DefineConstantPool";
            a[ActionCode::DefineFunction] = "DefineDefineFunction";
            a[ActionCode::DefineLocal] = "DefineDefineLocal";
            a[ActionCode::DefineLocal2] = "DefineDefineLocal2";
            a[ActionCode::Delete] = "DefineDelete";
            a[ActionCode::Delete2] = "DefineDelete2";
            a[ActionCode::Enumerate] = "DefineEnumerate";
            a[ActionCode::Equals2] = "DefineEquals2";
            a[ActionCode::GetMember] = "DefineGetMember";
            a[ActionCode::InitArray] = "DefineInitArray";
            a[ActionCode::InitObject] = "DefineInitObject";
            a[ActionCode::NewMethod] = "DefineNewMethod";
            a[ActionCode::NewObject] = "DefineNewObject";
            a[ActionCode::SetMember] = "DefineSetMember";
            a[ActionCode::TargetPath] = "DefineTargetPath";
            a[ActionCode::With] = "DefineWith";
            a[ActionCode::ToNumber] = "DefineToNumber";
            a[ActionCode::ToString] = "DefineToString";
            a[ActionCode::TypeOf] = "DefineTypeOf";
            a[ActionCode::Add2] = "DefineAdd2";
            a[ActionCode::Less2] = "DefineLess2";
            a[ActionCode::Modulo] = "DefineModulo";
            a[ActionCode::BitAnd] = "DefineBitAnd";
            a[ActionCode::BitLShift] = "DefineBitLShift";
            a[ActionCode::BitOr] = "DefineBitOr";
            a[ActionCode::BitRShift] = "DefineBitRShift";
            a[ActionCode::BitURShift] = "DefineBitURShift";
            a[ActionCode::BitXor] = "DefineBitXor";
            a[ActionCode::Decrement] = "DefineDecrement";
            a[ActionCode::Increment] = "DefineIncrement";
            a[ActionCode::PushDuplicate] = "DefinePushDuplicate";
            a[ActionCode::Return] = "DefineReturn";
            a[ActionCode::StackSwap] = "DefineStackSwap";
            a[ActionCode::StoreRegister] = "DefineStoreRegister";
            a[ActionCode::InstanceOf] = "DefineInstanceOf";
            a[ActionCode::Enumerate2] = "DefineEnumerate2";
            a[ActionCode::StrictEquals] = "DefineStrictEquals";
            a[ActionCode::Greater] = "DefineGreater";
            a[ActionCode::StringGreater] = "DefineStringGreater";
            a[ActionCode::DefineFunction2] = "DefineDefineFunction2";
            a[ActionCode::Extends] = "DefineExtends";
            a[ActionCode::CastOp] = "DefineCastOp";
            a[ActionCode::ImplementsOp] = "DefineImplementsOp";
            a[ActionCode::Try] = "DefineTry";
            a[ActionCode::Throw] = "DefineThrow";
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
	}

	action::action(int id, const std::shared_ptr<bit_stream>& bits)
		: id_(id)
	{
		bool done = false;
		while(!done) {
			uint8_t code = bits->read_unsigned8();
			if(code == 0) {
				done = true;
			} else {
				if(code >= 128) {
					uint16_t len = bits->read_unsigned16();
					auto new_code = bits->read_unsigned8(len);
					codestream_.emplace_back(static_cast<uint8_t>(len & 0xff));
					codestream_.emplace_back(static_cast<uint8_t>(len >> 8));
					std::copy(new_code.begin(), new_code.end(), codestream_.end());					
				} else {
					codestream_.emplace_back(code);
				}
			}
		}
	}

	action::~action()
	{
	}

	void action::execute(const character_ptr& ch)
	{
		std::vector<uint8_t>::const_iterator ip = codestream_.begin();
		while(ip != codestream_.end()) {
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
			case ActionCode::NextFrame:
				ch->next_frame();
				break;
			case ActionCode::PreviousFrame:
				ch->prev_frame();
				break;
			case ActionCode::Play:
				break;
			case ActionCode::Stop:
				break;
			case ActionCode::ToggleQuality:
				break;
			case ActionCode::StopSounds:
				break;
			case ActionCode::GotoFrame:
				break;
			case ActionCode::GetURL:
				break;
			case ActionCode::WaitForFrame:
				break;
			case ActionCode::SetTarget:
				break;
			case ActionCode::GotoLabel:
				break;
			case ActionCode::Push:
				break;
			case ActionCode::Pop:
				break;
			case ActionCode::Add:
				break;
			case ActionCode::Subtract:
				break;
			case ActionCode::Multiply:
				break;
			case ActionCode::Divide:
				break;
			case ActionCode::Equals:
				break;
			case ActionCode::Less:
				break;
			case ActionCode::And:
				break;
			case ActionCode::Or:
				break;
			case ActionCode::Not:
				break;
			case ActionCode::StringEquals:
				break;
			case ActionCode::StringLength:
				break;
			case ActionCode::StringAdd:
				break;
			case ActionCode::StringExtract:
				break;
			case ActionCode::StringLess:
				break;
			case ActionCode::MBStringLength:
				break;
			case ActionCode::MBStringExtract:
				break;
			case ActionCode::ToInteger:
				break;
			case ActionCode::CharToAscii:
				break;
			case ActionCode::AsciiToChar:
				break;
			case ActionCode::MBCharToAscii:
				break;
			case ActionCode::MBAsciiToChar:
				break;
			case ActionCode::Jump:
				break;
			case ActionCode::If:
				break;
			case ActionCode::Call:
				break;
			case ActionCode::GetVariable:
				break;
			case ActionCode::SetVariable:
				break;
			case ActionCode::GetURL2:
				break;
			case ActionCode::GotoFrame2:
				break;
			case ActionCode::SetTarget2:
				break;
			case ActionCode::GetProperty:
				break;
			case ActionCode::SetProperty:
				break;
			case ActionCode::CloneSprite:
				break;
			case ActionCode::RemoveSprite:
				break;
			case ActionCode::StartDrag:
				break;
			case ActionCode::EndDrag:
				break;
			case ActionCode::WaitForFrame2:
				break;
			case ActionCode::Trace:
				break;
			case ActionCode::GetTime:
				break;
			case ActionCode::RandomNumber:
				break;
			case ActionCode::CallFunction:
				break;
			case ActionCode::CallMethod:
				break;
			case ActionCode::ConstantPool:
				break;
			case ActionCode::DefineFunction:
				break;
			case ActionCode::DefineLocal:
				break;
			case ActionCode::DefineLocal2:
				break;
			case ActionCode::Delete:
				break;
			case ActionCode::Delete2:
				break;
			case ActionCode::Enumerate:
				break;
			case ActionCode::Equals2:
				break;
			case ActionCode::GetMember:
				break;
			case ActionCode::InitArray:
				break;
			case ActionCode::InitObject:
				break;
			case ActionCode::NewMethod:
				break;
			case ActionCode::NewObject:
				break;
			case ActionCode::SetMember:
				break;
			case ActionCode::TargetPath:
				break;
			case ActionCode::With:
				break;
			case ActionCode::ToNumber:
				break;
			case ActionCode::ToString:
				break;
			case ActionCode::TypeOf:
				break;
			case ActionCode::Add2:
				break;
			case ActionCode::Less2:
				break;
			case ActionCode::Modulo:
				break;
			case ActionCode::BitAnd:
				break;
			case ActionCode::BitLShift:
				break;
			case ActionCode::BitOr:
				break;
			case ActionCode::BitRShift:
				break;
			case ActionCode::BitURShift:
				break;
			case ActionCode::BitXor:
				break;
			case ActionCode::Decrement:
				break;
			case ActionCode::Increment:
				break;
			case ActionCode::PushDuplicate:
				break;
			case ActionCode::Return:
				break;
			case ActionCode::StackSwap:
				break;
			case ActionCode::StoreRegister:
				break;
			case ActionCode::InstanceOf:
				break;
			case ActionCode::Enumerate2:
				break;
			case ActionCode::StrictEquals:
				break;
			case ActionCode::Greater:
				break;
			case ActionCode::StringGreater:
				break;
			case ActionCode::DefineFunction2:
				break;
			case ActionCode::Extends:
				break;
			case ActionCode::CastOp:
				break;
			case ActionCode::ImplementsOp:
				break;
			case ActionCode::Try:
				break;
			case ActionCode::Throw:
				break;
			default:
				break;
			}
		}
	}
}
