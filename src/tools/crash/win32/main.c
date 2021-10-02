#include <libc/string.h>
#include <libc/stdio.h>
#include <sys/sys_types.h>
#include <sys/sys_windows.h>
#include <re_engine.h>
#include <entry.h>

/* Constants */
#define ID_PID      "Local\\pid"
#define ID_EVENT    "Local\\event"

#define PROCESS_ACCESS \
    (PROCESS_QUERY_INFORMATION |\
     PROCESS_SUSPEND_RESUME |\
     PROCESS_TERMINATE |\
     PROCESS_VM_READ)
#define THREAD_ACCESS \
    (THREAD_GET_CONTEXT |\
     THREAD_QUERY_INFORMATION |\
     THREAD_SUSPEND_RESUME)

/* Macros: */
#define KIB(x) ((x) / 1024)
#define MIB(x) ((x) / 1024 / 1024)

struct btargs {
    PCONTEXT ctx;
    HANDLE proc;
    HANDLE thread;
    int hassymbols;
};

struct dbgi {
    int crash;
    DWORD pid, tid;
    HANDLE proc, thread;
    PSYMBOL_INFO sym;
    EXCEPTION_RECORD er;
    CONTEXT cr;

    /* extra info */
    void *exe, *dll;
    int isassert, line, hassymbols;
    char cmdline[512], desc[512];
    char file[256], func[256], condstr[256];
};

/* Entry Point */
int
entry(int argc, char **argv);

/* Output */
static int
write_minidump(HANDLE file, struct dbgi *info);
static int
write_report(HANDLE file, struct dbgi *info);
static int
print_registers(HANDLE file, char *buf, size_t bufsiz, PCONTEXT ctx);
static int
print_backtrace(HANDLE file, char *buf, size_t bufsiz, struct btargs *args);
static int
print_threads(HANDLE file, char *buf, size_t bufsiz, struct dbgi *info);
static int
print_gfxinfo(HANDLE file, char *buf, size_t bufsiz);
static int
print_sysinfo(HANDLE file, char *buf, size_t bufsiz);
static int
print_cpuinfo(HANDLE file, char *buf, size_t bufsiz);
static int
print_meminfo(HANDLE file, char *buf, size_t bufsiz);

/* Misc */
static int
current_directory(char *buf, size_t size);
static char *
exc_desc(DWORD code);
static int
isdir(char const *path);
static int
mkdir(char const *path);
static char *
current_symbol_path(HANDLE proc);
static wchar_t *
current_symbol_path_w(HANDLE proc);

/* Strings */
#define STRSIZE(x) (sizeof((x))-1)
#define NEWLINE         "\r\n"
#define NEWLINE2        "\r\n\r\n"
#define STATUS          "Generating crash report ..."
#define ERR_MINIDUMP    "Error: Creating report"
#define ERR_REPORT      "Error: Creating minidump"

#define BIGSEPARATOR \
"================================================================================\r\n\r\n"
#define SMALLSEPARATOR \
"--------------------------------------------------------------------------------\r\n\r\n"
#define TINYSEPARATOR \
"----------------------------------------\r\n"

ENTRY_POINT(entry)

int
entry(int argc, char **argv)
{
    char *ptr, path[300], buf[512];
    SYSTEMTIME time;
    HANDLE map, file, event;
    struct dbgi info;

    /* We need the PID as the first argument! */
    if (argc != 2)
        return 0;

    map = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, ID_PID);
    if (map == NULL)
        goto e0;

    ptr = (char*)MapViewOfFile(map, FILE_MAP_ALL_ACCESS, 0, 0, 16384);
    if (buf == NULL)
        goto e1;

    event = OpenEventA(EVENT_ALL_ACCESS, FALSE, ID_EVENT);
    if (event == NULL)
        goto e2;

    /* == WAIT == */
    WaitForSingleObject(event, INFINITE);
    memcpy(&info, ptr, sizeof(struct dbgi));
    if (info.crash == 0) {
        CloseHandle(event);
        UnmapViewOfFile(ptr);
        CloseHandle(map);
        return 0;
    }

    puts(STATUS);
    info.proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, info.pid);
    if (info.proc == NULL)
        goto e3;

    if (!DebugActiveProcess(info.pid))
        goto e4;

    GetSystemTime(&time);
    if (current_directory(path, 300))
        goto e5;

    /* Check if the errors directory exists, if it doesn't, create it first. */
    snprintf(buf, 512, "%s\\..\\errors", path);
    if (!isdir(buf)) {
        if (mkdir(buf))
            goto e5;
    }

    /* == Minidump == */
    snprintf(buf, 512, "%s\\..\\errors\\minidump-%d-%02d-%02dT%02d-%02d-%02dZ.dmp",
            path, time.wYear, time.wMonth, time.wDay,
            time.wHour, time.wMinute, time.wSecond);
    file = CreateFileA(
            buf,
            GENERIC_WRITE,
            0,
            NULL,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL);
    if (file == INVALID_HANDLE_VALUE)
        goto report;

    if (write_minidump(file, &info)) {
        puts(ERR_MINIDUMP);
    }

    CloseHandle(file);

    /* == Report == */
report:
    snprintf(buf, 512, "%s\\..\\errors\\crash-%d-%02d-%02dT%02d-%02d-%02dZ.txt",
            path, time.wYear, time.wMonth, time.wDay,
            time.wHour, time.wMinute, time.wSecond);
    file = CreateFileA(
            buf,
            GENERIC_WRITE,
            0,
            NULL,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL);
    if (file == INVALID_HANDLE_VALUE)
        goto e5;

    if (write_report(file, &info)) {
        puts(ERR_REPORT);
    }

    CloseHandle(file);
    DebugActiveProcessStop(info.pid);
    /*CloseHandle(info.proc);*/
    CloseHandle(event);
    UnmapViewOfFile(ptr);
    CloseHandle(map);
    TerminateProcess(info.proc, 1);
    return 0;

e5: DebugActiveProcessStop(info.pid);
e4: /*CloseHandle(info.proc);*/
e3: CloseHandle(event);
    TerminateProcess(info.proc, 1);
e2: UnmapViewOfFile(ptr);
e1: CloseHandle(map);
e0: printf("Error: Creating crash report\n");
    return 1;
}

static int
write_report(HANDLE file, struct dbgi *info)
{
    size_t len;
    char buf[1024];
    ULONG64 cycles;
    SYSTEMTIME time, time2;
    FILETIME cftime, eftime, kftime, uftime, sftime;
    PEXCEPTION_RECORD ex = &info->er;
    PCONTEXT ctx = &info->cr;
    PROCESS_MEMORY_COUNTERS mem;
    struct btargs bt;
    int errorcode = 0;
    char *errordesc = exc_desc(ex->ExceptionCode);
    wchar_t *sympathw;

    sympathw = current_symbol_path_w(info->proc);
    SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
    info->hassymbols = SymInitializeW(info->proc, sympathw, TRUE);
    GetSystemTime(&time);
    GetSystemTimeAsFileTime(&sftime);
    GetProcessTimes(info->proc, &cftime, &eftime, &kftime, &uftime);
    QueryProcessCycleTime(info->proc, &cycles);

    /* Initialize all arguments: */
    bt.ctx = ctx;
    bt.proc = info->proc;
    bt.hassymbols = info->hassymbols;
    bt.thread = OpenThread(THREAD_ACCESS, FALSE, info->tid);
    if (bt.thread == NULL)
        return 1;

    len = snprintf(buf, 1024, "%s %s v%d.%d.%d.%d (rev. %s) on %s (%s)\r\n\r\n",
            BUILD_NAME, BUILD_TYPE,
            BUILD_VER_MAJOR, BUILD_VER_MINOR, BUILD_VER_PATCH, BUILD_VER_BUILD,
            BUILD_REVISION, PLATFORM_STRING, ARCH_STRING);
    WriteFile(file, buf, (DWORD)len, NULL, NULL);

    len = snprintf(buf, 1024, "%-16s %s\r\n", "Command", info->cmdline);
    WriteFile(file, buf, (DWORD)len, NULL, NULL);

    len = snprintf(buf, 1024, "%-16s %d-%02d-%02dT%02d-%02d-%02dZ\r\n\r\n",
            "Date", time.wYear, time.wMonth, time.wDay,
            time.wHour, time.wMinute, time.wSecond);
    WriteFile(file, buf, (DWORD)len, NULL, NULL);

    WriteFile(file, BIGSEPARATOR, STRSIZE(BIGSEPARATOR), NULL, NULL);

    len = snprintf(buf, 1024, "ERROR #%d: %s (%016llX)\r\n\r\n",
            errorcode, errordesc, (uintptr_t)ex->ExceptionAddress);
    WriteFile(file, buf, (DWORD)len, NULL, NULL);

    if (ex->ExceptionCode = EXCEPTION_ASSERTION && ex->NumberParameters == 5) {
        len = snprintf(buf, 1024,
                "File: %s\r\nFunc: %s\r\nLine: %d\r\nExpr: %s\r\nHint: %s\r\n",
                info->file,
                info->func,
                info->line,
                info->condstr,
                info->desc);
        WriteFile(file, buf, (DWORD)len, NULL, NULL);
    }

    WriteFile(file, NEWLINE, STRSIZE(NEWLINE), NULL, NULL);
    print_registers(file, buf, 1024, &info->cr);
    print_backtrace(file, buf, 1024, &bt);

    WriteFile(file, NEWLINE2, STRSIZE(NEWLINE2), NULL, NULL);
    WriteFile(file, "Statistics\r\n", STRSIZE("Statistics\r\n"), NULL, NULL);
    WriteFile(file, TINYSEPARATOR, STRSIZE(TINYSEPARATOR), NULL, NULL);

    FileTimeToSystemTime(&cftime, &time2);
    len = snprintf(buf, 1024, "%-24s %d-%02d-%02dT%02d-%02d-%02dZ\r\n",
            "Started", time2.wYear, time2.wMonth, time2.wDay,
            time2.wHour, time2.wMinute, time2.wSecond);
    WriteFile(file, buf, (DWORD)len, NULL, NULL);

    len = snprintf(buf, 1024, "%-24s %016llX\r\n",
            "EXE Address", (uintptr_t)info->exe);
    WriteFile(file, buf, (DWORD)len, NULL, NULL);

    len = snprintf(buf, 1024, "%-24s %016llX\r\n",
            "DLL Address", (uintptr_t)info->dll);
    WriteFile(file, buf, (DWORD)len, NULL, NULL);

    GetProcessMemoryInfo(info->proc, &mem, sizeof(PROCESS_MEMORY_COUNTERS));
    len = snprintf(buf, 1024, "%-24s %ld\r\n",
        "Page Faults", mem.PageFaultCount);
    WriteFile(file, buf, (DWORD)len, NULL, NULL);
    len = snprintf(buf, 1024, "%-24s %lld KiB\r\n%-24s %lld KiB\r\n",
        "Phys. Mem Used", KIB(mem.WorkingSetSize),
        "Phys. Mem Peak", KIB(mem.PeakWorkingSetSize));
    WriteFile(file, buf, (DWORD)len, NULL, NULL);
    len = snprintf(buf, 1024, "%-24s %lld KiB\r\n%-24s %lld KiB\r\n",
        "Virt. Mem Used", KIB(mem.PagefileUsage),
        "Virt. Mem Peak", KIB(mem.PeakPagefileUsage));
    WriteFile(file, buf, (DWORD)len, NULL, NULL);


    WriteFile(file, NEWLINE2, STRSIZE(NEWLINE2), NULL, NULL);
    WriteFile(file, "Engine Diagnostics\r\n",
            STRSIZE("Engine Diagnostics\r\n"), NULL, NULL);
    WriteFile(file, BIGSEPARATOR, STRSIZE(BIGSEPARATOR), NULL, NULL);

    /* TODO: Installation diagnostics */
    len = snprintf(buf, 1024, "%-24s %d.%d.%d.%d\r\n", "Version",
            BUILD_VER_MAJOR, BUILD_VER_MINOR, BUILD_VER_PATCH, BUILD_VER_BUILD);
    WriteFile(file, buf, (DWORD)len, NULL, NULL);
    len = snprintf(buf, 1024, "%-24s %s\r\n", "Revision", BUILD_REVISION);
    WriteFile(file, buf, (DWORD)len, NULL, NULL);
#if USING(ENGINE_DEBUG)
    len = snprintf(buf, 1024, "%-24s %s\r\n", "Debug", "On");
#else
    len = snprintf(buf, 1024, "%-24s %s\r\n", "Debug", "Off");
#endif
    WriteFile(file, buf, (DWORD)len, NULL, NULL);

    WriteFile(file, NEWLINE2, STRSIZE(NEWLINE2), NULL, NULL);
    WriteFile(file, "System Diagnostics\r\n",
            STRSIZE("System Diagnostics\r\n"), NULL, NULL);
    WriteFile(file, BIGSEPARATOR, STRSIZE(BIGSEPARATOR), NULL, NULL);

    WriteFile(file, TINYSEPARATOR, STRSIZE(TINYSEPARATOR), NULL, NULL);
    WriteFile(file, "    System\r\n",
            STRSIZE("    System\r\n"), NULL, NULL);
    WriteFile(file, TINYSEPARATOR, STRSIZE(TINYSEPARATOR), NULL, NULL);
    WriteFile(file, NEWLINE, STRSIZE(NEWLINE), NULL, NULL);
    print_sysinfo(file, buf, 1024);

    WriteFile(file, NEWLINE2, STRSIZE(NEWLINE2), NULL, NULL);
    WriteFile(file, TINYSEPARATOR, STRSIZE(TINYSEPARATOR), NULL, NULL);
    WriteFile(file, "    Processor\r\n",
            STRSIZE("    Processor\r\n"), NULL, NULL);
    WriteFile(file, TINYSEPARATOR, STRSIZE(TINYSEPARATOR), NULL, NULL);
    WriteFile(file, NEWLINE, STRSIZE(NEWLINE), NULL, NULL);
    print_cpuinfo(file, buf, 1024);

    WriteFile(file, NEWLINE2, STRSIZE(NEWLINE2), NULL, NULL);
    WriteFile(file, TINYSEPARATOR, STRSIZE(TINYSEPARATOR), NULL, NULL);
    WriteFile(file, "    Memory\r\n",
            STRSIZE("    Memory\r\n"), NULL, NULL);
    WriteFile(file, TINYSEPARATOR, STRSIZE(TINYSEPARATOR), NULL, NULL);
    WriteFile(file, NEWLINE, STRSIZE(NEWLINE), NULL, NULL);
    print_meminfo(file, buf, 1024);

    /* TODO: D3D Diagnostics */
    WriteFile(file, NEWLINE2, STRSIZE(NEWLINE2), NULL, NULL);
    WriteFile(file, TINYSEPARATOR, STRSIZE(TINYSEPARATOR), NULL, NULL);
    WriteFile(file, "    Graphics\r\n",
            STRSIZE("    Graphics\r\n"), NULL, NULL);
    WriteFile(file, TINYSEPARATOR, STRSIZE(TINYSEPARATOR), NULL, NULL);
    WriteFile(file, NEWLINE, STRSIZE(NEWLINE), NULL, NULL);
    print_gfxinfo(file, buf, 1024);

    WriteFile(file, NEWLINE2, STRSIZE(NEWLINE2), NULL, NULL);
    WriteFile(file, TINYSEPARATOR, STRSIZE(TINYSEPARATOR), NULL, NULL);
    WriteFile(file, "    Threads\r\n",
            STRSIZE("    Threads\r\n"), NULL, NULL);
    WriteFile(file, TINYSEPARATOR, STRSIZE(TINYSEPARATOR), NULL, NULL);
    print_threads(file, buf, 1024, info);

    WriteFile(file, NEWLINE2, STRSIZE(NEWLINE2), NULL, NULL);
    return 0;
}

static int
write_minidump(HANDLE file, struct dbgi *info)
{
    BOOL b;
    EXCEPTION_POINTERS ep;
    MINIDUMP_EXCEPTION_INFORMATION exinfo = {0};
    MINIDUMP_TYPE type = MiniDumpNormal;

    ep.ExceptionRecord = &info->er;
    ep.ContextRecord = &info->cr;
    exinfo.ThreadId = info->tid;
    exinfo.ExceptionPointers = &ep;
    exinfo.ClientPointers = TRUE;

#if USING(ENGINE_DEBUG)
        type = MiniDumpWithFullMemory
            | MiniDumpWithFullMemoryInfo
            | MiniDumpWithThreadInfo;
#endif

    b = MiniDumpWriteDump(
            info->proc,
            info->pid,
            file,
            type,
            NULL,
            NULL,
            NULL);
    if (!b) {
        printf("GetLastError: %d\n", HRESULT_CODE(GetLastError()));
        return HRESULT_CODE(GetLastError());
    }

    return !b;
}

static int
print_registers(HANDLE file, char *buf, size_t bufsiz, PCONTEXT ctx)
{
    size_t len;

    len = snprintf(buf, bufsiz,
            "RAX: %016llX\tRBX: %016llX\tRCX: %016llX\tRDX: %016llX\r\n",
            ctx->Rax, ctx->Rbx, ctx->Rcx, ctx->Rdx);
    WriteFile(file, buf, (DWORD)len, NULL, NULL);

    len = snprintf(buf, bufsiz,
            "RDI: %016llX\tRSI: %016llX\tRBP: %016llX\tRSP: %016llX\r\n",
            ctx->Rdi, ctx->Rsi, ctx->Rbp, ctx->Rsp);
    WriteFile(file, buf, (DWORD)len, NULL, NULL);

    len = snprintf(buf, bufsiz,
            "R8 : %016llX\tR9 : %016llX\tR10: %016llX\tR11: %016llX\r\n",
            ctx->R8, ctx->R9, ctx->R10, ctx->R11);
    WriteFile(file, buf, (DWORD)len, NULL, NULL);

    len = snprintf(buf, bufsiz,
            "R12: %016llX\tR13: %016llX\tR14: %016llX\tR15: %016llX\r\n",
            ctx->R12, ctx->R13, ctx->R14, ctx->R15);
    WriteFile(file, buf, (DWORD)len, NULL, NULL);

    len = snprintf(buf, bufsiz,
            "RIP: %016llX\tFlags: %08X\r\n",
            ctx->Rip, ctx->EFlags);
    WriteFile(file, buf, (DWORD)len, NULL, NULL);

    len = snprintf(buf, bufsiz,
            "CS: %04X DS: %04X ES: %04X FS: %04X GS: %04X SS: %04X\r\n",
            ctx->SegCs, ctx->SegDs, ctx->SegEs, ctx->SegFs, ctx->SegGs, ctx->SegSs);
    WriteFile(file, buf, (DWORD)len, NULL, NULL);

    WriteFile(file, NEWLINE, STRSIZE(NEWLINE), NULL, NULL);
    return 0;
}

static int
print_backtrace(HANDLE file, char *buf, size_t bufsiz, struct btargs *args)
{
    BOOL b;
    ULONG i;
    usize len;
    STACKFRAME64 frame = {0};
    char tmp[513];
    PSYMBOL_INFO sym;
    CONTEXT cr = *args->ctx;

    sym = (PSYMBOL_INFO)tmp;
    sym->SizeOfStruct = sizeof(SYMBOL_INFO);
    sym->MaxNameLen = 512 - sizeof(SYMBOL_INFO);

    frame.AddrPC.Offset = cr.Rip;
    frame.AddrPC.Mode = AddrModeFlat;
    frame.AddrStack.Offset = cr.Rsp;
    frame.AddrStack.Mode = AddrModeFlat;
    frame.AddrFrame.Offset = cr.Rbp;
    frame.AddrFrame.Mode = AddrModeFlat;

    for (i = 0; ; ++i) {
        b = StackWalk64(
                IMAGE_FILE_MACHINE_AMD64,
                args->proc,
                args->thread,
                &frame,
                &cr,
                NULL,
                SymFunctionTableAccess64,
                SymGetModuleBase64,
                NULL);
        if (!b || frame.AddrPC.Offset == 0)
            break;

        if (!args->hassymbols)
            goto nosym;

        b = SymFromAddr(
                args->proc,
                (DWORD64)frame.AddrPC.Offset,
                NULL,
                sym);

        if (b) {
            len = snprintf(buf, bufsiz, "#%02d %016llX (%s)\r\n",
                    i, frame.AddrPC.Offset, (char*)sym->Name);
            WriteFile(file, buf, (DWORD)len, NULL, NULL);
            continue;
        }

nosym:
        len = snprintf(buf, bufsiz, "#%02d %016llX\r\n",
                i, frame.AddrPC.Offset);
        WriteFile(file, buf, (DWORD)len, NULL, NULL);

    }

    return 0;
}

static int
print_threads(HANDLE file, char *buf, size_t bufsiz, struct dbgi *info)
{
    size_t len;
    HANDLE snap;
    THREADENTRY32 te32;
    CONTEXT ctx;
    struct btargs bt;
    bt.ctx = &ctx;
    bt.proc = info->proc;
    bt.hassymbols = info->hassymbols;

    /* Snaps ALL threads in the system, the processID (0) will be ignored. */
    snap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snap == INVALID_HANDLE_VALUE)
        return 1;

    te32.dwSize = sizeof(THREADENTRY32);
    if (!Thread32First(snap, &te32))
        return 1;

    do {
        if (te32.th32OwnerProcessID == info->pid) {
            if (te32.th32ThreadID == info->tid)
                continue;

            bt.thread = OpenThread(THREAD_ALL_ACCESS, FALSE, te32.th32ThreadID);
            if (bt.thread == NULL)
                continue;

            if (SuspendThread(bt.thread) == (DWORD)-1)
                goto next;

            memset(bt.ctx, 0x0, sizeof(CONTEXT));
            bt.ctx->ContextFlags = CONTEXT_FULL;
            if (!GetThreadContext(bt.thread, bt.ctx))
                goto next;

            len = snprintf(buf, bufsiz, "\r\n==== Thread ID: 0x%08X ====\r\n",
                    te32.th32ThreadID);
            WriteFile(file, buf, (DWORD)len, NULL, NULL);

            print_backtrace(file, buf, bufsiz, &bt);

next:
            CloseHandle(bt.thread);
        }
    } while (Thread32Next(snap, &te32));

    CloseHandle(snap);
    return 0;
}

#define STEPPING(x)     ((x) & 0xf)
#define MODEL(x)        (((x) >> 4) & 0xf)
#define FAMILY(x)       (((x) >> 8) & 0xf)
#define TYPE(x)         (((x) >>12) & 0xf)
#define EXTMODEL(x)     (((x) >>16) & 0xf)
#define EXTFAMILY(x)    (((x) >>20) & 0xf)
#define IS_BIT_SET(x,y) (((x) >> (y)) & 0x1)
#define IF_BIT_SET(a,b,c,d) (IS_BIT_SET((a),(b))?(c):(d))
static DWORD
CountSetBits(ULONG_PTR bitMask)
{
    DWORD LSHIFT = sizeof(ULONG_PTR)*8 - 1;
    DWORD bitSetCount = 0;
    ULONG_PTR bitTest = (ULONG_PTR)1 << LSHIFT;
    DWORD i;

    for (i = 0; i <= LSHIFT; ++i)
    {
        bitSetCount += ((bitMask & bitTest)?1:0);
        bitTest/=2;
    }

    return bitSetCount;
}

static int
print_sysinfo(HANDLE file, char *buf, size_t bufsiz)
{
    size_t len;
    DWORD size;
    WORD major, minor, srvpack;
    CHAR name[MAX_COMPUTERNAME_LENGTH+1];

    size = MAX_COMPUTERNAME_LENGTH;
    GetComputerNameA(name, &size);
    len = snprintf(buf, bufsiz, "%-16s %s\r\n", "Name", name);
    WriteFile(file, buf, (DWORD)len, NULL, NULL);

    GetWindowsVersionCUSTOM(&major, &minor, &srvpack);
    if (srvpack > 0) {
        len = snprintf(buf, bufsiz, "%-16s %d.%d (Service Pack %d)\r\n",
                "Windows", major, minor, srvpack);
        WriteFile(file, buf, (DWORD)len, NULL, NULL);
    } else {
        len = snprintf(buf, bufsiz, "%-16s %d.%d\r\n", "Windows", major, minor);
        WriteFile(file, buf, (DWORD)len, NULL, NULL);
    }

    return 0;
}

static int
print_cpuinfo(HANDLE file, char *buf, size_t bufsiz)
{
    uint a, b, c, d, maxleaf, done;
    uchar cpuinfo[16];
    uchar cpubrand[64];
    size_t len;
    DWORD size;
    SYSTEM_INFO sysinfo;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION pinfo = NULL;
    PCACHE_DESCRIPTOR cache;
    DWORD offs = 0, cores = 0, threads = 0,
          l1 = 0, l2 = 0, l3 = 0, pkg = 0;
    DWORD l1s = 0, l2s = 0, l3s = 0;
    WORD l1ls = 0, l2ls = 0, l3ls = 0;

    __cpuid((int*)cpuinfo, 0x80000000);
    maxleaf = *(int*)cpuinfo;
    if (maxleaf > 0x80000005) {
        __cpuid((int*)cpuinfo, 0x80000002);
        memcpy(cpubrand, cpuinfo, 16);
        __cpuid((int*)cpuinfo, 0x80000003);
        memcpy(cpubrand + 16, cpuinfo, 16);
        __cpuid((int*)cpuinfo, 0x80000004);
        memcpy(cpubrand + 32, cpuinfo, 16);

        len = snprintf(buf, bufsiz, "%-16s %s\r\n", "CPU", cpubrand);
        WriteFile(file, buf, (DWORD)len, NULL, NULL);
    }

    __cpuid((int*)cpuinfo, 0x0);
    maxleaf = *(int*)cpuinfo;
    memcpy(cpubrand, cpuinfo+4, 4);
    memcpy(cpubrand+4, cpuinfo+12, 4);
    memcpy(cpubrand+8, cpuinfo+8, 4);
    len = snprintf(buf, bufsiz, "%-16s %.12s\r\n", "Vendor", cpubrand);
    WriteFile(file, buf, (DWORD)len, NULL, NULL);

    __cpuid((int*)cpuinfo, 0x1);
    a = *(int*)cpuinfo;
    len = snprintf(buf, bufsiz,
    "%-16s %X %-16s %X %-16s %X\r\n%-16s %X %-16s %X %-16s %X\r\n",
    "Stepping", STEPPING(a),
    "Revision", TYPE(a),
    "Model", MODEL(a),
    "Ext. Model", EXTMODEL(a),
    "Family", FAMILY(a),
    "Ext. Family", EXTFAMILY(a));
    WriteFile(file, buf, (DWORD)len, NULL, NULL);

    d = *(int*)(cpuinfo+12);
    c = *(int*)(cpuinfo+8);
    len = snprintf(buf, bufsiz,
            "%-16s %s%s%s%s%s%s%s%s%s%s\r\n",
            "Instructions",
            IF_BIT_SET(d, 23, "MMX ", ""),
            IF_BIT_SET(d, 25, "SSE ", ""),
            IF_BIT_SET(d, 26, "SSE2 ", ""),
            (c & 0x1) ? "SSE3 " : "",
            IF_BIT_SET(c,  9, "SSSE3 ", ""),
            IF_BIT_SET(c, 19, "SSE4.1 ", ""),
            IF_BIT_SET(c, 20, "SSE4.2 ", ""),
            IF_BIT_SET(c, 25, "AES ", ""),
            IF_BIT_SET(c, 12, "FMA3 ", ""),
            IF_BIT_SET(c, 28, "AVX ", ""));
    WriteFile(file, buf, (DWORD)len, NULL, NULL);

    __cpuidex((int*)cpuinfo, 0x7, 0x0);
    b = *(int*)(cpuinfo+4);
    __cpuid((int*)cpuinfo, 0x80000001);
    c = *(int*)(cpuinfo+8);
    d = *(int*)(cpuinfo+12);
    len = snprintf(buf, bufsiz,
            "%-16s %s%s%s%s%s%s%s%s%s%s\r\n",
            "Extended",
            IF_BIT_SET(b,  5, "AXV2 ", ""),
            IF_BIT_SET(b,  3, "BMI1 ", ""),
            IF_BIT_SET(b,  8, "BMI2 ", ""),
            IF_BIT_SET(b, 16, "AVX512 ", ""),
            IF_BIT_SET(d, 22, "EMMX ", ""),
            IF_BIT_SET(d, 31, "3DNow! ", ""),
            IF_BIT_SET(d, 30, "Ex. 3DNow! ", ""),
            IF_BIT_SET(c,  6, "SSE4a ", ""),
            IF_BIT_SET(c, 16, "FMA4 ", ""),
            IF_BIT_SET(c,  5, "ABM ", ""));
    WriteFile(file, buf, (DWORD)len, NULL, NULL);

    GetSystemInfo(&sysinfo);

    done = 0;
    size = 0;
    while (!done) {
        if (!GetLogicalProcessorInformation(pinfo, &size)) {
            if (GetLastError() == 122 /* ERROR_INSUFFICIENT_BUFFER */) {
                if (pinfo) free(pinfo);
                if ((pinfo = malloc(size)) == NULL)
                    return 1;
            } else {
                return 1;
            }
        } else {
            done = 1;
        }
    }

    while (offs + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= size) {
        switch (pinfo->Relationship) {
        case RelationProcessorCore:
            cores++;
            threads += CountSetBits(pinfo->ProcessorMask);
            break;
        case RelationCache:
            cache = &pinfo->Cache;
            if (cache->Level == 1) {
                l1++;
                l1s = cache->Size;
                l1ls = cache->LineSize;
            } else if (cache->Level == 2) {
                l2++;
                l2s = cache->Size;
                l2ls = cache->LineSize;
            } else if (cache->Level == 3) {
                l3++;
                l3s = cache->Size;
                l3ls = cache->LineSize;
            }
            break;
        case RelationProcessorPackage:
            pkg++;
            break;
        }

        offs += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
        pinfo++;
    }

    len = snprintf(buf, bufsiz,
            "%-16s %d\r\n%-16s %d\r\n%-16s %d\r\n%-16s %d/%d/%d\r\n",
            "Cores", cores,
            "Threads", sysinfo.dwNumberOfProcessors,
            "Page Size", sysinfo.dwPageSize,
            "L1/L2/L3", l1, l2, l3);
    WriteFile(file, buf, (DWORD)len, NULL, NULL);

    len = snprintf(buf, bufsiz,
            "%-16s %d/%d\r\n%-16s %d/%d\r\n%-16s %d/%d\r\n",
            "L1 Size", l1s, l1ls,
            "L2 Size", l2s, l2ls,
            "L3 Size", l3s, l3ls);
    WriteFile(file, buf, (DWORD)len, NULL, NULL);

    return 0;
}

static int
print_meminfo(HANDLE file, char *buf, size_t bufsiz)
{
    size_t len;
    MEMORYSTATUSEX mem;
    mem.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&mem);

    len = snprintf(buf, bufsiz,
            "%-16s %ld%%\r\n", "Used", mem.dwMemoryLoad);
    WriteFile(file, buf, (DWORD)len, NULL, NULL);

    len = snprintf(buf, bufsiz,
            "%-16s %lld/%lld MiB\r\n",
            "Physical",
            MIB(mem.ullTotalPhys - mem.ullAvailPhys),
            MIB(mem.ullTotalPhys));
    WriteFile(file, buf, (DWORD)len, NULL, NULL);

    len = snprintf(buf, bufsiz,
            "%-16s %lld/%lld MiB\r\n",
            "Paging",
            MIB(mem.ullTotalPageFile - mem.ullAvailPageFile),
            MIB(mem.ullTotalPageFile));
    WriteFile(file, buf, (DWORD)len, NULL, NULL);

    return 0;
}

#define DEVICE_FLAGS (DISPLAY_DEVICE_ACTIVE | DISPLAY_DEVICE_PRIMARY_DEVICE)
static int
print_gfxinfo(HANDLE file, char *buf, size_t bufsiz)
{
    DWORD i;
    size_t len;
    DISPLAY_DEVICEA device = {0};
    device.cb = sizeof(DISPLAY_DEVICEA);

    for (i = 0; ; ++i) {
        if (!EnumDisplayDevicesA(NULL, i, &device, 0x00000001))
            break;

        if (device.StateFlags == DEVICE_FLAGS) {
            len = snprintf(buf, bufsiz,
                    "%-16s %s\r\n", "Name", device.DeviceString);
            WriteFile(file, buf, (DWORD)len, NULL, NULL);
        }
    }

    return 0;
}

static int
current_directory(char *buf, size_t size)
{
    wchar_t bufw[MAX_PATH+1];
    size_t i = 0, j = -1;
    if (!GetModuleFileNameW(NULL, bufw, (DWORD)size)) {
        return 1;
    }

    if (!WideCharToMultiByte(65001, 0, bufw, -1, buf, (int)size, NULL, NULL))
        return 1;

    while (buf[i] != '\0') {
        if (buf[i++] == '\\')
            j = i-1;
    }

    if (j == -1)
        return 1;

    buf[j] = '\0';
    return 0;
}

int
isdir(char const *path)
{
    DWORD attrib;
    wchar_t pathw[MAX_PATH+1];

    if (!MultiByteToWideChar(65001, 0, path, -1, pathw, MAX_PATH+1))
        return 0;

    attrib = GetFileAttributesW(pathw);
    return attrib != INVALID_FILE_ATTRIBUTES
       && (attrib & FILE_ATTRIBUTE_DIRECTORY);
}

int
mkdir(char const *path)
{
    wchar_t pathw[MAX_PATH+1];

    if (!MultiByteToWideChar(65001, 0, path, -1, pathw, MAX_PATH+1))
        return 0;

    return !CreateDirectoryW(pathw, NULL);
}

#define SUFFIX_DLL  "_hotreload.dll"
#define LEN_PREFIX  (sizeof(__GAME_DLL__)-1)
#define LEN_SUFFIX  (sizeof(SUFFIX_DLL)-1)
#define LEN_FULL    (sizeof(__GAME_DLL__)+sizeof(SUFFIX_DLL)-2)
static char *
current_symbol_path(HANDLE proc)
{
    usize i, n, len;
    DWORD bytes, length;
    static HMODULE mods[1024];
    static wchar_t pathw[MAX_PATH+1];
    static char path[MAX_PATH+1];
    static char dllname[LEN_FULL+1];

    /* Given, this is absolutely hideous, but does the job. */
    memcpy(dllname, __GAME_DLL__, LEN_PREFIX);
    memcpy(dllname + LEN_PREFIX, SUFFIX_DLL, LEN_SUFFIX);
    dllname[LEN_FULL] = '\0';

    if (EnumProcessModules(proc, mods, sizeof(mods), &bytes)) {
        for (i = 0, n = (bytes / sizeof(HMODULE)); i < n; ++i) {
            length = GetModuleFileNameExW(
                    proc, mods[i], pathw, sizeof(pathw)/sizeof(wchar_t));
            if (length) {
                if (!WideCharToMultiByte(65001, 0, pathw, -1, path, MAX_PATH, 0, 0))
                    return NULL;

                len = strlen(path);
                if (len >= LEN_FULL) {
                    if (strcmp(dllname, path + (len - LEN_FULL)) == 0) {
                        *(path + (len - LEN_FULL - 1)) = '\0';
                        return path;
                    }
                }
            }
        }
    }

    return NULL;
}
#undef LEN_FULL
#undef LEN_SUFFIX
#undef LEN_PREFIX
#undef SUFFIX_DLL

static wchar_t *
current_symbol_path_w(HANDLE proc)
{
    static wchar_t pathw[MAX_PATH+1];
    char *path = current_symbol_path(proc);

    if (!MultiByteToWideChar(65001, 0, path, -1, pathw, MAX_PATH+1))
        return 0;

    return pathw;
}

static char *
exc_desc(DWORD code)
{
    switch (code)
    {
    case EXCEPTION_ASSERTION:
        return "ASSERTION FAILURE";
    case EXCEPTION_ACCESS_VIOLATION:
        return "ACCESS VIOLATION";
    case EXCEPTION_DATATYPE_MISALIGNMENT:
        return "DATA MISALIGNMENT";
    case EXCEPTION_BREAKPOINT:
        return "BREAKPOINT";
    case EXCEPTION_SINGLE_STEP:
        return "SINGLE STEP";
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
        return "BOUNDS EXCEEDED";
    case EXCEPTION_FLT_DENORMAL_OPERAND:
        return "FLOAT DENORMAL OPERAND";
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        return "FLOAT DIVIDE BY ZERO";
    case EXCEPTION_FLT_INEXACT_RESULT:
        return "FLOAT INEXACT RESULT";
    case EXCEPTION_FLT_INVALID_OPERATION:
        return "FLOAT INVALID OPERATION";
    case EXCEPTION_FLT_OVERFLOW:
        return "FLOAT OVERFLOW";
    case EXCEPTION_FLT_STACK_CHECK:
        return "FLOAT STACK CHECK";
    case EXCEPTION_FLT_UNDERFLOW:
        return "FLOAT UNDERFLOW";
    case EXCEPTION_INT_DIVIDE_BY_ZERO:
        return "INTEGER DIVIDE BY ZERO";
    case EXCEPTION_INT_OVERFLOW:
        return "INTEGER OVERFLOW";
    case EXCEPTION_PRIV_INSTRUCTION:
        return "PRIVILIGED INSTRUCTION";
    case EXCEPTION_IN_PAGE_ERROR:
        return "IN PAGE ERROR";
    case EXCEPTION_STACK_OVERFLOW:
        return "STACK OVERFLOW";
    case EXCEPTION_ILLEGAL_INSTRUCTION:
        return "ILLEGAL INSTRUCTION";
    case EXCEPTION_NONCONTINUABLE_EXCEPTION:
        return "EXCEPTION";
    case EXCEPTION_INVALID_DISPOSITION:
        return "INVALID DISPOSITION";
    case EXCEPTION_GUARD_PAGE:
        return "GUARD PAGE";
    case EXCEPTION_INVALID_HANDLE:
        return "INVALID HANDLE";
    case EXCEPTION_POSSIBLE_DEADLOCK:
        return "DEADLOCK";
    case CONTROL_C_EXIT:
        return "Ctrl-C";
    default:
        return "UNKNOWN";
    }
}

