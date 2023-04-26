const system = platform === Platform.Windows ? "win32" :
			   platform === Platform.Linux   ? "linux" :
			   platform === Platform.OSX     ? "macos" :
			   platform === Platform.Wasm    ? "wasm" :
			   platform === Platform.Android ? "android" :
			   platform === Platform.iOS     ? "ios" :
			   								   "unknown";

let project = new Project('lit');

project.setCppStd('c++11');
project.kore = false;

project.addIncludeDir('Libraries/fenster');

project.addIncludeDir('Sources');
project.addIncludeDir('lua52');

if(system == "win32"){
    project.addLib('user32');
    project.addLib('gdi32');
    project.addLib('winmm');

    project.addDefine('_CRT_SECURE_NO_WARNINGS');
    // For LRDB
    project.addLib('ws2_32');
    project.addDefine('WIN32');
    project.addDefine('_WINDOWS');

    project.addIncludeDir('Backends/Windows/include');
}
else if(system == "linux"){
    project.addLib('X11');
    project.addLib('asound');
}
else if(system == "macos"){
    project.addLib('Cocoa');
    project.addLib('AudioToolbox');
}

project.addIncludeDir('Libraries/LRDB/include');
project.addIncludeDir('Libraries/LRDB/third_party/picojson');

project.addIncludeDir('Libraries/LRDB/third_party/asio/asio/include');
project.addDefine('ASIO_HAS_STD_ADDRESSOF');
project.addDefine('ASIO_HAS_STD_ARRAY');
project.addDefine('ASIO_HAS_CSTDINT');
project.addDefine('ASIO_HAS_STD_SHARED_PTR');
project.addDefine('ASIO_HAS_STD_TYPE_TRAITS');
project.addDefine('ASIO_HAS_STD_CHRONO');


project.addFiles(
    'Sources/**',
    'Libraries/fenster/fenster.h',
    'lua52/**'
);
project.setDebugDir('Deployment');

project.flatten();

resolve(project);
