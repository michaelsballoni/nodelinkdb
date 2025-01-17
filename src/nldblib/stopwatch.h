#pragma once

#include <chrono>
#include <string>

namespace nldb
{
	class stopwatch
	{
	public:
		stopwatch(const std::string& section)
			: m_section(section)
			, m_hits(0)
			, m_total_elapsed_ms(0)
		{
			start();
		}

		void start() 
		{
			m_point = m_clock.now();
		}

		void record() 
		{
			++m_hits;
			auto elapsed_ms_count = std::chrono::duration_cast<std::chrono::milliseconds>(m_clock.now() - m_point);
			m_total_elapsed_ms += (int)elapsed_ms_count.count();
		}

		void print() 
		{
			printf("%s: %d hits - %d ms total\n", 
				   m_section.c_str(), m_hits, m_total_elapsed_ms);
		}

	private:
		std::string m_section;
		int m_hits;
		int m_total_elapsed_ms;

		std::chrono::high_resolution_clock m_clock;
		std::chrono::time_point<std::chrono::high_resolution_clock> m_point;
	};
}
