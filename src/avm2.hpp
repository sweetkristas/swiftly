#pragma once

#include <map>
#include <string>
#include <cstdint>

#include "avm2_fwd.hpp"
#include "avm2_instructions.hpp"
#include "bit_reader.hpp"

/* ABC file structure
	u16 minor_version
	u16 major_version
	cpool_info constant_pool
	u30 method_count
	method_info method[method_count]
	u30 metadata_count
	metadata_info metadata[metadata_count]
	u30 class_count
	instance_info instance[class_count]
	class_info class[class_count]
	u30 script_count
	script_info script[script_count]
	u30 method_body_count
	method_body_info method_body[method_body_count]
*/

namespace avm2
{

	struct ns_info
	{
		enum NamspaceKind {
			CONSTANT_Namespace					= 0x08,
			CONSTANT_PackageNamespace			= 0x16,
			CONSTANT_PackageInternalNamespace	= 0x17,
			CONSTANT_ProtectedNamespace			= 0x18,
			CONSTANT_ExplicitNamespace			= 0x19,
			CONSTANT_StaticProtectedNamespace	= 0x1a,
			CONSTANT_PrivateNamespace			= 0x05,
		} kind;
		std::string name;

		ns_info(NamspaceKind k, const std::string& n) : kind(k), name(n) {}
		static ns_info_ptr read(swf::bit_stream_ptr bits, const constant_pool& cp);
	};

	struct ns_set_info
	{
		std::vector<ns_info_ptr> ns;

		static ns_set_info_ptr read(swf::bit_stream_ptr bits, const constant_pool& cp);
	};

	struct multiname_info
	{
		enum MultinameKind {
			CONSTANT_QName						= 0x07,
			CONSTANT_QNameA 					= 0x0d,
			CONSTANT_RTQName 					= 0x0f,
			CONSTANT_RTQNameA 					= 0x10,
			CONSTANT_RTQNameL 					= 0x11,
			CONSTANT_RTQNameLA 					= 0x12,
			CONSTANT_Multiname 					= 0x09,
			CONSTANT_MultinameA 				= 0x0e,
			CONSTANT_MultinameL 				= 0x1b,
			CONSTANT_MultinameLA 				= 0x1c,
		} kind;
		// Which of these is used depends on the value of 'kind'
		ns_info_ptr ns;
		std::string name;
		ns_set_info_ptr ns_set;

		static multiname_info_ptr read(swf::bit_stream_ptr bits, const constant_pool& cp);
	};

	class constant_pool
	{
	public:
		constant_pool(swf::bit_stream_ptr bits);
		virtual ~constant_pool();

		int32_t find_integer(uint32_t index) const;
		uint32_t find_uinteger(uint32_t index) const;
		double find_double(uint32_t index) const;
		std::string find_string(uint32_t index) const;
		ns_info_ptr find_namespace(uint32_t index) const;
		ns_set_info_ptr find_namespace_set(uint32_t index) const;
		multiname_info_ptr find_multiname(uint32_t index) const;
	private:
		std::vector<int32_t> integers_;
		std::vector<uint32_t> uintegers_;
		std::vector<double> doubles_;
		std::vector<std::string> strings_;
		std::vector<ns_info_ptr> namespaces_;
		std::vector<ns_set_info_ptr> ns_sets_;
		std::vector<multiname_info_ptr> multinames_;
	};

	struct option_info
	{
		option_info();
		explicit option_info(uint8_t knd, uint32_t val, const constant_pool& cp);
		enum OptionKind {
			CONSTANT_Int					= 0x03,
			CONSTANT_UInt					= 0x04,
			CONSTANT_Double					= 0x06,
			CONSTANT_Utf8					= 0x01,
			CONSTANT_True					= 0x0B,
			CONSTANT_False					= 0x0A,
			CONSTANT_Null					= 0x0C,
			CONSTANT_Undefined				= 0x00,
			CONSTANT_Namespace				= 0x08,
			CONSTANT_PackageNamespace		= 0x16,
			CONSTANT_PackageInternalNs		= 0x17,
			CONSTANT_ProtectedNamespace		= 0x18,
			CONSTANT_ExplicitNamespace		= 0x19,
			CONSTANT_StaticProtectedNs		= 0x1A,
			CONSTANT_PrivateNs				= 0x05,
		} kind;
		// exact value depends on kind.
		int32_t i;
		uint32_t ui;
		double d;
		std::string s;
		ns_info_ptr ns;

		static option_info_ptr read(swf::bit_stream_ptr bits, const constant_pool& cp);
	};
	
	class method_info
	{
	public:
		enum Flags {
			NEED_ARGUMENTS			= 0x01,
			NEED_ACTIVATION			= 0x02,
			NEED_REST				= 0x04,
			HAS_OPTIONAL			= 0x08,
			SET_DXNS				= 0x40,
			HAS_PARAM_NAMES			= 0x80,
		};

		method_info() : flags_(0) {}
		virtual ~method_info() {}

		static method_info_ptr read(swf::bit_stream_ptr bits, const constant_pool& cp);
	private:
		std::vector<multiname_info_ptr> param_types_;
		multiname_info_ptr return_type_;
		std::string name_;
		uint8_t flags_;
		std::vector<option_info_ptr> options_;
	};

	class class_info
	{
	public:
		class_info() {}
		virtual ~class_info() {}

		static class_info_ptr read(swf::bit_stream_ptr bits, const abc_file& abc);
	private:
		method_info_ptr cinit_;
		std::vector<trait_info_ptr> traits_;
	};

	struct trait_info
	{
		multiname_info_ptr name_;
		enum TraitKind {
			Trait_Slot		= 0,
			Trait_Method	= 1,
			Trait_Getter	= 2,
			Trait_Setter	= 3,
			Trait_Class		= 4,
			Trait_Function	= 5,
			Trait_Const		= 6,
		} kind;
		enum TraitAttribute {
			ATTR_Final		= 0x01,
			ATTR_Override	= 0x02,
			ATTR_Metadata	= 0x04,
		};
		uint8_t attributes;

		static trait_info_ptr read(swf::bit_stream_ptr bits, const abc_file& abc);
	};

	struct trait_slot : public trait_info
	{
		uint32_t slot_id;
		multiname_info_ptr type_name;
		option_info_ptr voption;
	};

	struct trait_class : public trait_info
	{
		uint32_t slot_id;
		class_info_ptr class_ptr;
	};

	struct trait_method : public trait_info
	{
		uint32_t disp_id;
		method_info_ptr method;
	};
	
	struct trait_function : public trait_info
	{
		uint32_t slot_id;
		method_info_ptr function;
	};
	
	class instance_info
	{
	public:
		enum Flags {
			CONSTANT_ClassSealed		= 0x01,
			CONSTANT_ClassFinal			= 0x02,
			CONSTANT_ClassInterface		= 0x04,
			CONSTANT_ClassProtectedNs	= 0x08,
		};
		instance_info() : flags_(0) {}
		virtual ~instance_info() {}

		static instance_info_ptr read(swf::bit_stream_ptr bits, const abc_file& abc);
	private:
		multiname_info_ptr name_;
		multiname_info_ptr super_name_;
		uint8_t flags_;
		ns_info_ptr protected_ns_;
		std::vector<multiname_info_ptr> interfaces_;
		method_info_ptr iinit_;
		std::vector<trait_info_ptr> traits_;
	};

	class script_info
	{
	public:
		script_info() {}
		virtual ~script_info() {}

		static script_info_ptr read(swf::bit_stream_ptr bits, const abc_file& abc);
	private:
		method_info_ptr sinit_;
		std::vector<trait_info_ptr> traits_;
	};
	
	class vm_exception_info
	{
	public:
		vm_exception_info() {}
		virtual ~vm_exception_info() {}

		static vm_exception_info_ptr read(swf::bit_stream_ptr bits, const abc_file& abc, const method_body_info& method_body);
	private:
		code_type_iterator from_;
		code_type_iterator to_;
		code_type_iterator target_;
		// he name of the type of exception that is to be monitored during the reign of this handler
		std::string type_;
		// the name of the variable that is to receive the exception object when the exception is thrown
		std::string name_;
	};
	
	class method_body_info
	{
	public:
		method_body_info() {}
		virtual ~method_body_info() {}

		code_type_iterator get_code_iterator(uint32_t position) const;

		static method_body_info_ptr read(swf::bit_stream_ptr bits, const abc_file& abc);
	private:
		method_info_ptr method_;
		// maximum number of evaluation stack slots used at any point during the execution of this body.
		uint32_t max_stack_;
		// index of the highest-numbered local register this method will use, plus one.
		uint32_t local_count_;
		// The minimum scope depth, relative to max_scope_depth, that may be accessed within the method.
		uint32_t init_scope_depth_;
		// Maximum scope depth that may be accessed within the method.
		uint32_t max_scope_depth_;
		// AVM2 instructions
		code_type code_;
		std::vector<vm_exception_info_ptr> exceptions_;
		std::vector<trait_info_ptr> traits_;
	};

	class abc_file
	{
	public:
		abc_file(uint32_t flags, const std::string& name, swf::bit_stream_ptr bits);
		virtual ~abc_file();

		const constant_pool& constants() const { return *constants_; }

		method_info_ptr find_method(uint32_t index) const;
		instance_info_ptr find_instance(uint32_t index) const;
		class_info_ptr find_class(uint32_t index) const;
		script_info_ptr find_script(uint32_t index) const;
		method_body_info_ptr find_method_body(uint32_t index) const;
	private:
		enum {
			ABC_LAZY_INITIALISE = 1
		};
		uint32_t flags_;
		std::string name_;

		uint16_t minor_version_;
		uint16_t major_version_;
		constant_pool_ptr constants_;
		std::vector<method_info_ptr> methods_;
		std::vector<instance_info_ptr> instances_;
		std::vector<class_info_ptr> classes_;
		std::vector<script_info_ptr> scripts_;
		std::vector<method_body_info_ptr> method_bodies_;
	};
}
