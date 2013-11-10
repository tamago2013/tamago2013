//============================================================================
// Name        : main.cpp
// Author      : Shun Kawamoto (KAW)
// Version     : 0.10
// Copyright   : Your copyright notice
// Description : Brief program to check GUIDs of IEEE 1394b cameras.
//============================================================================

#include <iostream>
#include <dc1394/dc1394.h>

#define LEFT_CAM 49712223532624764
#define RIGHT_CAM 49712223532624767

using namespace std;

void identify_cam(uint64_t val){
    if(val == LEFT_CAM){
        cout << "This is KAW's Left Camera." << endl;
    }else if(val == RIGHT_CAM){
        cout << "This is KAW's Right Camera." << endl;
    }else{
        //cout << "This does not belong to KAW." << endl;
    }

}

int main(void)
{
    dc1394_t * dc;
    dc1394error_t err;
    dc1394camera_list_t * list;

    //dc1394の関数を使う際には必須
    dc  = dc1394_new();
    if(!dc){
        cout << "dc1394_new error" << endl;
        return -1;
    }

    err = dc1394_camera_enumerate(dc, &list);
    DC1394_ERR_RTN(err, "Failed to enumerate cameras");

    if(list->num == 0){
        cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << endl;
        cout << "No cameras found" << endl;
        cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << endl;
    }else if(list->num >= 1){
        cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << endl;
        cout << "# of cameras : " << list->num << endl;
        cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << endl;

        for(unsigned i = 0; i < list->num; i++){
            cout << "GUID of camera[" << i << "] : " << /*hex <<*/  list->ids[i].guid  << dec << endl;
            identify_cam(list->ids[i].guid);
            cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << endl;
        }
    }

    dc1394_camera_free_list(list);

    cout << "End Successfully." << endl;

    return 0;
}
