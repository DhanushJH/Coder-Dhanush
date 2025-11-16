#include<iostream>
#include<fstream>
#include<string>
#include<curl/curl.h>

using namespace std;

// CallBack function used is curl_easy_setopt
static size_t WriteCallback(void* contents, size_t size,                    // size of the elemenets whcih is usually 1
                            size_t number_of_elements_present_in_response,  // number of elemets/data that comes in response
                            void* user_pointer)                             // User defined pointer in which the actual reponse will be stored
{
    // casting the userp from void* to std::string*
    // casting the contents from void* to char*
    // Appending the contents and size of total received elements to userp
    ((std::string*)user_pointer)->append((char *)contents, size * number_of_elements_present_in_response);

    // THis returns the total size of the repsponse. If there is a mismatch, it is treated as an error and transfer stops
    return size * number_of_elements_present_in_response;
}

int main()
{
    CURL* curl;                         
    CURLcode result;
    std::string ResponseBuffer;

    curl = curl_easy_init();
    if(curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "https://icanhazdadjoke.com/");
        curl_easy_setopt(curl, CURLOPT_CAINFO, "cacert.pem");

        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Accept: application/json");

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ResponseBuffer);

        // curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        // curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        result = curl_easy_perform(curl);

        if(result != CURLE_OK)
        {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(result) << std::endl;
        }
        else
        {
            std::ofstream outputResponseFile("API_Response.json");
            outputResponseFile << ResponseBuffer;
            outputResponseFile.close();
            std::cout << "Saved API Response to file successfully" << std::endl;
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }
    else
    {
        std::cout << "failed to initialize libcurl" << std::endl;
    }

    return 0;
}
