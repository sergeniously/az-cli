
include makeup/.mk

PROJECT_NAME=az-cli
PROJECT_VERSION=1.0.0
PROJECT_COPYRIGHT=Belenkov Sergey
PROJECT_EMAIL=sergeniously@mail.ru
PROJECT_URL=https://github.com/sergeniously/az-cli

$(call import_modules,Cpp)

$(call set_cxx_standard,c++2a)
