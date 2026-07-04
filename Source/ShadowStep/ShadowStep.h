// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

#pragma once

#include "CoreMinimal.h"

// Custom Logging
#include "Logging/StructuredLog.h"
#include "Misc/Paths.h"

#define ULOG_STR(x) #x
#define ULOG_XSTR(x) ULOG_STR(x)

#define ULOG(Format, ...) \
    UE_LOGFMT(LogTemp, Warning, \
              Format " [" __FILE__ ":" ULOG_XSTR(__LINE__) "]" \
              __VA_OPT__(,) __VA_ARGS__)

#define ULOGERROR(Format, ...) \
    UE_LOGFMT(LogTemp, Error, \
              Format " [" __FILE__ ":" ULOG_XSTR(__LINE__) "]" \
              __VA_OPT__(,) __VA_ARGS__)

#define ULOGVERBOSE(Format, ...) \
    UE_LOGFMT(LogTemp, Verbose, \
              Format " [" __FILE__ ":" ULOG_XSTR(__LINE__) "]" \
              __VA_OPT__(,) __VA_ARGS__)