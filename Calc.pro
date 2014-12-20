TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11

SOURCES += \
    core/main.cpp \
    core/application.cpp \
    core/exception.cpp \
    core/number.cpp \
    tokenizer/tokenizer.cpp \
    tokenizer/token.cpp \
    parser/expression.cpp

HEADERS += \
    core/application.h \
    core/exception.h \
    core/number.h \
    tokenizer/token.h \
    tokenizer/tokenizer.h \
    parser/expression.h \
    core/range.h
