//
// Created by blow_job on 8/21/24.
//

#ifndef PGPROXYSERVER_OBSERVER_H
#define PGPROXYSERVER_OBSERVER_H

#include <iostream>

struct Notify
{
    std::string notification;
};

using observable_t = Notify;



#endif //PGPROXYSERVER_OBSERVER_H
