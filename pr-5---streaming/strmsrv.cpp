#include "strmsrv.h"
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;


StreamService::StreamService()

{
    // Update as needed
    cUser_ = NULL; // setting user to NULL

}


StreamService::~StreamService()
{
    // deleting users and content;

    for (unsigned int i =0; i < content_.size(); i++){
        delete content_[i];
    }
    for (unsigned int i =0; i < users_.size(); i++){
        delete users_[i];
    }

}


void StreamService::readAndParseData(std::istream& is, Parser& p)
{
    p.parse(is, content_, users_);
    cout << "Read " << content_.size() << " content items." << endl;
    cout << "Read " << users_.size() << " users." << endl;
}


void StreamService::userLogin(const std::string& uname) 
{
    // checking if there is a user
    if (cUser_ != NULL)
    {
        throw runtime_error("error : there is user logged in");
    }
    // making user the username is valid
    else if (getUserIndexByName(uname) == -1)
    {
        throw invalid_argument("username is not valid");
        
    }
    // making user the username is valid
    else if (getUserIndexByName(uname) != -1)
    {
        cUser_ = users_[getUserIndexByName(uname)];
    }

}


void StreamService::userLogout()
{
    // checking if there is no users
    if (cUser_ == NULL)
    {
        // throw std::runtime_error("error : there is no user");
        return;
    }
    else // logging out by setting to NULL
    {
        cUser_ = NULL;

    }

}


std::vector<CID_T> StreamService::searchContent(const std::string& partial) const
{
    std::vector<CID_T> results;
    for(size_t i = 0; i < content_.size(); i++){
        // TO DO - modify this to also push back when the string 'partial'
        //  is contained in the name of the current content. Lookup the string
        //  class documentation to find an appropriate function that can be used
        //  to do this simply.
        
        // if partial equals * , populate the vector
        if(partial == "*" ){
            results.push_back(i);
        
        }
        // else if the name was found, use npos to 
        // double check and then populate the vector
        else {
          //  using find() and the constant string::npos 
          //to better search for content
            size_t find = (content_[i]->name()).find(partial);
            if (find != string::npos){
                results.push_back(i);
            }
        }    
    }
      return results;

}
  



std::vector<CID_T> StreamService::getUserHistory() const
{
    throwIfNoCurrentUser();
    return cUser_->history;
}


void StreamService::watch(CID_T contentID)
{

    
    throwIfNoCurrentUser(); // using the error identifer function

    if (!isValidContentID(contentID)) // checking error for valid content
    {
        throw ReviewRangeError("There is no such content");
    }

    // checking if accesing rating above the user limit 
    else if (content_[contentID]->rating()> (cUser_ ->ratingLimit))
    {
        throw RatingLimitError("Error, can't acces those rated content");
    }
    // if not do the needed operations

    // acces the history and id an then add addViewer to the history or 
    // basicall(watch it)
    cUser_ -> addToHistory(contentID);
    content_[contentID] -> addViewer(cUser_->uname);
    

}

// @brief Add a review with the given number of stars to the specified
//      *        content. We will NOT worry about a user reviewing content
//      *        more than once.
//      * 
//      * @param contentID ContentID to review
//      * @param numStars Number of stars for the review (must be 0-5)
//      * @throw UserNotLoggedInError if no User is currently logged in.
//      * @throw ReviewRangeError if the contentID is not valid or the 
//      *                    number of stars is out of the range 0-5
//      */

void StreamService::reviewShow(CID_T contentID, int numStars)
{
    throwIfNoCurrentUser();  // using the error identifer function

    // checking for a valid rating and valid content id
     if (!(isValidContentID(contentID)) || ( numStars < 0 || numStars > 5))              
     {
        throw ReviewRangeError("NO such a show exist");
     }
     // adding a review with the given number of stars 
    content_[contentID] -> review(numStars);


}

// helper function to check if there is matching id with my populated vector
bool search_vec(vector<CID_T> his_vec, CID_T content){ 
    for(int i:his_vec){ // basic linear search of vector
        if(i == content){ // checking for matches
            return true;
        }
    }
    return false;
}


CID_T StreamService::suggestBestSimilarContent(CID_T contentID) const
{
    // Change this when you are ready, but for now it is a 
    // dummy implementation

    throwIfNoCurrentUser(); // using the error identifer function

    //adding to history the contentis to of the current user
    cUser_->addToHistory(contentID); 

    // data members of this function
    vector<CID_T> his_vec;
    vector<User*> match_user;
    User *temp;
    int cur;
    vector<int> temp_vec;
    vector<vector<int>> final;
    vector<CID_T> cids;

    // first loop checking for if any content matches
    for(unsigned int i=0; i<users_.size(); i++){
        temp = users_[i]; // setting a temp pointer to users[i]
        his_vec = temp->history; //using vector to help
        if(search_vec(his_vec, contentID)){ // using the help of the helper func
            match_user.push_back(temp); // pushing back matching users
        }
    }
 // second loop checking for if any content that doesn't match with helper func
    for(unsigned int i=0; i<match_user.size(); i++){
        temp = match_user[i]; // setting a temp pointer to users[i]
        his_vec = temp->history; //using vector to help
        for(int j:his_vec){ //looping through the vector
        // using the help of the helper func to better 
        // identifer if not matches are found
            if(!search_vec(cUser_->history, j)){ 
                cids.push_back(j);
            }
        }
    }
    // checking if there is no contentID found
    if(cids.size() == 0)
        return -1;
    // sorting the vector of all contentID that was populated
    sort(cids.begin(), cids.end());
    cids.push_back(-1); // pushing back dummy varible for the sack of next loops

    // starting my comparison 
    cur = cids[0];

    for(unsigned int i=0; i<cids.size(); i++){
       // pushing back if matches my cur
        if(cids[i] == cur){
            temp_vec.push_back(cids[i]); // pushing back
        }
        else if(cids[i] != contentID){
            // moving on and increamenting to next contentID
            cur = cids[i];
            final.push_back(temp_vec); // push_back to my final vector
            temp_vec.clear(); // clearing 
            temp_vec.push_back(cids[i]); // push_back back the ids 
            
        }
    }
    
    unsigned int max_size = 0; // initializing max

    for(auto i:final){ // looking for the max out of the final vec
        // if (contentID)
        max_size = max((int)max_size, (int)i.size()); // getting max
    }
    for(auto i:final){ // returning the index of max contentID
        if(i.size() == max_size){ // comparison of i and max
            return i[0]; // returning
        }
    }

    return -1; 
}


void StreamService::displayContentInfo(CID_T contentID) const
{
    // Do not alter this
    if(! isValidContentID(contentID)){
        throw std::invalid_argument("Watch: invalid contentID");
    }

    // Call the display abitlity of the appropriate content object
    for(auto i:content_){  // looping through content 
        if(i->id() == contentID){  // checking if there is a valid id matches
            i->display(cout); // displaying content info 
        }
    }
}


bool StreamService::isValidContentID(CID_T contentID) const
{
    return (contentID >= 0) && (contentID < (int) content_.size());
}


void StreamService::throwIfNoCurrentUser() const
{
    if(cUser_ == NULL){
        throw UserNotLoggedInError("No user is logged in");
    }
}


int StreamService::getUserIndexByName(const std::string& uname) const
{
    for(size_t i = 0; i < users_.size(); i++){
        if(uname == users_[i]->uname) {
            return (int)i;
        }
    }
    return -1;
}
