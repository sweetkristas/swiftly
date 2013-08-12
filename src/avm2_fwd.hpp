#pragma once

#include <memory>
#include <vector>

namespace avm2
{
	// Forward declarations
	struct ns_info;
	typedef std::shared_ptr<ns_info> ns_info_ptr;
	struct ns_set_info;
	typedef std::shared_ptr<ns_set_info> ns_set_info_ptr;
	struct multiname_info;
	typedef std::shared_ptr<multiname_info> multiname_info_ptr;
	class constant_pool;
	typedef std::shared_ptr<constant_pool> constant_pool_ptr;
	struct option_info;
	typedef std::shared_ptr<option_info> option_info_ptr;
	class method_info;
	typedef std::shared_ptr<method_info> method_info_ptr;
	class class_info;
	typedef std::shared_ptr<class_info> class_info_ptr;
	struct trait_slot;
	typedef std::shared_ptr<trait_slot> trait_slot_ptr;
	struct trait_class;
	typedef std::shared_ptr<trait_class> trait_class_ptr;
	struct trait_method;
	typedef std::shared_ptr<trait_method> trait_method_ptr;
	struct trait_function;
	typedef std::shared_ptr<trait_function> trait_function_ptr;
	struct trait_info;
	typedef std::shared_ptr<trait_info> trait_info_ptr;
	class instance_info;
	typedef std::shared_ptr<instance_info> instance_info_ptr;
	class script_info;
	typedef std::shared_ptr<script_info> script_info_ptr;
	class vm_exception_info;
	typedef std::shared_ptr<vm_exception_info> vm_exception_info_ptr;
	class method_body_info;
	typedef std::shared_ptr<method_body_info> method_body_info_ptr;
	class abc_file;
	typedef std::shared_ptr<abc_file> abc_file_ptr;

	// Extra type definitions
	typedef std::vector<uint8_t> code_type;
	typedef std::vector<uint8_t>::const_iterator code_type_iterator;
}
