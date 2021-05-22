cmake_minimum_required(VERSION 3.10.2)
# 包含头文件
include_directories(src src/common/android/include)
# 开启汇编支持
enable_language(ASM)
#添加源文件
add_library(breakpad STATIC
        src/client/linux/crash_generation/crash_generation_client.cc
        src/client/linux/handler/exception_handler.cc
        src/client/linux/handler/minidump_descriptor.cc
        src/client/linux/log/log.cc
        src/client/linux/minidump_writer/linux_dumper.cc
        src/client/linux/minidump_writer/linux_ptrace_dumper.cc
        src/client/linux/minidump_writer/minidump_writer.cc
        src/client/linux/microdump_writer/microdump_writer.cc
        src/client/linux/dump_writer_common/ucontext_reader.cc
        src/client/linux/dump_writer_common/thread_info.cc
        src/client/minidump_file_writer.cc
        src/common/android/breakpad_getcontext.S
        src/common/convert_UTF.c
        src/common/md5.cc
        src/common/string_conversion.cc
        src/common/linux/elfutils.cc
        src/common/linux/file_id.cc
        src/common/linux/guid_creator.cc
        src/common/linux/linux_libc_support.cc
        src/common/linux/memory_mapped_file.cc
        src/common/linux/safe_readlink.cc
        )

target_link_libraries(breakpad log)


