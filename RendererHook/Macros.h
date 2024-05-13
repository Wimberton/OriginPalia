#pragma once

#define PRINT_DEBUG(a, ...) do {FILE *t = fopen("DEBUG_LOG.txt", "a"); fprintf(t, "%u " a, GetCurrentThreadId(), __VA_ARGS__); fclose(t); WSASetLastError(0);} while (0)
