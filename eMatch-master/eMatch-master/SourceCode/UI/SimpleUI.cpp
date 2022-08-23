#include "UI/SimpleUI.hpp"



#include <any>         // any_cast()

#include <iomanip>     // setw()

#include <iostream>    // streamsize

#include <limits>      // numeric_limits

#include <memory>      // unique_ptr, make_unique<>()

#include <string>      // string, getline()

#include <vector>



#include "Domain/Library/Books.hpp"    // Include for now - will replace next increment

#include "Domain/Session/SessionHandler.hpp"



#include "TechnicalServices/Logging/LoggerHandler.hpp"

#include "TechnicalServices/Persistence/PersistenceHandler.hpp"









namespace UI

{

  // Default constructor

  SimpleUI::SimpleUI()

  : _bookHandler   ( std::make_unique<Domain::Library::Books>()                     ),   // will replace these with factory calls in the next increment

    _loggerPtr     ( TechnicalServices::Logging::LoggerHandler::create()            ),

    _persistentData( TechnicalServices::Persistence::PersistenceHandler::instance() )

  {

    _logger << "Simple UI being used and has been successfully initialized";

  }









  // Destructor

  SimpleUI::~SimpleUI() noexcept

  {

    _logger << "Simple UI shutdown successfully";

  }





  std::unique_ptr<Domain::Session::SessionHandler> login(std::string username, std::string password)

  {

      // 2) Present login screen to user and get username, password, and valid role

      Domain::Session::UserCredentials credentials = { "", "", {""} };           // ensures roles[0] exists

      auto& selectedRole = credentials.roles[0];     // convenience alias      

      

      credentials.userName = username;

      credentials.passPhrase = password;

 

 
 
      if (username == "abc") {     selectedRole = "JobSeeker"; }
       if (username == "abcd") {     selectedRole = "JobSeekerTroubleshoot"; }


       else if (username == "admin")  {  selectedRole = "Administrator"; }




      // 3) Validate user is authorized for this role, and if so create session

      std::unique_ptr<Domain::Session::SessionHandler> sessionControl;

      sessionControl = Domain::Session::SessionHandler::authenticate(credentials);



      return sessionControl;

  }



  // Operations

  void SimpleUI::launch()

  {

    // 1) Fetch Role legal value list

    std::vector<std::string> roleLegalValues = _persistentData.findRoles();

    std::string username;

    std::string password;

    std::string selectedRole;

    std::unique_ptr<Domain::Session::SessionHandler> sessionControl;

    bool shutdown = false;

    do

    {

      std::cin.ignore(  std::numeric_limits<std::streamsize>::max(), '\n' );


      std::cout << "\n< Login Ematch >\n";
      std::cout << "  name: ";

      std::getline( std::cin, username );



      std::cout << "  password: ";

      std::getline( std::cin, password );



      sessionControl = login(username, password);


   
 
      if (username == "abc") {     selectedRole = "JobSeeker"; }
      if (username == "abcd") {     selectedRole = "JobSeekerTroubleshoot"; }
      else if (username == "admin")  {  selectedRole = "Administrator"; }



      if( sessionControl != nullptr )

      {

        //_logger << "Login Successful for \"" + credentials.userName + "\" as role \"" + selectedRole + "\"";

        _logger << "Login Successful for \"" + username + "\" as role \"" + selectedRole + "\"";

        
      }
      else {

          std::cout << "** Login failed\n";

          _logger << "Login failure for \"" + username + "\" as role \"" + selectedRole + "\"";
          
          continue;
      }

      

      std::vector<std::string> parameters(3);

      std::string nextPage = "SearchResult";

      do {

          if (nextPage == "SearchResult" && selectedRole == "JobSeeker") {    // Search job

              std::cout << "\n< Search Job >\n Enter criteria (to skip, enter 0): \n";

              std::cout << " Enter keyword:  ";  std::cin >> std::ws;  std::getline(std::cin, parameters[0]);

              std::cout << " Enter location: ";  std::cin >> std::ws;  std::getline(std::cin, parameters[1]);

              std::cout << " Enter category:   ";  std::cin >> std::ws;  std::getline(std::cin, parameters[2]);



              auto results = sessionControl->executeCommand("Search Job", parameters);

              //if (results.has_value()) _logger << "Received reply: \"" + std::any_cast<const std::string&>(results);

              nextPage = "ViewInfo";



              std::string res = std::any_cast<const std::string&>(results);

              if (res.find("[") != std::string::npos) {   // if error found, do again
                  std::cout << "\n[ERROR] No Job Found!\n\n";
                  sessionControl = login("abcd", "abcd");
                  selectedRole = "JobSeekerTroubleshoot";
                  break;
                  //nextPage = "SearchResult";
              }

          }

          else if (nextPage == "ViewInfo") {   // Get job info

              std::cout << "\n< Job Info >\n";

              std::cout << "Select job (enter 0 for options, enter -1 to go back): \n";

              std::cout << " Enter Number:  ";  std::cin >> std::ws;  std::getline(std::cin, parameters[0]);



              if (parameters[0] == "0") {     // go to previous page

                  sessionControl->display();

              }

              else if (parameters[0] == "-1") {     // go to previous page

                  nextPage = "SearchResult";

              }

              else {

                  auto results = sessionControl->executeCommand("Get Job Info", parameters);

                  //if (results.has_value()) _logger << "Received reply: \"" + std::any_cast<const std::string&>(results);

                  nextPage = "ApplyForJob";



                  std::string res = std::any_cast<const std::string&>(results);

                  if (res.find("[") != std::string::npos) {   // if error found, do again

                      nextPage = "ViewInfo";

                  }

              }

          }

          else if (nextPage == "ApplyForJob") {    // Apply for job

              char response;

              do {

                  std::cout << "\n< Apply for Job >\n Do you want to apply for this job? (Y/N): ";

                  std::cin >> response;

                  response = (char)std::toupper(response);

              } while (response != 'Y' && response != 'N');



              if (response == 'Y') {

                  auto results = sessionControl->executeCommand("Apply for Job", parameters);

                  //if (results.has_value()) _logger << "Received reply: \"" + std::any_cast<const std::string&>(results);

                  nextPage = "ViewApplication";



                  std::string res = std::any_cast<const std::string&>(results);

                  if (res.find("[") != std::string::npos) {   // if error found, go back

                      nextPage = "ViewInfo";

                  }

                  else {

                      std::cout << "\n----------------------------------------------------------------------------------------------\n";

                      std::cout << "Application Success!";

                      std::cout << "\n----------------------------------------------------------------------------------------------\n";

                  }

              }

              else if (response == 'N') { // go to previous page

                  nextPage = "ViewInfo";

              }

          }

          else if (nextPage == "ViewApplication") { // View application

              char response;

              do {

                  std::cout << "\n< View Applicatons >\n Do you want to view your application status? (Y/N): ";

                  std::cin >> response;

                  response = (char)std::toupper(response);

              } while (response != 'Y' && response != 'N');

              if (response == 'Y') {

                  auto results = sessionControl->executeCommand("View Applications", parameters);

                  //if (results.has_value()) _logger << "Received reply: \"" + std::any_cast<const std::string&>(results);

                  nextPage = "SearchResult";  // go to start page



                  std::string res = std::any_cast<const std::string&>(results);

                  if (res.find("[") != std::string::npos) {   // if error found

                      nextPage = "ViewInfo";

                  }

              }

              if (response == 'N') nextPage = "ViewInfo"; // go to previous page

          }
          
          else {

              break;

          }



      } while (true);



      // 4) Fetch functionality options for this role

      do
      {

          auto        commands = sessionControl->getCommands();

          std::string selectedCommand;

          unsigned    menuSelection;


          std::cout << "\n< Select Action> \n";
          do

          {

              for (unsigned i = 0; i != commands.size(); ++i) std::cout << std::setw(2) << i << " - " << commands[i] << '\n';

              std::cout << std::setw(2) << commands.size() << " - " << "Quit\n";



              std::cout << "  action (0-" << commands.size() << "): ";

              std::cin >> menuSelection;

          } while (menuSelection > commands.size());



          if (menuSelection == commands.size()) {
              shutdown = true;
              break;
          };



          selectedCommand = commands[menuSelection];

          _logger << "Command selected \"" + selectedCommand + '"';


          if(selectedRole == "JobSeekerTroubleshoot"){

              if (selectedCommand == "Troubleshoot Issues")
              {
                  auto results = sessionControl->executeCommand(selectedCommand, parameters);
                  sessionControl = NULL;
                  break;
              }
              else {
                  std::cout << "\n[ERROR] Error Occured. Select Troubleshoot Issues.\n\n";
              }
          
          }
          

          else if (selectedCommand == "Shutdown System")

          {

              std::cout << " System is shutting down for maintenance.......\n";
              
              _logger << "Ending session and terminating";
              exit(0);

              break;
          }


      } while (true);

      if (shutdown) {
          std::cout << "\npress enter to shut down...";
          std::string s = "";
          std::getchar();
          std::getline(std::cin, s);
          break;
      };

    } 
    while( true );




    _logger << "Ending session and terminating";

  }

}


