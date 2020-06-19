#include "Flow.h"


Flow::Flow(int ID) {
    this->ID = ID;
}

Flow::~Flow() {

}

void Flow::nextHop() {
    next_route = route[hop_count++];
}

void Flow::restartRoute() {
    next_route = 0;
    hop_count = 0;
}
