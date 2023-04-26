const system = platform === Platform.Windows ? "win32" :
			   platform === Platform.Linux   ? "linux" :
			   platform === Platform.OSX     ? "macos" :
			   platform === Platform.Wasm    ? "wasm" :
			   platform === Platform.Android ? "android" :
			   platform === Platform.iOS     ? "ios" :
			   								   "unknown";

let project = new Project('lit');

project.kore = false;

project.addIncludeDir('Libraries/fenster');

project.addIncludeDir('Sources');

if(system == "win32"){
    project.addLib('user32');
    project.addLib('gdi32');
    project.addLib('winmm');

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


project.addFiles(
    'Sources/**',
    'Libraries/fenster/fenster.h'
);
project.setDebugDir('Deployment');

project.flatten();

resolve(project);
