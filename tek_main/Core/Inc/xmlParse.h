/*
 * xmlParse.h
 *
 *  Created on: Nov 28, 2023
 *      Author: 566kb523
 */

#ifndef INC_XMLPARSE_H_
#define INC_XMLPARSE_H_
#include <iostream>
#include <regex>
struct DeviceParameters {
    int updateTime;
    std::string group;
    int writeFunc;
    bool subscribe;
    std::string dataType;
    std::string alias;
    int readFunc;
};

std::vector<DeviceParameters> parseConfigString(std::string config);


#endif /* INC_XMLPARSE_H_ */
