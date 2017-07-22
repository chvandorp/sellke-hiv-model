/* main.cpp
 * 		author: chris
 * 	main for immuno-epi-HIV-model
 */

#include"main.hpp"



int main(int argc, char* argv[]) {
	/* the program accepts a seed given as argument
	 * if no seed is given, the program will prompt for one.
	 */

	parstruct allParams = cl_argreader(argc, argv);


	/* simulate an epidemic */
	std::cout << "# running...\n";

	epidemic myEpidemic(&allParams);

	std::cout << "\n# finished.\n";
	return 0;
}
