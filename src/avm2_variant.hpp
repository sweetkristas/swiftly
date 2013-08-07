#pragma once

#include <cstdint>
#include <memory>

namespace avm2
{
	class object;
	typedef std::shared_ptr<object> object_ptr;

	class variant
	{
	public:
		variant();
		virtual ~variant();
	private:
		union {
			int32_t i_;
			uint32_t u_;
			double d_;
			bool b_;
		};
		object_ptr o_;
	};
}
