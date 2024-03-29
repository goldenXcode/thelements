LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := thelements

LOCAL_CFLAGS := -DANDROID_NDK \
                -DDISABLE_IMPORTGL

LOCAL_SRC_FILES := \
    app-android.c \
	app.c \
	collide.c \
	elementproperties.c \
    importgl.c \
    points.c \
    rendergl.c \
    saveload.c \
    setup.c \
    specials.c \
    update.c \

LOCAL_LDLIBS := -lGLESv1_CM -ldl -llog

include $(BUILD_SHARED_LIBRARY)
