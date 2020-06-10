#if defined(USE_DLL_MAIN_WRAPPER) && defined(DllMain) && defined(InitDMSfx)    \
    && defined(FiniDMSfx)

extern "C" BOOL APIENTRY DllMain(HMODULE, DWORD, LPVOID) __attribute__((used));

void InitDMSfx() __attribute__((constructor));
void InitDMSfx() { _wrapper_init(&(DllMain)); }

void FiniDMSfx() __attribute__((destructor));
void FiniDMSfx() { _wrapper_fini(&(DllMain)); }

#endif
