// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#pragma once

namespace ZoneTool
{
	template <typename T>
	class Function
	{
		using R = typename std::function<T>::result_type;

	protected:
		std::function<T> m_func;

	public:
		Function()
		{
		}

		Function(std::uintptr_t addr)
		{
			this->m_func = std::function<T>(reinterpret_cast<T*>(addr));
		}
		Function(FARPROC addr) : Function(std::uintptr_t(addr)) {}

		// Operators
		void operator=(const std::uintptr_t addr)
		{
			this->m_func = std::function<T>(reinterpret_cast<T*>(addr));
		}

		void operator=(const std::function<T> func)
		{
			this->m_func = func;
		}

		void operator=(const Function<T> func)
		{
			this->m_func = func->m_func;
		}

		template <typename... Args>
		R operator()(Args&&... args)
		{
			return this->m_func(std::forward<Args>(args)...);
		}
	};
}
