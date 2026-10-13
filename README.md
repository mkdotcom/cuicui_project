# cuicui_project

A ready-to-use, minimal [cuicui](https://github.com/mkdotcom/cuicui_core) app:
a title, an image, a text input with a button, and a scrollable list — for Android, iOS, the Web
and Windows (UWP).

```text
cuicui_project/
├── app/
│   ├── main.cpp        your app (start here: InitMyApp())
│   └── assets/         images and other files, available to wImage etc.
├── cuicui_core/        cuicui itself (cloned, see below)
├── modules/            optional cuicui modules (cloned, see modules/README.md)
└── projects/
    ├── android/        Android Studio / Gradle project
    ├── ios/            Xcode project
    ├── web/            emscripten build scripts
    └── uwp/            Visual Studio solution (Windows UWP)
```

> [!WARNING]
> cuicui is functional and used in shipping apps, but its API is not stable yet.

## Getting started

1. Create your project from this template (GitHub "Use this template"), or download it as a zip.
2. In the project folder, get cuicui:
   ```
   git clone https://github.com/mkdotcom/cuicui_core.git
   ```
3. Edit `app/main.cpp` and put your images in `app/assets/`.
4. Build for the platform(s) you want (below).

Your C++ files: every `.cpp` in `app/` is compiled automatically on Android. For the other
platforms, add new files to `projects/web/your_app_cpp_files.txt`, to the Xcode project and to the
Visual Studio project.

### Android
- Open `projects/android` with Android Studio (JDK 17, NDK 27), or run
  `projects/android/gradlew assembleDebug`.
- Change the package name in `projects/android/app/build.gradle` (`namespace` and
  `applicationId`, currently `com.example.cuicuiapp`) and in `MainActivity.java`.
- `app/assets/` is copied into the APK at each build.

### Web
- Install [emscripten](https://emscripten.org) and set its path in `projects/web/_setenv.bat`
  (or `source emsdk_env.sh` on Linux/macOS).
- Build: `projects/web/_compile.bat debug` (or `./_compile.sh debug`).
- Run: `projects/web/_serve.bat` (or `./_serve.sh`), then open http://localhost:8080.
- The app is in `projects/web/build/`: deploy that folder on any static web server.

### Windows (UWP)
- Open `projects/uwp/CuicuiUwp.sln` with Visual Studio 2022 (workload "Universal Windows
  Platform development" + C++ UWP tools), select `x64`, run.
- `app/assets/` is packaged automatically.
- Change the app identity and visual assets in `projects/uwp/App/Package.appxmanifest`.

### iOS
- Open `projects/ios/CuicuiApp.xcodeproj` with Xcode (on a Mac). Xcode downloads
  [SDWebImage](https://github.com/SDWebImage/SDWebImage) (used by `wImage` for images from URLs)
  through Swift Package Manager the first time.
- Select the `CuicuiApp` target, *Signing & Capabilities*: choose your team (the project has none)
  and change the bundle identifier (currently `com.example.cuicuiapp`).
- Run on a simulator or a device. `app/assets/` is copied into the app as the `assets` folder.
- When you add a `.cpp` file to the Xcode project, set its type to **Objective-C++ Source**
  (File inspector, *Type*): on iOS, cuicui's headers contain Objective-C.
- The host code is in `projects/ios/CuicuiApp/`: `ViewController.mm` starts cuicui and implements
  what cuicui needs from the app (photo picker, camera, status bar: the `CuiHostViewController`
  protocol of `cuicui_core/ios/cuicui_ios_host.h`). Change the app name in `Info.plist`.

## Learn more
- [cuicui README](https://github.com/mkdotcom/cuicui_core) — what cuicui is
- [Developer guide](https://github.com/mkdotcom/cuicui_core/blob/main/docs/GUIDE.md)
- [Platform support](https://github.com/mkdotcom/cuicui_core/blob/main/docs/SUPPORT.md)
- [Modules](https://github.com/mkdotcom/cuicui_core/blob/main/docs/MODULES.md)

## License
MIT — see [LICENSE](LICENSE).
