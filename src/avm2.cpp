#include "asserts.hpp"
#include "avm2.hpp"

namespace avm2
{
	abc_file::abc_file(uint32_t flags, const std::string& name, swf::bit_stream_ptr bits)
		: flags_(flags), name_(name)
	{
		// read version
		minor_version_ = bits->read_unsigned16();
		major_version_ = bits->read_unsigned16();
		// read constants
		constants_.reset(new constant_pool(bits));
		// read method signatures
		uint32_t method_count = bits->read_u30();
		for(uint32_t n = 0; n != method_count; ++n) {
			methods_.push_back(method_info::read(bits, *constants_));
		}
		// Read and discard metadata
		uint32_t item_count = bits->read_u30();
		for(uint32_t n = 0; n != item_count; ++n) {
			bits->read_u30();	// key, index into string constants
			bits->read_u30();	// value, index into string constants
		}
		// Read instances.
		uint32_t class_count = bits->read_u30();
		for(uint32_t n = 0; n != class_count; ++n) {
			instances_.push_back(instance_info::read(bits, *this));
		}
		// Read classes
		for(uint32_t n = 0; n != class_count; ++n) {
			classes_.push_back(class_info::read(bits, *this));
		}
		// Read scripts
		uint32_t script_count = bits->read_u30();
		for(uint32_t n = 0; n != script_count; ++n) {
			scripts_.push_back(script_info::read(bits, *this));
		}
		// Read method bodies
		uint32_t method_body_count = bits->read_u30();
		for(uint32_t n = 0; n != method_body_count; ++n) {
			method_bodies_.push_back(method_body_info::read(bits, *this));
		}
	}

	abc_file::~abc_file()
	{}

	method_info_ptr abc_file::find_method(uint32_t index) const
	{
		ASSERT_LOG(index < methods_.size(), "method index out of range: " << index << " >= " << methods_.size());
		return methods_[index];
	}

	instance_info_ptr abc_file::find_instance(uint32_t index) const
	{
		ASSERT_LOG(index < instances_.size(), "instance index out of range: " << index << " >= " << instances_.size());
		return instances_[index];
	}

	class_info_ptr abc_file::find_class(uint32_t index) const
	{
		ASSERT_LOG(index < classes_.size(), "instance index out of range: " << index << " >= " << classes_.size());
		return classes_[index];
	}

	script_info_ptr abc_file::find_script(uint32_t index) const
	{
		ASSERT_LOG(index < scripts_.size(), "instance index out of range: " << index << " >= " << scripts_.size());
		return scripts_[index];
	}

	method_body_info_ptr abc_file::find_method_body(uint32_t index) const
	{
		ASSERT_LOG(index < method_bodies_.size(), "method body index out of range: " << index << " >= " << method_bodies_.size());
		return method_bodies_[index];
	}

	constant_pool::constant_pool(swf::bit_stream_ptr bits)
	{
		size_t int_count = bits->read_u30();
		integers_.reserve(int_count);
		integers_.push_back(0);
		for(size_t n = 0; int_count && n != int_count-1; ++n) {
			integers_.push_back(bits->read_s32());
		}
		size_t uint_count = bits->read_u30();
		uintegers_.reserve(uint_count);
		uintegers_.push_back(0);
		for(size_t n = 0; uint_count && n != uint_count-1; ++n) {
			uintegers_.push_back(bits->read_u32());
		}
		size_t double_count = bits->read_u30();
		doubles_.reserve(double_count);
		doubles_.push_back(0.0);
		for(size_t n = 0; double_count && n != double_count-1; ++n) {
			doubles_.push_back(bits->read_double());
		}
		size_t string_count = bits->read_u30();
		strings_.reserve(string_count);
		strings_.push_back("");
		for(size_t n = 0; string_count && n != string_count-1; ++n) {
			strings_.push_back(bits->read_avm2_string());
		}

		size_t namespace_count = bits->read_u30();
		namespaces_.reserve(namespace_count);
		namespaces_.push_back(ns_info_ptr());
		for(size_t n = 0; namespace_count && n != namespace_count-1; ++n) {
			namespaces_.push_back(ns_info::read(bits, *this));
		}
		size_t ns_set_count = bits->read_u30();
		ns_sets_.reserve(ns_set_count);
		ns_sets_.push_back(ns_set_info_ptr());
		for(size_t n = 0; ns_set_count && n != ns_set_count-1; ++n) {
			ns_sets_.push_back(ns_set_info::read(bits, *this));
		}
		size_t multiname_count = bits->read_u30();
		multinames_.reserve(multiname_count);
		multinames_.push_back(multiname_info_ptr());
		for(size_t n = 0; multiname_count && n != multiname_count-1; ++n) {
			multinames_.push_back(multiname_info::read(bits, *this));
		}
	}

	constant_pool::~constant_pool()
	{}

	int32_t constant_pool::find_integer(uint32_t index) const
	{
		ASSERT_LOG(index < integers_.size(), "integer index out of range: " << index << " >= " << integers_.size());
		return integers_[index];
	}

	uint32_t constant_pool::find_uinteger(uint32_t index) const
	{
		ASSERT_LOG(index < uintegers_.size(), "unsigned integer index out of range: " << index << " >= " << uintegers_.size());
		return uintegers_[index];
	}

	double constant_pool::find_double(uint32_t index) const
	{
		ASSERT_LOG(index < doubles_.size(), "double index out of range: " << index << " >= " << doubles_.size());
		return doubles_[index];
	}

	std::string constant_pool::find_string(uint32_t index) const
	{
		ASSERT_LOG(index < strings_.size(), "String index out of range: " << index << " >= " << strings_.size());
		return strings_[index];
	}

	ns_info_ptr constant_pool::find_namespace(uint32_t index) const
	{
		ASSERT_LOG(index < namespaces_.size(), "namespace index out of range: " << index << " >= " << namespaces_.size());
		return namespaces_[index];
	}

	ns_set_info_ptr constant_pool::find_namespace_set(uint32_t index) const
	{
		ASSERT_LOG(index < ns_sets_.size(), "namespace set index out of range: " << index << " >= " << ns_sets_.size());
		return ns_sets_[index];
	}

	multiname_info_ptr constant_pool::find_multiname(uint32_t index) const
	{
		ASSERT_LOG(index < multinames_.size(), "multi-name set index out of range: " << index << " >= " << multinames_.size());
		return multinames_[index];
	}

	ns_info_ptr ns_info::read(swf::bit_stream_ptr bits, const constant_pool& cp)
	{
		ns_info::NamspaceKind kind = ns_info::NamspaceKind(bits->read_unsigned8());
		std::string name = cp.find_string(bits->read_u30());
		return ns_info_ptr(new ns_info(kind, name));
	}

	ns_set_info_ptr ns_set_info::read(swf::bit_stream_ptr bits, const constant_pool& cp)
	{
		ns_set_info nssi;
		uint32_t count = bits->read_u30();
		for(size_t n = 0; n != count; ++n) {
			uint32_t index = bits->read_u30();
			ASSERT_LOG(index != 0, "Indexes in ns_set array must not be zero.");
			nssi.ns.push_back(cp.find_namespace(index));
		}
		return ns_set_info_ptr(new ns_set_info(nssi));
	}

	multiname_info_ptr multiname_info::read(swf::bit_stream_ptr bits, const constant_pool& cp)
	{
		multiname_info* mi = new multiname_info();
		mi->kind = multiname_info::MultinameKind(bits->read_unsigned8());
		switch(mi->kind) {
			case CONSTANT_QName:
			case CONSTANT_QNameA:
				mi->ns = cp.find_namespace(bits->read_u30());
				mi->name = cp.find_string(bits->read_u30());
				return multiname_info_ptr(mi);
			case CONSTANT_RTQName:
			case CONSTANT_RTQNameA:
				mi->name = cp.find_string(bits->read_u30());
				return multiname_info_ptr(mi);
			case CONSTANT_RTQNameL:
			case CONSTANT_RTQNameLA:
				// nothing read.
				return multiname_info_ptr(mi);
			case CONSTANT_Multiname:
			case CONSTANT_MultinameA:
				mi->name = cp.find_string(bits->read_u30());
				mi->ns_set = cp.find_namespace_set(bits->read_u30());
				return multiname_info_ptr(mi);
			case CONSTANT_MultinameL:
			case CONSTANT_MultinameLA:
				mi->ns_set = cp.find_namespace_set(bits->read_u30());
				return multiname_info_ptr(mi);
			default:
				ASSERT_LOG(false, "Unexpected constant for multiname_info::MultinameKind: " << mi->kind);
		}
		return multiname_info_ptr();
	}

	option_info::option_info(uint8_t knd, uint32_t val, const constant_pool& cp)
	{
		kind = option_info::OptionKind(knd);
		switch(kind) {
			case CONSTANT_Int:		i = cp.find_integer(val); break;
			case CONSTANT_UInt:		ui = cp.find_uinteger(val); break;
			case CONSTANT_Double:	d = cp.find_double(val); break;
			case CONSTANT_Utf8:		s = cp.find_string(val); break;
			case CONSTANT_True:
			case CONSTANT_False:
			case CONSTANT_Null:
			case CONSTANT_Undefined:
				break;
			case CONSTANT_Namespace:
			case CONSTANT_PackageNamespace:
			case CONSTANT_PackageInternalNs:
			case CONSTANT_ProtectedNamespace:
			case CONSTANT_ExplicitNamespace:
			case CONSTANT_StaticProtectedNs:
			case CONSTANT_PrivateNs:
				ns = cp.find_namespace(val);
				break;
			default:
				ASSERT_LOG(false, "Invalid option_info kind: " << kind);
		}
	}

	option_info::option_info() : i(0), ui(0), d(0.0), kind(CONSTANT_Undefined)
	{
	}

	option_info_ptr option_info::read(swf::bit_stream_ptr bits, const constant_pool& cp)
	{
		option_info* oi = new option_info();
		uint32_t val = bits->read_u30();
		oi->kind = option_info::OptionKind(bits->read_unsigned8());
		switch(oi->kind) {
			case CONSTANT_Int:		oi->i = cp.find_integer(val); break;
			case CONSTANT_UInt:		oi->ui = cp.find_uinteger(val); break;
			case CONSTANT_Double:	oi->d = cp.find_double(val); break;
			case CONSTANT_Utf8:		oi->s = cp.find_string(val); break;
			case CONSTANT_True:
			case CONSTANT_False:
			case CONSTANT_Null:
			case CONSTANT_Undefined:
				break;
			case CONSTANT_Namespace:
			case CONSTANT_PackageNamespace:
			case CONSTANT_PackageInternalNs:
			case CONSTANT_ProtectedNamespace:
			case CONSTANT_ExplicitNamespace:
			case CONSTANT_StaticProtectedNs:
			case CONSTANT_PrivateNs:
				oi->ns = cp.find_namespace(val);
				break;
			default:
				ASSERT_LOG(false, "Invalid option_info kind: " << oi->kind);
		}
		return option_info_ptr(oi);
	}

	method_info_ptr method_info::read(swf::bit_stream_ptr bits, const constant_pool& cp)
	{
		method_info* mi = new method_info();

		uint32_t param_count = bits->read_u30();
		mi->return_type_ = cp.find_multiname(bits->read_u30());
		for(uint32_t n = 0; n != param_count; ++n) {
			mi->param_types_.push_back(cp.find_multiname(bits->read_u30()));
		}
		mi->name_ = cp.find_string(bits->read_u30());
		mi->flags_ = bits->read_unsigned8();
		if(mi->flags_ & method_info::HAS_OPTIONAL) {
			uint32_t option_count = bits->read_u30();
			for(uint32_t n = 0; n != option_count; ++n) {
				mi->options_.push_back(option_info::read(bits, cp));
			}
		}
		if(mi->flags_ & method_info::HAS_PARAM_NAMES) {
			// not used by interpreter, so read and discard
			for(uint32_t n = 0; n != param_count; ++n) {
				bits->read_u30();
			}
		}
		return method_info_ptr(mi);
	}

	class_info_ptr class_info::read(swf::bit_stream_ptr bits, const abc_file& abc)
	{
		class_info* ci = new class_info();
		ci->cinit_ = abc.find_method(bits->read_u30());
		uint32_t trait_count = bits->read_u30();
		for(uint32_t n = 0; n != trait_count; ++n) {
			ci->traits_.push_back(trait_info::read(bits, abc));
		}
		return class_info_ptr(ci);
	}

	trait_info_ptr trait_info::read(swf::bit_stream_ptr bits, const abc_file& abc)
	{
		uint32_t name_ndx = bits->read_u30();
		ASSERT_LOG(name_ndx != 0, "trait name index must not be zero.");
		multiname_info_ptr name = abc.constants().find_multiname(name_ndx);
		uint8_t kind_and_attr = bits->read_unsigned8();
		trait_info::TraitKind kind = trait_info::TraitKind(kind_and_attr & 0x0f);
		uint8_t attributes = kind_and_attr >> 4;
		trait_info_ptr traits;

		switch(kind) {
			case Trait_Slot:
			case Trait_Const: {
				trait_slot* slot = new trait_slot();
				slot->name_ = name;
				slot->kind = kind;
				slot->attributes = attributes;
				slot->slot_id = bits->read_u30();
				slot->type_name = abc.constants().find_multiname(bits->read_u30());
				uint32_t val = bits->read_u30();
				if(val) {
					slot->voption.reset(new option_info(bits->read_unsigned8(), val, abc.constants()));
				}
				traits.reset(slot);
				break;
			}
			case Trait_Class: {
				trait_class* cls = new trait_class();
				cls->name_ = name;
				cls->kind = kind;
				cls->attributes = attributes;
				cls->slot_id = bits->read_u30();
				cls->class_ptr = abc.find_class(bits->read_u30());
				traits.reset(cls);
				break;
			}
			case Trait_Function: {
				trait_function* fn = new trait_function();
				fn->name_ = name;
				fn->kind = kind;
				fn->attributes = attributes;
				fn->slot_id = bits->read_u30();
				fn->function = abc.find_method(bits->read_u30());
				traits.reset(fn);
				break;
			}

			case Trait_Method:
			case Trait_Getter:
			case Trait_Setter: {
				trait_method* method = new trait_method();
				method->name_ = name;
				method->kind = kind;
				method->attributes = attributes;
				method->disp_id = bits->read_u30();
				method->method = abc.find_method(bits->read_u30());
				traits.reset(method);
				break;
			}
			default: ASSERT_LOG(false, "unexpected kind of trait: " << kind);
		}
		if(traits->attributes & trait_info::ATTR_Metadata) {
			// read and discard metadata.
			uint32_t metadata_count = bits->read_u30();
			for(uint32_t n = 0; n != metadata_count; ++n) {
				bits->read_u30();
			}
		}
		return traits;
	}

	instance_info_ptr instance_info::read(swf::bit_stream_ptr bits, const abc_file& abc)
	{
		instance_info* ii = new instance_info();
		ii->name_ = abc.constants().find_multiname(bits->read_u30());
		ii->super_name_ = abc.constants().find_multiname(bits->read_u30());
		ii->flags_ = bits->read_unsigned8();
		if(ii->flags_ & instance_info::CONSTANT_ClassProtectedNs) {
			ii->protected_ns_ = abc.constants().find_namespace(bits->read_u30());
		}
		uint32_t interface_count = bits->read_u30();
		for(uint32_t n = 0; n != interface_count; ++n) {
			ii->interfaces_.push_back(abc.constants().find_multiname(bits->read_u30()));
		}
		ii->iinit_ = abc.find_method(bits->read_u30());
		uint32_t trait_count = bits->read_u30();
		for(uint32_t n = 0; n != trait_count; ++n) {
			ii->traits_.push_back(trait_info::read(bits, abc));
		}
		return instance_info_ptr(ii);
	}

	script_info_ptr script_info::read(swf::bit_stream_ptr bits, const abc_file& abc)
	{
		script_info* si = new script_info();
		si->sinit_ = abc.find_method(bits->read_u30());
		uint32_t trait_count = bits->read_u30();
		for(uint32_t n = 0; n != trait_count; ++n) {
			si->traits_.push_back(trait_info::read(bits, abc));
		}
		return script_info_ptr(si);
	}

	vm_exception_info_ptr vm_exception_info::read(swf::bit_stream_ptr bits, const abc_file& abc, const method_body_info& method_body)
	{
		vm_exception_info* ei = new vm_exception_info();
		ei->from_ = method_body.get_code_iterator(bits->read_u30());
		ei->to_ = method_body.get_code_iterator(bits->read_u30());
		ei->target_ = method_body.get_code_iterator(bits->read_u30());
		ei->type_ = abc.constants().find_string(bits->read_u30());
		ei->name_ = abc.constants().find_string(bits->read_u30());
		return vm_exception_info_ptr(ei);
	}

	method_body_info_ptr method_body_info::read(swf::bit_stream_ptr bits, const abc_file& abc)
	{
		method_body_info* body = new method_body_info();
		body->method_index_ = bits->read_u30();
		body->max_stack_ = bits->read_u30();
		body->local_count_ = bits->read_u30();
		body->init_scope_depth_ = bits->read_u30();
		body->max_scope_depth_ = bits->read_u30();
		uint32_t code_length = bits->read_u30();
		body->code_.reserve(code_length);
		for(uint32_t n = 0; n != code_length; ++n) {
			body->code_.push_back(bits->read_unsigned8());
		}
		uint32_t exception_count = bits->read_u30();
		for(uint32_t n = 0; n != exception_count; ++n) {
			body->exceptions_.push_back(vm_exception_info::read(bits, abc, *body));
		}
		uint32_t trait_count = bits->read_u30();
		for(uint32_t n = 0; n != trait_count; ++n) {
			body->traits_.push_back(trait_info::read(bits, abc));
		}
		// set the method_info structure with the correct method body.
		method_body_info_ptr mbi(body);
		auto mi = abc.find_method(body->method_index_);
		mi->set_method_body(mbi);
		return mbi;
	}

	code_type_iterator method_body_info::get_code_iterator(uint32_t position) const
	{
		ASSERT_LOG(position < code_.size(), "position resides outside of code size:" << position << " >= " << code_.size());
		return code_.begin() + position;
	}

	void abc_file::call_entry_point()
	{
		scripts_.back()->call();
	}

	void method_info::call() 
	{
		if(method_) {
			method_->call();
		}
	}

	void script_info::call()
	{
		ASSERT_LOG(sinit_ != NULL, "sinit_ was NULL.");
		sinit_->call();
	}
}
