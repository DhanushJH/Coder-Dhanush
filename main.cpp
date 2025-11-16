#include<iostream>
#include<fstream>
#include<string>
#include<curl/curl.h>
#include<json.hpp>

using json = nlohmann::json;

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
        
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);

        json jsonData; // Make an object of json

        try
        {
            jsonData = json::parse(ResponseBuffer); // Parse the JSON from readBuffer
        }
        catch (const std::exception &e)
        {
            std::cerr << "JSON parsing failed" << "\n";
            return 1;
        }

        std::string sJoke = jsonData.value("joke", "JOKE_NOT_FOUND"); // Read the json response. if key "joke" is present, inser the value of joke into sJoke, else "JOKE_NOT_FOUND"

        std::ofstream openfile("JOKES.txt", std::ios::app); // opens a file called JOKES.txt
        openfile << sJoke << "\n";                          // append the string sJoke into the file
        openfile.close();                                   // Close file

        std::ifstream insidefile("JOKES.txt");
        std::string sLine = "";
        int iCount;
        while (std::getline(insidefile, sLine))
            iCount++;

        std::cout << "Joke saved to file" << std::endl;
        std::cout << "Total number of jokes in file : " << iCount << std::endl;

        // std::ofstream outputResponseFile("API_Response.json");                   // was present in Day 1
        // outputResponseFile << ResponseBuffer;
        // outputResponseFile.close();
        // std::cout << "Saved API Response to file successfully" << std::endl;
    }
    else
    {
        std::cout << "failed to initialize libcurl" << std::endl;
    }

    return 0;
}
