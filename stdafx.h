/*
** CPSC489 Game Development Project
** Copyright (c) 2018  Steven Emory <mobysteve@gmail.com>
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/** \file   stdafx.h
 *  \brief  Precompiled header and program requirements.
 *  \author Steven F. Emory
 *  \date   01/29/2018
 */
#ifndef __STDAFX_H
#define __STDAFX_H

//
// Windows Version
//
#define WINVER 0x0602
#define _WIN32_WINNT 0x0602

//
// Windows Headers
//

#define NOMINMAX
#define STRSAFE_NO_DEPRECATE
#include<windows.h>
#include<windowsx.h>
#include<commctrl.h>
#include<shellapi.h>
#ifndef RC_INVOKED
#include<shlobj.h>
#include<tchar.h>
#include<strsafe.h>
#include<shlwapi.h>
#include<wincodec.h>
#include<atlbase.h>
#include<xaudio2.h>
#include<xaudio2fx.h>
#include<x3daudio.h>
#include<xapofx.h>
#include<xinput.h>
#endif
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "xaudio2.lib")
#pragma comment(lib, "xinput9_1_0.lib")

//
// Standard Headers
//

#ifndef RC_INVOKED
#include<iostream>
#include<iomanip>
#include<fstream>
#include<sstream>
#include<memory>
#include<algorithm>
#include<limits>
#include<string>
#include<vector>
#include<array>
#include<deque>
#include<cmath>
#include<unordered_map>
#include<unordered_set>
#include<map>
#include<set>
#include<regex>
#endif

//
// Boost Headers
//

#ifndef RC_INVOKED
#define BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE 
#include<boost/algorithm/string.hpp>
#include<boost/algorithm/string/classification.hpp>
#include<boost/algorithm/string/split.hpp>
#include<boost/algorithm/string/trim.hpp>
#include<boost/algorithm/string/trim_all.hpp>
#include<boost/signals2.hpp>
#endif

//
// DirectX Headers
//

/// \headerfile d3d11.h <d3d11.h>
/// \headerfile dxgi.h <dxgi.h>
/// \headerfile DirectXMath.h <DirectXMath.h>
#ifndef RC_INVOKED
#include<d3d11.h>
#include<dxgi.h>
#include<DirectXMath.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#endif

//
// DATA TYPES
//

// built-in types
typedef char sint08;
typedef unsigned char uint08;
typedef short sint16;
typedef unsigned short uint16;
typedef int sint32;
typedef unsigned int uint32;
typedef long long sint64;
typedef unsigned long long uint64;
typedef float real32;
typedef double real64;

// built-in character types
typedef unsigned char char08;
typedef char16_t char16;
typedef char32_t char32;

// string types
typedef std::basic_string<TCHAR> STDTSTRING;
typedef std::basic_string<CHAR>  STDSTRINGA;
typedef std::basic_string<WCHAR> STDSTRINGW;

// stringstream types
typedef std::basic_stringstream<TCHAR> STDTSTRINGSTREAM;
typedef std::basic_stringstream<CHAR>  STDSTRINGSTREAMA;
typedef std::basic_stringstream<WCHAR> STDSTRINGSTREAMW;

//
// BYTE ORDER FUNCTIONS
//
#pragma region BYTE_ORDER_FUNCTIONS

template<class T>
inline void reverse_byte_order(T* data)
{
 uint08* ptr = reinterpret_cast<uint08*>(data);
 std::reverse(ptr, ptr + sizeof(T)); 
}

template<class T>
inline void reverse_byte_order(T* data, size_t elem)
{
 for(size_t i = 0; i < elem; i++) {
     uint08* ptr = reinterpret_cast<uint08*>(&data[i]);
     std::reverse(ptr, ptr + sizeof(T));
    }
}

#pragma endregion BYTE_ORDER_FUNCTIONS

//
// STRING FUNCTIONS
//
#pragma region STRING_FUNCTIONS

#ifndef RC_INVOKED

/** \class   WideStringHash
 *  \brief   A string hashing function based on Bob Jenkin's One-at-a-Time algorithm.
 *  \details This string hashing algorithm, which I have used for many years now,
 *           has served me quite well. Useful to prevent textures from being loaded
 *           multiple times (filename -> map -> texture data). If I remember right, I
 *           got the code from http://www.burtleburtle.net/bob/hash/doobs.html.
 */
struct WideStringHash {
 size_t operator ()(const std::wstring& str)const;
};

/** \class   WideStringInsensitiveEqual
 *  \brief   A C++ function object for use with std::sort.
 *  \details Yeah, the C++ STL string classes do not have case-insensitive
 *           string sorting so we have to provide function objects (or lambdas)
 *           to do it ourselves.
 */
struct WideStringInsensitiveEqual {
 bool operator ()(const std::wstring& s1, const std::wstring& s2)const {
  return _wcsicmp(s1.c_str(), s2.c_str()) == 0;
 }
};

STDSTRINGA ConvertUTF16ToUTF8(const wchar_t* str);
STDSTRINGW ConvertUTF8ToUTF16(const char* str);

#endif

#pragma endregion STRING_FUNCTIONS

//
// Window Message Macros
//
#pragma region WINDOW_MESSAGE_MACROS

#ifndef RC_INVOKED

#define BEGIN_MESSAGE_HANDLER switch(message) {
#define BEGIN_COMMAND_HANDLER switch(LOWORD(wparam)) {
#define END_MESSAGE_HANDLER }
#define END_COMMAND_HANDLER }

#define WINDOW_PROCEDURE(function) LRESULT CALLBACK function(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
#define WINDOW_MESSAGE(function) LRESULT function(HWND window, WPARAM wparam, LPARAM lparam)
#define WINDOW_MESSAGE_HANDLER(message, function) case(message) : return function(window, wparam, lparam)
#define WINDOW_MESSAGE_DEFAULT default : return DefWindowProc(window, message, wparam, lparam)

#define WINDOW_COMMAND(function) LRESULT function(HWND window, WPARAM wparam, LPARAM lparam)
#define WINDOW_COMMAND_HANDLER(command, function) case(command) : return function(window, wparam, lparam)
#define WINDOW_COMMAND_DEFAULT default : return DefWindowProc(window, WM_COMMAND, wparam, lparam)

#define DIALOG_PROCEDURE(function) INT_PTR CALLBACK function(HWND dialog, UINT message, WPARAM wparam, LPARAM lparam)
#define DIALOG_MESSAGE(function) INT_PTR function(HWND dialog, WPARAM wparam, LPARAM lparam)
#define DIALOG_MESSAGE_HANDLER(message, function) case(message) : return function(dialog, wparam, lparam)
#define DIALOG_MESSAGE_DEFAULT default : return FALSE

#define DIALOG_COMMAND(function) INT_PTR function(HWND dialog, WPARAM wparam, LPARAM lparam)
#define DIALOG_COMMAND_HANDLER(command, function) case(command) : return function(dialog, wparam, lparam)
#define DIALOG_COMMAND_DEFAULT default : return FALSE

#define COMMAND_ENABLER(function) LRESULT function(HWND window, HMENU menu)

#endif

#pragma endregion WINDOW_MESSAGE_MACROS

#endif
