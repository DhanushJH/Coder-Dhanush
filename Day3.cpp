#include<iostream>
#include<fstream>
#include<string>
#include<iomanip>           // Input/Output Manipulators , used here for setw
#include<vector>
#include<ctime>             // For Time related codes
#include<curl/curl.h>
#include<json.hpp>

using json = nlohmann::json;
using namespace::std;

std::string sGenerateId ()
{
    auto Time = std::time(nullptr);;
    auto RandomNumberGen = rand() % 100000;

    return std::to_string(Time) + "-" + std::to_string(RandomNumberGen);
}

std::string sGetCurrentTimestamp ()
{
    auto Time = std::time(nullptr);
    std::tm* LocalTime = std::localtime(&Time);

    char cTimeBuffer[32];
    std::strftime(cTimeBuffer, sizeof(cTimeBuffer), "%Y-%m-%d %H-%M-%S", LocalTime);

    return std::string(cTimeBuffer);
}

static size_t WriteCallBack (void* contents, size_t size, size_t number_of_elements, void* user_pointer)
{
    ((std::string*)user_pointer)->append((char*)contents, size * number_of_elements);

    return size * number_of_elements;
}

int main()
{
    srand(time(NULL));
    CURL* curl;
    CURLcode result;
    std::string sResponseBuffer;

    curl = curl_easy_init();

    if(curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "https://icanhazdadjoke.com/");
        curl_easy_setopt(curl,CURLOPT_CAINFO, "cacert.pem");

        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Accept: application/json");

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallBack);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &sResponseBuffer);

        result = curl_easy_perform(curl);

        if(result != CURLE_OK)
        {
            std::cerr << "curl_failed :" << curl_easy_strerror(result) << std::endl;
            return 1;
        }
        
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    json JokeJson;
    try
    {
        JokeJson = json::parse(sResponseBuffer);
    }
    catch(const std::exception &e)
    {
        std::cerr << "Failed to parse Json Response" << std::endl;
        return 1;
    }

    std::string sJoke = JokeJson.value("joke","JOKE_NOT_FOUND");
    std::string sID = sGenerateId();
    std::string sTime = sGetCurrentTimestamp();

    json Entry;
    Entry["id"] = sID;
    Entry["timestamp"] = sTime;
    Entry["joke"] = sJoke;

    json LogData = json::array();
    std::ifstream insidefile("Jokes_log.json");
    if(insidefile.good())
    {
        try
        {
            insidefile >> LogData;
        }
        catch(const std::exception &e)
        {
            std::cout << "Data not found inside file" << std::endl;
        }
    }
    insidefile.close();

    LogData.push_back(Entry);

    std::ofstream finalfile("Jokes_log.json");
    finalfile << std::setw(4) << LogData << std::endl;
    finalfile.close();

    std::cout << "ID :" << sID << std::endl;
    std::cout << "TimeStamp :" << sTime << std::endl;
    std::cout << "Joke :" << sJoke << std::endl;

    return 0;
}