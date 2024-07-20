#include <curl/curl.h>
#include "jtbstr.h"

class Curl {
    CURL* curlpointer;
    static size_t write__callback(void* contents, size_t size, size_t nmemb, void* userdata);
public: 
    Curl(): curlpointer(curl_easy_init()) {
	if (!curlpointer) { throw std::runtime_error("curl initialization failed"); }
    }

    ~Curl() { curl_easy_cleanup(curlpointer); }

    enum class HTTP_MODE { GET, POST };

    void curl_execute(JTB::Str&,
		  JTB::Str&,
		  HTTP_MODE mode = HTTP_MODE::GET,
		  JTB::Str post_command = "", 
		  size_t (*write_callback)(void* contents, size_t size, size_t nmemb, void* userdata) = write__callback);
};

inline void Curl::curl_execute(JTB::Str& readBuffer,
		  JTB::Str& URL,
		  HTTP_MODE mode,
		  JTB::Str post_command, 
		  size_t (*write_callback)(void* contents, size_t size, size_t nmemb, void* userdata)) {

    curl_easy_setopt(curlpointer, CURLOPT_URL, URL.c_str());

    if (mode == HTTP_MODE::POST) {
	/* this ensures we send a post request <= 09/18/23 18:39:06 */ 
	curl_easy_setopt(curlpointer, CURLOPT_POST, 1L);

	/* this is what sets the posted data <= 09/18/23 18:39:16 */ 
	curl_easy_setopt(curlpointer, CURLOPT_POSTFIELDS, post_command.c_str());
    }     

    // Send all returned data to this function
    curl_easy_setopt(curlpointer, CURLOPT_WRITEFUNCTION, write_callback);

    // Pass our 'readBuffer' to the callback function
    curl_easy_setopt(curlpointer, CURLOPT_WRITEDATA, &readBuffer);

    curl_easy_setopt(curlpointer, CURLOPT_TIMEOUT, 5L);  // timeout after 5 seconds

    curl_easy_setopt(curlpointer, CURLOPT_CONNECTTIMEOUT, 10L);  // connection timeout after 5 seconds

    /* TURN THIS ON FOR DEBUGGING <= 09/18/23 13:25:46 */ 
    /* curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); */

    // Perform the request, and check for errors
    CURLcode res = curl_easy_perform(curlpointer);

    if(res != CURLE_OK) {
	/* endwin(); */
	/* std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl; */
	throw std::runtime_error(curl_easy_strerror(res));
    } 
}
