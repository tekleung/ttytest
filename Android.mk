LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_STATIC_LIBRARIES := libcutils libc
LOCAL_FORCE_STATIC_EXECUTABLE := true
LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES)
LOCAL_MODULE_TAGS := debug
LOCAL_SRC_FILES := tty_cli.c
LOCAL_MODULE := cli_tty

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)                                                                                                                          

LOCAL_STATIC_LIBRARIES := libcutils libc
LOCAL_FORCE_STATIC_EXECUTABLE := true
LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES)
LOCAL_MODULE_TAGS := debug
LOCAL_SRC_FILES := tty_ser.c
LOCAL_MODULE := ser_tty

include $(BUILD_EXECUTABLE)
