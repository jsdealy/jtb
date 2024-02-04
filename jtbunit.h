#include <iostream>
#include <functional>


namespace JTB {

/* Checks for a match between two values, 
 * the first
 * of which should be the 
 * output of a tested 
 * function, 
 * the second of 
 * which should be 
 * the expected correct output. <= 01/18/24 20:21:53 */ 
template <typename OutType>
void outputMatch(OutType functionOutput, OutType expectedOutput) {
    if (functionOutput == expectedOutput) {
	std::cout << ":)\tTest passed." << '\n';
    }
    else {
	std::cout << "\033[1;31m" << ":(\tTest failed." << "\n\tFunction output: " << functionOutput << "\n\tExpected output: " << expectedOutput << "\033[0m" << '\n';
    }
}

/* Checks for a match between a callback's return 
 * and an expected value. <= 01/18/24 20:20:54 */ 
template <typename OutType>
void outputMatch(OutType (*callback)(), OutType expectedOutput) {
    OutType ret = callback();
    if (ret == expectedOutput) {
	std::cout << ":)\tTest passed." << '\n';
    }
    else {
	std::cout << "\033[1;31m" << ":(\tTest failed." << "\n\tMethod output: " << ret << "\n\tExpected output: " << expectedOutput << "\033[0m" << '\n';
    }
}

/* Checks for a match between the returns of the 
 * two callbacks, 
 * which can be used to construct
 * objects and call methods on them. <= 01/18/24 20:16:17 */ 
template <typename OutType>
void outputMatch(OutType (*callback1)(), OutType (*callback2)()) {
    OutType ret1 = callback1();
    OutType ret2 = callback2();
    if (ret1 == ret2) {
	std::cout << ":)\tTest passed." << '\n';
    }
    else {
	std::cout << "\033[1;31m" << ":(\tTest failed." << "\n\tMethod output: " << ret1 << "\n\tExpected output: " << ret2 << "\033[0m" << '\n';
    }
}

/* @brief: Checks for an expected throw. 
 *
 * Should be passed a callback that 
 * is expected to throw 
 * and a temporary 
 *
 * exception of the expected
 * type with the expected
 * message. <= 01/18/24 20:24:09 */ 
template<typename OutType, typename ExceptionType>
void exceptionMatch(OutType (*callback)(), ExceptionType e) {
    try {
	callback();
    }
    catch (ExceptionType x) {
	if (std::string(x.what()) == std::string(e.what())) {
	    std::cout << ":)\tTest passed." << '\n';
	}
	else {
	    std::cout << "\033[1;31m" << ":(\tTest failed." << '\n';
	    std::cout << "\t\tException thrown: " << x.what() << '\n';
	    std::cout << "\t\tException expected: " << e.what() << "\033[0m" << '\n';

	}
    }
}

}

