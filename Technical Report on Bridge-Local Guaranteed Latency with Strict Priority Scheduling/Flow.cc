#include "Flow.h"


Flow::Flow(int ID) {
    this->ID = ID;
}

Flow::~Flow() {

}

bool Flow::nextHop() {
    if(_next_route_index >= max_hop)
        return false;

    next_route = route[_next_route_index];
    _next_route_index++;
    return true;
}

void Flow::restartRoute() {
    next_route = 0;
    _next_route_index = 0;
}
