ANT=`which ant`
ADB=`which adb`
ANDROID=`which android`
CMAKE=`which cmake`


app_dir=$(cd $(dirname $0) && pwd)
source_dir=$app_dir/../../..
build_dir=$app_dir

cmakeexternals=$app_dir/../CMakeBuild/build/CMakeExternals

set -x
