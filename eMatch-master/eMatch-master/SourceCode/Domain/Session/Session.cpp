#include "Domain/Session/Session.hpp"
#include "TechnicalServices/Persistence/PersistenceHandler.hpp"

#include <string>
#include <any>
#include <vector>

namespace  // anonymous (private) working area
{
  // 1)  First define all system events (commands, actions, requests, etc.)
  #define STUB(functionName)  std::any functionName( Domain::Session::SessionBase & /*session*/, const std::vector<std::string> & /*args*/ ) \
                              { return {}; }  // Stubbed for now

  STUB( bugPeople    )
  STUB( collectFines )
  STUB( help         )
  STUB( openArchives )
  STUB( payFines     )
  STUB( resetAccount )
  STUB( returnBook   )
  STUB( shutdown     )


  std::any checkoutBook( Domain::Session::SessionBase & session, const std::vector<std::string> & args )
  {
    // TO-DO  Verify there is such a book and the mark the book as being checked out by user
    std::string results = "Title \"" + args[0] + "\" checkout by \"" + session._credentials.userName + '"';
    session._logger << "checkoutBook:  " + results;
    return {results};
  }

  std::any troubleshootIssues(Domain::Session::SessionBase& session, const std::vector<std::string>& args)
  {
      // TO-DO  Verify there is such a book and the mark the book as being checked out by user
      std::string results = " Try logging off, clear cookies and cache and log back in. If problems persist please contact support@ematch.com";
      std::cout << "-----------------------------------------------------------------------------------------------------------------------\n";
      std::cout << results;
      std::cout << "\n-----------------------------------------------------------------------------------------------------------------------\n";
      //session._logger << "troubleshootIssues:  " + results;
      return { results };
  }
  

  std::any searchJob(Domain::Session::SessionBase& session, const std::vector<std::string>& args)
  {
      // TO-DO  Search job by criteria
      if (args.size() == 3) {
          std::string keyword = args[0] == "0" ? "" : args[0];
          std::string location = args[1] == "0" ? "" : args[1];
          std::string category = args[2] == "0" ? "" : args[2];
          std::string results = "Job \"" + keyword + "/" + location + "/" + category + "/\" searched by \"" + session._credentials.userName + '"';
          session._logger << "searchJob:  " + results;
          
          auto& persistentData = TechnicalServices::Persistence::PersistenceHandler::instance();
          std::vector<TechnicalServices::Persistence::JobInfo> searchResult = persistentData.searchByCriteria(args);
          
          if (!searchResult.empty()) {
              session.setSearchResult(searchResult);
              session.display();
              return { results };
          }
          else {
              results = "[Warning] No search results";
              return { results };
          }
      }
      else {
          std::string results = "[ERROR] ARGS NOT VALID";
          return { results };
      }
  }


  std::any getJobInfo(Domain::Session::SessionBase& session, const std::vector<std::string>& args)
  {
      // TO-DO  get job info
      int selectedNum = std::stoi(args[0]) - 1;
      std::vector<TechnicalServices::Persistence::JobInfo> searchResult = session._searchResult;
      
      
      if (selectedNum >= 0 && selectedNum < searchResult.size()) {
          
          TechnicalServices::Persistence::JobInfo selectedJob = searchResult.at(selectedNum);
          std::string results = "Job Info \"" + selectedJob.name + "\" viewed by \"" + session._credentials.userName + '"';
          session._logger << "jobInfo:  " + results;
          
          int jobId = selectedJob.id;
          session._selectedJobId = jobId;
          session.display(jobId);
          return { results };
      }
      else {
          std::string results = "[Warning] Number Out of Range";
          return { results };
      }
  }


  std::any applyForJob(Domain::Session::SessionBase& session, const std::vector<std::string>& args)
  {
      // TO-DO  make application
      int jobId = session._selectedJobId;
      TechnicalServices::Persistence::JobInfo selectedJob = session.getJob(jobId);
      
      std::string results = "Applied Job \"" + selectedJob.name + "\" by \"" + session._credentials.userName + '"';

      auto& persistentData = TechnicalServices::Persistence::PersistenceHandler::instance();
      bool success = persistentData.makeApplication(session._credentials.userName, jobId);
      
      if (!success) results = "[Warning] already applied!";
      
      session._logger << "Apply for Job:  " + results;
      return { results };
  }

  std::any viewApplications(Domain::Session::SessionBase& session, const std::vector<std::string>& args)
  {
      // TO-DO  Verify there is such a book and the mark the book as being checked out by user
      auto& persistentData = TechnicalServices::Persistence::PersistenceHandler::instance();
      std::string userName = session._credentials.userName;
      std::vector<TechnicalServices::Persistence::Application> appliedJobs = persistentData.getUserApplication(userName);

      std::string results = "Job applications \"length " + std::to_string(appliedJobs.size()) + "\" viewed by \"" + userName + '"';
      session._logger << "Application status:  " + results;
      session.display(appliedJobs);

      return { results };
  }
}    // anonymous (private) working area










namespace Domain::Session
{
  SessionBase::SessionBase( const std::string & description, const UserCredentials & credentials ) : _credentials( credentials ), _name( description )
  {
    _logger << "Session \"" + _name + "\" being used and has been successfully initialized";
  }




  SessionBase::~SessionBase() noexcept
  {
    _logger << "Session \"" + _name + "\" shutdown successfully";
  }


  void SessionBase::display(std::vector<TechnicalServices::Persistence::Application> appliedJobs) {
      
      if (!appliedJobs.empty()) {
          std::cout << "\n----------------------------------------------------------------------------------------------\n";
          std::cout << "Application status for " << appliedJobs.at(0).userName << "\n";
          int i = 1;
          for (const auto& app : appliedJobs) {
              //std::cout << i << ") " + job.name + " | " + job.location + " | " + job.category + " | " + job.description + " | " + job.qualification + " | " + job.salary + "\n";
              TechnicalServices::Persistence::JobInfo job = getJob(app.jobId);
              std::cout << i << ") job name: " + job.name + " | status: " + app.status + "\n";
              i++;
          }
          std::cout << "----------------------------------------------------------------------------------------------\n";
      }
  }

  TechnicalServices::Persistence::JobInfo SessionBase::getJob(int jobId) {
      for (const auto& job : _searchResult) if (job.id == jobId) return job;
  }

  void SessionBase::display() {
      std::vector<TechnicalServices::Persistence::JobInfo> searchResult = _searchResult;
      std::cout << "\n----------------------------------------------------------------------------------------------\n";
      std::cout << "searchResult size: " << searchResult.size() << "\n";
      int i = 1;
      for (const auto& job : searchResult) {
          //std::cout << i << ") " + job.name + " | " + job.location + " | " + job.category + " | " + job.description + " | " + job.qualification + " | " + job.salary + "\n";
          std::cout << i << ") " + job.name + " | " + job.location + " | " + job.category + "\n";
          i++;
      }
      std::cout << "----------------------------------------------------------------------------------------------\n";
  }


  void SessionBase::display(int jobId) {
      std::cout << "\n----------------------------------------------------------------------------------------------\n";
      std::cout << "Job info\n";
      for (const auto& job : _searchResult) {
          if (job.id == jobId) {
              std::cout << "name : " << job.name;
              std::cout << "\nlocation : " << job.location;
              std::cout << "\ncategory : " << job.category;
              std::cout << "\ntype : " << job.type;
              std::cout << "\ndescripton : " << job.description;
              std::cout << "\nqualification : " << job.qualification;
              std::cout << "\nsalary : " << job.salary;
              break;
          }
      }
      std::cout << "\n----------------------------------------------------------------------------------------------\n";
  }

  
  void SessionBase::setSearchResult(std::vector<TechnicalServices::Persistence::JobInfo> searchResult) {
      _searchResult = searchResult;
  }

  std::vector<std::string> SessionBase::getCommands()
  {
    std::vector<std::string> availableCommands;
    availableCommands.reserve( _commandDispatch.size() );

    for( const auto & [command, function] : _commandDispatch ) availableCommands.emplace_back( command );

    return availableCommands;
  }

  std::any SessionBase::executeCommand( const std::string & command, const std::vector<std::string> & args )
  {
    std::string parameters;
    for( const auto & arg : args )  parameters += '"' + arg + "\"  ";
    //_logger << "Responding to \"" + command + "\" request with parameters: " + parameters;

    auto it = _commandDispatch.find( command );
    if( it == _commandDispatch.end() )
    {
      std::string message = __func__;
      message += " attempt to execute \"" + command + "\" failed, no such command";

      //_logger << message;
      throw BadCommand( message );
    }

    auto results = it->second( *this, args);

    if( results.has_value() )
    {
      // The type of result depends on function called.  Let's assume strings for now ...
      //_logger << "Responding with: \"" + std::any_cast<const std::string &>( results );
    }

    return results;
  }








  // 2) Now map the above system events to roles authorized to make such a request.  Many roles can request the same event, and many
  //    events can be requested by a single role.
  AdministratorSession::AdministratorSession( const UserCredentials & credentials ) : SessionBase( "Administrator", credentials )
  {
    _commandDispatch = { {"View Logs",            help        },
                         {"Security",   resetAccount},
                         {"Shutdown System", shutdown    } };
  }




  TroubleShootSesson::TroubleShootSesson( const UserCredentials & credentials ) : SessionBase( "JobSeekerTroubleshoot", credentials )
   {
    _commandDispatch = { 
                         {"Troubleshoot Issues", troubleshootIssues},
                         {"Shutdown System", shutdown    }
                   };
  }



  JobSeekerSession::JobSeekerSession( const UserCredentials & credentials ) : SessionBase( "JobSeeker", credentials )
  {
    _commandDispatch = { {"Search Job",  searchJob},
                         {"Get Job Info", getJobInfo},
                         {"Apply for Job",   applyForJob},
                         {"View Applications", viewApplications} };

  }




  ManagementSession::ManagementSession( const UserCredentials & credentials ) : SessionBase( "Management", credentials )
  {
    _commandDispatch = { {"Bug People", bugPeople},
                         {"Help",       help} };
  }
}    // namespace Domain::Session
