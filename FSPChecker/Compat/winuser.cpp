#include <boost/log/trivial.hpp>
#include "winuser.h"

HWND FindWindowA(
  LPCSTR lpClassName,
  LPCSTR lpWindowName
){
    BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                    << __LINE__ << " ##TODO##" ;
    return NULL;
}

HWND FindWindowW(
  LPCWSTR lpClassName,
  LPCWSTR lpWindowName
){
    BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                    << __LINE__ << " ##TODO##" ;
    return NULL;
}

BOOL ShowWindow(
  HWND hWnd,
  int  nCmdShow
){
    BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                    << __LINE__ << " ##TODO##" ;
    return FALSE;

}