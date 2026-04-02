/**
 * The Warudo web server
 *
 * This file is distributed under the MIT License. See LICENSE for details.
 */

#ifndef WRD_VERSION_H
#define WRD_VERSION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "warudo.h"

// Output the version
WRD_API int wrd_version(warudo *config);

#ifdef __cplusplus
}
#endif

#endif // WRD_VERSION_H
