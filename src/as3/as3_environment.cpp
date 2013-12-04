#include "as3_environment.hpp"
#include "../avm2_instructions.hpp"
#include "../swf_player.hpp"

namespace avm2
{
	namespace
	{
		uint32_t read_u30(code_type_iterator& code)
		{
			uint32_t ret_val = 0;
			uint8_t uc;
			int shift_cnt = 0;
			do {
				uc = *code++;
				ret_val |= uint32_t(uc & 0x7f) << shift_cnt; 
				shift_cnt += 7;
			} while(uc & 0x80);
			return ret_val;
		}
	}

	environment::environment()
	{
	}

	environment::~environment()
	{
	}

	void environment::execute(code_type_iterator code) 
	{
		uint8_t c = *code++;
		switch(c) {
		case add: {
			as3_value_ptr v1 = stack_.top(); stack_.pop();
			as3_value_ptr v2 = stack_.top(); stack_.pop();
			as3_value* result = new as3_value(*v1 + *v2);
			stack_.push(as3_value_ptr(result));
		}
		case add_i: {
			int32_t n1 = stack_.top()->to_int32(); stack_.pop();
			int32_t n2 = stack_.top()->to_int32(); stack_.pop();
			as3_value* result = new as3_value(n1+n2);
			stack_.push(as3_value_ptr(result));
		}
		case astype: {
			as3_value_ptr v1 = stack_.top(); stack_.pop();
			uint32_t mname_index = read_u30(code);
			/* XXX
				Look up mname_index in multiname pool, assert that it isn't a runtime multiname.
				if v1 == object and type(v1) == mname
				  push v1
				else 
				  push null
			*/
			stack_.push(as3_value_ptr());
		}
		case astypelate: {
		}
		case bitand: {
		}
		case bitnot: {
		}
		case bitor: {
		}
		case bitxor: {
		}
		case call: {
		}
		case callmethod: {
		}
		case callproperty: {
		}
		case callproplex: {
		}
		case callpropvoid: {
		}
		case callstatic: {
		}
		case callsuper: {
		}
		case callsupervoid: {
		}
		case checkfilter: {
		}
		case coerce: {
		}
		case coerce_a: {
		}
		case coerce_s: {
		}
		case construct: {
		}
		case constructprop: {
		}
		case constructsuper: {
		}
		case convert_b: {
		}
		case convert_i: {
		}
		case convert_d: {
		}
		case convert_o: {
		}
		case convert_u: {
		}
		case convert_s: {
		}
		case debug: {
		}
		case debugfile: {
		}
		case debugline: {
		}
		case declocal: {
		}
		case declocal_i: {
		}
		case decrement: {
		}
		case decrement_i: {
		}
		case deleteproperty: {
		}
		case divide: {
		}
		case dup: {
		}
		case dxns: {
		}
		case dxnslate: {
		}
		case equals: {
		}
		case esc_xattr: {
		}
		case esc_xelem: {
		}
		case findproperty: {
		}
		case findpropstrict: {
		}
		case getdescendents: {
		}
		case getglobalscope: {
		}
		case getglobalslot: {
		}
		case getlex: {
		}
		case getlocal: {
		}
		case getlocal_0: {
		}
		case getlocal_1: {
		}
		case getlocal_2: {
		}
		case getlocal_3: {
		}
		case getproperty: {
		}
		case getscopeobject: {
		}
		case getslot: {
		}
		case getsuper: {
		}
		case greaterequals: {
		}
		case greaterthan: {
		}
		case hasnext: {
		}
		case hasnext2: {
		}
		case ifeq: {
		}
		case iffalse: {
		}
		case ifge: {
		}
		case ifgt: {
		}
		case ifle: {
		}
		case iflt: {
		}
		case ifnge: {
		}
		case ifngt: {
		}
		case ifnle: {
		}
		case ifnlt: {
		}
		case ifne: {
		}
		case ifstricteq: {
		}
		case ifstrictne: {
		}
		case iftrue: {
		}
		case in: {
		}
		case inclocal: {
		}
		case inclocal_i: {
		}
		case increment: {
		}
		case increment_i: {
		}
		case initproperty: {
		}
		case instanceof: {
		}
		case istype: {
		}
		case istypelate: {
		}
		case jump: {
		}
		case kill: {
		}
		case label: {
		}
		case lessequals: {
		}
		case lessthan: {
		}
		case lookupswitch: {
		}
		case lshift: {
		}
		case modulo: {
		}
		case multiply: {
		}
		case multiply_i: {
		}
		case negate: {
		}
		case negate_i: {
		}
		case newactivation: {
		}
		case newarray: {
		}
		case newcatch: {
		}
		case newclass: {
		}
		case newfunction: {
		}
		case newobject: {
		}
		case nextname: {
		}
		case nextvalue: {
		}
		case nop: {
		}
		case not: {
		}
		case pop: {
		}
		case popscope: {
		}
		case pushbyte: {
		}
		case pushdouble: {
		}
		case pushfalse: {
		}
		case pushint: {
		}
		case pushnamespace: {
		}
		case pushnan: {
		}
		case pushnull: {
		}
		case pushscope: {
		}
		case pushshort: {
		}
		case pushstring: {
		}
		case pushtrue: {
		}
		case pushuint: {
		}
		case pushundefined: {
		}
		case pushwith: {
		}
		case returnvalue: {
		}
		case returnvoid: {
		}
		case rshift: {
		}
		case setlocal: {
		}
		case setlocal_0: {
		}
		case setlocal_1: {
		}
		case setlocal_2: {
		}
		case setlocal_3: {
		}
		case setglobalslot: {
		}
		case setproperty: {
		}
		case setslot: {
		}
		case setsuper: {
		}
		case strictequals: {
		}
		case subtract: {
		}
		case subtract_i: {
		}
		case swap: {
		}
		case throw_ex: {
		}
		case typeof: {
		}
		case urshift: {
		}
		}
	}
}
