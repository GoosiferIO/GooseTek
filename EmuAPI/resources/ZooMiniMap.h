#ifndef ZOOMINIMAP_H
#define ZOOMINIMAP_H

#include "EmuBase.h"
#include <string>
#include <iostream>
#include "lua.h"

class ZooMiniMap : public EmuBase
{
    public:
        ZooMiniMap();
        ~ZooMiniMap();
    
        lua_State* L;
        void SetMiniMapCursor(int, int);
        void SetMiniMapCursor(std::string);
        void RestoreMiniMapCursor();
        int GetMiniMapCursorX();
        int GetMiniMapCursorY();
    private:
        bool dflt_set;
        int min_map_x;
        int min_map_y;
};

#endif