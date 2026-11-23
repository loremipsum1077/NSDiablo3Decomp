#include "DataArraysTemplated.h"














DataArrayList* gptTrackedDAList;





void DataArrayTrackerInit() {


    gptTrackedDAList = new DataArrayList;
    gptTrackedDAList->SetBlockCount_Tracked(1024);
}




void DataArrayTrackerDispose() {


    delete gptTrackedDAList;
    gptTrackedDAList = NULL;
}
