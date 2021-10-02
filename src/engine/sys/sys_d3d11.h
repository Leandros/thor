/* My own D3D11 declerations. */
#ifndef SYS_D3D11_H
#define SYS_D3D11_H

#include <libc/stddef.h> /* size_t */
#include <sys/sys_platform.h>

#if IS_WIN32 || IS_WIN64

/* mccp does not like files which are larger than 32768 lines. */
#if !defined(MCPP_RUNNING)

/* Stupid MicroSoft. Required for some reason. */
#define _USING_V110_SDK71_ 1

/* Also grab the C interfaces ... */
#ifndef CINTERFACE
#define CINTERFACE
#endif

/* .. and the OOP interface macros. */
#ifndef COBJMACROS
#define COBJMACROS
#endif

/*
 * Windows Version requirements: Vista
 * See here for more information regarding this define:
 * https://msdn.microsoft.com/en-us/library/aa383745(VS.85).aspx
 */
#define _WIN32_WINNT 0x0600
#include <WinSDKVer.h>

#include <windows.h>
#include <dxgi.h>

#include <d3d11.h>

#endif /* !defined(MCPP_RUNNING) */
#endif /* IS_WIN32 || IS_WIN64 */
#endif /* SYS_D3D11_H */

