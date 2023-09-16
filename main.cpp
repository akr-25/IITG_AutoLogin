#include <cpr/cpr.h>
#include <iostream>
#include <regex>
#include <unistd.h>

void logout()
{
  auto sslOpts = cpr::VerifySsl{false};

  cpr::Response r = cpr::Get(cpr::Url{"https://192.168.193.1:1442:1442/logout?"}, sslOpts);
}

bool isConnectionActive()
{
  auto sslOpts = cpr::VerifySsl{false};

  cpr::Response r = cpr::Get(cpr::Url{"https://www.wikipedia.com/"}, sslOpts);
  if (r.text.find("IITG") != std::string::npos) return false;
  return r.status_code != 0;
}

std::string getMagicNumber()
{
  auto sslOpts = cpr::VerifySsl{false};

  cpr::Response r = cpr::Get(cpr::Url{"https://192.168.193.1:1442/login?"}, sslOpts);

  // using regex_search to extract the "magic" number --- name="magic" value="01423845de38bc29"
  std::regex rgx("name=\"magic\" value=\"([0-9a-f]+)\"");
  std::smatch match;
  std::string s = r.text;
  std::regex_search(s, match, rgx);
  return match[1];
}

void keepAlive(const std::string &url)
{
  while (true)
  {
    auto sslOpts = cpr::VerifySsl{false};
    cpr::Response r = cpr::Get(cpr::Url{url}, sslOpts);
    std::cout << "keepAlive running status_code [" << r.status_code << "]" << std::endl;
    if (r.status_code != 200) {
      std::cout << "status_code != 200, logging out" << std::endl;
      logout();
      break;
    }
    std::this_thread::sleep_for(std::chrono::seconds(10));
  }
}

void login(const std::string &username, const std::string &password, const std::string &magicNumber)
{
  auto sslOpts = cpr::VerifySsl{false};

  cpr::Response r = cpr::Post(
      cpr::Url{"https://192.168.193.1:1442/"},
      cpr::Payload{{"username", username}, {"password", password}, {"magic", magicNumber}, {"4Tredir", "https://www.wikipedia.com/"}},
      cpr::Header {{
      "referer", "https://192.168.193.1:1442/login?action=login"
    } }, sslOpts);

  std::cout << "login status_code (303 is good) [" << r.status_code << "] " << std::endl;
  std::string keepAliveUrl = r.header["Location"];
  std::cout << "keepAliveUrl [" << keepAliveUrl << "]" << std::endl;
  keepAlive(keepAliveUrl);
}

void maintainConnection(const std::string& username,const std::string& password)
{
  while(true){
    if(!isConnectionActive()){
      std::cout << "Connection is not active" << std::endl;
      std::string magicNumber = getMagicNumber();
      login(username, password, magicNumber);
    }
    std::cout << "Connection is active" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(10));
  }
}


// define these using -DUSERNAME="aman200123007" -DPASSWORD="guesswhat?" in the makefile
#define USERNAME "aman200123007"
#define PASSWORD "guesswhat?"

int main()
{
  #ifndef USERNAME
  std::cout << "Please define USERNAME" << std::endl;
  return 1;
  #endif

  #ifndef PASSWORD
  std::cout << "Please define PASSWORD" << std::endl;
  return 1;
  #endif

  maintainConnection(USERNAME, PASSWORD);
  return 0;
}