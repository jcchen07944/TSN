#include "Flow.h"


Flow::Flow(int ID) {
    this->ID = ID;
}

Flow::~Flow() {

}

void Flow::nextHop() {
    next_route = route[_next_route_index++];
}

void Flow::restartRoute() {
    next_route = 0;
    _next_route_index = 0;
}
