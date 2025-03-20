#include <iostream>
#include <vector>
#include <cmath>
#include <cstdint>
#include <windows.h>
#include <cmath>
#include <chrono>
#include <thread>
#include <boost/multiprecision/cpp_int.hpp> 
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp> 
#include <boost/multiprecision/float128.hpp>
#include <fmt/format.h>
#include <fmt/os.h>

using namespace std;
namespace mp = boost::multiprecision; 
typedef mp::number<mp::cpp_int_backend<1048576,1048576, mp::unsigned_magnitude, mp::checked, void>> mynum;
typedef mp::cpp_bin_float_oct myfloat;
long double five = 5.0;
myfloat sqrt5 = std::sqrtl(five);
myfloat pperf = ((1.0 + sqrt5)/ 2.0f );
myfloat nperf = ((1.0 - sqrt5)/ 2.0f );

void FibofI(const uintmax_t& I, mynum* value, std::vector<std::string>* str )
{	
	*value = static_cast<mynum>( ((pow( pperf ,I) - pow( nperf,I )) / sqrt5));
	(*str)[I] = value->str();
}

int main()
{
	std::ios_base::sync_with_stdio(false);
	LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds;
	LARGE_INTEGER Frequency;
	LARGE_INTEGER Total;
	QueryPerformanceFrequency(&Frequency); 
	QueryPerformanceCounter(&StartingTime);
	mynum* values = new mynum[100000] {0};
	std::vector<std::thread> threads;
	std::vector<std::string> svalues;
	svalues.resize(100000,"");
	threads.reserve(std::thread::hardware_concurrency());
	
	

	for(int i = 0; i < 100000; i++)
	{		
	
		threads.emplace_back(FibofI,i,&values[i], &svalues);
		
		if(threads.size() == std::thread::hardware_concurrency())
		{
			for(std::thread &a: threads)
			{
				a.join();
			}
			threads.clear();
		}
	}
	for(std::thread &a: threads)
	{
		a.join();
	}
	threads.clear();
	
	QueryPerformanceCounter(&EndingTime);
	if(values) delete[] values;
	ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;
	ElapsedMicroseconds.QuadPart *= 1000.0;
	ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;
	cout << ElapsedMicroseconds.QuadPart / 1000.0f  << " s"<< std::endl;	
	auto now = std::chrono::system_clock::now();
    time_t time = std::chrono::system_clock::to_time_t(now);
    tm* localTime = std::localtime(&time);
    stringstream sbuffer;
	sbuffer << put_time( localTime,"%Y_%m_%d_%H_%M_%S" );
	auto buffer = fmt::format("out_{}.txt",sbuffer.str());
	cout  << "Creating File: " << buffer << endl;
	auto fs = fmt::output_file(buffer.c_str());
	for(uintmax_t i = 0; i < 100000; i++)
	{
		fs.print("{}\r\n",svalues[i]);
	}
	
	fs.flush();
	fs.close();	
	
	QueryPerformanceCounter(&Total);
	ElapsedMicroseconds.QuadPart = Total.QuadPart - StartingTime.QuadPart;
	ElapsedMicroseconds.QuadPart *= 1000.0;
	ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;
	
	cout << "Finished in "  << ElapsedMicroseconds.QuadPart / 1000.0f << "s !" << std::endl;
	return 0;
}