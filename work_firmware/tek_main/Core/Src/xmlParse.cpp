#include <iostream>
#include <regex>
#include "xmlParse.h"



struct Credentials {
    std::string user;
    std::string password;
    std::string brokerAddress;
};

Credentials parseCredentials(std::string config) {
    std::regex extractWithinQuotes("\"([^\"]*)\"");
    std::regex userRgx("User value=\".+\"");
    std::regex passwordRgx("Password value=\".+\"");
    std::regex brokerRgx("BrokerAddress value=\".+\"");
    std::smatch match;

    std::regex_search(config, match, userRgx);
    std::string userFetched = match[0];
    std::regex_search(userFetched, match, extractWithinQuotes);
    userFetched = match[1];

    std::regex_search(config, match, passwordRgx);
    std::string passwordFetched = match[0];
    std::regex_search(passwordFetched, match, extractWithinQuotes);
    passwordFetched = match[1];

    std::regex_search(config, match, brokerRgx);
    std::string brokerFetched = match[0];
    std::regex_search(brokerFetched, match, extractWithinQuotes);
    brokerFetched = match[1];

    return (struct Credentials){
        userFetched,
        passwordFetched,
        brokerFetched,
    };
}

std::vector<DeviceParameters> parseConfigString(std::string config) {
    std::vector<std::regex> expressions = {
        // std::regex(R"(guid=".*?")"),
        std::regex(R"(update_time_sec=".*?")"),
        std::regex(R"(group=".*?")"),
        std::regex(R"(write_func=".*?")"),
        std::regex(R"(subscribe=".*?")"),
        std::regex(R"(data_type=".*?")"),
        std::regex(R"(alias=".*?")"),
        std::regex(R"(read_func=".*?")")
    };

    std::sregex_iterator end;
    std::vector<std::vector<std::string>> prepared = {
        std::vector<std::string>(),
        std::vector<std::string>(),
        std::vector<std::string>(),
        std::vector<std::string>(),
        std::vector<std::string>(),
        std::vector<std::string>(),
        std::vector<std::string>(),
    };

    std::regex extractWithinQuotes("\"([^\"]*)\"");
    std::smatch match;
    std::vector<DeviceParameters> parsedData;

    for (int i = 0; i < expressions.size(); ++i) {
        std::sregex_iterator iter(config.begin(), config.end(), expressions[i]);
        while(iter != end) {
            for(int j = 0; j < iter->size(); ++j) {
                std::string fetchedMatch = (*iter)[j];
                std::regex_search(fetchedMatch, match, extractWithinQuotes);
                prepared[i].push_back(match[1]);
            }
            ++iter;
        }

    }

    int modemParameterCount = prepared[0].size() - prepared[1].size();

    for (int i = 0; i < prepared[0].size(); ++i) {
        if (i < modemParameterCount) {
            parsedData.push_back((struct DeviceParameters){
                std::stoi(prepared[0][i]),
                "",
                0,
                prepared[3][i] == "true",
                prepared[4][i],
                prepared[5][i],
                0,
            });
        }
        else {
            parsedData.push_back((struct DeviceParameters){
                std::stoi(prepared[0][i]),
                prepared[1][i - modemParameterCount],
                std::stoi(prepared[2][i - modemParameterCount]),
                prepared[3][i] == "true",
                prepared[4][i],
                prepared[5][i],
                std::stoi(prepared[6][i - modemParameterCount]),
            });
        }
    }

    return parsedData;
}


std::map<std::string, std::map<std::string, std::vector<DeviceParameters>>> groupDevices(const std::vector<DeviceParameters>& devices) {
    std::map<std::string, std::map<std::string, std::vector<DeviceParameters>>> groupedDevices;

    for (auto device: devices) {
        groupedDevices[std::to_string(device.updateTime)][device.group].push_back(device);
    }
    return groupedDevices;
}


