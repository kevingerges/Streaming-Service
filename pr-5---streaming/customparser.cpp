
#include <iostream>
#include <fstream>
#include <cctype>
#include <sstream>
#include "strmsrv.h"
#include "customparser.h"
#include "content.h"
#include "user.h"
// #include "jsonparser.h"

using namespace std;

const char* error_msg_1 = "Cannot read integer n";
const char* error_msg_2 = "Error in content parsing";
const char* error_msg_3 = "Error in user parsing";


// helper function to take out tabs and new lines and whitespaces
void strip(std::string &str) {
    for(char remover:{' ', '\t', '\n'}){
        str.erase(str.find_last_not_of(remover));
        str.erase(0, str.size() - 1);
    }
}


// To Do - Complete this function
void CustomFormatParser::parse(std::istream& is, std::vector<Content*>& content, std::vector<User*>& users)
{
    // Erase any old contents
    content.clear();
    users.clear();

    
    int n; is >> n;
    // throwing errors if failing
    if (is.fail())
    {
        throw ParserError(error_msg_1);

    }

    int id, type, reviews, stars, rating, episode, limit;
    string cname, username_line, uname, viewd_line;
    vector<string> user, view_str;
    vector<int> viewd;

    for(int i=0; i<n; i++){
        user.clear();
        is >> id >> type;
        // throwing errors if failing
        if (is.fail())
        {
          throw ParserError(error_msg_2);

        }
        is.ignore(100, '\n'); // Get rid of the \n character

        getline(is, cname);
        strip(cname);
        // checking types if 0 then movie if 1 then series
        if(type == 0)
        {
          // taking in values 
            is >> reviews >> stars >> rating;
            if (is.fail()){
              throw ParserError(error_msg_2);
            }
        }
            
        else
        {
          // taking in values 
            is >> reviews >> stars >> rating >> episode;
            if (is.fail())
            {
              throw ParserError(error_msg_2);
            }
        }
     
        getline(is, username_line); //Get rid of the \n character
        getline(is, username_line);

        Content *temp;

        if(type == 0)
        {
           // assginnig values to temp pointer by types
            temp = new Movie(id, cname, reviews, stars, rating);
        }
        else if (type == 1){
          // assginnig values to temp pointer by types
            temp = new Series(id, cname, reviews, stars, rating, episode);
        }
        // using stringstream to identify
        stringstream ss;
        ss << username_line;
        string temp_user;
        // taking in and checking 
        while (ss >> temp_user)
        {
          // adding the Viewer
          temp -> addViewer(temp_user);
        }
        // finally pushing back and populating it to the vector
        content.push_back(temp);
  
    }
  
    while (is >> uname )
    {
      // throwing errors if failing
        if (is.fail())
        {
          throw ParserError(error_msg_3);
        }
        is >> limit;
        // throwing errors if failing
        if (is.fail())
        {
          throw ParserError(error_msg_3);
        }
        string viewed_line;
        getline(is, viewed_line); //Get rid of the \n
        getline(is, viewed_line);
        stringstream temp_ss(viewed_line);
        int index;
        
        User *temp = new User(uname, limit); // creating a temo pointer

        while (temp_ss >> index){
           // adding to the history 
          temp->addToHistory(index);
        }
        // pushing back to temp
        users.push_back(temp);
  }
 
}
