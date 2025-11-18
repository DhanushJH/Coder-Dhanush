#include<iostream>
#include<fstream>
#include<iomanip>
#include<string>
#include<json.hpp>

using json = nlohmann::json;

json loadtoJSON()
{
    json DatainFormat = json::array();
    std::ifstream insidefile("Jokes_log.json");
    
    if(insidefile.good())
    {
        try
        {
                insidefile >> DatainFormat;
        }
        catch(const std::exception& e)
        {
            std::cerr << "File is empty" << '\n';
        }   
    }
    insidefile.close();

    return DatainFormat;
}

void SavetoFile(const json& DatainFormat)
{
    std::ofstream outputfile("Jokes_log.json");
    outputfile << std::setw(4) << DatainFormat << std::endl;
}

void DisplayallJokes(const json& DatainFormat)
{
    if(DatainFormat.empty())
    {
        std::cout << "No Jokes found in File" << std::endl;
        return;
    }

    std::cout << "<=====Saved Jokes=====>" << std::endl;
    for(int i = 0; i < DatainFormat.size(); i++)
    {
        std::cout << i + 1 << ". " << DatainFormat[i]["joke"] <<
                              " [" << DatainFormat[i]["id"] << "]" <<
                              "  " << DatainFormat[i]["timestamp"] << "\n";
    }
}

void SearchJokes(const json& DatainFormat)
{
    std::cout << "Enter the keyword to search :";
    std::string sKeyword;
    std::getline(std::cin, sKeyword);

    bool found = false;

    for(auto &entry : DatainFormat)
    {
        std::string sJoke = entry["joke"];
        if(sJoke.find(sKeyword) != std::string::npos)
        {
            found = true;
            std::cout << sJoke <<
                 " [" << entry["id"] << "]" <<
                 "  " << entry["timestamp"] << "\n";
        }
    }

    if(!found)
        std::cout << "No Match found" << "\n";
}

void DeleteJoke(json& DatainFormat)
{
    std::cout << "Enter the id of the joke to be deleted :";
    std::string sId;
    std::getline(std::cin,sId);
    
    for(int i = 0; i < DatainFormat.size(); i++)
    {
        if(DatainFormat[i]["id"] == sId)
        {
            DatainFormat.erase(DatainFormat.begin() + i);
            SavetoFile(DatainFormat);
            std::cout << "Deleted ID :" << sId << "\n";
            return;
        }
    }
    std::cout << "ID not found in File" << "\n";
}

int main()
{
    while(true)
    {
        json Database = loadtoJSON();

        std::cout << "<====JOKES DASHBOARD====>" << "\n";
        std::cout << "Total Jokes stored in file: " << Database.size() << "\n";
        std::cout << "1. View all jokes \n";
        std::cout << "2. Search jokes by Id \n";
        std::cout << "3. Delete Jokes by Id \n";
        std::cout << "4. Exit \n";

        std::string choice;
        std::getline(std::cin,choice);

        if(choice == "1")
            DisplayallJokes(Database);
        else if(choice == "2")
            SearchJokes(Database);
        else if(choice == "3")
            DeleteJoke(Database);
        else if(choice == "4")
            break;
        else
            std::cout << "Invalid Option" << "\n";
    }
    std::cout << "Exiting Dashboard \n";
    return 0;
}