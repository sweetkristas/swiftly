#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <cstdint>

#include "avm2_instructions.hpp"

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
	// Forward declarations
	struct trait_info;
	typedef std::shared_ptr<trait_info> trait_info_ptr;
	// Forward declarations ends

	typedef std::vector<uint8_t> code_type;
	typedef std::vector<uint8_t>::const_iterator code_type_iterator;

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
	};

	typedef std::shared_ptr<ns_info> ns_info_ptr;

	struct ns_set_info
	{
		std::vector<ns_info_ptr> ns;
	};

	typedef std::shared_ptr<ns_set_info> ns_set_info_ptr;

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
		std::string ns;
		std::string name;
		ns_set_info_ptr ns_set;
	};

	typedef std::shared_ptr<multiname_info> multiname_info_ptr;

	class constant_pool
	{
	public:
		constant_pool();
		virtual ~constant_pool();
	private:
		std::vector<int32_t> integers_;
		std::vector<uint32_t> uintegers_;
		std::vector<double> doubles_;
		std::vector<std::string> strings_;
		std::vector<ns_info_ptr> namespaces_;
		std::vector<ns_set_info_ptr> ns_sets_;
		std::vector<multiname_info_ptr> multinames_;
	};
	typedef std::shared_ptr<constant_pool> constant_pool_ptr;

	struct param_type
	{

	};

	struct option_info
	{
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
	};
	typedef std::shared_ptr<option_info> option_info_ptr;

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

		method_info();
		virtual ~method_info();
	private:
		std::vector<param_type> param_types_;
		multiname_info_ptr return_type_;
		std::string name_;
		uint8_t flags_;
		std::vector<option_info_ptr> options_;
	};
	typedef std::shared_ptr<method_info> method_info_ptr;


	class class_info
	{
	public:
		class_info();
		virtual ~class_info();
	private:
		method_info_ptr cinit_;
		std::vector<trait_info_ptr> traits_;
	};
	typedef std::shared_ptr<class_info> class_info_ptr;

	struct trait_slot
	{
		uint32_t slot_id;
		multiname_info_ptr type_name;
		option_info_ptr voption;
	};
	typedef std::shared_ptr<trait_slot> trait_slot_ptr;

	struct trait_class
	{
		uint32_t slot_id;
		class_info_ptr clss;
	};
	typedef std::shared_ptr<trait_class> trait_class_ptr;

	struct trait_method
	{
		uint32_t disp_id;
		method_info_ptr method;
	};
	typedef std::shared_ptr<trait_method> trait_method_ptr;

	struct trait_function
	{
		uint32_t slot_id;
		method_info_ptr function;
	};
	typedef std::shared_ptr<trait_function> trait_function_ptr;

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
	};
	typedef std::shared_ptr<trait_info> trait_info_ptr;

	class instance_info
	{
	public:
		enum Flags {
			CONSTANT_ClassSealed		= 0x01,
			CONSTANT_ClassFinal			= 0x02,
			CONSTANT_ClassInterface		= 0x04,
			CONSTANT_ClassProtectedNs	= 0x08,
		};
		instance_info();
		virtual ~instance_info();
	private:
		multiname_info_ptr name_;
		multiname_info_ptr super_name_;
		uint8_t flags_;
		ns_info_ptr protected_ns_;
		std::vector<multiname_info_ptr> interfaces_;
		method_info iinit_;
		std::vector<trait_info_ptr> traits_;
	};
	typedef std::shared_ptr<instance_info> instance_info_ptr;


	class script_info
	{
	public:
		script_info();
		virtual ~script_info();
	private:
		method_info_ptr sinit_;
		std::vector<trait_info_ptr> traits_;
	};
	typedef std::shared_ptr<script_info> script_info_ptr;

	class exception_info
	{
	public:
		exception_info();
		virtual ~exception_info();
	private:
		code_type_iterator from_;
		code_type_iterator to_;
		code_type_iterator target_;
		// he name of the type of exception that is to be monitored during the reign of this handler
		std::string type_;
		// the name of the variable that is to receive the exception object when the exception is thrown
		std::string name_;
	};
	typedef std::shared_ptr<exception_info> exception_info_ptr;

	class method_body_info
	{
	public:
		method_body_info();
		virtual ~method_body_info();
	private:
		uint32_t method_;
		// maximum number of evaluation stack slots used at any point during the execution of this body.
		uint32_t max_stack_;
		// index of the highest-numbered local register this method will use, plus one.
		uint32_t local_count_;
		// The minimum scope depth, relative to max_scope_depth, that may be accessed within the method.
		uint32_t init_scope_depth_;
		// Maximum scope depth that may be accessed within the method.
		uint32_t max_scope_depth_;
		// AVM2 instructions
		std::vector<uint8_t> code_;
		std::vector<exception_info_ptr> exceptions_;
		std::vector<trait_info_ptr> traits_;
	};
	typedef std::shared_ptr<method_body_info> method_body_info_ptr;

	class abc_file
	{
	public:
		abc_file();
		virtual ~abc_file();
	private:
		uint16_t minor_version_;
		uint16_t major_version_;
		constant_pool_ptr constants_;
		std::vector<method_info_ptr> methods_;
		std::vector<instance_info_ptr> instances_;
		std::vector<script_info_ptr> scripts_;
		std::vector<method_body_info_ptr> method_bodies_;
	};
}