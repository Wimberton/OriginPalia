#include "hook_mouse.hpp"
#include "console/console.hpp"
#include <Windows.h>
#include <memory>
#include "MinHook.h"
#include "menu/menu.hpp"

static std::add_pointer_t<int WINAPI( BOOL bShow )> oShowCursor;
int WINAPI hkShowCursor( BOOL bShow ) {
    if ( Menu::bShowMenu && !Mouse::enableApis  ) {
        auto result = oShowCursor( true );
        return bShow ? result : -1;
    } else {
        return oShowCursor( bShow );
    }
}

static std::add_pointer_t<BOOL WINAPI(LPPOINT lpPoint)> oGetCursorPos;
BOOL WINAPI hkGetCursorPos(LPPOINT lpPoint) {
    if ( Menu::bShowMenu && !Mouse::enableApis ) {
        lpPoint->x = 0;
        lpPoint->y = 0;
        return true;
    } else {
        return oGetCursorPos(lpPoint);
    }
}

static std::add_pointer_t<BOOL WINAPI( int x, int y )> oSetCursorPos;
BOOL WINAPI hkSetCursorPos( int x, int y ) {
    if ( Menu::bShowMenu && !Mouse::enableApis  ) {
        return true;
    } else {
        return oSetCursorPos( x, y );
    }
}

static std::add_pointer_t<HCURSOR  WINAPI( HCURSOR  hCursor )> oSetCursor;
HCURSOR WINAPI hkSetCursor( HCURSOR  hCursor ) {
    if ( Menu::bShowMenu && !Mouse::enableApis ) {
        return hCursor;
    } else {
        return oSetCursor( hCursor );
    }
}


namespace Mouse {

    void Hook() {

        static MH_STATUS statusShowCursor = MH_CreateHook(
            reinterpret_cast<void**>( ShowCursor ), &hkShowCursor,
            reinterpret_cast<void**>( &oShowCursor )
        );
        static MH_STATUS statusGetCursorPos = MH_CreateHook(
            reinterpret_cast<void**>( GetCursorPos ), &hkGetCursorPos,
            reinterpret_cast<void**>( &oGetCursorPos ) 
        );
        static MH_STATUS statusSetCursorPos = MH_CreateHook(
            reinterpret_cast<void**>( SetCursorPos ), &hkSetCursorPos,
            reinterpret_cast<void**>( &oSetCursorPos ) 
        );
        static MH_STATUS statusSetCursor = MH_CreateHook(
            reinterpret_cast<void**>( SetCursor ), &hkSetCursor,
            reinterpret_cast<void**>( &oSetCursor ) 
        );

        MH_EnableHook( ShowCursor );
        MH_EnableHook( GetCursorPos );
        MH_EnableHook( SetCursorPos );
        MH_EnableHook( SetCursor );

        LOG( "[+] Original ShowCursor: %llX\n", reinterpret_cast<UINT_PTR>( oShowCursor ) );
        LOG( "[+] Original GetCursorPos: %llX\n", reinterpret_cast<UINT_PTR>( oGetCursorPos ) );
        LOG( "[+] Original SetCursorPos: %llX\n", reinterpret_cast<UINT_PTR>( oSetCursorPos ) );
        LOG( "[+] Original SetCursor: %llX\n", reinterpret_cast<UINT_PTR>( oSetCursor ) );

    }

    void Unhook() {

    }

}
