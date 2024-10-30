// AddRoadClass.h
#ifndef ADD_ROAD_CLASS_H
#define ADD_ROAD_CLASS_H

class Road
{
public:
    int id;
    int start;
    int end;
    int length;
    int capacity;
    int currentCars;

    Road(int id = -1, int start = -1, int end = -1, int length = 0, int capacity = 0)
    {
        this->id = id;
        this->start = start;
        this->end = end;
        this->length = length;
        this->capacity = capacity;
        this->currentCars = 0;
    }
};

#endif // ADD_ROAD_CLASS_H
