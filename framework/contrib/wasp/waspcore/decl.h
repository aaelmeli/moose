#ifndef WASP_WASPCORE_DECL_HH_
#define WASP_WASPCORE_DECL_HH_
#ifdef BUILD_SHARED
#if defined _WIN32 || defined __CYGWIN__
#define WASP_PUBLIC __declspec(dllexport)
#define WASP_LOCAL
#else
#define WASP_PUBLIC
#define WASP_LOCAL __declspec(dllimport)
#endif
#else
#define WASP_PUBLIC
#define WASP_LOCAL
#endif
#endif /* WASP_WASPCORE_DECL_HH_*/
